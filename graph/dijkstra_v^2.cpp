#include<bits/stdc++.h>
using namespace std;

class dijkstra
{
    // graph[from]={to, cost}, O(V^2+E)
    public:
        vector<int> dist;
        dijkstra(int start, const vector<vector<pair<int,int> > > &graph)
        {
            vector<int> visited(graph.size(), 0);
            dist.assign(graph.size(), INT32_MAX/2);
            dist[start]=0;
            
            while(true)
            {
                int now=-1, shortest=INT32_MAX;
                for(int i=0; i<graph.size(); i++)
                    if(shortest>dist[i] and !visited[i])
                        now=i, shortest=dist[i];
                if(now==-1) break;

                visited[now]=1;
                for(auto it : graph[now])
                {
                    if(visited[it.first]) continue;
                    dist[it.first]=min(dist[it.first], dist[now]+it.second);
                }
            }
        }
};