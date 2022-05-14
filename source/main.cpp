#include "app.hpp"
#include "config.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>

int main(int, char* argv[]) {
    // init config with the absolute path to the project root
    const auto relPathToExecutable = std::filesystem::path(std::string(argv[0]));
    const auto absPathToExecutable = std::filesystem::canonical(relPathToExecutable);
    const auto absPathToProjectRoot = absPathToExecutable.parent_path().parent_path();

    try {
        Config::init(absPathToProjectRoot);

        // start up the application
        App app;
        app.run();
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
