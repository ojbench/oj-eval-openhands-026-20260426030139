#include <bits/stdc++.h>
using namespace std;

// Simple arithmetic expression evaluator supporting + - * / and parentheses
// Grammar (recursive descent):
// expr   := term ((+|-) term)*
// term   := factor ((*|/) factor)*
// factor := NUMBER | '(' expr ')' | ('+'|'-') factor
// Numbers are 64-bit signed. Division is integer division like C++ (truncate toward zero).

struct Lexer {
    string s; size_t i{0};
    explicit Lexer(string str): s(move(str)) {}
    void skip() { while (i < s.size() && isspace((unsigned char)s[i])) ++i; }
    bool match(char c) { skip(); if (i < s.size() && s[i] == c) { ++i; return true; } return false; }
    bool eof() { skip(); return i >= s.size(); }
    bool peek(char &c) { skip(); if (i < s.size()) { c = s[i]; return true; } return false; }
};

struct Parser {
    Lexer lx;
    explicit Parser(string s): lx(move(s)) {}

    long long parse() {
        long long v = expr();
        char c; if (lx.peek(c)) throw runtime_error("extra characters");
        return v;
    }

    long long expr() {
        long long v = term();
        while (true) {
            if (lx.match('+')) { long long r = term(); v = v + r; }
            else if (lx.match('-')) { long long r = term(); v = v - r; }
            else break;
        }
        return v;
    }
    long long term() {
        long long v = factor();
        while (true) {
            if (lx.match('*')) { long long r = factor(); v = v * r; }
            else if (lx.match('/')) { long long r = factor(); if (r==0) throw runtime_error("div0"); v = v / r; }
            else break;
        }
        return v;
    }
    long long factor() {
        if (lx.match('(')) { long long v = expr(); if (!lx.match(')')) throw runtime_error(") expected"); return v; }
        // unary + or -
        if (lx.match('+')) return factor();
        if (lx.match('-')) return -factor();
        // number
        return read_number();
    }
    long long read_number() {
        lx.skip(); long long val=0; bool any=false;
        while (!lx.eof()) {
            char c = lx.s[lx.i];
            if (isdigit((unsigned char)c)) {
                any=true; int d=c-'0';
                if (val > (LLONG_MAX - d)/10) val = LLONG_MAX/2; else val = val*10 + d;
                ++lx.i;
            } else break;
        }
        if (!any) throw runtime_error("number expected");
        return val;
    }
};

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    vector<long long> results;
    string line;
    while (getline(cin, line)) {
        bool allspace=true; for(char ch: line) if(!isspace((unsigned char)ch)) { allspace=false; break; }
        if (allspace) continue;
        try {
            Parser p(line);
            long long ans = p.parse();
            results.push_back(ans);
        } catch (...) {
            results.push_back(0);
        }
    }
    for (size_t i=0;i<results.size();++i){
        if (i) cout << '\n';
        cout << results[i];
    }
    return 0;
}
