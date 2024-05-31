template<class T>
tuple<T, T, T> xgcd(T a, T b)
{
    if(b==0) return {a, 1, 0};
    
    auto [gcd,x,y]=xgcd(b, a%b);
    return {gcd, y, x-a/b*y};
}

template<class T>
T inv_mod(T n, T mod)
{
    auto [g,x,y]=xgcd(n, mod);
    return g==1 ? (x+mod)%mod:-1;
}
