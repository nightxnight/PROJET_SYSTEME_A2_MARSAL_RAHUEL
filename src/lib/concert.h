//
// Created by Remi on 04/01/2021.
//
#ifndef PROJET_SYSTEME_A2_MARSAL_RAHUEL_CONCERT_H
#define PROJET_SYSTEME_A2_MARSAL_RAHUEL_CONCERT_H

#include"utils.h"

#define MISSING_FILE -4
#define WRONG_INFORMATION -3
#define INVALID_SEAT -2
#define ALREADY_RESERVED -1

#define RESERVATION_SUCCESS 1
#define CANCEL_SUCCESS 1

typedef struct Seat {
    char seatRow;
    int seatNumber;
    bool reserved;
} Seat;

typedef struct Bleacher {
    int bleachersRow;
    int seatPerRow;
    int seatTaken;
    Seat** seats;
} Bleacher;

typedef struct Concert {
    char* fileName;
    int concertId; char* concertName;
    char* artist;
    char* additionalInfos;
    Date concertDate;
    Bleacher bleacher;
} Concert;

typedef struct ConcertInformation{
    int concertId;
    char* libelleConcert;
} ConcertInformation;

typedef struct Details {
    char reservationId[11];
    int concertId;
    char* lastname;
    char* firstName;
    int numberOfSeatReserved;
    Seat* reservedSeat;
} Details;

/*
 * Fonction de serialization
 */
Concert* unserializeConcert(char* serializedStr);
char* serializeConcert(const Concert concert);

Bleacher* unserializeBleacher(char* serializedStr);
char* serializeBleacher(const Bleacher bleacher);

Seat* unserializeSeat(char* serializedStr);
char* serializeSeat(const Seat seat);

ConcertInformation* unserializeConcertInformation(char* serializedStr);
char* serializeConcertInformation(ConcertInformation concertInfos);

Details* unserializeDetails(char* serializedStr);
char* serializeDetails(const Details details);

/*
 * Opération sur les places de concert
 */
bool seatEqual(Seat seat1, Seat seat2);
bool isSeatFree(Concert* concert, Seat seat);
bool validSeat(Concert* concert, Seat seat);

Seat *getSeat(Bleacher* bleacher, const char seatRow, const int seatNumber);

bool blockSeat(Concert* concert, const Seat seat);
void deblockSeat(Concert* concert, const Seat seat);

bool reserveSeat(Concert* concert, const Details personalDetails);
int cancelReservation(Concert* concert, const char* fileNumber, char* password);

/*
 * Opération sur les réservations
 */

Details* createDetails(const int concertId, const char* firstname, const char* lastname, char* sReservation);
bool detailsEqual(Details details1, Details details2);
void saveReservation(Details* personalDetails, char fileNumber[11]);

/*
 * Autres fonction
 */
Concert* loadConcertsList(char* folderPath, int* numberOfConcert);
void destroyConcertInstance(Concert* concert);
void concertSaveChange(Concert concert);
void displayConcert(Concert concert);
ConcertInformation* concertToInfos(const Concert concert);
Concert* concertGetById(Concert* concertArray, const int id, const int numberOfConcert);
int idxOfConcertInArray(const Concert* listeConcert, const int concertId, const int numberOfConcert);




#endif /*PROJET_SYSTEME_A2_MARSAL_RAHUEL_CONCERT_H*/
