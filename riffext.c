#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char** argv) {

    if (argc < 2) {
        return 1;
    }

    FILE* f = fopen(argv[1], "rb");
    if (f == NULL) {
        puts("File doesn't exist");
        return 1;
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

        printf("file %i | filesize: %.01lfkB,\n", nfiles, (double) filesize/1000.0);
    }

    fclose(f);
    return 0;

cleanup:
    fclose(f);
    puts("An error occurred reading the file.");
    return 1;
}
