#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void advanceFP(FILE* f, int bytes) {
    for (int i = 0; i < bytes; i++) {
        fgetc(f);
    }
}

int main(int argc, char** argv) {

    if (argc < 2) {
        return 1;
    }

    FILE* f = fopen(argv[1], "rb");
    if (f == NULL) {
        puts("File doesn't exist");
        return 1;
    }

    long int minSize = -1;
    long int index = -1;

    if (argc > 3) {
        if (strcmp(argv[2], "-m") == 0) {
            minSize = strtol(argv[3], NULL, 10);
            if (minSize <= 0) {
                puts("Invalid argument for -m");
                return 1;
            }
        }
        if (strcmp(argv[2], "-i") == 0) {
            index = strtol(argv[3], NULL, 10);
            if (index <= 0) {
                puts("Invalid argument for -i");
                return 1;
            }
        }
    }

    int nfiles = 0;
    char buf[9];

    for (int ch = fgetc(f); ch != EOF; ch = fgetc(f)) {

        if (ch != 'R') {
            continue;
        }

        if (fgets(buf, 4, f) == NULL) goto cleanup;

        // RIFF found
        if (strncmp(buf, "IFF", 3) != 0) {
            continue;
        }

        uint32_t filesize;
        if (fread(&filesize, 1, 4, f) < 4) goto cleanup;

        // make sure it's actually a RIFF - check for WAVEfmt
        if (fgets(buf, 9, f) == NULL) goto cleanup;
        if (strncmp(buf, "WAVEfmt ", 8) != 0) {
            continue;
        }

        nfiles++;

        if ((index > 0 && index != nfiles) || (minSize > 0 && minSize > filesize)) {
            advanceFP(f, filesize);
            continue;
        }
        char outname[32] = {0};
        snprintf(outname, 31, "%i.wem", nfiles);

        FILE* out = fopen(outname, "wb");
        fwrite("RIFF", 1, 4, out);
        fwrite(&filesize, 1, 4, out);
        fwrite("WAVEfmt ", 1, 8, out);

        // WAVEfmt
        filesize -= 8;

        char* buffer = malloc(filesize);
        fread(buffer, 1, filesize, f);
        fwrite(buffer, 1, filesize, out);
        fclose(out);

    }
    fclose(f);
    printf("%i files extracted\n", nfiles);
    return 0;

cleanup:
    fclose(f);
    puts("An error occurred reading the file.");
    return 1;
}
