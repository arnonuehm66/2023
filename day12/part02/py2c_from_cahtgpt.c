#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


typedef enum {
    OPERATIONAL = '.',
    DAMAGED = '#',
    UNKNOWN = '?'
} SpringState;


//******************************************************************************
int is_valid_condition(char* spring_state, int* damaged_spring_record) {
    int len = strlen(spring_state);

    if (damaged_spring_record[0] <= len) {
        for (int i = 0; i < damaged_spring_record[0]; i++) {
            if (spring_state[i] == OPERATIONAL) {
                return 0;
            }
        }
        if (damaged_spring_record[0]               == len     ||
            spring_state[damaged_spring_record[0]] != DAMAGED) {
            return 1;
        }
    }
    return 0;
}

//******************************************************************************
int get_valid_spring_record_combinations(char* spring_state, int* damaged_spring_record) {
    if (!damaged_spring_record) {
        if (strchr(spring_state, DAMAGED) != NULL) {
            return 0;
        } else {
            return 1;
        }
    }

    if (strlen(spring_state) == 0) {
        if (!damaged_spring_record) {
            return 1;
        } else {
            return 0;
        }
    }

    int total_combinations = 0;

    // if "." or "?"
    if (spring_state[0] == OPERATIONAL || spring_state[0] == UNKNOWN) {
        total_combinations += get_valid_spring_record_combinations(spring_state + 1, damaged_spring_record);
    }

    // if "#" or "?"
    if (spring_state[0] == DAMAGED || spring_state[0] == UNKNOWN) {
        if (is_valid_condition(spring_state, damaged_spring_record)) {
            total_combinations += get_valid_spring_record_combinations(spring_state + damaged_spring_record[0] + 1, damaged_spring_record + 1);
        }
    }

    return total_combinations;
}

//******************************************************************************
int sum_spring_record_combinations(char* spring_condition_records) {
    int total_combinations = 0;
    char* spring_record;
    char* record_ptr = strtok(spring_condition_records, "\n");
    while (record_ptr != NULL) {
        spring_record = strdup(record_ptr);
        char* spring_state = strtok(spring_record, " ");
        char* damaged_spring_record_str = strtok(NULL, " ");
        char* token;
        int damaged_spring_record[3];
        int i = 0;
        while ((token = strtok(NULL, ",")) != NULL) {
            damaged_spring_record[i++] = atoi(token);
        }
        total_combinations += get_valid_spring_record_combinations(spring_state, damaged_spring_record);
        free(spring_record);
        record_ptr = strtok(NULL, "\n");
    }
    return total_combinations;
}

//******************************************************************************
int sum_spring_record_combinations_unfold(char* spring_condition_records) {
    int   total_combinations = 0;
    char* spring_record;
    char* record_ptr         = strtok(spring_condition_records, "\n");

    while (record_ptr != NULL) {
              spring_record             = strdup(record_ptr);
        char* spring_state              = strtok(spring_record, " ");
        char* damaged_spring_record_str = strtok(NULL, " ");
        char* token;
        int   damaged_spring_record[3];

        int i = 0;
        while ((token = strtok(NULL, ",")) != NULL) {
            damaged_spring_record[i++] = atoi(token);
        }

        for (int j = 0; j < 5; j++) {
            total_combinations += get_valid_spring_record_combinations(spring_state, damaged_spring_record);
        }

        free(spring_record);
        record_ptr = strtok(NULL, "\n");
    }
    return total_combinations;
}


//******************************************************************************
int main() {
    char* filename = "data.txt";
    FILE* file     = fopen(filename, "r");

    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return 1;
    }

    char* spring_condition_records = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&spring_condition_records, &len, file)) != -1) {
        // int sum = sum_spring_record_combinations(spring_condition_records);
        int sum = sum_spring_record_combinations_unfold(spring_condition_records);
        printf("Total valid combinations: %d\n", sum);
    }

    free(spring_condition_records);
    fclose(file);

    return 0;
}
