#include <bits/stdc++.h>

using namespace std;
typedef long double ld;
typedef pair<ld, ld> pld;

vector<pld> COORDINATE_LIST = {{0,0}, {1, 0}, {0, 1}, {1, 1}};
vector<string> CITY_ORDER;
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

struct Edge
{
    int st;
    int en;
    ld weight;
    Edge(int tmp_st, int tmp_en, ld tmp_weight)
    {
        st = tmp_st;
        en = tmp_en;
        weight = tmp_weight;
    }
};
struct DSU {
    vector<int> e;
    DSU (){}
    DSU (int N) { e = vector<int>(N, -1) ;}
    
    int get(int x) { return e[x] < 0 ? x : e[x] = get(e[x]); }
    
    bool same_set(int a, int b) { return get(a) == get(b); }
    int size(int x) { return -e[get(x)]; }
    
    bool unite (int x, int y){
    x = get(x), y = get(y);
        if (x == y) return false;
        if (e[x] > e[y]) swap(x,y);
        e[x] += e[y]; e[y] = x;
        return true;
    }
    
};

struct TwoApprox : Tour
{
    DSU dsu;
    vector<vector<int>> adj_list;
    vector<Edge> e;
    vector<int> pv;
    int prev_val;
    void dfs (int u, int p)
    {
        pv[u] = prev_val;
        prev_val = u;
        for (auto x : adj_list[u])
        {
            if (x != p)
            {
                dfs(x,u);
            }
        }
    }
    
    void reconstruct_path()
    {
        int current_vertex = prev_val;
        
        while (current_vertex >= 0)
        {
            path.push_back(current_vertex);
            current_vertex = pv[current_vertex];
        }
        path.push_back(prev_val);
    }
    
    TwoApprox (vector<pld> cities, function<ld(pld, pld)> temp_dist) : Tour(cities, temp_dist)
    {
        dsu = DSU(n);
        pv = vector<int> (n);
        adj_list = vector<vector<int>> (n);
        for (int i = 0; i < n; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                e.push_back(Edge(i, j, adj_matrix[i][j]));
            }
        }
        
        sort(e.begin(),e.end(), [&](Edge &x, Edge &y)
        {
            return x.weight < y.weight;
        });
        
        for (auto [x,y,w] : e)
        {
            if (dsu.unite(x,y))
            {
                adj_list[x].push_back(y);
                adj_list[y].push_back(x);
            }
        }
        
        prev_val = -1;
        dfs(0, -1);
        reconstruct_path();
    }
    
    
};
int main()
{
    init();
    TwoApprox g(COORDINATE_LIST, eucl_dist);
    g.print_path();
 return 0;
}


