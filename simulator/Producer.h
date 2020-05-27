
#include <atomic>
#include <optional>
#include <functional>
#include "Task.h"

#ifndef LINUXSHIP_PRODUCER_H
#define LINUXSHIP_PRODUCER_H


class Producer {
    int numTasks = -1;
    std::atomic_int taskCounter = 0;
    std::vector<Task> tasksParm;

    std::optional<int> nextTaskIndex() {
        for(int curr_counter = taskCounter.load(); curr_counter < numTasks; ) {
            if(taskCounter.compare_exchange_strong(curr_counter, curr_counter + 1)) {
                return {curr_counter};
            }
        }
        return {};
    }

public:
    Producer(){}
    std::optional<Task> getTask();
    void buildTasks( list<std::function<std::unique_ptr<AbstractAlgorithm>()>>& algoFactory, list<Travel>& travelList,list<string>& algoNames,const string& outputPath);
};


#endif //LINUXSHIP_PRODUCER_H
