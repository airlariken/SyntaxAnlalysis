//
//  viable_prefixDFA.hpp
//  SyntaxAnlalysis
//
//  Created by 陈梓玮 on 2021/5/4.
//

#ifndef viable_prefixDFA_hpp
#define viable_prefixDFA_hpp
#include "Header.h"

struct IniGrammar{
    char src = ' ';
    string des;
    IniGrammar(char s, string d):src(s), des(d){}
};
struct ProcessedGrammar{
    char src = ' ';
    string des_dot_front;//在.前面的
    string des_dot_back;//在.后面的
    ProcessedGrammar(char _src, string fro, string bck):src(_src), des_dot_front(fro), des_dot_back(bck){}
};

class ViablePrefixDFA{
private:
    vector<IniGrammar> Vini_grammar;
    vector<ProcessedGrammar> Vpro_grammar;
    vector<vector<char>> NFA_matrix;
    set<char>terminal_symbol;//终态符
    set<char>unterminal_symbol;//非终态符
public:
    
    void readGrammarFile();
    bool isTerminalSymbol(const char& c);
    bool isUnTerminalSymbol(const char& c);
    void addDot();
    //由于文法的nfa构造不是递归建立的，所以不能用以前stack的方式建nfa，要用邻接矩阵存
    void NFAConstructor();
    void DFAConstructor();
    
    
    
    void createAnalysisTable();
};

struct MatchFirstDotFrontCharPre
{
    char match_src;
    MatchFirstDotFrontCharPre(const char& s):match_src(s){}
    bool operator()(const ProcessedGrammar& g)
    {
        if (*(g.des_dot_front
            .end()-1) == match_src) {
            return true;
        }
        return false;
    }
};
struct MatchDotFrontPre
{
    string match_dot_front;
    char src;
    MatchDotFrontPre(const char& _src,string s):src(_src),match_dot_front(s){}
    bool operator()(const ProcessedGrammar& g)
    {
        if (g.des_dot_front == match_dot_front && src == g.src) {
            return true;
        }
        return false;
    }
};
#endif /* viable_prefixDFA_hpp */
