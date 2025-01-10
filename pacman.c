#include <ncurses.h>
#include <unistd.h>

#define BOARD_WIDTH 28
#define BOARD_HEIGHT 31

typedef struct character
{
    // Position
    int x;
    int y;
    char direction;
} character;

/*
1 = wall
0 = point
2 = empty space
*/
int board[BOARD_HEIGHT][BOARD_WIDTH] = {
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
{1,0,1,2,2,1,0,1,2,2,2,1,0,1,1,0,1,2,2,2,1,0,1,2,2,1,0,1},
{1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
{1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
{1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
{1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1},
{2,2,2,2,2,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,2,2,2,2,2},
{2,2,2,2,2,1,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,1,2,2,2,2,2},
{2,2,2,2,2,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,2,2,2,2,2},
{1,1,1,1,1,1,0,1,1,0,1,2,2,2,2,2,2,1,0,1,1,0,1,1,1,1,1,1},
{0,0,0,0,0,0,0,0,0,0,1,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0},
{1,1,1,1,1,1,0,1,1,0,1,2,2,2,2,2,2,1,0,1,1,0,1,1,1,1,1,1},
{2,2,2,2,2,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,2,2,2,2,2},
{2,2,2,2,2,1,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,1,2,2,2,2,2},
{2,2,2,2,2,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,2,2,2,2,2},
{1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
{1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
{1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1},
{1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
{1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
{1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
{1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
{1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void draw_board();
void move_character(character*);

int main()
{
    initscr();
    keypad(stdscr, TRUE); // Enables direction keys
    timeout(0);
    curs_set(0);

    // Enables color    
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);

    draw_board();

    // Create chacter pacman with default position and direction
    character pacman = {13, 23, 'r'};

    // Draw chracter
    attron(COLOR_PAIR(2));
    mvaddch(pacman.y, pacman.x, '@');
    while (true)
    {
        int ch = getch();
        if (ch == 'q')
        {
            break;
        }
        mvaddch(pacman.y, pacman.x, ' ');

        switch (ch)
        {
        case KEY_LEFT:
            pacman.direction = 'l';
            break;
            
        case KEY_RIGHT:
            pacman.direction = 'r';
            break;

        case KEY_DOWN:
            pacman.direction = 'd';
            break;
        
        case KEY_UP:
            pacman.direction = 'u';
            break;
        }

        move_character(&pacman);
        mvaddch(pacman.y, pacman.x, '@');
        refresh();
        usleep(150000);  // 500,000 microseconds = 0.5 seconds
    }

    attroff(COLOR_PAIR(2));
    endwin();

    return 0;
}

/*
    Move a character across the board by one unit in a specified direction
    direction can be 'l', 'r', 'd', 'u' for left, right, down and up.
*/
void move_character(character* char_moving)
{
    switch(char_moving->direction)
    {
        case 'l':
            int left_index = char_moving->x - 1;
            int left_char = left_index < 0 ? 0 : board[char_moving->y][left_index];
            if (!left_char)
            {
                char_moving->x--;
            }
            break;

        case 'r':
            int right_index = char_moving->x + 1;
            int right_char = right_index >= 28 ? 0 : board[char_moving->y][right_index];
            if (!right_char) 
            {
                char_moving->x++;
            }
            break;
        
        case 'd':
            int down_index = char_moving->y + 1;
            int down_char = down_index >= 31 ? 0 : board[down_index][char_moving->x];
            if (!down_char)
            {
                char_moving->y++;
            }
            break;

        case 'u':
            int up_index = char_moving->y - 1;
            int up_char = up_index < 0 ? 0 : board[up_index][char_moving->x];
            if (!up_char)
            {
                char_moving->y--;
            }
            break;
    }
}

/*
Function to initialize the pacman game board.
*/
void draw_board()
{
    for (int row = 0; row < BOARD_HEIGHT; row++)
    {
        for (int col = 0; col < BOARD_WIDTH; col++)
        {
            if (board[row][col] == 1)
            {
                // Get neighbor indexes
                int up_index = row - 1;
                int down_index = row + 1;
                int right_index = col + 1;
                int left_index = col - 1;

                // Get values in neighbor indexes... if out of range, it's considered empty (0)
                int up_char = up_index < 0 ? 0 : board[up_index][col];  // use 'col' for up and down
                int down_char = down_index >= 31 ? 0 : board[down_index][col];  // use 'col' for up and down
                int right_char = right_index >= 28 ? 0 : board[row][right_index];  // use 'row' for right and left
                int left_char = left_index < 0 ? 0 : board[row][left_index];  // use 'row' for right and left 

                attron(COLOR_PAIR(1));
                // First check for corner cases:
                if (down_char && right_char && board[down_index][right_index] == 0)
                {
                    mvaddch(row, col, ACS_ULCORNER);
                }
                else if (down_char && left_char && board[down_index][left_index] == 0)
                {
                    mvaddch(row, col, ACS_URCORNER);
                }
                else if (up_char && right_char && board[up_index][right_index] == 0)
                {
                    mvaddch(row, col, ACS_LLCORNER);
                }
                else if (up_char && left_char && board[up_index][left_index] == 0)
                {
                    mvaddch(row, col, ACS_LRCORNER);
                }
                // Alternative corners
                else if (down_char && right_char && board[up_index][left_index] == 0 && left_char == 0 && up_char == 0)
                {
                    mvaddch(row, col, ACS_ULCORNER);
                }
                else if (down_char && left_char && board[up_index][right_index] == 0 && right_char == 0 && up_char == 0)
                {
                    mvaddch(row, col, ACS_URCORNER);
                }
                else if (up_char && right_char && board[down_index][left_index] == 0 && left_char == 0 && down_char == 0)
                {
                    mvaddch(row, col, ACS_LLCORNER);
                }
                else if (up_char && left_char && board[down_index][right_index] == 0 && right_char == 0 && down_char == 0)
                {
                    mvaddch(row, col, ACS_LRCORNER);
                }
                
                // Lines...
                else if (up_char && down_char)
                {
                    mvaddch(row, col, ACS_VLINE);
                }
                else if (left_char && right_char)
                {
                    mvaddch(row, col, ACS_HLINE);
                }
                attroff(COLOR_PAIR(1));
            }
            else if (board[row][col] == 0)
            {
                // Draw points
                mvaddch(row, col, ACS_BULLET);
            }
        }
    }
    refresh();
}
