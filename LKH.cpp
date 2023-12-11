#include "graph_template.h"

const int BRANCH_BOTH_BELOW = 2;
const int CONSIDER_ALL_BELOW = 7;
const int BRANCH_ONE_ABOVE = 7;
const int NUM_CLOSEST_NEIGHBORS = 10;

struct LKH_step {
    int node;
    int index;
    ld gain;
    LKH_step( int n, int i, ld g ) {
        node = n; index = i; gain = g;
    }
};

struct LKH : Tour {

    // change is an alternating path, where we try to get tour XOR change to be hamiltonian
    vector<int> change;

    vector<vector<int>> closest_neighbors = vector<vector<int>>(n);

    vector<int> tour = vector<int>(n);                     // index in tour --> vertex
    vector<int> tour_inv = vector<int>(n);                   // vertex --> index_in_tour

    LKH(vector<pld> cities, function<ld(pld, pld)> temp_dist) : Tour(cities, temp_dist) {
        // constructor code
        for(int i = 0; i < n; i++) tour[i] = i; // init default path

        // find closest neighbors
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                if(i == j) continue;
                closest_neighbors[i].push_back( j );
                sort(closest_neighbors[i].begin(), closest_neighbors[i].end(), [this,i] (int a, int b) -> bool { return adj_matrix[i][a] < adj_matrix[i][b]; });

                if(closest_neighbors[i].size() > NUM_CLOSEST_NEIGHBORS) {
                    closest_neighbors[i].pop_back();
                }
            }
        }

        int iterations = 0;
        while(true) {
            bool found = find_positive_change();

            if(!found) break;

            update_tour_with_change();

            // print_explicit_tour();
            iterations ++;
            cout << "ITERATION: " << iterations << endl;
            print_tour();
        }

        // print_tour();
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
        cout << "(" << city_coords[tour[0]].first << "," << city_coords[tour[0]].second << ")" << endl;
    }

    bool find_positive_change () {

        update_tour_inv();
        change.clear();

        stack<LKH_step> LKH_stack;

        for (int i = 0; i < n; i++)
            LKH_stack.push(LKH_step(tour[i], 0, 0));

        int num_steps = 0;

        while (!LKH_stack.empty()) // should run O(n^2) times
        {
            // each step should be constant time

            num_steps += change.size() * change.size();

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
                            
                            // cout << num_steps << endl;
                            // cout << change.size() << endl;

                            return true;
                        }
                    }
                }

                // otherwise, find a link not in the current tour
                ld max_gain = -1;
                int max_node = -1;

                vector<int> nodes_to_consider;
                if( prev.index <= CONSIDER_ALL_BELOW ) {
                    for(int node = 0; node < n; node++) {
                        nodes_to_consider.push_back(node);
                    }
                } else {
                    for(int k = 0; k < closest_neighbors[prev.node].size(); k++) {
                        nodes_to_consider.push_back(closest_neighbors[prev.node][k]);
                    }
                }

                for (int node : nodes_to_consider)
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

        int current_node = change_nodes[0];

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
            current_node = (current_node + n + direction) % n;
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
    init("city_list.txt");
    LKH tour = LKH(COORDINATE_LIST, earth_dist);
    // tour.print_path();
    return 0;
}

