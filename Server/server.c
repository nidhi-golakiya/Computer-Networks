#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define SERVER_PORT 5430
#define MAX_PENDING 5
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

int main(){

  struct sockaddr_in sin;
  char buf[MAX_LINE];
  int len;
  int s, new_s;
  char str[INET_ADDRSTRLEN];
	
  //build address data structure
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(SERVER_PORT);

  //Socket creation
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("simplex-talk: socket");
    exit(1);
  }

  inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
  printf("Server is using address %s and port %d.\n", str, SERVER_PORT);

	//Binding socket
  if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
    perror("simplex-talk: bind");
    exit(1);
  }
  else
    printf("Server bind done.\n");

  listen(s, MAX_PENDING);
  
	//wait for connection
  do
	{
    if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0)
		{
      perror("simplex-talk: accept");
		}
    printf("Server Listening.\n");

		//Assigning station information

		sf1.station_number = 1;
	  sf1.station_name_size = 30;
	  strcpy(sf1.station_name,"English Songs");
	  strcpy(sf1.multicast_address,"239.192.5.10"); 
	  sf1.data_port =5431;
	  sf1.bit_rate=system("ffmpeg -i 1.mpeg 2>&1 | grep start | sed 's/ \(.*\), start/\1/g' | awk -F ',' '{print $2}'|awk -F ':' '{print $2}'");

	  sf2.station_number = 2;
	  sf2.station_name_size = 30;
	  strcpy(sf2.station_name,"Hindi Songs");
	  strcpy(sf2.multicast_address,"239.192.5.10"); 
	  sf2.data_port = 5432;
	  sf2.bit_rate=system("ffmpeg -i 7.mpeg 2>&1 | grep start | sed 's/ \(.*\), start/\1/g' | awk -F ',' '{print $2}'|awk -F ':' '{print $2}'");

		sf3.station_number = 3;
	  sf3.station_name_size = 30;
	  strcpy(sf3.station_name,"Gujarati Songs");
	  strcpy(sf3.multicast_address,"239.192.5.10"); 
	  sf3.data_port = 5433;
	  sf3.bit_rate=system("ffmpeg -i 11.mpeg 2>&1 | grep start | sed 's/ \(.*\), start/\1/g' | awk -F ',' '{print $2}'|awk -F ':' '{print $2}'");

		//Sending every station's information
		
		send(new_s, &(sf1), sizeof(sf1) , 0);
		send(new_s, &(sf2), sizeof(sf2) , 0);
		send(new_s, &(sf3), sizeof(sf3) , 0);		
			
		close(new_s);
  }while(1);
}
