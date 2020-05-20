/*Data struct to represent a container*/
#include <string>
#include <cmath>
using  std::string ;

#ifndef SHIPGIT_CONTAINER_H
#define SHIPGIT_CONTAINER_H
#define IMAGINARY_CONTAINER "IMAGINARY_CONTAINER"

class Container {
    int weight;
    string destination;
    string id;
    int portIndex = 0;
    bool isLegalContainer;
    bool isContainerLoaded=false;
    bool isContainerReject=false;

public:

    Container(int weight = 0, const string &destination = "", const string &id = IMAGINARY_CONTAINER,
              bool isLegalContainer = true) : weight(weight),
                                              destination(destination),
                                              id(id), isLegalContainer(isLegalContainer) {};

    Container(const Container &copyContainer) : weight(copyContainer.weight), destination(copyContainer.destination),
                                                id(copyContainer.id) {}

    /* return false if values are illegal*/
    static bool isLegalParamContainer(int weight, string &destination, string &id);

    const string &getId() { return this->id; }

    const string &getDestination() { return this->destination; }

    void setPortIndex(int i) { this->portIndex = i; }

    int getPortIndex() const { return this->portIndex; }

    bool isContainerLegal() { return this->isLegalContainer; }

    void setIsContainerLoaded(bool val) { this->isContainerLoaded = val; }

    bool getIsContainerLoaded() const { return this->isContainerLoaded; }

    void setIsContainerReject(bool val) { this->isContainerReject = val; }

    bool getIsContainerReject() const { return this->isContainerReject; }

    static bool isPortValid(const string &port);

    static bool isLegalId(const string &id);
    int getWeight(){ return this->weight;}

};


#endif //SHIPGIT_CONTAINER_H
