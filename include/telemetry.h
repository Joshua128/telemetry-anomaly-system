#ifndef TELEMETRY_H
#define TELEMETRY_H


typedef struct {
    long timestamp;
    int device_id;
    double temperature;
    double cpu_usage;
    double latency_ms;
    int error_count;
    int is_anomaly;
} TelemetryMessage;

void generate_telemetry_message(TelemetryMessage *msg);
void generate_normalVariable(double mean, double stddev, double *out_value);
void clamp(double *value, double min, double max);


#endif