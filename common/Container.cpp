#include <string>
#include "Container.h"
#include <cmath>

using std::string ;

bool isOwnerCodeValid(const string &ownerCode) {
    if (ownerCode.length() < 4) {
        return false;
    }
    for (int i = 0; i < 3; ++i) {
        if (!isalpha(ownerCode[i])) {
            return false;
        }
    }
    return !(toupper(ownerCode[3]) != 'U' && toupper(ownerCode[3]) != 'J' && toupper(ownerCode[3]) != 'Z');
}

bool isSerialNumberValid(const string &ownerCode) {
    if (ownerCode.length() != 11 || !isdigit(ownerCode[10])) {
        return false;
    }
    int letterArray[26] = {10, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 34, 35,
                           36, 37, 38};
    int sum = 0;
    for (int i = 0; i < 4; i++) {
        sum += letterArray[toupper(ownerCode[i]) - 65] * pow(2, i);
    }
    for (int i = 4; i < 10; i++) {
        if(isdigit(ownerCode[i])) {
            sum += (ownerCode[i] - 48) * pow(2, i);
        } else{
            return false;
        }
    }
    int afterOp = (sum / 11) * 11;
    return (sum - afterOp)%10 == (ownerCode[10] - 48);

}

bool Container::isLegalId(const string &id) {
    return (isOwnerCodeValid(id) && isSerialNumberValid(id));
}

bool Container::isPortValid(const string &port) {
    if (port.length() != 5) {
        return false;
    }
    for (int i = 0; i < 5; i++) {
        if (!isalpha(port[i])) {
            return false;
        }
    }
    return true;
}

bool Container::isLegalParamContainer(int weight, string& destination, string& id) {
    return (weight > 0 && isPortValid(destination) && isLegalId(id));
}
