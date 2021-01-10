//
// Created by Remi on 04/01/2021.
//
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

#include<string.h>
#include<ctype.h>

#include"utils.h"

void error(char* message) {
    printf("error : %s\n", message);
    exit(EXIT_FAILURE);
}

char* getFileContent(char* filePath) {
    int fileDescriptor = open(filePath, O_RDONLY);
    if(fileDescriptor == -1) error("unknown file");

    char buffer[BUFSIZ];

    int readChars;
    if((readChars = read(fileDescriptor, buffer, BUFSIZ)) == -1) error("read file");

    close(fileDescriptor);

    char* fileContent = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
    strcpy(fileContent, buffer);

    return fileContent;
}
char* getFileContentWithFileDescriptor(int fileDescriptor) {
    char buffer[BUFSIZ];

    int readChars;
    if((readChars = read(fileDescriptor, buffer, BUFSIZ)) == -1) error("read file");

    char* fileContent = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
    strcpy(fileContent, buffer);

    return fileContent;
}
void overwriteFile(const char* filePath, const char* content) {
    int fileDescriptor = open(filePath, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    if(fileDescriptor == -1) error("file creation");

    int writeChar;
    if ((writeChar = write(fileDescriptor, content, strlen(content))) != strlen(content)) {
            close(fileDescriptor);
            error("file write");
    }
    close(fileDescriptor);
}

bool fileExist(char* filePath) {
    if(access(filePath, F_OK) == 0 ) return TRUE;
    else return FALSE;
}

bool verifyDate(Date date) {
    if((date.month < 1) || (12 < date.month)) return FALSE;
    int numberOfDayInMonth;
    switch(date.month) {
    case 1 : case 3 : case 5 : case 7 : case 8 : case 10 : case 12 :
            numberOfDayInMonth = 31; break;
    case 4 : case 6 : case 9 : case 11 :
            numberOfDayInMonth = 30; break;
    case 2 :
            if(date.year%4 == 0)
                if(date.year%100 != 0) numberOfDayInMonth = 29;
                else if(date.year%400 == 0)  numberOfDayInMonth = 29;
                    else numberOfDayInMonth = 28;
            else numberOfDayInMonth = 28;
    }
    if((0 < date.day) || (numberOfDayInMonth < date.day)) return FALSE;
    return TRUE;
}
char* dateToStr(Date date) {
    char* sDate = (char*) malloc(10 * sizeof(char));
    sprintf(sDate, "%d/%d/%d", date.day, date.month, date.year);
    return sDate;
}
Date* strToDate(char sDate[20]) {
    Date* date = (Date*) malloc(sizeof(Date));
    if(!date) error("memory allocation");

    char* token;
    char* rest = sDate;

    token = strtok_r(rest, "/", &rest);
    date->day = atoi(token);

    token = strtok_r(rest, "/", &rest);
    date->month = atoi(token);

    token = strtok_r(rest, "/", &rest);
    date->year = atoi(token);

    return date;
}

void incrementNumericalStr(char* str) {
    int idx = strlen(str) -1;
    while(idx >= 0) {
        if((unsigned char) str[idx] == ASCII_9) {
            str[idx] = '0';
            idx--;
        } else {
            str[idx] += 1;
            break;
        }
    }
}


bool is_numeric(char* string) {
    for(int i=0; i < strlen(string); i++) {
        if(!isdigit(string[i])) return FALSE;
    }
    return TRUE;
}

bool strStartWidth(const char *a, const char *b) {
    if(strncmp(a, b, strlen(b)) == 0) return 1;
    return 0;
}

Properties* loadProperties(char* serializedProperties) {
    Properties *properties = (Properties *) malloc(sizeof(Properties));
    if (!properties) error("memory allocation");

    char *token;
    char *rest = serializedProperties;

    while ((token = strtok_r(rest, ";", &rest))) {
        char *attributeRest = token;

        char *attributeName = strtok_r(attributeRest, ":", &attributeRest);
        char *attributeValue = strtok_r(attributeRest, ":", &attributeRest);

        if (strcmp("lastFileNumber", attributeName) == 0) {
            strcpy(properties->lastFileNumber, attributeValue);
            properties->lastFileNumber[10] = '\0';
        }
        else if (strcmp("lastConcertIdGiven", attributeName) == 0)
            properties->lastConcertIdGiven = atoi(attributeValue);
    }

    return properties;
}

void saveProperties(Properties properties, char* filePath) {
    char buffer[128];
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "lastFileNumber:%s;lastConcertIdGiven:%d;", properties.lastFileNumber, properties.lastConcertIdGiven);
    overwriteFile(filePath, buffer);
}

