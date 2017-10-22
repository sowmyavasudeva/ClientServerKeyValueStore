Implementation of Key-Value store using client-server socket programming

Server program:
1. The program takes the port number as an argument (using command line arguments). It is a multi-threaded process.
2. It validates the port number (Checks if it is a valid number and if it is greater than 2000 - to avoid the default port numbers).
3. It binds and listens to the port for client connections.
4. Server maintains a map to hold the key-value store (<strings,strings> pair).
5. Once a client connection is established, it spawns a new thread for the client and invoke a function.
6. The newly spawned thread will be communicating with the client as long as the client is alive.
7. The threads recieve messages from the corresponding client it is communicating with, checks if it is get/put/del and does the requested service to the map.
6. The server thread then sends a response to the client depending on the service requested by the client.
7. The threads print the thread ID, action received, the response and all the entries present in the map after each request from the client is processed.
8. When each thread is spawned, it is assigned to one of the CPU cores in Round-Robin fashion.
9. Messages are displayed with timestamps when a client connection is established, when a client exits, when the server receives a message from the client.

Client program:
1. The program takes the server IP address and the port number as command line arguments.
2. It connects to the server and sends (get/put/del) requests to the server. The format of the request message is
	get,<key>
	put,<key>,<value>
	del,<key>
3. The action strings get,del and put are not case sensitive. The key and the values are case sensitive.
3. The server responds to the request and the client prints the response with timestamp.
4. The client requests and the expected responses from the server are given below:
	REQUEST						RESPONSE
	get,<key>					<value>
							Not Found (if <key> is not present)
													
	put,<key>,<value>				Already Exists if <key> is already available
							Success (if insertion is successful)
							
															
	del,<key>					Success (Deletes the key-value entry if key exists)
							Not Found (if the key is not available)