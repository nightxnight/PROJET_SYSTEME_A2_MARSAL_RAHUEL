//
// Created by Remi on 04/01/2021.
//

#include<stdio.h>
#include<unistd.h>
#include <pthread.h>

#include"../lib/concert.c"
#include"../lib/connection.c"

#define NB_CLIENT_MAX 10
#define PROPERTIES_FILE_PATH "./../../res/properties"

int numberOfConcert;
Concert* listeConcert;
ConcertInformation* concertInformationList;
pthread_mutex_t* listeMutex;

Connection* hostConnection;

Properties* properties;
pthread_mutex_t mutexProperties;

typedef struct ThreadData {
    int threadNumber;
    Connection* connectionPtr;
} ThreadData;
pthread_t threadTab[NB_CLIENT_MAX];

void sendConcertInfoList(const int socket, const int threadNumber) {
    char sNumberOfFile[8]; sprintf(sNumberOfFile, "%d", numberOfConcert);
    send(socket, sNumberOfFile, strlen(sNumberOfFile), 0);
    char buffer[L_CONFIRM_MESSAGE];
    recv(socket, buffer, L_CONFIRM_MESSAGE, 0);
    for(int i=0; i < numberOfConcert; i++) {
        char* serializedConcertInfo = serializeConcertInformation(concertInformationList[i]);
        send(socket, serializedConcertInfo, strlen(serializedConcertInfo), 0);
        free(serializedConcertInfo);
        recv(socket, buffer, L_CONFIRM_MESSAGE, 0);
    }
}

void sendConcert(const int socket, const int concertId, const int threadNumber) {
    int wantedMutex = idxOfConcertInArray(listeConcert, concertId, numberOfConcert);
    pthread_mutex_lock(&listeMutex[wantedMutex]);

    char* serializedConcert = serializeConcert(*concertGetById(listeConcert, concertId, numberOfConcert));

    pthread_mutex_unlock(&listeMutex[wantedMutex]);

    send(socket, serializedConcert, strlen(serializedConcert), 0);
    free(serializedConcert);
}

void handleConcertReservation(const Connection* connexionClient, const int concertId, const int threadNumber) {
    while(1) {
        char buffer[32];
        memset(buffer,0,sizeof(buffer));
        recv(connexionClient->socket, buffer, 32, 0);
        if (strcmp("reserver", buffer) == 0) {
            send(connexionClient->socket, CONFIRM_MESSAGE, L_CONFIRM_MESSAGE, 0);

            char reservationBuffer[1024];
            memset(reservationBuffer, 0, sizeof(reservationBuffer));
            recv(connexionClient->socket, reservationBuffer, 1024, 0);

            if (strcmp("CANCEL", reservationBuffer) == 0) return;

            char *serializedDetails = (char *) malloc((strlen(reservationBuffer) + 1) * sizeof(char));
            if (!serializedDetails) error("memory allocation");
            strcpy(serializedDetails, reservationBuffer);

            Details *details = unserializeDetails(serializedDetails);
            free(serializedDetails);

            int wantedMutex = idxOfConcertInArray(listeConcert, concertId, numberOfConcert);
            pthread_mutex_lock(&listeMutex[wantedMutex]);
            int reservationResult = reserveSeat(concertGetById(listeConcert, concertId, numberOfConcert), *details);
            pthread_mutex_unlock(&listeMutex[wantedMutex]);

            char resultMessage[256];
            memset(resultMessage, 0, sizeof(resultMessage));
            if (reservationResult == RESERVATION_SUCCESS) {

                pthread_mutex_lock(&mutexProperties);
                incrementNumericalStr(properties->lastFileNumber);
                saveReservation(details, properties->lastFileNumber);
                saveProperties(*properties, PROPERTIES_FILE_PATH);
                pthread_mutex_unlock(&mutexProperties);

                sprintf(resultMessage,"%sLa reservation a bien ete effectue.\nVotre numéro de dossier : %s\nConservez le bien.\nLe nom de famille que vous avez communiqué constitue son mot de passe.\n", COLOR_GREEN, details->reservationId);
                printf("Thread number %d made a reservation, id = %s\n", threadNumber, details->reservationId);
                free(details);

            } else if (reservationResult == ALREADY_RESERVED)
                sprintf(resultMessage,"%sUn ou plusieurs des sièges demandés sont déjà ou viennent d'être réservé.\nNous sommes désolés.", COLOR_RED);
            else
                sprintf(resultMessage,"%sIl semble qu'il y a une erreur dans votre demande.\nLa reservation est impossible", COLOR_RED);
            send(connexionClient->socket, resultMessage, strlen(resultMessage), 0);
        }else if(strcmp("actualiser", buffer) == 0) {
            sendConcert(connexionClient->socket, concertId, threadNumber);
        } else if(strcmp("exit", buffer) == 0)
            break;
    }
}

void handleConcertCancelReservation(const Connection* connexionClient, const int threadNumber) {
    send(connexionClient->socket, CONFIRM_MESSAGE, L_CONFIRM_MESSAGE, 0);

    while(1) {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        recv(connexionClient->socket, buffer, 1024, 0);
        if (strcmp("exit",buffer) == 0) break;

        char filePath[64];
        memset(filePath, 0, sizeof(char));
        sprintf(filePath,"./../../res/reservations/reservation-%s", buffer);
        if(fileExist(filePath) == FALSE) {
            send(connexionClient->socket, "error", strlen("error"), 0);
            continue;
        } else send(connexionClient->socket, CONFIRM_MESSAGE, L_CONFIRM_MESSAGE, 0);

        char* serializedDetails = getFileContent(filePath);
        Details* details = unserializeDetails(serializedDetails);
        free(serializedDetails);

        memset(buffer, 0, sizeof(buffer));
        recv(connexionClient->socket, buffer, 1024, 0);
        if(strcmp("CANCEL", buffer) == 0) {
            continue;
        }
        int result = cancelReservation(concertGetById(listeConcert, details->concertId, numberOfConcert), details->reservationId, buffer);
        free(details);

        char resultMessage[128];
        memset(resultMessage, 0, sizeof(resultMessage));

        if (result == WRONG_INFORMATION) sprintf(resultMessage, "%sLes informations transmises sont incorrectes.", COLOR_RED);
        else if (result == MISSING_FILE)
            sprintf(resultMessage, "%sReservation introuvable, verifier votre numéro de dossier.\n", COLOR_RED);
        else if (result == CANCEL_SUCCESS) {
            sprintf(resultMessage, "%sVotre reservation a bien ete annule.", COLOR_GREEN);
            printf("Thread number %d cancelled a reservation\n", threadNumber);
        }
        else sprintf(resultMessage, "%sErreur lors de la tentative d'annulation de votre reservation.", COLOR_RED);

        send(connexionClient->socket, resultMessage, strlen(resultMessage), 0);
    }
}

void *actionThread(void *arg) {
    ThreadData* data = (ThreadData*) arg;
    int threadNumber = data->threadNumber;
    Connection* connexionClient = data->connectionPtr;
    free(data);

    printf("Thread number %d started\n",threadNumber);

    sendConcertInfoList(connexionClient->socket, threadNumber);
    char buffer[32];
    while(1) {
        memset(buffer,0,sizeof(buffer));
        recv(connexionClient->socket, buffer, 32, 0);

        if(is_numeric(buffer)) {
            int idConcert = atoi(buffer);
            if(idxOfConcertInArray(listeConcert, idConcert, numberOfConcert) != -1) {
                sendConcert(connexionClient->socket, idConcert, threadNumber);
                handleConcertReservation(connexionClient, idConcert, threadNumber);
            } else send(connexionClient->socket, "error", 5, 0);
        } else if(strcmp("annuler-reservation", buffer) == 0) {
            handleConcertCancelReservation(connexionClient, threadNumber);
        } else if(strcmp("exit", buffer) == 0) break;
    }

    close(connexionClient->socket);
    free(connexionClient);

    printf("killing thread number %d\n", threadNumber);
    pthread_cancel(threadTab[threadNumber]);
}

int main(const int argc, const char* argv[]) {
    system("clear");

    printf("Starting serveur.prog\n");
    char* serializedProperties = getFileContent(PROPERTIES_FILE_PATH);
    properties = loadProperties(serializedProperties);
    printf("Properties loaded\n");
    char* concertFolderPath = "./../../res/concerts";
    listeConcert = loadConcertsList(concertFolderPath, &numberOfConcert);
    printf("Concert loaded\n");
    concertInformationList = (ConcertInformation*) malloc(numberOfConcert * sizeof(ConcertInformation));
    if(!concertInformationList) error("memory allocation");
    for(int i=0; i < numberOfConcert; i++) {
        ConcertInformation* concertInfo = concertToInfos(listeConcert[i]);
        concertInformationList[i] = *concertInfo;
        free(concertInfo);
    }
    printf("Concert infos ready to send!\n");
    listeMutex = (pthread_mutex_t*) malloc(numberOfConcert * sizeof(listeMutex));
    if(!listeMutex) error("memory allocation");

    hostConnection = prepareHost(6000);
    printf("Host ready");
    int nbClientConnecte = 0;
    while(nbClientConnecte < NB_CLIENT_MAX){

        Connection* connexionClient = (Connection*) malloc(sizeof(Connection));
        if(!connexionClient) error("memory allocation");
        socklen_t coordLen = sizeof(connexionClient->coord);

        if ((connexionClient->socket = accept(hostConnection->socket, (struct sockaddr *) &connexionClient->coord, &coordLen)) == -1)
            error("connection failed");
        else {
            ThreadData* data = (ThreadData*) malloc(sizeof(ThreadData));
            data->threadNumber = nbClientConnecte;
            data->connectionPtr = connexionClient;
            if (pthread_create(&threadTab[data->threadNumber], NULL, actionThread, data) != 0){
                error("thread creation");
                exit(EXIT_FAILURE);
            }
            nbClientConnecte++;
        }
    }

    close(hostConnection->socket);
    free(hostConnection);

    printf("serveur.prog finished\n");
    return EXIT_SUCCESS;
}