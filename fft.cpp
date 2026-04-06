class FFT
{
public:
    static void fft(vector <complex<long double>> &a, bool invert){
        int n = a.size();
        for (int i=1,j=0;i<n;i++){
            int bit = n >> 1;
            for (;j>=bit;bit>>=1) j -= bit;
            j += bit;
            if (i < j) swap(a[i],a[j]);
        }
        for (int len=2;len<=n;len<<=1){
            long double ang = 2*M_PI/len*(invert?-1:1);
            complex<long double> wlen(cos(ang),sin(ang));
            for (int i=0;i<n;i+=len){
                complex<long double> w(1);
                for (int j=0;j<len/2;j++){
                    complex<long double> u = a[i+j], v = a[i+j+len/2]*w;
                    a[i+j] = u+v;
                    a[i+j+len/2] = u-v;
                    w *= wlen;
                }
            }
        }
        if (invert){
            for (int i=0;i<n;i++) a[i] /= n;
        }
    }
    static void multiply(vector<int> &a, vector<int> &b, vector<int> &ret){
        vector<complex<long double>> fa(a.begin(), a.end()), fb(b.begin(), b.end());
        int n = 1;
        while (n < (int)a.size()+(int)b.size()) n <<= 1;
        fa.resize(n); fb.resize(n);
        fft(fa,false); fft(fb,false);
        for (int i=0;i<n;i++) fa[i] *= fb[i];
        fft(fa,true);
        ret.resize(n);
        for (int i=0;i<n;i++) ret[i] = long(fa[i].real()+(fa[i].real()>0?0.5:-0.5));
    }
};
