#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "scanner.h"

std::string assemble(std::vector<Token>);

std::string wordify(std::string);
std::string int_to_hex(std::string);

/*
 * C++ Starter code for CS241 A3
 * All code requires C++14, so if you're getting compile errors make sure to
 * use -std=c++14.
 *
 * This file contains the main function of your program. By default, it just
 * prints the scanned list of tokens back to standard output.
 */
int main() {
  std::string line;
  std::vector<std::vector<Token>> tokenFile;
  std::vector<std::string> results;

  try {
    while (getline(std::cin,line)) {
      std::vector<Token> tokenLine = scan(line);
      tokenFile.emplace_back(tokenLine);
    }

    for (auto l : tokenFile) {
      results.emplace_back(assemble(l));
    }

    for (int i =0;i < results.size(); i++) {
      std::cout << results[i];
      if(i < results.size() -1 ) std::cout<< std::endl;
    }
  }

  catch (ScanningFailure &f) {
    std::cout<<"CAUGHT SOMETHING" << std::endl;
    std::cerr << f.what() << std::endl;
    return 1;
  }

  catch (...){
    std::cerr <<"ERROR" << std::endl;
  }

  // You can add your own catch clause(s) for other kinds of errors.
  // Throwing exceptions and catching them is the recommended way to
  // handle errors and terminate the program cleanly in C++. Do not
  // use the std::exit function, which may leak memory.
  
  return 0;
}

std::string assemble (std::vector<Token> t){
  Token temp = t[0];

  if (temp.getKind() == 2)//WORD
  {
    temp = t[1];
    if (temp.getKind() == 7)//HEXINT
      return (wordify(temp.getLexeme()));
    
    else if(temp.getKind() == 6)//INT
      return (wordify(int_to_hex(temp.getLexeme())));
    
    else
      throw(20);
  }

  else{
    throw (20);
  }
  return "";
}


std::string wordify(std::string s){
  std::string temp = s;
  int i = 10 - s.length();

  temp.insert(2, i, '0');
  return temp;
}

std::string int_to_hex(std::string s){
  std::stringstream ss;
  ss << std::hex << std::stoi(s);

  std::string result = ss.str();
  result.insert(0,"0x");

  return result;
}
