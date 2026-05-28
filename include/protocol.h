#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>
#include "telemetry.h"

#define PROTOCOL_MAX_LINE 256

int protocol_encode_text(const TelemetryMessage *msg, char *buffer, size_t buffer_size);

int protocol_decode_text(const char *line, TelemetryMessage *out_msg);

int protocol_validate_message(const TelemetryMessage *msg);

int protocol_to_csv_line(const TelemetryMessage *msg, char *buffer, size_t buffer_size);

#endif