//
// Created by Remi on 04/01/2021.
//

#ifndef PROJET_SYSTEME_A2_MARSAL_RAHUEL_CONNEXION_H
#define PROJET_SYSTEME_A2_MARSAL_RAHUEL_CONNEXION_H

#define CONFIRM_MESSAGE "received"
#define L_CONFIRM_MESSAGE 8

/*
 * Connection function and structures
 */

typedef struct Connection {
    int socket;
    struct sockaddr_in coord;
} Connection;

int createSocket();

//Client
Connection* createConnection();
Connection* openConnection(const char* toAddr, const int port);

//Server
Connection* prepareHost(const int port);

#endif /*PROJET_SYSTEME_A2_MARSAL_RAHUEL_CONNEXION_H*/