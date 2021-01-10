//
// Created by Remi on 04/01/2021.
//
#include<stdio.h>
#include<unistd.h>

#include"../lib/connection.c"
#include"../lib/concert.c"

#define IP_SERVER "127.0.0.1"
#define PORT_SERVER 6000

char buffer[BUFSIZ];
char confirmationBuffer[L_CONFIRM_MESSAGE];

Connection* connection;

int nbConcertDispo;
ConcertInformation* listeConcertInfos;
Concert* concert;

void clearBuffer() {
    memset(buffer,0,sizeof(buffer));
}

void readInput() {
    clearBuffer();
    scanf("%s", &buffer);
}

void reserve() {
    send(connection->socket, "reserver", strlen("reserver"), 0);
    recv(connection->socket, confirmationBuffer, L_CONFIRM_MESSAGE, 0);
    printf("Vous allez effectuer une demande de reservation.\n");
    printf(COLOR_RED);
    printf("Vous pouvez annuler a tout moment en entrant \"CANCEL\".\n");
    printf(COLOR_WHITE);
    entrerplace:
    printf("\nEntrez la ou les places souhaitez, séparez les par des virgules (exemple : A-1,A-2).\n");

    readInput();
    char sReservedSeat[1024];
    memset(sReservedSeat,0,sizeof(sReservedSeat));

    if(strcmp("CANCEL", buffer) == 0) {
        send(connection->socket, "CANCEL", strlen("CANCEL"), 0);
        return;
    }
    else if(strstr(buffer, "-") != NULL) {
        int numberOfSeatReserved = 0;
        char* token;
        char* rest = buffer;
        while ((token = strtok_r(rest, ",", &rest))) {
            if(strstr(token, "-") == NULL) goto invalidSeat;
            char *seatRow = strtok_r(token, "-", &token);
            if(strlen(seatRow) != 1) goto pasplace;
            if(strlen(token) == 0) goto pasplace;
            char *seatNumber = strtok_r(token, "-", &token);
            if(!is_numeric(seatNumber)) goto pasplace;
            char serizalizedSeat[8];
            memset(serizalizedSeat,0,sizeof(serizalizedSeat));
            sprintf(serizalizedSeat,"%c-%d", seatRow[0], atoi(seatNumber));
            Seat* seat = unserializeSeat(serizalizedSeat);
            if(validSeat(concert, *seat) == FALSE) goto invalidSeat;
            else if(isSeatFree(concert, *seat) == FALSE) goto seatNotFree;
            else sprintf(sReservedSeat, "%s%c-%d,", sReservedSeat, seatRow[0], atoi(seatNumber));
            numberOfSeatReserved++;
            free(seat);
        }
        sprintf(sReservedSeat, "%d@%s", numberOfSeatReserved, strdup(sReservedSeat));
        sReservedSeat[strlen(sReservedSeat) - 1] = '\0';
    } else {
        pasplace:
        printf("Vous n'avez pas entre une place.\n"); goto entrerplace;
        invalidSeat:
        printf("Un ou plusieurs des sieges entre n'est pas valide.\n"); goto entrerplace;
        seatNotFree:
        printf("Un ou plusieurs sieges que vous voulez reserve n'est pas disponible.\n"); goto entrerplace;
    }

    printf("\nVous aller reserver les places suivantes : %s\n", sReservedSeat);
    char firstname[25]; char lastname[25];
    while(1) {
        printf("Veuillez indiquer votre prenom (25 caractères maximum : )\n");
        memset(firstname, 0, sizeof(firstname));
        readInput();
        if(strcmp("CANCEL", buffer) == 0) {
            send(connection->socket, "CANCEL", strlen("CANCEL"),  0);
            return;
        } else if(strlen(buffer) > 25) {
            printf("Le prenom saisie est trop long.\n");
        } else {
            strcpy(firstname, buffer);
            break;
        }
    }

    while(1) {
        printf("Veuillez indiquer votre nom (25 caractères maximum : )\n");
        memset(lastname, 0, sizeof(lastname));
        readInput();
        if(strcmp("CANCEL", buffer) == 0) {
            send(connection->socket, "CANCEL", strlen("CANCEL"),  0);
            return;
        } else if(strlen(buffer) > 25) {
            printf("Le nom saisie est trop long.\n");
        } else {
            strcpy(lastname, buffer);
            break;
        }
    }

    printf(COLOR_RED);
    printf("En tapant \"CONFIRMER\" vous aller effectuer à la demande de réservation.\n");
    printf(COLOR_WHITE);

    readInput();
    if(strcmp("CONFIRMER", buffer) == 0) {
        Details* details = createDetails(concert->concertId, firstname, lastname, sReservedSeat);
        char* serializedDetails = serializeDetails(*details);
        send(connection->socket, serializedDetails, strlen(serializedDetails), 0);
        free(serializedDetails);
        clearBuffer();
        recv(connection->socket, buffer, BUFSIZ, 0);
        printf("%s\n.", buffer);
    } else {
        send(connection->socket, "CANCEL", strlen("CANCEL"),  0);
        printf("Votre demande n'a pas été pris en prise en compte.\n");
    }
    printf(COLOR_WHITE);
    printf("entrez \"OK\" pour retourner sur le concert\n");
    readInput();
}

void afficherMenu() {
    system("clear");
    printf("Bienvenue sur notre serveur de reservation!\n");
    printf("Ici reserver les places de vos concerts favoris.\n");
    printf("\n");
    printf("Concert à venir :\n");
    for(int i=0; i < nbConcertDispo; i++) {
        printf("\t%d - %s\n", i+1, listeConcertInfos[i].libelleConcert);
    }
    printf("taper \"annuler-reservation\" pour procéder à l'annulation d'une reservation.\n");
    printf("\nTaper \"exit\" pour quitter l'application.\n");
}

void afficherMenuConcert() {
    clearBuffer();
    recv(connection->socket, buffer, BUFSIZ, 0);
    if(strcmp("error", buffer) == 0) {
        printf(COLOR_RED);
        printf("Erreur ce concert n'existe pas, retour au menu.\n\n");
        printf(COLOR_WHITE);
        sleep(1);
        return;
    } else {
        concert = unserializeConcert(buffer);
    }

    affichageConcert:
    system("clear");
    displayConcert(*concert);
    printf("\nEntrez \"reserver\" pour effectuer une demande de reservation.\n");
    printf("Entrez \"actualiser\" pour actualiser la liste des places.\n");
    printf("Entrez \"exit\" pour revenir au menu principal.\n");
    while(1) {
        readInput();
        if(strcmp("exit", buffer) ==0) {
            send(connection->socket, "exit", strlen("exit"), 0);
            break;
        } else if(strcmp("reserver", buffer) == 0) {
            reserve();
            send(connection->socket, CONFIRM_MESSAGE, L_CONFIRM_MESSAGE, 0);
            afficherMenuConcert();
        } else if(strcmp("actualiser", buffer) == 0) {
            send(connection->socket, "actualiser", strlen("actualiser"), 0);
        } else {
            printf(COLOR_RED);
            printf("commande inconnue\n");
            printf(COLOR_WHITE);
        }
    }
    destroyConcertInstance(concert);
}

void afficherMenuAnnulation() {
    recv(connection->socket, buffer, BUFSIZ, 0);
    affichageAnnulation:
    system("clear");
    printf("Effectuer une demande d'annulation de reservation\n");
    printf("Vous pouvez quitter ce menu en entrant \"exit\"\n");
    printf("\nVeuillez fournir un numéro de dossier à 10 chiffres : \n");
    while(1) {
        readInput();
        if (strcmp("exit", buffer) == 0) {
            send(connection->socket, "exit", strlen("exit"), 0);
            break;
        } else if (strlen(buffer) == 10) {
            send(connection->socket, buffer, 10, 0);
            clearBuffer();
            recv(connection->socket, buffer, BUFSIZ, 0);
            if (strcmp("error", buffer) == 0) {
                printf(COLOR_RED);
                printf("Votre reservation est introuvable.\nVérifier votre numero de dossier.\n");
                printf(COLOR_WHITE);
            } else {
                printf("Veuillez entrez votre mot de passe : \n");
                printf("Vous pouvez annuler avec \"CANCEL\".\n");
                readInput();
                if(strcmp("CANCEL", buffer) == 0) {
                    send(connection->socket, "CANCEL", strlen("CANCEL"), 0);
                    goto affichageAnnulation;
                }
                send(connection->socket, buffer, strlen(buffer), 0);
                recv(connection->socket, buffer, BUFSIZ, 0);
                printf("%s\n", buffer);
                printf(COLOR_WHITE);
                printf("entrez \"OK\" pour revenir au menu d'annulation\n");
                readInput();
                goto affichageAnnulation;
            }
        } else {
            printf(COLOR_RED);
            printf("Vous devez fournir un numéro de dossier.\n");
            printf(COLOR_WHITE);
        }
    }
}

void loadConcertInfosList() {
    char sNbConcertDispo[8];
    recv(connection->socket, sNbConcertDispo, 8, 0);
    nbConcertDispo = atoi(sNbConcertDispo);

    listeConcertInfos = (ConcertInformation*) malloc(nbConcertDispo * sizeof(ConcertInformation));
    send(connection->socket, CONFIRM_MESSAGE, L_CONFIRM_MESSAGE, 0);

    for(int i=0; i < nbConcertDispo; i++) {
        recv(connection->socket, buffer, BUFSIZ, 0);
        ConcertInformation* concertInfo = unserializeConcertInformation(buffer);

        listeConcertInfos[i] = *concertInfo;

        clearBuffer();
        free(concertInfo);
        send(connection->socket, CONFIRM_MESSAGE, L_CONFIRM_MESSAGE, 0);
    }
}

int main(const int argc, const char* argv[]) {
    system("clear");
    printf("starting client.prog\n");

    printf("Connexion à %s:%d\n", IP_SERVER, PORT_SERVER);
    connection = openConnection(IP_SERVER, PORT_SERVER);
    printf("Connexion établie\n");

    loadConcertInfosList();
    afficherMenu();
    while(1) {
        readInput();
        if(is_numeric(buffer)) {
            int idx = atoi(buffer);
            if((idx < 0) || (nbConcertDispo < idx)) {
                printf(COLOR_RED);
                printf("Le numéro que vous avez indiqué est invalide.\n\n\n");
                printf(COLOR_WHITE);
            } else {
                idx--;
                clearBuffer();
                sprintf(buffer,"%d", listeConcertInfos[idx].concertId);
                send(connection->socket, buffer, strlen(buffer), 0);
                system("clear");
                printf("Le serveur envoie les informations concernant : %s\n", listeConcertInfos[idx].libelleConcert);
                afficherMenuConcert();
                system("clear");
                afficherMenu();
            }
        } else if(strcmp("annuler-reservation", buffer) == 0) {
            send(connection->socket, "annuler-reservation", strlen("annuler-reservation"), 0);
            afficherMenuAnnulation();
            afficherMenu();
        } else if(strcmp("exit", buffer) == 0) break;
        else {
            printf(COLOR_RED);
            printf("Commande inconnu\n");
            printf(COLOR_WHITE);
        }
    }

    printf("Deconnexion de %s:%d\n", IP_SERVER, PORT_SERVER);
    send(connection->socket, "exit", 4, 0);

    close(connection->socket);
    free(connection);
    printf("client.prog finished\n");
    return EXIT_SUCCESS;
}
