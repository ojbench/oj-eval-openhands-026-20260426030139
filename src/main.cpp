#include "src.hpp"

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    vector<long long> results;
    string line;
    while (getline(cin, line)) {
        bool allspace=true; for(char ch: line) if(!isspace((unsigned char)ch)) { allspace=false; break; }
        if (allspace) continue;
        try {
            long long ans = evaluate_expression(line);
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
