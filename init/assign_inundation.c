#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rhessys.h"

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
