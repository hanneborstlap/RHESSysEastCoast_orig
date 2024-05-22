#include <stdio.h>
#include <stdlib.h>

int readInundationDepths(const char *depth_filename, const char *dur_filename, const char *date_filename, const char *patchID_filename, double **depths, int **durs, char ***dates, int **patchIDs);

struct date createDateFromDateString(const char* dateString) {
    // printf("STARTING CREATEDATE \n");
    struct date result;
    // printf("MAKE COPY \n");
    // char* copy = strdup(dateString); // Make a copy of the input string
 
      // Calculate the length of the input string
    size_t length = strlen(dateString);

    // Allocate memory for a copy of the input string and null terminator
    char* copy = malloc(length + 1); // Add 1 for the null terminator
    if (copy == NULL) {
        perror("Failed to allocate memory");
        printf("EMPTRY STRING \n");
        exit(EXIT_FAILURE);
    }

    // Copy the input string and null terminator
    strncpy(copy, dateString, length);
    copy[length] = '\0'; // Ensure null termination

    // printf("Tokenizing date string: %s\n", copy);

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

void processInundationData() {
    const char *depth_filename = "inundation/inundation_depth.txt";
    const char *dur_filename = "inundation/inundation_dur.txt";
    const char *date_filename = "inundation/inundation_date.txt";
    const char *patchID_filename = "inundation/inundation_patchID.txt";

    double *depths;
    int *durs;
    char **dates;
    int *patchIDs;

    int num_values = readInundationDepths(depth_filename, dur_filename, date_filename, patchID_filename, &depths, &durs, &dates, &patchIDs);

    if (num_values == -1) {
        fprintf(stderr, "Error reading data from files.\n");
        return;
    }

    // Print the read data
    for (int i = 0; i < num_values; i++) {
        printf("Depth: %lf, Duration: %d, Date: %s, PatchID: %d\n", depths[i], durs[i], dates[i], patchIDs[i]);
    }

    // Free allocated memory
    for (int i = 0; i < num_values; i++) {
        free(dates[i]);
    }
    free(dates);
    free(depths);
    free(durs);
    free(patchIDs);
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
    *dates = (char **)malloc(count * sizeof(char *));
    *patchIDs = (int *)malloc(count * sizeof(int));

    if (*depths == NULL || *durs == NULL || *dates == NULL || *patchIDs == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file_depth);
        fclose(file_dur);
        fclose(file_date);
        fclose(file_patchID);
        return -1;
    }

    for (int i = 0; i < count; i++) {
        (*dates)[i] = (char *)malloc(11 * sizeof(char));
        if ((*dates)[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            fclose(file_depth);
            fclose(file_dur);
            fclose(file_date);
            fclose(file_patchID);
            for (int j = 0; j < i; j++) {
                free((*dates)[j]);
            }
            free(*depths);
            free(*durs);
            free(*dates);
            free(*patchIDs);
            return -1;
        }
    }

    int ii;
    for (ii = 0; ii < count; ii++) {
        if (fscanf(file_dur, "%d", &(*durs)[ii]) != 1) {
            fprintf(stderr, "Error reading duration at index %d\n", ii);
            break;
        }

        if (fscanf(file_date, "%10s", (*dates)[ii]) != 1) {
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

    fclose(file_depth);
    fclose(file_dur);
    fclose(file_date);
    fclose(file_patchID);

    printf("%d values scanned\n", ii);
    return ii; // return the number of values successfully read
}
