#include "graph_template.h"
    
struct NearestNeighbor : Tour
{
    int find_nearest(int current_vertex)
    {
        visit(current_vertex);
        ld shortest_dist = INF;
        int next_vertex = -1;
        for (int i = 0; i < n; i++)
        {
            if (!check_vis(i) && adj_matrix[current_vertex][i] < shortest_dist)
            {
                shortest_dist = adj_matrix[current_vertex][i];
                next_vertex = i;
            }
        }
        return next_vertex;
    }
    
    
    NearestNeighbor (vector<pld> cities, function<ld(pld, pld)> temp_dist, int start_vertex) : Tour(cities, temp_dist) 
    {
        int current_vertex = start_vertex;
        while (current_vertex != -1)
        {
            path.push_back(current_vertex);
            current_vertex = find_nearest(current_vertex);
        }
    }
};


int main()
{
    init("city_list.txt");
    vector<Tour> tours;
    for (int i = 0; i < COORDINATE_LIST.size(); i++)
    {
        tours.push_back(NearestNeighbor(COORDINATE_LIST, earth_dist, i));
    }
    sort(tours.begin(),tours.end());
    tours[0].print_path();
 return 0;
}


