#include <stdio.h>
#include "protocol.h" 


// Encode a TelemetryMessage into a text line (CSV format) 
int protocol_encode_text(const TelemetryMessage *msg, char *buffer, size_t buffer_size) {
    return snprintf(buffer, buffer_size, "%ld,%d,%.2f,%.2f,%.2f,%d,%d",
                    msg->timestamp,
                    msg->device_id,
                    msg->temperature,
                    msg->cpu_usage,
                    msg->latency_ms,
                    msg->error_count,
                    msg->is_anomaly);
}

// Decode a text line (CSV format) into a TelemetryMessage
int protocol_decode_text(const char *line, TelemetryMessage *out_msg) {
    return sscanf(line, "%ld,%d,%lf,%lf,%lf,%d,%d",
                  &out_msg->timestamp,
                  &out_msg->device_id,
                  &out_msg->temperature,
                  &out_msg->cpu_usage,
                  &out_msg->latency_ms,
                  &out_msg->error_count,
                  &out_msg->is_anomaly);
}

// Validate a TelemetryMessage (e.g., check for reasonable values)
int protocol_validate_message(const TelemetryMessage *msg) {
    if (msg->device_id < 0) return 0;
    if (msg->temperature < -50 || msg->temperature > 150) return 0;
    if (msg->cpu_usage < 0 || msg->cpu_usage > 100) return 0;
    if (msg->latency_ms < 0) return 0;
    if (msg->error_count < 0) return 0;
    return 1; // Valid message
}

