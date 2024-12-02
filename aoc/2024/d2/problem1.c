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

int check_if_safe(char* report)
{	
	int* levels = NULL;
	int num = parce_report(report, &levels);
	if (levels == NULL) {
		fprintf(stderr, "Report parcing failed.\n");
		return 0;
	}
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
	free(levels);
	// printf("Safe report\n");
	return 1;

unsafe:
	free(rate);
	free(levels);
	return 0;
}


int main()
{
    int sum = 0;
    char buffer[ROW_LENGTH];
	fptr = fopen("./input.txt", "r");
    for (int i = 0; i < ROWS; i++) {
    	fgets(buffer, ROW_LENGTH, fptr);
		sum += check_if_safe(buffer);
    }
	printf("%d\n", sum);
	return 0;
}
