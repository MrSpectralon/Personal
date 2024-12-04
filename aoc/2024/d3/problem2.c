#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int BUFFER_SIZE = 4096;


int check_value_pattern(char* bptr){
	int start = 0;
	int end = 0;
	char temp[4];
		
	int num1 = 0;
	
	int out_of_names = 0;


	for (int i = 1; i < 9; i++) {
		
		switch (bptr[i]) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (start == 0) start = i;
				if ((i - start) > 3 ) return 0;
				break;
			case ',':
				if(start != 0){
					end = i;
					for (int j = 0; j < (end - start); j++) temp[j] = bptr[start+j];
					temp[end-start] = '\0';
					num1 = atoi(temp);
					out_of_names += end-start;
					start = 0;
					continue;
				}
				return 0;
			case ')':
				if (start != 0){
					end = i;
					for (int j = 0; j < (end - start); j++) temp[j] = bptr[start+j];
					temp[end-start] = '\0';
					out_of_names += end - start;
					return num1 * atoi(temp);
				}
				return 0;
			default:
				return 0;
		}
	}
	return 0;
}

int check_input(char* input, int* activated){

	int sum = 0;
	char mul_pattern[] = "mul(";
	char stop_pattern[] = "don't()";
	char start_pattern[] = "do()";
	
	int mul_pos = 0;
	int stop_pos = 0;
	int start_pos = 0;
	
	
	int do_ = 0;
	int dont = 0;

	for (int i = 0; i < strlen(input); i++) {
		if (input[i] == mul_pattern[mul_pos] && *activated) {
			if (mul_pos == 3) {
				char* bptr = NULL;
				bptr = &input[i];
				sum += check_value_pattern(bptr);
				mul_pos = 0;
			}
			else mul_pos++;
		}
		else mul_pos = 0;

		if (input[i] == stop_pattern[stop_pos]) {
			if (stop_pos == 6) {
				dont++;
				*activated = 0;
				stop_pos = 0;
			}
			else stop_pos++;
		}
		else stop_pos = 0;

		if (input[i] == start_pattern[start_pos]) {
			if (start_pos == 3) {
				do_++;
				*activated = 1;
				start_pos = 0;
			}
			else {
				start_pos++;
			}
		}
		else {
			start_pos = 0;
		}
		
	}
	return sum;

}


int main(){
	unsigned long sum = 0;

	int activated = 1;
	char buffer[BUFFER_SIZE];
	while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
		sum += check_input(buffer, &activated);
		// break;
	}

	printf("Total sum: %lu\n", sum);
	return 0;
}
