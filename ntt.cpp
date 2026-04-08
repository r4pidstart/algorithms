class NTT {
public:
    static const int MOD=998244353, W=3;
    static int64_t power(int64_t base, int64_t exp) {
        int64_t res=1;
        base%=MOD;
        while (exp>0) {
            if (exp%2==1) res=(res*base)%MOD;
            base=(base*base)%MOD;
            exp /= 2;
        }
        return res;
    }

    static void ntt(vector<int64_t>& a, bool invert) {
        const int n=a.size();
        for (int i=1, j=0; i<n; i++) {
            int bit=n>>1;
            for (; j&bit; bit>>=1) j^=bit;
            j^=bit;
            if (i<j) swap(a[i], a[j]);
        }
        
        for (int len=2; len<=n; len<<=1) {
            int64_t wlen=power(W, (MOD-1)/len);
            if (invert) wlen=power(wlen, MOD-2);
            
            for (int i=0; i<n; i+=len) {
                int64_t w=1;
                for (int j=0; j<len/2; j++) {
                    int64_t u=a[i+j], v=(a[i+j+len/2]*w)%MOD;
                    
                    a[i+j]=(u+v)%MOD;
                    a[i+j+len/2]=(u-v+MOD)%MOD;
                    w=(w*wlen)%MOD;
                }
            }
        }
        if (invert) {
            int64_t n_inv=power(n, MOD-2);
            for (int i=0; i<n; i++)
                a[i]=(a[i]*n_inv)%MOD;
        }
    }

    static vector<int64_t> multiply(const vector<int64_t>& a, const vector<int64_t>& b) {
        vector<int64_t> fa(a.begin(), a.end()), fb(b.begin(), b.end());
        int n=1;
        while (n<(int)a.size()+(int)b.size()) n<<=1;
        fa.resize(n); fb.resize(n);
        ntt(fa, false), ntt(fb, false);
        
        for (int i=0; i<n; i++)
            fa[i]=(fa[i]*fb[i])%MOD;
        
        ntt(fa, true);
        
        vector<int64_t> ret(a.size()+b.size()-1);
        for (int i=0; i<ret.size(); i++) {
            ret[i]=fa[i];
        }
        return ret;
    }
};
