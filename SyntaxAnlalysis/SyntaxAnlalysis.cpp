#include<iostream>
using namespace std;
#include<vector>
#include<string>
#include<fstream>
/*
S->E
E->aA
E->bB
A->d
A->cA
B->d
B->cB
*/
/*
S->E
E->aAcBe
A->b
A->Ab
B->d
*/
#define ACC 666 //接受状态

vector<char>End;
vector<char>unEnd;
vector<string> initial_syntax;          //原始文法用作预测分析表的规约
bool ifend(char c) {
    for (int i = 0; i < unEnd.size(); i++) {
        if (c == unEnd[i]) {
            return false;
        }
    }
    return true;
}
struct Node {
	bool flag = false;
	int id;
	char start;
	vector<char>s;
	int getnum();
	int dnum = 0;
};
int Node::getnum() {
	for (int i = 0; i < s.size(); i++) {
		if (s[i] == '$') { return i; }
	}
	return -1;
}
//项目规范族构造
struct Term{
public:
	vector<Node*>T;
	void initial();
	void DFS(Node*p,int&id);
	void out();
};

void Term::initial() {
	int id = 0;
    ifstream fin("/Users/chenziwei/C++/SyntaxAnlalysis/SyntaxAnlalysis/grammar.txt");
    if(fin.fail()){
        cerr<<"cant open grammar.txt"<<endl;
        exit(-1);
    }
//	for (int i = 0; i < n; i++) {
    while (!fin.eof()) {//读到文件结尾eof
		vector<char>val;
		string s;
		fin >> s;
        initial_syntax.push_back(s);//原始文法做好保存
        for (int j = 0; j < s.length(); j++) {
            val.push_back(s[j]);
            if (j == 1 || j == 2 | j == '#') {//遇到->或者#也即空串跳过
                continue;
            }
            if(s[j]<='Z'&&s[j]>='A')
                unEnd.push_back(s[j]);//插入非终态符
            else{
                End.push_back(s[j]);//插入终态符
            }
        }
        Node*p = new Node;
        p->id = id++;
        p->start = val[0];
        p->s.push_back('$');
        for (int j = 3; j < val.size(); j++) {
            if(val[3]!='#'){
                p->s.push_back(val[j]);
            }
        }
        T.push_back(p);
        DFS(p,id);
    }
    
    //删除重复的元素
    sort(unEnd.begin(), unEnd.end());
    auto ite1 = unique(unEnd.begin(), unEnd.end());
    unEnd.erase(ite1, unEnd.end());
    sort(End.begin(), End.end());
    auto ite2 = unique(End.begin(), End.end());
    End.erase(ite2, End.end());
}


void Term::DFS(Node*p,int&id) {
	for (int i = 0; i < p->s.size() - 1; i++) {
		Node*q= new Node;
		q->id = id++;
		q->start = p->start;
		q->s = p->s;
		//交换
		char t = q->s[i];
		q->s[i] = q->s[i+1];
		q->s[i + 1] = t;
		T.push_back(q);
		p = q;
	}
}
void Term::out(){
	for (int i = 0; i < T.size(); i++) {
		cout << T[i]->id << " " << T[i]->start << "->";
		for (int j = 0; j < T[i]->s.size(); j++) {
			cout << T[i]->s[j];
		}
		cout << endl;
	}
}

class DFA {
public:
	int id;
	vector<Node*>s;
	vector<char>ch;//
	vector<DFA*>next;
	vector<char>Ch;
	void outdfa();
	int ifinch(char x);
	bool ifind(Node*p);
};
bool DFA:: ifind(Node*p) {
	for (int i = 0; i < s.size(); i++) {
		if (s[i]->id == p->id) {
			return true;
		}
	}
	return false;
}
int DFA::ifinch(char x) {
	for (int i = 0; i < Ch.size(); i++) {
		if (Ch[i] == x) {
			return i;
		}
	}
	return -1;
}
void DFA::outdfa() {
	cout <<"DFAid:"<< id << endl;
	for (int i = 0; i < s.size(); i++) {
		cout << s[i]->start << "->";
		for (int j = 0; j < s[i]->s.size(); j++) {
			cout << s[i]->s[j];
		}
		cout << endl;
	}
	cout << endl;
}
class DFAconstruct {
public:	
	int idn = 0;
	Term Te;
	vector<DFA*>ALLdfa;
	vector<DFA*>Q;
	void construct();
	void closure(DFA*d);
	void bfsco(DFA*d);
	Node* go(Node*cur,char x);
	void out();
};
void DFAconstruct::out() {
	cout << "----------------------------------------------------------------" << endl;
	for (int i = 0; i < ALLdfa.size(); i++) {
		ALLdfa[i]->outdfa();
	}
	for (int i = 0; i < ALLdfa.size(); i++) {
        if (ALLdfa[i]->Ch.size() == 0)
            continue;
        
		cout<<"srcdfa:" << ALLdfa[i]->id << endl;
		for (int j = 0; j < ALLdfa[i]->Ch.size(); j++) {
			cout << "char:" << ALLdfa[i]->Ch[j] << endl;
			cout << "nextdfa:" << ALLdfa[i]->next[j]->id << endl;
			cout << endl;
		}
	}
}
void DFAconstruct::construct() {
	idn = 0;

	//初状态0
	Node*p = Te.T[0];
	DFA*d = new DFA;
	d->id = idn++;
	p->flag = true;
	p->dnum = d->id;
	d->s.push_back(p);
	closure(d);	
	ALLdfa.push_back(d);
	Q.push_back(d);
	while (!Q.empty())
	{
		DFA*curr = Q.front();
		Q.erase(Q.begin());
		bfsco(curr);
	}
}


void DFAconstruct::bfsco(DFA*d) {
	for (int i = 0; i < d->ch.size(); i++) {
		char x = d->ch[i];
		Node*cur = go(d->s[i], x);//移位
		if(cur->flag==true){//已经存在DFA中
			//找到DFA
			d->Ch.push_back(x);
			DFA*d1 = ALLdfa[cur->dnum];
			d->next.push_back(d1);
		}
		else if (d->ifinch(x)!=-1) {//ch[]中存在char x
			DFA*d3=d->next[d->ifinch(x)];
			d3->s.push_back(cur);
			closure(d3);
		}
		else
		{
			//新建DFA 获取closure
			d->Ch.push_back(x);
			DFA*d2 = new DFA;
			d2->id = idn++;
			cur->dnum = d2->id;
			cur->flag = true;
			d2->s.push_back(cur);
			closure(d2);
			ALLdfa.push_back(d2);
			Q.push_back(d2);
			d->next.push_back(d2);
		}
	}
	
}

Node* DFAconstruct::go(Node*c, char x) {
	return Te.T[c->id + 1];
}

void DFAconstruct::closure(DFA*d) {
	vector<Node*>l;
	l = d->s;
	while (!l.empty())
	{
		Node*p = l.front();
		l.erase(l.begin());
		if (p->getnum() == p->s.size() - 1) {//$在末尾
			continue;
		}
		else if (ifend(p->s[p->getnum() + 1])) {//不在末尾 $后面是终结符
			continue;
		}
		else {//不在末尾 $后面是非终结符
			char x = p->s[p->getnum() + 1];
			for (int i = 0; i < Te.T.size(); i++) {
				if (Te.T[i]->start == x && Te.T[i]->s[0] == '$'&&!d->ifind(Te.T[i])) {//不在d中
					Te.T[i]->dnum = d->id;
					Te.T[i]->flag = true;
					d->s.push_back(Te.T[i]);				
					l.push_back(Te.T[i]);
				}
			}
		}

	}
	//更新ch数组
	d->ch.clear();
	for (int i = 0; i < d->s.size(); i++) {
		Node*cu = d->s[i];
		if (cu->getnum() != cu->s.size() - 1) {//不在末尾
			d->ch.push_back(cu->s[cu->getnum()+1]);
		}
	}
}


class PridictionAnalysisTable
{
private:
    vector<char>terminal_symbol;                //终结符
    vector<char>unterminal_symbol;              //非终结符
    vector<vector<int>> action_table;           //负数是规约到该数字为顺序的文法，正数是移进操作
    vector<vector<int>> goto_table;             //goto表
    vector<DFA*> DFA_set;                       //存放所有的DFA状态信息
    
public:
    //constructor 传入上一个部分给出的DFA状态集，终结符集，非终结符集并初始化两个table
    PridictionAnalysisTable(vector<DFA*> D, vector<char> end, vector<char> unend);
    
    //主要函数
    void fillReduceState(); //填上需要规约的状态
    void fillACC_Pos();    //填上ACC状态
    void outputActionTable();
    void outputGotoTable();
    void creatActionTable();
    
    //移进规约接口
    void Interface(vector<vector<int>>& __action_table, vector<vector<int>>& __goto_table, vector<char> &__terminal_symbol, vector<char> &__unterminal_symbol);//传入四个vector后该函数返回信息
};
//constructor 传入上一个部分给出的DFA状态集，终结符集，非终结符集并初始化两个table
PridictionAnalysisTable::PridictionAnalysisTable(vector<DFA*> D, vector<char> end, vector<char> unend):DFA_set(D), terminal_symbol(end), unterminal_symbol(unend){
    action_table.resize(DFA_set.size(),vector<int>(End.size()+1,0));//ini多一个acc位
    goto_table.resize(DFA_set.size(),vector<int>(unEnd.size(),0));//ini
}

//填上需要规约的状态
void PridictionAnalysisTable::fillReduceState()
{
    for (int i = 0; i < action_table.size(); ++i) {
        if (DFA_set[i]->Ch.size() == 0) {
            //回去找原始的文法位置
            string ini_syn;
            ini_syn += DFA_set[i]->s[0]->start;
            ini_syn += "->";
            for (int x = 0; x < DFA_set[i]->s[0]->s.size()-1 ; ++x) {//将项目读出
                ini_syn += DFA_set[i]->s[0]->s[x];
            }
//                cout<<"规约文法是"<<ini_syn<<endl;
            auto fi1 = find(initial_syntax.begin(), initial_syntax.end(), ini_syn);//找到原始文法的下标按下标规约
            for (int j = 0; j < terminal_symbol.size()+1; ++j)
                action_table[i][j] -= fi1-initial_syntax.begin();//负数代表规约
        }
    }
}
//填上ACC状态
void PridictionAnalysisTable::fillACC_Pos()
{
    for (int i = 0; i < terminal_symbol.size(); ++i) {
        action_table[1][i] = 0;
    }
    action_table[1][terminal_symbol.size()] = ACC;//状态1的最后一个位置一定是acc
}
void PridictionAnalysisTable::creatActionTable()
{
    fillReduceState();//填好规约状态
    for (int i = 0; i < action_table.size(); ++i) {
        for (int j = 0; j < DFA_set[i]->Ch.size(); ++j) {
            auto fi = find(terminal_symbol.begin(), terminal_symbol.end(),DFA_set[i]->Ch[j]);
            if (fi != terminal_symbol.end()) {                                  //这是个终结符
                int next_DFA_state = (int)(fi - terminal_symbol.begin());
                action_table[i][next_DFA_state] = DFA_set[i]->next[j]->id;
            }
            else{                                                                //非终结符
                auto fi = find(unterminal_symbol.begin(), unterminal_symbol.end(),DFA_set[i]->Ch[j]);
                int next_DFA_state = (int)(fi - unterminal_symbol.begin());
                goto_table[i][next_DFA_state] = DFA_set[i]->next[j]->id;
            }
        }
    }
    fillACC_Pos();//填好acc状态
    //输出结果
    outputActionTable();
    outputGotoTable();
}
void PridictionAnalysisTable::outputActionTable()
{
    cout<<"Action Table:"<<endl;
    cout<<' '<<'\t';
    for (int i = 0; i < terminal_symbol.size(); ++i) {
        cout<<terminal_symbol[i]<<'\t';
    }
    cout<<'#';
    cout<<endl;
    for (int i = 0; i < action_table.size(); ++i) {
        cout<<i<<'\t';
        for (int j = 0; j < action_table[i].size(); ++j) {
            cout<<action_table[i][j]<<'\t';
        }
        cout<<endl;
    }
}
void PridictionAnalysisTable::outputGotoTable()
{
    cout<<"Goto Table:"<<endl;
    cout<<' '<<'\t';
    for (int i = 0; i < unterminal_symbol.size(); ++i) {
        cout<<unterminal_symbol[i]<<'\t';
    }
    cout<<endl;
    for (int i = 0; i < goto_table.size(); ++i) {
        cout<<i<<'\t';
        for (int j = 0; j < goto_table[i].size(); ++j) {
            cout<<goto_table[i][j]<<'\t';
        }
        cout<<endl;
    }
}
void PridictionAnalysisTable::Interface(vector<vector<int>>& __action_table, vector<vector<int>>& __goto_table, vector<char> &__terminal_symbol, vector<char> &__unterminal_symbol)//传入四个vector后该函数返回四个带数据的参数
{
    __action_table.clear();
    __goto_table.clear();
    __terminal_symbol.clear();
    __unterminal_symbol.clear();
    __action_table.resize(action_table.size(), vector<int>(action_table[0].size(), 0));
    __goto_table.resize(goto_table.size(), vector<int>(goto_table[0].size(), 0));
    for (int i = 0; i < action_table.size(); ++i) {                                 //深拷贝
        for (int j = 0; j < action_table[i].size(); ++j) {
            __action_table[i][j] = action_table[i][j];
        }
    }
    for (int i = 0; i < goto_table.size(); ++i) {                                    //深拷贝
        for (int j = 0; j < goto_table[i].size(); ++j) {
            __goto_table[i][j] = goto_table[i][j];
        }
    }
    __terminal_symbol = terminal_symbol;
    __unterminal_symbol = unterminal_symbol;
    return;
}

int main() {
	DFAconstruct D;
	D.Te.initial();
	D.Te.out();
	D.construct();
	D.out();
    PridictionAnalysisTable my_table(D.ALLdfa, End, unEnd);
    my_table.creatActionTable();
}
