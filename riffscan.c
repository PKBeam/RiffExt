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

    int simple = 0;
    if (argc > 2 && strcmp(argv[2], "-s") == 0) {
        simple = 1;
        puts("Using simple output");
    }

    FILE* f = fopen(argv[1], "rb");
    if (f == NULL) {
        puts("File doesn't exist");
        return 1;
    }

    int fsTotal = 0;
    int fsMax = 0;
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

        fsTotal += filesize;
        if (fsMax < filesize) {
            fsMax = filesize;
        }
        if (simple) {
            printf(
                "file %i | size: %.01lfkB\n",
                nfiles,
                (double)filesize / 1000.0
            );
            advanceFP(f, filesize - 8);
            continue;
        }

        uint32_t fmtSize;
        uint16_t audioFormat;
        uint16_t numChannels;
        uint32_t sampleRate;
        uint32_t byteRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample;
        uint32_t dataSize;

        if (fread(&fmtSize, 1, 4, f) < 4) goto cleanup;
        if (fread(&audioFormat, 1, 2, f) < 2) goto cleanup;
        if (fread(&numChannels, 1, 2, f) < 2) goto cleanup;
        if (fread(&sampleRate, 1, 4, f) < 4) goto cleanup;
        if (fread(&byteRate, 1, 4, f) < 4) goto cleanup;
        if (fread(&blockAlign, 1, 2, f) < 2) goto cleanup;
        if (fread(&bitsPerSample, 1, 2, f) < 2) goto cleanup;

        // unknown cases
        if (audioFormat != 0xFFFF && (bitsPerSample == 0 || numChannels == 0 || sampleRate == 0)) {
            printf(
                "file %i | format: 0x%04X, size: %.01lfkB, length: N/A, bitrate: %.01lfkbps, sample rate: %.01lfkHz, channels: %i, bits/sample: %i\n",
                nfiles,
                (int)audioFormat,
                (double)filesize / 1000.0,
                (double)byteRate * 8.0 / 1000.0,
                (double)sampleRate / 1000.0,
                (int)numChannels,
                (int)bitsPerSample
            );
            advanceFP(f, filesize - 32);
            continue;
        }

        double numSamples;
        dataSize = fmtSize - 16;

        // wwise audio stores this differently
        if (audioFormat == 0xFFFF) {
            advanceFP(f, 8);
            uint32_t samples;
            if (fread(&samples, 1, 4, f) < 4) goto cleanup;
            dataSize -= 12;
            numSamples = (double)samples;
        }

        // read chunks until we find data chunk
        do {
            advanceFP(f, dataSize);
            if (fgets(buf, 5, f) == NULL) goto cleanup;
            if (fread(&dataSize, 1, 4, f) < 4) goto cleanup;
        } while (strncmp(buf, "data", 4) != 0);

        if (audioFormat != 0xFFFF) {
            numSamples = ((double)dataSize * 8.0 / numChannels) / bitsPerSample;
        }
        double length = ((double)numSamples) / ((double)sampleRate);
        printf(
            "file %i | format: 0x%04X, size: %.01lfkB, length: %i:%02i, bitrate: %.01lfkbps, sample rate: %.01lfkHz, channels: %i\n",
            nfiles,
            (int)audioFormat,
            (double)filesize / 1000.0,
            (int)(length / 60),
            ((int)length) % 60,
            (double)byteRate * 8.0 / 1000.0,
            (double)sampleRate / 1000.0,
            (int)numChannels
        );
        advanceFP(f, dataSize);
    }
    fclose(f);
    printf(
        "\n"
        "%i files scanned\n"
        "largest file: %.01lfkB\n"
        "avg. filesize: %.01lfkB\n",
        nfiles,
        (double)fsMax/1000.0,
        (double)fsTotal/nfiles/1000.0
    );
    return 0;

cleanup:
    fclose(f);
    puts("An error occurred reading the file.");
    return 1;
}
