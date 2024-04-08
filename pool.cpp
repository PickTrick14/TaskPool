#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <stack>
#include "pool.h"

unsigned long long GraphPool::GetId() const {
    return rand() % size;
}

