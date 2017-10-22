#include <iostream>
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
using std::map;

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
     int socketfd, newsocketfd, portno;
     socklen_t clientlength;
     char message[256];
	 std::string key,value;
     struct sockaddr_in server, client;
     map<std::string, std::string> keyValueStore;
	 
	 //Checking the number of input arguments
     if(argc < 2){
		printTimeStamp();
		cout<<"Error - invalid argument\n";
		return 0;
	 }
	 
	 //Converting the port number to integer and checking if it's greater than 2000
     portno = atoi(argv[1]);
	 if(portno <= 2000)
	 {
		 cout<<"Error - Use a port number greater than 2000";
		 return 0;
	 }
	 
	 //Socket creation and binding
     socketfd = socket(AF_INET, SOCK_STREAM, 0);
     if (socketfd < 0) {
        printTimeStamp(); cout<<"error creating a socket\n";
     	return 0;
	 }
	 
	 printTimeStamp(); cout<<"Socket creation successful\n";
	 
     bzero((char *) &server, sizeof(server));
     server.sin_family = AF_INET;
     server.sin_addr.s_addr = INADDR_ANY;
     server.sin_port = htons(portno);
     if (bind(socketfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
		printTimeStamp();
        cout<<"error while binding\n";
		return 0;
	 }
	 printTimeStamp(); cout<<"Socket binding to the port successful... Listening for client connections\n";
	 
	 //Server is listening to the port for client connections
     listen(socketfd,10);
     clientlength = sizeof(client);
	 
	 //Client connection and communication - Implementation of key-value pair
     while(1){
	     newsocketfd = accept(socketfd, (struct sockaddr *) &client, &clientlength);
	     if (newsocketfd < 0){ 
			 printTimeStamp(); cout<<"error on accepting the client\n";
			 continue;
		 } else {
			 printTimeStamp(); cout<<"A client connection has been established\n";
		 }
		 
		 //Reads get/put/del actions from the client and executes it
	     while(1){
		     bzero(message,256);
			 key.clear();
			 value.clear();
		     if(read(newsocketfd,message,255) <= 0){
		     	printTimeStamp(); cout<<"error reading from socket\n";
				break;
			 }
		     printTimeStamp(); cout<<"Client's action "<<message<<endl;
			 if(strcmp(message,"get")==0){
				if(read(newsocketfd,message,255) <= 0){
					printTimeStamp(); cout<<"error reading from socket\n";
				} else {
					key = message;
					if(keyValueStore.find(key) == keyValueStore.end())
						value = "Not Found";
					else
						value = keyValueStore.find(key)->second;
					printTimeStamp(); cout<<"The value is "<<value<<endl;
					if(write(newsocketfd,value.c_str(),value.length()) < 0) {
						printTimeStamp(); cout<<"error writing the value to socket\n";
						break;
				    }
				}
		     }
			 else if(strcmp(message,"put") == 0){
				 if(read(newsocketfd,message,255) <= 0){
					printTimeStamp(); cout<<"error reading from socket\n";
				 }
				 key = message;
				 bzero(message,256);
				 if(read(newsocketfd,message,255) <= 0){
					printTimeStamp(); cout<<"error reading from socket\n";
				 }
				 value = message;
				 if(keyValueStore.insert(std::pair<std::string,std::string>(key,value)).second == false){
					if(write(newsocketfd,"Already Exists",18) < 0) {
					printTimeStamp(); cout<<"error writing to socket\n";
					break;
				 }
				}
				 else {
					if(write(newsocketfd,"Success",18) < 0) {
						printTimeStamp(); cout<<"error writing to socket\n";
						break;
					}
				}
			 }
			 else if(strcmp(message,"del") == 0){
				 if(read(newsocketfd,message,255) <= 0){
					printTimeStamp(); cout<<"error reading from socket\n";
				} else {
					key = message;
					keyValueStore.erase(key);
					if(write(newsocketfd,"Success",18) < 0) {
						printTimeStamp(); cout<<"error writing to socket\n";
						break;
				    }
				}
			 }
			 else {
				 printTimeStamp(); cout<<"Invalid Action request from the client\n";
			 }
			 printTimeStamp(); cout<<"Printing the Key-Value list\n";
			 for(map<string,string>::iterator it=keyValueStore.begin(); it!=keyValueStore.end(); ++it)
				cout << it->first << " => " << it->second << '\n';
		     }
         close(newsocketfd);
	 }
     close(socketfd);
     return 0; 
}