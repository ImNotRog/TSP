#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

using namespace std;

struct rgen {
    uniform_real_distribution<double> dist;
    default_random_engine re;
    rgen() {
        uniform_real_distribution<double> dist(0, 1);
        re.seed(time(NULL));
    }

    double rand() {
        return dist(re);
    }
};