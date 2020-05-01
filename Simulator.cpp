#include "WeightBalanceCalculator.h"
#include "AbstractAlgorithm.h"
#include "Simulator.h"
#include "FileHandler.h"
#include <map>

using std::string;
using std::map;

void deleteRejectDoubleID(list<shared_ptr<CargoOperation>>& cargoOps, int countErase,list<SimulatorError>& listError,const string& id,bool onShip){
    int countRemoveOp=cargoOps.size();
    //already on ship remove all with this id
    if(onShip){
        cargoOps.remove_if([&id](shared_ptr<CargoOperation>& cargoOp){ return cargoOp->getOp()== AbstractAlgorithm::Action::REJECT && cargoOp->getContainer()->getId()==id; });
    }
        //not on ship remove all with this id except first
    else{
        auto startPlace= cargoOps.begin();
        while (startPlace!= cargoOps.end() && (*startPlace)->getContainer()->getId()!=id){
            startPlace++;
        }
        std::remove_if(startPlace, cargoOps.end(),[&id](shared_ptr<CargoOperation>& cargoOp){ return cargoOp->getOp()== AbstractAlgorithm::Action::REJECT && cargoOp->getContainer()->getId()==id; });
    }

    countRemoveOp= countRemoveOp - cargoOps.size();
    if(countRemoveOp!= (countErase-1)){
        countErase--;
        listError.emplace_back("because: id- "+ id+ " rejected "+ std::to_string(countRemoveOp)+ " times but there are "+std::to_string(countErase )+" container with this id to reject by reason of double id",SimErrorType::GENERAL_PORT);
    }
}

void deleteDoubleID(list<shared_ptr<Container>>& loadList,Ship* ship, const string& id,bool onShip){
    //already on ship remove all with this id
    if(onShip){
        loadList.remove_if([&id](shared_ptr<Container>& cont){ return cont->getId() == id; });
    }
    //not on ship remove all with this id except first
    else{
        auto startPlace= loadList.begin();
        while (startPlace!= loadList.end() && (*startPlace)->getId()!=id){
            startPlace++;
        }
        std::remove_if(startPlace, loadList.end(),[&id](shared_ptr<Container>& cont){ return cont->getId() == id; });
    }
}
void connectContainerFromShip(Ship* ship, list<shared_ptr<CargoOperation>>& cargoOps){
    for( const auto& cargoOp: cargoOps){
        auto inShip = ship->getShipMap().getContainerIDOnShip().find(cargoOp->getContainer()->getId());
        if(inShip!=ship->getShipMap().getContainerIDOnShip().end()) {
            for (int i = 0; i < ship->getShipMap().getHeight(); i++) {
                for (int j = 0; j < ship->getShipMap().getRows(); j++) {
                    for (int k = 0; k < ship->getShipMap().getCols(); k++) {
                        if (ship->getShipMap().getShipMapContainer()[i][j][k] != nullptr) {
                            if(ship->getShipMap().getShipMapContainer()[i][j][k]->getId() == cargoOp->getContainer()->getId()){
                                cargoOp->setContainer(ship->getShipMap().getShipMapContainer()[i][j][k]);
                            }
                        }
                    }
                }
            }
        }
    }
}
void connectContainerToCargoOp(list<shared_ptr<Container>>& loadList, Ship* ship, list<shared_ptr<CargoOperation>>& cargoOps,list<SimulatorError>& listError){
    map< string, int> containerMap;
    /*create map of id to container in load list*/
    for (const auto& cont: loadList) {
        containerMap[cont->getId()]=containerMap[cont->getId()]+1;
    }
    std::cout<< containerMap["RTRJ0000093"]<<std::endl;
    //get for every id the specific number of container with id- load list + ship
    for(auto& pair : containerMap){
        bool onShip=false;
        auto inShip = ship->getShipMap().getContainerIDOnShip().find(pair.first);
        if(inShip!=ship->getShipMap().getContainerIDOnShip().end()) {
            pair.second++;
            onShip=true;
        }
        int intOnShip=0;
        if(onShip){
            intOnShip=1;
        }

        if(pair.second +intOnShip >1){
            deleteDoubleID(loadList,ship,pair.first, onShip);
            deleteRejectDoubleID(cargoOps,pair.second+intOnShip,listError,pair.first,onShip);
        }
    }
    for( const auto& cargoOp: cargoOps){
       for(const auto& cont : loadList){
           if(cargoOp->getContainer()->getId() == cont->getId()){
               cargoOp->setContainer(cont);
           }
       }
    }
    connectContainerFromShip(ship,cargoOps);
}
void setUpDirectories(const string &directoryRoot) {
    if (fs::exists(directoryRoot)) {
        fs::remove_all(directoryRoot);
    }
    fs::create_directory(directoryRoot);
    fs::create_directory(directoryRoot + "/" + "Travel_File_Errors");
}

void Simulator::travelErrorsToFile(const string &fileName) {
    for (const Travel &travel:this->travelList) {
        travel.errorsToFile(fileName);
    }
}

void Simulator::buildTravel(const fs::path &path) {
    list<string> route = FileHandler::fileToRouteList(path.string() + "/route",
                                                      "SimulatorFiles/Travel_File_Errors/" + path.filename().string() +
                                                      "FileErrors");
    if (!route.empty()) {
        Ship *ship = FileHandler::createShipFromFile(path.string() + "/shipPlan",
                                                     "SimulatorFiles/Travel_File_Errors/" +
                                                     path.filename().string() +
                                                     "FileErrors");
        if (ship != nullptr) {
            ship->setShipRoute(route);
            travelList.emplace_back(path.string(), path.filename().string(), ship);
        }
    }
}

Simulator::Simulator(const string &simulationDirectory) {
    setUpDirectories("SimulatorFiles");
  //  this->algoList.push_back(new NaiveStowageAlgorithm(nullptr, calculator));
   // this->algoList.push_back(new MoreNaiveAlgorithm(nullptr, calculator));
    this->algoList.push_back(new IncorrectAlgorithm(nullptr, calculator));
    for (auto &p: fs::directory_iterator(simulationDirectory)) {
        buildTravel(p);
    }
    travelErrorsToFile("SimulatorFiles/Travel_File_Errors");
    this->rootPath = simulationDirectory;
}


void checkIfShipEmpty(Ship *ship, list<SimulatorError> &errorList, int numberLoads, int numberUnloads) {
    if (numberLoads > numberUnloads) {
        errorList.emplace_back("algorithm gives " + std::to_string(numberLoads) + " load operation, but gives only " +
                               std::to_string(numberUnloads) + " unload operation- ship not empty",
                               SimErrorType::TRAVEL_END);
    } else {
        if (numberLoads < numberUnloads) {
            errorList.emplace_back(
                    "algorithm gives " + std::to_string(numberUnloads) + " unload operation, but gives only" +
                    std::to_string(numberLoads) + " load operation", SimErrorType::TRAVEL_END);
        }
    }
    for (int i = 0; i < ship->getShipMap().getHeight(); i++) {
        for (int j = 0; j < ship->getShipMap().getRows(); j++) {
            for (int k = 0; k < ship->getShipMap().getCols(); k++) {
                if (ship->getShipMap().getShipMapContainer()[i][j][k] != nullptr &&
                    ship->getShipMap().getShipMapContainer()[i][j][k] != ship->getShipMap().getImaginary()) {
                    errorList.emplace_back(
                            "Container with id- " + ship->getShipMap().getShipMapContainer()[i][j][k]->getId() +
                            " is still on the ship at the end of the travel", SimErrorType::TRAVEL_END);
                }
            }
        }
    }
}

void Simulator::runOneTravel(Travel &travel, AbstractAlgorithm *pAlgo, const string &fileName) {
    list<SimulatorError> listError;
    pAlgo->readShipPlan(travel.getTravelPath() + "/shipPlan");
    pAlgo->setWeightBalanceCalculator(calculator);
    pAlgo->readShipRoute(travel.getTravelPath() + "/route");
    string path = fileName + "/" + travel.getTravelName();
    fs::create_directory(path);
    int errorAmount = 0;
    int numberLoads = 0, numberUnloads = 0;
    while (!travel.didTravelEnd()) {
        listError={};
        list<shared_ptr<Container>> loadList = travel.getContainerList(path);
        //path to read container list and write cargo op
        const string writeTo =
                path + "/" + travel.getShip()->getCurrentPort() + "." + std::to_string(travel.getCurrentVisitNumber()) +
                "Operations";
        pAlgo->getInstructionsForCargo(travel.getNextCargoFilePath(), writeTo);
        //read cargo op file and make list
        list<shared_ptr<CargoOperation>> cargoOps = FileHandler::createCargoOpsFromFile(writeTo, loadList);
        connectContainerToCargoOp(loadList,travel.getShip(),cargoOps, listError);
        checkAlgoCorrect(travel.getShip(), cargoOps, loadList, travel.getShip()->getCurrentPort(),
                                     numberLoads, numberUnloads, listError);
        errorAmount += listError.size();
        FileHandler::simulatorErrorsToFile(listError, path, travel.getTravelName(),
                                           travel.getShip()->getCurrentPort(), travel.getCurrentVisitNumber());
        travel.goToNextPort();
    }
    list<SimulatorError> listErrorEnd;
    checkIfShipEmpty(travel.getShip(), listErrorEnd, numberLoads, numberUnloads);
    FileHandler::simulatorErrorsToFile(listErrorEnd, path, travel.getTravelName());
    travel.setToOriginalTravel();
    if (errorAmount == 0) {
        FileHandler::simulatorErrorsToFile(listError, path, travel.getTravelName(), "", 0, true);
    }
}

void Simulator::deleteEmptyFiles() {
    for (auto &simFiles: fs::recursive_directory_iterator("SimulatorFiles")) {
        if (!fs::is_directory(simFiles) && fs::file_size(simFiles) == 0) {
            fs::remove(simFiles);
        }
    }
}

void Simulator::run() {
    const string directoryRoot = "SimulatorFiles";
    int algoNum = 1;
    for (AbstractAlgorithm *pAlgo: algoList) {
        string fileName = directoryRoot + "/" + std::to_string(algoNum);
        algoNum++;
        fs::create_directory(fileName);
        for (Travel &travel: travelList) {
            runOneTravel(travel, pAlgo, fileName);
        }
    }
    deleteEmptyFiles();
}

bool indexInLimit(Ship *ship, MapIndex index) {
    if (index.getHeight() < 0 || index.getHeight() >= ship->getShipMap().getHeight()) {
        return false;
    }
    if (index.getRow() < 0 || index.getRow() >= ship->getShipMap().getRows()) {
        return false;
    }
    return !(index.getCol() < 0 || index.getCol() >= ship->getShipMap().getCols());
}

bool containerAbove(Ship *ship, MapIndex index) {
    for (int i = ship->getShipMap().getHeight() - 1; i > index.getHeight(); i--) {
        if (ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()] != nullptr) {
            return true;
        }
    }
    return false;
}

void indexAccessible(Ship *ship, CargoOperation &cargoOp, list<SimulatorError> &errorList) {
    if (!indexInLimit(ship, cargoOp.getIndex())) {
        errorList.emplace_back("illegal index, exceeds ship plan limits- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    if (containerAbove(ship, cargoOp.getIndex())) {
        errorList.emplace_back("cannot reach container in index, blocked above by containers- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
}

bool solidGround(Ship *ship, MapIndex index) {
    if (index.getHeight() == 0) {
        return true;
    }
    int i = index.getHeight() - 1;
    return ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()] != nullptr;
}

void checkIfAllUnloaded(Ship *ship, const string &port, list<SimulatorError> &errorList) {
    for (int i = 0; i < ship->getShipMap().getHeight(); i++) {
        for (int j = 0; j < ship->getShipMap().getRows(); j++) {
            for (int k = 0; k < ship->getShipMap().getCols(); k++) {
                if (ship->getShipMap().getShipMapContainer()[i][j][k] != nullptr &&
                    ship->getShipMap().getShipMapContainer()[i][j][k] != ship->getShipMap().getImaginary()) {
                    if (ship->getShipMap().getShipMapContainer()[i][j][k]->getDestination() == port) {
                        errorList.emplace_back("Container on the ship with id- " +
                                               ship->getShipMap().getShipMapContainer()[i][j][k]->getId() +
                                               " has current destination -" + port +
                                               "- but container still on the ship when ship left the port",
                                               SimErrorType::GENERAL_PORT);
                        return;
                    }
                }
            }
        }
    }
}

bool compByPortDest(const shared_ptr<Container> cont1, const shared_ptr<Container> cont2) {
    return cont1->getPortIndex() < cont2->getPortIndex();
}

void orderListLoadContainer(list<shared_ptr<Container>> &containerListToLoad, list<string> &route) {
    map<string, int> portNumberMap;
    int number = 1;
    for (const string &port : route) {
        auto itr = portNumberMap.find(port);
        if (itr == portNumberMap.end()) {
            portNumberMap[port] = number;
            number++;
        }
    }
    for (auto cont : containerListToLoad) {
        auto itr = portNumberMap.find(cont->getDestination());
        // container destination is not in route- should reject
        if (itr == portNumberMap.end()) {
            cont->setPortIndex(0);
        } else {
            cont->setPortIndex(portNumberMap[itr->first]);
        }
    }
}

void checkLoadOperation(Ship *ship, CargoOperation &cargoOp, list<shared_ptr<Container>> &loadList,
                        map<string, CargoOperation *> &rememberToLoadAgainIdToIndex, const string &currentPort,
                        int &maxNumberPortLoaded, list<SimulatorError> &errorList) {

    if (!cargoOp.getContainer()->isContainerLegal()) {
        errorList.emplace_back("load container with illegal parameters- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        //although operation wants to load container with  illegal parameters, algo still gives an operation to this container
        cargoOp.getContainer()->setIsContainerLoaded(1);
        return;
    }
    int numberOfIdInList = 0;
    shared_ptr<Container> containerFromList = nullptr;
    indexAccessible(ship, cargoOp, errorList);
    if (ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] !=
        nullptr) {
        errorList.emplace_back("index is occupied by container or cannot be place due to ship plan- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        cargoOp.getContainer()->setIsContainerLoaded(1);
        return;
    }
    if (!solidGround(ship, cargoOp.getIndex())) {
        errorList.emplace_back("cannot place container without underneath platform- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        cargoOp.getContainer()->setIsContainerLoaded(1);
        return;
    }
    for (auto pair: rememberToLoadAgainIdToIndex) {
        if (pair.first == cargoOp.getContainer()->getId()) {
            numberOfIdInList++;
        }
    }
    for (auto container: loadList) {
        if (container->getId() == cargoOp.getContainer()->getId() && container->getIsContainerLoaded() != 1) {
            numberOfIdInList++;
            containerFromList = container;
        }
    }
    if (numberOfIdInList <= 0) {
        errorList.emplace_back("container with this id does not exist in port- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        cargoOp.getContainer()->setIsContainerLoaded(1);
        return;;
    }
    //load succeed
    ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] = cargoOp.getContainer();
    cargoOp.getContainer()->setIsContainerLoaded(1);
    //load from remember
    if (containerFromList == nullptr) {
        ship->getShipMap().getContainerIDOnShip().insert(cargoOp.getContainer()->getId());
        auto itrRemember = rememberToLoadAgainIdToIndex.find(cargoOp.getContainer()->getId());
        if (itrRemember != rememberToLoadAgainIdToIndex.cend()) {
            rememberToLoadAgainIdToIndex.erase(itrRemember);
        }
    }
        //load from loadList
    else {
        auto itr = ship->getShipMap().getContainerIDOnShip().find(cargoOp.getContainer()->getId());
        if (itr != ship->getShipMap().getContainerIDOnShip().cend()) {
            errorList.emplace_back("should not load container because this id is already on the ship",
                                   SimErrorType::OPERATION_PORT, cargoOp);
            return;
        }
        ship->getShipMap().getContainerIDOnShip().insert(cargoOp.getContainer()->getId());
        if (cargoOp.getContainer()->getDestination() == currentPort) {
            errorList.emplace_back("should not load container with current port destination",
                                   SimErrorType::OPERATION_PORT, cargoOp);
            return;
        }
        if (containerFromList->getPortIndex() == 0) {
            errorList.emplace_back("should not load container with destination-" + containerFromList->getDestination() +
                                   " which is not in the ship's route", SimErrorType::OPERATION_PORT, cargoOp);
            return;
        }
        if (containerFromList->getPortIndex() > maxNumberPortLoaded) {
            maxNumberPortLoaded = containerFromList->getPortIndex();
        }
    }
}

void checkUnloadOperation(Ship *ship, CargoOperation &cargoOp, const string &currentPort,
                          map<string, CargoOperation *> &rememberToLoadAgainIdToIndex,
                          list<SimulatorError> &errorList) {
    indexAccessible(ship, cargoOp, errorList);
    shared_ptr<Container> containerOnTheShipInThisPlace = ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()];
    if (containerOnTheShipInThisPlace == nullptr ||
        containerOnTheShipInThisPlace == ship->getShipMap().getImaginary()) {
        errorList.emplace_back(
                "no container in this index to unload or this index is not part of the ship plan- operation ignored",
                SimErrorType::OPERATION_PORT,
                cargoOp);
        return;
    }
    if (containerOnTheShipInThisPlace->getId() != cargoOp.getContainer()->getId()) {
        errorList.emplace_back("the container id does not match container id on the ship- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    //unload succeed
    ship->getShipMap().getContainerIDOnShip().erase(
            ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()]->getId());
    ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] = nullptr;
    //maybe future error
    if (cargoOp.getContainer()->getDestination().compare(currentPort) != 0) {
        cargoOp.getContainer()->setIsContainerLoaded(0);
        rememberToLoadAgainIdToIndex[cargoOp.getContainer()->getId()] = &cargoOp;
    }
}

void checkMoveOperation(Ship *ship, CargoOperation &cargoOp, list<SimulatorError> &errorList) {
    indexAccessible(ship, cargoOp, errorList);
    shared_ptr<Container> containerOnTheShipInThisPlace = ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()];
    if (containerOnTheShipInThisPlace == nullptr ||
        containerOnTheShipInThisPlace == ship->getShipMap().getImaginary()) {
        errorList.emplace_back(
                "no container in this index to unload or this index is not part of the ship plan- operation ignored",
                SimErrorType::OPERATION_PORT,
                cargoOp);
        return;
    }
    if (containerOnTheShipInThisPlace->getId() != cargoOp.getContainer()->getId()) {
        errorList.emplace_back("the container id does not match container id on the ship- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    //check place to move
    if (!indexInLimit(ship, cargoOp.getMoveIndex())) {
        errorList.emplace_back("index to move is not in ship plan- operation ignored", SimErrorType::OPERATION_PORT,
                               cargoOp);
        return;
    }
    if (containerAbove(ship, cargoOp.getMoveIndex())) {
        errorList.emplace_back("cannot reach move index, it is blocked by containers above it- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    containerOnTheShipInThisPlace = ship->getShipMap().getShipMapContainer()[cargoOp.getMoveIndex().getHeight()][cargoOp.getMoveIndex().getRow()][cargoOp.getMoveIndex().getCol()];
    if (containerOnTheShipInThisPlace != nullptr) {
        errorList.emplace_back(
                "cannot move container to index occupied by other container, or space is not valid to this ship plan- operation ignored",
                SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    if (!solidGround(ship, cargoOp.getMoveIndex())) {
        errorList.emplace_back("cannot place container without underneath platform- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    containerOnTheShipInThisPlace = ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()];
    //move succeed
    ship->getShipMap().getShipMapContainer()[cargoOp.getMoveIndex().getHeight()][cargoOp.getMoveIndex().getRow()][cargoOp.getMoveIndex().getCol()] = containerOnTheShipInThisPlace;
    ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] = nullptr;

}

void checkRejectOperation(Ship *ship, CargoOperation &cargoOp, list<shared_ptr<Container>> &loadList,
                          int maxNumberPortLoaded,
                          list<SimulatorError> &errorList, const string &currentPort) {
    cargoOp.getContainer()->setIsContainerReject(1);
    //container destination is here no reason to load
    if (cargoOp.getContainer()->getDestination() == currentPort) {
        return;
    }
    //container illegal should reject
    if (!cargoOp.getContainer()->isContainerLegal()) {
        return;
    }
    int numIdInLoadList = 0;
    for (auto cont :loadList) {
        if (cont->getId() == cargoOp.getContainer()->getId()) {
            numIdInLoadList++;
        }
    }
    if (numIdInLoadList == 0) {
        errorList.emplace_back("rejected container with id which is not in the container list to load in this port",
                               SimErrorType::OPERATION_PORT,
                               cargoOp);
        return;
    }
    //check container Id on ship
    auto itr = ship->getShipMap().getContainerIDOnShip().find(cargoOp.getContainer()->getId());
    if (itr == ship->getShipMap().getContainerIDOnShip().cend()) {
        //destination in route
        if (cargoOp.getContainer()->getPortIndex() > 0) {
            MapIndex index = MapIndex::firstLegalIndexPlace(ship->getShipMap());
            if (index.validIndex()) {
                errorList.emplace_back(
                        "rejected container with valid destination while there is still place on the ship",
                        SimErrorType::OPERATION_PORT, cargoOp);
                return;
            } else {
                if (maxNumberPortLoaded > cargoOp.getContainer()->getPortIndex()) {
                    errorList.emplace_back(
                            "rejected container with closer destination and loaded container with a further destination instead",
                            SimErrorType::OPERATION_PORT, cargoOp);
                    return;
                }
            }
        }
    }
}

//make sure nothing left on port with no reason
void nothingLeftNoReason(map<string, CargoOperation *> &rememberToLoadAgainIdToIndex, list<SimulatorError> &errorList,
                         const string &currentPort) {
    for (auto pair : rememberToLoadAgainIdToIndex) {
        errorList.emplace_back(
                "unload container in port- " + currentPort + " instead of destination port " +
                pair.second->getContainer()->getDestination() +
                " ", SimErrorType::OPERATION_PORT, *pair.second);
    }
}

void checkAllContainersRejectedOrLoaded(list<shared_ptr<Container>> &loadList, list<SimulatorError> &errorList) {
    for (auto container: loadList) {
        if (container->getIsContainerLoaded() == 0 && container->getIsContainerReject() == 0) {
            errorList.emplace_back("container id- " + container->getId() + " was not rejected or loaded",
                                   SimErrorType::GENERAL_PORT);
            break;
        }
    }
}

list<SimulatorError>
Simulator::checkAlgoCorrect(Ship *ship, list<shared_ptr<CargoOperation>> &cargoOpsList,
                            list<shared_ptr<Container>> &loadList,
                            const string &currentPort, int &numberLoads, int &numberUnloads,list<SimulatorError>& errorList) {
    map<string, CargoOperation *> rememberToLoadAgainIdToIndex;
    int number = 1;
    int maxNumberPortLoaded = 0;
    orderListLoadContainer(loadList, ship->getShipRoute());
    loadList.sort(compByPortDest);
    for (auto cargoOp: cargoOpsList) {
        cargoOp->setPlaceInList(number);
        number++;
        AbstractAlgorithm::Action op = cargoOp->getOp();
        if(cargoOp->getContainer()!=nullptr) {
            if (this->calculator.tryOperation((char) op, cargoOp->getContainer()->getWeight(),
                                              cargoOp->getIndex().getCol(),
                                              cargoOp->getIndex().getRow()) !=
                WeightBalanceCalculator::BalanceStatus::APPROVED) {
                //TODO: calculator denied operation
                errorList.emplace_back("weight calculator does not approve this operation- operation ignored",
                                       SimErrorType::OPERATION_PORT, *cargoOp);
            } else {
                switch (op) {
                    case AbstractAlgorithm::Action::LOAD:
                        numberLoads++;
                        checkLoadOperation(ship, *cargoOp, loadList, rememberToLoadAgainIdToIndex, currentPort,
                                           maxNumberPortLoaded, errorList);
                        break;
                    case AbstractAlgorithm::Action::UNLOAD:
                        numberUnloads++;
                        checkUnloadOperation(ship, *cargoOp, currentPort, rememberToLoadAgainIdToIndex, errorList);
                        break;
                    case AbstractAlgorithm::Action::MOVE:
                        checkMoveOperation(ship, *cargoOp, errorList);
                        break;
                    case AbstractAlgorithm::Action::REJECT:
                        break;
                    default:
                        //TODO: deal with operation which not CargoOperation - maybe in targil 2
                        break;
                }
            }
        }
        else{
            errorList.emplace_back("operation number"+ std::to_string(cargoOp->getPlaceInList()) + "use unknown container ID",
                                   SimErrorType::GENERAL_PORT);
        }
    }
    for (auto cargoOp: cargoOpsList) {
        if (cargoOp->getOp() == AbstractAlgorithm::Action::REJECT) {
            checkRejectOperation(ship, *cargoOp, loadList, maxNumberPortLoaded, errorList, currentPort);
        }
    }
    nothingLeftNoReason(rememberToLoadAgainIdToIndex, errorList,
                        currentPort); //make sure nothing left on port with no reason
    checkAllContainersRejectedOrLoaded(loadList, errorList);
    checkIfAllUnloaded(ship, currentPort, errorList);
    return errorList;
}

Simulator::~Simulator() {
    for (AbstractAlgorithm *algo : algoList) {
        delete algo;
    }
}