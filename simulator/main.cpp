#include "Simulator.h"
#include <filesystem>
#include <iostream>
#include "map"

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

//    std::multimap<pair<int, int>, string> algoSumErrorMap = {{std::make_pair(0, 1), "this"},
//                                                             {std::make_pair(1, 1), "is"},
//                                                             {std::make_pair(2, 7), "hi"},
//                                                             {std::make_pair(0, 9), "what"}};
//
//    list<string> algosInOrder;
//
//    for (auto &pair:algoSumErrorMap) {
//        algosInOrder.emplace_back(pair.second);
//    }
//    for(auto pair: algoSumErrorMap){
//        std::cout << pair.second << "\n";
//    }

}