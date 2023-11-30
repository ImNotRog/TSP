#include <bits/stdc++.h>

using namespace std;
typedef long double ld;
typedef pair<ld, ld> pld;

vector<pld> COORDINATE_LIST = {{0,0}};

struct Graph
{
    int n;
    vector<pld> city_coords;
    vector<vector<ld>> adj_matrix;
    
    Graph (vector<pld> cities)
    {
        n = cities.size();
        city_coords = cities;
        adj_matrix = vector<vector<ld>> (n, vector<ld> (n));
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                adj_matrix[i][j] = eucl_dist(city_coords[i], city_coords[j]);
            }
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
    Tour (vector<pld> cities) : Graph(cities)
    {
        visited = vector<bool> (size());
    }
    
};


int main()
{
    
    Tour g (COORDINATE_LIST);
    cout << g.size() << "\n";
 return 0;
}


