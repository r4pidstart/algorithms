class prim
{
    // graph[from]={to, cost}, O(ElogV)
    public:
        long long cost=0;
        prim(const vector<vector<pair<int, int> > >& graph)
        {
            if(graph.size()==1) return;
            int cnt=0;
            vector<int> visited(graph.size(), 0);
            priority_queue<pair<int, int>, vector<pair<int, int> >, greater<pair<int, int> > > pq;            
            pq.push({0,1});
            while(!pq.empty())
            {
                auto [now_cost, now]=pq.top(); pq.pop();
                if(visited[now]) continue;
                visited[now]=1, cnt++, cost+=now_cost;
                for(auto it : graph[now])
                    pq.push({it.second, it.first});
            }

            // if(cnt != graph.size()-1) cost=-1;
        }
};