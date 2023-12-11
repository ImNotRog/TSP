#include "graph_template.h"

struct ExactSolution : Tour
{
    vector<vector<ld>> dist;
    vector<vector<int>> prev;
    int final_vertex;
    
    
    void reconstruct_path()
    {
        int current_vertex = final_vertex;
        int current_mask = (1 << n) - 1;

        while (current_mask > 0)
        {
            path.push_back(current_vertex);
            int temp_vertex = current_vertex;
            current_vertex = prev[current_mask][current_vertex];
            current_mask ^= (1 << temp_vertex);
            
        }
        path.push_back(final_vertex);
    }
    ExactSolution (vector<pld> cities, function<ld(pld, pld)> temp_dist) : Tour(cities, temp_dist) 
    {
        dist = vector<vector<ld>> ((1 << n), vector<ld> (n, INF));
        prev = vector<vector<int>> ((1 << n), vector<int> (n));
        dist[1][0] = 0;
        for (int mask = 0; mask < (1 << n); mask++)
        {
            for (int j = 0; j < n; j++)
            {
                if (mask & (1 << j))
                {
                    for (int k = 0; k < n; k++)
                    {
                        if (j != k && mask & (1 << k))
                        {
                            ld new_dist = dist[mask^(1 << k)][j] + adj_matrix[j][k];
                            if (new_dist < dist[mask][k])
                            {
                                dist[mask][k] = new_dist;
                                prev[mask][k] = j;
                            }
                        }
                    }
                }
            }
        }
        ld min_dist  = INF;
        final_vertex = -1;
        for (int i = 0; i < n; i++)
        {
            if (dist[(1 << n) - 1][i] + adj_matrix[i][0] < min_dist)
            {
                min_dist = dist[(1 << n) - 1][i] + adj_matrix[i][0];
                final_vertex = i;
            }
        }
        path_length = min_dist;
        reconstruct_path();
    }
};


int main()
{
    init("small_city_list.txt");
    ExactSolution tour = ExactSolution(COORDINATE_LIST, earth_dist);
    tour.print_path();
 return 0;
}


