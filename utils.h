#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <stack>
#include <array>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <functional>
#include <cassert>
#include <set>
#include <random>
#include "distance_functions.h"

using namespace std;
typedef long double ld;
typedef pair<ld, ld> pld;
typedef pair<int,int> pii;
const ld INF = 1e18;

int RGEN_IT = 0;

struct rgen
{
    uniform_real_distribution<double> dist;
    default_random_engine re;

    rgen()
    {
        uniform_real_distribution<double> dist(0, 1);
        re.seed(time(NULL) + RGEN_IT);
        RGEN_IT ++;
    }

    double rand()
    {
        return dist(re);
    }
};
