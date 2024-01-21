#include <cairo.h>
#include <gtk/gtk.h>

//global dec
cairo_surface_t* surface;
cairo_t* cr;
float arr[3];
GtkWidget *area;
GtkWidget *frame;
GtkGesture *drag;
GtkGesture *gesture; 


// to get x,y co-ordinates and rgb values
static void image_clicked(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data) {

    GdkPixbuf *pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, cairo_image_surface_get_width(surface),                 
                     cairo_image_surface_get_height(surface));

     int index = (int)(y) * gdk_pixbuf_get_width(pixbuf) + (int)(x);
     int channels = gdk_pixbuf_get_n_channels(pixbuf);
     guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

     guchar *pixel = pixels + index * channels;
     guchar r = pixel[0];
     guchar g = pixel[1];
     guchar b = pixel[2];
     g_print("Clicked at: x = %d, y = %d\n", (int)(x), (int)(y));
     g_print("R = %d, G = %d, B = %d\n", r, g, b);

     arr[0] = (float) r / 255.0;
     arr[1] = (float) g / 255.0;
     arr[2] = (float) b / 255.0;

}

static double start_x;
static double start_y;

// draw_brush function to paint color in dragged area
static void
draw_brush (GtkWidget *widget,
            double     x,
            double     y)
{

//local rgb values
    float l1 = arr[0];
    float l2 = arr[1];
    float l3 = arr[2];

  cr = cairo_create (surface);
  //cairo_set_source_rgb (cr, l1, l2, l3);  
  cairo_set_source_rgb (cr, arr[0], arr[1], arr[2]);  
  cairo_rectangle (cr, x - 3, y - 3, 6, 6);
  cairo_fill (cr);
  gtk_widget_queue_draw (GTK_WIDGET(widget));  
}

//drag_begin, drag_update and drag_end functions to perform drag operations
static void
drag_begin (GtkGestureDrag *gesture,
            double          x,
            double          y,
            GtkWidget      *area)
{
  start_x = x;
  start_y = y;

  draw_brush (area, x, y);
}

static void
drag_update (GtkGestureDrag *gesture,
             double          x,
             double          y,
             GtkWidget      *area)
{
  draw_brush (area, start_x + x, start_y + y);
}

static void
drag_end (GtkGestureDrag *gesture,
          double          x,
          double          y,
          GtkWidget      *area)
{
  draw_brush (area, start_x + x, start_y + y);
}

//draw_function to display 
static void draw_function (GtkDrawingArea *area,
               cairo_t        *cr,
               int             width,
               int             height,
               gpointer        data)
{
    cairo_set_source_surface(cr, surface, 0, 0);

    cairo_paint(cr);

    cairo_set_source_rgb (cr, arr[0], arr[1], arr[2]);
    //g_print("%f %f %f \n", arr[0], arr[1], arr[2]);
    cairo_rectangle (cr, 300, 0, 50, 50);
    cairo_fill (cr);
}

//get_color button's function, press initialized here
static void get_color_clicked(GtkButton *button, gpointer user_data) {
    g_print("Get Color button clicked\n");

    //press 
    gesture = gtk_gesture_click_new();
    g_signal_connect(gesture, "pressed", G_CALLBACK(image_clicked), NULL);
    gtk_widget_add_controller(area, GTK_EVENT_CONTROLLER(gesture));

    gtk_widget_queue_draw(GTK_WIDGET(area));


}

//paint button's function, drag operations initialized here
static void set_color_clicked(GtkButton *button, gpointer user_data) {
    g_print("Set Color button clicked\n");
  //drag
  drag = gtk_gesture_drag_new ();
  //gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (drag), GDK_BUTTON_PRIMARY);
  gtk_widget_add_controller (area, GTK_EVENT_CONTROLLER (drag));
  
  g_signal_connect (drag, "drag-begin", G_CALLBACK (drag_begin), area);
  g_signal_connect (drag, "drag-update", G_CALLBACK (drag_update), area);
  g_signal_connect (drag, "drag-end", G_CALLBACK (drag_end), area);


}

//save function
static void save_image(GtkButton *button, gpointer user_data) {
    const gchar *filename = "polish2_op.png";

    cairo_surface_write_to_png(surface, filename);
    g_print("Image saved as: %s\n", filename);
}


static void activate(GtkApplication* app, gpointer user_data)
{
    GtkWidget* window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Image Viewer");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);

    surface = cairo_image_surface_create_from_png("xyz.png");

    area = gtk_drawing_area_new ();
    gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (area),
                                  draw_function,
                                  NULL, NULL);

    //paint                              
    gesture = gtk_gesture_click_new();
    g_signal_connect(gesture, "pressed", G_CALLBACK(image_clicked), NULL);
    gtk_widget_add_controller(area, GTK_EVENT_CONTROLLER(gesture));

    // Create a grid container
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    //Paint button
    GtkWidget *set_color_button = gtk_button_new_with_label("Paint");
    g_signal_connect(set_color_button, "clicked", G_CALLBACK(set_color_clicked), NULL);
    //get color button
    GtkWidget *get_color_button = gtk_button_new_with_label("Get Color");
    g_signal_connect(get_color_button, "clicked", G_CALLBACK(get_color_clicked), NULL);
    //Save button
    GtkWidget *save_button = gtk_button_new_with_label("Save");
    g_signal_connect(save_button, "clicked", G_CALLBACK(save_image), NULL);
    // Add the buttons to the grid
    gtk_grid_attach(GTK_GRID(grid), get_color_button, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), set_color_button, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), save_button, 2, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), area, 0, 1, 5, 5);

    gtk_window_set_child(GTK_WINDOW(window), grid);
 
    gtk_window_present (GTK_WINDOW(window));

  
}

int main(int argc, char** argv)
{
    GtkApplication* app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
