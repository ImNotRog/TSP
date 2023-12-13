#include "graph_template.h"


struct Annealing : Tour {

    rgen rng;
    ld TEMPERATURE = 25;
    ld TEMP_STEP = 0.99;

    Annealing(vector<pld> cities, function<ld(pld, pld)> temp_dist) : Tour(cities, temp_dist)
    {
        for(int i = 0; i < n; i++) {
            path.push_back(i);
        } // initialize path

        for(int i = 0; i < 1000; i++) {
            
            int a = rand_node();
            int b = rand_node();

            for(int j = 0; j < 500; j++) {
                reverse();
                swap();
                joint_swap_all();
            }

            calc_path_length();
            // cout << path_length << endl;

            TEMPERATURE *= TEMP_STEP;
        }
    }

    void print_explicit_tour () {
        for (int node : path) {
            cout << "(" << city_coords[node].first << "," << city_coords[node].second << "), ";
        }
        cout << "(" << city_coords[path[0]].first << "," << city_coords[path[0]].second << ")" << endl;
    }

    void swap() {
        int ai = rand_node();
        int bi = ((int)floor(ai + 1 + rng.rand() * (n - 1))) % n;
        swap(ai,bi);
    }
    void swap(int ai, int bi) {     
        int a = path[ai];
        int b = path[bi];
        int ap = path[ (ai + (n-1)) % n ];
        int bp = path[ (bi + (n-1)) % n ];
        int an = path[ (ai + 1) % n ];
        int bn = path[ (bi + 1) % n ];

        ld edges_diff_a = adj_matrix[a][bp] + adj_matrix[a][bn] - ( adj_matrix[a][ap] + adj_matrix[a][an]);
        ld edges_diff_b = adj_matrix[b][ap] + adj_matrix[b][an] - ( adj_matrix[b][bp] + adj_matrix[b][bn]);

        ld diff = edges_diff_a + edges_diff_b;
        double rand = rng.rand();

        if(rand < exp( -diff / TEMPERATURE ) ) {
            // commit to the switch
            path[ai] = b;
            path[bi] = a;
        }
    }
    void swap_all() {
        for(int i = 0; i < n; i++) {
            for(int j = i+1; j < n; j++) {
                swap(i,j);
            }
        }
    }

    void joint_swap() {
        joint_swap(rand_node());
    }
    void joint_swap(int ai) {
        int bi = (ai + 1) % n;
        int a = path[ai];
        int b = path[bi];
        int ap = path[(ai + (n - 1)) % n];
        int bn = path[(bi + 1) % n];

        ld diff = adj_matrix[a][bn] - adj_matrix[a][ap] + adj_matrix[b][ap] - adj_matrix[b][bn];

        double rand = rng.rand();
        if (rand < exp(-diff / TEMPERATURE))
        {
            path[ai] = b;
            path[bi] = a;
        }
    }

    void joint_swap_all () {
        for(int i = 0; i < n; i++) joint_swap(i);
    }

    void reverse() {
        int ai = rand_node();
        int bi = ((int)floor(ai + 1 + rng.rand() * (n - 1))) % n;
        reverse(ai,bi);
    }
    void reverse (int ai, int bi) {
        
        int a = path[ai]; int b = path[bi];
        int ap = path[(ai + (n-1)) % n];
        int bn = path[(bi + 1) % n];

        ld diff = adj_matrix[a][bn] - adj_matrix[a][ap] + adj_matrix[b][ap] - adj_matrix[b][bn];

        double rand = rng.rand();
        if (rand < exp(-diff / TEMPERATURE)){

            // cout << diff << "< diff  ai,bi > " << ai << " " << bi << endl;
            if(bi < ai) {
                int temp = ai;
                ai = bi;
                bi = temp;
            }
            // print_path();
            for(int i = ai; i < (bi + ai)/2; i++) {
                int temp = path[i];
                path[i] = path[ bi - 1 - (i-ai) ];
                path[bi - 1 - (i - ai)] = temp;
            }
            // print_path();
        }
    }
    void reverse_all() {
        for(int i = 0; i < n; i++) {
            for(int j = i+1; j < n; j++) {
                reverse(i,j);
            }
        }
    }

    int rand_node() {
        return floor(rng.rand()*n);
    }
};

int main() {
    COORDINATE_LIST = {
        {1, 1},
        {1, -1},
        {0, -1},
        {-1, 1},
        {0, 1},
        {-1, 0},
        {-1, -1},
        {1, 0},
    };
    init("city_list.txt");
    vector<Tour> tours;
    for (int i = 0; i < 10; i++)
    {
        cout << i << "\n";
        tours.push_back(Annealing(COORDINATE_LIST, earth_dist));
    }
    sort(tours.begin(),tours.end());
    tours[0].print_path();
    //~ a.print_explicit_tour();
    return 0;
}
