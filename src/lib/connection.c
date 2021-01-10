//
// Created by Remi on 04/01/2021.
//
#include<string.h>

#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<sys/socket.h>
#include<sys/types.h>

#include"connection.h"
#include"utils.c"

int createSocket() {
    int tempSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(tempSocket < 0) error("socket creation"); else return tempSocket;
}

Connection* createConnection() {
    Connection* connection = (Connection*) malloc(sizeof(Connection));
    if(connection == NULL) error("connection memory allocation");

    connection->socket = createSocket();
    memset(&connection->coord, 0x00, sizeof(struct sockaddr_in));
    connection->coord.sin_family = PF_INET;

    return connection;
}

Connection* openConnection(const char* toAddr, const int port) {
    Connection* connection = createConnection();

    inet_aton(toAddr, &connection->coord.sin_addr);
    connection->coord.sin_port = htons(port);

    if(connect(connection->socket, (struct sockaddr*) &connection->coord, sizeof(connection->coord)) == -1) error("connection failed"); else return connection;
}

Connection* prepareHost(const int port) {
    Connection* connection = createConnection();

    if (setsockopt(connection->socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) error("setsockopt(SO_REUSEADDR) failed");

    connection->coord.sin_port = htons(port);
    connection->coord.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(connection->socket, (struct sockaddr*) &connection->coord, sizeof(connection->coord)) == -1) error("bind error");

    if (listen(connection->socket, 5) == -1) error("listen error");

    return connection;
}
