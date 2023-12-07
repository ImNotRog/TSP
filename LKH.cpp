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

vector<pld> COORDINATE_LIST = {{0,0}, {1, 0}, {0, 1}, {1, 1}, {2,1}, {3,1}, {5,1}};
const ld INF = 1e18;

const int BRANCH_ALL_BELOW = 2;

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
        update_tour_inv();

        // change = { 0, 6, 3, 4 };
        // cout << change_is_hamiltonian() << endl;

        stack<LKH_step> LKH_stack;

        for(int i = 0; i < n; i++) LKH_stack.push( LKH_step( i, 0, 0 ));

        while( !LKH_stack.empty() ) {
            LKH_step prev = LKH_stack.top();
            LKH_stack.pop();
            
            change[prev.index] = prev.node;
            while( change.size() > prev.index + 1 ) change.pop_back(); // reset state

            if(prev.index % 2 == 0) {
                // inf even, we wish to find a link in the current tour

                if (prev.index <= BRANCH_ALL_BELOW) {
                    
                }
            }
        }
    }

    void update_tour_inv() {
        for(int i = 0; i < n; i++) { 
            tour_inv[tour[i]] = i;
        }
    }

    void update_tour_with_change () {
        // annoying ass implementatoin
    }

    bool change_is_hamiltonian() {
        // check if tour XOR change is hamiltonian
        // O(m^2)

        int m = change.size();

        vector<pii> links; // given by index in tour
        vector<int> change_nodes; // given by index in tour
        for(int i = 0; i < m; i++) {
            links.push_back({ tour_inv[change[i]], tour_inv[change[(i+1)%m]] }); // even = in tour, odd = out
            // cout << links[i].first << " " << links[i].second << endl;
            change_nodes.push_back( tour_inv[change[i]] );
        }

        vector<bool> nodes_visited = vector<bool>(n);

        int current_node = change_nodes[0];

        while(true) {

            if(nodes_visited[current_node]) break;
            nodes_visited[current_node] = true;

            int direction = 1;
            // one way is blocked off
            // traverse along graph
            for(int i = 0; i < m; i++) {
                if (links[i] == make_pair(current_node, (current_node + 1) % n) 
                    || links[i] == make_pair((current_node + 1) % n, current_node)) {
                    direction = -1; // then we're going backwards
                }
            }

            // cout << "DIR: " << direction << endl;

            if(direction == 1) {
                for (int i = 0; i < m; i++)
                {
                    while (change_nodes[i] > current_node) change_nodes[i] -= n;
                    while (change_nodes[i] < current_node) change_nodes[i] += n; // get every change node above current node

                    // cout << change_nodes[i] << endl;
                }

                int min_node = 10 * m;

                for (int i = 0; i < m; i++) {
                    if(change_nodes[i] < min_node && change_nodes[i] > current_node) {
                        min_node = change_nodes[i];
                    }
                }

                current_node = min_node;
            }

            if(direction == -1) {
                for (int i = 0; i < m; i++)
                {
                    while (change_nodes[i] < current_node) change_nodes[i] += n;
                    while (change_nodes[i] > current_node) change_nodes[i] -= n; // get every change node below current node
                }

                int max_node = -(10 * m);
                for (int i = 0; i < m; i++)
                {
                    if (change_nodes[i] > max_node && change_nodes[i] < current_node)
                        max_node = change_nodes[i];
                }

                current_node = max_node;
            }

            // cout << "B: " << current_node << endl;

            if (nodes_visited[current_node]) break;
            nodes_visited[current_node] = true;

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
            if(!nodes_visited[change_nodes[i]]) {
                // cout << i << " " << change[i] << endl;
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

