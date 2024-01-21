#include <gtk/gtk.h>

static void
print_hello (GtkWidget *widget,
             gpointer   data)
{
  g_print ("Hello World\n");
}
void key_button_clicked(GtkButton *button, gpointer user_data)
{
    // Get the label text from the button
    const gchar *key_label = gtk_button_get_label(button);

    // Print the pressed key to the console
    g_print("%s", key_label);
}
static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *button;
  GtkWidget *lab;

  /* create a new window, and set its title */
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Virtual Keyboard");
  gtk_window_set_default_size (GTK_WINDOW (window), 400, 200);
  



  grid = gtk_grid_new();
  gtk_window_set_child(GTK_WINDOW(window), grid);

  const gchar *keys[4][10] = {
        {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
        {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"},
        {"A", "S", "D", "F", "G", "H", "J", "K", "L", ";"},
        {"Z", "X", "C", "V", "B", "N", "M", ",", ".", "/"}
    };

    // Create key buttons and connect their clicked signals
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            GtkWidget *button = gtk_button_new_with_label(keys[row][col]);
            g_signal_connect(button, "clicked", G_CALLBACK(key_button_clicked), NULL);
            gtk_widget_set_hexpand(button, TRUE);
            gtk_widget_set_vexpand(button, TRUE);
            gtk_grid_attach(GTK_GRID(grid), button, col, row, 1, 1);
        }
    }
/////This code is basic box and button
  // /* Here we construct the container that is going pack our buttons */
  // grid = gtk_grid_new ();

  // /* Pack the container in the window */
  // gtk_window_set_child (GTK_WINDOW (window), grid);

  // button = gtk_button_new_with_label ("Q");
  // g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);

  // /* Place the first button in the grid cell (0, 0), and make it fill
  //  * just 1 cell horizontally and vertically (ie no spanning)
  //  */
  // gtk_grid_attach (GTK_GRID (grid), button, 0, 0, 1, 1);

  // button = gtk_button_new_with_label ("W");
  // g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);

  // /* Place the second button in the grid cell (1, 0), and make it fill
  //  * just 1 cell horizontally and vertically (ie no spanning)
  //  */
  // gtk_grid_attach (GTK_GRID (grid), button, 1, 0, 1, 1);

  // button = gtk_button_new_with_label ("Quit");
  // g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_window_destroy), window);

  // /* Place the Quit button in the grid cell (0, 1), and make it
  //  * span 2 columns.
  //  */
  // gtk_grid_attach (GTK_GRID (grid), button, 0, 1, 2, 1);
  g_signal_connect_swapped(window, "destroy", G_CALLBACK(gtk_window_destroy), NULL);
  gtk_widget_show (window);

}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}

