#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::vector;

struct Tree{
    int val;
    int childCount;
    vector<Tree*> children;

    Tree(int v, int c) 
    {
        val = v;
        childCount = c;

        int k,l;
        for (int i=0; i<c; i++)
        {   
            cin >> k >> l;
            Tree * u = new Tree(k,l);
            children.emplace_back(u);
        }
    }
    ~Tree()
    {
        for (auto n: children)
            delete n;
        children.clear();
    }

    void printPostOrder()
    {
        for (auto n: children)
            n->printPostOrder();
        cout << val << " " << childCount << endl; 
    }    
};

int main(void)
{
    int i,j;
    cin >>i>>j;
    Tree * t = new Tree(i,j);
    t->printPostOrder();
    delete t;
}
