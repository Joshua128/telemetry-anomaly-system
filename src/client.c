#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "telemetry.h"
#include <stdio.h>
#include "protocol.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

sig_atomic_t volatile got_sigINT = 0;

void sig_int_handler(int signum){
    if(signum == SIGINT){
        got_sigINT = 1;
    }
}


void generate_message(char *buffer, size_t buffer_sz){
    TelemetryMessage msg;
    generate_telemetry_message(&msg);
    protocol_encode_text(&msg, buffer, buffer_sz);
} 

//Client main is the function each client will use when set up on a seperate thread eventually.

int client_main(){
    signal(SIGINT, sig_int_handler);
    int socket_fd;
    struct sockaddr_in server_addr;
    char buffer[256];
    // Create a socket
    socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd < 0){
        perror("socket creation failed");
        return -1;
    }
    // Set up the server address structure
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);
    // Connect to the server
    if(connect(socket_fd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
        perror("connect failed");
        return -1;
    }
    printf("Connected to server!\n");
    // Send telemetry messages to the server

    while(got_sigINT == 0){
        generate_message(buffer,sizeof(buffer));
        if(send(socket_fd,buffer,strlen(buffer),0) < 0){ //dont need mutex here since server doesnt care about message order and each client has its own socket
            perror("send failed");
            return -1;
        }
        sleep(1); 
    }
    close(socket_fd);
    return 0;
}


