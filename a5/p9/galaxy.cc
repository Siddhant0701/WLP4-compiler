#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using std::vector;
using std::string;
using std::string;
using std::istream;
using std::ostream;
using std::stringstream;

void skipLine(istream&);
void skipGrammar(istream&);
string trim(const string &);
vector<string> split(string x);
int parse(vector<string>, int, int);
int bracketparse(vector<string> v, int);
int skip(vector<string>, int);

//Skip the grammar part of the input.
void skipLine(istream &in) {
  string s;
  getline(in, s);
}
 
void skipGrammar(istream &in) {
  int i, numTerm, numNonTerm, numRules;

  // read the number of terminals and move to the next line
  in >> numTerm;
  skipLine(in);
  
  // skip the lines containing the terminals
  for (i = 0; i < numTerm; i++) {
    skipLine(in);
  }
  
  // read the number of non-terminals and move to the next line
  in >> numNonTerm;
  skipLine(in);
  
  // skip the lines containing the non-terminals
  for (i = 0; i < numNonTerm; i++) {
    skipLine(in);
  }
  
  // skip the line containing the start symbol
  skipLine(in);
  
  // read the number of rules and move to the next line
  in >> numRules;
  skipLine(in);
  
  // skip the lines containing the production rules
  for (i = 0; i < numRules; i++) {
    skipLine(in);
  }
}

string trim(const string &str) {
  size_t begin = str.find_first_not_of(" \t\n");
  if (begin == string::npos) return "";

  size_t end = str.find_last_not_of(" \t\n");

  return str.substr(begin, end - begin + 1);
}

int main(void)
{
    vector<string> terminals{"BOF", "EOF", "id" ,"-", "(", ")"};
    vector<string> nterminals{"S", "expr", "term"};
    string start = "S";

    vector<string> derivation;

    vector<vector<string>> rules{
        {"S", "BOF", "expr", "EOF"},
        {"expr", "term"},
        {"expr", "expr", "-", "term"},
        {"term", "id"},
        {"term", "(" , "expr", ")"}
    };
    skipGrammar(std::cin);

    string line;
    while (getline(std::cin, line)) derivation.emplace_back(trim(line));

    vector <string> temp;
    vector <string> result;

    int counter=0;
    temp = split(derivation[0]);
    temp.erase(temp.begin());

    while (temp.size() > 0 && counter < derivation.size())
    {
        if(std::find(terminals.begin(), terminals.end(), temp[0]) != terminals.end())
        {
            result.emplace_back(temp[0]);
            temp.erase(temp.begin());
        }
        else
        {
            temp.erase(temp.begin());
            vector<string> r = split(derivation[++counter]);
            for (int i=r.size()-1; i>=1;i--) temp.emplace(temp.begin(), r[i]);
            r.clear();
        }
    }

    int derivation_res = parse(result,0,result.size());
    std::cout << derivation_res << std::endl;
}

int parse(vector<string> v, int start, int end)
{
    bool s= false;
    int result;
    for (int i=start; i<end; i++)
    {
        if(v[i]=="id")
        {
            if(!s) 
            {
                result = 42;
                s = true;
            }
            else result -=42;
        }

        else if(v[i] == "(")
        {
            if(!s) 
            {
                result = bracketparse(v,i);
                s = true;
            }
            else result-=bracketparse(v,i);

            i+=skip(v,i);
        }
        else continue;
    }   
    return result;
}

int bracketparse(vector<string>v, int start)
{
    int stack = 1;
    int counter = start+1;
    while (stack != 0)
    {
        if(v[counter] =="(") stack++;
        else if(v[counter] == ")") stack--;
        counter++;
    }
    return parse(v, start+1, counter-1);
}

vector<string> split(string x)
{
    vector<string> res;
    string line;
    stringstream ss{x};
    while(getline(ss, line, ' ')) res.emplace_back(line);
    return res;
}

int skip(vector<string> v, int i)
{
    int stack = 1;
    int counter = 1;
    while (stack != 0)
    {
        if(v[i+counter] =="(") stack++;
        else if(v[i+counter] == ")") stack--;
        counter++;
    }
    return counter;
}
