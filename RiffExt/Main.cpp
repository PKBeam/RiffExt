#include "Args.h"
#include "RIFF.h"

#include <cstdint>
#include <print>
#include <filesystem>
#include <fstream>

constexpr auto BUFFER_SIZE = 16z * 1024 * 1024; // 16 MiB
char buffer[BUFFER_SIZE];

auto formatFileSize(size_t size_) -> std::string {
    constexpr auto divisor = 1024;
    double size = size_;
    std::array<std::string_view, 4> units = {"B", "KiB", "MiB", "GiB"};
    
    int i = 0;
    while (i < units.size() && size >= divisor) {
        i++;
        size /= divisor;
    }

    return std::format("{:.2f} {}", size, units[i]);
}

void dumpFile(std::ifstream& inFile, size_t offset, size_t size, std::string_view outFileName) {
    auto oldPos = inFile.tellg();

    std::ofstream outFile{outFileName.data(), std::ios::binary};
    inFile.seekg(offset, std::ios::beg);
    std::copy_n(std::istreambuf_iterator<char>(inFile), size,
                std::ostreambuf_iterator<char>(outFile));

    inFile.seekg(oldPos);
}

int main(int argc, const char* argv[]) {
	auto maybeOptions = Args::Parse(argc, argv);
    if (!maybeOptions) {
        exit(1);
    }
    auto options = *maybeOptions;

    std::ifstream file{options.inFile.data(), std::ios::binary};
    auto filenameShort = std::filesystem::path(options.inFile).filename().string();
    auto fileSize = std::filesystem::file_size(options.inFile);
    std::println("File {} has size {:.2f} KiB", filenameShort, fileSize / 1024.f);

    // read file in chunks
    file.seekg(RIFF::Chunks::IdSize, std::ios::cur);
    unsigned numRiffs = 0;
    while (!file.eof()) {
        file.seekg(-1 * RIFF::Chunks::IdSize, std::ios::cur);
        file.read(buffer, BUFFER_SIZE);
        auto numRead = file.gcount();
        for (auto i = 0; i < numRead; ++i) {
            // check for prefix
            if (auto riff = RIFF::FindRIFF(buffer + i)) {

                if (riff->size > fileSize) { // definitely not a valid RIFF
                    continue;
                }

                if (options.strictWav && !riff->isWav()) {
                    std::println("Ignoring RIFF with unknown chunk id {}", riff->chunkIdName());
                    continue;
                }

                // probably a valid RIFF

                ++numRiffs;
                size_t end = file.tellg() == std::ifstream::pos_type(-1) ? fileSize : (size_t) file.tellg();
                size_t pos = end - (numRead - i);
                std::println("RIFF {} at offset 0x{:X} with size {}", numRiffs, pos, formatFileSize(riff->size));

                if (options.scan) {
                    if (riff->isWav()) {
                        RIFF::FmtChunk fmtChunk{buffer + i + sizeof *riff};
                        std::println("  {}", fmtChunk.description());
                    } else {
                        std::println("  Unknown chunk identifier {}", riff->chunkIdName());
                    }
                }

                if (options.dump) {
                    auto outFileName = std::format("{}_{}.dat", filenameShort, numRiffs);
                    auto outFileSize = RIFF::Chunks::IdSize + 4 + riff->size;
                    dumpFile(file, pos, outFileSize, outFileName);
                }
            }
        }
    }
	return 0;
}