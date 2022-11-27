#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <string>

int getch()
{
    int ch;
    // https://man7.org/linux/man-pages/man3/termios.3.html
    struct termios t_old, t_new;

    // https://man7.org/linux/man-pages/man3/termios.3.html
    // tcgetattr() gets the parameters associated with the object referred
    //   by fd and stores them in the termios structure referenced by
    //   termios_p
    tcgetattr(STDIN_FILENO, &t_old);
    
    // copy old to new to have a base for setting c_lflags
    t_new = t_old;

    // https://man7.org/linux/man-pages/man3/termios.3.html
    //
    // ICANON Enable canonical mode (described below).
    //   * Input is made available line by line (max 4096 chars).
    //   * In noncanonical mode input is available immediately.
    //
    // ECHO   Echo input characters.
    t_new.c_lflag &= ~(ICANON | ECHO);
    
    // sets the attributes
    // TCSANOW: the change occurs immediately.
    tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

    ch = getchar();

    // reset stored attributes
    tcsetattr(STDIN_FILENO, TCSANOW, &t_old);

    return ch;
}

const char *getPass()
{
    int show_asterisk = 0;

    const char BACKSPACE = 127;
    const char RETURN = 10;

    unsigned char ch = 0;
    std::string password;

    printf("Password: ");

    while ((ch = getch()) != RETURN)
    {
        if (ch == BACKSPACE)
        {
            if (password.length() != 0)
            {
                if (show_asterisk)
                {
                    printf("\b \b"); // backslash: \b
                }
                password.resize(password.length() - 1);
            }
        }
        else
        {
            password += ch;
            if (show_asterisk)
            {
                printf("*");
            }
        }
    }
    printf("\n");
    return password.c_str();
}
