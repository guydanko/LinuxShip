
#ifndef LINUXSHIP_ALGORITHMREGISTRAR_H
#define LINUXSHIP_ALGORITHMREGISTRAR_H

#include <string>
#include <functional>
#include "../interfaces/AbstractAlgorithm.h"
#include <dlfcn.h>
#include <memory>
#include <list>
#include <iostream>
#include <map>


using std::string;
using std::unique_ptr;
using std::list;

class AlgorithmRegistrar {
    friend class AlgorithmRegistration;
    static AlgorithmRegistrar instance;

    struct DlCloser {
        void operator()(void *dlhandle) const noexcept;
    };

    typedef std::function<std::unique_ptr<AbstractAlgorithm>()> AlgorithmFactory;
    typedef std::unique_ptr<void, DlCloser> DlHandler;

    std::map<std::string, DlHandler> _handles;
    std::vector<AlgorithmFactory> _factories;
    std::vector<std::string> _algoNames;

    typedef decltype(_factories)::const_iterator const_iterator;

    AlgorithmRegistrar() = default;

    ~AlgorithmRegistrar();

    AlgorithmRegistrar(const AlgorithmRegistrar &) = delete;

    AlgorithmRegistrar &operator=(const AlgorithmRegistrar &) = delete;

    list<std::function<unique_ptr<AbstractAlgorithm>()>> algorithmFactories;

    inline void registerAlgorithm(AlgorithmFactory factory) { _factories.push_back(factory); };

    void setNameForLastAlgorithm(const std::string &algorithmName) {
        std::cout << "assert" << std::endl;
        _algoNames.push_back(algorithmName);
    }

public:

    enum {
        ALGORITHM_REGISTERED_SUCCESSFULY = 0, FILE_CANNOT_BE_LOADED = -1, NO_ALGORITHM_REGISTERED = -2
    };

    int loadAlgorithm(const char *path, const std::string &so_file_name_without_so_suffix);

    list<unique_ptr<AbstractAlgorithm>> getAlgorithms() const;

    const std::vector<std::string> &getAlgorithmNames() const { return _algoNames; }

    size_t size() const { return algorithmFactories.size(); }

    static AlgorithmRegistrar &getInstance(){ return instance;};

};


#endif //LINUXSHIP_ALGORITHMREGISTRAR_H
