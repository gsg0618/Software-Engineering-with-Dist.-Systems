#include <gtk/gtk.h>

// Global variables
GtkWidget *result_label;

// Callback function for button clicks
static void button_clicked(GtkButton *button, gpointer user_data) {
  const gchar *text = gtk_button_get_label(button);
  gchar *current_result = g_strdup(gtk_label_get_text(GTK_LABEL(result_label)));

  if (g_strcmp0(text, "C") == 0) {
    g_free(current_result);
    gtk_label_set_text(GTK_LABEL(result_label), "0");
    return;
  }

  // Check for the equals button
  if (g_strcmp0(text, "=") == 0) {
    // Evaluate the expression using glib's expression parser
    GError *error = NULL;
    gdouble result = g_strtod(current_result, NULL);

    gchar *new_result = g_strdup_printf("%.2f", result);
    gtk_label_set_text(GTK_LABEL(result_label), new_result);
    g_free(current_result);
    g_free(new_result);
    return;
  }
  // Handle other buttons by appending their text to the current result
  gchar *new_result = g_strconcat(current_result, text, NULL);
  gtk_label_set_text(GTK_LABEL(result_label), new_result);
  g_free(current_result);
  g_free(new_result);
}




static void
activate (GtkApplication *app,
          gpointer        user_data)
{

  GtkWidget *window;
  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Calculator");
  //g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  GtkWidget *grid = gtk_grid_new();
  gtk_window_set_child(GTK_WINDOW(window), grid);

  // Create the result label
  result_label = gtk_label_new("0");
  gtk_label_set_xalign(GTK_LABEL(result_label), 1.0); // Align text to the right
  gtk_grid_attach(GTK_GRID(grid), result_label, 0, 0, 4, 3);

  // Create the number buttons
  GtkWidget *button1 = gtk_button_new_with_label("1");
  g_signal_connect(button1, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_grid_attach(GTK_GRID(grid), button1, 0, 1, 1, 1);

  GtkWidget *button2 = gtk_button_new_with_label("2");
  g_signal_connect(button2, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_grid_attach(GTK_GRID(grid), button2, 1, 1, 1, 1);
  
  GtkWidget *button3 = gtk_button_new_with_label("3");
  g_signal_connect(button3, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_grid_attach(GTK_GRID(grid), button3, 2, 1, 1, 1);

  GtkWidget *button4 = gtk_button_new_with_label("4");
  g_signal_connect(button4, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_grid_attach(GTK_GRID(grid), button4, 0, 2, 1, 1);

  GtkWidget *button5 = gtk_button_new_with_label("5");
  g_signal_connect(button5, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_grid_attach(GTK_GRID(grid), button5, 1, 2, 1, 1);

  GtkWidget *button6 = gtk_button_new_with_label("6");
  g_signal_connect(button6, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_grid_attach(GTK_GRID(grid), button6, 2, 2, 1, 1);

  GtkWidget *button7 = gtk_button_new_with_label("7");
  g_signal_connect(button7, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_grid_attach(GTK_GRID(grid), button7, 0, 3, 1, 1);

  GtkWidget *button8 = gtk_button_new_with_label("8");
  g_signal_connect(button8, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_grid_attach(GTK_GRID(grid), button8, 1, 3, 1, 1);

  GtkWidget *button9 = gtk_button_new_with_label("9");
  g_signal_connect(button9, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_grid_attach(GTK_GRID(grid), button9, 2, 3, 1, 1);

  GtkWidget *button0 = gtk_button_new_with_label("0");
  g_signal_connect(button0, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_grid_attach(GTK_GRID(grid), button0, 1, 4, 1, 1);

  // Create the operator buttons
  GtkWidget *button_plus = gtk_button_new_with_label("+");
  g_signal_connect(button_plus, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_grid_attach(GTK_GRID(grid), button_plus, 3, 1, 1, 1);

GtkWidget *button_minus = gtk_button_new_with_label("-");
  g_signal_connect(button_minus, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_grid_attach(GTK_GRID(grid), button_minus, 3, 2, 1, 1);

  GtkWidget *button_multiply = gtk_button_new_with_label("*");
  g_signal_connect(button_multiply, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_grid_attach(GTK_GRID(grid), button_multiply, 3, 3, 1, 1);

  GtkWidget *button_divide = gtk_button_new_with_label("/");
  g_signal_connect(button_divide, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_grid_attach(GTK_GRID(grid), button_divide, 3, 4, 1, 1);

  // Create the equals button
  GtkWidget *button_equals = gtk_button_new_with_label("=");
  g_signal_connect(button_equals, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_grid_attach(GTK_GRID(grid), button_equals, 2, 4, 1, 1);

  // Show all the widgets
  gtk_window_present(window);



 

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
