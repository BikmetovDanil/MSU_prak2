#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "parser.h"

using namespace std;
extern vector<Ident> TID;
using std::string;
using std::vector;
using std::ostream;

int transform(int, int);
bool write_poliz;

class Executer{
	Lex pc_el;
	int index, size;
	bool is_str;
	int get_expr(Poliz&);
public:
	void execute(Poliz& prog);
};

void Executer::execute(Poliz& prog){
	Stack <int, 1000> args;
	is_str = false;
	string c_str;
	int i, j, k;
	index = 0;
	size = prog.get_free();
	while(index < size){
		pc_el = prog[index];
		switch(pc_el.get_type()){
			case POLIZ_ADDRESS:
				i = pc_el.get_value();
				if(TID[i].get_type() == LEX_ARR){
					j = get_expr(prog);
					/*if(j > 0){
						pc_el = prog[++index];
						k = TID[i][j - 1];
						TID[k].put_type(pc_el.get_type());
						args.push(k);
						cout << k << endl;
					}else args.push(i);*/
					pc_el = prog[++index];
					k = TID[i][j];
					TID[k].put_type(pc_el.get_type());
					args.push(k);
				}else args.push(i);
				break;
			case LEX_NUM:
				args.push(pc_el.get_value());
				break;
			case POLIZ_LABEL:
				args.push(pc_el.get_value());
				break;
			case LEX_IDENT:
				i = pc_el.get_value();
				args.push(TID[i].get_value());
				break;
			case LEX_STR:
				is_str = true;
				c_str = pc_el.get_name();
				break;
			case LEX_NOT:
				args.push(!args.pop());
				break;
			case LEX_OR:
				i = args.pop();
				args.push(args.pop() || i);
				break;
			case LEX_AND:
				i = args.pop();
				args.push(args.pop() && i);
				break;
			case POLIZ_GO:
				index = args.pop() - 1;
				break;
			case POLIZ_FGO:
				i = args.pop();
				if(!args.pop()) index = i - 1;
				break;
			case POLIZ_CHANGE:
				pc_el = prog[++index];
				i = pc_el.get_value();
				TID[pc_el.get_value()].put_type(pc_el.get_type());
				break;
			case KEY_WRITE:
				pc_el = prog[++index];
				if(pc_el.get_type() == LEX_STR) cout << pc_el.get_name();
				else if(pc_el.get_type() == LEX_IDENT){
					i = pc_el.get_value();
					if(TID[i].get_type() == LEX_ARR){
						j = get_expr(prog);
						k = TID[i][j];
						switch(TID[k].get_type()){
							case LEX_NUM:
								cout << TID[k].get_value();
								break;
							case LEX_STR:
								cout << TID[k].get_name();
								break;
							case LEX_BOOL:
								cout << TID[k].get_value();
								break;
							case LEX_NULL:
								cout << "null";
								break;
							case LEX_UNDEF:
								cout << "undefined";
								break;
							default: throw("Can't write this");
						}
					}else{
						switch(TID[i].get_type()){
							case LEX_NUM:
								cout << TID[i].get_value();
								break;
							case LEX_STR:
								cout << TID[i].get_name();
								break;
							case LEX_BOOL:
								cout << TID[i].get_value();
								break;
							case LEX_NULL:
								cout << "null";
								break;
							case LEX_UNDEF:
								cout << "undefined";
								break;
							default: throw("Can't write this");
						}
					}
				}else if(pc_el.get_type() == LEX_NUM) cout << pc_el.get_value();
				else if(pc_el.get_type() == LEX_BOOL) cout << pc_el.get_value();
				else throw("Can't write this");
				break;
			case KEY_READ:
				//READ
				break;
			case LEX_PLUS:
				i = args.pop();
				j = args.pop();
				args.push(args.pop() + args.pop());
				break;
			case LEX_MUL:
				args.push(args.pop() * args.pop());
				break;
			case LEX_MINUS:
				i = args.pop();
				args.push(args.pop() - i);
				break;
			case LEX_DIV:
				i = args.pop();
				if(i){
					args.push(args.pop() / i);
					break;
				}else throw "POLIZ: divide by zero";
			case LEX_MOD:
				i = args.pop();
				if(i){
					args.push(args.pop() % i);
					break;
				}else throw "POLIZ: zero in mod";
			case LEX_PLUSD:
				i = args.pop();
				j = args.pop();
				args.push(i + j);
				TID[j].put_value(i + TID[j].get_value());
				TID[j].put_assign();
				break;
			case LEX_MINUSD:
				if(is_str) throw "Illegal action with string format";
				i = args.pop();
				j = args.pop();
				args.push(i - j);
				TID[j].put_value(i - TID[j].get_value());
				TID[j].put_assign();
				break;
			case LEX_MULD:
				if(is_str) throw "Illegal action with string format";
				i = args.pop();
				j = args.pop();
				args.push(i * j);
				TID[j].put_value(i * TID[j].get_value());
				TID[j].put_assign();
				break;
			case LEX_DIVD:
				if(is_str) throw "Illegal action with string format";
				i = args.pop();
				j = args.pop();
				args.push(i / j);
				TID[j].put_value(i / TID[j].get_value());
				TID[j].put_assign();
				break;
			case LEX_EQ:
				args.push(args.pop() == args.pop());
				break;
			case LEX_LESS:
				i = args.pop();
				args.push(args.pop() < i);
				break;
			case LEX_GRT:
				i = args.pop();
				args.push(args.pop() > i);
				break;
			case LEX_LEQ:
				i = args.pop();
				args.push(args.pop() <= i);
				break;
			case LEX_GEQ:
				i = args.pop();
				args.push(args.pop() >= i);
				break;
			case LEX_NEQ:
				i = args.pop();
				args.push(args.pop() != i);
				break;
			case LEX_DEF:
				if(is_str){
					i = args.pop();
					TID[i].put_name(c_str);
					TID[i].put_assign();
					is_str = false;
				}else{
					i = args.pop();
					j = args.pop();
					TID[j].put_value(i);
					TID[j].put_assign();
				}
				break;
			default:
				break;
		}
		index++;
	}
}

int Executer::get_expr(Poliz& prog){
	Stack <int, 1000> args;
	int i, j;
	while(index < size){
		pc_el = prog[index];
		switch(pc_el.get_type()){
			case POLIZ_EXPR:
				i = args.pop();
				return i;
			case POLIZ_ADDRESS:
				args.push(pc_el.get_value());
				break;
			case LEX_NUM:
				args.push(pc_el.get_value());
				break;
			case POLIZ_LABEL:
				args.push(pc_el.get_value());
				break;
			case LEX_IDENT:
				i = pc_el.get_value();
				args.push(TID[i].get_value());
				break;
			case LEX_PLUS:
				i = args.pop();
				j = args.pop();
				args.push(i + j);
				break;
			case LEX_MUL:
				args.push(i * j);
				break;
			case LEX_MINUS:
				i = args.pop();
				args.push(args.pop() - i);
				break;
			case LEX_DIV:
				i = args.pop();
				if(i){
					args.push(args.pop() / i);
					break;
				}else throw "POLIZ: divide by zero";
			case LEX_MOD:
				i = args.pop();
				if(i){
					args.push(args.pop() % i);
					break;
				}else throw "POLIZ: zero in mod";
		}
		index++;
	}
	return 0;
}

class Interpretator{
	Parser pars;
	Executer E;
public:
	Interpretator(char* program): pars(program) {};
	void interpretate();
};

void Interpretator::interpretate(){
	pars.analyze();
	if(write_poliz) pars.prog.print();
	E.execute(pars.prog);
}

int main(int argc, char* argv[]){
	if(argc < 2){
		std::cerr << "Введите файл с программой" << std::endl;
		exit(1);
	}
	if(argc > 2 && !strcmp(argv[2], "poliz")) write_poliz = true;
	else write_poliz = false;
	
	cout << "Выполняю программу" << endl;
	Interpretator I(argv[1]);
	try{
		I.interpretate();
	}catch(int c){
		if(c == EOF) cout << "Ошибка: неожиданный конец файла" << endl;
		else cout << "Ошибка в символе: " << (char)c << endl;
		exit(2);
	}catch(Lex lex){
		cout << "Ошибка в лексеме: " << lex << endl;
		exit(3);
	}catch(const char* s){
		cout << s << endl;
		exit(4);
	}
	
	/*for(vector<Ident>::iterator it = TID.begin(); it != TID.end(); it++){
		cout << "var " << (*it).get_name() << " = " << (*it).get_value() << ", type: " << (*it).get_type() << endl;
	}*/
	return 0;
}

int transform(int val, int offset){
	return (offset << 16) + val;
}
