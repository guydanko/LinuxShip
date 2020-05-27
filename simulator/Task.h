#include "../interfaces/AbstractAlgorithm.h"
#include "Travel.h"
#include <functional>
#ifndef LINUXSHIP_TASK_H
#define LINUXSHIP_TASK_H


class Task {
    std::function<std::unique_ptr<AbstractAlgorithm>()> algoFuctory;
    Travel travel;
    string fileName;
    string errorFileName;
    string algoName;
public:
    Task(std::function<std::unique_ptr<AbstractAlgorithm>()>& algoFuctory, Travel& travel,string& fileName,string& errorFileName,string& algoName): algoFuctory(algoFuctory),travel(travel), fileName(fileName), errorFileName(errorFileName), algoName(algoName){}
    std::unique_ptr<AbstractAlgorithm> getAlgo(){ return this->algoFuctory();}
    Travel& getTravel(){ return this->travel;}
    const string& getFileName(){return this->fileName;}
    const string& getErrorFileName(){return this->errorFileName;}
    const string& getAlgoName(){return this->algoName;}
};


#endif //LINUXSHIP_TASK_H
