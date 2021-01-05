//
// Created by Remi on 04/01/2021.
//
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

#include<string.h>

#include"utils.h"

void error(char* message) {
    printf("error : %s\n", message);
    exit(EXIT_FAILURE);
}

char* getFileContent(char* filePath) {
    int fileDescriptor = open(filePath, O_RDONLY);
    if(fileDescriptor == -1) error("unknown file");

    char* buffer = (char*) malloc(BUFSIZ * sizeof(char));
    if(!buffer) error("memory allocation");

    int readChars;
    if((readChars = read(fileDescriptor, buffer, BUFSIZ)) == -1) error("read file");

    close(fileDescriptor);

    char* fileContent = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
    strcpy(fileContent, buffer);

    free(buffer);
    return fileContent;
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


