#ifndef NETWORK_H
#define NETWORK_H
#include "common.h"


int get_local_ip(std::vector<std::string> &ip_info);
int init_server(std::string ip, int port, int waiting_num=32, bool nonblock=false, std::string protocol_type="tcp");
int init_client(std::string ip, int port, int myport=-1, bool nonblock=false);
int init_udp_server(std::string ip, int port, bool nonblock);

#endif