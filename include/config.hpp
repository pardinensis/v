#pragma once

#include <filesystem>
#include <optional>

namespace Config {
    void init(const std::filesystem::path& pathToProjectRoot);

    template<typename T>
    T get(const std::string& option, const std::optional<T>& defaultValue = std::nullopt);
}
