/***********************************
 * OS Lab 1 Part 3                  *
 *                                  *
 * Usage:                           *
 *      ./part3.exe [num children/2]*
 *                                  *
 * Spawns pair of child processes   *
  **********************************
 * For: Professor Megherbi          *
 * By: David Tyler                  *
 * ID: 00952042                     *
 *                                  *
 * 9/22/14                          *
 * *********************************/

#include <stdio.h>
#include <stdlib.h>

//Macro to find sum of all numbers less than X
#define SSUM(X) (X * (X + 1)) / 2

int main(int argc, char **argv)
{
    int child_pid;
    int pid;
    int n;

    if(argc < 2) {
        printf("Usage: part3.exe [number of children/2]\n\0");
        return 1;
    }

    n = atoi(argv[1]) * 2;

    for(n; n > 0; n--) {
        child_pid = fork();
        pid = getpid();
        if(child_pid > 0) {
            wait(child_pid);
        }
        else if((child_pid == 0) && !(n & 1)) { //Run on odd numbers
            printf("I am the first process with process ID %d\n\0", pid);
            printf("%d\nI am process %d and I am terminating\n\0", SSUM(10), pid);
            exit(1);
        }
        else if((child_pid == 0) && (n & 1)) { //Run on even numbers
            printf("I am the second process with process ID %d\n\0", pid);
            printf("%d\nI am process %d and I am terminating\n\0", SSUM(100), pid);
            exit(1);
        }
        else {
            printf("Too many processes already running");
            exit(1);
        }

    }
    return 0;
}
