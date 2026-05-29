#include <stdio.h> 
#include "telemetry.h"
#include "protocol.h"
#include <string.h>
#include <math.h>
#include <time.h>

#include <stdlib.h>
//Creates normal and abnormal telemetry messages 


// Function to generate a random double value between 0 and 1
static double generate_random_value(){
    return (double)rand() / RAND_MAX;
}

void clamp(double *value, double min, double max){
    if(*value < min) *value = min;
    if(*value > max) *value = max;
}
//generates a value from normal distribution using Box-Muller transform 
void generate_normalVariable(double mean, double stddev, double *out_value){

    double u1 = generate_random_value();
    double u2 = generate_random_value();
    double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
    *out_value = mean + z0 * stddev; 
}

void generate_telemetry_message(TelemetryMessage *msg){
    msg->timestamp = time(NULL);
    msg->device_id = rand() % 1000; // Random device ID between 0 and 999
    generate_normalVariable(25.0, 5.0, &msg->temperature); 
    clamp(&msg->temperature, -50.0, 150.0); // Clamp temperature to reasonable range
    generate_normalVariable(50.0, 20.0, &msg->cpu_usage); 
    clamp(&msg->cpu_usage, 0.0, 100.0); // Clamp CPU usage to 0-100%
    generate_normalVariable(100.0, 50.0, &msg->latency_ms); 
    if(msg->latency_ms < 0) msg->latency_ms = 0;
    msg->error_count = rand() % 10; 
    msg->is_anomaly = (generate_random_value() < 0.05) ? 1 : 0; // 5% chance of being an anomaly

    if(msg->is_anomaly){
        // Introduce anomalies by adding random spikes to temperature and latency
        msg->temperature += (generate_random_value() - 0.5) * 50.0; // Spike of up to +-25 degrees
        clamp(&msg->temperature, -50.0, 150.0);
        msg->latency_ms += (generate_random_value() - 0.5) * 200.0; // Spike of up to +-100 ms
        if(msg->latency_ms < 0) msg->latency_ms = 0;
        clamp(&msg->latency_ms, 0.0, 500.0); // Clamp latency to reasonable range
        
    }
}

