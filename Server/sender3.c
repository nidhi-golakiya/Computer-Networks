#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h> 
#include <time.h>
#include <arpa/inet.h> 


#define MC_PORT 5433
#define BUF_SIZE 40750
#define MAX_PENDING 5

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


int main(int argc, char * argv[])
{
	clock_t t;
  int s,s_tcp,new_s_tcp; 					//socket descriptor
  struct sockaddr_in sin;     		//socket struct
	struct sockaddr_in sin_tcp;
  char buf[BUF_SIZE],filename[BUF_SIZE];
  int len;
	struct sockaddr_in mcast_saddr; //multicast sender
  socklen_t mcast_saddr_len;
  char str[INET_ADDRSTRLEN];

	//Multicast specific
  char *mcast_addr; 							//multicast address
	struct ip_mreq mcast_req;  			//multicast join struct 
  

  //Code to take multicast address.
  if (argc==2) {
    mcast_addr = argv[1];
  }
  else {
    fprintf(stderr, "usage: sender multicast_address\n");
    exit(1);
  }
   
  //Create a socket
  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("server: socket");
    exit(1);
  }

  //build address data structure
  memset((char *)&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = inet_addr(mcast_addr);
  sin.sin_port = htons(MC_PORT);

  memset(buf, 0, sizeof(buf));
  
	mcast_req.imr_multiaddr.s_addr = inet_addr(mcast_addr);
  mcast_req.imr_interface.s_addr = htonl(INADDR_ANY);

  //send multicast join message
  if ((setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
		  (void*) &mcast_req, sizeof(mcast_req))) < 0) {
    perror("mcast join receive: setsockopt()");
    exit(1);
  }

	FILE* fp; 
	int count = 0;
	do
	{
		memset(&mcast_saddr, 0, sizeof(mcast_saddr));
   	mcast_saddr_len = sizeof(mcast_saddr);

    memset(buf, 0, sizeof(buf));
			
		int x;
			
		for(x=11;x<=13;x++)	// Continuously transmitting english songs
		{
			char str1[20];
			char str2[100];
			char sn[20];
			sprintf(str1,"%d",x);
			strcpy(sn,str1);
			strcat(sn,".mpeg");
			fp = fopen(sn, "rb");

			// Converting audio file to streamable audio file.
			strcpy(str2,"ffmpeg -i ");
			strcat(str2,sn); 
			strcat(str2," -f mpegts ");
			strcat(str2,sn);
			//system("ffmpeg -i inputfile.mp4 -f mpegts streamable_output.mp4");

			int frame,i;
			fseek(fp, 0, SEEK_END);
			long fsize = ftell(fp);
			printf("file size : %ld\n",fsize);

			// Calculate no. of frames
			if ((fsize % BUF_SIZE) != 0)
			{
				frame = (fsize/BUF_SIZE)+1;
			}
			else
			{
				frame = fsize/BUF_SIZE;	
			}
			char sname[30];
			char name[30];
			s1.song_name_size = fsize;
			
			//Assigning current song name and next song name
			if(x == 11)
			{
				strcpy(sname,"Vhalam Aavo Ne");
				strcpy(name,"Satrangi Re");
			}
			else if(x == 12)
			{
				strcpy(sname,"Satrangi Re");
				strcpy(name,"Chand Ne Kaho");
			}
			else
			{
				strcpy(sname,"Chand Ne Kaho");
				strcpy(name,"Vhalam Aavo Ne");
			}
			strcpy(s1.song_name,sname);
			strcpy(s1.next_song_name,name);
			s1.duration=system("ffmpeg -i 1.mpeg 2>&1 | grep start | sed 's/ \(.*\), start/\1/g' | awk -F ',' '{print $1}'|awk -F ' ' '{print $2}'"); // Assigning duration of every song
					
			sendto(s, &(s1), sizeof(s1) ,	0, (struct sockaddr*)&sin, sizeof(sin));		// Sending song info structure
					
			fseek(fp, 0, SEEK_SET);
					
			t = clock(); 

			// Sending song frame by frame
			if(frame==0 || frame == 1)
			{
				char *string = malloc(fsize + 1);
				fread(string, 1, fsize+1, fp);
				fseek(fp, 0, SEEK_SET);
			   sendto(s, string, fsize+1,	0, 
					(struct sockaddr*)&sin, sizeof(sin));	
			}
			else
			{
				for(i=1;i<=frame;i++)
				{
					count++;	
					char *string = malloc(BUF_SIZE);
					fread(string, 1, BUF_SIZE, fp);
					fseek(fp, 0, SEEK_CUR);
				  sendto(s, string, BUF_SIZE,0, 
							(struct sockaddr*)&sin,sizeof(sin));
					
					sleep(0.5); 
				}
				printf("No. of frames sent : %d",count);
			}
			t = clock() - t; 
	  	double time_taken = ((double)t)/CLOCKS_PER_SEC;
  		printf("fun() took %f seconds to execute \n", time_taken);
			fclose(fp);
			printf("\n----SEND AGAIN----\n"); 
			sleep(3);
		}
	}while(1);
    
  memset(buf, 0, sizeof(buf));
  close(s);  
  return 0;
}
