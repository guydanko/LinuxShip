
#include <dlfcn.h>

#include "AlgorithmRegistrar.h"

AlgorithmRegistrar AlgorithmRegistrar::instance;

void AlgorithmRegistrar::DlCloser::operator()(void *dlhandle) const noexcept {
    dlclose(dlhandle);
}

AlgorithmRegistrar::~AlgorithmRegistrar() {
    _factories.clear();
    _handles.clear();
}

list<unique_ptr<AbstractAlgorithm>> AlgorithmRegistrar::getAlgorithms() const {
    list<unique_ptr<AbstractAlgorithm>> algorithms;
    for (auto algorithmFactoryFunc : algorithmFactories) {
        algorithms.push_back(algorithmFactoryFunc());
    }
    return algorithms;
}

int AlgorithmRegistrar::loadAlgorithm(const char *path, const std::string &so_file_name_without_so_suffix) {
    size_t size = instance.size();

    DlHandler algoHandle(dlopen(path,RTLD_LAZY));
    // this is mockup code, real code will load the .so files
    std::unique_ptr<void, DlCloser> handle(dlopen(path, RTLD_LAZY));

    if (!handle) {
        return FILE_CANNOT_BE_LOADED;
    }
    if (instance.size() == size) {
        return NO_ALGORITHM_REGISTERED; // no algorithm registered
    }

    instance.setNameForLastAlgorithm(so_file_name_without_so_suffix);

    return ALGORITHM_REGISTERED_SUCCESSFULY;
}