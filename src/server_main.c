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
#include <pthread.h>

int main(){
    // Start the server in a separate thread or process
    server_start(); // This will block and run the server loop
    return 0;
}

