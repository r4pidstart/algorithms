template<typename T>
class Hld_With_Lazy_Seg
{
    template<typename U>
    class lazy_seg_tree;

public:
    int n, dfn_timer;
    vector<int> sz, depth, parent, top, in, out, heavy;
    vector<T> edge_weight_to_parent;
    lazy_seg_tree<T> seg;

    void dfs_sz(const vector<vector<pair<int, T> > >& graph, int now, int par, int dep)
    {
        sz[now]=1;
        parent[now]=par;
        depth[now]=dep;
        int max_subtree_siz=0;

        for(auto& [next, cost] : graph[now])
        {
            if(next==par) continue;
            edge_weight_to_parent[next]=cost;
            dfs_sz(graph, next, now, dep+1);
            sz[now]+=sz[next];
            if(sz[next]>max_subtree_siz)
            {
                max_subtree_siz=sz[next];
                heavy[now]=next;
            }
        }
    }
    void dfs_hld(const vector<vector<pair<int, T> > >& graph, int now, int par, int top_node)
    {
        in[now]=dfn_timer++;
        top[now]=top_node;

        if(heavy[now]!=-1)
            dfs_hld(graph, heavy[now], now, top_node);
        
        for(auto& [next, cost] : graph[now])
        {
            if(next==par or next==heavy[now]) continue;
            dfs_hld(graph, next, now, next);
        }
        out[now]=dfn_timer;
    }
public: 
    Hld_With_Lazy_Seg(const vector<vector<pair<int, T> > >& graph, int root=1) : seg(graph.size())
    {
        n=graph.size();
        sz.assign(n, 0);
        depth.assign(n, 0);
        parent.assign(n, -1);
        heavy.assign(n, -1);
        edge_weight_to_parent.assign(n, 0);

        dfs_sz(graph, root, -1, 0);

        dfn_timer=0;
        in.assign(n, 0);
        out.assign(n, 0);
        top.assign(n, 0);

        dfs_hld(graph, root, -1, root);

        vector<T> mapped_arr(n, 0);
        for(int i = 0; i < n; i++) 
            mapped_arr[in[i]] = edge_weight_to_parent[i];
        seg.init(mapped_arr);
    }

    void update_route_plus(int a, int b, T value)
    {
        while(top[a] != top[b])
        {
            if(depth[top[a]]<depth[top[b]]) swap(a, b);
            seg.update_plus(in[top[a]], in[a], value);
            a=parent[top[a]];
        }
        if(depth[a]>depth[b]) swap(a, b);
        if(a!=b)
            seg.update_plus(in[a]+1, in[b], value);
    }
    void update_route_multiply(int a, int b, T value)
    {
        while(top[a] != top[b])
        {
            if(depth[top[a]]<depth[top[b]]) swap(a, b);
            seg.update_multiply(in[top[a]], in[a], value);
            a=parent[top[a]];
        }
        if(depth[a]>depth[b]) swap(a, b);
        if(a!=b)
            seg.update_multiply(in[a]+1, in[b], value);
    }
    void update_subtree_plus(int a, T value)
    {
        if(in[a]+1 <= out[a]-1)
            seg.update_plus(in[a]+1, out[a]-1, value);
    }
    void update_subtree_multiply(int a, T value)
    {
        if(in[a]+1 <= out[a]-1)
            seg.update_multiply(in[a]+1, out[a]-1, value);
    }

    T get_route(int a, int b)
    {
        T ret=0; 
        while(top[a] != top[b])
        {
            if(depth[top[a]]<depth[top[b]]) swap(a, b);
            ret=seg.f_plus(ret, seg.get(in[top[a]], in[a]));
            a=parent[top[a]];
        }
        if(depth[a]>depth[b]) swap(a, b);
        if(a!=b)
            ret=seg.f_plus(ret, seg.get(in[a]+1, in[b]));
        return ret;
    }
    T get_subtree(int a)
    {
        if(in[a]+1 <= out[a]-1)
            return seg.get(in[a]+1, out[a]-1);
        return lazy_seg_tree<T>::base;
    }

    int get_lca(int u, int v) 
    {
        while (top[u]!=top[v]) 
        {
            if (depth[top[u]]<depth[top[v]]) swap(u, v);
            u=parent[top[u]];
        }
        return depth[u]<depth[v] ? u:v;
    }

private:
    //
    template <typename U>
    class lazy_seg_tree // 1-based
    {
        public:
            const U initial=U();
            inline static const U base=U();
            inline U f_plus(const U& a, const U& b) 
            {
                return (a+b);
            }
            inline U f_multiply(const U& a, const U& b) 
            {
                return (a*b);
            }
            int size;
            vector<U> tree, lazy_plus, lazy_multiply, lazy_assign, lazy_assign_flag;
            U _init(const vector<U>& arr, int now, int s, int e)
            {
                if(s==e) return tree[now]=arr[s];
                else return tree[now]=f_plus(_init(arr, now*2, s, (s+e)/2), _init(arr, now*2+1, ((s+e)/2)+1, e));
            }
            U _get(int l, int r, int now, int s, int e)
            {
                if(s>r or e<l) return base;
                else if(l<=s and e<=r) return tree[now];
                _lazy(now, s, e);
                return f_plus(_get(l, r, now*2, s, (s+e)/2), _get(l, r, now*2+1, ((s+e)/2)+1, e));
            }
            U _update_plus(int l, int r, U v, int now, int s, int e)
            {
                if(s>r or e<l) return tree[now];
                else if(l<=s and e<=r) 
                {
                    if(lazy_assign_flag[now])
                    {
                        lazy_assign[now]=f_plus(lazy_assign[now], v);
                        tree[now]=f_plus(tree[now], v*(e-s+1));
                        return tree[now];
                    }
                    lazy_plus[now]=f_plus(lazy_plus[now], v);
                    return tree[now]=f_plus(tree[now], v*(e-s+1));
                }
                _lazy(now, s, e);
                return tree[now]=f_plus(_update_plus(l, r, v, now*2, s, (s+e)/2), _update_plus(l, r, v, now*2+1, ((s+e)/2)+1, e));
            }
            U _update_multiply(int l, int r, U v, int now, int s, int e)
            {
                if(s>r or e<l) return tree[now];
                else if(l<=s and e<=r) 
                {
                    if(lazy_assign_flag[now])
                    {
                        lazy_assign[now]=f_multiply(lazy_assign[now], v);
                        tree[now]=f_multiply(tree[now], v);
                        return tree[now];
                    }
                    if(lazy_plus[now])
                        lazy_plus[now]=f_multiply(lazy_plus[now], v);
                    lazy_multiply[now]=f_multiply(lazy_multiply[now], v);
                    return tree[now]=f_multiply(tree[now], v);
                }
                _lazy(now, s, e);
                return tree[now]=f_plus(_update_multiply(l, r, v, now*2, s, (s+e)/2), _update_multiply(l, r, v, now*2+1, ((s+e)/2)+1, e));
            }
            U _update_assign(int l, int r, U v, int now, int s, int e)
            {
                if(s>r or e<l) return tree[now];
                else if(l<=s and e<=r) 
                {
                    lazy_assign[now]=v;
                    lazy_assign_flag[now]=1;
                    lazy_plus[now]=0;
                    lazy_multiply[now]=1;
                    return tree[now]=f_multiply(v, (e-s+1));
                }
                _lazy(now, s, e);
                return tree[now]=f_plus(_update_assign(l, r, v, now*2, s, (s+e)/2), _update_assign(l, r, v, now*2+1, ((s+e)/2)+1, e));
            }
            void _lazy_update_plus(int now, int s, int e)
            {
                if(lazy_plus[now])
                {
                    int m=(s+e)/2;
                    tree[now*2]=f_plus(tree[now*2], lazy_plus[now]*(m-s+1));
                    tree[now*2+1]=f_plus(tree[now*2+1], lazy_plus[now]*(e-m));
                    
                    if(lazy_assign_flag[now*2])
                        lazy_assign[now*2]=f_plus(lazy_assign[now*2], lazy_plus[now]);
                    else
                        lazy_plus[now*2]=f_plus(lazy_plus[now*2], lazy_plus[now]);
                    if(lazy_assign_flag[now*2+1])
                        lazy_assign[now*2+1]=f_plus(lazy_assign[now*2+1], lazy_plus[now]);
                    else
                        lazy_plus[now*2+1]=f_plus(lazy_plus[now*2+1], lazy_plus[now]);

                    lazy_plus[now]=0;
                }
            }
            void _lazy_update_multiply(int now, int s, int e)
            {
                if(lazy_multiply[now]!=1)
                {
                    tree[now*2]=f_multiply(tree[now*2], lazy_multiply[now]);
                    tree[now*2+1]=f_multiply(tree[now*2+1], lazy_multiply[now]);

                    if(lazy_assign_flag[now*2])
                        lazy_assign[now*2]=f_multiply(lazy_assign[now*2], lazy_multiply[now]);
                    else
                    {
                        lazy_multiply[now*2]=f_multiply(lazy_multiply[now*2], lazy_multiply[now]);
                        lazy_plus[now*2]=f_multiply(lazy_plus[now*2], lazy_multiply[now]); // !!
                    }
                    if(lazy_assign_flag[now*2+1])
                        lazy_assign[now*2+1]=f_multiply(lazy_assign[now*2+1], lazy_multiply[now]);
                    else
                    {
                        lazy_multiply[now*2+1]=f_multiply(lazy_multiply[now*2+1], lazy_multiply[now]);
                        lazy_plus[now*2+1]=f_multiply(lazy_plus[now*2+1], lazy_multiply[now]); // !!
                    }

                    lazy_multiply[now]=1;
                }
            }
            void _lazy_update_assign(int now, int s, int e)
            {
                if(lazy_assign_flag[now])
                {
                    int m=(s+e)/2;
                    tree[now*2]=f_multiply(lazy_assign[now], (m-s+1));
                    lazy_assign[now*2]=lazy_assign[now];
                    lazy_assign_flag[now*2]=1;
                    tree[now*2+1]=f_multiply(lazy_assign[now], (e-m));
                    lazy_assign[now*2+1]=lazy_assign[now];
                    lazy_assign_flag[now*2+1]=1;

                    lazy_assign[now]=0;
                    lazy_assign_flag[now]=0;
                    lazy_plus[now*2]=0; // !!
                    lazy_multiply[now*2]=1; // !!
                    lazy_plus[now*2+1]=0; // !!
                    lazy_multiply[now*2+1]=1; // !!
                }
            }
            void _lazy(int now, int s, int e)
            {
                _lazy_update_assign(now, s, e);
                _lazy_update_multiply(now, s, e);
                _lazy_update_plus(now, s, e);
            }
        public:
            lazy_seg_tree(int size)
            {
                this->size=size;
                tree.assign(size*4, initial);
                lazy_plus.assign(size*4, 0);
                lazy_multiply.assign(size*4, 1);
                lazy_assign.assign(size*4, 0);
                lazy_assign_flag.assign(size*4, 0);
            }
            U init(const vector<U>& arr, int now=0, int s=0, int e=0)
                { return now ? _init(arr, now, s, e):_init(arr, 1, 0, size-1); }
            U get(int l, int r, int now=0, int s=0, int e=0)
                { return now ? _get(l, r, now, s, e):_get(l, r, 1, 0, size-1); }
            U update_plus(int l, int r, U value, int now=0, int s=0, int e=0)
                { return now ? _update_plus(l, r, value, now, s, e):_update_plus(l, r, value, 1, 0, size-1); }
            U update_multiply(int l, int r, U value, int now=0, int s=0, int e=0)
                { return now ? _update_multiply(l, r, value, now, s, e):_update_multiply(l, r, value, 1, 0, size-1); }
            U update_assign(int l, int r, U value, int now=0, int s=0, int e=0)
                { return now ? _update_assign(l, r, value, now, s, e):_update_assign(l, r, value, 1, 0, size-1); }
    };
};
