class NTT_CRT {
protected:
    class NTT {
    public:
        const static int64_t W=3;
        static int64_t power(int64_t base, int64_t exp, int64_t mod) {
            int64_t res=1;
            base%=mod;
            while (exp>0) {
                if (exp%2==1) res=(res*base)%mod;
                base=(base*base)%mod;
                exp /= 2;
            }
            return res;
        }

        static void ntt(vector<int64_t>& a, bool invert, int64_t mod) {
            const int n=a.size();
            for (int i=1, j=0; i<n; i++) {
                int bit=n>>1;
                for (; j&bit; bit>>=1) j^=bit;
                j^=bit;
                if (i<j) swap(a[i], a[j]);
            }
            
            for (int len=2; len<=n; len<<=1) {
                int64_t wlen=power(W, (mod-1)/len, mod);
                if (invert) wlen=power(wlen, mod-2, mod);
                
                for (int i=0; i<n; i+=len) {
                    int64_t w=1;
                    for (int j=0; j<len/2; j++) {
                        int64_t u=a[i+j], v=(a[i+j+len/2]*w)%mod;
                        
                        a[i+j]=(u+v)%mod;
                        a[i+j+len/2]=(u-v+mod)%mod;
                        w=(w*wlen)%mod;
                    }
                }
            }
            if (invert) {
                int64_t n_inv=power(n, mod-2, mod);
                for (int i=0; i<n; i++)
                    a[i]=(a[i]*n_inv)%mod;
            }
        }

        static vector<int64_t> multiply(const vector<int64_t>& a, const vector<int64_t>& b, int64_t mod) {
            vector<int64_t> fa(a.begin(), a.end()), fb(b.begin(), b.end());
            int n=1;
            while (n<(int)a.size()+(int)b.size()) n<<=1;
            fa.resize(n); fb.resize(n);
            ntt(fa, false, mod), ntt(fb, false, mod);
            
            for (int i=0; i<n; i++)
                fa[i]=(fa[i]*fb[i])%mod;
            
            ntt(fa, true, mod);
            
            vector<int64_t> ret(a.size()+b.size()-1);
            for (int i=0; i<ret.size(); i++) {
                ret[i]=fa[i];
            }
            return ret;
        }
    };

    template<class T>
    static tuple<T, T, T> xgcd(T a, T b)
    {
        if(b==0) return {a, 1, 0};
        
        auto [gcd,x,y]=xgcd(b, a%b);
        return {gcd, y, x-a/b*y};
    }

    template<class T>
    static T inv_mod(T n, T mod)
    {
        auto [g,x,y]=xgcd(n, mod);
        return g==1 ? (x+mod)%mod:-1;
    }
public:
    static vector<__int128_t> multiply(const vector<int64_t>& a, const vector<int64_t>& b)
    {
        const __int128_t mod1=998'244'353, mod2=1'004'535'809, mod3=469'762'049;
        vector<int64_t> res1=NTT::multiply(a, b, mod1);
        vector<int64_t> res2=NTT::multiply(a, b, mod2);
        vector<int64_t> res3=NTT::multiply(a, b, mod3);

        const __int128_t mod_mul=(__int128_t)mod1*mod2*mod3;
        const __int128_t m1=mod2*mod3, m2=mod1*mod3, m3=mod1*mod2;
        const __int128_t y1=inv_mod(m1%mod1, mod1);
        const __int128_t y2=inv_mod(m2%mod2, mod2);
        const __int128_t y3=inv_mod(m3%mod3, mod3);

        const __int128_t term1=m1*y1, term2=m2*y2, term3=m3*y3;

        vector<__int128_t> res(res1.size());
        for(int i=0; i<res.size(); i++)
        {
            __int128_t val=( (__int128_t)res1[i]*term1 + 
                           (__int128_t)res2[i]*term2 + 
                           (__int128_t)res3[i]*term3 ) % mod_mul;
            res[i]=val;
        }
        return res;
    }
};
std::ostream& operator<<(std::ostream& os, __int128_t n) {
    if (n == 0) return os << "0";
    __int128_t temp = n < 0 ? -n : n;
    std::string s;
    while (temp > 0) {
        s += (char)('0' + (temp % 10));
        temp /= 10;
    }
    if (n < 0) s += '-';
    std::reverse(s.begin(), s.end());
    return os << s;
}
