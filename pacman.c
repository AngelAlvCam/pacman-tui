#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define BOARD_WIDTH 28
#define BOARD_HEIGHT 31
#define GHOSTS_NUMBER 4
#define TOTAL_DOTS 252
#define GHOST_SPAWN_X 12
#define GHOST_SPAWN_Y 15
#define DIZZY_LIMIT 60

typedef struct character
{
    // Position
    int x[2];
    int y[2];
    int direction;
    int color;
    int status; // 1 active, 0 in cage  
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

int busy_board[BOARD_HEIGHT][BOARD_WIDTH] = {0};

void draw_board();
int move_character(character*, int);
void move_ghosts(character*);
void move_pacman(character*);
int collision(character*, character*);
int check_collisions(character*, character*);
void draw_characters(character*);
void initialize(character*);

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
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_GREEN, COLOR_BLACK);

    draw_board();

    // Create ghost character, default direction is up (3) to get out of the cage when the game starts
    character* characters = calloc(5, sizeof(character));
    initialize(characters);
    
    // Initialize busy board
    for (int i = 0; i < GHOSTS_NUMBER + 1; i++)
    {
        busy_board[(characters + i)->y[1]][(characters + i)->x[1]] = 1;
    }

    // Draw chracter
    while (true)
    {
        // Draw score
        mvprintw(0, BOARD_WIDTH + 1, "score: %5d", score);

        // Info about ghost1

        int ch = getch();
        if (ch == 'q')
        {
            break;
        }

        switch (ch)
        {
        case KEY_LEFT:
            characters->direction = 0; // left
            break;
            
        case KEY_RIGHT:
            characters->direction = 1; // right
            break;

        case KEY_DOWN:
            characters->direction = 2; // down
            break;
        
        case KEY_UP:
            characters->direction = 3; // up
            break;
        }

        // Draw pacman and update position
        move_pacman(characters);

        // Move ghosts
        move_ghosts(characters + 1);

        // Check if the game should stop because of a collision between pacman and a ghost
        if (check_collisions(characters, characters + 1))
        {
            draw_characters(characters);
            mvaddstr(4, BOARD_WIDTH + 1, "COLLISION");
            break;
        }

        // draw characters
        draw_characters(characters);

        // Check if the game should stop because the maximum amount of points has been reached
        if (dots_counter == TOTAL_DOTS)
        {
            mvaddstr(4, BOARD_WIDTH + 1, "you win!");
            break;
        }
        
        // If dizzy flag has been enabled, count until 30
        if (dizzy)
        {
            dizzy++;
            if (dizzy == 60)
            {
                dizzy = 0;
            }
        }
        
        // Print busy board
        // for (int i = 0; i < BOARD_HEIGHT; i++)
        // {
        //     for (int j = 0; j < BOARD_WIDTH; j++)
        //     {
        //         mvprintw(i, j + BOARD_WIDTH + 1, "%d", busy_board[i][j]);
        //     }
        // }
        refresh();

        usleep(150000);

    }

    timeout(-1);
    getch();
    endwin();
    free(characters);

    return 0;
}

void initialize(character* characters)
{
    // Set pac-man attributes
    characters->x[0] = 13;
    characters->x[1] = 13;
    characters->y[0] = 23;
    characters->y[1] = 23;
    characters->direction = 1;
    characters->color = 2;
    characters->status = 0;

    // Set ghosts attributes
    for (int i = 0; i < GHOSTS_NUMBER; i++)
    {
        characters++;
        characters->x[0] = GHOST_SPAWN_X + i;
        characters->x[1] = GHOST_SPAWN_X + i;
        characters->y[0] = GHOST_SPAWN_Y;
        characters->y[1] = GHOST_SPAWN_Y;
        characters->direction = 3;
        characters->color = 3 + i;
        characters->status = 0;
    }
}

/*
    Function to check if there has been a collision between pacman
    and a ghost
*/
int check_collisions(character* pacman, character* ghosts)
{
    character* current = ghosts;
    int flag = FALSE;
    for (int i = 0; i < GHOSTS_NUMBER; i++)
    {
        if (collision(pacman, current))
        {
            if (dizzy)
            {
                // increase score by 200
                score += 200;

                // Change coordinates of ghost
                busy_board[current->y[1]][current->x[1]]--;

                current->y[1] = GHOST_SPAWN_Y; 
                current->x[1] = GHOST_SPAWN_X + i;

                busy_board[current->y[1]][current->x[1]]++;

                // update ghost status
                current->status = 0;
                current->direction = 3;
            }
            else
            {
                // If at least one collision is dettected... return TRUE to break the game
                flag = TRUE;
            }
        }
        current++;
    }

    return flag;
}

/*
    Functions that prints all the characters on the board. (pacman and ghosts)
*/
void draw_characters(character* characters)
{
    character* current = characters;
    
    // First... print pacman
    // delete from the old coordinates...
    if (!busy_board[current->y[0]][current->x[0]]) // Old coordinates aren't busy, then clear
    {
        mvaddch(current->y[0], current->x[0], ' ');
    }

    // Print pacman in its new coordinates
    attron(COLOR_PAIR(current->color));
    mvaddch(current->y[1], current->x[1], '@');
    attroff(COLOR_PAIR(current->color));

    // Print the rest of the characters (ghosts)    
    current++;
    for (int i = 1; i < GHOSTS_NUMBER + 1; i++)
    {
        // Check if it is necessary to print something in the old coordinates
        if (!busy_board[current->y[0]][current->x[0]]) // Old coordinates aren't busy, then clear
        {
            if (board[current->y[0]][current->x[0]] == 0)
            {
                mvaddch(current->y[0], current->x[0], ACS_BULLET);
            }
            else if (board[current->y[0]][current->x[0]] == -1)
            {
                mvaddch(current->y[0], current->x[0], ' ');
            }
            else if (board[current->y[0]][current->x[0]] == -2)
            {
                mvaddch(current->y[0], current->x[0], ACS_DIAMOND);
            }
            
        }

        // Draw ghost in its new coords
        int color;
        if (dizzy)
        {
            // Color blue
            color = 1;
        }
        else
        {
            // Real color
            color = current->color;
        }
        attron(COLOR_PAIR(color));
        mvaddch(current->y[1], current->x[1], '@');
        attroff(COLOR_PAIR(color));

        // Next ghost
        current++;
    }
}

/*
    Function to move the ghosts across the board
*/
void move_ghosts(character* ghosts)
{
    // First... move the ghosts across the board... this is "try to change the coordinates"
    character* current = ghosts;
    for (int i = 0; i < GHOSTS_NUMBER; i++)
    {
        if (current->status == 1) // active status
        {
            // Mark the current coords as free
            busy_board[current->y[1]][current->x[1]]--;
            
            // Try to move the current character
            if (!move_character(current, FALSE))
            {
                // Change direction to a valid one 
                int random = arc4random() % 4;
                while (random == current->direction)
                {
                    random = arc4random() % 4;
                }
                current->direction = random;

                // Update direction and move
                move_character(current, FALSE);
            }

            // Mark the new coordinates as busy
            busy_board[current->y[1]][current->x[1]]++;
        }
        else if (current->status == 0)
        {
            // Get out of the cage
            busy_board[current->y[1]][current->x[1]]--;
            move_character(current, TRUE);

            // Rebuild the wall
            if (current->y[1] == GHOST_SPAWN_Y - 4)
            {
                mvaddch(current->y[1] + 1, current->x[1], ACS_HLINE);
                current->status = 1;
            }
            busy_board[current->y[1]][current->x[1]]++;

        }

        // Change to the next ghost
        current++;
    }
}

/*
    Function that detects a collision between two given characters
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
void move_pacman(character* pacman)
{
    // Mark the current coordinates as free
    busy_board[pacman->y[1]][pacman->x[1]]--;

    // Try to move the character in the latest direction
    move_character(pacman, FALSE);

    // mark the new coords as busy
    busy_board[pacman->y[1]][pacman->x[1]]++;
    
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
        score += 10;
        dots_counter++;
        board[pacman->y[1]][pacman->x[1]] = -1;
        // Disable all the ghosts
        dizzy = 1;
    }
}

/*
    Move a character across the board by one unit in a specified direction
    direction can be 0, 1, 2, 3 for left, right, down and up.
    Direction is a member of character struct.
    Returns TRUE is there was not a collision with a wall.
    It updates the x or y members of the character pointer if the movement was
    succesfull.
*/
int move_character(character* char_moving, int not_walls)
{
    int move = FALSE;
    
    char_moving->x[0] = char_moving->x[1];
    char_moving->y[0] = char_moving->y[1];

    switch(char_moving->direction)
    {
        case 0: // left
            int left_index = char_moving->x[1] - 1;
            int left_char = left_index < 0 ? 0 : board[char_moving->y[1]][left_index];
            if (not_walls || left_char <= 0)
            {
                char_moving->x[1]--;
                move = TRUE;
            }
            break;

        case 1: // right 
            int right_index = char_moving->x[1] + 1;
            int right_char = right_index >= BOARD_WIDTH ? 0 : board[char_moving->y[1]][right_index];
            if (not_walls || right_char <= 0) 
            {
                char_moving->x[1]++;
                move = TRUE;
            }
            break;
        
        case 2: // down
            int down_index = char_moving->y[1] + 1;
            int down_char = down_index >= BOARD_HEIGHT ? 0 : board[down_index][char_moving->x[1]];
            if (not_walls || down_char <= 0)
            {
                char_moving->y[1]++;
                move = TRUE;
            }
            break;

        case 3: // up
            int up_index = char_moving->y[1] - 1;
            int up_char = up_index < 0 ? 0 : board[up_index][char_moving->x[1]];
            if (not_walls || up_char <= 0)
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
