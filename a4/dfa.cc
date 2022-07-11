#include <iostream>
#include <vector>
#include <map>
#include <sstream>

using std::vector;
using std::string;
using std::map;

class dfa{
    vector<string> language; //Language
    vector<string> states; //All states
    string start_state; //Initial state
    vector<string> accepting; //Accepting states
    vector<vector<string>> transitions; //Transition rules

    string change_state(string current, string character){
        for(auto rule:transitions){
            if(rule[0] == current && rule[1]== character)
                return rule[2];
        }
        return "";
    }
    bool check_accepting(string s){
        for(auto state: accepting)
            if(state == s) return true;
        return false;
    }

public:

    dfa(){
        int num;
        string s;
        
        std::cin >> num;
        for(int i=0; i< num; i++)
        {
            std::cin>>s;
            this->language.emplace_back(s);
        }

        std::cin >> num;
        for(int i=0; i< num; i++)
        {
            std::cin>>s;
            this->states.emplace_back(s);
        }

        std::cin >> start_state;

        std::cin >> num;
        for(int i=0; i< num; i++)
        {
            std::cin>>s;
            this->accepting.emplace_back(s);
        }

        std::cin >> num;
        for(int i=0; i< num; i++)
        {
            vector<string> v;
            std::cin>>s;
            v.emplace_back(s);
            std::cin>>s;
            v.emplace_back(s);
            std::cin>>s;
            v.emplace_back(s);
            this->transitions.emplace_back(v);
        }
    }
    ~dfa(){
        language.clear();
        states.clear();
        accepting.clear();
        for (auto t : transitions) {t.clear();}
        transitions.clear();
    }
    bool recognize(vector <string> s)
    {
        string current = this->start_state;
        for(auto character:s)
        {
            current = change_state(current,character);
            if (current == "") return false;
        }

        return check_accepting(current);
    }
    friend std::ostream& operator <<(std::ostream&, const dfa&);
};

std::ostream& operator <<(std::ostream& os, const dfa& d)
{
    for(auto n: d.language)
        os << n << "\n";

    
    for(auto n: d.states)
        os << n << "\n";

    for(auto n: d.accepting)
        os << n <<"\n";

    for(auto n: d.transitions)
        os << n[0]<<" "<<n[1]<<" "<<n[2] <<"\n";

    return os;
}

int main()
{
    dfa * machine = new dfa();
    string line;
    int i=0;
    while(getline(std::cin, line))
    {
        if(i==0) 
        {
            i++;
            continue;
        }
        vector<string> v;
        std::stringstream ss(line);
        string tmp;

        while(getline(ss,tmp,' '))
            v.emplace_back(tmp);
        
        if(machine->recognize(v)) std::cout << "true" <<std::endl;
        else std::cout <<"false" <<std::endl;
    }
    delete machine;
}
