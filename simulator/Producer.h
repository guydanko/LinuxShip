
#include <atomic>
#include <optional>
#include <functional>
#include "Task.h"

#ifndef LINUXSHIP_PRODUCER_H
#define LINUXSHIP_PRODUCER_H


class Producer {
    int numTasks = -1;
    std::atomic_int taskCounter = 0;
    std::vector<Task> taskList;

    std::optional<int> nextTaskIndex() ;

public:
    Producer(){}
    std::optional<Task> getTask();
    void buildTasks( list<std::function<std::unique_ptr<AbstractAlgorithm>()>>& algoFactory, list<Travel>& travelList,list<string>& algoNames,const string& outputPath);
    std::vector<Task>& getTaskList(){ return this->taskList;}
};


#endif //LINUXSHIP_PRODUCER_H
