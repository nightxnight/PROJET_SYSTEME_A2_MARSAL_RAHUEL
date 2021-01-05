//
// Created by Remi on 04/01/2021.
//

#include<stdio.h>
#include<unistd.h>

#include"../lib/connection.c"

int main(const int argc, const char* argv[]) {

    Connection* hostConnection = prepareHost(6000);
    Connection clientConnection;

    socklen_t coordLen = sizeof(clientConnection.coord);
    if ((clientConnection.socket = accept(hostConnection->socket, (struct sockaddr *) &clientConnection.coord,
                                        &coordLen)) == -1) error("connection failed");
    printf("Client connected\n");
    close(hostConnection->socket);
    close(clientConnection.socket);
}