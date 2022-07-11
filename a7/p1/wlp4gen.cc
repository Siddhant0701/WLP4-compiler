#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

struct Tree;

bool nonterminal (string);
bool declaration(Tree *, vector<string>);
string getType(Tree *);
string getID(Tree *);

struct Tree {
    vector <string> rule;
    vector <Tree *> children;

    Tree(vector <string> v): rule{v} {}
    void makeTree() {
        for (int i=1 ; i< rule.size(); i++)
        {
            vector <string> temp;
            string s;
            getline(cin,s);

            istringstream ss {s};
            while (ss >> s) temp.emplace_back(s);

            Tree * t = new Tree(temp);
            
            if(nonterminal(rule[i])) t->makeTree();

            children.emplace_back(t);
        }
    }

    ~Tree() {
        rule.clear();
        for (auto i : children) delete i;
    }
};

bool nonterminal(string s) {return (s[0] == tolower((char)s[0]));}

string getType(Tree * t)
{
    if(t->children[0]->rule.size()==2) return "int";
    return "int*";
}

string getID(Tree * t) {return t->children[1]->rule[1];}

bool declaration(Tree * t, vector<string> v)
{
    bool res= true;
    res = res && (!(t->rule[0] == "ID") || (std::find(v.begin(), v.end(), t->rule[1]) != v.end()));
    for (auto n : t->children) res = res && declaration(n,v);
    return res;
}

int main() {
    Tree * myTree;
    try
    {
    
        vector <string> v;
        string s;
        getline(cin,s);

        istringstream ss {s};
        while (ss >> s) v.emplace_back(s);
        
        myTree = new Tree(v);
        myTree->makeTree();


        Tree * t1 = myTree->children[1]->children[0]->children[3];
        Tree * t2 = myTree->children[1]->children[0]->children[5];

        string type1 = getType(t1);
        string id1 = getID(t1);

        string type2 = getType(t2);
        string id2 = getID(t2);

        pair <string,string> g1(type1,id1);
        pair <string,string> g2(type2,id2);

        vector <string> ids;
        vector <pair<string, string>> results {g1,g2};

        if(std::find(ids.begin(), ids.end(), id1) == ids.end()) ids.emplace_back(id1);
        else throw 1;

        if(std::find(ids.begin(), ids.end(), id2) == ids.end()) ids.emplace_back(id2);
        else throw 1;

        Tree * t3 = myTree->children[1]->children[0]->children[8];

        while (t3->rule.size() != 1)
        {
            string id3 = getID(t3->children[1]);
            pair <string,string> g(getType(t3->children[1]), id3);
            
            if(std::find(ids.begin(), ids.end(), id3) == ids.end()) ids.emplace_back(id3);
            else throw 1;

            results.emplace_back(g);
            t3 = t3->children[0];
        }

        if(!declaration(myTree->children[1]->children[0]->children[9], ids) || 
        !declaration(myTree->children[1]->children[0]->children[11], ids)) throw 1;

        string wain = "wain: ";
        wain += type1;
        wain += " ";
        wain += type2;

        cerr << wain << endl;
        for(int i=0 ; i< results.size()-1; i++) cerr << results[i].second << " " << results[i].first << endl;
        cerr << results[results.size()-1].second << " " << results[results.size()-1].first << endl;

        
        delete myTree;
        return 0;

    }

    catch(...){
        cerr << "ERROR" << endl;
        delete myTree;
        return 1;
    }

}
