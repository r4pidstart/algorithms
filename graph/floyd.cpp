class Floyd
{
    // adjacant matrix 1~n, O(v^3)
    public:
        vector<vector<int> > dist;
        Floyd(const vector<vector<int> >& graph)
        {
            dist=graph;
            for(int i=1; i<dist.size(); i++)
                for(int j=1; j<dist.size(); j++)
                    for(int k=1; k<dist.size(); k++)
                        dist[j][k]=min(dist[j][k], dist[j][i]+dist[i][k]);
        }
};