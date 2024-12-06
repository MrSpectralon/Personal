#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int PAGE_RULE_LEN = 7;
const int PAGE_LEN = 100;

void free_rules_map(int*** map){
    if (map == NULL) return;
    for (int i = 0; map[i] != NULL; i++) {
        for (int j = 0; j < 2; j++) {
            free(map[i][j]);
            map[i][j] = NULL;
        }
        free(map[i]);
        map[i] = NULL;
    }
    free(map);
}

int*** make_rules_map(char** rules, int rules_len){
    int*** map = NULL;
    int unique_rules[rules_len] = {};
    int num_unique_rules = 0;
    
    int row1[rules_len] = {};
    int row2[rules_len] = {};
    for (int i = 0; i < rules_len; i++) {
        char val1[3];
        char val2[3];
        int v1 = 0;
        int v2 = 0;
        
        val1[0] = rules[i][0];
        val1[1] = rules[i][1];
        val1[2] = '\0';
        v1 = atoi(val1);
        val2[0] = rules[i][3];
        val2[1] = rules[i][4];
        val2[2] = '\0';
        v2 = atoi(val2);
        row1[i] = v1;
        row2[i] = v2;
    }

    for (int i = 0; i < rules_len; i++) {
        for (int j = 0; j <= num_unique_rules; j++) {
            if (num_unique_rules == 0) {
                unique_rules[j] = row1[i];
                num_unique_rules++;
                break;
            }
            if (unique_rules[j] == row1[i]) {
                break;
            }
            if(unique_rules[j] == 0){
                unique_rules[j] = row1[i];
                num_unique_rules++;
                break;
            }
        }
    }
    
    map = malloc((num_unique_rules+1) * sizeof(int*));
    if (map == NULL) return NULL;
    for (int i = 0; i <= num_unique_rules; i++) {
        map[i] = NULL;
    }
    for (int i = 0; i < num_unique_rules; i++) {
        map[i] = malloc(2 * sizeof(int*));
        if (map[i] == NULL) goto free2d;
        for (int y = 0; y < 2; y++) {
            map[i][y] = NULL;
        }
        map[i][0] = malloc(sizeof(int));
        if (map[i][0] == NULL) goto free3d;
    }
    
    for (int i = 0; i < num_unique_rules; i++) {
        int num_allowed_after = 0;
        int num_added = 0;
        map[i][0][0] = unique_rules[i];
        for (int x = 0; x < rules_len; x++) {
            if (map[i][0][0] == row1[x]) num_allowed_after++;
        }
        map[i][1] = malloc((num_allowed_after+1) * sizeof(int));
        if (map[i][1] == NULL) goto free3d;

        map[i][1][num_allowed_after] = -1;
        for (int j = 0; j < rules_len;  j++) {
            if (map[i][0][0] != row1[j]) continue;
            map[i][1][num_added] = row2[j];
            num_added++;
        }
    }
    
    return map;

free3d:
    for (int i = 0; i < num_unique_rules; i++) {
        for (int j = 0; j < 2; j++) {
            free(map[i][j]);
            map[i][j] = NULL;
        }
    }
free2d:
    for (int i = 0; i < num_unique_rules; i++) {
        free(map[i]);
        map[i] = NULL;
    }
    free(map);
    return NULL;
}

void free_int_pages(int** int_pages){
    
    for (int i = 0; int_pages[i] != NULL; i++) {
        free(int_pages[i]);
    }
    free(int_pages);
}

int** convert_pages_to_int(char** pages, int pages_len){
    int** int_pages = NULL;
    
    int_pages = malloc((pages_len+1) * sizeof(int*));
    if (int_pages == NULL) return NULL;
    for (int i = 0; i <= pages_len; i++) int_pages[i] = NULL;

    for (int i = 0; i < pages_len; i++) {
        int num = 1;
        for (int j = 2; j < strlen(pages[i]); j++) {
            if (pages[i][j] == ',') num++;
        }
        int_pages[i] = malloc((num+1) * sizeof(int));
        if (int_pages[i] == NULL) goto cleanup;
        int_pages[i][num] = -1;
    }
    
    for (int i = 0; int_pages[i] != NULL; i++) {
        int indx = 0;
        for (int j = 0; j < strlen(pages[i]); j++) {
            if ((pages[i][j] != ',') && (j != strlen(pages[i]) -1)) continue;
            char val[3];
            val[0] = pages[i][j-2];
            val[1] = pages[i][j-1];
            val[2] = '\0';
            int num = atoi(val);
            int_pages[i][indx] = num;
            indx++;
        }
    }
    return int_pages;

cleanup:
    for (int i = 0; int_pages[i] != NULL ; i++) {
        free(int_pages[i]);
        int_pages[i] = NULL;
    }
    free(int_pages);
    return NULL;
}

int check_valid_page_order(int* page, int** rules, int location){
       
    for (int y = 0; rules[1][y] != -1; y++) {
        for (int z = location-1; z >= 0; z--) {
            
            if (rules[1][y] == page[z]) {
                return 0;
            }
        }

    }
    return 1;
}

int tired(int*** rules_map, int* integer_pages){
    int j = 0;
    for (; integer_pages[j] != -1; j++){}
    
    int middle = j/2;
    for (; j >= 0; j--) {

        for (int x = 0; rules_map[x] != NULL; x++) {
                    
            if (rules_map[x][0][0] != integer_pages[j]) {
                continue;
            }
            int res = check_valid_page_order(integer_pages, rules_map[x], j);
            if (!res) {
                return 0;
            }
        }
    }
    return middle;
}

int eh(char** rules, int rules_len, char** pages, int pages_len){
    int sum = 0;
    int*** rules_map = NULL;
    int** integer_pages = NULL;

    rules_map = make_rules_map(rules, rules_len);
    if (rules_map == NULL) {
        return 0;
    }
    //  Structure:
    //  [amount_of_rules] -> [0] -> {rule_value}
    //  [amount_of_rules] -> [1] -> {allowed_after_rule_value, ...}

    
    integer_pages = convert_pages_to_int(pages, pages_len);
    if (integer_pages == NULL) goto cleanup;
    // Structure:
    //  [page_number] -> {num1, num2, ...}
    
        
    for (int i = 0; integer_pages[i] != NULL; i++) {
        int middle = tired(rules_map, integer_pages[i]);
        if (middle > 0) {
            sum += integer_pages[i][middle];
        }   
    }
    free_rules_map(rules_map);
    free_int_pages(integer_pages);
    return sum;

cleanup:
    free_rules_map(rules_map);
    free_int_pages(integer_pages);
    return 0;
}

int main(){
    
    char** page_rules = NULL;
    char** pages = NULL;

    int num_of_page_rules = 0;
    int num_of_pages = 0;
    char rule_buffer[PAGE_RULE_LEN];
    char page_buffer[PAGE_LEN];


    while (fgets(rule_buffer, PAGE_RULE_LEN, stdin) != NULL) {
        if (!strcmp(rule_buffer, "\n")) break;
        num_of_page_rules++;
    }

    while (fgets(page_buffer, PAGE_LEN, stdin) != NULL) {
        num_of_pages++;
    }
    
    if(freopen(NULL, "r", stdin) == NULL){
        fprintf(stderr, "Failed to rewind stdin.\n");
        return 0;
    }
    
    page_rules = malloc(num_of_page_rules * sizeof(char*));
    if (page_rules == NULL) {
        fprintf(stderr, "Failed to allocate memory for page rules(d1).\n");
        goto cleanup;
    }
    
    for (int i = 0; i < num_of_page_rules; i++) {
        fgets(rule_buffer, PAGE_RULE_LEN, stdin);
        page_rules[i] = strdup(rule_buffer);
        if (page_rules[i] == NULL){ 
            goto cleanup;
            fprintf(stderr, "Failed to allocate memory for page rule(d2).\n");
        }
    }

    //gets rid of newline separator in between the rules and pages info.
    fgets(rule_buffer, PAGE_RULE_LEN, stdin);

    pages = malloc(num_of_pages * sizeof(char*));
    if (pages == NULL) {
        fprintf(stderr, "Failed to allocate memory for pages(d1).\n");
        goto cleanup;
    }
    for (int i = 0; i < num_of_pages; i++) {
        fgets(page_buffer, PAGE_LEN, stdin);
        pages[i] = strdup(page_buffer);
        if (pages[i] == NULL) {
            fprintf(stderr, "Failed to allocate memory for page(d2).\n");
            goto  cleanup;
        }
    }
    
    printf("Aggregated numbers: %d\n", eh(page_rules, num_of_page_rules, pages, num_of_pages));

cleanup:

    if(page_rules != NULL){
        for (int i = 0; i < num_of_page_rules; i++) {
            free(page_rules[i]);
        }
    }
    free(page_rules);
    
    if(pages != NULL){
        for (int i = 0; i < num_of_pages; i++) {
            free(pages[i]);
        }
    }
    free(pages);

    return 0;
}
