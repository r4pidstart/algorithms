class DLX
{  
    struct Node 
    {
        int row;
        int size; // only for column header
        Node *column;
        Node *u, *d, *l, *r;
    };

    static void make_linked_list(const vector<vector<int> >& table, Node& head, int primary_cols=0)
    {
        vector<Node*> column(table[0].size());
        for(int i=0; i<table[0].size(); i++)
        {
            column[i]=new Node();
            column[i]->size=0;
            column[i]->column=column[i];
        }
        
        int n=primary_cols ? primary_cols : table[0].size();
        head.l=column[n-1];
        head.r=column[0];

        for(int i=0; i<table[0].size(); i++)
        {
            column[i]->u=column[i];
            column[i]->d=column[i];

            if(i<n)
            {
                if(i==0) column[i]->l=&head;
                else column[i]->l=column[i-1];
    
                if(i==n-1) column[i]->r=&head;
                else column[i]->r=column[i+1];
            }
            else
            {
                column[i]->l=column[i];
                column[i]->r=column[i];
            }
        }

        vector<int> randomized_row_order(table.size());
        iota(randomized_row_order.begin(), randomized_row_order.end(), 0);
        mt19937 g(random_device{}());
        shuffle(randomized_row_order.begin(), randomized_row_order.end(), g);

        // for(int i=0; i<table.size(); i++) // row
        for(int i : randomized_row_order)
        {
            Node* last=nullptr;
            for(int j=0; j<table[i].size(); j++) // column
            {
                if(table[i][j])
                {
                    Node* now=new Node();
                    now->row=i, now->column=column[j];
                    now->u=column[j]->u;
                    now->d=column[j]; 
                    column[j]->u->d=now;
                    column[j]->u=now;
                    column[j]->size++;

                    if(last)
                    {
                        now->l=last;
                        now->r=last->r;
                        now->r->l=now;
                        last->r=now;
                    }
                    else
                    {
                        now->l=now;
                        now->r=now;
                    }

                    last=now;
                }
            }
        }         
    }

    static void cover(Node* column)
    {
        column->r->l=column->l;
        column->l->r=column->r;

        for(Node* now=column->d; now!=column; now=now->d)
            for(Node* now_row=now->r; now_row!=now; now_row=now_row->r)
            {
                now_row->column->size--;
                now_row->d->u=now_row->u;
                now_row->u->d=now_row->d;
            }
    }

    static void uncover(Node* column)
    {
        for(Node* now=column->u; now!=column; now=now->u)
            for(Node* now_row=now->l; now_row!=now; now_row=now_row->l)
            {
                now_row->column->size++;
                now_row->d->u=now_row;
                now_row->u->d=now_row;
            }

        column->l->r=column;
        column->r->l=column;
    }

public:
    static int dlx(vector<int>& tmp, Node& head)
    {
        // end condition!
        if(head.r==&head) return 1;

        // find the column with the smallest size
        Node* ptr=nullptr;
        int low=INT32_MAX;
        for(Node* now=head.r; now!=&head; now=now->r)
        {
            if(now->size < low)
            {
                if(now->size==0) return 0; // there's no answer
                low=now->size;
                ptr=now;
            }
        }

        // yes
        cover(ptr);
        for(Node* now=ptr->d; now!=ptr; now=now->d)
        {
            tmp.push_back(now->row);
            for(Node* next=now->r; next!=now; next=next->r)
                cover(next->column);
            
            if(dlx(tmp, head)) return 1;
            
            for(Node* next=now->l; next!=now; next=next->l)
                uncover(next->column);
            tmp.pop_back();
        }
        uncover(ptr);

        return 0;
    }

    static vector<int> solve(vector<vector<int> >& table)
    {
        Node head;
        make_linked_list(table, head);
        vector<int> tmp;
        dlx(tmp, head);

        return tmp;
    }
    static pair<vector<int>, bool> solve(vector<vector<int> >& table, vector<int>& preselected, int primary_cols=0)
    {
        Node head;
        make_linked_list(table, head, primary_cols);

        for(int i : preselected)
        {
            Node* target_node=nullptr;
            for(Node* col=head.r; col!=&head; col=col->r)
            {
                for(Node* node=col->d; node!=col; node=node->d)
                {
                    if(node->row == i)
                    {
                        target_node=node;
                        break;
                    }
                }
                if(target_node) break;
            }
            if(!target_node) return {{}, false}; // contradiction

            cover(target_node->column);
            for(Node* next = target_node->r; next != target_node; next = next->r)
                cover(next->column);
        }

        vector<int> tmp;
        if(!dlx(tmp, head))
            return {{}, false};

        return {tmp, true};
    }
};
