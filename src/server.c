#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(){

    int socket_fd;
    struct sockaddr_in server_addr;
    // Create a socket
    socket_fd = socket(AF_NET,SOCK_STREAM,0);
    if(socket_fd < 0){
        perror("socket creation failed");
        return -1;
    }
    // Set up the server address structure
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_NET;
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
        // Handle client connection (e.g., read/write data)
        close(client_fd);
        

    }


}