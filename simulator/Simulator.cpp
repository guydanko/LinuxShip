#include "Simulator.h"
#include "Task.h"
#include <memory>

string getShipPlanPath(const fs::path &path) {
    std::error_code er;
    int planFiles = 0;
    string fileName;
    for (auto &p: fs::directory_iterator(path, er)) {
        if (p.path().filename().extension().string() == ".ship_plan") {
            fileName = p.path().string();
            planFiles++;
        }
    }
    return planFiles == 1 ? fileName : "";
}

string getRouteFilePath(const fs::path &path) {
    int routeFiles = 0;
    std::error_code er;
    string fileName;
    for (auto &p: fs::directory_iterator(path, er)) {
        if (p.path().filename().extension().string() == ".route") {
            fileName = p.path().string();
            routeFiles++;
        }
    }
    return routeFiles == 1 ? fileName : "";
}

void Simulator::travelErrorsToFile(const string &writeTo) {
    for (const Travel &travel:this->travelList) {
        travel.generalTravelErrorsToFile(writeTo + "/" + travel.getTravelName() + "_general.errors");
    }
}

void Simulator::cleanFiles(list<string> &algoNames) {
    std::error_code er;
    for (auto &algoName : algoNames) {
        for (Travel travel: travelList) {
            fs::remove_all(this->outputPath + "/" + algoName + "_" + travel.getTravelName() + "_crane_instructions",
                           er);
        }
    }
    fs::remove(this->outputPath + "/simulation.results", er);
}

void Simulator::createAlgoFactory() {
    std::error_code er;
    for (auto &p: fs::directory_iterator(this->algoPath, er)) {
        if (p.path().extension().string() == ".so") {
            int result = AlgorithmRegistrar::getInstance().loadAlgorithm(p.path().string().c_str(),
                                                                         p.path().stem().string());
            AlgorithmRegistrar::printAlgoRegistrationError(outputPath + "/errors/algoRegistration.errors",
                                                           p.path().stem().string(), result);
        }
    }
    this->algoFactory = AlgorithmRegistrar::getInstance().getAlgoFactories();
}

void Simulator::createAlgoXTravel() {
    std::error_code er;
    for (auto &p: fs::directory_iterator(this->travelPath, er)) {
        if (fs::is_directory(p, er)) { buildTravel(p); }
    }
    travelErrorsToFile(this->outputPath + "/errors");
    createAlgoFactory();
}

void Simulator::buildTravel(const fs::path &path) {
    list<string> route = {};
    int travelError = 0;
    const string errorFileName = this->outputPath + "/errors/" + path.filename().string() + "_general.errors";
    string shipPlanPath = getShipPlanPath(path), routePath = getRouteFilePath(path);;
    if (shipPlanPath.empty() || routePath.empty()) {
        FileHandler::reportPlanRouteErrors(shipPlanPath, routePath, errorFileName);
        return;
    }

    auto shipPtr = std::make_shared<shared_ptr<ShipMap>>(std::make_shared<ShipMap>());
    travelError |= FileHandler::createShipMapFromFile(shipPlanPath, shipPtr,
                                                      errorFileName);
    travelError |= FileHandler::fileToRouteList(routePath, route, errorFileName);
    if (Travel::isTravelErrorLegal(travelError)) {
        travelList.emplace_back(path.string(), path.filename().string(), shipPlanPath, routePath, *shipPtr, route,
                                travelError);
    }

}

void Simulator::setUpFakeFile() {
    std::ofstream ofs(this->outputPath + "/errors/fakeFile_313246811_");
    if (ofs) { ofs.close(); }
}

int Simulator::initAlgoWithTravelParam(Travel &travel, std::unique_ptr<AbstractAlgorithm>& pAlgo, list<SimulatorError> &errorList,
                                       bool &correctAlgo) {
    unsigned int algoInitError = 0;
    try {

        algoInitError |= pAlgo->readShipPlan(travel.getShipPlanPath());
        algoInitError |= pAlgo->readShipRoute(travel.getRoutePath());
        this->calculator.readShipPlan(travel.getShipPlanPath());
        pAlgo->setWeightBalanceCalculator(calculator);
    }
    catch (...) {
        errorList.emplace_front(
                "algorithm throw exception in init function (readShipPlan/readShipRoute/setWeightBalanceCalculator. Can't run this algorithm on this travel",
                SimErrorType::TRAVEL_INIT);
        correctAlgo = false;
    }
    return algoInitError;
}
int countOperation(list<shared_ptr<CargoOperation>>& cargoOps){
    int sum=0;
    for( auto cargoOp :cargoOps){
        if(cargoOp->getOp()==AbstractAlgorithm::Action::UNLOAD || cargoOp->getOp()==AbstractAlgorithm::Action::LOAD){
            sum+=5;
        }
        if(cargoOp->getOp()==AbstractAlgorithm::Action::MOVE){
            sum+=3;
        }
    }
    return sum;
}
/* returns amount of operations in a travel-algo pair*/
int Simulator::runOneTravel(Travel &travel, std::unique_ptr<AbstractAlgorithm> pAlgo, const string &travelAlgoDirectory,
                            const string &errorFileName){
    int algoInitError = 0;
    std::error_code er;
    const string fakeFilePath = this->outputPath + "/errors/fakeFile_313246811_";
    fs::create_directory(this->outputPath+ "/"+travelAlgoDirectory, er);
    bool correctAlgo = true;
    int sumCargoOperation = 0;
    if (travel.isTravelLegal()) {
        list<SimulatorError> errorList;
        algoInitError = initAlgoWithTravelParam(travel, pAlgo, errorList, correctAlgo);
        if (correctAlgo) {
            correctAlgo = SimulatorAlgoCheck::compareErrorAlgoSimulationInit(algoInitError,
                                                                             travel.getTravelError(), errorList,
                                                                             correctAlgo);
        }
        SimulatorError::simulatorErrorsToFile(errorList, errorFileName, travel.getTravelName());
        list<shared_ptr<Container>> doubleIdList = {};
        if (correctAlgo) {
            bool throwException = false;
            while (!travel.didTravelEnd() && !throwException) {
                set<string> rejectedID = {};
                int simulationInstError = 0;
                errorList = {};
                list<shared_ptr<Container>> loadList = {};
                simulationInstError |= travel.getContainerList(errorFileName, loadList);
                const string writeTo = travelAlgoDirectory + "/" + travel.getCurrentPort() + "_" +
                                       std::to_string(travel.getCurrentVisitNumber()) + ".crane_instructions";
                int algoGetInsError = 0;
                try {
                    const string nextLoadFile = fs::exists(travel.getNextCargoFilePath(), er)
                                                ? travel.getNextCargoFilePath() : fakeFilePath;
                    algoGetInsError = pAlgo->getInstructionsForCargo(nextLoadFile, writeTo);
                }
                catch (...) {
                    throwException = true;
                }
                if (!throwException) {
                    list<shared_ptr<CargoOperation>> cargoOps = {};
                    FileHandler::createCargoOpsFromFile(writeTo, cargoOps, errorFileName);
                    sumCargoOperation += countOperation(cargoOps);
                    simulationInstError |= SimulatorAlgoCheck::connectContainerToCargoOp(loadList, travel.getShipMap(),
                                                                                         cargoOps, errorList,
                                                                                         doubleIdList,
                                                                                         travel.getRoute(),
                                                                                         rejectedID, correctAlgo);
                    simulationInstError |= SimulatorAlgoCheck::checkAlgoCorrect(travel.getShipMap(), travel.getRoute(),
                                                                                this->calculator, cargoOps, loadList,
                                                                                travel.getCurrentPort(), errorList,
                                                                                doubleIdList, correctAlgo);
                    SimulatorAlgoCheck::algoErrorInstVsSimulationErrorInst(algoGetInsError, simulationInstError,
                                                                           errorList,
                                                                           correctAlgo);
                    SimulatorError::simulatorErrorsToFile(errorList, errorFileName, travel.getTravelName(),
                                                          travel.getCurrentPort(), travel.getCurrentVisitNumber());
                    travel.goToNextPort();
                }
            }
            if (!throwException) {
                errorList = {};
                SimulatorAlgoCheck::checkIfShipEmpty(travel.getShipMap(), errorList, correctAlgo);
                SimulatorError::simulatorErrorsToFile(errorList, errorFileName, travel.getTravelName());
            }
        }
    }
    if (correctAlgo) {
        return sumCargoOperation;
    } else {
        return -1;
    }


}

void Simulator::deleteEmptyFiles() {
    std::error_code er;
    for (auto &simFiles: fs::directory_iterator(this->outputPath + "/errors", er)) {
        const string file = simFiles.path().string();
        if (fs::file_size(simFiles, er) == 0) {
            fs::remove(simFiles, er);
        }
    }
    if (fs::is_empty(this->outputPath + "/errors", er)) {
        fs::remove_all(this->outputPath + "/errors", er);
    }
    if (fs::exists(this->outputPath + "/simulation.results", er) &&
        fs::is_empty(this->outputPath + "/simulation.results", er)) {
        fs::remove_all(this->outputPath + "/simulation.results", er);
    }
}

list<string> getAlgosByOrder(unordered_map<string, unordered_map<string, int>> &algoOpMap) {
    list<string> algosInOrder;
    multimap<pair<int, int>, string> algoSumErrorMap;
    for (auto &algo: algoOpMap) {
        int algoErrors = 0, algoSum = 0;
        for (auto &travelName: algo.second) {
            if (travelName.second == -1) {
                algoErrors++;
            } else {
                algoSum += travelName.second;
            }
        }
        algoSumErrorMap.insert(std::make_pair(std::make_pair(algoErrors, algoSum), algo.first));
    }
    for (auto &pair:algoSumErrorMap) {
        algosInOrder.emplace_back(pair.second);
    }
    return algosInOrder;
}

void Simulator::printResults(unordered_map<string, unordered_map<string, int>> simResults) {
    list<string> algosInOrder = getAlgosByOrder(simResults);
    list<string> travelNameOrder = {};
    for (auto &travel: this->travelList) {
        travelNameOrder.emplace_back(travel.getTravelName());
    }
    FileHandler::printSimulatorResults(this->outputPath + "/simulation.results", algosInOrder, travelNameOrder,
                                       simResults);
}
void Simulator::workerFunction(){
    std::optional<Task> currentTask = producer.getTask();
    while(currentTask) {
       int numOp= runOneTravel(currentTask.value().getTravel(),currentTask.value().getAlgo(),currentTask.value().getFileName(),currentTask.value().getErrorFileName());
       resultMap[currentTask->getAlgoName()][currentTask->getTravel().getTravelName()] = numOp;
       currentTask = producer.getTask();
    }
}
void  Simulator::initializeWorkers(){
    for(int i=0; i<this->numThreads ; ++i) {
        workers.push_back(std::thread([this]{ this->workerFunction();}));
    }
}
void Simulator::waitTillFinish() {
    for(auto& t : workers) {
        t.join();
    }
}
void Simulator::runOnlyMain(){
    for(auto currentTask : producer.getTaskList()) {
        int numOp= runOneTravel(currentTask.getTravel(),currentTask.getAlgo(),currentTask.getFileName(),currentTask.getErrorFileName());
        resultMap[currentTask.getAlgoName()][currentTask.getTravel().getTravelName()] = numOp;
    }
}
void Simulator::run() {
    setUpFakeFile();
    createAlgoXTravel();
    list<string> algoNames = AlgorithmRegistrar::getInstance().getAlgorithmNames();
    cleanFiles(algoNames);
    this->producer.buildTasks(this->algoFactory,this->travelList,algoNames, this->outputPath);

    if(numThreads>1){
        initializeWorkers();
        waitTillFinish();
    }
    else{
        runOnlyMain();
    }
    printResults(resultMap);
    deleteEmptyFiles();
}

