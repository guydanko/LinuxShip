#include "ShipMap.h"


ShipMap::ShipMap(int height, int rows, int cols) : height(height), rows(rows), cols(cols) {
    imaginaryContainer = new Container();
    shipMapContainer.resize(height);
    for (int i = 0; i < height; i++) {
        shipMapContainer[i].resize(rows);
        for (int j = 0; j < rows; j++) {
            shipMapContainer[i][j].resize(cols);
            for (int k = 0; k < cols; k++) {
                shipMapContainer[i][j][k] = nullptr;
            }
        }
    }
}

ShipMap::ShipMap(const ShipMap &from) : height(from.getHeight()), rows(from.getRows()), cols(from.getCols()) {
    *this = from;
}

ShipMap &ShipMap::operator=(const ShipMap &other) {
    if (this != &other) {
        imaginaryContainer = new Container();
        shipMapContainer.resize(height);
        for (int i = 0; i < height; i++) {
            shipMapContainer[i].resize(rows);
            for (int j = 0; j < rows; j++) {
                shipMapContainer[i][j].resize(cols);
                for (int k = 0; k < cols; k++) {
                    if (other.shipMapContainer[i][j][k] != nullptr) {
                        if (other.shipMapContainer[i][j][k] == other.getImaginary()) {
                            shipMapContainer[i][j][k] = imaginaryContainer;
                        }
                    } else {
                        shipMapContainer[i][j][k] = nullptr;
                    }
                }
            }
        }
    }
    return *this;
}

void ShipMap::initShipMapContainer(int height, int row, int col) {
    for (int i = 0; i < height; i++) {
        shipMapContainer[i][row][col] = imaginaryContainer;
    }
}

void ShipMap::clearContainers() {
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < rows; j++) {
            for (int k = 0; k < cols; k++) {
                if (shipMapContainer[i][j][k] != imaginaryContainer) {
                    shipMapContainer[i][j][k] = nullptr;
                }
            }
        }
    }
}