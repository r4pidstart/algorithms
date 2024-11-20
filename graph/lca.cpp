class LCA
{
    private:
        int max_depth;
        vector<int> depth;
        vector<vector<int> > parent;

        int preprocess(int now, int prev, int d, const vector<vector<int> >& graph)
        {
            depth[now]=d;
            parent[now][0]=prev;
            for(int i=1; i<=max_depth; i++)
                parent[now][i]=parent[parent[now][i-1]][i-1];
          
            for(int next : graph[now])
            {
                if(next==prev) continue;
                preprocess(next, now, d+1, graph);
            }
            return 0;
        }

    public:
        LCA(const vector<vector<int> >& graph) // 1 based
        {   
            max_depth=ceil(log2(graph.size()));
            depth=vector<int>(graph.size());
            parent=vector<vector<int> >(graph.size(), vector<int>(max_depth+1));
            preprocess(1 /* root */, 0, 1, graph);
        }
        
        int get(int a, int b) const
        {
            if(a==1 or b==1) return 1;

            if(depth[a] < depth[b])
                swap(a, b);
            
            // make depth[a]==depth[b]
            for(int i=max_depth; i>=0; i--)
                if(depth[parent[a][i]] >= depth[b])
                    a=parent[a][i];

            if(a==b) return a;
                
            // find lca
            for(int i=max_depth; i>=0; i--)
                if(parent[a][i]!=parent[b][i])
                    a=parent[a][i], b=parent[b][i];

            return parent[a][0];        
        }
};
