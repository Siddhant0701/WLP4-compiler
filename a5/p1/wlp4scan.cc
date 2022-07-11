#include <string>
#include <vector>
#include <set>
#include <cstdint>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <utility>
#include <array>
#include <iostream>

class Token {
  public:
    enum Kind {
      ID = 0,
      
      LPAREN,
      RPAREN,
      LBRACE,
      RBRACE,
      LBRACK,
      RBRACK,
      
      WHITESPACE,
      COMMENT,

      NUM,
      ZERO,

      BECOMES,
      EQ,
      NE,
      LT,
      GT,
      LE,
      GE,

      PLUS,
      MINUS,
      STAR,
      SLASH,
      PCT,
      
      SEMI,
      AMP,
      COMMA,

      WAIN,
      INT,
      IF,
      ELSE,
      WHILE,
      PRINTLN,
      RETURN,
      Null,
      NEW,
      DELETE
    };

  private:
    Kind kind;
    std::string lexeme;

  public:
    Token(Kind kind, std::string lexeme): kind(kind), lexeme(std::move(lexeme)) {}

    Kind getKind() const { return kind; }
    const std::string &getLexeme() const { return lexeme; }

    int64_t toNumber() const
    {
        std::istringstream iss;
        int64_t result;

        iss.str(lexeme);
        iss >> result;
        return result;
    }
};


std::ostream &operator<<(std::ostream &out, const Token &tok) {
  switch (tok.getKind()) {
    case Token::ID:         out << "ID";         break;
    case Token::LPAREN:     out << "LPAREN";     break;
    case Token::RPAREN:     out << "RPAREN";     break;
    case Token::WHITESPACE: out << "WHITESPACE"; break;
    case Token::COMMENT:    out << "COMMENT";    break;
    case Token::NUM:        out << "NUM";        break;
    case Token::ZERO:       out << "NUM";        break;
    case Token::LBRACE:     out << "LBRACE";     break;
    case Token::RBRACE:     out << "RBRACE";     break;
    case Token::BECOMES:    out << "BECOMES";    break;
    case Token::EQ:         out << "EQ";         break;
    case Token::NE:         out << "NE";         break;
    case Token::LT:         out << "LT";         break;
    case Token::GT:         out << "GT";         break;
    case Token::LE:         out << "LE";         break;
    case Token::GE:         out << "GE";         break;
    case Token::PLUS:       out << "PLUS";       break;
    case Token::MINUS:      out << "MINUS";      break;
    case Token::STAR:       out << "STAR";       break;
    case Token::SLASH:      out << "SLASH";      break;
    case Token::PCT:        out << "PCT";        break;
    case Token::COMMA:      out << "COMMA";      break;
    case Token::SEMI:       out << "SEMI";       break;
    case Token::LBRACK:     out << "LBRACK";     break;
    case Token::RBRACK:     out << "RBRACK";     break;
    case Token::AMP:        out << "AMP";        break;
    case Token::WAIN:       out << "WAIN";       break;
    case Token::INT:        out << "INT";        break;
    case Token::IF:         out << "IF";         break;
    case Token::ELSE:       out << "ELSE";       break;
    case Token::WHILE:      out << "WHILE";      break;
    case Token::PRINTLN:    out << "PRINTLN";    break;
    case Token::RETURN:     out << "RETURN";     break;
    case Token::Null:       out << "NULL";      break;
    case Token::NEW:        out << "NEW";        break;
    case Token::DELETE:     out << "DELETE";     break;
  }
  out << " " << tok.getLexeme();

  return out;
}

class ScanningFailure {
    std::string message;

  public:
    ScanningFailure(std::string message): message(std::move(message)) {}

    const std::string &what() const { return message; }
};

class AsmDFA {
  public:
    enum State {
      ID,
      NUM,

      LPAREN,
      RPAREN,
      LBRACE,
      RBRACE,
      
      BECOMES,
      EQ,
      TEMP,
      NE,
      LT,
      GT,
      LE,
      GE,

      PLUS,
      MINUS,
      STAR,
      SLASH,
      
      PCT,
      COMMA,
      SEMI,
      LBRACK,
      RBRACK,
      AMP,
      ZERO,
 
      WHITESPACE,
      COMMENT,

      START,
      FAIL,

      LARGEST_STATE = FAIL
    };

    private:

    std::set<State> acceptingStates;
    std::array<std::array<State, 128>, LARGEST_STATE + 1> transitionFunction;

    Token::Kind stateToKind(State s) const {
      switch(s) {
        case ID:         return Token::ID;
        case NUM:        return Token::NUM;
        case LPAREN:     return Token::LPAREN;
        case RPAREN:     return Token::RPAREN;
        case LBRACE:     return Token::LBRACE;
        case RBRACE:     return Token::RBRACE;
        case BECOMES:      return Token::BECOMES;
        case EQ:      return Token::EQ;
        case NE:      return Token::NE;
        case LT:      return Token::LT;
        case GT:      return Token::GT;
        case LE:      return Token::LE;
        case GE:      return Token::GE;
        case PLUS:    return Token::PLUS;
        case MINUS:   return Token::MINUS;
        case STAR:    return Token::STAR;
        case SLASH:   return Token::SLASH;
        case PCT:     return Token::PCT;
        case COMMA:   return Token::COMMA;
        case SEMI:    return Token::SEMI;
        case LBRACK:  return Token::LBRACK;
        case RBRACK:  return Token::RBRACK;
        case AMP:     return Token::AMP;
 
        case ZERO:       return Token::NUM;
        case COMMENT:    return Token::COMMENT;
        case WHITESPACE: return Token::WHITESPACE;
        default: throw ScanningFailure("ERROR: Cannot convert state to kind.");
      }
    }

public:
    std::vector<Token> simplifiedMaximalMunch(const std::string &input) const {
      std::vector<Token> result;

      State state = start();
      std::string munchedInput;

      // We can't use a range-based for loop effectively here
      // since the iterator doesn't always increment.
      for (std::string::const_iterator inputPosn = input.begin();
           inputPosn != input.end();) {

        State oldState = state;
        state = transition(state, *inputPosn);

        if (!failed(state)) {
          munchedInput += *inputPosn;
          oldState = state;

          ++inputPosn;
        }

        if (inputPosn == input.end() || failed(state)) {
          if (accept(oldState)) {
            result.push_back(Token(stateToKind(oldState), munchedInput));

            munchedInput = "";
            state = start();
          } else {
            if (failed(state)) {
              munchedInput += *inputPosn;
            }
            throw ScanningFailure("ERROR: Simplified maximal munch failed on input: "
                                 + munchedInput);
          }
        }
      }

      return result;
    }

    AsmDFA() {
        acceptingStates = {        
            ID,
            NUM,
            LPAREN,
            RPAREN,
            LBRACE,
            RBRACE,
            BECOMES,
            EQ,
            NE,
            LT,
            GT,
            LE,
            GE,
            PLUS,
            MINUS,
            STAR,
            SLASH,
            PCT,
            COMMA,
            SEMI,
            LBRACK,
            RBRACK,
            AMP,
            ZERO,
            WHITESPACE,
            COMMENT
        };
        
        for (size_t i = 0; i < transitionFunction.size(); ++i) {
            for (size_t j = 0; j < transitionFunction[0].size(); ++j) {
                transitionFunction[i][j] = FAIL;
            }
        }

        registerTransition(START, isalpha, ID);
        registerTransition(START, "0", ZERO);
        registerTransition(START, "123456789", NUM);
        registerTransition(NUM, isdigit, NUM);
        
        registerTransition(START, "(", LPAREN);
        registerTransition(START, ")", RPAREN);
        registerTransition(START, "{", LBRACE);
        registerTransition(START, "}", RBRACE);
        registerTransition(START, "=", BECOMES);
        registerTransition(START, "-", MINUS);
        registerTransition(START, isspace, WHITESPACE);
        registerTransition(START, ",", COMMA);

        registerTransition(BECOMES, "=", EQ);
        registerTransition(START, "!", TEMP);
        registerTransition(TEMP, "=", NE);
        registerTransition(START, "<", LT);
        registerTransition(LT, "=", LE);
        registerTransition(START, ">", GT);
        registerTransition(GT, "=", GE);
        
        registerTransition(START, "+", PLUS);
        registerTransition(START, "-", MINUS);
        registerTransition(START, "*", STAR);
        registerTransition(START, "/", SLASH);
        registerTransition(SLASH, "/", COMMENT);
        registerTransition(START, "%", PCT);
        registerTransition(START, ",", COMMA);
        registerTransition(START, ";", SEMI);
        registerTransition(START, "[", LBRACK);
        registerTransition(START, "]", RBRACK);
        registerTransition(START, "&", AMP);
        
        registerTransition(ID, isalnum, ID);
        registerTransition(COMMENT, [](int c) -> int { return c != '\n'; }, COMMENT);
        registerTransition(WHITESPACE, isspace, WHITESPACE);
    };

    void registerTransition(State oldState, const std::string &chars, State newState) {
      for (char c : chars) {
        transitionFunction[oldState][c] = newState;
      }
    }

    void registerTransition(State oldState, int (*test)(int), State newState) {

      for (int c = 0; c < 128; ++c) {
        if (test(c)) {
          transitionFunction[oldState][c] = newState;
        }
      }
    }

    State transition(State state, char nextChar) const {
      return transitionFunction[state][nextChar];
    }
    bool failed(State state) const { return state == FAIL; }
    bool accept(State state) const { return acceptingStates.count(state) > 0; }
    State start() const { return START; }
};

std::vector<Token> scan(const std::string &input) {
  static AsmDFA theDFA;

  std::vector<Token> tokens = theDFA.simplifiedMaximalMunch(input);

  std::vector<Token> newTokens;

  for (auto &token : tokens) {
    if (token.getKind() == Token::ID) {
      std::string text = token.getLexeme();
      if (text == "wain") {
        newTokens.push_back(Token(Token::WAIN, "wain"));
      }
      else if (text == "int") {
        newTokens.push_back(Token(Token::INT, "int"));
      }
      else if (text == "if") {
        newTokens.push_back(Token(Token::IF, "if"));
      }
      else if (text == "else") {
        newTokens.push_back(Token(Token::ELSE, "else"));
      }
      else if (text == "while") {
        newTokens.push_back(Token(Token::WHILE, "while"));
      }
      else if (text == "println") {
        newTokens.push_back(Token(Token::PRINTLN, "println"));
      }
      else if (text == "return") {
        newTokens.push_back(Token(Token::RETURN, "return"));
      }
      else if (text == "NULL") {
        newTokens.push_back(Token(Token::Null, "NULL"));
      }
      else if (text == "new") {
        newTokens.push_back(Token(Token::NEW, "new"));
      }
       else if (text == "delete") {
        newTokens.push_back(Token(Token::DELETE, "delete"));
      }
      else {
        newTokens.push_back(token);
        //throw ScanningFailure("ERROR: DOTID token unrecognized: " +
      }
    } else if (token.getKind() == Token::NUM) {
      int64_t num = token.toNumber();
      if (num <= 2147483647) {
        newTokens.push_back(token);
      } else {
        throw ScanningFailure("ERROR: Numeric literal out of range");
      }

    } else if (token.getKind() != Token::WHITESPACE && token.getKind() != Token::Kind::COMMENT) {
      newTokens.push_back(token);
    }
  }

  return newTokens;
}



int main() {
    std::string line;
    while (getline(std::cin, line)) {
        
        std::vector<Token> tokens;
        try{
            tokens = scan(line);
        } 
        
        catch (ScanningFailure &f) {
            std::cerr << f.what() << std::endl;
        }
        
        for (int i = 0; i < tokens.size(); i++) {
            std::cout << tokens[i] << std::endl;
        }
    }
}
