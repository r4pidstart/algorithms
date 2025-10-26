class Hld_With_Lazy_Seg
{
    template<typename T>
    class lazy_seg_tree;
    
public:
    vector<lazy_seg_tree<int64_t> > segs;
    vector<int> depth, subtree_size, group_idx_of_node, 
                top_node_of_group, bottom_node_of_group, parent;
    vector<tuple<int, int, int64_t> > edges; // {a, b, cost}
    map<pair<int, int>, int> edge_idx; // {a, b} -> edge_idx
    map<int, pair<int, int> > edge_to_seg_idx; // edge_idx -> {group_idx, seg_idx}
    int group_idx=0;

    int get_subtree_size(const vector<vector<pair<int, int64_t> > >& graph, int now, int par)
    {
        subtree_size[now]=1;
        for(auto& x: graph[now])
        {
            if(x.first==par) continue;
            depth[x.first]=depth[now]+1;
            subtree_size[now]+=get_subtree_size(graph, x.first, now);
        }
        return subtree_size[now];
    }
    int heavy_light_decomposition(const vector<vector<pair<int, int64_t> > >& graph, int now, int par)
    {
        group_idx_of_node[now]=group_idx;

        int heavy_child=-1, heavy_child_sub_tree_size=-1;
        for(auto& [next, cost] : graph[now])
        {
            if(next==par) continue;
            edge_idx[{now, next}]=edges.size();
            edges.push_back({now, next, cost});
            if(subtree_size[next]>heavy_child_sub_tree_size)
            {
                heavy_child_sub_tree_size=subtree_size[next];
                heavy_child=next;
            }
        }

        if(heavy_child==-1) // end of the group
        {
            assert(bottom_node_of_group.size()==group_idx);
            bottom_node_of_group.push_back(now);
            return 0;
        }

        parent[heavy_child]=now;
        heavy_light_decomposition(graph, heavy_child, now);
        for(auto& [next, cost] : graph[now])
        {
            if(next==par or next==heavy_child) continue;
            assert(top_node_of_group.size()==group_idx+1);
            top_node_of_group.push_back(next);
            group_idx++;
            parent[next]=now;
            heavy_light_decomposition(graph, next, now); // make a new group
        }

        return 0;
    }
    int init_segment_tree()
    {
        vector<int> visited(depth.size());
        for(int i=0; i<top_node_of_group.size(); i++)
        {
            assert(top_node_of_group[i]!=-1 and bottom_node_of_group[i]!=-1);

            // for each group, init the lazy segment tree
            stack<pair<int, int> > stk; // cur, edge_idx
            int cur=bottom_node_of_group[i];
            while(cur!=-1 and !visited[cur])
            {
                stk.push({cur, edge_idx[{parent[cur], cur}]});
                visited[cur]=1;
                cur=parent[cur];
            }

            assert(stk.size());
            stk.pop();

            // vector<int64_t> tmp(1, lazy_seg_tree<long>::base);
            vector<int64_t> tmp(1, 0);
            tmp.reserve(stk.size());

            cur=top_node_of_group[i];
            while(stk.size())
            {
                int target_edge=stk.top().second;
                stk.pop();

                edge_to_seg_idx[target_edge]={i, tmp.size()};
                tmp.push_back(get<2>(edges[target_edge]));
            }
            segs.emplace_back(tmp.size());
            segs.back().init(tmp);
        }
        return 0;
    }

public:
    Hld_With_Lazy_Seg(const vector<vector<pair<int, int64_t> > >& graph)
    {
        subtree_size.assign(graph.size(), 0);
        depth.assign(graph.size(), 0);
        get_subtree_size(graph, 1, -1);

        parent.assign(graph.size(), -1);
        group_idx_of_node.assign(graph.size(), -1);
        top_node_of_group.push_back(1); // init
        heavy_light_decomposition(graph, 1, -1);
        init_segment_tree();
    }

    int update_one_edge(int edge_idx, int64_t value)
    {
        if(edge_to_seg_idx.count(edge_idx))
        {
            auto [target_group, target_seg_idx]=edge_to_seg_idx[edge_idx];
            segs[target_group].update(target_seg_idx, target_seg_idx, value);
        }
        get<2>(edges[edge_idx])+=value; // lazy!
        return 0;
    }
    int update_one_edge(int a, int b, int64_t value)
    {
        if(a==-1 or b==-1)
            return 0;
        if(!edge_idx.count({a, b}) and !edge_idx.count({b, a}))
            return 0; // do nothing when the edge do not exists
        if(depth[a]>depth[b]) swap(a, b);
        assert(parent[b]==a);
        return update_one_edge(edge_idx[{a, b}], value);
    }
    int update_route(int a, int b, int64_t value)
    {
        while(group_idx_of_node[a] != group_idx_of_node[b])
        {
            int top_node_of_group_a=top_node_of_group[group_idx_of_node[a]],
                top_node_of_group_b=top_node_of_group[group_idx_of_node[b]];
            
            if(depth[top_node_of_group_a] > depth[top_node_of_group_b])
                swap(a, b), swap(top_node_of_group_a, top_node_of_group_b);
            
            int b_seg_idx=group_idx_of_node[b];
            int b_seg_range_end=depth[b]-depth[top_node_of_group_b]+1;
            segs[b_seg_idx].update(1, b_seg_range_end-1, value);

            update_one_edge(edge_idx[{parent[top_node_of_group_b], top_node_of_group_b}], value);
            b=parent[top_node_of_group_b];
        }

        int group_idx=group_idx_of_node[a];
        int top_node=top_node_of_group[group_idx];

        int a_seg_depth=depth[a]-depth[top_node]+1,
            b_seg_depth=depth[b]-depth[top_node]+1;

        segs[group_idx].update(min(a_seg_depth, b_seg_depth), max(a_seg_depth, b_seg_depth)-1, value);

        return 0;
    }
    int64_t get_route(int a, int b)
    {
        int64_t ret=lazy_seg_tree<int64_t>::base;
        while(group_idx_of_node[a] != group_idx_of_node[b])
        {
            int top_node_of_group_a=top_node_of_group[group_idx_of_node[a]], 
                top_node_of_group_b=top_node_of_group[group_idx_of_node[b]];
            
            if(depth[top_node_of_group_a] > depth[top_node_of_group_b])
                swap(a, b), swap(top_node_of_group_a, top_node_of_group_b);
        
            int b_seg_idx=group_idx_of_node[b];
            int b_seg_range_end=depth[b]-depth[top_node_of_group_b]+1;
            ret=segs[0].f(ret, segs[b_seg_idx].get(1, b_seg_range_end-1));
            
            // move b
            ret=segs[0].f(ret, get<2>(edges[edge_idx[{parent[top_node_of_group_b], top_node_of_group_b}]]));
            b=parent[top_node_of_group_b];
        }
        
        int group_idx=group_idx_of_node[a];
        int top_node=top_node_of_group[group_idx];

        int a_seg_depth=depth[a]-depth[top_node]+1,
            b_seg_depth=depth[b]-depth[top_node]+1;
        
        ret=segs[0].f(ret, segs[group_idx].get(
                min(a_seg_depth, b_seg_depth), max(a_seg_depth, b_seg_depth)-1
            ));

        return ret;
    }

private:
    template <typename T>
    class lazy_seg_tree // 1-based
    {
        public:
            const T initial=T();
            inline static const T base=T();
            inline T f(const T& a, const T& b) 
            {
                return (a+b);
            }
            int size;
            vector<T> tree, lazy;
            T _init(const vector<T>& arr, int now, int s, int e)
            {
                if(s==e) return tree[now]=arr[s];
                else return tree[now]=f(_init(arr, now*2, s, (s+e)/2), _init(arr, now*2+1, ((s+e)/2)+1, e));
            }
            T _get(int l, int r, int now, int s, int e)
            {
                if(s>r or e<l) return base;
                else if(l<=s and e<=r) return tree[now];
                _lazy_update(now, s, e);
                return f(_get(l, r, now*2, s, (s+e)/2), _get(l, r, now*2+1, ((s+e)/2)+1, e));
            }
            T _update(int l, int r, T v, int now, int s, int e)
            {
                if(s>r or e<l) return tree[now];
                else if(l<=s and e<=r) 
                {
                    lazy[now]=f(lazy[now], v*(e-s+1));
                    return tree[now]=f(tree[now], v*(e-s+1));
                }
                _lazy_update(now, s, e);
                return tree[now]=f(_update(l, r, v, now*2, s, (s+e)/2), _update(l, r, v, now*2+1, ((s+e)/2)+1, e));
            }
            void _lazy_update(int now, int s, int e)
            {
                if(lazy[now])
                {
                    int m=(s+e)/2;
                    tree[now*2]=f(tree[now*2], lazy[now]/(e-s+1) * (m-s+1));
                    lazy[now*2]=f(lazy[now*2], lazy[now]/(e-s+1) * (m-s+1));
                    tree[now*2+1]=f(tree[now*2+1], lazy[now]/(e-s+1) * (e-m));
                    lazy[now*2+1]=f(lazy[now*2+1], lazy[now]/(e-s+1) * (e-m));
                    lazy[now]=0;
                }
            }
        public:
            lazy_seg_tree(int size)
            {
                this->size=size;
                tree.assign(size*4, initial);
                lazy.assign(size*4, 0);
            }
            T init(const vector<T>& arr, int now=0, int s=0, int e=0)
                { return now ? _init(arr, now, s, e):_init(arr, 1, 0, size-1); }
            T get(int l, int r, int now=0, int s=0, int e=0)
                { return now ? _get(l, r, now, s, e):_get(l, r, 1, 0, size-1); }
            T update(int l, int r, T value, int now=0, int s=0, int e=0)
                { return now ? _update(l, r, value, now, s, e):_update(l, r, value, 1, 0, size-1); }
    };
};
