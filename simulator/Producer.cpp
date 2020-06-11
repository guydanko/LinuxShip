#include "Simulator.h"
#include "Producer.h"

std::optional<Task> Producer::getTask() {

    std::optional<int> taskIndex = nextTaskIndex();
    if (taskIndex) {
        int i = 0;
        auto currentAlgoName = algoNames.begin();
        for (auto &algoF : algoFactory) {
            string algoName = *currentAlgoName;
            currentAlgoName++;
            for (Travel &travel: travelList) {
                if (i == taskIndex.value()) {
                    string fileName =
                            outputPath + "/" + algoName + "_" + travel.getTravelName() + "_crane_instructions";

                    string errorFile = outputPath + "/errors/" + algoName + "_" + travel.getTravelName() + ".errors";
                    return Task(algoF, travel, fileName, errorFile, algoName);
                } else {
                    i++;
                }
            }
        }
        //should not be here
        return {};
    }
    return {};
}

std::optional<int> Producer::nextTaskIndex() {
    for (int curr_counter = taskCounter.load(); curr_counter < numTasks;) {
        if (taskCounter.compare_exchange_strong(curr_counter, curr_counter + 1)) {
            return {curr_counter};
        }
    }
    return {};
}