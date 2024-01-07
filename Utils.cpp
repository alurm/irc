#include "Utils.hpp"
#include <stdio.h>
int countParams(char **params) {
    int count = 0;
    while (params[count]) {
        count++;
    }
    return count;
}
