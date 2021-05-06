//
//  main.cpp
//  SyntaxAnlalysis
//
//  Created by 陈梓玮 on 2021/5/4.
//

#include <iostream>
#include "viable_prefixDFA.hpp"
int main(int argc, const char * argv[]) {

    ViablePrefixDFA my_prefixDFA;
    my_prefixDFA.readGrammarFile();
    my_prefixDFA.addDot();
    my_prefixDFA.NFAConstructor();
    my_prefixDFA.DFAConstructor();
    my_prefixDFA.createAnalysisTable();
    return 0;
}
