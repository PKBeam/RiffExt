#pragma once

#include <ranges>
#include <vector>
#include <optional>
#include <print>
#include <string_view>

namespace Args {

    struct Options {
        bool dump = false;
        bool scan = false;
        bool strictWav = false;
        std::string_view inFile{};
    };

    auto Parse(const std::vector<std::string_view>& args) -> std::optional<Options> {
        Options opts;
        for (auto const& [index, arg] : args | std::views::enumerate) {
            if (arg == "-h" || arg == "--help") {
                std::println("Usage: RiffExt [options] <input file>");
                return std::nullopt;
            }

            if (arg == "-d" || arg == "--dump") {
                opts.dump = true;
            } else if (arg == "-s" || arg == "--scan") {
                opts.scan = true;
            } else if (arg == "-w" || arg == "--wav") {
                opts.strictWav = true;
            } else if (arg.starts_with("-")) {
                std::println("Unrecognised argument {}", arg);
                return std::nullopt;
            } else {
                if (index == args.size() - 1) {
                    opts.inFile = arg;
                } else {
                    std::println("Filename ({}) must be specified last", arg);
                    return std::nullopt;
                }
            }
        }

        if (opts.inFile.empty()) {
            std::println("Filename not specified");
            return std::nullopt;
        }

        return opts;
    }

    auto Parse(int argc, const char* argv[]) -> std::optional<Options> {
        if (argc < 2) {
            return std::nullopt;
        }

        std::vector<std::string_view> args;
        for (auto i = 1; i < argc; i++) {
            args.push_back(argv[i]);
        }
        return Args::Parse(args);
    }
}