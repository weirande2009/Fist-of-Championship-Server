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
    // Instantiate a MainClient object
    MainClient *main_client = new MainClient(fd, socket_addr);
    // Set client no
    main_client->SetClientNo(this->clients.size());
    // Add client to clients
    this->clients.push_back(main_client);
    // Add client to the pool
    this->client_pool[fd] = main_client;
    return 0;
}

/******************************************
 * Function: Get client object
 * Parameters: 0
 * Return: None
 *****************************************/
MainClient* ClientPool::GetClient(int fd){
    return this->client_pool[fd];
}

void ClientPool::RemoveClient(int fd){
    MainClient* client = GetClient(fd);
    if(client != nullptr){
        client_pool.erase(fd);
        clients.erase(clients.begin()+client->GetClientNo());
        close(fd);
        delete client;
    }
}
