class Hld_With_Seg
{
private:
    template<typename T>
    class seg_tree;

    vector<seg_tree<int64_t> > segs;
    vector<int> depth, subtree_size, group_idx_of_node, depth_of_node, 
                top_node_of_group, bottom_node_of_group, parent;
    vector<tuple<int, int, int64_t> > edges; // {a, b, cost}
    map<pair<int, int>, int> edge_idx; // {a, b} -> edge_idx
    map<int, pair<int, int> > edge_to_seg_idx; // edge_idx -> {group_idx, seg_idx}
    
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
    int heavy_light_decomposition(const vector<vector<pair<int, int64_t> > >& graph, int now, int par, int dep)
    {
        static int group_idx=0;

        group_idx_of_node[now]=group_idx;
        depth_of_node[now]=dep;

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
        heavy_light_decomposition(graph, heavy_child, now, dep+1);
        for(auto& [next, cost] : graph[now])
        {
            if(next==par or next==heavy_child) continue;
            assert(top_node_of_group.size()==group_idx+1);
            top_node_of_group.push_back(next);
            group_idx++;
            parent[next]=now;
            heavy_light_decomposition(graph, next, now, dep+1); // make a new group
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
    Hld_With_Seg(const vector<vector<pair<int, int64_t> > >& graph)
    {
        subtree_size.assign(graph.size(), 0);
        depth.assign(graph.size(), 0);
        get_subtree_size(graph, 1, -1);

        parent.assign(graph.size(), -1);
        group_idx_of_node.assign(graph.size(), -1);
        depth_of_node.assign(graph.size(), -1);
        top_node_of_group.push_back(1); // init
        heavy_light_decomposition(graph, 1, -1, 0);
        init_segment_tree();
    }

    int update_one_edge(int edge_idx, int64_t value)
    {
        if(edge_to_seg_idx.count(edge_idx))
        {
            auto [target_group, target_seg_idx]=edge_to_seg_idx[edge_idx];
            segs[target_group].update(target_seg_idx, value);
        }
        get<2>(edges[edge_idx])=value;
        return 0;
    }
    int update_one_edge(int a, int b, int64_t value)
    {
        if(parent[b]!=a) swap(a, b);
        assert(parent[b]==a);
        return update_one_edge(edge_idx[{a, b}], value);
    }
    int update_route(int a, int b, int64_t value)
    {
        // TODO
        return 0;
    }
    int64_t get_route(int a, int b)
    {
        int64_t ret=segs[0].base;
        while(group_idx_of_node[a] != group_idx_of_node[b])
        {
            int top_node_of_group_a=top_node_of_group[group_idx_of_node[a]], 
                top_node_of_group_b=top_node_of_group[group_idx_of_node[b]];
            
            if(depth[top_node_of_group_a] > depth[top_node_of_group_b])
                swap(a, b), swap(top_node_of_group_a, top_node_of_group_b);
        
            int b_seg_idx=group_idx_of_node[b];
            int b_seg_range_end=depth[b]-depth[top_node_of_group_b]+1;
            // TODO: check segs[].get(1, 0);
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
    class seg_tree // 1-based
    {
        public:
            T initial=T();
            static const T base=T();
            inline T f(const T& a, const T& b) 
            {
                return max(a, b);
            }
            int size;
            vector<T> tree;
            T _init(const vector<T>& arr, int now, int s, int e)
            {
                if(s==e) return tree[now]=arr[s];
                else return tree[now]=f(_init(arr, now*2, s, (s+e)/2), _init(arr, now*2+1, ((s+e)/2)+1, e)); 
            }
            T _get(int l, int r, int now, int s, int e)
            {
                if(s>r or e<l) return base;
                else if(l<=s and e<=r) return tree[now];
                else return f(_get(l, r, now*2, s, (s+e)/2), _get(l, r, now*2+1, ((s+e)/2)+1, e));
            }
            T _update(int i, T v, int now, int s, int e)
            {
                if(!(s<=i and i<=e)) return tree[now];
                else if(s==e and s==i) return tree[now]=v;
                else return tree[now]=f(_update(i, v, now*2, s, (s+e)/2), _update(i, v, now*2+1, ((s+e)/2)+1, e));
            }
        public:
            seg_tree(int size)
            {
                this->size=size;
                tree.assign(size*4, initial);
            }
            T init(const vector<T>& arr, int now=0, int s=0, int e=0)
                { return now ? _init(arr, now, s, e):_init(arr, 1, 0, size-1); }
            T get(int l, int r, int now=0, int s=0, int e=0)
                { return now ? _get(l, r, now, s, e):_get(l, r, 1, 0, size-1); }
            T update(int index, T value, int now=0, int s=0, int e=0)
                { return now ? _update(index, value, now, s, e):_update(index, value, 1, 0, size-1); }
    };

};
