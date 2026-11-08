#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

void enableRawMode();
void disableRawMode();

int main() {
    enableRawMode();

    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d (%c)\r\n", c, c);
        }

        if (c == 'q') break;
    }

    return 0;
}



struct termios orig_termios;

void die(const char *s) {
    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tcsetattr");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode); // function that runs disavleRawMode() as the program closes, either a main() return or a exit()

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); // inputF (BRKINT & NPCK & STRIP => old) (CRNL => Ctrl+M (CarriageReturnNewLine)) (XON => Ctrl+S & Ctrl+Q (xOFF xON))
    raw.c_oflag &= ~(OPOST); // outputF (POST => \r\n)
    raw.c_cflag |= ~(CS8); // sets every byte to 8 bits 
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // localF (CANON => enable canonical mode) (EXTEN => Ctrl+V & Ctrl+O) (SIG => Ctrl+C & Ctrl+Z)

    // ControlCaracters
    raw.c_cc[VMIN] = 0; // min numb of bytes for read
    raw.c_cc[VTIME] = 1; // max time for read

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}
