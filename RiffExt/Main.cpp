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

auto processRiff(std::ifstream& file, RIFF::RiffData riff, Args::Options options) {
    auto filenameShort = std::filesystem::path(options.inFile).filename().string();
    auto fileSize = std::filesystem::file_size(options.inFile);

    auto startPos = file.tellg();
    // probably a valid RIFF
    std::print("RIFF @ 0x{:X}", (size_t)file.tellg());
    file.seekg(sizeof riff, std::ios::cur);

    if (options.scan) {
        if (riff.isWav()) {
            auto fmtChunk = RIFF::Util::GetBytes<RIFF::FmtChunk>(file);
            file.seekg(fmtChunk.size - (sizeof fmtChunk - sizeof RIFF::ChunkHeader), std::ios::cur);
            RIFF::ChunkHeader chunk;
            do {
                chunk = RIFF::ReadChunk(file);
                assert(chunk.validId());
            } while (chunk.idName() != RIFF::Chunks::data);
            std::print(": {}, {}, {}", fmtChunk.duration(chunk.size), formatFileSize(riff.size), fmtChunk.description());
        } else {
            std::print(" - Unknown chunk identifier {}", riff.chunkIdName());
        }
    }
    std::println();

    if (options.dump) {
        auto outFileName = std::format("{}_{}.dat", filenameShort, (size_t)startPos);
        auto outFileSize = RIFF::Chunks::IdSize + 4 + riff.size;
        dumpFile(file, startPos, outFileSize, outFileName);
    }
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
            const char* bufferPtr = buffer + i;
            if (auto riff = RIFF::PeekRIFF(bufferPtr)) {
                if (riff->size > fileSize) { // definitely not a valid RIFF
                    continue;
                }

                if (options.strictWav && !riff->isWav()) {
                    std::println("Ignoring RIFF with unknown chunk id {}", riff->chunkIdName());
                    continue;
                }

                auto oldPos = file.tellg();
                size_t end = file.tellg() == std::ifstream::pos_type(-1) ? fileSize : (size_t)file.tellg();
                file.seekg(end - numRead + i, std::ios::beg);
                processRiff(file, *riff, options);
                file.seekg(oldPos, std::ios::cur);
            }
        }
    }
	return 0;
}