#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace RIFF {

namespace {
    static auto toString(uint32_t field) -> std::string {
        char buf[5];
        std::memcpy(buf, &field, 4);
        buf[4] = '\0';
        return buf;

    }
}

namespace Chunks {
    constexpr auto IdSize = 4z;
    using Identifier = char[IdSize + 1];
    
    constexpr Identifier RIFF = "RIFF";
    constexpr Identifier WAVE = "WAVE";
    constexpr Identifier fmt  = "fmt ";
}

struct FmtChunk {
    uint32_t id;
    uint32_t size;
    uint16_t format;
    uint16_t channels;
    uint32_t frequency;
    uint32_t bytesPerSec;
    uint16_t bytesPerBlock;
    uint16_t bitsPerSample;

    FmtChunk(const char* data) {
        std::memcpy(this, data, sizeof * this);
    }

    auto formatName() const -> std::string {
        switch (format) {
        case 0x0001: return "PCM";
        case 0x0003: return "Float";
        case 0xFFFF: return "WWise";
        default: return std::format("Unknown ({})", format);
        }
    }

    auto description() const -> std::string {
        return std::format("{} format, {} Hz, {} channel", formatName(), channels, frequency);
    }
};
static_assert(sizeof FmtChunk == 24);

struct RiffData {
    uint32_t id;
    uint32_t size;
    uint32_t chunkId;


    RiffData(const char* data) {
        std::memcpy(this, data, sizeof *this);
    }

    auto idName() const -> std::string {
        return toString(id);
    }

    auto chunkIdName() const -> std::string {
        return toString(chunkId);
    }

    auto isWav() const -> bool {
        return chunkIdName() == RIFF::Chunks::WAVE;
    }
};
static_assert(sizeof RiffData == 12);

auto FindRIFF(const char* data) -> std::optional<RiffData> {
    RiffData riffData{data};

    if (riffData.idName() != Chunks::RIFF) {
        return std::nullopt;
    }

    return riffData;
}

}