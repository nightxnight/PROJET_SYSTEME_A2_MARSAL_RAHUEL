#include<stdio.h>
#include"lib/concert.c"

int main(const int argc, const char* argv[]) {

    int numberOfFile;
    char* folderPath = "../res/concerts";
    char** fileList = loadConcertsList(folderPath, &numberOfFile);
    char filePath[100];
    Concert** listeConcert = (Concert**) malloc(numberOfFile * sizeof(Concert));
    for(int i=0; i < numberOfFile; i++) {
        strcat(filePath, folderPath);
        strcat(filePath, "/");
        strcat(filePath, fileList[i]);
        listeConcert[i] = unserializeConcert(getFileContent(filePath));
        listeConcert[i]->fileName = strdup(filePath);
        strcpy(filePath, "");
    }

    char* buffer = serializeConcert(*listeConcert[0]);
    printf("%s\n", buffer);
    free(buffer);


    /*
    Seat newBlocked; newBlocked.seatNumber = 15; newBlocked.seatRow = 'D';

    if(isSeatFree(*listeConcert[0], newBlocked) == TRUE) blockSeat(&listeConcert[0]->blockedSeats, newBlocked);

    Details det;
    reserveSeat(listeConcert[0], newBlocked, det);

    for(int i=0; i < numberOfFile; i++) {
        displayConcert(*listeConcert[i]);
    }
     */

    /*
    char* filePath = "../res/reservations/reservation-1";
    Details* details = unserializeDetails(getFileContent(filePath));
    char* buffer = serializeDetails(*details);
    printf("%s\n", buffer);
    free(buffer);
    */

}