#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "telemetry.h"
#include "protocol.h"
#include <stdio.h>
#include <unistd.h>
int server_start(){

    int socket_fd;
    struct sockaddr_in server_addr;
    // Create a socket
    socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd < 0){
        perror("socket creation failed");
        return -1;
    }
    // Set up the server address structure
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);
    // Bind the socket to the address and port
    if(bind(socket_fd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
        perror("bind failed");
        return -1;
    }
    //listen for incoming connections
    if(listen(socket_fd,5) < 0){
        perror("listen failed");
        return -1;
    }
    printf("Server is listening on port 8080...\n");
    // Accept incoming connections
    while(1){
        int client_fd;
        client_fd = accept(socket_fd,NULL,NULL);
        if(client_fd < 0){
            perror("accept failed");
            return -1;
        }
        printf("Client connected!\n");
        

        //handle telemetry messages from client conencted
        char buffer[256];
        while(1){
            ssize_t bytes_recv = recv(client_fd,buffer,sizeof(buffer)-1,0);
            if(bytes_recv < 0){
                perror("recv failed");
                break;
            } else if(bytes_recv == 0){
                printf("Client disconnected.\n");
                break;
            }
            buffer[bytes_recv] = '\0'; // Null-terminate the received data
            TelemetryMessage msg; 
            if(protocol_decode_text(buffer,&msg) == 7){
                if(protocol_validate_message(&msg)){
                    printf("Received valid message: %s\n", buffer);
                } else {
                    printf("Received invalid message: %s\n", buffer);
                }
            } else {
                printf("Failed to decode message: %s\n", buffer);
            }
        }

        close(client_fd);
        break;  //One client for now, breaks on disconnect

    }

    close(socket_fd);
    return 0;

}