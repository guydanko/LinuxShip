#include "Simulator.h"
#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

    if(argc !=2 ){
        std::cerr << "Cannot run simulator, program must receive one parameter!\n";
        return EXIT_FAILURE;

    }
    const string root = argv[1];
    if(fs::exists(root)) {
        Simulator sim(root);
        sim.run();
        return EXIT_SUCCESS;
    } else{
        std::cerr << "Root for simulator directory does not exist, cannot run simulator\n";
        return EXIT_FAILURE;
    }

}