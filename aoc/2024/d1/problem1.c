
#include <stdio.h>
#include <stdlib.h>
FILE* fptr = NULL;

const int ROW_LENGTH = 15;
const int ROWS = 1000;

void sort(int* arr, int len)
{
    int tmp = 0;
    int indx = 0;
    for (int i = 0; i < len; i++) {
	tmp = arr[i];
	indx = i;
	for (int j = i;  j < len; j++) {
	    if (arr[j] < tmp) {
		tmp = arr[j];
		indx = j;
	    }
	}
	arr[indx] = arr[i];
	arr[i] = tmp;
    }
}


int main()
{
    int sum = 0;
    int row1[ROWS];
    int row2[ROWS];
    
    char buffer[ROW_LENGTH];
    char w1[6];
    char w2[6];
    
    fptr = fopen("./input.txt", "r");
    for (int i = 0; i < ROWS; i++) {
	fgets(buffer, ROW_LENGTH, fptr);
	for (int j = 0; j < 5; j++) {
	    w1[j] = buffer[j];
	    w2[j] = buffer[j+8];
	}
	row1[i] = atoi(w1);
	row2[i] = atoi(w2);
	buffer[0] = '\0';
    }
    
    sort(row1, ROWS);
    sort(row2, ROWS);
    for (int i = 0; i < ROWS; i++) {
	sum += abs(row1[i] - row2[i]);
    }
    printf("%d\n", sum);
    return 0;
}
