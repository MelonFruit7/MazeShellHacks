#include <gtk/gtk.h>



int windowWidth = 500, windowHeight = 500;



void defineCSS(GtkWidget *widget, GtkCssProvider *cssProvider, char* class);

void activate(GtkApplication *app, gpointer user_data) {
	GtkWidget *window = gtk_application_window_new(app);
	gtk_window_set_default_size(GTK_WINDOW(window), windowWidth, windowHeight);
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	gtk_window_set_title(GTK_WINDOW(window), "aMAZEing");
	
	GtkCssProvider *cssProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(cssProvider, "styles.css", NULL);

	GtkWidget *vBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(window), vBox);

	GtkWidget *startBtn = gtk_button_new_with_label("Start");
	gtk_box_pack_start(GTK_BOX(vBox), startBtn, FALSE, FALSE, 0);

	

	defineCSS(startBtn, cssProvider, "navBtn");
	defineCSS(vBox, cssProvider, "vBox");
	defineCSS(window, cssProvider, "screen");	
	

	gtk_widget_show_all(window);
}

int main(int argc, char** argv) {
	GtkApplication *app = gtk_application_new("in.maze", G_APPLICATION_NON_UNIQUE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int ret = g_application_run(G_APPLICATION(app), argc, argv);

	return ret;
}


void defineCSS(GtkWidget *widget, GtkCssProvider *cssProvider, char* class) { //Used to define css for a widget
  GtkStyleContext *context = gtk_widget_get_style_context(widget);
  gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
  gtk_style_context_add_class(context, class);
}
