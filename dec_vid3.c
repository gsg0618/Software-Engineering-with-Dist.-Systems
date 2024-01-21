#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/types.h>
#include <gtk/gtk.h>

#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#define INBUF_SIZE 4096
//Global Declarations
static AVFormatContext *fmt_ctx = NULL;
struct SwsContext *sws_ctx = NULL;
const char *filename, *outfilename;
int target_frame_number = 0;
AVCodecContext *c = NULL;
AVFrame *frame;
AVPacket *pkt;
GtkApplication *app;

//Fucntion to save selected frame into pgm files using co-efficient values
static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize, const char *filename, double coefficient, int is_ppm) {
    FILE *f;
    int i;
    
    f = fopen(filename, "wb");
    //Check if the file was opened properly
    if (!f) {
        fprintf(stderr, "Could not open file %s\n", filename);
        exit(1);
    }
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++) {
        for (int j = 0; j < xsize; j++) {
            unsigned char intensity = (unsigned char)(coefficient * buf[i * wrap + j] +
                                                coefficient * buf[i * wrap + j + 1] +
                                                coefficient * buf[i * wrap + j + 2]);
            fwrite(&intensity, 1, 1, f);
        }
    }

    fclose(f);
}

//Fucntion to save selected frame into a ppm file
static void ppm_save(AVFrame *frame, int xsize, int ysize, const char *filename) {
    FILE *f = fopen(filename, "wb");
    // Check if the file was opened properly
    if (!f) {
        printf("Failed to open output file\n");
        exit(1);
    }
    fprintf(f, "P6\n%d %d\n255\n", xsize, ysize);
    sws_ctx = sws_getContext(xsize, ysize, frame->format, xsize, ysize, AV_PIX_FMT_RGB24,
                    0, NULL, NULL, NULL);
    if (!sws_ctx) {
        printf("SwsContext initialization failed \n");
        fclose(f);
        exit(1);
    }
    unsigned char *buff = (unsigned char *)malloc(xsize * ysize * 3);

    if (!buff) {
        printf("RGB buffer failed\n");
        sws_freeContext(sws_ctx);
        fclose(f);
        exit(1);
    }

    unsigned char *dst[1] = {buff};
    int line[1] = {xsize * 3};
    sws_scale(sws_ctx, (const uint8_t * const *)frame->data, frame->linesize, 0, ysize, dst, line);
    
    for (int y = 0; y < ysize; y++) {
        fwrite(buff + y * xsize * 3, 1, xsize * 3, f);
    }
    free(buff);
    sws_freeContext(sws_ctx);
    fclose(f);
}

// Decode function to save target frame and save images
int decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt, const char *filename, int target) {
    char buf[1024];
    int ret;
    double coeff[] = {0.278, 0.560, 0.111};
    ret = avcodec_send_packet(dec_ctx, pkt);

    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return 0;
        else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }

        if (dec_ctx->frame_num == target) {
            printf("Saving frame %" PRId64 "\n", dec_ctx->frame_num);
            snprintf(buf, sizeof(buf), "image.ppm");
            ppm_save(frame, frame->width, frame->height, buf);
            for(int i = 0; i < 3; i++)
            {
                snprintf(buf, sizeof(buf), "image%d.pgm", i);
                pgm_save(frame->data[0], frame->linesize[0], frame->width, frame->height, buf, coeff[i], 0);
            }
            return 0;
        }
    }
return 0;
}

// Callback function for GTK application activation
static void on_app_activate(GApplication *app, gpointer user_data)
{
    //GTK declarations
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *window1;
    GtkWidget *image1;
    GtkWidget *window2;
    GtkWidget *image2;
    GtkWidget *window3;
    GtkWidget *image3;

    window = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window), "Image.ppm");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);
    window1 = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window1), "Image0.ppm");
    gtk_window_set_default_size(GTK_WINDOW(window1), 600, 600);
    window2 = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window2), "Image1.ppm");
    gtk_window_set_default_size(GTK_WINDOW(window2), 600, 600);
    window3 = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window3), "Image2.ppm");
    gtk_window_set_default_size(GTK_WINDOW(window3), 600, 600);

    image = gtk_image_new_from_file("image.ppm");
    image1 = gtk_image_new_from_file("image0.pgm");
    image2 = gtk_image_new_from_file("image1.pgm");
    image3 = gtk_image_new_from_file("image2.pgm");

    gtk_window_set_child(GTK_WINDOW(window), image);
    gtk_window_present(GTK_WINDOW(window));
    gtk_window_set_child(GTK_WINDOW(window1), image1);
    gtk_window_present(GTK_WINDOW(window1));
    gtk_window_set_child(GTK_WINDOW(window2), image2);
    gtk_window_present(GTK_WINDOW(window2));
    gtk_window_set_child(GTK_WINDOW(window3), image3);
    gtk_window_present(GTK_WINDOW(window3));

}

//Main function
int main(int argc, char **argv) {
    // Check the number of command-line arguments
    if (argc <= 2) {
        fprintf(stderr, "Usage: %s <input file> <frame number>\n", argv[0]);
        exit(1);
    }
    // Retrieve command-line arguments
    filename = argv[1];
    target_frame_number = atoi(argv[2]);
    // Allocate AVPacket
    pkt = av_packet_alloc();
    if (!pkt) {
        fprintf(stderr, "Failed to allocate AVPacket\n");
        exit(1);
    }
    // Open the input file
    if (avformat_open_input(&fmt_ctx, filename, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", filename);
        exit(1);
    }
    // Retrieve stream information
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }
    int stream_index = -1;
    const AVCodec *codec = NULL;
    AVCodecParameters *codec_params = NULL;
    // Find the video codec and stream index
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        AVStream *stream = fmt_ctx->streams[i];
        codec_params = stream->codecpar;

        if (codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
            codec = avcodec_find_decoder(codec_params->codec_id);
            stream_index = i;
            break;
        }
    }
    // Check if video codec is found
    if (!codec) {
        fprintf(stderr, "Failed to find video codec\n");
        exit(1);
    }
    // Allocate AVCodecContext
    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Failed to allocate AVCodecContext\n");
        exit(1);
    }
    // Copy codec parameters to context
    if (avcodec_parameters_to_context(c, codec_params) < 0) {
        fprintf(stderr, "Failed to copy codec parameters to context\n");
        exit(1);
    }
    // Open the codec
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Failed to open codec\n");
        exit(1);
    }
    // Allocate AVFrame
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Failed to allocate AVFrame\n");
        exit(1);
    }
    // Read frames from the input file
    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == stream_index) {
            AVPacket *pkt_copy = av_packet_clone(pkt);
            avcodec_send_packet(c, pkt_copy);
            decode(c, frame, pkt_copy, outfilename, target_frame_number);
            av_packet_unref(pkt_copy);
        }
        av_packet_unref(pkt);
    }
    avcodec_send_packet(c, NULL);
    decode(c, frame, NULL, outfilename, target_frame_number);
    // GTK Application
    app = gtk_application_new("com.example.image_viewer", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
    int status = g_application_run(G_APPLICATION (app), 0, NULL);
    g_object_unref(app); 
    return status;
    // Clean up 
    avformat_close_input(&fmt_ctx);
    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    return 0;
}
