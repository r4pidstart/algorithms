struct Aho
{
    static Aho* root;
    map<char, Aho*> next;
    vector<string> searched;
    Aho* failure;
    Aho() {}
    Aho(int) { Aho::root=this; }
    ~Aho()
    {
        for(auto [c, n] : next)
            delete n;
    }

    void insert(const string& s, size_t idx=0)
    {
        char nc=s[idx];
        if(nc=='\0')
        {
            searched.push_back(s);
            return;
        }
        else if(!next[nc]) 
            next[nc]=new Aho();
        next[nc]->insert(s, idx+1);
    }
    int check(const string& s, size_t idx=0)
    {
        int nc=s[idx];
        if(searched.size()) return 1;
        if(nc=='\0') return 0;
        if(this!=Aho::root and next.find(nc) == next.end())
            return failure->check(s, idx);
        if(next.find(nc) != next.end())
            return next[nc]->check(s, idx+1);
        return check(s, idx+1);
    }
    // vector<string> find(const string& s, size_t idx=0) {}
    void get_failure()
    {
        root->failure=root;
        queue<Aho*> q;

        for(auto [c, n] : root->next)
            n->failure=root, q.push(n);
        while(q.size())
        {
            Aho* curr=q.front();
            q.pop();

            for(auto [c, n] : curr->next)
            {
                Aho* dest=curr->failure;
                while(dest != root and dest->next.find(c) == dest->next.end())
                    dest=dest->failure;
                if(dest->next.find(c) != dest->next.end())
                    dest=dest->next[c];
                n->failure=dest;

                n->searched.insert(n->searched.end(), n->failure->searched.begin(), n->failure->searched.end());
                q.push(n);
            }
        }
    }
};
Aho* Aho::root=0;
