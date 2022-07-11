#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

int findTransition (vector <vector <string>>, string, string);
void printRule(vector<string>);
int main(void)
{
    int i;
    string s;
    vector <int> results;

    //CFG STUFF
    vector <string> nonterminals;
    vector <string> terminals;
    string start;
    vector <vector<string>> rules;

    cin >> i;
    for (int j=0 ; j<i; j++)
    {
        cin >> s;
        nonterminals.emplace_back(s);
    }

    cin >> i;
    for (int j=0 ; j<i; j++)
    {
        cin >> s;
        terminals.emplace_back(s);
    }

    cin >> start;

    cin >>i;
    getline(cin, s);
    for (int j=0 ; j<i; j++)
    {
        vector<string> v;
        getline(cin, s);

        istringstream ss(s);
        while (ss >> s) v.emplace_back(s);
        rules.emplace_back(v);
    }

    //DFA STUFF
    int states;
    vector <vector<string>> transitions;

    cin >> states;

    cin >> i;
    getline(cin, s);
    for (int j=0 ; j<i; j++)
    {
        vector<string> v;
        getline(cin, s);

        istringstream ss(s);
        while (ss >> s) v.emplace_back(s);
        transitions.emplace_back(v);
    }

    //SEQUENCE TO BE PARSED
    vector<string> sequence;

    while (cin >> s) sequence.emplace_back(s);


    int initial_size = sequence.size(); 
    string curr_state= "0";
    vector <string> sym_stack;
    vector <string> state_stack;
    
    while (sequence.size() > 0)
    {
        int to_apply = findTransition(transitions, curr_state,sequence[0]);
        if (to_apply == -1)
        {
            cerr << "ERROR at " << initial_size -sequence.size() + 1<< endl;
            return 1;
        }

        vector <string> v = transitions[to_apply];

        if(v[2] == "shift")
        {
            state_stack.emplace_back(curr_state);
            sym_stack.emplace_back(sequence[0]);
            sequence.erase(sequence.begin());
            curr_state = v[3];
        }

        else 
        {
            string curr_rule = v[3];
            int curr_rule_int;
            istringstream ss2(curr_rule);
            ss2 >> curr_rule_int;

            vector <string> temp = rules[curr_rule_int];

            for(int k=0 ; k< temp.size()-1; k++)
            {
                sym_stack.erase(sym_stack.end());
                curr_state = state_stack[state_stack.size()-1];
                state_stack.erase(state_stack.end());
            }

            sequence.insert(sequence.begin(),temp[0]);

            results.emplace_back(curr_rule_int);
        }
    }
    results.emplace_back(0);
    for (auto n : results) printRule(rules[n]);
}


int findTransition (vector <vector <string>> v, string s, string i)
{
    for (int j=0; j< v.size(); j++)
    {
        if ((v[j][0] == s) && (v[j][1] == i)) return j;
    }
    return -1;
}

void printRule(vector<string> v)
{
    for(int i=0; i<v.size()-1; i++)
        cout << v[i] <<" ";
    cout << v[v.size()-1] << endl;
}
