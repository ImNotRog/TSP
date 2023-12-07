#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <stack>
#include <array>

using namespace std;
typedef long double ld;
typedef pair<ld, ld> pld;
typedef pair<int,int> pii;

vector<pld> COORDINATE_LIST = {{6.59, 2.13}, {35.45, 46.55}, {5.13, 2.43}, {26.41, 18.36}, {0.06, 50.26}, {37.62, 0.17}, {0.03, 77.7}, {81.41, 73.5}, {3.85, 91.05}, {33.74, 60.32}, {0.78, 91.1}, {3.93, 0.63}, {30.66, 0.01}, {0.18, 0.27}, {39.55, 56.3}, {23.41, 40.06}, {7.66, 73.71}, {53.35, 27.24}, {3.83, 74.18}, {84.8, 2.4}, {1.21, 1.29}, {0.92, 74.36}, {46.1, 14.34}, {30.5, 1.13}, {5.78, 12.86}, {15.41, 4.9}, {33.41, 0.06}, {3.14, 0.97}, {38.17, 50.22}, {5.02, 49.5}, {87.43, 76.75}, {33.68, 3.44}, {3.59, 35.9}, {79.15, 6.44}, {6.29, 11.58}, {8.86, 0.12}, {82.64, 25.43}, {0.04, 37.08}, {25.43, 66.85}, {23.82, 0}, {1.92, 4.67}, {22.23, 60.22}, {24.91, 48.24}, {72.38, 15.79}, {14.98, 8.36}, {1.5, 48.84}, {7.94, 75.17}, {4.44, 0.33}, {24.4, 60.73}, {18.21, 4.09}, {4.01, 28.5}, {35.67, 0.17}, {76.35, 0.17}, {19.31, 13.07}, {8.3, 0.4}, {51.6, 0.04}, {4.52, 5.99}, {0, 7.91}, {2.09, 78.53}, {0.02, 12.3}, {20.16, 18.87}, {27.75, 30.9}, {3.42, 1.6}, {61.08, 78.66}, {21.24, 85.63}, {67.53, 80.38}, {20.2, 13.74}, {6.28, 0.01}, {45.26, 5.06}, {61.15, 84.09}, {90.89, 12.23}, {0, 0.87}, {95.64, 0.58}, {4.33, 36.61}, {97.73, 0.2}, {79.73, 4.61}, {81.85, 0.02}, {80.19, 48.57}, {90.04, 5.35}, {0.88, 10.8}, {40.73, 0.87}, {21.2, 0.1}, {0.13, 65.88}, {46.15, 38.63}, {29.95, 72.5}, {1.43, 65.64}, {31.12, 0}, {12.28, 6.28}, {2, 77.05}, {0.4, 0.03}, {4.71, 2.24}, {16.03, 65.01}, {61.51, 72.02}, {70.2, 7.06}, {2.35, 74.49}, {0.02, 37.5}, {0.4, 0.02}, {24.46, 2.75}, {12.42, 5.77}, {0.22, 0.8}, {400, 50}};

const ld INF = 1e18;

const int BRANCH_BOTH_BELOW = 2;
const int BRANCH_ONE_ABOVE = 5;

ld sq(ld x)
{
    return x * x;
}

ld eucl_dist(pld x, pld y)
{
    return sqrtl(sq(x.first - y.first) + sq(x.second - y.second));
}

struct LKH_step {
    int node;
    int index;
    ld gain;
    LKH_step( int n, int i, ld g ) {
        node = n; index = i; gain = g;
    }
};

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

struct LKH : Tour {

    // change is an alternating path, where we try to get tour XOR change to be hamiltonian
    vector<int> change;

    vector<int> tour = vector<int>(n);                     // index in tour --> vertex
    vector<int> tour_inv = vector<int>(n);                   // vertex --> index_in_tour

    LKH(vector<pld> cities, function<ld(pld, pld)> temp_dist) : Tour(cities, temp_dist) {
        // constructor code
        for(int i = 0; i < n; i++) tour[i] = i; // init default path
        // cout << change_is_hamiltonian() << endl;

        int iterations = 0;
        while(true) {
            bool found = find_positive_change();

            if(!found) break;

            update_tour_with_change();
            // print_tour();
            // print_explicit_tour();
            iterations ++;
            cout << "ITERATION: " << iterations << endl;
        }

        print_tour();
        print_explicit_tour();
    }

    ld tour_length () {
        ld len = 0;
        for(int i = 0; i < n; i++) {
            len += adj_matrix[tour[i]][tour[ (i+1) % n]];
        }
        return len;
    }

    void print_tour () {
        cout << "length: " << tour_length() << endl;
        cout << "path: ";
        for(int node : tour) {
            cout << node << " ";
        }
        cout << endl;
    }

    void print_explicit_tour () {
        for (int node : tour) {
            cout << "(" << city_coords[node].first << "," << city_coords[node].second << "), ";
        }
        cout << "(" << city_coords[0].first << "," << city_coords[0].second << ")" << endl;
    }

    bool find_positive_change () {

        update_tour_inv();
        change.clear();

        stack<LKH_step> LKH_stack;

        for (int i = 0; i < n; i++)
            LKH_stack.push(LKH_step(i, 0, 0));

        int num_steps = 0;

        while (!LKH_stack.empty())
        {
            num_steps ++;

            LKH_step prev = LKH_stack.top();

            LKH_stack.pop();

            if (change.size() > prev.index)
            {
                change[prev.index] = prev.node;
                while (change.size() > prev.index + 1)
                    change.pop_back(); // reset state
            }
            else
            {
                change.push_back(prev.node);
            }
            // for( int node : change ) {
            //     cout << node << " ";
            // }
            // cout << endl;

            // two options
            vector<int> adjacencies = {tour[(tour_inv[prev.node] + 1) % n], tour[(tour_inv[prev.node] + n - 1) % n]};

            if (prev.index % 2 == 0)
            {
                // index even, we wish to find a link in the current tour

                for (int node : adjacencies)
                {

                    bool already_added = false;
                    for (int i = 0; i < change.size(); i++)
                    {
                        if (change[i] == node)
                            already_added = true;
                    }

                    if (!already_added)
                    {
                        if (prev.index <= BRANCH_BOTH_BELOW)
                        { // or closed
                            LKH_stack.push(LKH_step(node, prev.index + 1, prev.gain + adj_matrix[prev.node][node]));
                        }
                        else if (!are_tour_adjacent(node, change[0]))
                        {

                            // we can try closing the loop
                            change.push_back(node);

                            if (change_is_hamiltonian())
                            {
                                // if we can close it, then sure, add it
                                LKH_stack.push(LKH_step(node, prev.index + 1, prev.gain + adj_matrix[prev.node][node]));
                            }

                            change.pop_back();


                        }
                    }
                }
            }
            else
            {
                // index odd, we wish to find a link not in the current tour

                // first: check if we can immediately close the tour, then we win
                if( !are_tour_adjacent(prev.node, change[0]) ) {
                    if(prev.gain - adj_matrix[prev.node][change[0]] > 0) {
                        if(change_is_hamiltonian()) {
                            
                            cout << num_steps << endl;

                            return true;
                        }
                    }
                }

                // otherwise, find a link not in the current tour
                ld max_gain = -1;
                int max_node = -1;

                for (int node = 0; node < n; node++)
                {
                    if (adjacencies[0] == node || adjacencies[1] == node)
                        continue;

                    bool already_added = false;
                    for (int i = 0; i < change.size(); i++)
                    {
                        if (change[i] == node)
                            already_added = true;
                    }
                    if (already_added)
                        continue;

                    if (prev.index >= BRANCH_ONE_ABOVE)
                    {
                        if (prev.gain - adj_matrix[prev.node][node] > max_gain)
                        {
                            max_gain = prev.gain - adj_matrix[prev.node][node];
                            max_node = node;
                        }
                    }
                    else
                    {
                        if (prev.gain - adj_matrix[prev.node][node] > 0)
                        {
                            // add if gain > 0
                            LKH_stack.push(LKH_step(node, prev.index + 1, prev.gain - adj_matrix[prev.node][node]));
                        }
                    }
                }

                if (prev.index >= BRANCH_ONE_ABOVE && max_gain > 0)
                {
                    LKH_stack.push(LKH_step(max_node, prev.index + 1, max_gain));
                }
            }
        }

        return false;
    }

    void update_tour_inv() {
        for(int i = 0; i < n; i++) { 
            tour_inv[tour[i]] = i;
        }
    }

    // the next 3 assume tour_inv is updated

    bool are_tour_adjacent(int node1, int node2)
    {
        int index1 = tour_inv[node1];
        int index2 = tour_inv[node2];
        if (abs(index1 - index2) <= 1 || abs(index1 - index2) == n - 1)
            return true;
        return false;
    }

    void update_tour_with_change () {
        // annoying ass implementation

        vector<int> new_tour = vector<int>(n);


        int m = change.size();

        vector<pii> links;        // given by index in tour
        vector<int> change_nodes; // given by index in tour
        for (int i = 0; i < m; i++)
        {
            links.push_back({tour_inv[change[i]], tour_inv[change[(i + 1) % m]]}); // even = in tour, odd = out
            change_nodes.push_back(tour_inv[change[i]]);
        }

        int current_node = 0;
        int direction = 1;

        for(int t = 0; t < n; t++)
        {
            new_tour[t] = tour[current_node];

            bool jumped = false;
            for (int i = 1; i < m; i += 2) {
                if (links[i].first == current_node)
                {
                    current_node = links[i].second;
                    jumped = true;
                    break;
                }
                if (links[i].second == current_node)
                {
                    jumped = true;
                    current_node = links[i].first;
                    break;
                }
            }

            if(jumped) {
                // check direction

                direction = 1;

                // one way is blocked off
                // traverse along graph
                for (int i = 0; i < m; i++)
                {
                    if (links[i] == make_pair(current_node, (current_node + 1) % n) || links[i] == make_pair((current_node + 1) % n, current_node))
                    {
                        direction = -1; // then we're going backwards
                    }
                }

                t++;
                new_tour[t] = tour[current_node];
            }

            // traverse tour
            current_node += direction;
        }

        tour = new_tour;
    }

    bool change_is_hamiltonian() {
        // check if tour XOR change is hamiltonian
        // O(m^2)

        // cout << "RUNNING!!" << endl;

        int m = change.size();

        vector<pii> links; // given by index in tour
        vector<int> change_nodes; // given by index in tour
        for(int i = 0; i < m; i++) {
            links.push_back({ tour_inv[change[i]], tour_inv[change[(i+1)%m]] }); // even = in tour, odd = out
            // cout << links[i].first << " " << links[i].second << endl;
            change_nodes.push_back( tour_inv[change[i]] );
        }

        // vector<bool> nodes_visited = vector<bool>(n);
        vector<bool> change_nodes_visited = vector<bool>(m);

        int current_node = change_nodes[0]; // given by index in tour

        while(true) {
            
            // visit node
            bool repeated = false;
            for(int i = 0 ; i < m; i++) {
                if( current_node == ( change_nodes[i] + 3 * n ) % n ) {
                    if ( change_nodes_visited[i] ) {
                        repeated = true;
                    }
                    change_nodes_visited[i] = true;
                }
            }
            if(repeated) break;

            int direction = 1;
            // one way is blocked off
            // traverse along graph
            for(int i = 0; i < m; i++) {
                if (links[i] == make_pair(current_node, (current_node + 1) % n) 
                    || links[i] == make_pair((current_node + 1) % n, current_node)) {
                    direction = -1; // then we're going backwards
                }
            }

            if(direction == 1) {
                for (int i = 0; i < m; i++)
                {
                    while (change_nodes[i] > current_node) change_nodes[i] -= n;
                    while (change_nodes[i] < current_node) change_nodes[i] += n; // get every change node above current node
                }

                int min_node = 10 * n;

                for (int i = 0; i < m; i++) {
                    if(change_nodes[i] < min_node && change_nodes[i] > current_node) {
                        min_node = change_nodes[i];
                    }
                }

                current_node = min_node % n;
            }

            if(direction == -1) {
                for (int i = 0; i < m; i++)
                {
                    while (change_nodes[i] < current_node) change_nodes[i] += n;
                    while (change_nodes[i] > current_node) change_nodes[i] -= n; // get every change node below current node
                }

                int max_node = -(10 * n);
                for (int i = 0; i < m; i++)
                {
                    if (change_nodes[i] > max_node && change_nodes[i] < current_node)
                        max_node = change_nodes[i];
                }

                current_node = (max_node + 2 * n ) % n;
            }

            // cout << "B: " << current_node << endl;

            // visit node
            repeated = false;
            for(int i = 0 ; i < m; i++) {
                if( current_node == ( change_nodes[i] + 3 * n ) % n ) {
                    if ( change_nodes_visited[i] ) {
                        repeated = true;
                    }
                    change_nodes_visited[i] = true;
                }
            }
            if(repeated) break;

            // traverse across odd edge
            for(int i = 1; i < m; i += 2) {
                if(links[i].first == current_node) {
                    current_node = links[i].second;
                    break;
                }
                if(links[i].second == current_node) {
                    current_node = links[i].first;
                    break;
                }
            }
        }

        for(int i = 0; i < m; i++) {
            if(!change_nodes_visited[i]) {
                return false;
            }
        }

        return true;
    }
};

int main() {
    LKH tour = LKH(COORDINATE_LIST, eucl_dist);
    // tour.print_path();
    return 0;
}

