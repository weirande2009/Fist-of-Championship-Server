#include "server/ClientPool.h"

/******************************************
 * Function: Initialize ClientPool
 * Parameters: 0
 * Return: None
 *****************************************/
ClientPool::ClientPool(){

}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
ClientPool::~ClientPool(){

}

/******************************************
 * Function: Add client to client pool
 * Parameters: 0
 * Return: None
 *****************************************/
int ClientPool::AddClient(int fd, sockaddr_in socket_addr){
    // Add client to the pool
    this->client_pool[fd] = std::make_unique<MainClient>(fd, socket_addr);
    return 0;
}

/******************************************
 * Function: Get client object
 * Parameters: 0
 * Return: None
 *****************************************/
MainClient* ClientPool::GetClient(int fd){
    return this->client_pool[fd].get();
}

void ClientPool::RemoveClient(int fd){
    MainClient* client = GetClient(fd);
    if(client != nullptr){
        client_pool.erase(fd);
        close(fd);
    }
}
