#include <cctype>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

enum class TokenType { NUMBER, PLUS, MUL, LPAREN, RPAREN, END };

struct Token {
  TokenType type;
  double value;

  Token(TokenType type, double value = 0) : type(type), value(value) {}
};

class Lexer {
public:
  Lexer(const std::string &input) : input(input), pos(0) {}

  Token getNextToken() {
    while (pos < input.size() && std::isspace(input[pos])) {
      ++pos;
    }

    if (pos >= input.size()) {
      return Token(TokenType::END);
    }

    char current_char = input[pos];

    if (std::isdigit(current_char)) {
      return number();
    } else if (current_char == '+') {
      ++pos;
      return Token(TokenType::PLUS);
    } else if (current_char == '*') {
      ++pos;
      return Token(TokenType::MUL);
    } else if (current_char == '(') {
      ++pos;
      return Token(TokenType::LPAREN);
    } else if (current_char == ')') {
      ++pos;
      return Token(TokenType::RPAREN);
    }

    throw std::runtime_error("Invalid character in input");
  }

private:
  std::string input;
  size_t pos;

  Token number() {
    size_t start_pos = pos;
    while (pos < input.size() &&
           (std::isdigit(input[pos]) || input[pos] == '.')) {
      ++pos;
    }
    double value = std::stod(input.substr(start_pos, pos - start_pos));
    return Token(TokenType::NUMBER, value);
  }
};

class ASTNode {
public:
  virtual ~ASTNode() = default;

  virtual double evaluate() const = 0;

  virtual void print_infix() const = 0;

  virtual void print_tree(int indent = 0) const = 0;
};

class NumberNode : public ASTNode {
public:
  explicit NumberNode(double value) : value(value) {}

  double evaluate() const override { return value; }

  void print_infix() const override { std::cout << value; }

  void print_tree(int indent = 0) const override {
    print_indent(indent);
    std::cout << "Number(" << value << ")\n";
  }

private:
  double value;

  void print_indent(int indent) const {
    for (int i = 0; i < indent; ++i) {
      std::cout << "  ";
    }
  }
};

class BinaryOpNode : public ASTNode {
public:
  BinaryOpNode(TokenType op, std::shared_ptr<ASTNode> left,
               std::shared_ptr<ASTNode> right)
      : op(op), left(left), right(right) {}

  double evaluate() const override {
    if (op == TokenType::PLUS) {
      return left->evaluate() + right->evaluate();
    } else if (op == TokenType::MUL) {
      return left->evaluate() * right->evaluate();
    }
    throw std::runtime_error("Unknown operator");
  }

  void print_infix() const override {
    std::cout << "(";
    left->print_infix();
    if (op == TokenType::PLUS) {
      std::cout << " + ";
    } else if (op == TokenType::MUL) {
      std::cout << " * ";
    }
    right->print_infix();
    std::cout << ")";
  }

  void print_tree(int indent = 0) const override {
    print_indent(indent);
    std::cout << "BinaryOp(" << (op == TokenType::PLUS ? "+" : "*") << ")\n";
    left->print_tree(indent + 1);
    right->print_tree(indent + 1);
  }

private:
  TokenType op;
  std::shared_ptr<ASTNode> left;
  std::shared_ptr<ASTNode> right;

  void print_indent(int indent) const {
    for (int i = 0; i < indent; ++i) {
      std::cout << "  ";
    }
  }
};

class Parser {
public:
  Parser(Lexer &lexer) : lexer(lexer), current_token(lexer.getNextToken()) {}

  std::shared_ptr<ASTNode> parse() { return expr(); }

private:
  Lexer &lexer;
  Token current_token;

  void consume(TokenType expected) {
    if (current_token.type == expected) {
      current_token = lexer.getNextToken();
    } else {
      throw std::runtime_error("Unexpected token");
    }
  }

  std::shared_ptr<ASTNode> expr() {
    std::shared_ptr<ASTNode> node = term();

    while (current_token.type == TokenType::PLUS) {
      TokenType op = current_token.type;
      consume(TokenType::PLUS);
      node = std::make_shared<BinaryOpNode>(op, node, term());
    }

    return node;
  }

  std::shared_ptr<ASTNode> term() {
    std::shared_ptr<ASTNode> node = factor();

    while (current_token.type == TokenType::MUL) {
      TokenType op = current_token.type;
      consume(TokenType::MUL);
      node = std::make_shared<BinaryOpNode>(op, node, factor());
    }

    return node;
  }

  std::shared_ptr<ASTNode> factor() {
    if (current_token.type == TokenType::NUMBER) {
      double value = current_token.value;
      consume(TokenType::NUMBER);
      return std::make_shared<NumberNode>(value);
    } else if (current_token.type == TokenType::LPAREN) {
      consume(TokenType::LPAREN);
      std::shared_ptr<ASTNode> node = expr();
      consume(TokenType::RPAREN);
      return node;
    }

    throw std::runtime_error("Expected number or parentheses");
  }
};

int main() {
  bool is_running = true;
  while (is_running) {
    std::string input;
    std::cout << ">>> ";
    std::getline(std::cin, input);

    try {
      Lexer lexer(input);
      Parser parser(lexer);
      std::shared_ptr<ASTNode> ast = parser.parse();

      std::cout << "Infix notation: ";
      ast->print_infix();
      std::cout << "\n";

      std::cout << "Tree structure:\n";
      ast->print_tree();

      double result = ast->evaluate();
      std::cout << "Result: " << result << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }

  return 0;
}
