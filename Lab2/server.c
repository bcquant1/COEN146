//
//  Behnam Dezfouli
//  CSEN, Santa Clara University
//

// This program implements a server that accepts connection from a client and copies the received bytes to a file
//
// The input arguments are as follows:
// argv[1]: Sever's port number



#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>


int main (int argc, char *argv[])
{
	char    message[10] = "received!";  // message to be sent to the client when the destination file name is received
	int     net_bytes_read;             // number of bytes received over socket
	int     socket_fd = 0;              // socket descriptor
	int     connection_fd = 0;          // new connection descriptor
	struct  sockaddr_in serv_addr;      // Address format structure
	char    net_buff[1024];             // buffer to hold characters read from socket
	FILE    *dest_file;                 // pointer to the file that will include the received bytes over socket
	
	
	if (argc < 2) // Note: the name of the program is counted as an argument
	{
		printf ("Port number not specified!\n");
		return 1;
	}
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));
	
	socket_fd = socket(AF_INET,SOCK_STREAM,0);
	if( (socket_fd < 0) ) {
		printf("error creating socket\n");
		return -1;
	}
	if(bind(socket_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0) {
		printf("error binding socket\n");
		return -1;
	}
	if(listen(socket_fd,10) < 0) {
		printf("error listening to socket\n");
		return -1;
	}
	
	int n_socket_fd = accept(socket_fd,(struct sockaddr *) NULL,NULL);
	if(n_socket_fd < 0) {
		printf("error establishing connection\n");
		return -1;
	}
	
	bzero(net_buff,256);
	net_bytes_read = read(n_socket_fd,net_buff,255);
	if (net_bytes_read < 0) {
		printf("error reading\n");
		return -1;
	}
	printf("Filename: %s\n",net_buff);

	net_bytes_read = write(n_socket_fd, "Copy Ready",10);
	if (net_bytes_read < 0) {
		printf("error writing\n");
		return -1;
	}

	dest_file = fopen(net_buff,"w");
	bzero(net_buff,10);
	int i = 0;
	int pack_size;
	while(pack_size = read(n_socket_fd,net_buff,10), pack_size > 0) {
		printf("packet #%d; size: %d\n",i,pack_size);
		fwrite(net_buff,1,pack_size,dest_file);
		++i;
	}
	fclose(dest_file);
}
