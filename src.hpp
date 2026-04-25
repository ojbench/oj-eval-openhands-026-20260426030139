#pragma once
#include <bits/stdc++.h>
using namespace std;

// Prework: arithmetic calculator using visitor and std::any
// Provide a visitor base, an evaluator "calculator", an AST and a parser.

struct number;
struct unary;
struct binary;

struct visitor {
    virtual std::any visit(number*) = 0;
    virtual std::any visit(unary*) = 0;
    virtual std::any visit(binary*) = 0;
    virtual ~visitor() = default;
};

struct expr {
    virtual std::any accept(visitor* v) = 0;
    virtual ~expr() = default;
};

struct number : expr {
    long long value;
    explicit number(long long v) : value(v) {}
    std::any accept(visitor* v) override { return v->visit(this); }
};

struct unary : expr {
    char op;
    std::unique_ptr<expr> right;
    unary(char o, std::unique_ptr<expr> r) : op(o), right(std::move(r)) {}
    std::any accept(visitor* v) override { return v->visit(this); }
};

struct binary : expr {
    std::unique_ptr<expr> left;
    char op;
    std::unique_ptr<expr> right;
    binary(std::unique_ptr<expr> l, char o, std::unique_ptr<expr> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
    std::any accept(visitor* v) override { return v->visit(this); }
};

struct calculator : visitor {
    std::any visit(number* n) override { return std::any(n->value); }
    std::any visit(unary* u) override {
        long long rv = std::any_cast<long long>(u->right->accept(this));
        if (u->op == '+') return std::any(+rv);
        if (u->op == '-') return std::any(-rv);
        return std::any(0LL);
    }
    std::any visit(binary* b) override {
        long long lv = std::any_cast<long long>(b->left->accept(this));
        long long rv = std::any_cast<long long>(b->right->accept(this));
        switch (b->op) {
            case '+': return std::any(lv + rv);
            case '-': return std::any(lv - rv);
            case '*': return std::any(lv * rv);
            case '/': {
                if (rv == 0) return std::any(0LL);
                return std::any(lv / rv);
            }
        }
        return std::any(0LL);
    }
};

// Simple lexer/parser for expressions
struct Lexer {
    string s; size_t i{0};
    explicit Lexer(string str): s(move(str)) {}
    void skip() { while (i < s.size() && isspace((unsigned char)s[i])) ++i; }
    bool match(char c) { skip(); if (i < s.size() && s[i]==c) { ++i; return true; } return false; }
    bool eof() { skip(); return i>=s.size(); }
    bool peek(char &c) { skip(); if (i < s.size()) { c = s[i]; return true; } return false; }
};

struct Parser {
    Lexer lx;
    explicit Parser(string s): lx(move(s)) {}

    unique_ptr<expr> parse() {
        auto e = expr_();
        char c; if (lx.peek(c)) throw runtime_error("extra");
        return e;
    }
    unique_ptr<expr> expr_() {
        auto left = term_();
        while (true) {
            if (lx.match('+')) { auto right = term_(); left = make_unique<binary>(move(left), '+', move(right)); }
            else if (lx.match('-')) { auto right = term_(); left = make_unique<binary>(move(left), '-', move(right)); }
            else break;
        }
        return left;
    }
    unique_ptr<expr> term_() {
        auto left = factor_();
        while (true) {
            if (lx.match('*')) { auto right = factor_(); left = make_unique<binary>(move(left), '*', move(right)); }
            else if (lx.match('/')) { auto right = factor_(); left = make_unique<binary>(move(left), '/', move(right)); }
            else break;
        }
        return left;
    }
    unique_ptr<expr> factor_() {
        if (lx.match('(')) {
            auto e = expr_();
            if (!lx.match(')')) throw runtime_error(") expected");
            return e;
        }
        if (lx.match('+')) return make_unique<unary>('+', factor_());
        if (lx.match('-')) return make_unique<unary>('-', factor_());
        // number
        long long val = 0; bool any=false; lx.skip();
        while (!lx.eof()) {
            char c = lx.s[lx.i];
            if (isdigit((unsigned char)c)) { any=true; int d=c-'0';
                if (val > (LLONG_MAX - d)/10) val = LLONG_MAX/2; else val = val*10 + d; ++lx.i; }
            else break;
        }
        if (!any) throw runtime_error("number expected");
        return make_unique<number>(val);
    }
};

inline long long evaluate_expression(const string &line) {
    Parser p(line);
    auto tree = p.parse();
    calculator calc;
    return any_cast<long long>(tree->accept(&calc));
}
