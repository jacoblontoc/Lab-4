#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int sum_array(const int *array, size_t count, int *overflow);

static int read_integer(FILE *file, int *value) {
    char *line = NULL;
    size_t capacity = 0;
    ssize_t length = getline(&line, &capacity, file);
    int valid = 0;

    if (length >= 0 && !memchr(line, '\0', (size_t)length)) {
        char *end;
        errno = 0;
        long number = strtol(line, &end, 10);
        if (end != line && errno == 0 && number >= INT_MIN && number <= INT_MAX) {
            while (isspace((unsigned char)*end)) {
                end++;
            }
            if (*end == '\0') {
                *value = (int)number;
                valid = 1;
            }
        }
    }
    free(line);
    return valid;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <data-file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror(argv[1]);
        return 1;
    }

    int *array = NULL;
    int count;
    int status = 1;
    if (!read_integer(file, &count) || count < 0 ||
        (size_t)count > SIZE_MAX / sizeof(*array)) {
        fprintf(stderr, "Invalid integer count.\n");
        goto cleanup;
    }
    if (count > 0) {
        array = malloc((size_t)count * sizeof(*array));
        if (!array) {
            fprintf(stderr, "Could not allocate the integer array.\n");
            goto cleanup;
        }
    }

    for (int i = 0; i < count; i++) {
        if (!read_integer(file, &array[i])) {
            fprintf(stderr, "Missing or invalid integer on line %zu.\n", (size_t)i + 2);
            goto cleanup;
        }
    }
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (!isspace((unsigned char)ch)) {
            fprintf(stderr, "Extra data after the declared integers.\n");
            goto cleanup;
        }
    }
    if (ferror(file)) {
        fprintf(stderr, "Could not read the data file.\n");
        goto cleanup;
    }

    int overflow;
    int sum = sum_array(array, (size_t)count, &overflow);
    if (overflow) {
        fprintf(stderr, "Sum is outside the signed 32-bit range.\n");
        goto cleanup;
    }
    status = printf("Sum: %d\n", sum) < 0;

cleanup:
    free(array);
    fclose(file);
    return status;
}
