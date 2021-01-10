//
// Created by Remi on 09/01/2021.
//
#include<stdio.h>
#include<stdlib.h>

#include<string.h>

#include<unistd.h>
#include<dirent.h>
#include<fcntl.h>

#include"concert.h"

/*
 * Fonctions de sérialization
 */

Concert* unserializeConcert(char* serializedStr) {
    Concert* concert = (Concert*) malloc(sizeof(Concert));
    if(!concert) error("memory allocation");

    char* token;
    char* rest = serializedStr;

    while ((token = strtok_r(rest, ";", &rest))) {
        char *attributeRest = token;

        char *attributeName = strtok_r(attributeRest, ":", &attributeRest);
        char *attributeValue = strtok_r(attributeRest, ":", &attributeRest);

        if (strcmp("id", attributeName) == 0) concert->concertId = atoi(attributeValue);
        else if (strcmp("artist", attributeName) == 0) {
            concert->artist = (char *) malloc(strlen(attributeValue) * sizeof(char));
            if(!concert->artist) error("memory allocation");
            strcpy(concert->artist, attributeValue);
        } else if (strcmp("name", attributeName) == 0) {
            concert->concertName = (char *) malloc(strlen(attributeValue) * sizeof(char));
            if(!concert->concertName) error("memory allocation");
            strcpy(concert->concertName, attributeValue);
        } else if (strcmp("additionnalInfos", attributeName) == 0) {
            concert->additionalInfos = (char *) malloc(strlen(attributeValue) * sizeof(char));
            if(!concert->additionalInfos) error("memory allocation");
            strcpy(concert->additionalInfos, attributeValue);
        } else if (strcmp("date", attributeName) == 0) {
            Date *date = strToDate(attributeValue);
            concert->concertDate = *date;
            free(date);
        } else if (strcmp("bleacher", attributeName) == 0) {
            Bleacher *bleacher = unserializeBleacher(attributeValue);
            concert->bleacher = *bleacher;
        }
    }
    return concert;
}
char* serializeConcert(const Concert concert) {
    char* buffer = (char*) malloc(BUFSIZ * sizeof(char));
    if(!buffer) error("memory allocation");

    char* sBleacher = serializeBleacher(concert.bleacher);
    char* sDate = dateToStr(concert.concertDate);
    sprintf(buffer, "id:%d;artist:%s;name:%s;additionnalInfos:%s;date:%s;%s", concert.concertId, concert.artist, concert.concertName, concert.additionalInfos, sDate, sBleacher);

    free(sBleacher);
    free(sDate);

    char* sConcert = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
    strcpy(sConcert, buffer);

    free(buffer);

    return sConcert;
}

Bleacher* unserializeBleacher(char* serializedStr) {
    Bleacher *bleacher = (Bleacher *) malloc(sizeof(Bleacher));
    if (!bleacher) error("memory allocation");

    char *token;
    char *rest = serializedStr;

    while ((token = strtok_r(rest, "|", &rest))) {
        char *attributeRest = token;

        char *attributeName = strtok_r(attributeRest, "=", &attributeRest);
        char *attributeValue = strtok_r(attributeRest, "=", &attributeRest);

        if (strcmp("bleachersRow", attributeName) == 0) bleacher->bleachersRow = atoi(attributeValue);
        else if (strcmp("seatPerRow", attributeName) == 0) bleacher->seatPerRow = atoi(attributeValue);
        else if (strcmp("reservedSeats", attributeName) == 0) {
            bleacher->seats = (Seat **) malloc(bleacher->bleachersRow * sizeof(Seat*));
            if(!bleacher->seats) error("memory allocation");
            for(int i=0; i < bleacher->bleachersRow; i++) {
                bleacher->seats[i] = (Seat*) malloc(bleacher->seatPerRow * sizeof(Seat));
                if(!bleacher->seats[i]) error("memory allocation");
            }

            for(int i=0; i < bleacher->bleachersRow; i++)
                for(int j=0; j < bleacher->seatPerRow; j++) {
                    bleacher->seats[i][j].seatRow = (char) ASCII_A_CODE + i;
                    bleacher->seats[i][j].seatNumber = j+1;
                }

            char *serializedSeats = attributeValue;

            char *seatTaken = strtok_r(serializedSeats, "@", &serializedSeats);
            bleacher->seatTaken = atoi(seatTaken);

            char *listOfSeats = strtok_r(serializedSeats, "@", &serializedSeats);

            for (int i = 0; i < bleacher->seatTaken; i++) {
                char *seatCode = strtok_r(listOfSeats, ",", &listOfSeats);
                Seat *seat = unserializeSeat(seatCode);
                getSeat(bleacher, seat->seatRow, seat->seatNumber)->reserved=TRUE;
                free(seat);
            }
        }
    }

    return bleacher;
}

char* serializeBleacher(const Bleacher bleacher) {
    char* buffer = (char*) malloc(BUFSIZ * sizeof(char));
    if(!buffer) error("memory allocation");

    char* sReservedSeat = (char*) malloc(BUFSIZ * sizeof(char));
    if(!sReservedSeat) error("memory allocation");
    sReservedSeat[0] = '\0';

    for(int i=0; i < bleacher.bleachersRow; i++)
        for(int j=0; j < bleacher.seatPerRow; j++) {
            if(bleacher.seats[i][j].reserved == TRUE) {
                char* serializedSeat = serializeSeat(bleacher.seats[i][j]);
                sprintf(sReservedSeat, "%s%s,", sReservedSeat, serializedSeat);
                free(serializedSeat);
            }
        }

    sReservedSeat[strlen(sReservedSeat) - 1] = ';';

    sprintf(buffer, "bleacher:bleachersRow=%d|seatPerRow=%d|reservedSeats=%d@%s", bleacher.bleachersRow, bleacher.seatPerRow, bleacher.seatTaken, sReservedSeat);

    free(sReservedSeat);

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
char* serializeSeat(const Seat seat) {
    char* buffer = (char*) malloc(8 * sizeof(char));
    if(!buffer) error("memory allocation");

    sprintf(buffer, "%c-%d", seat.seatRow, seat.seatNumber);

    char* sSeat = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
    if(!sSeat) error("memory allocation");
    strcpy(sSeat, buffer);
    free(buffer);

    return sSeat;
}

ConcertInformation* unserializeConcertInformation(char* serializedStr) {
    ConcertInformation* concertInfos = (ConcertInformation*) malloc(sizeof(ConcertInformation));
    if(!concertInfos) error("memory allocation");

    char* token;
    char* rest = serializedStr;

    while ((token = strtok_r(rest, ";", &rest))) {
        char *attributeRest = token;

        char *attributeName = strtok_r(attributeRest, ":", &attributeRest);
        char *attributeValue = strtok_r(attributeRest, ":", &attributeRest);

        if (strcmp("concertId", attributeName) == 0) concertInfos->concertId = atoi(attributeValue);
        else if (strcmp("libelle", attributeName) == 0) {
            concertInfos->libelleConcert = (char *) malloc(strlen(attributeValue) * sizeof(char));
            if (!concertInfos->libelleConcert) error("memory allocation");
            strcpy(concertInfos->libelleConcert, attributeValue);
        }
    }
    return concertInfos;
}

char* serializeConcertInformation(ConcertInformation concertInfos)  {
    char* buffer = (char*) malloc(BUFSIZ * sizeof(char));

    sprintf(buffer, "concertId:%lld;libelle:%s;", concertInfos.concertId, concertInfos.libelleConcert);

    char* sConcertInfos = (char*) malloc((strlen(buffer) + 1) *sizeof(char));
    strcpy(sConcertInfos, buffer);
    free(buffer);

    return sConcertInfos;
}

Details* unserializeDetails(char* serializedStr) {
    Details* details = (Details*) malloc(sizeof(Details));
    if(!details) error("memory allocation");

    char* token;
    char* rest = serializedStr;

    while ((token = strtok_r(rest, ";", &rest))) {
        char* attributeRest = token;

        char* attributeName = strtok_r(attributeRest, ":", &attributeRest);
        char* attributeValue = strtok_r(attributeRest, ":", &attributeRest);

        if(strcmp("reservationId", attributeName) == 0) {
            strcpy(details->reservationId, attributeValue);
            details->reservationId[10] = '\0';
        }
        else if(strcmp("concertId", attributeName) == 0) details->concertId = atoi(attributeValue);
        else if(strcmp("firstname", attributeName) == 0){
            details->firstName = (char*) malloc(strlen(attributeValue) * sizeof(char));
            if(!details->firstName) error("memory allocation");
            strcpy(details->firstName, attributeValue);
        }
        else if(strcmp("lastname", attributeName) == 0) {
            details->lastname = (char*) malloc(strlen(attributeValue) * sizeof(char));
            if(!details->lastname) error("memory allocation");
            strcpy(details->lastname, attributeValue);
        }
        else if(strcmp("reservedSeats", attributeName) == 0) {
            char *serializedSeats = attributeValue;

            char *seatTakenStr = strtok_r(serializedSeats, "@", &serializedSeats);
            details->numberOfSeatReserved = atoi(seatTakenStr);

            details->reservedSeat = (Seat*) malloc(details->numberOfSeatReserved * sizeof(Seat));
            if(!details->reservedSeat) error("memory allocation");

            char *listOfSeats = strtok_r(serializedSeats, "@", &serializedSeats);
            for (int i = 0; i < details->numberOfSeatReserved ; i++) {
                char *seatCode = strtok_r(listOfSeats, ",", &listOfSeats);
                Seat *seat = unserializeSeat(seatCode);
                details->reservedSeat[i] = *seat;
                free(seat);
            }
        }
    }

    return details;
}
char* serializeDetails(const Details details) {
    char* buffer = (char*) malloc(BUFSIZ * sizeof(char));
    if(!buffer) error("memory allocation");

    char* sReservedSeat = (char*) malloc(BUFSIZ * sizeof(char));
    if(!sReservedSeat) error("memory allocation");
    sReservedSeat[0] = '\0';

    for(int i=0; i < details.numberOfSeatReserved; i++) {
        char* serializedSeat = serializeSeat(details.reservedSeat[i]);
        sprintf(sReservedSeat, "%s%s,", sReservedSeat, serializedSeat);
        free(serializedSeat);
    }

    sReservedSeat[strlen(sReservedSeat) - 1] = ';';

    sprintf(buffer, "reservationId:%s;concertId:%d;firstname:%s;lastname:%s;reservedSeats:%d@%s", details.reservationId,details.concertId, details.firstName, details.lastname, details.numberOfSeatReserved, sReservedSeat);

    free(sReservedSeat);

    char* sDetail = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
    strcpy(sDetail, buffer);

    free(buffer);

    return sDetail;
}

/*
 * Opérations sur les places
 */

bool seatEqual(Seat seat1, Seat seat2) {
    if(seat1.seatRow != seat2.seatRow) return FALSE;
    if(seat1.seatNumber != seat2.seatNumber) return FALSE;
    return FALSE;
}
bool isSeatFree(Concert* concert, Seat seat) {
    if(getSeat(&concert->bleacher, seat.seatRow, seat.seatNumber)->reserved == TRUE) return FALSE;
    else return TRUE;
}
bool validSeat(Concert* concert, Seat seat) {
    int rowNumber = concert->bleacher.bleachersRow + ASCII_A_CODE;
    int seatPerRow = concert->bleacher.seatPerRow;

    if((seat.seatRow < (char) ASCII_A_CODE) || ((char) rowNumber < seat.seatRow)) return FALSE;
    if ((seat.seatNumber < 0) || (seatPerRow < seat.seatNumber)) return FALSE;

    return TRUE;
}

Seat *getSeat(Bleacher* bleacher, const char seatRow, const int seatNumber) {
    int rowIdx = (int) seatRow - ASCII_A_CODE;
    return &bleacher->seats[rowIdx][seatNumber-1];
}

bool blockSeat(Concert* concert, const Seat seat) {
    if(isSeatFree(concert, seat) == TRUE) {
        getSeat(&concert->bleacher, seat.seatRow, seat.seatNumber)->reserved = TRUE;
        return TRUE;
    } else {
        return FALSE;
    }
}
void deblockSeat(Concert* concert, const Seat seat) {
    getSeat(&concert->bleacher, seat.seatRow, seat.seatNumber)->reserved = FALSE;
}

int reserveSeat(Concert* concert, const Details personalDetails) {
    for(int i=0; i < personalDetails.numberOfSeatReserved; i++) {
        if(validSeat(concert, personalDetails.reservedSeat[i]) == FALSE) return INVALID_SEAT;
    }

    for(int j=0; j < personalDetails.numberOfSeatReserved; j++) {
        if(isSeatFree(concert, personalDetails.reservedSeat[j]) == FALSE) return ALREADY_RESERVED;
    }

    for(int  k=0; k < personalDetails.numberOfSeatReserved; k++) {
        blockSeat(concert, personalDetails.reservedSeat[k]);
        concert->bleacher.seatTaken++;
    }

    concertSaveChange(*concert);

    return RESERVATION_SUCCESS;
}

int cancelReservation(Concert* concert, const char* fileNumber, char* password) {
    char filePath[64]; sprintf(filePath, "./../../res/reservations/reservation-%s", fileNumber);

    int fileDescriptor = open(filePath, O_RDONLY);
    if(fileDescriptor == -1) return MISSING_FILE;

    char* serializedReservation = getFileContentWithFileDescriptor(fileDescriptor);
    close(fileDescriptor);

    Details* details = unserializeDetails(serializedReservation);
    free(serializedReservation);

    if(strcmp(password, details->lastname)) return WRONG_INFORMATION;

    for(int i=0; i < details->numberOfSeatReserved; i++) {
        if(validSeat(concert, details->reservedSeat[i]) == FALSE) return INVALID_SEAT;
        deblockSeat(concert, details->reservedSeat[i]);
        concert->bleacher.seatTaken--;
    }

    remove(filePath);

    concertSaveChange(*concert);

    return CANCEL_SUCCESS;
}



/*
 * Opérations sur les réservation
 */

bool detailsEqual(Details details1, Details details2) {
    if(strcmp(details1.lastname, details2.lastname) != 0) return FALSE;
    if(strcmp(details1.firstName, details2.firstName) != 0) return FALSE;
    return TRUE;
}

void saveReservation(Details* personalDetails, char fileNumber[11]) {

    strcpy(personalDetails->reservationId, fileNumber);

    char* serializedReservation = serializeDetails(*personalDetails);

    char reservationFile[128]; sprintf(reservationFile, "./../../res/reservations/reservation-%s", personalDetails->reservationId);

    int fileDescriptor = open(reservationFile, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    if(fileDescriptor == -1) error("file creation"); close(fileDescriptor);

    overwriteFile(reservationFile, serializedReservation);
    free(serializedReservation);
}

Details* createDetails(const int concertId, const char* firstname, const char* lastname, char* sReservation) {
    Details* details = (Details*) malloc(sizeof(Details));

    strcpy(details->reservationId,"-1");
    details->concertId = concertId;
    details->firstName = (char*) malloc((strlen(firstname) + 1) * sizeof(char));
    if(!details->firstName) error("memory allocation");
    strcpy(details->firstName, firstname);
    details->lastname = (char*) malloc((strlen(lastname) + 1) * sizeof(char));
    if(!details->lastname) error("memory allocation");
    strcpy(details->lastname, lastname);

    char *serializedSeats = sReservation;

    char *seatTakenStr = strtok_r(serializedSeats, "@", &serializedSeats);
    details->numberOfSeatReserved = atoi(seatTakenStr);

    details->reservedSeat = (Seat*) malloc(details->numberOfSeatReserved * sizeof(Seat));
    if(!details->reservedSeat) error("memory allocation");

    char *listOfSeats = strtok_r(serializedSeats, "@", &serializedSeats);
    for (int i = 0; i < details->numberOfSeatReserved ; i++) {
        char *seatCode = strtok_r(listOfSeats, ",", &listOfSeats);
        Seat *seat = unserializeSeat(seatCode);
        details->reservedSeat[i] = *seat;
            free(seat);
    }

    return details;
}

/*
 * Autres fonctions
 */

Concert* loadConcertsList(char* folderPath, int* numberOfConcert) {
    DIR* folder = opendir(folderPath);
    if(folder == NULL) error("unknown folder");

    struct dirent *entry;
    int fileCount=0;
    while ((entry = readdir(folder)) != NULL) {
        if (entry->d_type == DT_REG)
            fileCount++;
    }

    rewinddir(folder);

    Concert* listConcert = (Concert*) malloc(fileCount * sizeof(Concert));
    int i = 0;
    while ((entry = readdir(folder)) != NULL) {
        if (entry->d_type == DT_REG) {
            char filePath[128];
            sprintf(filePath, "%s/%s", folderPath, entry->d_name);

            char* fileContent = getFileContent(filePath);
            Concert* concert = unserializeConcert(fileContent);
            free(fileContent);

            listConcert[i] = *concert;
            listConcert[i].fileName = (char*) malloc((strlen(filePath) + 1) * sizeof(char));
            if(!listConcert[i].fileName) error("memory allocation");
            strcpy(listConcert[i].fileName, filePath);

            strcpy(filePath, "");
            i++;
        }
    }

    closedir(folder);
    *numberOfConcert = fileCount;
    return listConcert;
}

void destroyConcertInstance(Concert* concert) {
    free(concert->artist);
    free(concert->concertName);
    free(concert->additionalInfos);
    free(concert->fileName);
    free(concert);
}
void concertSaveChange(Concert concert) {
    char* sConcert = serializeConcert(concert);
    overwriteFile(concert.fileName, sConcert);
    free(sConcert);
}

void displayConcert(Concert concert) {
    printf(COLOR_WHITE);
    printf("%s avec %s, le %d/%d/%d\n", concert.concertName, concert.artist, concert.concertDate.day, concert.concertDate.month, concert.concertDate.year);
    printf("informations supplementaire : %s\n", concert.additionalInfos);
    printf("\nPlace disponible : %d/%d\n", concert.bleacher.bleachersRow * concert.bleacher.seatPerRow - concert.bleacher.seatTaken, concert.bleacher.bleachersRow * concert.bleacher.seatPerRow);
    printf("Liste des places :\n");
    for(int i=0; i < concert.bleacher.bleachersRow; i++) {
        printf("Rangée %c ", (char) i + ASCII_A_CODE);
        for(int j=0; j < concert.bleacher.seatPerRow; j++) {
            if(concert.bleacher.seats[i][j].reserved == TRUE) printf(COLOR_RED);
            else printf(COLOR_GREEN);
            printf("[%2d] ", concert.bleacher.seats[i][j].seatNumber);
        }
        printf("\n");
        printf(COLOR_WHITE);
    }
}

ConcertInformation* concertToInfos(const Concert concert) {
    char* buffer = (char*) malloc(1024 * sizeof(char));
    buffer[0] = '\0';
    ConcertInformation* concertInfo = (ConcertInformation*) malloc(sizeof(ConcertInformation));
    concertInfo->concertId = concert.concertId;
    sprintf(buffer, "%s avec %s - %d/%d/%d", concert.concertName, concert.artist, concert.concertDate.day, concert.concertDate.month, concert.concertDate.year);

    concertInfo->libelleConcert = (char*) malloc((strlen(buffer) + 1 ) * sizeof(char));
    strcpy(concertInfo->libelleConcert, buffer);
    free(buffer);
    return concertInfo;
}

Concert* concertGetById(Concert* concertArray, const int id, const int numberOfConcert) {
    for(int i=0; i < numberOfConcert; i++) {
        if(concertArray[i].concertId == id) return &concertArray[i];
    }
    return NULL;
}

int idxOfConcertInArray(const Concert* listeConcert, const int concertId, const int numberOfConcert) {
    for(int i=0; i < numberOfConcert; i++) {
        if(listeConcert[i].concertId == concertId) return i;
    }
    return -1;
}