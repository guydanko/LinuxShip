#include "../interfaces/AbstractAlgorithm.h"
#include "Travel.h"

#ifndef LINUXSHIP_TASK_H
#define LINUXSHIP_TASK_H


class Task {
    AbstractAlgorithm* algo;
    Travel travel;
    string fileName;
    string errorFileName;
    string algoName;
public:
    Task(AbstractAlgorithm* algo, Travel& travel,string& fileName,string& errorFileName,string& algoName): algo(algo),travel(travel), fileName(fileName), errorFileName(errorFileName), algoName(algoName){}
    AbstractAlgorithm* getAlgo(){ return this->algo;}
    Travel& getTravel(){ return this->travel;}
    const string& getFileName(){return this->fileName;}
    const string& getErrorFileName(){return this->errorFileName;}
    const string& getAlgoName(){return this->algoName;}
};


#endif //LINUXSHIP_TASK_H
