//
// Created by Remi on 04/01/2021.
//
#include<stdio.h>
#include<unistd.h>

#include"../lib/connection.c"

int main(const int argc, const char* argv[]) {
    Connection* connection = openConnection("127.0.0.1", 6000);
    printf("connection established!\n");
    close(connection->socket);
}
