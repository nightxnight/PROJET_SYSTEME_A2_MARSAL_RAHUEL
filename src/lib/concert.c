//
// Created by Remi on 04/01/2021.
//
#include<dirent.h>
#include<unistd.h>
#include<fcntl.h>

#include<string.h>

#include"concert.h"
#include"utils.c"

char** loadConcertsList(char* folderPath, int* numberOfFile) {
    DIR* folder = opendir(folderPath);
    if(folder == NULL) error("unknown folder");

    struct dirent *entry;
    int fileCount=0;
    while ((entry = readdir(folder)) != NULL) {
        if (entry->d_type == DT_REG)
            fileCount++;
    }

    char** fileList = (char**) malloc(fileCount * sizeof(char));
    if(fileList == NULL) error("memory allocation");

    rewinddir(folder);
    int i = 0;
    while ((entry = readdir(folder)) != NULL) {
        if (entry->d_type == DT_REG) {
            fileList[i] = (char*) malloc(strlen(entry->d_name) * sizeof(char));
            if(fileList[i] == NULL) error("memory allocation");

            strcpy(fileList[i], entry->d_name);
            i++;
        }
    }
    closedir(folder);
    *numberOfFile = fileCount;
    return fileList;
}

Concert* unserializeConcert(char* serializedStr) {
    Concert* concert = (Concert*) malloc(sizeof(Concert));
    if(!concert) error("memory allocation");

    char* token;
    char* rest = serializedStr;
    char* attributeRest;

    while ((token = strtok_r(rest, ";", &rest))) {
        attributeRest = token;

        char* attributeName = strtok_r(attributeRest, ":", &attributeRest);
        char* attributeValue = strtok_r(attributeRest, ":", &attributeRest);

        if(strcmp("id", attributeName) == 0) concert->concertId = atoi(attributeValue);
        else if(strcmp("artist", attributeName) == 0){
            concert->artist = (char*) malloc(strlen(attributeValue) * sizeof(char));
            strcpy(concert->artist, attributeValue);
        }
        else if(strcmp("additionnalInfos", attributeName) == 0) {
            concert->additionalInfos = (char*) malloc(strlen(attributeValue) * sizeof(char));
            strcpy(concert->additionalInfos, attributeValue);
        }
        else if(strcmp("date", attributeName) == 0) {
            Date* date = strToDate(attributeValue);
            concert->concertDate = *date;
            free(date);
        }
        else if(strcmp("bleachersRow", attributeName) == 0) concert->bleachersRow = atoi(attributeValue);
        else if(strcmp("seatPerRow", attributeName) == 0) concert->seatPerRow = atoi(attributeValue);
        else if(strcmp("reservedSeats", attributeName) == 0) concert->reservedSeats = *unserializeBleacher(attributeValue);
    }
    free(serializedStr);

    concert->blockedSeats.seatTaken = 0;

    return concert;
}

char* serializeConcert(const Concert concert) {
    char* buffer = (char*) malloc(BUFSIZ * sizeof(char));
    if(!buffer) error("memory allocation");

    char* sBleacher = serializeBleacher(concert.reservedSeats);
    char* sDate = dateToStr(concert.concertDate);
    sprintf(buffer, "id:%d;artist:%s;additionnalInfos:%s;date:%s;bleachersRow:%d;seatPerRow:%d;reservedSeats:%s", concert.concertId, concert.artist, concert.additionalInfos, sDate, concert.bleachersRow, concert.seatPerRow, sBleacher);

    free(sBleacher);
    free(sDate);

    char* sConcert = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
    strcpy(sConcert, buffer);

    free(buffer);

    return sConcert;
}

Bleacher* unserializeBleacher(char* serializedStr) {
    Bleacher* bleacher = (Bleacher*) malloc(sizeof(Bleacher));
    if(!bleacher) error("memory allocation");

    char* seatTaken = strtok_r(serializedStr, "@", &serializedStr);
    bleacher->seatTaken = atoi(seatTaken);
    bleacher->seats = (Seat*) malloc(bleacher->seatTaken * sizeof(Seat));
    if(!bleacher->seats) error("memory allocation");

    char* listOfSeats = strtok_r(serializedStr, "@", &serializedStr);

    for(int i=0; i < bleacher->seatTaken; i++) {
        char* seatCode = strtok_r(listOfSeats, ",", &listOfSeats);
        bleacher->seats[i] = *unserializeSeat(seatCode);
    }
    return bleacher;
}

char* serializeBleacher(Bleacher bleacher) {
    char* buffer = (char*) malloc(BUFSIZ * sizeof(char));
    if(!buffer) error("memory allocation");

    sprintf(buffer, "%d@", bleacher.seatTaken);

    for(int i=0; i < bleacher.seatTaken; i++) {
        char* sSeat = serializedSeat(bleacher.seats[i]);
        strcat(buffer, sSeat);
        strcat(buffer, ",");
        free(sSeat);
    }

    buffer[strlen(buffer) - 1] = ';';

    char* sBleacher = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
    strcpy(sBleacher, buffer);
    free(buffer);

    return sBleacher;
}

Seat* unserializeSeat(char* serializedStr) {
    Seat* seat = (Seat*) malloc(sizeof(Seat));
    if(!seat) error("memory allocation");

    char seatRow = strtok_r(serializedStr, "-", &serializedStr)[0];
    int seatNumber = atoi(strtok_r(serializedStr, "-", &serializedStr));

    seat->seatRow = seatRow;
    seat->seatNumber = seatNumber;

    return seat;
}
char* serializedSeat(Seat seat) {
    char* buffer = (char*) malloc(100 * sizeof(char));
    if(!buffer) error("memory allocation");

    sprintf(buffer, "%c-%d", seat.seatRow, seat.seatNumber);

    char* sSeat = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
    if(!sSeat) error("memory allocation");
    strcpy(sSeat, buffer);
    free(buffer);

    return sSeat;
}


void destroyConcertInstance(Concert* concert) {

}

bool seatEqual(Seat seat1, Seat seat2) {
    return (seat1.seatRow == seat2.seatRow) ? (seat1.seatNumber == seat2.seatNumber) ? TRUE : FALSE : FALSE;
}

bool isSeatFree(Concert concert, Seat seat) {
    if(concert.reservedSeats.seatTaken == 0) return TRUE;
    for(int i=0; i < concert.reservedSeats.seatTaken; i++) {
        if(seatEqual(seat, concert.reservedSeats.seats[i]) == TRUE) return FALSE;
    }

    if(concert.blockedSeats.seatTaken == 0) return TRUE;
    for(int j=0; j < concert.blockedSeats.seatTaken; j++) {
        if(seatEqual(seat, concert.blockedSeats.seats[j]) == TRUE) return FALSE;
    }
    return TRUE;
}

void blockSeat(Bleacher* bleacher, const Seat seat) {
    if(bleacher->seatTaken == 0) bleacher->seats = (Seat*) malloc(sizeof(Seat));
    else bleacher->seats = (Seat*) realloc(bleacher->seats, (bleacher->seatTaken + 1) * sizeof(Seat));
    if(!bleacher->seats) error("memory allocation");

    bleacher->seats[bleacher->seatTaken] = seat;
    bleacher->seatTaken++;
}

bool deblockSeat(Bleacher* bleacher, const Seat seat) {
    int idx = -1;
    for(int i=0; i < bleacher->seatTaken; i++) {
        if(seatEqual(seat, bleacher->seats[i]) == TRUE) idx = i;
    }
    if(idx == -1) return FALSE;
    free(&bleacher->seats[idx]);
    bleacher->seatTaken--;
    return TRUE;
}

bool reserveSeat(Concert* concert, const Seat seat, const Details personalDetails) {
    blockSeat(&concert->reservedSeats, seat);
    deblockSeat(&concert->blockedSeats, seat);
}


bool cancelBooking(Concert* concert, const char* fileNumber, const Details personalDetails) {
}




void displayConcert(Concert concert) {
    printf("concert id : %d\n", concert.concertId);
    printf("Associated file : %s\n", concert.fileName);
    printf("artist : %s\n", concert.artist);
    printf("additional information : %s\n", concert.additionalInfos);
    printf("date : %d/%d/%d\n", concert.concertDate.day, concert.concertDate.month, concert.concertDate.year);
    printf("number of row : %d\n", concert.bleachersRow);
    printf("seats per row : %d\n", concert.seatPerRow);
    for(int j=0; j < concert.blockedSeats.seatTaken; j++) {
        printf("\tblocked : %c-%d\n", concert.blockedSeats.seats[j].seatRow, concert.blockedSeats.seats[j].seatNumber);
    }
    for(int k=0; k < concert.reservedSeats.seatTaken; k++) {
        printf("\treserved : %c-%d\n", concert.reservedSeats.seats[k].seatRow, concert.reservedSeats.seats[k].seatNumber);
    }
}


Details* unserializeDetails(char* serializedStr) {
    Details* details = (Details*) malloc(sizeof(Details));
    if(!details) error("memory allocation");

    char* token;
    char* rest = serializedStr;
    char* attributeRest;

    while ((token = strtok_r(rest, ";", &rest))) {
        attributeRest = token;

        char* attributeName = strtok_r(attributeRest, ":", &attributeRest);
        char* attributeValue = strtok_r(attributeRest, ":", &attributeRest);

        if(strcmp("concertId", attributeName) == 0) details->concertId = atoi(attributeValue);
        else if(strcmp("firstname", attributeName) == 0){
            details->firstName = (char*) malloc(strlen(attributeValue) * sizeof(char));
            strcpy(details->firstName, attributeValue);
        }
        else if(strcmp("lastname", attributeName) == 0) {
            details->lastname = (char*) malloc(strlen(attributeValue) * sizeof(char));
            strcpy(details->lastname, attributeValue);
        }
        else if(strcmp("reservedSeats", attributeName) == 0) details->reservation = *unserializeBleacher(attributeValue);
    }
    free(serializedStr);

    return details;
}

char* serializeDetails(Details details) {
    char* buffer = (char*) malloc(BUFSIZ * sizeof(char));
    if(!buffer) error("memory allocation");

    char* sBleacher = serializeBleacher(details.reservation);

    sprintf(buffer, "concertId:%d;firstname:%s;lastname:%s;reservedSeats:%s", details.concertId, details.firstName, details.lastname, sBleacher);

    free(sBleacher);

    char* sDetail = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
    strcpy(sDetail, buffer);

    free(buffer);

    return sDetail;
}
