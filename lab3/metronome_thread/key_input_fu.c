#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include "userLEDmmap.h"

// Set to 0 when Ctrl-c is pressed
// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig) {
    printf("\nCtrl-C pressed, cleaning up and exiting...\n");
    keepgoing = 0;
    exit_LED();
    exit_KEY();
    exit(0);
}

// Global termios structs
static struct termios old_tio;
static struct termios new_tio;

// Initialize new terminal i/o settings
void init_termios(int echo) {
    tcgetattr(STDIN_FILENO, &old_tio);       // Grab old_tio terminal i/o setting
    new_tio = old_tio;                       // Copy old_tio to new_tio
    new_tio.c_lflag &= ~ICANON;              // disable buffered i/o
    new_tio.c_lflag &= echo ? ECHO : ~ECHO;  // Set echo mode
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_tio) < 0)
        perror("tcsetattr ~ICANON");  // Set new_tio terminal i/o setting
}

// Restore old terminal i/o settings
void reset_termios(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
}

// Read one character without Enter key: Blocking
char getch(void) {
    char ch = 0;
    if (read(STDIN_FILENO, &ch, 1) < 0)  // Read one char
        perror("read()");
    return ch;
}

void init_KEY() {
    signal(SIGINT, signal_handler);

    int echo = 0;
    init_termios(echo);
}

void exit_KEY() {
    reset_termios();
}