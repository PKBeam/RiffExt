#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char** argv) {

    if (argc < 2) {
        return 1;
    }
    FILE* f = fopen(argv[1], "rb");
    int ch = fgetc(f);
    int nfiles = 0;
    int c = 1;
    while (ch != EOF) {
        // check for R
        if (ch == 'R') {
            //puts("R found");
            char buf[4];
            if (fgets(buf, 4, f) == NULL) {
                goto cleanup;
            }

            // RIFF found
            if (strncmp(buf, "IFF", 3) == 0) {
                //puts("RIFF found");
                uint32_t filesize;
                if (fread(&filesize, 1, 4, f) < 4) {
                    goto cleanup;
                }

                char outname[32];
                snprintf(outname, 31, "%i.wem", ++nfiles);

                FILE* out = fopen(outname, "w");
                fwrite("RIFF", 1, 4, out);
                fwrite(&filesize, 1, 4, out);
                for (int i = 0; i < filesize; i++) {
                    fputc(fgetc(f), out);
                }
                fclose(out);
            }
        }
        ch = fgetc(f);
    }

cleanup:
    fclose(f);
    return 0;
}
