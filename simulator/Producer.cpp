#include "Simulator.h"
#include "Producer.h"


void
Producer::buildTasks(list<std::function<std::unique_ptr<AbstractAlgorithm>()>> &algoFactory, list<Travel> &travelList,
                     list<string> &algoNames, const string &outputPath) {
    tasksParm.reserve(algoFactory.size() * travelList.size());
    auto currentAlgoName = algoNames.begin();
    for (auto algoF : algoFactory) {
        string algoName = *currentAlgoName;
        currentAlgoName++;
        for (Travel travel: travelList) {
            string fileName = outputPath + "/" + algoName + "_" + travel.getTravelName() + "_crane_instructions";

            string errorFile = outputPath + "/errors/" + algoName + "_" + travel.getTravelName() + ".errors";
            tasksParm.emplace_back(algoF, travel, fileName, errorFile, algoName);
        }
    }
    numTasks = tasksParm.size();
}

std::optional<Task> Producer::getTask() {

    std::optional<int> taskIndex = nextTaskIndex();
    if (taskIndex) {
        return {tasksParm[taskIndex.value()]};
    } else return {};
}