using namespace std;

typedef long double ld;
typedef pair<ld, ld> pld;

const ld PI = acosl(-1);

ld sq (ld x)
{
    return x * x;
}

ld eucl_dist (pld x, pld y)
{
    return sqrtl(sq(x.first - y.first) + sq(x.second - y.second));
}

ld earth_dist(pld x, pld y)
{
    const ld EARTH_RADIUS = 6371;
    ld xl = x.first * PI/180;
    ld yl = y.first * PI/180;
    ld dx = (y.first - x.first) * PI/180;
    ld dy = (y.second - x.second) * PI/180;
    ld tmp = sinl(dx/2) * sinl(dx/2) + cosl(xl) * cosl(yl) * sinl(dy/2) * sinl(dy/2);
    return EARTH_RADIUS * 2 * atan2l(sqrtl(tmp), sqrtl(1 - tmp));
}
