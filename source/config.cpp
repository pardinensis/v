#include "config.hpp"

#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

static YAML::Node rootNode;

void Config::init(const std::filesystem::path& pathToProjectRoot) {
    auto configFilePath = pathToProjectRoot / std::string("config.yaml");
    auto configFileStream = std::ifstream(configFilePath);
    if (!configFileStream.is_open()) {
        throw std::runtime_error("config file does not exist");
    }

    try {
        rootNode = YAML::Load(configFileStream);
        if (!rootNode.IsMap()) {
            throw YAML::ParserException(YAML::Mark(), "the root node is not a map");
        }
    }
    catch (const YAML::ParserException& e) {
        std::cerr << "failed to parse config file " << configFilePath << std::endl;
        throw e;
    }
}

template<typename T>
T Config::get(const std::string& option, const std::optional<T>& defaultValue) {
    std::string token;
    std::stringstream ss(option);
    T value;
    try {
        // store nodes in a vector to avoid overriding them
        // even an assignment by value like "YAML::Node a = b" may modify the source (in this case b)
        std::vector<YAML::Node> nodes {rootNode};
        while(std::getline(ss, token, '.')) {
            nodes.emplace_back(nodes.back()[token]);
        }
        value = nodes.back().as<T>();
    }
    catch (const YAML::Exception& e) {
        if (defaultValue.has_value()) {
            value = defaultValue.value();
        }
        else {
            std::cerr << "failed to retrieve " << option << " from config file" << std::endl;
            throw e;
        }
    }
    return value;
}

template bool Config::get<bool>(const std::string& option, const std::optional<bool>& defaultValue);
template int Config::get<int>(const std::string& option, const std::optional<int>& defaultValue);
template std::string Config::get<std::string>(const std::string& option, const std::optional<std::string>& defaultValue);
