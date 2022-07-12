#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

//DECLARATIONS
struct Tree;
struct Function;

bool nonterminal (string);
bool declaration(Tree *, vector<string>, vector<string>);

void checkValid(Tree *, vector<Function>, string);

bool isInt(Tree*);
bool isPtr(Tree*);
bool checkArgs(Tree*, vector<Function>, string);

string getType(Tree *);
string getID(Tree *);
string getIDType(string,string, vector<Function>);


void generateWain(Tree *, vector <Function>, vector<string>*);
void generatePrologue(vector<string>*);
void generateEpilogue(vector<string>*);
void code(Tree *, Function, vector<string>*);
void push(int, vector<string>*);
void pop(int, vector<string>*);


//STRUCTURES
struct Tree {
    vector <string> rule;
    vector <Tree *> children;
    string type;

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
    vector <string> types;
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

        ids.emplace_back(id);
        types.emplace_back(type);
        used.emplace_back(id);

        if (param) params.emplace_back(type);
    }
    Function(){}
    ~Function(){
        params.clear();
        ids.clear();
        types.clear();
        used.clear();
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

//BOOLS
bool nonterminal(string s) {return (s[0] == tolower((char)s[0]));}
bool declaration(Tree * t, vector<string> v, vector <string> v2)
{
    bool res= true;
    res = res && (!(t->rule[0] == "ID") || (std::find(v.begin(), v.end(), t->rule[1]) != v.end())
        || (std::find(v2.begin(), v2.end(), t->rule[1]) != v2.end()));
    for (auto n : t->children) res = res && declaration(n,v, v2);
    return res;
}
bool checkArgs(Tree * t, vector<Function> procedures, string s)
{
    int ind;
    for(ind = 0; ind < procedures.size(); ind++) {if (procedures[ind].name == s) break;}
    Function func = procedures[ind];

    vector <string> temp;
    if(t->rule[3] != "arglist") return func.params.size() == 0;

    Tree * x = t->children[2];

    while(x->rule.size() > 2)
    {
        temp.emplace_back(x->children[0]->type);
        x = x->children[2];
    }
    temp.emplace_back(x->children[0]->type);

    if(temp.size()  != func.params.size()) return false;
    for(int i=0; i<temp.size(); i++)
    {
        if(temp[i] != func.params[i]) return false;
    }
    return true;
}

//GETS
string getType(Tree * t)
{
    if(t->children[0]->rule.size()==2) return "int";
    return "int*";
}
string getID(Tree * t) {return t->children[1]->rule[1];}
string getIDType(string s, string f, vector<Function> v)
{
    int ind;
    for(ind = 0; ind < v.size(); ind++)
    {
        if (v[ind].name == f) break;
    }

    Function func = v[ind];
    for (int i=0 ; i < func.ids.size();i++)
    {
        if (func.ids[i] == s) return func.types[i];
    }
    throw 1;
}

void checkValid(Tree * t, vector <Function> procedures, string s = "")
{
    if(t->rule.size() > 1 && t->rule[1] == "procedure") s = t->children[0]->children[1]->rule[1];
    if(t->rule.size() > 1 &&t->rule[1] == "main") s = "wain";

    if (!(nonterminal(t->rule[0])))
    {
        if(t->rule[0] == "NUM")
            t->type = "int";
        
        else if(t->rule[0] == "NULL")
            t->type = "int*";
    }
    
    else
    {
        for (auto a : t->children) checkValid(a, procedures, s); 

        if(t->rule[0] == "factor")
        {
            if(t->rule[1] == "NUM")
                t->type = "int";
            
            else if(t->rule[1] == "NULL")
                t->type = "int*";
            
            else if(t->rule[1] == "ID" && t->rule.size() == 2)
                t->type = getIDType(t->children[0]->rule[1], s, procedures);
            
            else if(t->rule[1] == "LPAREN")
                t->type = t->children[1]->type;
            
            else if (t->rule[1] == "AMP" && t->children[1]->type == "int")
                t->type = "int*";
            
            else if(t->rule[1] == "STAR" && t->children[1]->type == "int*")
                t->type = "int";
            
            else if(t->rule[1] == "NEW" && t->children[3]->type == "int")
                t->type = "int*";
            
            else if(t->rule[1] == "ID" && t->rule.size() >= 4)
            {   
                if(!checkArgs(t,procedures,t->children[0]->rule[1])) throw 1;
                t->type = "int";
            }
            
            else throw 1;
        }

        else if(t->rule[0] == "lvalue")
        {
            if(t->rule[1] == "ID")
                t->type = getIDType(t->children[0]->rule[1], s, procedures);
            
            else if(t->rule[1] == "LPAREN")
                t->type = t->children[1]->type;
            
            else if(t->rule[1] == "STAR" && t->children[1]->type == "int*")
                t->type = "int";
            
            else throw 1;
        }
    
        else if(t->rule[0] == "term")
        {
            if(t->rule[1] == "factor")
                t->type = t->children[0]->type;
            else
            {
                t->type = "int";
                if(t->children[0]->type != t->children[2]->type || t->children[0]->type != "int") throw 1;
            }
        }
    
        else if(t->rule[0] == "expr")
        {
            if(t->rule[1] == "term")
                t->type = t->children[0]->type;
            
            else if(t->rule[2] == "PLUS")
            {
                if(t->children[0]->type == "int" && t->children[2]->type == "int")
                    t->type = "int";
                
                else if(t->children[0]->type == "int*" && t->children[2]->type == "int")
                    t->type = "int*";
                
                else if(t->children[0]->type == "int" && t->children[2]->type == "int*")
                    t->type = "int*";
                
                else  throw 1;
            }
        
            else if(t->rule[2] == "MINUS")
            {
                if(t->children[0]->type == "int" && t->children[2]->type == "int")
                    t->type = "int";
                
                else if(t->children[0]->type == "int*" && t->children[2]->type == "int")
                    t->type = "int*";

                 else if(t->children[0]->type == "int*" && t->children[2]->type == "int*")
                    t->type = "int";

                else throw 1;
            }
        
            else throw 1;
        }
    
        else if(t->rule[0] == "dcls" && t->rule.size() > 1)
        {
            if(!(((t->rule[4] == "NUM") && (t->children[1]->children[0]->rule.size() == 2)) ||
                (t->rule[4] == "NULL" && t->children[1]->children[0]->rule.size() == 3))) throw 1;
        }
    
        else if(t->rule[0] == "test")
        {
            if(t->children[0]->type != t->children[2]->type) throw 1;
        }
    
        else if(t->rule[0] == "statement" && t->rule.size() > 3)
        {
            if(t->rule[1] == "WHILE" || t->rule[1] == "IF" || t->rule[1] == "statements") return;
            if(!((t->children[0]->type == t->children[2]->type && t->rule[1] == "lvalue") ||
                (t->rule[1] == "PRINTLN" && t->children[2]->type == "int") ||
                (t->rule[1] == "DELETE" && t->children[3]->type == "int*"))) throw 1;
        }

        else if(t->rule[0] == "procedure")
        {
            if(t->children[9]->type != "int") throw 1;
        }
        
        else if(t->rule[0] == "main")
        {
            if((t->children[11]->type != "int") || 
            (t->children[5]->children[0]->rule.size() != 2)) throw 1;
        }
    }
}


// GENERATE CODE

void generateWain(Tree * t, vector<Function> procedures, vector<string> * codeGen)
{
    generatePrologue(codeGen);
    code(t->children[8], procedures[0], codeGen);
    code(t->children[9], procedures[0], codeGen);
    code(t->children[11], procedures[0], codeGen);
    generateEpilogue(codeGen);
}
void generatePrologue(vector<string>* codeGen)
{
    codeGen->emplace_back("lis $4");
    codeGen->emplace_back(".word 4");
    codeGen->emplace_back("sub $29, $30, $4");
    codeGen->emplace_back("sw $1, 0($29)");
    codeGen->emplace_back("sub $30, $30, $4");
    codeGen->emplace_back("sw $2, -4($29)");
    codeGen->emplace_back("sub $30, $30, $4");
}
void generateEpilogue(vector<string>* codeGen)
{
    codeGen->emplace_back("add $30, $30, $4");
    codeGen->emplace_back("add $30, $30, $4");
    codeGen->emplace_back("jr $31");
}
void code(Tree * t, Function f, vector<string> * codeGen)
{
    if(t->rule[0] == "ID")
    {   
        int index;
       for(int i=0; i < f.ids.size(); i++)
        {
            index =i;
            if(t->rule[1] == f.ids[i]) break;
        }
        if(index>1) index = f.ids.size() + 1 -index;
        codeGen->emplace_back("lw $3, " + to_string(-4*index) + "($29)");
    }

    else if(t->rule[0] == "NUM") 
    {
        codeGen->emplace_back("lis $3");
        codeGen->emplace_back(".word " + (t->rule[1]));
    }

    else if(t->rule[0] == "expr") 
    {
        if(t->rule.size() == 2) code (t->children[0], f, codeGen);
        else
        {
            code(t->children[0],f, codeGen);
            push(3, codeGen);
            code(t->children[2], f, codeGen);
            pop(5, codeGen);

            if(t->rule[2] == "PLUS") codeGen->emplace_back("add $3, $5, $3");
            else if(t->rule[2] == "MINUS") codeGen->emplace_back("sub $3, $5, $3");

        }
    }
    
    else if(t->rule[0] == "term") 
    {
        if(t->rule.size() == 2) code(t->children[0], f, codeGen);
        else
        {
            code(t->children[0],f, codeGen);
            push(3, codeGen);
            code(t->children[2],f, codeGen);
            pop(5, codeGen);

            if(t->rule[2] == "STAR") 
            {
                codeGen->emplace_back("mult $5, $3"); 
                codeGen->emplace_back("mflo $3");
            }
            else if(t->rule[2] == "SLASH") 
            {
                codeGen->emplace_back("div $5, $3"); 
                codeGen->emplace_back("mflo $3");
            }
            else if(t->rule[2] == "PCT") 
            {
                codeGen->emplace_back("div $5, $3");
                codeGen->emplace_back("mfhi $3");
            }
        }
    }
    
    else if(t->rule[0] == "factor") 
    {
        if(t->rule.size() == 2) code(t->children[0],f, codeGen);
        else code(t->children[1],f, codeGen);
    }

    else if(t->rule[0] == "statement")
    {
        if(t->rule[1] == "PRINTLN")
        {
            if((*codeGen)[0] != ".import print") codeGen->insert(codeGen->begin(), ".import print");
            push(1,codeGen);
            code(t->children[2], f, codeGen);
            codeGen->emplace_back("add $1, $3, $0");
            push(31, codeGen);
            codeGen->emplace_back("lis $5");
            codeGen->emplace_back(".word print");
            codeGen->emplace_back("jalr $5");
            pop(31,codeGen);
            pop(1,codeGen);
        }
        else
        {
            code(t->children[2],f,codeGen);

            int ind;
            Tree * x = t->children[0];
            while(x->rule[1] != "ID") x = x->children[1];
            
            for(int i=0; i < f.ids.size();i++)
            {
                if(x->children[0]->rule[1] == f.ids[i])
                {
                    ind = i;
                    break;
                }
            }
            if(ind>1) ind = f.ids.size() + 1 -ind;
            codeGen->emplace_back("sw $3, " + to_string(ind*-4) + "($29)");
        }
    }

    else if(t->rule[0] == "statements")
    {
        if(t->rule.size() != 1)
        {
            code(t->children[0], f, codeGen);
            code(t->children[1], f, codeGen);
        }
    }

    else if(t->rule[0] == "dcls")
    {
        if(t->rule.size() != 1)
        {
            code(t->children[0],f,codeGen);
            codeGen->emplace_back("lis $5");
            codeGen->emplace_back(".word " + t->children[3]->rule[1]);
            push(5,codeGen);
        }
    }

    else if(t->rule[0] == "lvalue")
    {
        if(t->rule.size() == 2) code(t->children[0], f, codeGen);
        else code(t->children[1], f, codeGen);
    }
}

void push(int a, vector<string>* codeGen)
{
    codeGen->emplace_back("sw $"+ to_string(a) +", -4($30)");
    codeGen->emplace_back("sub $30, $30, $4");
}
void pop(int a, vector <string> * codeGen)
{
    codeGen->emplace_back("add $30, $30, $4");
    codeGen->emplace_back("lw $"+ to_string(a) +", -4($30)");
}

int main() {
    Tree * myTree;
    vector <Function> procedures;
    vector <string> * codeGen = new vector<string>;
    try
    {
        //MAKE TREES AND PROCEDURES
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

        checkValid(myTree, procedures);
        generateWain(myTree->children[1]->children[0], procedures, codeGen);\

        for (auto a : *codeGen) cout << a << endl;

        delete myTree;
        delete codeGen;
        return 0;
    }

    catch(...){
        cerr << "ERROR" << endl;
        delete myTree;
        delete codeGen;
        return 1;
    }

}
