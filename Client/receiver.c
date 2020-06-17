#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <arpa/inet.h> 
#include <pthread.h>
#include <unistd.h>
#include <gtk/gtk.h>

#define BUF_SIZE 64000
#define t 7
int flag=0;
FILE *fp;
//Signal for pressing button "Quit"
void static Quit(GtkWidget *widget,gpointer data)
{
	//Kill the process of ffplay and exit
	system("kill $(ps -ef | awk '{if($8 == \"ffplay\") {print $2,$6,$8}}' | sort -r -k2 |  head -1 | awk '{print $1}')");
	exit(1);
}

//Signal for pressing button "Pause"
void static Pause(GtkWidget *widget,gpointer data)
{
	//Stop the process of ffplay
	flag =1;
	system("kill -STOP $(ps -ef | awk '{if($8 == \"ffplay\") {print $2,$6,$8}}' | sort -r -k2 |  head -1 | awk '{print $1}')");
	//system("rm eng.mpeg");
	
	//return 0;
}

//Signal for pressing button "Resume"
void static Resume(GtkWidget *widget,gpointer data)
{
	//system("gnome-terminal -- sh -c 'ffplay eng.mpeg -nodisp;'");	//system("ffplay -i eng.mpeg -nodisp");
	//Continue the process of ffplay
	flag = 0;
	//fseek(fp,0,SEEK_END);
	//system("gnome-terminal -- sh -c 'ffplay eng.mpeg -nodisp;'");
	system("kill -CONT $(ps -ef | awk '{if($8 == \"ffplay\") {print $2,$6,$8}}' | sort -r -k2 |  head -1 | awk '{print $1}')");
}

//Structure for stream information messages.
struct song_info
{
	uint8_t song_name_size;
	char song_name[30];
  uint16_t duration;
	char next_song_name[30];
}s1;

//Function to clear buffer.
void clearBuf(char* b) 
{ 
	int i; 
	for (i = 0; i < BUF_SIZE; i++) 
		b[i] = '\0'; 
} 

//Thread for receiving data continuously
void* print_once(void *k)
{
	int y = *((int *) k);
   //printf("port : %d\n",y);
	int s; 														//socket descriptor
  struct sockaddr_in sin;
  char *if_name; 										//name of interface
  struct ifreq ifr; 								//interface struct
  char buf[BUF_SIZE],filename[BUF_SIZE];
  int len;
	char *host;
	struct hostent *hp;

	//Multicast specific
  char *mcast_addr; 							//multicast address
  struct ip_mreq mcast_req;  			//multicast join struct
  struct sockaddr_in mcast_saddr; //multicast sender
  socklen_t mcast_saddr_len;
	//FILE *fp;

  mcast_addr = "239.192.5.10";
  
  /*if(argc == 2) {
  	  if_name = argv[2];
  	}
  else*/
	if_name = "wlan0";

  //create socket
  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("receiver: socket");
    exit(1);
  }

  //build address data structure
  memset((char *)&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(y);
    
  //Use the interface specified
  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name , if_name, sizeof(if_name)-1);
  
  if ((setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&ifr, sizeof(ifr))) < 0)
  {
  	perror("receiver: setsockopt() error");
    exit(1);
  }

  //bind the socket
  if ((bind(s, (struct sockaddr *) &sin, sizeof(sin))) < 0) {
    perror("receiver: bind()");
    exit(1);
  }
  
  //build IGMP join message structure
  mcast_req.imr_multiaddr.s_addr = inet_addr(mcast_addr);
  mcast_req.imr_interface.s_addr = htonl(INADDR_ANY);

  //send multicast join message
  if ((setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
		  (void*) &mcast_req, sizeof(mcast_req))) < 0) {
    perror("mcast join receive: setsockopt()");
    exit(1);
  } 
 
  //receive multicast messages
  printf("\nReady to listen!\n\n");
	int count = 0;
	while(1) 
	{
    //reset sender struct
    memset(&mcast_saddr, 0, sizeof(mcast_saddr));
    mcast_saddr_len = sizeof(mcast_saddr);
    
    //clear buffer and receive
    memset(buf, 0, sizeof(buf));
		
		fp=fopen("eng.mpeg","wb");
		printf("\n---------Data Received---------\n"); 
		//if(flag==0)
		//{
		//int i=0;
		//Continuously receiving data sent by sender
		for(;;)
		{
			int data,d;
			if(count % 20 == 0)
			{
				//d = recvfrom(s, &(s1),sizeof(s1), 0,(struct sockaddr*)&mcast_saddr, &mcast_saddr_len);
				/*strcpy(s1.song_name,"Thousand years");
				strcpy(s1.next_song_name,"I don't wanna live forever");
				printf("Current song Name : %s\n",s1.song_name);
				printf("Next Song Name : %s\n",s1.next_song_name);*/
			}
			
				data=recvfrom(s, buf,sizeof(buf), 0,(struct sockaddr*)&mcast_saddr, &mcast_saddr_len);

				if(data < 0 || d < 0)
				{
					printf("Error in receiver");
				}
				count++;
				char str[2000] ;
				fwrite (buf , 1 , data , fp );
				
				if(count == 10)
				{
					system("gnome-terminal -- sh -c 'ffplay eng.mpeg -nodisp;'");		//Play songs in ffplay
				}
		}		
	}
		fclose(fp);
				
		printf("\n-------------------------------\n"); 
		clearBuf(buf);
		printf("\n----RECEIVE AGAIN----\n"); 
	//}
  return 0;
    
}

int main(int argc, char *argv[])
{
	//Get port number according to the station selected
  int *arg = malloc(sizeof(*arg));
  int d = atoi(argv[1]);
  *arg = d;

	//Calling the thread
  pthread_t thread1;
  pthread_create(&thread1, 0, print_once, arg);

	//GUI code for controlling functions
  GtkWidget *window,*button,*label,*label1,*label2;
  GtkWidget *vbox;
  
  GtkWidget *toolbar;
  GtkWidget *view;
  GtkWidget *statusbar;
  GtkToolItem *exit;
  GtkTextBuffer *buffer;

  gtk_init(&argc, &argv);

  window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(window,"delete-event",G_CALLBACK(gtk_main_quit),NULL);
  gtk_window_set_title(GTK_WINDOW(window),"Control Functions");

  vbox=gtk_vbox_new(10,20);
	if(d==5431)
	{
		strcpy(s1.song_name,"Current Song : Thousand years");
		strcpy(s1.next_song_name,"Next Song : I don't wanna live forever");
		printf("Current song Name : %s\n",s1.song_name);
		printf("Next Song Name : %s\n",s1.next_song_name);
	}
	else if(d==5432)
	{	
		strcpy(s1.song_name,"Current Song : Ek Pyaar Ka Nagma");
		strcpy(s1.next_song_name,"Next Song : Ajeeb Dasta Hai Yeh");
		printf("Current song Name : %s\n",s1.song_name);
		printf("Next Song Name : %s\n",s1.next_song_name);
	}
	else
	{
		strcpy(s1.song_name,"Current Song : Satrangi Re");
		strcpy(s1.next_song_name,"Next Song : Vhalam Aavo Ne");
		printf("Current song Name : %s\n",s1.song_name);
		printf("Next Song Name : %s\n",s1.next_song_name);
	}
	label = gtk_label_new(s1.song_name);
	label1 = gtk_label_new(s1.song_name_size);
	label2 = gtk_label_new(s1.next_song_name);
  gtk_label_set_line_wrap(GTK_LABEL(label),TRUE);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 0);
	gtk_label_set_line_wrap(GTK_LABEL(label1),TRUE);
  gtk_box_pack_start(GTK_BOX(vbox), label1, FALSE, TRUE, 0);
	gtk_label_set_line_wrap(GTK_LABEL(label2),TRUE);
  gtk_box_pack_start(GTK_BOX(vbox), label2, FALSE, TRUE, 0);

	//Button for Quit
  button=gtk_button_new_with_label("Quit");
  g_signal_connect(button,"clicked",G_CALLBACK(Quit),(gpointer) "Stop");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,5);

	//Button for Pause
	button=gtk_button_new_with_label("Pause");
  g_signal_connect(button,"clicked",G_CALLBACK(Pause),(gpointer) "Pause");
  gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,5);

	//Button for Resume
	button=gtk_button_new_with_label("Resume");
  g_signal_connect(button,"clicked",G_CALLBACK(Resume),(gpointer) "Resume");
  gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,5);

	//Button for Change Station
	button=gtk_button_new_with_label("Change Station");
  g_signal_connect(button,"clicked",G_CALLBACK(Quit),(gpointer) "Next Station");
  gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,5);

  gtk_container_set_border_width(GTK_CONTAINER(window),200);
  gtk_container_add(GTK_CONTAINER(window),vbox); 
  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}
