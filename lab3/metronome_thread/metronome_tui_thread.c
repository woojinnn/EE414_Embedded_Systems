#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>  // read()

#include "userLEDmmap.h"

int main(void) {
    char *timeStr[4] = {"2/4", "3/4", "4/4", "6/8"};
    char c;

    // initial values
    keepgoing = 1;
    run = False;
    timeSig_idx = 1;
    tempo = 90;

    // Welcome message
    printf("\n=== METRONOME_TUI_THREAD ===\n");
    printf("  Menu for Metronome TUI:\n");
    printf("'z': Time signature 2/4 > 3/4 > 4/4 > 6/8 > 2/4 ...\n");
    printf("'c': Dec tempo\t\t\tDec tempo by 5 (min tempo 30)\n");
    printf("'b': Inc tempo\t\t\tInc tempo by 5 (max tempo 200)\n");
    printf("'m': Start/Stop\t\t\tToggles start and stop\n");
    printf("'q': Quit this program\n");
    init_KEY();
    init_LED();

    while (keepgoing) {
        c = getch();
        timeSig_idx2 = 0;

        pthread_mutex_lock(&mutex_lock);
        switch (c) {
            case 'q':
                break;
            case 'z':  // time-signature: inc Timesig
                timeSig_idx = (timeSig_idx + 1) % 4;
                break;
            case 'c':  // dec tempo
                tempo -= 5;
                if (tempo < 30)
                    tempo = 30;
                break;
            case 'b':  // inc tempo
                tempo += 5;
                if (tempo > 200)
                    tempo = 200;
                break;
            case 'm':  // Start/Stop
                if (run == True)
                    run = False;
                else if (run == False)
                    run = True;
                break;
            default:
                printf("Invalid key: Please press another key!\n");
                break;
        }
        pthread_mutex_unlock(&mutex_lock);
        if (c == 'q') {
            printf("Quit\n");
            break;
        }
        printf("Key %c: TimeSig %s, Tempo %d, Run %d\n", c, timeStr[timeSig_idx], tempo, run);

        fflush(stdout);
    }
    fflush(stdout);

    exit_KEY();
    exit_LED();

    return 0;
}