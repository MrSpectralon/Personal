#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int LINE_LEN = 150;

int check_match(char** input, int y_pos, int x_pos){
    if (input[y_pos][x_pos] != 'A') return 0;
    
    if (!(
        ((input[y_pos-1][x_pos-1] == 'M') && (input[y_pos+1][x_pos+1] == 'S'))
        ||
        ((input[y_pos-1][x_pos-1] == 'S') && (input[y_pos+1][x_pos+1] == 'M'))
    )) return 0;
    
    if (!(
        ((input[y_pos-1][x_pos+1] == 'M') && (input[y_pos+1][x_pos-1] == 'S'))
        ||
        ((input[y_pos-1][x_pos+1] == 'S') && (input[y_pos+1][x_pos-1] == 'M'))
    )) return 0;
    
    return 1;
}


int get_xmas_matches(char** input, int lines){
    int matches = 0;
    for (int y = 1; y < (lines - 1); y++) {
        int linelen = strlen(input[y]);
        for (int x = 1; x < (linelen - 1); x++) {
            if (check_match(input, y, x)) matches++;
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
