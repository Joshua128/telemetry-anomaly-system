#include <stdio.h>
#include "telemetry.h"
#include "protocol.h"
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "server.h"
#include "client.h"
#include <unistd.h>

int main(){
    // Start the server in a separate thread or process
    if(fork() == 0){
        server_start();
        return 0;
    }
    // Give the server a moment to start up
    sleep(1);
    // Run the client to send telemetry messages
    client_main();
    return 0;
}

