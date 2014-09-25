/****************************************
 * OS Lab 1 Part 2                      *
 *                                      *
 * Usage:                               *
 *      ./part2.exe n                   *
 *                                      *
 * Spawns n*2 child threads             *
 ****************************************
 * For: Professor Megherbi              *
 * By: David Tyler                      *
 * ID: 00952042                         *
 *                                      *
 * 9/22/14                              *
 * **************************************/

#include <stdio.h>

int main(int argc, char **argv)
{
    int n;
    int child_pid;
    int pid;
    
    if(argc < 2) { //Check for correct number of arguments
        printf("Usage: part2.exe [number of childen/2]\n\0");
        return 1;
    }

    n = atoi(argv[1]) * 2; //get input int and multiply by 2
 
    //Loop until we have created requirned number of processes
    for(n; n > 0; n--) {        
        child_pid = fork();
        if(child_pid > 0) {
            wait(child_pid); //wait for child to exit
        }
        else if(child_pid == 0) {
            pid = getpid();
            printf("I am process %d and I am terminating\n\0",pid);
            exit(1);
        }
    }
    return 0;
}
