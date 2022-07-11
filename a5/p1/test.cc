#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <utility>
#include <set>
#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <cstdint>
#include <ostream>

/*
 * C++ Starter code for CS241 A3
 * All code requires C++14, so if you're getting compile errors make sure to
 * use -std=c++14.
 *
 * This file contains helpers for asm.cc and you don't need to modify it.
 * Read the scanner.h file for a description of the helper functions.
 *
 * This code may be helpful to understand when you write a scanner in a 
 * later assignment. However, you do not need to understand how it works 
 * to write the assembler.
 */

class Token {
  public:
    enum Kind {
      ID = 0,
      COMMA,
      LPAREN,
      RPAREN,
      WHITESPACE,
      COMMENT,
      NUM,
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
      SEMI,
      LBRACK,
      RBRACK,
      AMP,
      ZERO,

      WAIN,
      INT,
      IF,
      ELSE,
      WHILE,
      PRINTLN,
      RETURN,
      NULLT,
      NEW,
      DELETE
    };

  private:
    Kind kind;
    std::string lexeme;

  public:
    Token(Kind kind, std::string lexeme);

    Kind getKind() const;
    const std::string &getLexeme() const;

    /* Converts a token to the corresponding number.
     * Only works on tokens of type INT, HEXINT, or REG.
     */
    int64_t toNumber() const;
    /* Further notes about the toNumber function:
     *
     * This function returns a 64-bit integer instead of 32-bit because
     * .word accepts both signed and unsigned 32-bit integers. The union 
     * of the signed and unsigned ranges cannot be represented by a single 
     * 32-bit type. Even when not working with .word, you should store the
     * result of this function in an int64_t variable, or you might get
     * unexpected behaviour due to C++'s automatic type conversions.
     *
     * If the number represented by the token does not fit in the signed 
     * 64-bit range, there are two possibilities:
     * - If the token is a positive decimal integer or a hexadecimal 
     *   integer, then 2^{63}-1 (the maximum int64_t value) is returned.
     * - If the token is a negative decimal integer, then -2^{63} (the
     *   minimum int64_t value) is returned.
     *
     * Hexadecimal integers are always interpreted as unsigned. For 
     * example, the integer 0xffffffffffffffff (the 64-bit two's 
     * complement representation of -1) will actually be interpreted as 
     * 2^{64}-1, which is out of the signed 64-bit range. Thus, by the above 
     * rule, the value 2^{63}-1 is returned. This behaviour may seem unusual,
     * but is actually important for detecting out of range hex integers.
     * For example, consider: beq $0, $0, 0xffffffffffffffff
     * If the hex integer here was interpreted as -1, that would be bad!
     */

};

std::vector<Token> scan(const std::string &input);

std::ostream &operator<<(std::ostream &out, const Token &tok);

Token::Token(Token::Kind kind, std::string lexeme):
  kind(kind), lexeme(std::move(lexeme)) {}

  Token:: Kind Token::getKind() const { return kind; }
const std::string &Token::getLexeme() const { return lexeme; }

std::ostream &operator<<(std::ostream &out, const Token &tok) {
  switch (tok.getKind()) {
    case Token::ID:         out << "ID";         break;
    case Token::LPAREN:     out << "LPAREN";     break;
    case Token::RPAREN:     out << "RPAREN";     break;
    case Token::WHITESPACE: out << "WHITESPACE"; break;
    case Token::COMMENT:    out << "COMMENT";    break;
    case Token::NUM:        out << "NUM";        break;
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
    case Token::ZERO:       out << "NUM";        break;
    case Token::WAIN:       out << "WAIN";       break;
    case Token::INT:        out << "INT";        break;
    case Token::IF:         out << "IF";         break;
    case Token::ELSE:       out << "ELSE";       break;
    case Token::WHILE:      out << "WHILE";      break;
    case Token::PRINTLN:    out << "PRINTLN";    break;
    case Token::RETURN:     out << "RETURN";     break;
    case Token::NULLT:      out << "NULL";      break;
    case Token::NEW:        out << "NEW";        break;
    case Token::DELETE:     out << "DELETE";     break;
  }
  out << " " << tok.getLexeme();

  return out;
}

int64_t Token::toNumber() const {
  std::istringstream iss;
  int64_t result;

  if (kind == NUM) {
    iss.str(lexeme);
  } else {
    // This should never happen if the user calls this function correctly
    return 0;
  }

  iss >> result;
  return result;
}

class ScanningFailure {
    std::string message;

  public:
    ScanningFailure(std::string message);

    // Returns the message associated with the exception.
    const std::string &what() const;
};
ScanningFailure::ScanningFailure(std::string message):
  message(std::move(message)) {}

const std::string &ScanningFailure::what() const { return message; }

/* Representation of a DFA, used to handle the scanning process.
 */
class AsmDFA {
  public:
    enum State {
      // States that are also kinds
      ID,
      NUM,
      LPAREN,
      RPAREN,
      LBRACE,
      RBRACE,
      BECOMES,
      EQ,
      EXCL,
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
      // return, if, else, while, println, wain, int, new, delete, NULL
      LBRACK,
      RBRACK,
      AMP,
      ZERO,
 
      WHITESPACE,
      COMMENT,

      // States that are not also kinds
      FAIL,
      START,

      // Hack to let this be used easily in arrays. This should always be the
      // final element in the enum, and should always point to the previous
      // element.

      LARGEST_STATE = START
    };

  private:
    /* A set of all accepting states for the DFA.
     * Currently non-accepting states are not actually present anywhere
     * in memory, but a list can be found in the constructor.
     */
    std::set<State> acceptingStates;

    /*
     * The transition function for the DFA, stored as a map.
     */

    std::array<std::array<State, 128>, LARGEST_STATE + 1> transitionFunction;

    /*
     * Converts a state to a kind to allow construction of Tokens from States.
     * Throws an exception if conversion is not possible.
     */
    Token::Kind stateToKind(State s) const {
      switch(s) {
        case ID:         return Token::ID;
        case NUM:        return Token::NUM;
        case LPAREN:     return Token::LPAREN;
        case RPAREN:     return Token::RPAREN;
        case LBRACE:     return Token::LBRACE;
        case RBRACE:     return Token::RBRACE;
        // Return, if, else, while, println, wain,  will be checked on id

        case BECOMES:      return Token::BECOMES;
        // String int check on id
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
        case WHITESPACE: return Token::WHITESPACE;
        case COMMENT:    return Token::COMMENT;
        default: throw ScanningFailure("ERROR: Cannot convert state to kind.");
      }
    }


  public:
    /* Tokenizes an input string according to the Simplified Maximal Munch
     * scanning algorithm.
     */
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

    /* Initializes the accepting states for the DFA.
     */
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
       //Non-accepting states are DOT, MINUS, ZEROX, DOLLARS, START

      // Initialize transitions for the DFA
      for (size_t i = 0; i < transitionFunction.size(); ++i) {
        for (size_t j = 0; j < transitionFunction[0].size(); ++j) {
          transitionFunction[i][j] = FAIL;
        }
      }

      registerTransition(START, isalpha, ID);
      registerTransition(START, "0", ZERO);
      registerTransition(START, "123456789", NUM);
      registerTransition(NUM, isdigit, NUM);
      registerTransition(START, "-", MINUS);
      registerTransition(START, isspace, WHITESPACE);
      registerTransition(START, ",", COMMA);
      registerTransition(START, "(", LPAREN);
      registerTransition(START, ")", RPAREN);
      registerTransition(START, "{", LBRACE);
      registerTransition(START, "}", RBRACE);
      registerTransition(START, "=", BECOMES);
      registerTransition(BECOMES, "=", EQ);
      registerTransition(START, "!", EXCL);
      registerTransition(EXCL, "=", NE);
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
      registerTransition(COMMENT, [](int c) -> int { return c != '\n'; },
          COMMENT);
      registerTransition(WHITESPACE, isspace, WHITESPACE);
    }

    // Register a transition on all chars in chars
    void registerTransition(State oldState, const std::string &chars,
        State newState) {
      for (char c : chars) {
        transitionFunction[oldState][c] = newState;
      }
    }

    // Register a transition on all chars matching test
    // For some reason the cctype functions all use ints, hence the function
    // argument type.
    void registerTransition(State oldState, int (*test)(int), State newState) {

      for (int c = 0; c < 128; ++c) {
        if (test(c)) {
          transitionFunction[oldState][c] = newState;
        }
      }
    }

    /* Returns the state corresponding to following a transition
     * from the given starting state on the given character,
     * or a special fail state if the transition does not exist.
     */
    State transition(State state, char nextChar) const {
      return transitionFunction[state][nextChar];
    }

    /* Checks whether the state returned by transition
     * corresponds to failure to transition.
     */
    bool failed(State state) const { return state == FAIL; }

    /* Checks whether the state returned by transition
     * is an accepting state.
     */
    bool accept(State state) const {
      return acceptingStates.count(state) > 0;
    }

    /* Returns the starting state of the DFA
     */
    State start() const { return START; }
};

std::vector<Token> scan(const std::string &input) {
  static AsmDFA theDFA;

  std::vector<Token> tokens = theDFA.simplifiedMaximalMunch(input);

  // We need to:
  // * Throw exceptions for WORD tokens whose lexemes aren't ".word".
  // * Remove WHITESPACE and COMMENT tokens entirely.

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
        newTokens.push_back(Token(Token::NULLT, "NULL"));
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

    } else if (token.getKind() != Token::WHITESPACE
        && token.getKind() != Token::Kind::COMMENT) {
      newTokens.push_back(token);
    }
  }

  return newTokens;
}


int main() {
  std::string line;
  while (std::getline(std::cin, line)) {
    // Tokenize

    std::vector<Token> tokens;
    try {
      tokens = scan(line);
    } catch (ScanningFailure &f) {
      std::cerr << f.what() << std::endl;
    }

    for (int i = 0; i < tokens.size(); i++) {
      std::cout << tokens[i] << std::endl;
    }
  }

}

