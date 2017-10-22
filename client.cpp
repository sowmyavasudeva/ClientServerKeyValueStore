#include<iostream>
#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/time.h>
using namespace std;

void printTimeStamp(){
	 char time_buf[60];
	 struct timeval tv;
     struct tm* ptm;
     char time_string[60];
     long milliseconds;
	 
	 gettimeofday (&tv, NULL);
     ptm = localtime (&tv.tv_sec);
	 strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
	 milliseconds = tv.tv_usec / 1000;
	 sprintf (time_buf,"%s,%03ld", time_string, milliseconds);
	 cout<<time_buf<<" :  ";
}

int main(int argc, char *argv[])
{
    int sockfd, n, portno;
    char ip[15];
    struct sockaddr_in client;
    char message[256];
    
    if(argc<3){
		printTimeStamp();
		cout<<"error - invalid arguments\n";
		return 0;
    }

    strcpy(ip,argv[1]);
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
		printTimeStamp();
        cout<<"error creating socket\n";
		return 0;
	}
    
    bzero((char *) &client, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr(ip);
    client.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &client,sizeof(client)) < 0){ 
		printTimeStamp();
        cout<<"error connecting the client\n"; 
		return 0;
    }
    while(1){
		printTimeStamp();
		cout<<"Please enter the message: ";
		bzero(message,256);
		cin>>message;
		n = write(sockfd,message,strlen(message));
		if (n < 0){
			 printTimeStamp();
			 cout<<"error writing to socket";
			 return 0;
		}
		bzero(message,256);
		n = read(sockfd,message,255);
		if (n < 0) {
			 printTimeStamp();
			 cout<<"error reading from socket";
			 return 0;
		}
		printTimeStamp();
		cout<<message<<endl;
    }
    close(sockfd);
    return 0;
}