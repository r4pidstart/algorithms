class bipartite_matching
{
    // O(VE)
    private:
        vector<vector<int> > graph;
        vector<int> btoa, visited;
        int visit_cnt;
        bool make_match(int now)
        {
            visited[now]=visit_cnt;
            for(int next : graph[now])
                if(btoa[next] == -1 or (visited[btoa[next]] != visit_cnt and make_match(btoa[next])))
                {
                    btoa[next]=now;
                    return 1;
                }
            return 0;
        }
    public: 
        bipartite_matching(int sizeA, int sizeB)
        {
            graph.assign(sizeA, vector<int>());
            btoa.assign(sizeB, -1);
            visited.assign(sizeA, 0);
        }
        inline void add_edge(int fromA, int toB)
        {
            graph[fromA].push_back(toB);
        }
        int find_match(void) 
        {
            int match=0;
            for(int i=0; i<graph.size(); i++)
            {
                visit_cnt++;
                if(make_match(i)) match++;
            }
            return match;
        }
};
