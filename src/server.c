#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "telemetry.h"
#include "protocol.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

pthread_mutex_t csv_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for synchronizing access to the CSV file


int write_server_csv(const TelemetryMessage *msg){
    pthread_mutex_lock(&csv_mutex);
    FILE *fp = fopen("data/telemetry_data.csv", "a");
    if(fp == NULL){
        perror("Failed to open file");
        pthread_mutex_unlock(&csv_mutex);
        return -1;
    }
    fprintf(fp, "%ld,%d,%.2f,%.2f,%.2f,%d,%d\n",
            msg->timestamp,
            msg->device_id,
            msg->temperature,
            msg->cpu_usage,
            msg->latency_ms,
            msg->error_count,
            msg->is_anomaly);
    fclose(fp);
    pthread_mutex_unlock(&csv_mutex);
    return 0;

}

void *handle_client(void *arg){
    int client_fd = (intptr_t)arg;
    char buffer[256];
    while(1){
        ssize_t bytes_recv = recv(client_fd,buffer,sizeof(buffer)-1,0);
        if(bytes_recv < 0){
            perror("recv failed");
            close(client_fd);
            return NULL;
        } else if(bytes_recv == 0){
            printf("Client disconnected.\n");
            close(client_fd);
            return NULL;
        }
        buffer[bytes_recv] = '\0'; // Null-terminate the received data
        TelemetryMessage msg; 
        if(protocol_decode_text(buffer,&msg) == 7){
            if(protocol_validate_message(&msg)){
                printf("Received valid message: %s\n", buffer);
                //Write to a csv file
                if(write_server_csv(&msg) < 0){
                    printf("Failed to write message to CSV: %s\n", buffer);
                }
        }
        } else {
            printf("Failed to decode message: %s\n", buffer);
        }
    }
    close(client_fd);
    return NULL;
}




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
    // Accept incoming connections by creating a new thread for each client that connects
    while(1){
        int client_fd;
        client_fd = accept(socket_fd,NULL,NULL);
        if(client_fd < 0){
            perror("accept failed");
            return -1;
        }
        
        pthread_t client_thread_id;
        if(pthread_create(&client_thread_id,NULL,handle_client,(void *)(intptr_t)client_fd) != 0){
            perror("Failed to create client thread");
            close(client_fd);
            continue;
        }
        pthread_detach(client_thread_id); // Detach the thread to allow it to run independently
        
    }


    close(socket_fd);
    return 0;

}