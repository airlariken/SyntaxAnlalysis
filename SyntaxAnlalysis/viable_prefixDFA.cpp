//
//  viable_prefixDFA.cpp
//  SyntaxAnlalysis
//
//  Created by 陈梓玮 on 2021/5/4.
//

#include "viable_prefixDFA.hpp"

void ViablePrefixDFA::readGrammarFile()
{
    fstream fin("/Users/chenziwei/C++/SyntaxAnlalysis/SyntaxAnlalysis/grammar.txt");
    if (fin.fail()) {
        cerr<<"cant open grammar.txt"<<endl;
    }
    char src;   string temp;    string des;
    fin>>src>>temp>>des;
    //增广文法
    char add_grammar_src = '@';//增广文法
    unterminal_symbol.insert('@');
    string add_grammar_des = "";//增广文法
    add_grammar_des.push_back(src);//增广文法
    Vini_grammar.push_back(IniGrammar(add_grammar_src, add_grammar_des));//增广文法
    Vini_grammar.push_back(IniGrammar(src, des));
    if (isTerminalSymbol(src)) {
        terminal_symbol.insert(src);
    }
    else{
        unterminal_symbol.insert(src);
    }
    for (int i = 0; i < des.length(); ++i) {
        if (isTerminalSymbol(des[i]))
            terminal_symbol.insert(des[i]);
        else
            unterminal_symbol.insert(des[i]);
    }
    
    //继续读
    while (!fin.eof()) {
        fin>>src>>temp>>des;
//        cout<<src<<temp<<des<<endl;
        Vini_grammar.push_back(IniGrammar(src, des));
        if (isTerminalSymbol(src)) {
            terminal_symbol.insert(src);
        }
        else{
            unterminal_symbol.insert(src);
        }
        for (int i = 0; i < des.length(); ++i) {
            if (isTerminalSymbol(des[i]))
                terminal_symbol.insert(des[i]);
            else
                unterminal_symbol.insert(des[i]);
        }
    }
}

void ViablePrefixDFA::addDot()
{
    for (int i = 0; i < Vini_grammar.size(); ++i) {
        for (int j = 0; j < Vini_grammar[i].des.length()+1; ++j) {
            string temp_fro = Vini_grammar[i].des.substr(0,j);
            string temp_back = Vini_grammar[i].des.substr(j);
            ProcessedGrammar t_gra(Vini_grammar[i].src,temp_fro, temp_back);
            Vpro_grammar.push_back(t_gra);
        }
    }
}
void ViablePrefixDFA::NFAConstructor()
{
    cout<<"NFA矩阵"<<endl;
    NFA_matrix.resize(Vpro_grammar.size(), vector<char>(Vpro_grammar.size(), ' '));//初始化NFA矩阵
    for (int i = 0; i < NFA_matrix.size(); ++i) {
        for (int j = 0; j < Vpro_grammar.size(); ++j) {
            if (Vpro_grammar[j].des_dot_back.length() > 0) {         //如果是未规约态
                char next_char = *Vpro_grammar[i].des_dot_back.begin();
                if (unterminal_symbol.find(next_char) != unterminal_symbol.end()) {//如果是非终态符,要构建空串到达所有src为该非终态符的的状态
                    auto fi = find_if(Vpro_grammar.begin()+i, Vpro_grammar.end(), MatchFirstDotFrontCharPre(next_char));
                    int next_state = (int)(fi-Vpro_grammar.begin());
                    NFA_matrix[i][next_state] = next_char;
                    for (int x = 0; x < Vpro_grammar.size(); ++x) {
                        if (Vpro_grammar[x].src == next_char && Vpro_grammar[x].des_dot_front.length() == 0) {
                            NFA_matrix[i][x] = '*';
                        }
                    }
                }
                else if (terminal_symbol.find(next_char) != terminal_symbol.end()){     //如果是终态
                    char next_char = *Vpro_grammar[i].des_dot_back.begin();
                    string match_dot_front = Vpro_grammar[i].des_dot_front;
                    match_dot_front += next_char;
                    auto fi = find_if(Vpro_grammar.begin(), Vpro_grammar.end(), MatchDotFrontPre(Vpro_grammar[i].src,match_dot_front));
                    if (fi != Vpro_grammar.end()) {
                        int next_state = (int)(fi - Vpro_grammar.begin());
                        NFA_matrix[i][next_state] = next_char;
                    }
                }
            }
            
            
        }
    }
    
    //output
    cout<<'\t';
    for (int i = 0; i < NFA_matrix.size(); ++i)
        cout<<i+1<<'\t';
    
    cout<<endl;
    for (int i = 0; i < NFA_matrix.size(); ++i) {
        cout<<i+1<<'\t';
        for (int j = 0; j < NFA_matrix[i].size(); ++j) {
            cout<<NFA_matrix[i][j]<<'\t';
        }
        cout<<endl;
    }
}
void ViablePrefixDFA::DFAConstructor()
{
    bool *need_clear = new bool[Vpro_grammar.size()];
    for (int i = 0; i < Vpro_grammar.size(); ++i)   need_clear[i] = false;
    
    for (int i = 0; i < NFA_matrix.size(); ++i) {
        for (int j = 0; j < NFA_matrix[i].size(); ++j) {
            if (NFA_matrix[i][j] == '*') {
                NFA_matrix[i][j] = ' ';
                need_clear[j] = true;
                for (int x = 0; x < NFA_matrix[j].size(); ++x) {//将j行的所有信息移到i行
                    if (NFA_matrix[j][x] != ' ') {
                        NFA_matrix[i][x] = NFA_matrix[j][x];
                    }

                }
            }
        }
    }
    
    for (int i = 0; i < Vpro_grammar.size(); ++i) {
        if (need_clear[i] == true) {
            for (int j = 0; j < Vpro_grammar.size(); ++j) {
                NFA_matrix[i][j] = ' ';
            }
        }
    }
    //output
    cout<<"DFA矩阵"<<endl;
    cout<<'\t';
    for (int i = 0; i < NFA_matrix.size(); ++i)
        cout<<i+1<<'\t';
    
    cout<<endl;
    for (int i = 0; i < NFA_matrix.size(); ++i) {
        cout<<i+1<<'\t';
        for (int j = 0; j < NFA_matrix[i].size(); ++j) {
            cout<<NFA_matrix[i][j]<<'\t';
        }
        cout<<endl;
    }
}
bool ViablePrefixDFA::isTerminalSymbol(const char& c)
{
    if (c >= 'a' && c <= 'z') {
        return true;
    }
    return false;
}
bool ViablePrefixDFA::isUnTerminalSymbol(const char& c)
{
    if (c == '@')//增广文法
        return true;
    if (c >= 'A' && c <= 'Z') {
        return true;
    }
    return false;
}
void ViablePrefixDFA::createAnalysisTable()
{
    for (auto it = terminal_symbol.begin(); it != terminal_symbol.end(); ++it) {
        cout<<setw(10)<<*it<<setw(10);
    }
    for (auto it = unterminal_symbol.begin(); it != unterminal_symbol.end(); ++it) {
        cout<<setw(10)<<*it<<setw(10);
    }
    cout<<endl;
    for (int i = 0; i < NFA_matrix.size(); ++i) {
        for (auto it = terminal_symbol.begin(); it != terminal_symbol.end(); ++it) {
            auto fi = find(NFA_matrix[i].begin(), NFA_matrix[i].end(), *it);
            if (fi != NFA_matrix[i].end()) {
                cout<<setw(10)<<'s'<<fi-NFA_matrix[i].begin()+1<<setw(10);
            }
            else
                cout<<setw(10)<<"   "<<setw(10);
        }
        for (auto it = unterminal_symbol.begin(); it != unterminal_symbol.end(); ++it) {
            auto fi = find(NFA_matrix[i].begin(), NFA_matrix[i].end(), *it);
            if (fi != NFA_matrix[i].end()) {
                cout<<setw(10)<<"go"<<fi-NFA_matrix[i].begin()+1<<setw(10);
            }
            else
                cout<<setw(10)<<"   "<<setw(10);
            
        }
        cout<<endl;
    }
}
