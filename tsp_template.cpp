#include "graph_template.h"

int main()
{
    init("city_list.txt");
    Tour g(COORDINATE_LIST, earth_dist);
    cout << g.size() << "\n";
 return 0;
}


