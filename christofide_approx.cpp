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

struct Christofide : Tour
{
    DSU dsu;
    vector<vector<int>> adj_list;
    vector<set<int>> adj_set;
    vector<Edge> e, unused, possible;
    set<int> odds;
    vector<int> compress, decomp;
    vector<int> decompressed_path;
    
    void reconstruct_path()
    {
        visited = vector<bool> (n);
        for (auto x : decompressed_path)
        {
            if (!visited[x])
            {
                visited[x] = 1;
                path.push_back(x);
            }
        }
        path.push_back(0);
    }
    void gen_euc ()
    {
        for (int i = 0; i < n; i++)
        {
            for (auto x : adj_list[i])
            {
                adj_set[i].insert(x);
            }
        }
        set<pair<int,int>> final_edges;
        for (auto [x,y,w] : e)
        {
            final_edges.insert({x,y});
        }
        vector<int> d(n + 1);
        for (int i = 0; i <= n; i++)
        {
            d[i] = adj_set[i].size();
        }
        decompressed_path.push_back(0);
        for (int i = 0; i < n; i++)
        {
            if (!visited[i])
            {
                vector<int> st;
                vector<int> z;
                st.push_back(i);
                while (!st.empty())
                {
                    int u = st.back();
                    visited[u] = 1;
                    if (d[u] == 0)
                    {
                        z.push_back(u);
                        st.pop_back();
                    } else
                    {
                        int v = *adj_set[u].begin();
                        st.push_back(v);
                        adj_set[u].erase(v);
                        adj_set[v].erase(u);
                        d[u]--,d[v]--;
                    }
                }
                
                
                for (int j = 0; j < z.size(); j++)
                {
                    pair<int,int> q = {z[j], z[(j + 1) % z.size()]};
                    
                    if (final_edges.count(q) || final_edges.count({q.second, q.first}))
                    {
                        decompressed_path.push_back(q.second);
                    }
                }
            }
        }
    }

    void gen_MST()
    {
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
            } else
            {
                unused.push_back(Edge(x,y,w));
            }
        }
        
        for (int i = 0; i < n; i++)
        {
            if (adj_list[i].size() % 2)
            {
                odds.insert(i);
            }
        }
        
        for (auto [x,y,w] : unused)
        {
            if (odds.count(x) && odds.count(y))
            {
                possible.push_back(Edge(x,y,w));
            }
        }
    }
    
    void gen_matching()
    {
        decomp = vector<int> (n);
        for (auto x : odds)
        {
            compress.push_back(x);
        }
        
        for (int i = 0; i < compress.size(); i++)
        {
            decomp[compress[i]] = i;
        }
        
        for (auto x : odds)
        {
            cout << decomp[x] << " ";
        }
        cout << "\n";
        
        ofstream fout("blossom5/send.txt");
        fout << odds.size() << " " << possible.size() << "\n";
        for (int i = 0; i < possible.size(); i++)
        {
            fout << decomp[possible[i].st] << " " << decomp[possible[i].en] << " " << possible[i].weight << "\n";
        }
        fout << "\n";
        fout.close();
        system("cd blossom5; ./blossom5 -V -e send.txt -w soln.txt");
        ifstream fin("blossom5/soln.txt");
        
        int current_n, current_m;
        fin >> current_n >> current_m;
        for (int i = 0; i < current_m; i++)
        {
            int st, en;
            fin >> st >> en;
            st = compress[st], en = compress[en];
            adj_list[st].push_back(en);
            adj_list[en].push_back(st);
        }
    }
    
    Christofide (vector<pld> cities, function<ld(pld, pld)> temp_dist) : Tour(cities, temp_dist)
    {
        dsu = DSU(n);
        adj_list = vector<vector<int>> (n);
        adj_set = vector<set<int>>(n);
        gen_MST();
        gen_matching();
        gen_euc();
        reconstruct_path();
    }
    
    
};
int main()
{
    init();
    Christofide g(COORDINATE_LIST, eucl_dist);
    g.print_path();
 return 0;
}


