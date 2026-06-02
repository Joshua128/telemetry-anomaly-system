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

// used for clients to connect to server and send telemetry messages
int main(){
    client_main();  
    return 0;
}