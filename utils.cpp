#include "utils.h"

int randomInt(int low, int high) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dist(low, high - 1);
    return dist(gen);
}


