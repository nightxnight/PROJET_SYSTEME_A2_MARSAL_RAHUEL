//
// Created by Remi on 04/01/2021.
//
#ifndef PROJET_SYSTEME_A2_MARSAL_RAHUEL_CONCERT_H
#define PROJET_SYSTEME_A2_MARSAL_RAHUEL_CONCERT_H

#include"utils.h"

typedef struct Seat {
    char seatRow;
    int seatNumber;
} Seat;

typedef struct Bleacher {
    int seatTaken;
    Seat* seats;
} Bleacher;

typedef struct Concert {
    char* fileName; int concertId;
    char* artist; char* additionalInfos; Date concertDate;
    int bleachersRow; int seatPerRow;
    Bleacher reservedSeats; Bleacher blockedSeats;
} Concert;

typedef struct Details {
    int concertId;
    char* lastname;
    char* firstName;
    Bleacher reservation;
} Details;

char** loadConcertsList(char* folderPath, int* numberOfFile);

Concert* unserializeConcert(char* serializedStr);
char* serializeConcert(const Concert concert);

Bleacher* unserializeBleacher(char* serializedStr);
char* serializeBleacher(const Bleacher bleacher);

Seat* unserializeSeat(char* serializedStr);
char* serializedSeat(const Seat seat);

void destroyConcertInstance(Concert* concert);

bool seatEqual(Seat seat1, Seat seat2);
bool isSeatFree(Concert Concert, Seat seat);

void blockSeat(Bleacher* bleacher, const Seat seat);
bool deblockSeat(Bleacher* bleacher, const Seat seat);

bool reserveSeat(Concert* concert, const Seat seat, const Details personalDetails);
bool cancelBooking(Concert* concert, const char* fileNumber, const Details personalDetails);

void displayConcert(Concert concert);

Details* unserializeDetails(char* serializedStr);
char* serializeDetails(const Details details);

#endif //PROJET_SYSTEME_A2_MARSAL_RAHUEL_CONCERT_H
