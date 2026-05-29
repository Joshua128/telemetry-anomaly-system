#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "telemetry.h"
#include <stdio.h>
#include "protocol.h"
#include <unistd.h>
#include <arpa/inet.h>


void generate_message(char *buffer, size_t buffer_sz){
    TelemetryMessage msg;
    generate_telemetry_message(&msg);
    protocol_encode_text(&msg, buffer, buffer_sz);
} 



int client_main(){
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
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Represents local host
    server_addr.sin_port = htons(8080);
    // Connect to the server
    if(connect(socket_fd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
        perror("connect failed");
        return -1;
    }
    printf("Connected to server!\n");
    // Send telemetry messages to the server
    char buffer[256];
    for(int i = 0; i < 10; i++){
        generate_message(buffer, sizeof(buffer));
        send(socket_fd, buffer, strlen(buffer), 0);
        printf("Sent message: %s\n", buffer);
        sleep(1); // Wait for a second before sending the next message
    }
    close(socket_fd);
    return 0;
}

