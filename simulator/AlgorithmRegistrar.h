
#ifndef LINUXSHIP_ALGORITHMREGISTRAR_H
#define LINUXSHIP_ALGORITHMREGISTRAR_H

#include <string>
#include <functional>
#include "../interfaces/AbstractAlgorithm.h"
//#include <dlfcn.h>
#include "dlfcn.h" //earase!!!!!
#include <memory>
#include <list>
#include <iostream>


using std::string;
using std::unique_ptr;
using std::list;

class AlgorithmRegistrar {
    list<std::string> algorithmNames;
    list<std::function<unique_ptr<AbstractAlgorithm>()>> algorithmFactories;
    void registerAlgorithm(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
        instance.algorithmFactories.push_back(algorithmFactory);
    }
    void setNameForLastAlgorithm(const std::string& algorithmName) {
      std::cout<<"assert"<<std::endl;
        algorithmNames.push_back(algorithmName);
    }
public:
    friend class AlgorithmRegistration;
    enum {ALGORITHM_REGISTERED_SUCCESSFULY = 0, FILE_CANNOT_BE_LOADED = -1, NO_ALGORITHM_REGISTERED = -2};
    int loadAlgorithm(const char *path, const std::string& so_file_name_without_so_suffix);

    list<unique_ptr<AbstractAlgorithm>> getAlgorithms()const ;

    const list<std::string>& getAlgorithmNames()const {return algorithmNames; }

    size_t size()const {return algorithmFactories.size(); }
    static AlgorithmRegistrar& getInstance() {return instance; }

private:
    static AlgorithmRegistrar instance;
};


#endif //LINUXSHIP_ALGORITHMREGISTRAR_H
