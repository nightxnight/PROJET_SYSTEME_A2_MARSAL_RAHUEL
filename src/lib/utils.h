//
// Created by Remi on 04/01/2021.
//
#ifndef PROJET_SYSTEME_A2_MARSAL_RAHUEL_UTILS_H
#define PROJET_SYSTEME_A2_MARSAL_RAHUEL_UTILS_H

#define TRUE 1
#define FALSE 0

typedef int bool;

void error(char* message);

char* getFileContent(char* filePath);



typedef struct Date {
    int day;
    int month;
    int year;
} Date;

bool verifyDate(Date date);
char* dateToStr(Date date);
Date* strToDate(char sDate[20]);

#endif //PROJET_SYSTEME_A2_MARSAL_RAHUEL_UTILS_H
