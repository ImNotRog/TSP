#include "graph_template.h"

// DOESNT WORK

struct AntColony : Tour {
    rgen rng;
    vector<vector<ld>> pheremones;
    vector<vector<ld>> pheremone_updates;

    ld DECAY = 0.05;
    int ANTS_PER = 10;
    int ITERATIONS = 300;
    double ALPHA = 1;

    ld avg_path_length = 0;
    int ants = 0;

    vector<int> best_path;
    ld best_path_length;

    AntColony(vector<pld> cities, function<ld(pld, pld)> temp_dist) : Tour(cities, temp_dist)
    {
        // init pheremones
        for(int i = 0; i < n; i++) {
            pheremones.push_back(vector<ld>(n));
            pheremone_updates.push_back(vector<ld>(n));

            for(int j = 0; j < n; j++) {
                pheremones[i][j] = 1 / adj_matrix[i][j]; // give the ants some ideas yk
            }
        }

        for(int i = 0; i < ITERATIONS; i++) {
            for(int j = 0; j < ANTS_PER; j++) {
                ant();
            }
            update();
            ALPHA = 1 + ((double) i / ITERATIONS) * 10;
        }

        path = best_path;
    }

    int next() {
        int node = path[path.size() - 1];
        
        ld total = 0;
        for(int i = 0; i < n; i++) {
            if(!visited[i] && i != node) {
                total += pow(pheremones[node][i], ALPHA);
            }
        }
        ld rand_num = total * rng.rand();
        for(int i = 0; i < n; i++) {
            if(!visited[i] && i != node) {
                rand_num -= pow(pheremones[node][i], ALPHA);
                if(rand_num < 0) return i;
            }
        }

        return -1;
    }

    void ant() {
        int start_node = rng.rand() * n;
        path = { start_node };
        for(int i = 0; i < n; i++) visited[i] = false;
        for(int i = 0; i < n-1; i++) {
            path.push_back(next());
            visited[path[i]] = true;
        }

        calc_path_length();
        avg_path_length += path_length;
        ants ++;
        ld pheremone_update = pow(path_length, -1);

        if(path_length < best_path_length) best_path = path;

        for(int i = 0; i < n; i++) {
            pheremone_updates[ path[i] ][ path[(i+1)%n] ] += pheremone_update;
        }
    }

    void update() {
        for(int i = 0; i < n; i++)
        {
            // cout << pheremones[i][(i + 1) % n] << " " << pheremone_updates[i][(i + 1) % n] << endl;

            for(int j = 0; j < n; j++) {
                pheremones[i][j] *= 1 - DECAY;
                pheremones[i][j] += pheremone_updates[i][j] / ants;
                pheremone_updates[i][j] = 0;
            }
        }

        avg_path_length /= ants;
        // cout << "AVERAGE PATH LENGTH: " << avg_path_length << endl;
        // print_path();

        ants = 0;
        avg_path_length = 0;
    }

};

int main()
{
    // COORDINATE_LIST = {
    //     {1, 1},
    //     {1, -1},
    //     {0, -1},
    //     {-1, 1},
    //     {0, 1},
    //     {-1, 0},
    //     {-1, -1},
    //     {1, 0},
    // };
    init("small_city_list.txt");
    AntColony ac = AntColony(COORDINATE_LIST, earth_dist);
    vector<Tour> tours;
    for (int i = 0; i < 10; i++) {
        
        tours.push_back(AntColony(COORDINATE_LIST, earth_dist));
        cout << i << " " << tours[i].path_length << "\n";
    }
    sort(tours.begin(), tours.end(),[](Tour a, Tour b) -> bool { return a.path_length < b.path_length; });
    tours[0].print_path();
    //~ a.print_explicit_tour();
    return 0;
}

