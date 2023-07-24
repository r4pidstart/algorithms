class Floyd
{
    // adjacent matrix 1~n, O(V^3)
    public:
        vector<vector<int> > dist;
        Floyd(const vector<vector<int> >& graph)
        {
            dist=graph;
            for(int k=1; k<dist.size(); k++)
                for(int i=1; i<dist.size(); i++)
                    for(int j=1; j<dist.size(); j++)
                        dist[i][j]=min(dist[i][j], dist[i][k]+dist[k][j]);
        }
};
