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

auto scanRiff(
    std::ifstream& file, 
    std::string_view filenameShort, 
    RIFF::RiffData riff, 
    size_t startOffset, 
    Args::Options options
) -> std::optional<double> {
    if (!riff.isWav()) {
        if (options.verbose) {
            std::print("Unknown chunk identifier {}", riff.chunkIdName());
        }
        return std::nullopt;
    }
    auto fmtChunk = RIFF::Util::GetBytes<RIFF::FmtChunk>(file);
    // if the fmt chunk is larger than the standard size, skip the extra data
    auto prevPos = file.tellg();
    auto fileSize = std::filesystem::file_size(options.inFile);
    if ((size_t)prevPos + fmtChunk.size > fileSize) {
        if (options.verbose) {
            std::print("Bad fmt chunk size found, skipping scan...");
        }
        return std::nullopt;
    }
    file.seekg(fmtChunk.size - (sizeof fmtChunk - sizeof RIFF::ChunkHeader), std::ios::cur);
    std::vector<RIFF::ChunkHeader> chunks;
    do {
        assert(!file.eof());
        auto chunk = RIFF::ReadChunk(file);
        if (!chunk.validId()) {
            if (options.verbose) {
                std::print("Unknown chunk ID found, skipping scan...");
            }
            return std::nullopt;
        }
        chunks.push_back(chunk);
    } while (chunks.back().idName() != RIFF::Chunks::data);
    auto duration = fmtChunk.duration(chunks.back().size);
    if (!options.minDuration || duration > *options.minDuration) {
        std::println(
            "{}, RIFF @ 0x{:X}, {:>5}, {:>10}, {}", 
            filenameShort, 
            startOffset, 
            fmtChunk.durationString(duration), 
            formatFileSize(riff.size), 
            fmtChunk.description()
        );
        if (options.verbose && !chunks.empty()) {
            std::print("  ");
            for (auto chunk : chunks) {
                std::print("(Chunk {}, size 0x{:X}) ", chunk.idName(), chunk.size);
            }
        }
    }
    return duration;
}

auto processRiff(std::ifstream& file, std::string_view filenameShort, RIFF::RiffData riff, Args::Options options) {
    auto fileSize = std::filesystem::file_size(options.inFile);

    auto startPos = file.tellg();
    // probably a valid RIFF
    file.seekg(sizeof riff, std::ios::cur);

    std::optional<double> duration;
    if (options.scan) {
        duration = scanRiff(file, filenameShort, riff, (size_t)startPos, options);
    }

    if (options.dump) {
        if (options.minDuration && (!duration || *duration > *options.minDuration)) {
            return;
        }
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
    //std::println("File {} has size {:.2f} KiB", filenameShort, fileSize / 1024.f);

    // read file in chunks
    file.seekg(RIFF::Chunks::IdSize, std::ios::cur);
    while (!file.eof()) {
        file.seekg(-1 * RIFF::Chunks::IdSize, std::ios::cur);
        file.read(buffer, BUFFER_SIZE);
        if (file.eof()) { // hitting eof will disable any seeking, so clear it and remember to break later
            file.clear();
        }
        auto numRead = file.gcount();
        for (auto i = 0; i < numRead; ++i) {
            // check for prefix
            if (auto riff = RIFF::PeekRIFF(buffer + i)) {
                if (riff->size > fileSize) { // definitely not a valid RIFF
                    continue;
                }

                if (options.strictWav && !riff->isWav()) {
                    std::println("Ignoring RIFF with unknown chunk id {}", riff->chunkIdName());
                    continue;
                }

                // align the file to the start of the RIFF
                size_t end = file.tellg() == std::ifstream::pos_type(-1) ? fileSize : (size_t)file.tellg();
                auto oldPos = file.tellg();
                file.seekg(end - numRead + i, std::ios::beg);
                processRiff(file, filenameShort, *riff, options);
                // when finished, skip to the end of the RIFF
                file.seekg(oldPos, std::ios::beg);
                auto newPos = (size_t)oldPos + riff->chunkSize();
                i += newPos - oldPos - 1;
            }
        }
        if (numRead < BUFFER_SIZE) {
            break;
        }
    }
	return 0;
}