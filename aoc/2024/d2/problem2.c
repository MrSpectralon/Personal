#include <stdio.h>
#include <stdlib.h>
#include <string.h>
FILE* fptr = NULL;

const int ROW_LENGTH = 100;
const int ROWS = 1000;

typedef enum Rate { ASCENDING, DESCENDING} Rate;


int parce_level(char* report, int start, int end)
{
	char buffer[3];
	for (int j = 0; j <= abs(start - end); j++) {
		buffer[j] = report[start + j];
	}
	return atoi(buffer);
}

int parce_report(char* report, int** l)
{
	int* levels = NULL;
	//Number of levels
	int nol = 1;
	for (int i = 0; i < strlen(report); i++) {
		if (report[i] == ' ') nol++;
	}
	
	levels = calloc(nol, sizeof(int));
	if(levels == NULL) return 0;
	
	int pos = 0;
	int level = 0;
	for (int i = 0; i < strlen(report); i++) {
		if (report[i] == ' ') {
			levels[level] = parce_level(report, pos, i-1);
			pos = i+1;
			level++;
			continue;
		}
		if (i == strlen(report)-1) {
			levels[level] = parce_level(report, pos, i-1);
		}
	}
	*l = levels;
	return nol;
}
int check_if_safe(int num, int* levels)
{	
	Rate* rate = NULL;

	for (int i = 0; i < num - 1; i++) {
		int diff = levels[i] - levels[i+1];
		if (diff == 0) {
			// printf("Unsafe - No difference\n");
			goto unsafe;
		}
		if(rate == NULL){
			rate = malloc(sizeof(Rate));
			if (rate == NULL) {
				fprintf(stderr, "Error allocating rate enum.\n");
				goto unsafe;
			}
			*rate = (diff > 0) ? DESCENDING : ASCENDING;
		}
		
		if ((*rate == ASCENDING && diff > 0) || (*rate == DESCENDING && diff < 0) ) {
			// printf("Unsafe - Rate direction change.\n");
			goto unsafe;
		}

		if (*rate == ASCENDING && (diff < 0 && diff > -4)) {
			continue;
		}
		if (*rate == DESCENDING && (diff > 0 && diff < 4)){
			continue;
		}
		// printf("Unsafe - Not within rate range.\n");
		goto unsafe;
	}
	free(rate);
	// printf("Safe report\n");
	return 1;

unsafe:
	free(rate);
	return 0;
}


int tolerate_bad(int num, int* levels)
{
	int temp[num];
	int x = 0;
	for (int i = 0; i < num; i++) {
		x = 0;
		for (int j = 0; j < num; j++) {
			if (i == j) {
				continue;
			}
			temp[x] = levels[j];
			x++;
		}
		if (check_if_safe(num-1, temp)) {
			printf("Original: ");
			for (int f = 0; f < num; f++) {
				printf("%d ", levels[f]);
			}
			printf("\n");
			printf("Working: ");
			for (int f = 0; f < num - 1; f++) {
				printf("%d ", temp[f]);
			}
			printf("\n");
			return 1;
		}
	}
	return 0;
}



int main()
{
    int sum = 0;
	int* levels = NULL;
    char buffer[ROW_LENGTH];
	fptr = fopen("./input.txt", "r");
    for (int i = 0; i < ROWS; i++) {
    	
		fgets(buffer, ROW_LENGTH, fptr);
		int num = parce_report(buffer, &levels);
		
		if (levels == NULL) {
			fprintf(stderr, "Report parcing failed.\n");
			return 0;
		}

		if (!check_if_safe(num, levels)) {
			sum += tolerate_bad(num, levels);
		}
		else
		{
			sum++;
		}
		
		free(levels);
		levels = NULL;
    }
	printf("%d\n", sum);
	return 0;
}
