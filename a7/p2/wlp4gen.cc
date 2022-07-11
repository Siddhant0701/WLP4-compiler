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
string getprocedures(Tree *);

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

string getprocedures(Tree * t)
{
    vector <string> ids;
    string res = "";
    res += t->children[1]->rule[1];
    res+=":";

    Tree * x = t->children[3];
    if (x->rule.size() == 1) return res;

    x  = x ->children[0];
    while(x->rule.size() != 2){
        res += " ";
        res += getType(x->children[0]);

        string temp = getID(x->children[0]);

        if(std::find(ids.begin(), ids.end(), temp) == ids.end()) ids.emplace_back(temp);
        else throw 1;
        x = x->children[2];
    }
    res += " ";
    res += getType(x->children[0]);

    string temp = getID(x->children[0]);

    if(std::find(ids.begin(), ids.end(), temp) == ids.end()) ids.emplace_back(temp);
    else throw 1;

    return res;
}


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
        vector<string> procedures;
        vector <string> names;


        string s;
        getline(cin,s);

        istringstream ss {s};
        while (ss >> s) v.emplace_back(s);
        
        myTree = new Tree(v);
        myTree->makeTree();

        Tree * t;

        if(myTree->children[1]->rule.size() != 2)
        {
            t = myTree->children[1];

            while(t->rule.size() == 3) 
            {
                string s = getprocedures(t->children[0]);
                procedures.emplace_back(s);
                istringstream ss {s};
                ss >> s;
                if(std::find(names.begin(), names.end(), s) == names.end()) names.emplace_back(s);
                else throw 1;
                t = t->children[1];
            }
            t = t->children[0];
        }

        else
            t = myTree->children[1]->children[0];


        Tree * t1 = t->children[3];
        Tree * t2 = t->children[5];

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

        Tree * t3 = t->children[8];

        while (t3->rule.size() != 1)
        {
            string id3 = getID(t3->children[1]);
            pair <string,string> g(getType(t3->children[1]), id3);
            
            if(std::find(ids.begin(), ids.end(), id3) == ids.end()) ids.emplace_back(id3);
            else throw 1;

            results.emplace_back(g);
            t3 = t3->children[0];
        }

        if(!declaration(t->children[9], ids) || 
        !declaration(t->children[11], ids)) throw 1;

        string wain = "wain: ";
        wain += type1;
        wain += " ";
        wain += type2;

        for (auto k : procedures) cerr << k << endl;
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
