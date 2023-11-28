struct intersect
{
    static inline double ccw(pair<long,long> a, pair<long,long> b, pair<long,long> c)
    {
        return (a.first*b.second)+(b.first*c.second)+(c.first*a.second)-(b.first*a.second)-(c.first*b.second)-(a.first*c.second);
    }

    static int check(pair<long,long> a, pair<long,long> b, pair<long,long> c, pair<long,long> d)
    {
        double one=ccw(a,b,c)*ccw(a,b,d), two=ccw(c,d,a)*ccw(c,d,b);
        if(one <= 0 && two <= 0)
        {
            if(one==0 && two==0) // 일직선인경우
            {
                if(a > b) swap(a,b);
                if(c > d) swap(c,d);
                if(c<=b && a<=d) return 1;
            }
            else return 1;
        }
        return 0;
    }
};
