/*
    Program test_single_key.c

    Get a key input without hitting Enter key
    using termios

    Global old_termios and new_termios for efficient key inputs
*/
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>  // read()

#define True 1
#define False 0

int key_hit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

// Global termios structs
static struct termios old_tio;
static struct termios new_tio;

// Initialize new terminal i/o settings
void init_termios(int echo) {
    tcgetattr(0, &old_tio);                  // Grab old_tio terminal i/o setting
    new_tio = old_tio;                       // Copy old_tio to new_tio
    new_tio.c_lflag &= ~ICANON;              // disable buffered i/o
    new_tio.c_lflag &= echo ? ECHO : ~ECHO;  // Set echo mode
    if (tcsetattr(0, TCSANOW, &new_tio) < 0)
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

int timeSig[4][6] = {{7, 1, 0, 0, 0, 0},
                     {7, 1, 1, 0, 0, 0},
                     {7, 1, 3, 1, 0, 0},
                     {7, 1, 1, 3, 1, 1}};
char *timeStr[4] = {"2/4", "3/4", "4/4", "6/8"};

int main(void) {
    char c;
    int echo;
    int timeSig_idx = 1, tempo = 90, run = False;

    // Init termios: Disable buffered IO with arg "echo"
    echo = 0;  // Disable echo
    init_termios(echo);

    printf("\n=== ALGO_METRONOME_TUI ===\n");
    printf("  Menu for Metronome TUI:\n");
    printf("'z': Time signature 2/4 > 3/4 > 4/4 > 6/8 > 2/4 ...\n");
    printf("'c': Dec tempo\t\t\tDec tempo by 5\n");
    printf("'b': Inc tempo\t\t\tInc tempo by 5\n");
    printf("'m': Start/Stop\t\t\tToggles start and stop\n");
    printf("'q': Quit this program\n");

    // Test loop
    int i = 0;
    printf("Current status: TimeSig %s, Tempo %d, Run %d\n", timeStr[timeSig_idx], tempo, run);
    printf("single key input until 'q' key\n");
    while (1) {
        c = getch();
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
        if (c == 'q') {
            printf("Quit\n");
            break;
        }
        printf("Key %c: TimeSig %s, Tempo %d, Run %d\n", c, timeStr[timeSig_idx], tempo, run);

        fflush(stdout);
    }
    fflush(stdout);

    // Reset termios
    reset_termios();

    return 0;
}