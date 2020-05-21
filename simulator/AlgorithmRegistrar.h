//Registrar class to manage dynamic load of shared algorithm
#include "../interfaces/AbstractAlgorithm.h"
#include <functional>
#include <dlfcn.h>
#include <memory>
#include <list>
#include <iostream>
#include <map>
#include <fstream>

using std::ofstream;
using std::string;
using std::unique_ptr;
using std::list;

#ifndef LINUXSHIP_ALGORITHMREGISTRAR_H
#define LINUXSHIP_ALGORITHMREGISTRAR_H


class AlgorithmRegistrar {
    friend class AlgorithmRegistration;

    struct DlCloser {
        void operator()(void *dlhandle) const noexcept;
    };

    typedef std::function<std::unique_ptr<AbstractAlgorithm>()> AlgorithmFactory;
    typedef std::unique_ptr<void, DlCloser> DlHandler;

    std::map<std::string, DlHandler> handles;
    std::list<AlgorithmFactory> factories;
    std::list<std::string> algoNames;

    AlgorithmRegistrar() = default;

    ~AlgorithmRegistrar();

    void registerAlgorithm(AlgorithmFactory factory) { factories.push_back(factory); };

    void setNameForLastAlgorithm(const std::string &algorithmName) {
        algoNames.push_back(algorithmName);
    }

public:

    enum RegistrationError {
        ALGORITHM_REGISTERED_SUCCESSFULY = 0, FILE_CANNOT_BE_LOADED = -1, NO_ALGORITHM_REGISTERED = -2
    };

    AlgorithmRegistrar(const AlgorithmRegistrar &) = delete;

    AlgorithmRegistrar &operator=(const AlgorithmRegistrar &) = delete;

    int loadAlgorithm(const char *path, const std::string &so_file_name_without_so_suffix);

    const std::list<std::string> &getAlgorithmNames() const { return algoNames; }

    size_t size() const { return factories.size(); }

    static AlgorithmRegistrar &getInstance();

    static void printAlgoRegistrationError(const string &fileName, const string &algoName,
                                           int result);

    std::list<AlgorithmFactory> getAlgoFactories() { return this->factories; };

};


#endif //LINUXSHIP_ALGORITHMREGISTRAR_H
