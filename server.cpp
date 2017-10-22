#include<iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <map>
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
     int sockfd, newsockfd, portno;
     socklen_t clientlen;
     char message[256];
     struct sockaddr_in server, client;
     
     if(argc < 2){
		printTimeStamp();
		cout<<"Error - invalid argument";
		return 0;
	 }
     portno = atoi(argv[1]);
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) {
        printTimeStamp();
		cout<<"error creating a socket\n";
     	return 0;
	 }
     bzero((char *) &server, sizeof(server));
     server.sin_family = AF_INET;
     server.sin_addr.s_addr = INADDR_ANY;
     server.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
		printTimeStamp();
        cout<<"error while binding\n";
		return 0;
	 }
     listen(sockfd,3);
     clientlen = sizeof(client);
     while(1){
	     newsockfd = accept(sockfd, (struct sockaddr *) &client, &clientlen);
	     if (newsockfd < 0){ 
			printTimeStamp();
			cout<<"error on accepting the client\n";
		 }
	     while(1){
		     bzero(message,256);
		     if(read(newsockfd,message,255) <= 0){
		     	printTimeStamp();
				cout<<"error reading from socket\n";
				break;
			 }
		     printTimeStamp();
			 cout<<"Here is the message: "<<message<<endl;
		     if(write(newsockfd,"I got your message",18) < 0) {
					printTimeStamp();
					cout<<"error writing to socket\n";
					break;
				 }
			 }
             close(newsockfd);
		 }
     close(sockfd);
     return 0; 
}