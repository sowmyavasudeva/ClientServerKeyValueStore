#include <iostream>
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
#include <ctype.h>
using namespace std;

//To get the local time in the format (Year-Month-Date Hour:Minute:Seconds,Milliseconds)
void printTimeStamp(){
	 char time_buffer[60];
	 struct timeval time_val;
     	 struct tm* time_ptr;
     	 char time_string[60];
     	 long milliseconds;
	 
	 gettimeofday (&time_val, NULL); 
     	 time_ptr = localtime (&time_val.tv_sec);
	 strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", time_ptr);
	 milliseconds = time_val.tv_usec / 1000;
	 sprintf (time_buffer,"%s,%03ld", time_string, milliseconds);
	 cout<<time_buffer<<" :  ";
}

int main(int argc, char *argv[])
{
    int socketfd, portno;
    char ip[15];
    struct sockaddr_in client;
    char message[256];
    
	//Check the arguments
    if(argc < 3){
		printTimeStamp(); cout<<"error - invalid arguments (needs IP and port no. as arguments)\n";
		return 0;
    }
    strcpy(ip,argv[1]);
    portno = atoi(argv[2]);
	
	if(portno <= 2000) {
		cout<<"Invalid port number\n";
		return 0;
	}
    
	//Socket creation and connecting
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
		printTimeStamp(); cout<<"error creating socket\n";
		return 0;
	}
	printTimeStamp(); cout<<"Socket creation successful\n";
	
    bzero((char *) &client, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr(ip);
    client.sin_port = htons(portno);
    if (connect(socketfd,(struct sockaddr *) &client,sizeof(client)) < 0){ 
		printTimeStamp(); cout<<"error connecting the client\n"; 
		return 0;
    }
	printTimeStamp(); cout<<"The client is connected to the server\n";
	
    //Client communication with the server - client sends get,put and del requests to the server
	while(1){
		printTimeStamp(); cout<<"Please enter the action (get,key / put,key,value / del,key): ";
		bzero(message,256);
		cin>>message;
		if (write(socketfd,message,strlen(message)) < 0){
			 printTimeStamp(); cout<<"error writing to socket\n";
			 return 0;
		}
		
		bzero(message,256);
		if(read(socketfd,message,255) < 0){
			 printTimeStamp(); cout<<"error reading from socket\n";
			 return 0;
		}
		printTimeStamp();
		cout<<message<<endl;
    }
    close(socketfd);
    return 0;
}
