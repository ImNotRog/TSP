#include <bits/stdc++.h>

using namespace std;
typedef long double ld;
typedef pair<ld, ld> pld;

vector<pld> COORDINATE_LIST = {{0,0}, {1, 0}, {0, 1}, {1, 1}};
const ld INF = 1e18;

void init()
{
    ifstream fin("city_list.txt");
    COORDINATE_LIST.clear();
    string city, state;
    ld x, y;
    while (fin >> city >> state >> x >> y)
    {
        COORDINATE_LIST.push_back({x,y});
        CITY_ORDER.push_back(city);
    }
}

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
    init();
    ExactSolution tour = ExactSolution(COORDINATE_LIST, eucl_dist);
    tour.print_path();
 return 0;
}


