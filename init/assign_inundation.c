// file_processing.c
#include <stdio.h>
#include <stdlib.h>
#include "file_processing.h"

int readInundationDepths(const char *filename, float depths[], int max_values) {

FILE *file_ex_inundation_depth = fopen("inundation/inundation_depth.txt", "r");
    if (file_ex_inundation_depth == NULL) {
    	fprintf(stderr, "Error: Could not open file 'inundation/inundation_depth.txt'\n");
  	exit(EXIT_FAILURE);
    }

    FILE *file_ex_inundation_dur = fopen("inundation/inundation_duration.txt", "r");
    if (file_ex_inundation_dur == NULL) {
 	fprintf(stderr, "Error: Could not open file 'inundation/inundation_duration.txt'\n");
        exit(EXIT_FAILURE);
    }

    FILE *file_ex_inundation_date = fopen("inundation/inundation_date.txt", "r");
    if (file_ex_inundation_date == NULL) {
      fprintf(stderr, "Error: Could not open file 'inundation/inundation_date.txt'\n");
      exit(EXIT_FAILURE);
    }

    FILE *file_ex_inundation_patchID = fopen("inundation/inundation_patchID.txt", "r");
    if (file_ex_inundation_patchID == NULL) {
      fprintf(stderr, "Error: Could not open file 'inundation/inundation_patchID.txt'\n");
      exit(EXIT_FAILURE);
    }

    int count_t = 0;
    double temp_in;

    // printf("READ IN FILES\n");

    while (fscanf(file_ex_inundation_depth, "%lf", &temp_in) == 1) {
        count_t++;
    }

    // printf("%d FILE LENGTH\n", count_t);

    rewind(file_ex_inundation_depth);

    int ex_inundation_dur[count_t];
    double ex_inundation_depth[count_t];
    char ex_inundation_date[count_t][11];
    int ex_inundation_patchID[count_t];
	    
    int ii = 0;
    double last_value = 0.0;

    for (ii = 0; ii < count_t; ii++)
    {
        
     // printf("Attempting to read at index %d\n", ii);
    
     if (fscanf(file_ex_inundation_dur, "%d", &ex_inundation_dur[ii]) != 1) {
        fprintf(stderr, "Error reading ex_inundation_dur at index %d\n", ii);
        break;
    }
    
     if (fscanf(file_ex_inundation_date, "%10s", &ex_inundation_date[ii]) != 1) {
        fprintf(stderr, "Error reading ex_inundation_date at index %d\n", ii);
        break;
    }
        

     if (fscanf(file_ex_inundation_depth, "%lf", &ex_inundation_depth[ii]) != 1) {
        fprintf(stderr, "Error reading ex_inundation_depth at index %d\n", ii);
        break;
    }
    
     if (fscanf(file_ex_inundation_patchID, "%d", &ex_inundation_patchID[ii]) != 1) {
        fprintf(stderr, "Error reading ex_inundation_patchID at index %d\n", ii);
        break;
     }
        
    // printf("%s ex_inundation_date \n", ex_inundation_date[ii]);
        
    }
    
    printf("%d FILES SCANNED\n");

}
