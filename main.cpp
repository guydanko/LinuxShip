#include "Simulator.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

    if (argc == 1) {
        std::cerr << "Cannot run simulator, program must receive one parameter!\n";
        /*fatal error in file*/
        return EXIT_FAILURE;

    }

    const string root = argv[1];
    if (fs::exists(root)) {
        Simulator sim(root, argv[2], argv[3]);
        sim.run();
        return EXIT_SUCCESS;
    } else {
        /*fatal error in file*/
        std::cerr << "Root for simulator directory does not exist, cannot run simulator\n";
        return EXIT_FAILURE;
    }

}