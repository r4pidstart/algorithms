int spfa(vector<vector<pair<int, int> > >& graph, vector<int>& dist, int start)
{
    vector<int> cycle(graph.size()), in_q(graph.size());
    queue<int> q;
    q.push(start);
    dist[start]=0;

    while(q.size())
    {
        auto now=q.front(); q.pop();
        in_q[now]=0;
        for(auto next : graph[now])
            if(dist[next.first] > dist[now]+next.second)
            {
                dist[next.first]=dist[now]+next.second;
                if(!in_q[next.first])
                {
                    cycle[next.first]++;
                    if(cycle[next.first]==graph.size())
                        return 1;
                    in_q[next.first]=1;
                    q.push(next.first);
                }
            }
    }
    return 0;
}
