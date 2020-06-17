#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <gtk/gtk.h>

#define SERVER_PORT 5430
#define MAX_LINE 512000

//Structure for station information.
struct station_info
{
	uint8_t station_number;
	int station_name_size;
	char station_name[30];
	char multicast_address[100];
	int data_port;
	uint32_t bit_rate;
}sf1,sf2,sf3;

//Signal for pressing button "English Songs"
void static station1(GtkWidget *widget,gpointer data)
{
	char str1[20];
	char st[20];
	int d = sf1.data_port;
	printf("\nd : %d",d);
	sprintf(str1,"%d",d);
	printf("\ns : %s",str1);
	strcpy(st,"./r ");
	strcat(st,str1);

	//Run receiver file with argument given as port number.
	system("gcc -o r receiver.c `pkg-config --cflags --libs gtk+-3.0` -lpthread -w");
	system(st);
}

//Signal for pressing button "Hindi Songs"
void static station2(GtkWidget *widget,gpointer data)
{
	char str1[20];
	char st[20];
	int d = sf2.data_port;
	printf("\nd : %d",d);
	sprintf(str1,"%d",d);
	printf("\ns : %s",str1);
	strcpy(st,"./r ");
	strcat(st,str1);

	//Run receiver file with argument given as port number.
	system("gcc -o r receiver.c `pkg-config --cflags --libs gtk+-3.0` -lpthread -w");
	system(st);
}

//Signal for pressing button "Gujarati Songs"
void static station3(GtkWidget *widget,gpointer data)
{
	char str1[20];
	char st[20];
	int d = sf3.data_port;
	printf("\nd : %d",d);
	sprintf(str1,"%d",d);
	printf("\ns : %s",str1);
	strcpy(st,"./r ");
	strcat(st,str1);

	//Run receiver file with argument given as port number.
	system("gcc -o r receiver.c `pkg-config --cflags --libs gtk+-3.0` -lpthread -w");
	system(st);
}

int main(int argc, char *argv[]) {

	FILE *fp;
  struct hostent *hp;
  struct sockaddr_in sin;
  char *host;
  char buf[MAX_LINE],filename[MAX_LINE];
  int s;
  int len;
  
  //Take server's IP address/host name and translate host name into peer's IP address
  host = argv[1];					
  hp = gethostbyname(host);
  if (!hp) {
    fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
    exit(1);
  }
	else
	{
		printf("%s\n",host);
	}

	//build address data structure
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
  sin.sin_port = htons(SERVER_PORT);

  //Socket creation
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("simplex-talk: socket");
    exit(1);
  }
  else
    printf("Client created socket.\n");

	//Connecting with server
  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
      perror("simplex-talk: connect");
      close(s);
      exit(1);
    }
  else
    printf("Client connected.\n");

		//Receiving every station's information
		len = recv(s, &(sf1), sizeof(sf1), 0);
		len = recv(s, &(sf2), sizeof(sf2), 0);
		len = recv(s, &(sf3), sizeof(sf3), 0);

		//Code for GUI

  	GtkWidget *window,*button,*vbox,*label,*label1,*label2;
		GtkToolItem *exit;
		GtkWidget *toolbar;

  	gtk_init(&argc, &argv);

    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window,"delete-event",G_CALLBACK(gtk_main_quit),NULL);
    gtk_window_set_title(GTK_WINDOW(window),"Station List");

    vbox=gtk_vbox_new(10,20);

		label = gtk_label_new("School Of Engineering And Applied Science");
		label1 = gtk_label_new("Computer Networks Lab Project");
		label2 = gtk_label_new("Group - 5");
  	gtk_label_set_line_wrap(GTK_LABEL(label),TRUE);
  	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 0);
		gtk_label_set_line_wrap(GTK_LABEL(label1),TRUE);
  	gtk_box_pack_start(GTK_BOX(vbox), label1, FALSE, TRUE, 0);
		gtk_label_set_line_wrap(GTK_LABEL(label2),TRUE);
  	gtk_box_pack_start(GTK_BOX(vbox), label2, FALSE, TRUE, 0);

		//Button for station1
    button=gtk_button_new_with_label("English Songs");
    g_signal_connect(button,"clicked",G_CALLBACK(station1),(gpointer) "Station 1");
    gtk_box_pack_start(GTK_BOX(vbox),button,0,0,0);

		//Button for station2
    button=gtk_button_new_with_label("Hindi Songs");
    g_signal_connect(button,"clicked",G_CALLBACK(station2),(gpointer) "Station 2");
    gtk_box_pack_start(GTK_BOX(vbox),button,0,0,0);

		//Button for station3
		button=gtk_button_new_with_label("Gujarati Songs");
    g_signal_connect(button,"clicked",G_CALLBACK(station3),(gpointer) "Station 3");
    gtk_box_pack_start(GTK_BOX(vbox),button,0,0,0);

		toolbar = gtk_toolbar_new();
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);

		//Exit button
		exit = gtk_tool_button_new_from_stock(GTK_STOCK_QUIT);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), exit, -1);

	gtk_widget_set_size_request(exit, 300, 30);

	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 1);

  g_signal_connect(G_OBJECT(exit), "clicked",      			G_CALLBACK(gtk_main_quit), NULL);

    gtk_container_set_border_width(GTK_CONTAINER(window),100);
    gtk_container_add(GTK_CONTAINER(window),vbox); 
    gtk_widget_show_all(window);
    gtk_main();
		return 0;
}

//gcc -o c client.c `pkg-config --cflags --libs gtk+-3.0` -w
