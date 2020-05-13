#include <dlfcn.h>
#include "AlgorithmRegistrar.h"

void AlgorithmRegistrar::DlCloser::operator()(void *dlhandle) const noexcept {
    dlclose(dlhandle);
}

AlgorithmRegistrar::~AlgorithmRegistrar() {
    factories.clear();
    handles.clear();
}

AlgorithmRegistrar& AlgorithmRegistrar::getInstance(){
    static AlgorithmRegistrar instance;
    return instance;
}

std::list<std::unique_ptr<AbstractAlgorithm>> AlgorithmRegistrar::getAlgorithms() const {
    std::list<std::unique_ptr<AbstractAlgorithm>> algorithms;
    for (auto factory:factories) {
        algorithms.push_back(factory());
    }
    return algorithms;
}

int AlgorithmRegistrar::loadAlgorithm(const char *path, const std::string &so_file_name_without_so_suffix) {


    size_t size = getInstance().size();

    std::unique_ptr<void, DlCloser> algoHandle(dlopen(path, RTLD_LAZY));

    if (algoHandle != nullptr) {
        if (getInstance().size() == size) {
            return NO_ALGORITHM_REGISTERED; // no algorithm registered
        }
     handles[path] = std::move(algoHandle);
    } else {
        return FILE_CANNOT_BE_LOADED; //dlopen failed
    }

    getInstance().setNameForLastAlgorithm(so_file_name_without_so_suffix);
    return ALGORITHM_REGISTERED_SUCCESSFULY;
}