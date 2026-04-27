template <typename T>
struct convex_hull
{
    struct intersect
    {
        static inline double ccw(pair<T,T> a, pair<T,T> b, pair<T,T> c)
        {
            return ((__int128_t)a.first*b.second)+((__int128_t)b.first*c.second)+((__int128_t)c.first*a.second)-((__int128_t)b.first*a.second)-((__int128_t)c.first*b.second)-((__int128_t)a.first*c.second);
        }

        static int check(pair<T,T> a, pair<T,T> b, pair<T,T> c, pair<T,T> d)
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

    static __int128_t get_dist_sq(const pair<T, T>& a, const pair<T, T>& b)
    {
        return (__int128_t)(a.first-b.first)*(a.first-b.first)+(__int128_t)(a.second-b.second)*(a.second-b.second);
    }

    static pair<T, T> ref_point;
    static inline int comp_ccw(pair<T, T>& a, pair<T, T>& b)
    {
        double direction=intersect::ccw(ref_point, a, b);
        if(direction==0)
            return get_dist_sq(ref_point, a)<get_dist_sq(ref_point, b);
        return direction>0;
    }

    static vector<pair<T, T> > get_convex_hull(const vector<pair<T, T> >& arr)
    {
        if(arr.size()<3)
            return arr;

        auto copied=arr;
        nth_element(copied.begin(), copied.begin(), copied.end());
        ref_point=copied[0];

        sort(copied.begin()+1, copied.end(), comp_ccw);

        deque<pair<T, T> > dq;
        dq.push_back(copied[0]);
        dq.push_back(copied[1]);

        for(int i=2; i<copied.size(); i++)
        {
            while(dq.size()>=2)
            {
                if(intersect::ccw(dq[dq.size()-2], dq[dq.size()-1], copied[i])>0)
                    break;
                else
                    dq.pop_back();
            }
            dq.push_back(copied[i]);
        }
        return vector<pair<T, T> >(dq.begin(), dq.end());
    }

    static int check_if_inside(const pair<T, T>& point, const vector<pair<T, T> >& hull)
    {
        for(int i=0; i<hull.size(); i++)
        {
            if(convex_hull<T>::intersect::ccw(hull[i], hull[(i+1)%hull.size()], point)<=0)
                return 0;
        }
        return 1;
    }

    static pair<pair<T, T>, pair<T, T> > rotating_calipers(const vector<pair<T, T> >& hull)
    {
        int n=hull.size();
        if(n==1) return {hull[0], hull[0]};
        if(n==2) return {hull[0], hull[1]};

        pair<pair<T, T>, pair<T, T> > ans;
        T max_dist=0;
        int counterpart=1;
        for(int i=0; i<n; i++)
        {
            while(true)
            {
                pair<T, T> v1={hull[i].first-hull[(i+1)%n].first, hull[i].second-hull[(i+1)%n].second};
                pair<T, T> v2={hull[counterpart].first-hull[(counterpart+1)%n].first, hull[counterpart].second-hull[(counterpart+1)%n].second};

                if(convex_hull<T>::intersect::ccw({0,0}, v1, v2)>0)
                    counterpart=(counterpart+1)%n;
                else
                    break;

                T dist=get_dist_sq(hull[i], hull[counterpart]);
                if(dist>max_dist)
                {
                    ans={hull[i], hull[counterpart]};
                    max_dist=dist;
                }
            }
            T dist=get_dist_sq(hull[i], hull[counterpart]);
            if(dist>max_dist)
            {
                ans={hull[i], hull[counterpart]};
                max_dist=dist;
            }
        }

        return ans;
    }
};
template<typename T>
pair<T, T> convex_hull<T>::ref_point;
