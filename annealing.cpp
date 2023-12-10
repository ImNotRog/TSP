#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <random>

using namespace std;
typedef long double ld;
typedef pair<ld, ld> pld;
typedef pair<int, int> pii;

const ld INF = 1e18;

vector<pld> COORDINATE_LIST = {{6.59, 2.13}, {35.45, 46.55}, {5.13, 2.43}, {26.41, 18.36}, {0.06, 50.26}, {37.62, 0.17}, {0.03, 77.7}, {81.41, 73.5}, {3.85, 91.05}, {33.74, 60.32}, {0.78, 91.1}, {3.93, 0.63}, {30.66, 0.01}, {0.18, 0.27}, {39.55, 56.3}, {23.41, 40.06}, {7.66, 73.71}, {53.35, 27.24}, {3.83, 74.18}, {84.8, 2.4}, {1.21, 1.29}, {0.92, 74.36}, {46.1, 14.34}, {30.5, 1.13}, {5.78, 12.86}, {15.41, 4.9}, {33.41, 0.06}, {3.14, 0.97}, {38.17, 50.22}, {5.02, 49.5}, {87.43, 76.75}, {33.68, 3.44}, {3.59, 35.9}, {79.15, 6.44}, {6.29, 11.58}, {8.86, 0.12}, {82.64, 25.43}, {0.04, 37.08}, {25.43, 66.85}, {23.82, 0}, {1.92, 4.67}, {22.23, 60.22}, {24.91, 48.24}, {72.38, 15.79}, {14.98, 8.36}, {1.5, 48.84}, {7.94, 75.17}, {4.44, 0.33}, {24.4, 60.73}, {18.21, 4.09}, {4.01, 28.5}, {35.67, 0.17}, {76.35, 0.17}, {19.31, 13.07}, {8.3, 0.4}, {51.6, 0.04}, {4.52, 5.99}, {0, 7.91}, {2.09, 78.53}, {0.02, 12.3}, {20.16, 18.87}, {27.75, 30.9}, {3.42, 1.6}, {61.08, 78.66}, {21.24, 85.63}, {67.53, 80.38}, {20.2, 13.74}, {6.28, 0.01}, {45.26, 5.06}, {61.15, 84.09}, {90.89, 12.23}, {0, 0.87}, {95.64, 0.58}, {4.33, 36.61}, {97.73, 0.2}, {79.73, 4.61}, {81.85, 0.02}, {80.19, 48.57}, {90.04, 5.35}, {0.88, 10.8}, {40.73, 0.87}, {21.2, 0.1}, {0.13, 65.88}, {46.15, 38.63}, {29.95, 72.5}, {1.43, 65.64}, {31.12, 0}, {12.28, 6.28}, {2, 77.05}, {0.4, 0.03}, {4.71, 2.24}, {16.03, 65.01}, {61.51, 72.02}, {70.2, 7.06}, {2.35, 74.49}, {0.02, 37.5}, {0.4, 0.02}, {24.46, 2.75}, {12.42, 5.77}, {0.22, 0.8}, {400, 50}, {0, 1000}};
vector<string> CITY_ORDER;

struct rgen
{
    uniform_real_distribution<double> dist;
    default_random_engine re;
    rgen()
    {
        uniform_real_distribution<double> dist(0, 1);
        re.seed(time(NULL));
    }

    double rand()
    {
        return dist(re);
    }
};

ld sq(ld x)
{
    return x * x;
}

ld eucl_dist(pld x, pld y)
{
    return sqrtl(sq(x.first - y.first) + sq(x.second - y.second));
}

void init()
{
    ifstream fin("city_list.txt");
    COORDINATE_LIST.clear();
    string city, state;
    ld x, y;
    while (fin >> city >> state >> x >> y)
    {
        COORDINATE_LIST.push_back({x, y});
        CITY_ORDER.push_back(city);
    }
}

struct Graph
{
    int n;
    vector<pld> city_coords;
    vector<vector<ld>> adj_matrix;
    function<ld(pld, pld)> dist;
    Graph(vector<pld> cities, function<ld(pld, pld)> temp_dist)
    {
        n = cities.size();
        city_coords = cities;
        dist = temp_dist;
        adj_matrix = vector<vector<ld>>(n, vector<ld>(n));
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                adj_matrix[i][j] = dist(city_coords[i], city_coords[j]);
            }
        }
    }

    int size()
    {
        return n;
    }

    ld get_weight(int start, int end)
    {
        return adj_matrix[start][end];
    }

    ld get_index(pld cur_city)
    {
        for (int i = 0; i < city_coords.size(); i++)
        {
            if (city_coords[i] == cur_city)
            {
                return i;
            }
        }
        return -1;
    }
};

struct Tour : Graph
{
    vector<bool> visited;
    vector<int> path;
    ld path_length;
    Tour(vector<pld> cities, function<ld(pld, pld)> temp_dist) : Graph(cities, temp_dist)
    {
        path_length = INF;
        visited = vector<bool>(n);
    }

    bool operator<(const Tour &other) { return path_length < other.path_length; }

    bool check_vis(int x)
    {
        return visited[x];
    }

    void visit(int x)
    {
        assert(!visited[x]);
        visited[x] = 1;
    }

    void calc_path_length()
    {
        path_length = 0;
        for (int i = 1; i < n; i++)
        {
            path_length = path_length + adj_matrix[path[i]][path[i - 1]];
        }
        path_length = path_length + adj_matrix[path[n - 1]][path[0]];
    }

    void print_path()
    {
        calc_path_length();
        cout << "length: " << path_length << "\n";
        cout << "path: ";
        for (auto vertex : path)
        {
            cout << vertex << " ";
        }
        cout << "\n";
    }
};

struct Annealing : Tour {

    rgen rng;
    ld TEMPERATURE = 1;
    ld TEMP_STEP = -0.0001;

    Annealing(vector<pld> cities, function<ld(pld, pld)> temp_dist) : Tour(cities, temp_dist)
    {
        for(int i = 0; i < n; i++) {
            path.push_back(i);
        } // initialize path

        while(true) {
            
            int a = rand_node();
            int b = rand_node();

            disjoint_swap();

            TEMPERATURE += TEMP_STEP;

            if(TEMPERATURE <= 0) break;
        }
    }

    void print_explicit_tour () {
        for (int node : path) {
            cout << "(" << city_coords[node].first << "," << city_coords[node].second << "), ";
        }
        cout << "(" << city_coords[path[0]].first << "," << city_coords[path[0]].second << ")" << endl;
    }

    void disjoint_swap() { 
        int ai = rand_node();
        int bi = ((int) floor( ai + 1 + rng.rand() * (n-2) )) % n;
        int a = path[ai];
        int b = path[bi];
        int ap = path[ (ai + (n-1)) % n ];
        int bp = path[ (bi + (n-1)) % n ];
        int an = path[ (ai + 1) % n ];
        int bn = path[ (bi + 1) % n ];

        ld edges_diff_a = adj_matrix[a][bp] + adj_matrix[a][bn] -( adj_matrix[a][ap] + adj_matrix[a][an]);
        ld edges_diff_b = adj_matrix[b][ap] + adj_matrix[b][an] -( adj_matrix[b][bp] + adj_matrix[b][bn]);

        ld diff = edges_diff_a + edges_diff_b;
        double rand = rng.rand();

        if(rand < exp( diff / TEMPERATURE ) ) {
            // commit to the switch
            path[ai] = b;
            path[bi] = a;
        }

    }

    void joint_swap () {

    }

    int rand_node() {
        return floor(rng.rand()*n);
    }
};

int main() {
    init();
    Annealing a = Annealing(COORDINATE_LIST, eucl_dist);
    a.print_path();
    a.print_explicit_tour();
    return 0;
}
