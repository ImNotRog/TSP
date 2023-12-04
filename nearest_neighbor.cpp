#include <bits/stdc++.h>

using namespace std;
typedef long double ld;
typedef pair<ld, ld> pld;

vector<pld> COORDINATE_LIST = {{0,0}, {1, 0}, {0, 1}, {1, 1}};
const ld INF = 1e18;

ld sq (ld x)
{
    return x * x;
}

ld eucl_dist (pld x, pld y)
{
    return sqrtl(sq(x.first - y.first) + sq(x.second - y.second));
}
    
struct Graph
{
    int n;
    vector<pld> city_coords;
    vector<vector<ld>> adj_matrix;
    function<ld(pld,pld)> dist;
    Graph (vector<pld> cities, function<ld(pld, pld)> temp_dist)
    {
        n = cities.size();
        city_coords = cities;
        dist = temp_dist;
        adj_matrix = vector<vector<ld>> (n, vector<ld> (n));
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
    }
    
};

struct Tour : Graph
{
    vector<bool> visited;
    vector<int> path;
    ld path_length;
    Tour (vector<pld> cities, function<ld(pld, pld)> temp_dist) : Graph(cities, temp_dist)
    {
        path_length = INF;
        visited = vector<bool> (n);
    }
    
    bool operator<(const Tour &other) { return path_length < other.path_length; }
    
    bool check_vis (int x)
    {
        return visited[x];
    }
    
    void visit (int x)
    {
        assert(!visited[x]);
        visited[x] = 1;
    }
    
    void calc_path_length()
    {
        path_length = 0;
        for (int i = 1; i < n; i++)
        {
            path_length = path_length + adj_matrix[path[i]][path[i-1]];
        }
        path_length = path_length + adj_matrix[path[n-1]][path[0]];
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
    vector<Tour> tours;
    for (int i = 0; i < COORDINATE_LIST.size(); i++)
    {
        tours.push_back(NearestNeighbor(COORDINATE_LIST, eucl_dist, i));
    }
    sort(tours.begin(),tours.end());
    tours[0].print_path();
 return 0;
}


