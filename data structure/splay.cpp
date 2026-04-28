template <typename T>
class Splay
{
public:
    struct Node
    {
        T key;
        Node *l, *r, *p; // left, right, parent
        int subtree_size=0; // include this node
        int flipped=0;
    };
    Node* root=nullptr;

    void flip(int s, int e)
    {
        Node* target=gather(s, e);
        target->flipped^=1;
    }

    void push(Node* x)
    {
        if(x->flipped)
        {
            swap(x->l, x->r);
            if(x->l) x->l->flipped^=1;
            if(x->r) x->r->flipped^=1;
            x->flipped=0;
        }
    }

    void update(Node* x)
    {
        x->subtree_size=1;
        if(x->l) 
        {
            x->subtree_size+=x->l->subtree_size;
        }
        if(x->r) 
        {
            x->subtree_size+=x->r->subtree_size;
        }
    }

    void rotate(Node *x)
    {
        Node *p=x->p;
        if(p==nullptr) return; // x is already root

        Node *b;
        if(x==p->l) // left rotation
        {
            b=x->r;
            p->l=b;
            x->r=p;
        }
        else //rright rotation
        {
            b=x->l;
            p->r=b;
            x->l=p;
        }

        if(b) b->p=p;
        x->p=p->p;
        p->p=x;

        Node *gp=x->p;
        if(gp)
        {
            if(gp->l==p) gp->l=x;
            else gp->r=x;
        }
        else
            root=x; // now x is root

        update(p); // child first
        update(x);
    }
    
    // if g is nullptr, splay x as root else splay x until g is parent of x
    void splay(Node *x, Node *g=nullptr)
    {
        vector<Node*> s;
        for(Node* p=x; p!=g; p=p->p) s.push_back(p);
        for(int i=s.size()-1; i>=0; i--) push(s[i]);

        while(x->p != g) // while x is not root or until x's parent is g
        {
            Node *p=x->p;
            if(p->p == g) // grandparent is g(nullptr or g)
            {
                rotate(x);
                break;
            }
            Node *gp=p->p;
            if(x==p->l and p==gp->l) rotate(p), rotate(x);
            else rotate(x), rotate(x);
        }
        if(g==nullptr) root=x; // now x is root
    }

    // insert key and splay it to root
    void insert(const T& key)
    {
        if(root==nullptr)
        {
            Node* new_node=new Node({key});
            root=new_node;
            return;
        }
        
        Node *p=root;
        Node **insert_location;
        while(true)
        {
            if(key==p->key) return;
            if(key < p->key)
            {
                if(p->l) p=p->l;
                else // empty. insert here
                {
                    insert_location=&p->l;
                    break;
                }
            }
            else
            {
                if(p->r) p=p->r;
                else // empty. insert here
                {
                    insert_location=&p->r;
                    break;
                }
            }
        }

        Node* new_node=new Node({key, nullptr, nullptr, p});
        *insert_location=new_node;
        splay(new_node);
    }

    bool find(const T& key)
    {
        // find key and splay it to root
        Node* p=root;
        while(p)
        {
            if(key==p->key) break;
            if(key < p->key)
            {
                if(p->l==nullptr) break;
                p=p->l;
            }
            else
            {
                if(p->r==nullptr) break;
                p=p->r;
            }
        }
        splay(p);
        return key==p->key;
    }
    
    void remove(const T& key)
    {
        if(find(key)==false) return;
        Node* p=root;
        if(p->l and p->r)
        {
            Node *left=p->l, *right=p->r;
            left->p=nullptr;

            root=left;
            Node* x=root;
            while(x->r) x=x->r;
            splay(x); // now x is root 

            x->r=right;
            right->p=x;
            update(x);
            delete p;
        }
        else if(p->l)
        {
            root=p->l;
            root->p=nullptr;
            delete p;
        }
        else if(p->r)
        {
            root=p->r;
            root->p=nullptr;
            delete p;
        }
        else
        {
            root=nullptr;
            delete p;
        }
    }

    void nth_element(int n)
    {
        Node* p=root;
        if(p==nullptr or n<0 or n>=p->subtree_size)
            return; 

        while(true)
        {
            push(p);
            while(p->l and p->l->subtree_size>n) 
            {
                p=p->l;
                push(p);
            }
            if(p->l) n-=p->l->subtree_size;
            if(n==0) break;
            n--; // this node
            p=p->r;
        }
        splay(p); // now nth element is root
    }

    // return the root of subtree that contains [s, e]
    Node* gather(int s, int e)
    {
        nth_element(e+1);
        Node* eplus1=root;
        nth_element(s-1);
        splay(eplus1, root);
        return root->r->l;
    }

    // cut [s, e] and paste it after kth element(remaining sequence after cut [s, e])
    Node* cut_and_paste(int s, int e, int k)
    {
        Node* target=gather(s, e);
        root->r->l=nullptr;

        update(root->r);
        update(root);

        gather(k, k);

        root->r->l=target;
        target->p=root->r;
        update(root->r);
        update(root);
    }
};
