#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

struct Tree;
struct Function;

bool nonterminal (string);
bool declaration(Tree *, vector<string>, vector<string>);

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
struct Function {
    string name;
    vector <string> params;
    vector <string> ids;
    vector <string> used;

    void print()
    {
        cerr << name << ":";
        for (auto a :params) cerr << " " << a;
        cerr <<endl;
        for(auto a : ids) cerr << a <<endl;
    }
    void insert(string id, string type, bool param= false)
    {
        string s;
        for(auto a : used) {if(a == id) throw 1;}

        s = id + " " + type;
        ids.emplace_back(s);
        used.emplace_back(id);

        if (param) params.emplace_back(type);
    }
    Function(){}
    ~Function(){
        params.clear();
        ids.clear();
    }
    void makeFunction(Tree * t, vector <string> names)
    {
        //Name
        this->name = t->children[1]->rule[1];
        
        Tree * x;
        //Parameters
        if (name == "wain")
        {
            Tree * t1 = t->children[3];
            Tree * t2 = t->children[5];

            string type1 = getType(t1);
            string id1 = getID(t1);

            string type2 = getType(t2);
            string id2 = getID(t2);
            
            insert(id1, type1, true);
            insert(id2, type2, true);
        }
        else
        {
            x = t->children[3];
            if (x->rule.size() != 1)
            {
                string tempId, tempType;
                x  = x ->children[0];
                while(x->rule.size() != 2){
                    tempId = getID(x->children[0]);
                    tempType = getType(x->children[0]);
                    insert(tempId, tempType, true);
                    x = x->children[2];
                }
                tempId = getID(x->children[0]);
                tempType = getType(x->children[0]);
                insert(tempId, tempType, true);
            }
        }

        //Checks and other variables
        int dclst = 6;
        if (name == "wain") dclst = 8;
        x = t->children[dclst];
        while (x->rule.size() != 1)
        {
            string tempId = getID(x->children[1]);
            string tempType = getType(x->children[1]);

            insert(tempId, tempType);
            x = x->children[0];
        }

        int st1=9;
        int st2=7;
        if (name == "wain") st2 = 11;
        
        vector <string> xx = names;
        xx.emplace_back(this->name);
        if(!declaration(t->children[st1], used, xx) || 
        !declaration(t->children[st2], used, xx)) throw 1;
    }
};

bool nonterminal(string s) {return (s[0] == tolower((char)s[0]));}
bool declaration(Tree * t, vector<string> v, vector <string> v2)
{
    bool res= true;
    res = res && (!(t->rule[0] == "ID") || (std::find(v.begin(), v.end(), t->rule[1]) != v.end())
        || (std::find(v2.begin(), v2.end(), t->rule[1]) != v2.end()));
    for (auto n : t->children) res = res && declaration(n,v, v2);
    return res;
}

string getType(Tree * t)
{
    if(t->children[0]->rule.size()==2) return "int";
    return "int*";
}
string getID(Tree * t) {return t->children[1]->rule[1];}

int main() {
    Tree * myTree;
    vector <Function> procedures;
    try
    {
        vector <string> v;
        vector <string> names;


        string s;
        getline(cin,s);

        istringstream ss {s};
        while (ss >> s) v.emplace_back(s);
        
        myTree = new Tree(v);
        myTree->makeTree();

        Tree * t= myTree->children[1];

        while(t->rule.size() == 3) 
        {
            Function f;
            f.makeFunction(t->children[0], names);
            procedures.emplace_back(f);
            if(std::find(names.begin(), names.end(), f.name) == names.end()) names.emplace_back(f.name);
            else throw 1;
            t = t->children[1];
        }
        t = t->children[0];
        Function f;
        f.makeFunction(t, names);
        procedures.emplace_back(f);
    
        for (auto temp : procedures) temp.print();

        //for (auto temp : procedures) delete temp;

        delete myTree;
        return 0;
    }

    catch(...){
        cerr << "ERROR" << endl;
        //for (auto temp : procedures) delete temp;
        delete myTree;
        return 1;
    }

}
