#include "graph_template.h"

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
    init("city_list.txt");
    Christofide g(COORDINATE_LIST, earth_dist);
    g.print_path();
 return 0;
}


