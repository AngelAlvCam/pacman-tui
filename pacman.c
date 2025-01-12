#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define BOARD_WIDTH 28
#define BOARD_HEIGHT 31
#define GHOSTS_NUMBER 4
#define TOTAL_DOTS 252
#define GHOST_SPAWN_X 11
#define GHOST_SPAWN_Y 15

typedef struct character
{
    // Position
    int x[2];
    int y[2];
    int direction;
    int color;
    int status; // -1 cage, 0 dead, 1 hunting 
} character;

int score = 0; // Global variable to store the score
int dots_counter = 0;
int dizzy = 0; // Global variable that determines the status of a ghost

/*
1 = wall
0 = point
2 = empty space
*/
int board[BOARD_HEIGHT][BOARD_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
        {1,-2,1,2,2,1,0,1,2,2,2,1,0,1,1,0,1,2,2,2,1,0,1,2,2,1,-2,1},
        {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
        {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1},
        {2,2,2,2,2,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,2,2,2,2,2},
        {2,2,2,2,2,1,0,1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,1,0,1,2,2,2,2,2},
        {2,2,2,2,2,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,2,2,2,2,2},
        {1,1,1,1,1,1,0,1,1,-1,1,-1,-1,-1,-1,-1,-1,1,-1,1,1,0,1,1,1,1,1,1},
  {-1,-1,-1,-1,-1,-1,0,0,0,-1,1,-1,-1,-1,-1,-1,-1,1,-1,0,0,0,-1,-1,-1,-1,-1,-1},
        {1,1,1,1,1,1,0,1,1,-1,1,-1,-1,-1,-1,-1,-1,1,-1,1,1,0,1,1,1,1,1,1},
        {2,2,2,2,2,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,2,2,2,2,2},
        {2,2,2,2,2,1,0,1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,1,0,1,2,2,2,2,2},
        {2,2,2,2,2,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,2,2,2,2,2},
        {1,1,1,1,1,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
        {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
        {1,-2,0,0,1,1,0,0,0,0,0,0,0,-1,-1,0,0,0,0,0,0,0,1,1,0,0,-2,1},
        {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
        {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
        {1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
        {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void draw_board();
int move_character(character*);
void move_ghost(character*, int);
void move_pacman(character*, int);
int collision(character*, character*);
void draw_ghosts(character*);
int check_collisions(character*, character*);

int main()
{
    initscr();
    keypad(stdscr, TRUE); // Enables direction keys
    timeout(0);
    curs_set(0);

    // Enables color    
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK); // Pacman color
    init_pair(3, COLOR_RED, COLOR_BLACK); // Red ghost color
    init_pair(4, COLOR_CYAN, COLOR_BLACK); // 
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_GREEN, COLOR_BLACK);

    draw_board();

    // Create chacter pacman with default position and direction
    character pacman = {{13,13}, {23,23}, 1, 2, 0}; // 1 == right

    // Create ghost character, default direction is up (3) to get out of the cage when the game starts
    character ghosts[GHOSTS_NUMBER] = {
        {{13, 13}, {11, 11}, 0, 3, 1}, 
        {{GHOST_SPAWN_X + 1, GHOST_SPAWN_X + 1}, {GHOST_SPAWN_Y, GHOST_SPAWN_Y}, 3, 4, 0}, 
        {{GHOST_SPAWN_X + 2, GHOST_SPAWN_X + 2}, {GHOST_SPAWN_Y, GHOST_SPAWN_Y}, 3, 5, 0}, 
        {{GHOST_SPAWN_X + 3, GHOST_SPAWN_X + 3}, {GHOST_SPAWN_Y, GHOST_SPAWN_Y}, 3, 6, 0}, 
    };

    // Draw chracter
    while (true)
    {
        int ch = getch();
        if (ch == 'q')
        {
            break;
        }

        switch (ch)
        {
        case KEY_LEFT:
            pacman.direction = 0; // left
            break;
            
        case KEY_RIGHT:
            pacman.direction = 1; // right
            break;

        case KEY_DOWN:
            pacman.direction = 2; // down
            break;
        
        case KEY_UP:
            pacman.direction = 3; // up
            break;
        }

        // Draw pacman and update position
        move_pacman(&pacman, pacman.color);
        mvprintw(0, BOARD_WIDTH + 1, "pacman: x_old = %2d, y_old = %2d, direction = %d", pacman.x[0], pacman.y[0], pacman.direction);
        mvprintw(1, BOARD_WIDTH + 1, "        x_new = %2d, y_new = %2d", pacman.x[1], pacman.y[1]);
        mvprintw(5, BOARD_WIDTH + 1, "score: %d", score);

        // Draw ghosts
        draw_ghosts(ghosts);

        if (dots_counter == TOTAL_DOTS)
        {
            mvaddstr(4, BOARD_WIDTH + 1, "you win!");
            break;
        }

        // 
        if (check_collisions(&pacman, ghosts))
        {
            mvaddstr(4, BOARD_WIDTH + 1, "COLLISION");
            break;
        }

        refresh();

        usleep(150000);
        
        // If dizzy flag has been enabled, count until 30
        if (dizzy)
        {
            dizzy++;
            if (dizzy == 30)
            {
                dizzy = 0;
            }
        }
    }

    timeout(-1);
    getch();
    endwin();

    return 0;
}

/*
    Function to check if there has been a collision between pacman
    and a ghost
*/
int check_collisions(character* pacman, character* ghosts)
{
    int collision_flag = FALSE;

    character* current = ghosts;
    for (int i = 0; i < GHOSTS_NUMBER; i++)
    {
        // Status must be 1 (active)
        if (!dizzy && current->status == 1 && collision(pacman, current))
        {
            collision_flag = TRUE;
        }
        current++;
    }

    return collision_flag;
}

/*
    Function to draw the ghosts and move them through the board
*/
void draw_ghosts(character* ghosts)
{
    character* current = ghosts;
    for (int i = 0; i < GHOSTS_NUMBER; i++)
    {
        if (current->status == 1) // active status
        {
            if (dizzy)
            {
                move_ghost(current, 1);
            }
            else
            {
                move_ghost(current, current->color);
            }
            current++;
        }
    }
}

/*
    Functions that detects a collision between two given characters
*/
int collision(character* char_1, character* char_2)
{
    // This case triggers if one character is not moving and the other overlaps with him
    // or if both characters arrive at the same at the same time
    if (char_1->x[1] == char_2->x[1] && char_1->y[1] == char_2->y[1])
    {
        return TRUE;
    }

    // Check complex cases
    else
    {
        // If they are moving in the same row
        if (char_1->y[1] == char_2->y[1] && char_1->x[0] == char_2->x[1] && char_1->x[1] == char_2->x[0])
        {
            return TRUE;
        }

        // If they are moving in the same colum
        if (char_1->x[1] == char_2->x[1] && char_1->y[0] == char_2->y[1] && char_1->y[1] == char_2->y[0])
        {
            return TRUE;
        }
    }

    return FALSE;

}

/*
    Moves pacman based on its current position and direction members.
    pacman color is the index for the color pallet to print pacman.
*/
void move_pacman(character* pacman, int pacman_color)
{
    // Try to move
    mvaddch(pacman->y[1], pacman->x[1], ' '); // put empty space in current position because there aren't points left in that cell
    move_character(pacman);
    attron(COLOR_PAIR(pacman_color));
    mvaddch(pacman->y[1], pacman->x[1], '@');
    attroff(COLOR_PAIR(pacman_color));
    
    // Add points if the cell value is 0, then flip them to -1
    if (board[pacman->y[1]][pacman->x[1]] == 0)
    {
        // increase one point and mark the cell as 'no points left'
        score += 10;
        dots_counter++;
        board[pacman->y[1]][pacman->x[1]] = -1;
    }
    // if energizer dot... dizzy all the ghosts
    else if (board[pacman->y[1]][pacman->x[1]] == -2)
    {
        dots_counter++;
        board[pacman->y[1]][pacman->x[1]] = -1;
        // Disable all the ghosts
        dizzy = 1;
    }
}

void move_ghost(character* ghost, int ghost_color)
{
    // if pacman has passed in the current cell, draw empty
    // otherwise, draw a bullet point
    if (board[ghost->y[1]][ghost->x[1]] == 0)
    {
        mvaddch(ghost->y[1], ghost->x[1], ACS_BULLET);
    }
    else if (board[ghost->y[1]][ghost->x[1]] == -1)
    {
        mvaddch(ghost->y[1], ghost->x[1], ' ');
    }
    else if (board[ghost->y[1]][ghost->x[1]] == -2)
    {
        mvaddch(ghost->y[1], ghost->x[1], ACS_DIAMOND);
    }
    
    // Ghosts don't override the board, so... before moving, get the items in the current 
        
    /* Try to move in the current direnction of the ghost, if there is a collision
    with a wall, try to move in a different location*/
    if (move_character(ghost) != TRUE)
    {
        // Change direction to a valid one 
        srand(time(0));
        int random = rand() % 4;
        while (random == ghost->direction)
        {
            random = rand() % 4;
        }

        // Update direction and move
        ghost->direction = random; 
        move_character(ghost);
    }

    // Draw ghost
    attron(COLOR_PAIR(ghost_color));
    mvaddch(ghost->y[1], ghost->x[1], '@');
    attroff(COLOR_PAIR(ghost_color));

}

/*
    Move a character across the board by one unit in a specified direction
    direction can be 0, 1, 2, 3 for left, right, down and up.
    Direction is a member of character struct.
    Returns TRUE is there was not a collision with a wall.
    It updates the x or y members of the character pointer if the movement was
    succesfull.
*/
int move_character(character* char_moving)
{
    int move = FALSE;
    
    char_moving->x[0] = char_moving->x[1];
    char_moving->y[0] = char_moving->y[1];

    switch(char_moving->direction)
    {
        case 0: // left
            int left_index = char_moving->x[1] - 1;
            int left_char = left_index < 0 ? 0 : board[char_moving->y[1]][left_index];
            if (left_char <= 0)
            {
                char_moving->x[1]--;
                move = TRUE;
            }
            break;

        case 1: // right 
            int right_index = char_moving->x[1] + 1;
            int right_char = right_index >= BOARD_WIDTH ? 0 : board[char_moving->y[1]][right_index];
            if (right_char <= 0) 
            {
                char_moving->x[1]++;
                move = TRUE;
            }
            break;
        
        case 2: // down
            int down_index = char_moving->y[1] + 1;
            int down_char = down_index >= BOARD_HEIGHT ? 0 : board[down_index][char_moving->x[1]];
            if (down_char <= 0)
            {
                char_moving->y[1]++;
                move = TRUE;
            }
            break;

        case 3: // up
            int up_index = char_moving->y[1] - 1;
            int up_char = up_index < 0 ? 0 : board[up_index][char_moving->x[1]];
            if (up_char <= 0)
            {
                char_moving->y[1]--;
                move = TRUE;
            }
            break;
    }

    // Portals?
    if (char_moving->x[1] < 0)
    {
        char_moving->x[1] = BOARD_WIDTH - 1;
    }
    else if (char_moving->x[1] >= BOARD_WIDTH)
    {
        char_moving->x[1] = 0;
    }

    if (char_moving->y[1] < 0)
    {
        char_moving->y[1] = BOARD_HEIGHT - 1;
    }
    else if (char_moving->y[1] >= BOARD_HEIGHT)
    {
        char_moving->y[1] = 0;
    }

    return move;
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
                if (down_char == 1 && right_char == 1 & board[down_index][right_index] <= 0)
                {
                    mvaddch(row, col, ACS_ULCORNER);
                }
                else if (down_char == 1 && left_char == 1 && board[down_index][left_index] <= 0)
                {
                    mvaddch(row, col, ACS_URCORNER);
                }
                else if (up_char == 1 && right_char == 1 && board[up_index][right_index] <= 0)
                {
                    mvaddch(row, col, ACS_LLCORNER);
                }
                else if (up_char == 1 && left_char == 1 && board[up_index][left_index] <= 0)
                {
                    mvaddch(row, col, ACS_LRCORNER);
                }
                // Alternative corners
                else if (down_char == 1 && right_char == 1 && board[up_index][left_index] <= 0 && left_char <= 0 && up_char <= 0)
                {
                    mvaddch(row, col, ACS_ULCORNER);
                }
                else if (down_char == 1 && left_char == 1 && board[up_index][right_index] <= 0 && right_char <= 0 && up_char <= 0)
                {
                    mvaddch(row, col, ACS_URCORNER);
                }
                else if (up_char == 1 && right_char == 1 && board[down_index][left_index] <= 0 && left_char <= 0 && down_char <= 0)
                {
                    mvaddch(row, col, ACS_LLCORNER);
                }
                else if (up_char == 1 && left_char == 1 && board[down_index][right_index] <= 0 && right_char <= 0 && down_char <= 0)
                {
                    mvaddch(row, col, ACS_LRCORNER);
                }
                
                // Lines...
                else if (up_char == 1 && down_char == 1)
                {
                    mvaddch(row, col, ACS_VLINE);
                }
                else if (left_char == 1 && right_char == 1)
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
            else if (board[row][col] == -2)
            {
                // Special points?
                mvaddch(row, col, ACS_DIAMOND);
            }
        }
    }
    refresh();
}
