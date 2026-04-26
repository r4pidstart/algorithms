class FWHT {
public:
    static const int64_t MOD=998244353, W=3;
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

    static void ntt_xor(vector<int64_t>& a, bool invert) {
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
                for (int j=0; j<len/2; j++) {
                    int64_t u=a[i+j], v=a[i+j+len/2];
                    a[i+j]=(u+v)%MOD;
                    a[i+j+len/2]=(u-v+MOD)%MOD;
                }
            }
        }
        if (invert) {
            int64_t n_inv=power(n, MOD-2);
            for (int i=0; i<n; i++)
                a[i]=(a[i]*n_inv)%MOD;
        }
    }
    static vector<int64_t> fwht_xor(const vector<int64_t>& a, const vector<int64_t>& b) {
        vector<int64_t> fa(a.begin(), a.end()), fb(b.begin(), b.end());
        int n=1;
        while (n<max((int)a.size(), (int)b.size())) n<<=1;
        fa.resize(n); fb.resize(n);
        ntt_xor(fa, false), ntt_xor(fb, false);
        
        for (int i=0; i<n; i++)
            fa[i]=(fa[i]*fb[i])%MOD;
        
        ntt_xor(fa, true);
        
        vector<int64_t> ret(n);
        for (int i=0; i<ret.size(); i++) {
            ret[i]=fa[i];
        }
        return ret;
    }
    static vector<int64_t> fwht_xor_power(const vector<int64_t>& a, int64_t exp) {
        vector<int64_t> fa(a.begin(), a.end());
        int n=1;
        while (n<(int)a.size()) n<<=1;
        fa.resize(n);
        ntt_xor(fa, false);
        
        for (int i=0; i<n; i++)
            fa[i]=power((fa[i]%MOD+MOD)%MOD, exp);
        
        ntt_xor(fa, true);
        
        vector<int64_t> ret(n);
        for (int i=0; i<ret.size(); i++) {
            ret[i]=fa[i];
        }
        return ret;
    }

    static void ntt_or(vector<int64_t>& a, bool invert) {
        const int n=a.size();
        for (int len=1; len<n; len<<=1) {
            for (int i=0; i<n; i+=(len<<1)) {
                for (int j=0; j<len; j++) {
                    int64_t u=a[i+j];
                    int64_t v=a[i+j+len];
                    if (!invert) {
                        a[i+j+len]=(u+v) % MOD;
                    } else {
                        a[i+j+len]=(v-u+MOD) % MOD;
                    }
                }
            }
        }
    }
    static vector<int64_t> fwht_or(const vector<int64_t>& a, const vector<int64_t>& b) {
        vector<int64_t> fa(a.begin(), a.end()), fb(b.begin(), b.end());
        int n=1;
        while (n<max((int)a.size(), (int)b.size())) n<<=1;
        fa.resize(n); fb.resize(n);
        ntt_or(fa, false), ntt_or(fb, false);
        
        for (int i=0; i<n; i++)
            fa[i]=(fa[i]*fb[i])%MOD;
        
        ntt_or(fa, true);
        
        vector<int64_t> ret(n);
        for (int i=0; i<ret.size(); i++) {
            ret[i]=fa[i];
        }
        return ret;
    }

        static void ntt_and(vector<int64_t>& a, bool invert) {
        const int n=a.size();
        for (int len=1; len<n; len<<=1) {
            for (int i=0; i<n; i+=(len<<1)) {
                for (int j=0; j<len; j++) {
                    int64_t u=a[i+j+len];
                    int64_t v=a[i+j];
                    if (!invert) {
                        a[i+j]=(u+v) % MOD;
                    } else {
                        a[i+j]=(v-u+MOD) % MOD;
                    }
                }
            }
        }
    }
    static vector<int64_t> fwht_and(const vector<int64_t>& a, const vector<int64_t>& b) {
        vector<int64_t> fa(a.begin(), a.end()), fb(b.begin(), b.end());
        int n=1;
        while (n<max((int)a.size(), (int)b.size())) n<<=1;
        fa.resize(n); fb.resize(n);
        ntt_and(fa, false), ntt_and(fb, false);
        
        for (int i=0; i<n; i++)
            fa[i]=(fa[i]*fb[i])%MOD;
        
        ntt_and(fa, true);
        
        vector<int64_t> ret(n);
        for (int i=0; i<ret.size(); i++) {
            ret[i]=fa[i];
        }
        return ret;
    }
};
