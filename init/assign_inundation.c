#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rhessys.h"

int countValues(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return -1; // Indicate failure
    }

    int count = 0;
    double temp_in;
    while (fscanf(file, "%lf", &temp_in) == 1) {
        count++;
    }

    if (!feof(file) && ferror(file)) {
        fprintf(stderr, "Error reading file '%s'\n", filename);
        fclose(file);
        return -1; // Indicate failure
    }

    fclose(file);
    return count;
}

int readInundationDepths(const char *depth_filename, const char *dur_filename, const char *date_filename, const char *patchID_filename, double depths[], int durs[], char dates[][11], int patchIDs[], int max_values) {
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

    int count_t = 0;
    double temp_in;

    // count the number of values in the depth file (representative of all other files as well) 
    while (fscanf(file_depth, "%lf", &temp_in) == 1 && count_t < max_values) {
        count_t++;
    }

    if (!feof(file_depth) && ferror(file_depth)) {
        fprintf(stderr, "Error reading file '%s'\n", depth_filename);
        fclose(file_depth);
        fclose(file_dur);
        fclose(file_date);
        fclose(file_patchID);
        return -1;
    }

    // file_depth was read and used to assign sizes of arrays, need to rewind to read in values again later
    rewind(file_depth);

    int ii = 0;
    for (ii = 0; ii < count_t; ii++) {
        if (fscanf(file_dur, "%d", &durs[ii]) != 1) {
            fprintf(stderr, "Error reading duration at index %d\n", ii);
            break;
        }

	// date is assumed to be in MM/DD/YYYY format 
        if (fscanf(file_date, "%10s", dates[ii]) != 1) {
            fprintf(stderr, "Error reading date at index %d\n", ii);
            break;
        }

        if (fscanf(file_depth, "%lf", &depths[ii]) != 1) {
            fprintf(stderr, "Error reading depth at index %d\n", ii);
            break;
        }

        if (fscanf(file_patchID, "%d", &patchIDs[ii]) != 1) {
            fprintf(stderr, "Error reading patch ID at index %d\n", ii);
            break;
        }
    }

    // Close all files
    fclose(file_depth);
    fclose(file_dur);
    fclose(file_date);
    fclose(file_patchID);

    printf("%d values scanned\n", ii);
    return ii; // return the number of values successfully read
}


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

    const char *depth_filename = "inundation/inundation_depth.txt";
    const char *dur_filename = "inundation/inundation_duration.txt";
    const char *date_filename = "inundation/inundation_date.txt";
    const char *patchID_filename = "inundation/inundation_patchID.txt";

// Determine the number of records in the depth file
    int max_values = 500; 

    if (max_values < 0) {
        fprintf(stderr, "Failed to determine the number of records.\n");
        return 1;
    }

    // Allocate arrays based on the number of values
    double *ex_inundation_depth = (double *)malloc(max_values * sizeof(double));
    int *ex_inundation_dur = (int *)malloc(max_values * sizeof(int));
    char (*ex_inundation_date)[11] = (char (*)[11])malloc(max_values * sizeof(*ex_inundation_date));
    int *ex_inundation_patchID = (int *)malloc(max_values * sizeof(int));
    struct date *inundation_date_f = malloc(11 * sizeof(struct date));

    if (ex_inundation_depth == NULL || ex_inundation_dur == NULL || ex_inundation_date == NULL || ex_inundation_patchID == NULL) {
        fprintf(stderr, "Error allocating memory.\n");
        free(ex_inundation_depth);
        free(ex_inundation_dur);
        free(ex_inundation_date);
        free(ex_inundation_patchID);
        return 1;
    }

    // Read the data from files into arrays
    int num_records = readInundationDepths(depth_filename, dur_filename, date_filename, patchID_filename, ex_inundation_depth, ex_inundation_dur, ex_inundation_date, ex_inundation_patchID, max_values);

    if (num_records < 0) {
        fprintf(stderr, "Failed to read data from files.\n");
        free(ex_inundation_depth);
        free(ex_inundation_dur);
        free(ex_inundation_date);
        free(ex_inundation_patchID);
        return 1;
    }

    printf("Successfully read %d records.\n", num_records);
    
    int jj = 0; 
    for (int jj = 0; jj < max_values; jj++) {
	struct date inundation_date_f[jj] = createDateFromDateString(ex_inundation_date[jj]);
    }
