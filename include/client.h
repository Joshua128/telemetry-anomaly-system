#ifndef CLIENT_H
#define CLIENT_H
int client_send_telemetry(const char *server_ip, int server_port);
int client_main();

#endif