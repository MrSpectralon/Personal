
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    UP, 
    RIGHT, 
    DOWN,
    LEFT
} Direction;

const char* Direction_str[] = { "Up", "Right", "Down", "Left" };

typedef struct {
    int x, y;
    Direction direction;

} Investigate;



const int BUFFER_LEN = 135;
int* get_start_coordinates(char** lab_map);
int check_visit(char** lab_map, Investigate* i);


void rotate(Investigate** inv);
int investigation_incr(Investigate** i);

void free_coordinates(char** lab_map);
char** get_input();
int walk(char** lab_map, Investigate* investigation);

void free_lab_map(char** lab_map){
    if (lab_map == NULL) {
        return;
    }
    for (int i = 0; lab_map[i] != NULL; i++) {
        free(lab_map[i]);
    }
    free(lab_map);
}

    
int investigation_incr(Investigate** i){
    switch ((*i)->direction) {
        case UP:
            (*i)->y -= 1;
            if ((*i)->y < 0) return 0;
            break;
        case RIGHT:
            (*i)->x += 1;
            break;
        case DOWN:
            (*i)->y += 1;
            break;
        case LEFT:
            (*i)->x -= 1;
            if ((*i)->x < 0) return 0;
            break;
    }
    return 1;
}

int walk(char** lab_map, Investigate* i){
    int x, y;
    switch (i->direction) {
        case UP:
            y = i->y - 1;
            if (i->y < 0) return 2;
            x = i->x;
            break;
        case RIGHT:
            x = i->x + 1;
            y = i->y;
            break;
        case DOWN:
            y = i->y + 1;
            x = i->x;
            break;
        case LEFT:
            x = i->x - 1;
            if (x < 0) return 2;
            y = i->y;
            break;
    }
    if (lab_map[y] == NULL) return 2;
    if (lab_map[y][x] == '\n') return 2;
    if (lab_map[y][x] == '#') {
        
        return 1;
    }
    
    lab_map[y][x] = 'X';
    return 0;
}

void rotate(Investigate** inv){
    if ((*inv)->direction == LEFT) {
        (*inv)->direction = UP;
        return;
    }
    (*inv)->direction++;
}

int* get_start_coordinates(char** lab_map){
    
    int* coordinates = malloc(sizeof(int) * 2);
    if (coordinates == NULL) {
        fprintf(stderr, "Error occurred when allocating memory for coordinates.\n");
        return NULL;
    }

    for (int y = 0; lab_map[y] != NULL; y++) {
        for (int x = 0; lab_map[y][x] != '\0'; x++) {
            if (lab_map[y][x] == '^') {
                coordinates[0] = y;
                coordinates[1] = x;
                return coordinates;
            }
        }
    }
    free(coordinates);
    return NULL;
}

char** get_input(){
    char** lab_map = NULL;
    char buffer[BUFFER_LEN];
    
    int y_len = 0;

    while (fgets(buffer, BUFFER_LEN, stdin) != NULL) {
        y_len++;
    }
    if (freopen(NULL, "r", stdin) == NULL) {
        fprintf(stderr, "Failed to rewind input stream.\n");
        return NULL;
    }

    lab_map = malloc(sizeof(char*) * (y_len+1));
    if (lab_map == NULL) {
        fprintf(stderr, "Failed to allocate memory for y axis in lab_map.\n");
        return NULL;
    }
    
    for (int i = 0; i <= y_len; i++) {
        lab_map[i] = NULL;
    }

    for (int i = 0; i < y_len; i++) {
        fgets(buffer, BUFFER_LEN, stdin);
        lab_map[i] = strdup(buffer);
        if (lab_map == NULL) goto cleanup;
    }
    
    return lab_map;

    cleanup:
    for (int i = 0; lab_map[i] != NULL; i++) {
        free(lab_map[i]);
    }
    free(lab_map);
    return NULL;
}

int main(){
    char** lab_map = get_input();
    int* start_coordinates = NULL;
    Investigate* investigation = NULL;
    int sum = 0;

    if (lab_map == NULL) return 0;
    start_coordinates = get_start_coordinates(lab_map);
    if (start_coordinates == NULL) {
        printf("Found no start coordinates.\n");
        goto cleanup;
    }
    investigation = malloc(sizeof(Investigate));
    investigation->direction = UP;
    investigation->y = start_coordinates[0];
    investigation->x = start_coordinates[1];
    free(start_coordinates);
    start_coordinates = NULL;
    printf("%d, %d\n", investigation->x, investigation->y);
    lab_map[investigation->y][investigation->x] = 'X';


    int investigating = 1;
    
    while (investigating) {
        int status = walk(lab_map, investigation);
        switch (status) {
            case 2:
                investigating = 0;
                break;
            case 1:
                rotate(&investigation);
                break;
            case 0:
                if (!investigation_incr(&investigation)){
                    investigating = 0;
                }
                break;
            default:
                fprintf(stderr, "WTF?");
                break;
        }
    }
    
    for (int i = 0; lab_map[i] != NULL; i++) {
        for (int j = 0; lab_map[i][j] != '\n'; j++) {
            if (lab_map[i][j] == 'X') sum++;
        }
    
    }

    for (int i = 0; lab_map[i] != NULL; i++) {
        printf("%s", lab_map[i]);
    }
    printf("Sum: %d\n", sum);
cleanup:
    
    free(investigation);
    free(start_coordinates);
    free_lab_map(lab_map);
    return 0;

}
