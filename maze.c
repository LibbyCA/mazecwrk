/**
 * @file maze.c
 * @author Libby Atack
 * @brief Code for the maze game for COMP1921 Assignment 2
 * NOTE - You can remove or edit this file however you like - this is just a provided skeleton code
 * which may be useful to anyone who did not complete assignment 1.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// defines for max and min permitted dimensions
#define MAX_DIM 100
#define MIN_DIM 5

// defines for the required autograder exit codes
#define EXIT_SUCCESS 0
#define EXIT_ARG_ERROR 1
#define EXIT_FILE_ERROR 2
#define EXIT_MAZE_ERROR 3

// coordinates of player
typedef struct __Coord
{
    int x;
    int y;
} coord;

typedef struct __Maze
{
    // pointer
    char **map;
    int height;
    int width;
    coord start;
    coord end;
} maze;

/**
 * @brief Initialise a maze object - allocate memory and set attributes
 *
 * @param this pointer to the maze to be initialised
 * @param height height to allocate
 * @param width width to allocate
 * @return int 0 on success, 1 on fail
 */
int create_maze(maze *this, int height, int width)
{

    // allocates memory to columns of maze
    // sizeof assures correct amount of memory needed

    this->map = malloc(height * sizeof(char *));
    if (this->map == NULL) {
        return EXIT_MAZE_ERROR;
    }

    // allocates memory to rows of maze
    // check for memory allocation errors

    for (int i = 0; i <height; i++) {
        this->map[i] = malloc(width * sizeof(char));
        if (this->map[i] == NULL) {
            return EXIT_MAZE_ERROR;
        }
    }

    this->height = height;
    this->width = width;

    return EXIT_SUCCESS;
}

/**
 * @brief Free the memory allocated to the maze struct
 *
 * @param this the pointer to the struct to free
 */
void free_maze(maze *this)
{

    // free deallocates memory previously used to store rows and columns for maze 

    for (int i = 0; i < this->height; i++) {
        free(this->map[i]);
    }

    free(this->map);
    this->map = NULL;
}

/**
 * @brief Validate and return the width of the mazefile
 *
 * @param file the file pointer to check
 * @return int 0 for error, or a valid width (5-100)
 */
int get_width(FILE *file)
{

    // check whether file has been read in correctly

    if (file == NULL) 
    {
        printf("File not loaded in properly");
        return EXIT_MAZE_ERROR;
    }

    int width = 0;
    char ch; 

    // count number of characters across
    // increase width until a space or reaches the end of the file
    // use fgetc to read file in by character 

    while ((ch = fgetc(file)) != '\n' && ch !=EOF)
    {
        width++;
    }

    // check width of maze is within given bounds

    if (width < MIN_DIM || width > MAX_DIM)
    {
        printf("Invalid width of maze \n");
        return 0;
    }

    rewind(file);
    return width;
}

/**
 * @brief Validate and return the height of the mazefile
 *
 * @param file the file pointer to check
 * @return int 0 for error, or a valid height (5-100)
 */
int get_height(FILE *file)
{

    // check whether file has been read in fully

    if (file == NULL){
        printf("File not loaded properly");
        return EXIT_MAZE_ERROR;
    }
    int height = 0;
    char ch; 

    // checks it hasn't reached the end of file
    // counts the number of new lines to determine height

    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == '\n')
        {
            height++;
        }
    }
    
    // check height of maze is within given bounds

    if (height < MIN_DIM || height > MAX_DIM)
    {
        printf("Invalid height of maze \n");
        return EXIT_MAZE_ERROR;
    }

    rewind(file);
    return height; 
}

/**
 * @brief read in a maze file into a struct
 *
 * @param this Maze struct to be used
 * @param file Maze file pointer
 * @return int 0 on success, 1 on fail
 */
int read_maze(maze *this, FILE *file)
{
    if (file == NULL)
    {
        printf("File not loaded properly");
        return EXIT_FILE_ERROR;
    }

    this->width = get_width(file);
    this->height = get_height(file);

    this->map = malloc(this->height* sizeof(char *));
    if (this->map == NULL) {
        printf("Memory allocation failed");
        return EXIT_MAZE_ERROR;
    }

    // allocates memory to rows in maze
    // check memory allocation was successfully
    // deallocates memory

    for (int i = 0; i < this->height; i++) {
        this->map[i] = malloc(this->width * sizeof(char));
        if (this->map[i] == NULL){
            printf("Memory allocation failed");
            for (int j = 0; j < i; j++){
                free(this->map[j]);
            }
            free(this->map);
            return EXIT_MAZE_ERROR;
        }
    }

    int row_width = 0;
    for(int i = 0; i < this ->height; i++)
    {
        for (int j = 0; j < this->width; j++){
            char ch = fgetc(file);

            // establish coordinates of start point

            if (ch == 'S')
            {
                this->start.x = i;
                this->start.y = j;
            }

            // establish coordinates of end point

            else if (ch == 'E'){
                this->end.x = i;
                this->end.y = j;
            }

            // check only valid characters are in maze

            else if (ch != ' ' && ch != '#' && ch !='S' && ch!= 'E'){
                return EXIT_MAZE_ERROR;
            }
            this->map[i][j] = ch;
            row_width++;
        }

        // check all rows are the same number of characters across

        if (row_width != this->width){
            printf("All rows must be same length\n");
            return EXIT_MAZE_ERROR;
        }
        row_width = 0;
        fgetc(file);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Prints the maze out - code provided to ensure correct formatting
 *
 * @param this pointer to maze to print
 * @param player the current player location
 */
void print_maze(maze *this, coord *player)
{
    // make sure we have a leading newline..
    printf("\n");
    for (int i = 0; i < this->height; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            // decide whether player is on this spot or not
            if (player->x == i && player->y == j)
            {
                printf("X");
            }
            else
            {
                printf("%c", this->map[i][j]);
            }
        }
        // end each row with a newline.
        printf("\n");
    }
}

/**
 * @brief Validates and performs a movement in a given direction
 *
 * @param this Maze struct
 * @param player The player's current position
 * @param direction The desired direction to move in
 */
void move(maze *this, coord *player, char direction)
{
    coord current = *player;
    switch(direction) {
        case 'W':
            current.x--;
            break;
        case 'S':
            current.x++;
            break;
        case 'D':
            current.y++;
            break;
        case 'A':
            current.y--;
            break;
        default:
            printf("Invalid direction: %c\n", direction);
            return;      
    }

    // validation check to whether player is moving outside the maze

    if (current.x < 0 || current.x >= this->height || current.y < 0 || current.y >= this->width)
    {
        printf("You cannot go outside the maze \n");
        return;
    }

    // validation check to whether player is moving into a wall

    if(this->map[current.x][current.y] == '#')
    {
        printf("You have hit a wall\n");
        return;
    }

    *player = current;
}

/**
 * @brief Check whether the player has won and return a pseudo-boolean
 *
 * @param this current maze
 * @param player player position
 * @return int 0 for false, 1 for true
 */
int has_won(maze *this, coord *player)
{
    // COMMMENTHFUDGHAFIUGESI

    if (this == NULL || player == NULL)
    {
        printf("Invalid arguments");
        return EXIT_SUCCESS;
    }

    if (player->x == this->end.x && player->y == this->end.y)
    {
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char *argv[])
{
    // check the correct about of arguments are inputted - only one mazefile

    if (argc != 2)
    {
        printf("Usage: %s <mazefile>\n", argv[0]);
        return EXIT_ARG_ERROR;
    }
    
    coord *player;
    maze *this_maze = malloc(sizeof(maze));

    // open maze file and check it opened correctly

    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        printf("Failed to open file: %s\n", argv[1]);
        return EXIT_FILE_ERROR;
    }

    // read in maze file, output error if not read in correctly

    if (read_maze(this_maze, f) != EXIT_SUCCESS)
    {
        printf("Failed to read maze from file: %s\n", argv[1]);
        fclose(f);
        return EXIT_MAZE_ERROR;
    }

    player = &(this_maze->start);

    // maze game loop

    while(!has_won(this_maze, player))
    {
        
        char input[256], direction;
        printf("Enter your move: ");

        // make sure only one character is entered and only valid characters are inputted
        
        fgets(input, sizeof(input), stdin);

        if(input[1]!= '\n' || strchr("WASDMwasdm", input[0]) == NULL)
        {
            printf("Invalid input\n");
            continue;
        }
        else
        // make all inputs uppercase
        {
            direction = toupper(input[0]);


        switch(direction)
        {
            case 'W': case 'A': case 'S': case 'D':
                 move(this_maze, player, direction);
                 break;
            case 'M':
                print_maze(this_maze, player);
                break;
            default:
                printf("Invalid input");
                break;

        }
        
    }
        
    }

    // only post congrats message if player has made it to the end

    if (has_won(this_maze, player))
    {
        printf("Congrats, you have made to the end of the maze\n");
    }

    
    free_maze(this_maze);
    fclose(f);
    return EXIT_SUCCESS;

}
