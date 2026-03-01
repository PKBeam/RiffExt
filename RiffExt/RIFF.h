#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <optional>
#include <string_view>

namespace RIFF {

namespace Util {
    static auto ToString(uint32_t field) -> std::string {
        char buf[5];
        std::memcpy(buf, &field, 4);
        buf[4] = '\0';
        return buf;

    }

    template<typename T>
    auto GetBytes(std::ifstream& file) -> T {
        char buf[sizeof(T) + 1];
        file.get(buf, sizeof(T) + 1);
        T t;
        std::memcpy(&t, buf, sizeof(T));
        return t;
    }

    template<typename T>
    auto GetBytes(const char*& data) -> T {
        T t;
        std::memcpy(&t, data, sizeof(T));
        data += sizeof(T);
        return t;
    }
}

namespace Chunks {
    constexpr auto IdSize = 4z;
    using Identifier = char[IdSize + 1];
    
    constexpr Identifier RIFF = "RIFF";
    constexpr Identifier WAVE = "WAVE";
    constexpr Identifier fmt  = "fmt ";
    constexpr Identifier data = "data";
}

struct ChunkHeader {
    uint32_t id;
    uint32_t size;

    auto chunkSize() const -> size_t {
        return 8 + size;
    }

    auto idName() const -> std::string {
        return Util::ToString(id);
    }

    auto validId() -> bool {
        auto name = idName();
        return std::all_of(name.begin(), name.end(), 
            [](char c) { return std::isalnum(c) || c == ' '; }
       );
    }
};
static_assert(sizeof ChunkHeader == 8);

struct FmtChunk : ChunkHeader {
    uint16_t format;
    uint16_t channels;
    uint32_t frequency;
    uint32_t bytesPerSec;
    uint16_t bytesPerBlock;
    uint16_t bitsPerSample;

    auto formatName() const -> std::string {
        switch (format) {
        case 0x0001: return "PCM";
        case 0x0003: return "Float";
        case 0xFFFF: return "WWise";
        default: return std::format("Unknown ({})", format);
        }
    }

    auto duration(uint32_t dataSize) const -> std::string {
        unsigned totalSeconds = dataSize / bytesPerSec;
        auto minutes = totalSeconds / 60;
        auto seconds = totalSeconds % 60;
        return std::format("{}:{:02d}", minutes, seconds);
    }

    auto description() const -> std::string {
        return std::format("{} Hz, {} channel, {} format", frequency, channels, formatName());
    }
};
static_assert(sizeof FmtChunk == 24);

struct RiffData : ChunkHeader {
    uint32_t chunkId;

    auto chunkIdName() const -> std::string {
        return Util::ToString(chunkId);
    }

    auto isWav() const -> bool {
        return chunkIdName() == RIFF::Chunks::WAVE;
    }
};
static_assert(sizeof RiffData == 12);

auto PeekRIFF(const char* data) -> std::optional<RiffData> {
    auto riffData = Util::GetBytes<RiffData>(data);

    if (riffData.idName() != Chunks::RIFF) {
        return std::nullopt;
    }

    return riffData;
}

auto ReadChunk(std::ifstream& file) -> ChunkHeader {
    auto chunk = Util::GetBytes<ChunkHeader>(file);
    file.seekg(chunk.size, std::ios::cur);
    return chunk;
}

}