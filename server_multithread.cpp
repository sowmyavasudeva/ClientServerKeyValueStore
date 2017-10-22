#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ctype.h>
#include <map>
#include <pthread.h>
#include <sstream>
#include <mutex>
#include <unistd.h>
using namespace std;
using std::map;

void* clientCommunication(void*);
int threadsocketfd, cpuCounter=-1, numCores = sysconf(_SC_NPROCESSORS_ONLN);	//numCores will hold the total number of cores in a system (to assign each thread to a core)
char message[256];
map<std::string, std::string> keyValueStore;
mutex mtx;

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
     socklen_t clientlength;
     struct sockaddr_in server, client;
     pthread_t tid;
	 
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
	 
	 //Client connection and communication - Implementation of key-value pair
     while(1){
	     clientlength = sizeof(client);
	     threadsocketfd = accept(socketfd, (struct sockaddr *) &client, &clientlength);
	     if (threadsocketfd < 0){ 
			 printTimeStamp(); cout<<"error on accepting the client\n";
			 continue;
		 } else {
			 printTimeStamp(); cout<<"A client connection has been established\n";
		 }

	     //spawning a new thread to communicate with each client
	    if(pthread_create(&tid, NULL, clientCommunication, NULL) < 0){
			cout<<"Error creating a thread\n";
		}		
	 }
     close(socketfd);
     return 0; 
}

//This function gets invoked whenever a new thread is spawned for communicating with the newly joined client
void* clientCommunication(void *ptr)
{
	std::string key,value,action;
	int counter,readStatus;

	//assign CPU cores to threads in round robin fashion
	cpuCounter = (cpuCounter + 1) % numCores;
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(cpuCounter,&mask);
	if(sched_setaffinity(0, sizeof(mask), &mask) < 0){
		printTimeStamp(); cout<<"Error while assigning the thread to a core..Letting OS choose the core by default..\n";
	}else{
		printTimeStamp(); cout<<"This thread is assigned to the core "<<cpuCounter<<"\n";
	}

	int newsocketfd = threadsocketfd;
	     //Reads get/put/del actions from the client and executes it
	     while(1){
		         bzero(message,256);
			 key.clear();
			 value.clear();
			 action.clear();
			 readStatus = read(newsocketfd,message,255);
		         if(readStatus < 0){
		     	        printTimeStamp(); cout<<"error reading from socket\n";
				break;
			 }else if(readStatus == 0){
			 	printTimeStamp(); cout<<"A client has exit the connection\n";
				break;
			 }

			 //parsing the message received from the client
			 counter=0;	//counter will have the number of inputs received from the client's message
		    	 std::stringstream ss;
			 ss.str(message);
			 std::getline(ss, action, ',');
			if(!action.empty()){
			 	counter++;
			 	getline(ss,key,',');
			 	if(!key.empty()){
					counter++;
					getline(ss,value,',');
					if(!value.empty())
						counter++;
				}
			}
			
		     for(int i=0;i<action.length();i++) action[i]=tolower(action[i]);
	
		     cout<<"Thread ID : "<<pthread_self()<<endl;
		     printTimeStamp(); cout<<"Client's action "<<action<<endl;
			 if(strcmp(action.c_str(),"get")==0 && counter==2){
					mtx.lock();
					if(keyValueStore.find(key) == keyValueStore.end())
						value = "Not Found";
					else
						value = keyValueStore.find(key)->second;
					mtx.unlock();
					printTimeStamp(); cout<<"The value is "<<value<<endl;
					if(write(newsocketfd,value.c_str(),value.length()) < 0) {
						printTimeStamp(); cout<<"error writing the value to socket\n";
						break;
				    }
				}
			 else if(strcmp(action.c_str(),"put")==0 && counter==3){
					mtx.lock();
					bool status = keyValueStore.insert(std::pair<std::string,std::string>(key,value)).second;
					mtx.unlock();
				 	if(status == false){
						if(write(newsocketfd,"Already Exists",15) < 0) {
							printTimeStamp(); cout<<"error writing to socket\n";
							break;
				 		}
					}
				 	else {
						if(write(newsocketfd,"Success",8) < 0) {
						printTimeStamp(); cout<<"error writing to socket\n";
						break;
						}
					}
			 	}
			 else if(strcmp(action.c_str(),"del")==0 && counter==2){
					mtx.lock();
					int index = keyValueStore.erase(key);
					mtx.unlock();
					if(index > 0){
					if(write(newsocketfd,"Success",8) < 0) {
						printTimeStamp(); cout<<"error writing to socket\n";
						break;
				        }
				    } else{
					if(write(newsocketfd,"Not Found",10) < 0) {
				                printTimeStamp(); cout<<"error writing to socket\n";
				                break;
				        }
				   }
			       }
			 else {
				 printTimeStamp(); cout<<"Invalid Action request from the client\n";
				 if(write(newsocketfd,"Invalid Action",15) < 0) {
					   	printTimeStamp(); cout<<"error writing to socket\n";
						break;
				 }
			 }
			 printTimeStamp(); cout<<"Printing the Key-Value list\n";
			 for(map<string,string>::iterator it=keyValueStore.begin(); it!=keyValueStore.end(); ++it)
				cout << it->first << " => " << it->second << '\n';
		     }

	close(newsocketfd);

}
