#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void receive_file(int socket, const char *filename) {
    // open file
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    int bytes_received;
    // write recieved bytes to the file
    while ((bytes_received = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    fclose(file);
    printf("Image saved as %s\n", filename);
}

// Activate function
static void activate (GtkApplication *app, gpointer user_data)
{
    //GTK initilization
    GtkWidget *window;
    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Icon");
    gtk_window_set_default_size (GTK_WINDOW (window), 400, 400);
    // Create a Gtk image widget
    GtkWidget* image = gtk_image_new_from_file ("op.jpg");

    gtk_window_set_child(GTK_WINDOW(window), image);
    gtk_window_present (GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if (argc < 2) {
        printf("Usage: %s <server IP>\n", argv[0]);
        return -1;
    }
    // Convert the server IP address from string to binary format
    if (inet_pton(AF_INET, argv[1], &(serv_addr.sin_addr)) <= 0) {
        printf("Invalid server address. Please provide a valid IP address.\n");
        return -1;
    }
    // Create a socket
    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) { // AF_UNIX as the client/server comm. is on same machine
        printf("\n Socket creation error \n");
        return -1;
    }
    serv_addr.sin_family = AF_UNIX; // AF_UNIX for local comm instead of AF_INET
    serv_addr.sin_port = htons(PORT);
    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    // Receive the image file from the server
    receive_file(sock, "op.jpg");

    GtkApplication *app;
    int status;
    app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref (app);

    return 0;
}
