#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int LINE_LEN = 150;

typedef enum {
    UP, 
    DOWN, 
    LEFT, 
    RIGHT, 
    DIAGONAL_UP_LEFT, 
    DIAGONAL_UP_RIGHT, 
    DIAGONAL_DOWN_LEFT, 
    DIAGONAL_DOWN_RIGHT
} Direction;

const char* DirectionStrings[] = {
    "UP", 
    "DOWN", 
    "LEFT", 
    "RIGHT", 
    "DIAGONAL_UP_LEFT", 
    "DIAGONAL_UP_RIGHT", 
    "DIAGONAL_DOWN_LEFT", 
    "DIAGONAL_DOWN_RIGHT"
};

int check_match(char** input, int y_len, int x_len, int y_pos, int x_pos, Direction direction){
    char pattern[] = "XMAS";
    int x = x_pos;
    int y = y_pos;

    int x_dir;
    int y_dir;
    switch (direction) {
        case UP:
            if(y - (strlen(pattern)-1) < 0) return 0;
            x_dir = 0;
            y_dir = -1;
            break;

        case DOWN:
            if(y + (strlen(pattern)-1) > y_len) return 0;
            x_dir = 0;
            y_dir = 1;
            break;

        case LEFT:
            if(x - (strlen(pattern)-1) < 0) return 0;
            x_dir = -1;
            y_dir = 0;
            break;

        case RIGHT:
            if(x + (strlen(pattern)-1) > x_len) return 0;
            x_dir = 1;
            y_dir = 0;
            break;

        case DIAGONAL_UP_LEFT:
            if(y - (strlen(pattern)-1) < 0) return 0;
            if(x - (strlen(pattern)-1) < 0) return 0;
            x_dir = -1;
            y_dir = -1;
            break;

        case DIAGONAL_UP_RIGHT:
            if(x + (strlen(pattern)-1) > x_len) return 0;
            if(y - (strlen(pattern)-1) < 0) return 0;
            x_dir = 1;
            y_dir = -1;
            break;

        case DIAGONAL_DOWN_LEFT:
            if(y + (strlen(pattern)-1) > y_len) return 0;
            if(x - (strlen(pattern)-1) < 0) return 0;
            x_dir = -1;
            y_dir = 1;
            break;

        case DIAGONAL_DOWN_RIGHT:
            if(y + (strlen(pattern)-1) > y_len) return 0;
            if(x + (strlen(pattern)-1) > x_len) return 0;
            x_dir = 1;
            y_dir = 1;
            break;
        default:
            fprintf(stderr, "Invalid direction.\n");
            return 0;
    }
    
    for (int i = 0; (((x_pos >= 0 && x_pos < x_len) && (y_pos >= 0 && y_pos < y_len)) && i < strlen(pattern)); i++) {
        if (input[y_pos][x_pos] == pattern[i]) {
            if(i == (strlen(pattern)-1)) return 1;
            y_pos += y_dir;
            x_pos += x_dir;
        }
        else {
            return 0;
        }
    }

    return 0;
    
}


int get_xmas_matches(char** input, int lines){
    int matches = 0;
    for (int y = 0; y < lines; y++) {
        int linelen = strlen(input[y]);
        for (int x = 0; x < linelen; x++) {
            for (Direction dir = 0; dir<= DIAGONAL_DOWN_RIGHT; dir++) {
                if (check_match(input, lines, linelen, y, x, dir)) matches++;
            }
        }
    }
    return matches;
}


int main()
{
    char** input = NULL;
    int lines = 0;

    {
        char ch;
        while ((ch = getchar()) != EOF) {
            if (ch == '\n') lines++;
        }
    }
    
    input = malloc(lines * sizeof(char*));
    if (input == NULL) return -1;

    for (int i = 0; i < lines; i++) {
        input[i] = calloc(LINE_LEN, sizeof(char));
        if (input[i] == NULL) return -1;
    }

    if (freopen(NULL, "r", stdin) == NULL){
        fprintf(stderr, "Failed to rewind stdin.\n");
        return -1;
    }

    for (int i = 0; i < lines; i++) {
        fgets(input[i], LINE_LEN, stdin);
    }
    printf("Number of XMAS: %d\n", get_xmas_matches(input, lines));

    for (int i = 0; i < lines; i++) {
        free(input[i]);
    }
    free(input);
}
