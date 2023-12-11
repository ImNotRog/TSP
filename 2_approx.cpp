#include "graph_template.h"

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
    
    void gen_MST()
    {
        ld mst_weight = 0;
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
                mst_weight += w;
                adj_list[x].push_back(y);
                adj_list[y].push_back(x);
            }
        }
        //~ cout << mst_weight << "\n";
    }
    
    TwoApprox (vector<pld> cities, function<ld(pld, pld)> temp_dist) : Tour(cities, temp_dist)
    {
        dsu = DSU(n);
        pv = vector<int> (n);
        adj_list = vector<vector<int>> (n);
        gen_MST();
        prev_val = -1;
        dfs(0, -1);
        reconstruct_path();
    }
    
    
};
int main()
{
    init("city_list.txt");
    TwoApprox g(COORDINATE_LIST, earth_dist);
    g.print_path();
 return 0;
}


