#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>

void enableRawMode();
void disableRawMode();

int main() {
    enableRawMode();

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d (%c)\r\n", c, c);
        }
    }

    return 0;
}



struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode); // function that runs disavleRawMode() as the program closes, either a main() return or a exit()

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(ICRNL | IXON); // inputF (CRNL => Ctrl+M (CarriageReturnNewLine)) (XON => Ctrl+S & Ctrl+Q (xOFF xON))
    raw.c_oflag &= ~(OPOST); // outputF (POST => \r\n)
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // localF (CANON => enable canonical mode) (EXTEN => Ctrl+V & Ctrl+O) (SIG => Ctrl+C & Ctrl+Z)

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
