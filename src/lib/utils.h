//
// Created by Remi on 04/01/2021.
//
#ifndef PROJET_SYSTEME_A2_MARSAL_RAHUEL_UTILS_H
#define PROJET_SYSTEME_A2_MARSAL_RAHUEL_UTILS_H

#define ASCII_A_CODE 65
#define ASCII_Z_CODE 90
#define ASCII_0 48
#define ASCII_9 57

#define COLOR_WHITE "\033[0;37m"
#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"

#define TRUE 1
#define FALSE 0

typedef int bool;

void error(char* message);

char* getFileContent(char* filePath);
char* getFileContentWithFileDescriptor(int fileDescriptor);
void overwriteFile(const char* filePath, const char* content);
bool fileExist(char* filePath);

//Server side only
typedef struct Properties {
    char lastFileNumber[11];
    int lastConcertIdGiven;
} Properties;

typedef struct Date {
    int day;
    int month;
    int year;
} Date;

bool verifyDate(Date date);
char* dateToStr(Date date);
Date* strToDate(char sDate[20]);

bool is_numeric(char* string);

bool strStartWidth(const char *pre, const char *str);

Properties* loadProperties(char* serializedProperties);
void saveProperties(Properties properties, char* filePath);

void incrementNumericalStr(char* str);
#endif /*PROJET_SYSTEME_A2_MARSAL_RAHUEL_UTILS_H*/
