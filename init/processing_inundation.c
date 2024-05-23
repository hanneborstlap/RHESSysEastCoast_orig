#include <stdio.h>
#include <stdlib.h>

struct date {
    int month;
    int day;
    int year;
};

struct date createDateFromDateString(const char* dateString) {
    struct date result;
    char* copy = strdup(dateString); // Duplicate the input string
    if (copy == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    char* token = strtok(copy, "/");
    if (token != NULL) {
        result.month = atoi(token);
        token = strtok(NULL, "/");
    }
    if (token != NULL) {
        result.day = atoi(token);
        token = strtok(NULL, "/");
    }
    if (token != NULL) {
        result.year = atoi(token);
    }

    free(copy);
    return result;
}

int readInundationDepths(const char *depth_filename, const char *dur_filename, const char *date_filename, const char *patchID_filename, double **depths, int **durs, char ***dates, int **patchIDs) {
    FILE *file_depth = fopen(depth_filename, "r");
    if (file_depth == NULL) {
        fprintf(stderr, "Error: Could not open file '%s'\n", depth_filename);
        return -1;
    }

    FILE *file_dur = fopen(dur_filename, "r");
    if (file_dur == NULL) {
        fprintf(stderr, "Error: Could not open file '%s'\n", dur_filename);
        fclose(file_depth);
        return -1;
    }

    FILE *file_date = fopen(date_filename, "r");
    if (file_date == NULL) {
        fprintf(stderr, "Error: Could not open file '%s'\n", date_filename);
        fclose(file_depth);
        fclose(file_dur);
        return -1;
    }

    FILE *file_patchID = fopen(patchID_filename, "r");
    if (file_patchID == NULL) {
        fprintf(stderr, "Error: Could not open file '%s'\n", patchID_filename);
        fclose(file_depth);
        fclose(file_dur);
        fclose(file_date);
        return -1;
    }

    int count = 0;
    double temp_in;
    while (fscanf(file_depth, "%lf", &temp_in) == 1) {
        count++;
    }

    if (!feof(file_depth) && ferror(file_depth)) {
        fprintf(stderr, "Error reading file '%s'\n", depth_filename);
        fclose(file_depth);
        fclose(file_dur);
        fclose(file_date);
        fclose(file_patchID);
        return -1;
    }

    rewind(file_depth);

    *depths = (double *)malloc(count * sizeof(double));
    *durs = (int *)malloc(count * sizeof(int));
    *dates = (char**)malloc(count * sizeof(char*));
    *patchIDs = (int *)malloc(count * sizeof(int));

    for (int i = 0; i < count; ++i) {
        (*dates)[i] = (char*)malloc(11 * sizeof(char)); // Assuming date format "YYYY-MM-DD"
        //snprintf((*dates)[i], 11, "2023/05/0%d", i + 1); // Example dates
    }

    if (*depths == NULL || *durs == NULL || *dates == NULL || *patchIDs == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file_depth);
        fclose(file_dur);
        fclose(file_date);
        fclose(file_patchID);
        return -1;
    }



    int ii;
    for (ii = 0; ii < count; ii++) {
        if (fscanf(file_dur, "%d", &(*durs)[ii]) != 1) {
            fprintf(stderr, "Error reading duration at index %d\n", ii);
            break;
        }

        if (fscanf(file_date, "%11s", (*dates)[ii]) != 1) {
           fprintf(stderr, "Error reading date at index %d\n", ii);
           break;
        }

        if (fscanf(file_depth, "%lf", &(*depths)[ii]) != 1) {
            fprintf(stderr, "Error reading depth at index %d\n", ii);
            break;
        }

        if (fscanf(file_patchID, "%d", &(*patchIDs)[ii]) != 1) {
            fprintf(stderr, "Error reading patch ID at index %d\n", ii);
            break;
        }
        
    }
    
    // printf("Date: %s\n", (*dates[4]));
    
    fclose(file_depth);
    fclose(file_dur);
    fclose(file_date);
    fclose(file_patchID);

    // printf("%d values scanned\n", ii);
    // printf("%d count\n", count);
    return ii; // return the number of values successfully read
}



