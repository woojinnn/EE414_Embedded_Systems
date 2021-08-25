#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>  // read()

#include "metro_client.h"

// GLobal termios structs
static struct termios old_tio;
static struct termios new_tio;

// Set to 0 when Ctrl-c is pressed
// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig) {
    printf("\nCtrl-C pressed, cleaning up and exiting...\n");
    exit_KEY();
    exit(0);
}

/* TermiOS setting */
// Initialize new terminal i/o settings
void init_termios(int echo) {
    tcgetattr(0, &old_tio);                   // Grab old_tio terminal i/o setting
    new_tio = old_tio;                        // Copy old_tio to new_tio
    new_tio.c_lflag &= ~ICANON;               // disable buffered i/o
    new_tio.c_lflag &= echo ? ECHO : ~ECHO;   // Set echo mode
    if (tcsetattr(0, TCSANOW, &new_tio) < 0)  // Set new_tio terminal i/o setting
        perror("tcsetattr ~ICANON");
}

// Restore old terminal i/o settings
void reset_termios(void) {
    tcsetattr(0, TCSANOW, &old_tio);
}

void init_KEY() {
    signal(SIGINT, signal_handler);

    // Init termios: Disable buffered IO with arg 'echo'
    int echo = 0;
    init_termios(echo);

    printf("    Algo_Metronome_TUI\n\n");
    printf("  Menu for Metronome TUI:\n");
    printf("'z': Time signature 2/4 > 3/4 > 4/4 > 6/8 > 2/4 ...\n");
    printf("'c': Dec tempo            Dec tempo by 5\n");
    printf("'b': Inc tempo            Inc tempo by 5\n");
    printf("'m': Start/Stop           Toggles start and stop\n");
    printf("'q': Quit this program\n");
}

int key_hit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

// Read one character without Enter key: Blocking
char getch(void) {
    char ch = 0;
    read(0, &ch, 1);
    return ch;
}

void exit_KEY() {
    reset_termios();
}