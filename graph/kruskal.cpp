class kruskal
{
    // edge={cost, from, to}, O(ElogE+E)
    private:
        class Dsu
        {
            private:
                vector<int> parent;
            public:
                Dsu(int size)
                {
                    parent.assign(size, -1);
                }
                int find(int i)
                {
                    if(parent[i]<0) return i;
                    else return parent[i]=find(parent[i]);
                }
                int merge(int a, int b)
                {
                    a=find(a), b=find(b);
                    if(a==b) return 1;

                    if(parent[a]>parent[b]) swap(a,b);
                    parent[a]+=parent[b];
                    parent[b]=a;
                    return 0;
                }
        };
    public:
        long long cost=0;
        vector<int> used_edge;
        kruskal(int size, vector<pair<int, pair<int,int> > >& edge)
        {
            Dsu dsu(size);
            used_edge.assign(edge.size(),0);
            sort(edge.begin(), edge.end());
            int cnt=0;
            for(int i=0; i<edge.size(); i++)
                if(!dsu.merge(edge[i].second.first, edge[i].second.second))
                    cost+=edge[i].first, used_edge[i]=1, cnt++;

            if(cnt != size-1) cost=-1;
        }
};