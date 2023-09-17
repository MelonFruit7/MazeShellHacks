#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo.h>
#include <stdlib.h>
#include <time.h>


int lineLength = 20;
int windowWidth = 500, windowHeight = 500;
int prevWindowWidth = 500, prevWindowHeight = 500; //if it works, it works. 

int** board; //Board for maze generation
GtkWidget *darea; //Drawing board

void defineCSS(GtkWidget *widget, GtkCssProvider *cssProvider, char* class); //Defines CSS for widgets
gboolean drawing(GtkWidget *widget, cairo_t *cr, gpointer data); //Called when a draw event is seen
void createMaze(cairo_t *cr, int x, int y, int direction); //Creates a random maze
int availableSpots(int boardX, int boardY); //Checks how many spots you can move on a board.
void mazeRedraw(cairo_t *cr); //Redraw the last generated maze on the board
void clearBoard(int newLL); //Clear and allocate 
void mazeMakeInput(GtkWidget *widget, gpointer data); //Called when user clicks on create button
void changeSize(GtkWidget *widget, GdkRectangle *allocation, gpointer data); //Called when the window changes size

void activate(GtkApplication *app, gpointer user_data) {
	srand(time(0));
	//Allocate for the default board
	board = malloc(windowHeight/lineLength * sizeof(int*));
	for (int i = 0; i < windowHeight/lineLength; i++) {
		board[i] = malloc(windowWidth/lineLength * sizeof(int));
		for (int j = 0; j < windowWidth/lineLength; j++) board[i][j] = 0;
	}

	GtkWidget *window = gtk_application_window_new(app);
	gtk_window_set_default_size(GTK_WINDOW(window), windowWidth, windowHeight);
	gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	gtk_window_set_title(GTK_WINDOW(window), "aMAZEing");
	
	GtkCssProvider *cssProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(cssProvider, "styles.css", NULL);

	GtkWidget *vBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(window), vBox);

	GtkWidget *navBar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_box_pack_start(GTK_BOX(vBox), navBar, FALSE, FALSE, 0);

	GtkWidget *createBtn = gtk_button_new_with_label("Create");
	gtk_box_pack_start(GTK_BOX(navBar), createBtn, FALSE, FALSE, 0);

	GtkWidget *LL = gtk_entry_new(); //Line Length
	gtk_entry_set_placeholder_text(GTK_ENTRY(LL), "Line Length");
	gtk_box_pack_start(GTK_BOX(navBar), LL, TRUE, TRUE, 0);

	darea = gtk_drawing_area_new();
	gtk_box_pack_start(GTK_BOX(vBox), darea, TRUE, TRUE, 0);

	//Signals
	g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(drawing), NULL);
	g_signal_connect(createBtn, "clicked", G_CALLBACK(mazeMakeInput), LL);
	g_signal_connect(window, "size-allocate", G_CALLBACK(changeSize), NULL);

	//CSS define
	defineCSS(navBar, cssProvider, "navBtn");
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

void changeSize(GtkWidget *widget, GdkRectangle *allocation, gpointer data) {
	prevWindowWidth = windowWidth;
	prevWindowHeight = windowHeight;

	windowWidth = gtk_widget_get_allocated_width(widget);
	windowHeight = gtk_widget_get_allocated_height(widget);
	if (prevWindowWidth == windowWidth && prevWindowHeight == windowHeight) return;
	clearBoard(lineLength);
}


gboolean drawing(GtkWidget *widget, cairo_t *cr, gpointer data) {
  int x = windowWidth/2, y = windowHeight/2;
  cairo_set_source_rgb(cr, 0, 1, 0); //Change line color
  cairo_set_line_width(cr, 1); //Change line width

  if (board[0][0] != 0) { //If a maze was already generated
	mazeRedraw(cr); 
	cairo_stroke(cr);
	return TRUE;
  }
	  
  createMaze(cr, x, y, 1); //Create the maze
  cairo_stroke(cr); 
  gtk_widget_queue_draw(darea); //Request a new draw (Aka clear the drawing area and call drawing() again)
  return TRUE;  
}

void createMaze(cairo_t *cr, int x, int y, int direction) { //Create the maze
	int boardX = x/lineLength, boardY = y/lineLength; //The board x and y has to be adjusted.
	board[boardY][boardX] = direction; //Set direction in the board

	while (availableSpots(boardX, boardY) != 0) { //Loop until all points (up, right, down, left) next to (x,y) have been filled with a direction
		cairo_move_to(cr, x, y);
		switch(rand()%4) {
			case 0: 
				if (boardX+1 < windowWidth/lineLength && board[boardY][boardX+1] == 0) {
					cairo_line_to(cr, x+lineLength, y); 
					createMaze(cr, x+lineLength, y, 1);
				}	
			break;
			case 1: 
				if (boardY+1 < windowHeight/lineLength && board[boardY+1][boardX] == 0) {
					cairo_line_to(cr, x, y+lineLength);
					createMaze(cr, x, y+lineLength, 2);
				}
			break;
			case 2: 		 
				if (boardX-1 >= 0 && board[boardY][boardX-1] == 0) {
					cairo_line_to(cr, x-lineLength, y);
					createMaze(cr, x-lineLength, y, 3);
				}
			break;
			case 3: 
				if (boardY-1 >= 0 && board[boardY-1][boardX] == 0) {
					cairo_line_to(cr, x, y-lineLength);
					createMaze(cr, x, y-lineLength, 4);
				}
		}
	}
	return;
}
int availableSpots(int boardX, int boardY) { //Check available positions around a slot in the board
	int spots = 0;
	if (boardX+1 < windowWidth/lineLength && board[boardY][boardX+1] == 0) spots++;
	if (boardY+1 < windowHeight/lineLength && board[boardY+1][boardX] == 0) spots++;
	if (boardX-1 >= 0 && board[boardY][boardX-1] == 0) spots++;
	if (boardY-1 >= 0 && board[boardY-1][boardX] == 0) spots++;
	return spots;
}

void mazeRedraw(cairo_t *cr) { //Used to redraw the maze from the board
	for (int j = 0; j < windowHeight/lineLength; j++) {
		for (int i = 0; i < windowWidth/lineLength; i++) {
			cairo_move_to(cr, i*lineLength, j*lineLength);
			//You have to be a magician to read this
			//Things are inverted based on where x and y values go
			switch(board[j][i]) {
				case 1: cairo_line_to(cr, i*lineLength - lineLength, j*lineLength);
				break;
				case 2: cairo_line_to(cr, i*lineLength, j*lineLength - lineLength);
				break;
				case 3: cairo_line_to(cr, i*lineLength + lineLength, j*lineLength);
				break;
				case 4: cairo_line_to(cr, i*lineLength, j*lineLength + lineLength);
			}
		}	
	}
}

void clearBoard(int newLL) {
	for (int i = 0; i < prevWindowHeight/lineLength; i++) free(board[i]);
	free(board);
	board = (int**)malloc(windowHeight/newLL * sizeof(int*));
	for (int i = 0; i < windowHeight/newLL; i++) {
		board[i] = (int*)malloc(windowWidth/newLL*sizeof(int));
		for (int j = 0; j < windowWidth/newLL; j++) board[i][j] = 0;
	}
}

void mazeMakeInput(GtkWidget *widget, gpointer data) {
	GtkWidget *LL = (GtkWidget *)data;
	int check = atoi(gtk_entry_get_text(GTK_ENTRY(LL))); //Get the number in the text box
	if (check > 1) { 
		clearBoard(check); //Check the board and pass in the new line length
		lineLength = check;
	} else {
		clearBoard(lineLength);
	}
	gtk_widget_queue_draw(darea);
}

void defineCSS(GtkWidget *widget, GtkCssProvider *cssProvider, char* class) { //Used to define css for a widget
  GtkStyleContext *context = gtk_widget_get_style_context(widget);
  gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
  gtk_style_context_add_class(context, class);
}
