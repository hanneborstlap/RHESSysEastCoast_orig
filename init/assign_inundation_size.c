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
