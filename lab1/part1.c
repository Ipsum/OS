/********************************
 * OS Lab 1 Part 1a             *
 *                              *
 * Usage:                       *
 *      ./part1a.exe            *
 *                              *
 * Spawns a child process       *
 ********************************
 * For: Professor Megherbi      *
 * By: David Tyler              *
 * ID: 00952042                 *
 *                              *
 * 9/22/14                      *
 * *****************************/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int pid;
    int child_pid;

    for(;;) {
        sleep(1);
        child_pid = fork();
        if (child_pid > 0) {
            //wait for child
            wait(child_pid);
        }
        else if (child_pid == 0) {
            //we are now in the child code
            printf("Child Running\n\0");
            exit(1);
        }
    }
}
