/*Producer class which is in charge of handing tasks to multiple thread workers*/
#include <atomic>
#include <optional>
#include <functional>
#include "Task.h"

#ifndef LINUXSHIP_PRODUCER_H
#define LINUXSHIP_PRODUCER_H


class Producer {
    int numTasks = -1;
    std::atomic_int taskCounter = 0;
    list<std::function<std::unique_ptr<AbstractAlgorithm>()>>& algoFactory;
    list<Travel>& travelList;
    list<string> &algoNames;
    const string &outputPath;


    std::optional<int> nextTaskIndex() ;

public:
    Producer(list<std::function<std::unique_ptr<AbstractAlgorithm>()>> &algoFactory, list<Travel> &travelList,
              list<string> &algoNames, const string &outputPath): algoFactory(algoFactory), travelList(travelList), algoNames(algoNames), outputPath(outputPath) {}
    std::optional<Task> getTask();
    void setNumTasks(int num){this->numTasks=num;}
};


#endif //LINUXSHIP_PRODUCER_H
