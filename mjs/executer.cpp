#include "executer.h"

using namespace std;

int strToInt(string s){
	int res = 0;
	for(int i = 0; i < s.length(); i++){
		if(isdigit(s[i])){
			res *= 10;
			res += s[i] - '0';
		}else return res;
	}
	return res;
}

void Executer::execute(Poliz& prog){
	Stack <Arg, 1000> args;
	Arg t, u;
	int i, j, k;
	index = 0;
	size = prog.get_free();
	while(index < size){
		pc_el = prog[index];
		switch(pc_el.get_type()){
			case POLIZ_ADDRESS:
				i = pc_el.get_value();
				if(TID[i].get_type() == LEX_ARR){
					index++;
					t = get_expr(prog);
					j = t.value;
					pc_el = prog[++index];
					k = TID[i][j];
					TID[k].put_type(pc_el.get_type());
					args.push(Arg(POLIZ_ADDRESS, k));
				}else args.push(Arg(POLIZ_ADDRESS, i));
				break;
			case LEX_NUM:
				args.push(Arg(LEX_NUM, pc_el.get_value()));
				break;
			case POLIZ_LABEL:
				args.push(Arg(POLIZ_LABEL, pc_el.get_value()));
				break;
			case LEX_IDENT:
				i = pc_el.get_value();
				if(TID[i].get_type() == LEX_ARR){
					index++;
					t = get_expr(prog);
					j = t.value;
					pc_el = prog[++index];
					k = TID[i][j];
					if(TID[k].get_type() == LEX_STR) args.push(Arg(LEX_STR, TID[k].get_name()));
					else args.push(Arg(TID[k].get_type(), TID[k].get_value()));
				}else{
					if(TID[i].get_type() == LEX_STR) args.push(Arg(LEX_STR, TID[i].get_name()));
					else args.push(Arg(TID[i].get_type(), TID[i].get_value()));
				}
				break;
			case LEX_STR:
				args.push(Arg(LEX_STR, pc_el.get_name()));
				break;
			case LEX_NOT:
				t = args.pop();
				t.value = !t.value;
				args.push(t);
				break;
			case LEX_OR:
				t = args.pop();
				u = args.pop();
				t.value = t.value || u.value;
				args.push(t);
				break;
			case LEX_AND:
				t = args.pop();
				u = args.pop();
				t.value = t.value && u.value;
				args.push(t);
				break;
			case POLIZ_GO:
				t = args.pop();
				index = t.value - 1;
				break;
			case POLIZ_FGO:
				t = args.pop();
				u = args.pop();
				if(!u.value) index = t.value - 1;
				break;
			case POLIZ_CHANGE:
				pc_el = prog[++index];
				i = pc_el.get_value();
				TID[i].put_type(pc_el.get_type());
				break;
			case KEY_WRITE:
				pc_el = prog[++index];
				if(pc_el.get_type() == LEX_STR) cout << pc_el.get_name();
				else if(pc_el.get_type() == LEX_IDENT){
					i = pc_el.get_value();
					if(TID[i].get_type() == LEX_ARR){
						t = get_expr(prog);
						j = t.value;
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
				t = args.pop();
				u = args.pop();
				if(u.type == LEX_NUM){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value + t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_NUM, u.value + t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(u.value) + t.name));
				}else if(u.type == LEX_BOOL){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value + t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_BOOL, u.value + t.value));
					else if(t.type == LEX_STR){
						if(u.value) args.push(Arg(LEX_STR, "true" + t.name));
						else args.push(Arg(LEX_STR, "false" + t.name));
					}
				}else if(u.type == LEX_STR){
					if(t.type == LEX_NUM) args.push(Arg(LEX_STR, u.name + to_string(t.value)));
					else if(t.type == LEX_BOOL){
						if(t.value) args.push(Arg(LEX_NUM, u.value + "true"));
						else args.push(Arg(LEX_NUM, u.value + "false"));
					}else if(t.type == LEX_STR) args.push(Arg(LEX_STR, u.name + t.name));
				}
				break;
			case LEX_MUL:
				t = args.pop();
				u = args.pop();
				if(u.type == LEX_NUM){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value * t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_NUM, u.value * t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(u.value * strToInt(t.name))));
				}else if(u.type == LEX_BOOL){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value * t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_BOOL, u.value * t.value));
					else if(t.type == LEX_STR){
						if(u.value) args.push(Arg(LEX_STR, t.name));
						else args.push(Arg(LEX_STR, ""));
					}
				}else if(u.type == LEX_STR){
					if(t.type == LEX_NUM) args.push(Arg(LEX_STR, to_string(strToInt(u.name) * t.value)));
					else if(t.type == LEX_BOOL){
						if(t.value) args.push(Arg(LEX_STR, u.name));
						else args.push(Arg(LEX_STR, ""));
					}else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(strToInt(u.name) * strToInt(t.name))));
				}
				break;
			case LEX_MINUS:
				t = args.pop();
				u = args.pop();
				if(u.type == LEX_NUM){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value - t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_NUM, u.value - t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(u.value - strToInt(t.name))));
				}else if(u.type == LEX_BOOL){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value - t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_BOOL, u.value - t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, t.name));
				}else if(u.type == LEX_STR){
					if(t.type == LEX_NUM) args.push(Arg(LEX_STR, to_string(strToInt(u.name) - t.value)));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_STR, u.name));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(strToInt(u.name) - strToInt(t.name))));
				}
				break;
			case LEX_DIV:
				t = args.pop();
				u = args.pop();
				if((t.type == LEX_STR && strToInt(t.name) == 0) || (t.type != LEX_STR && t.value == 0)){
					args.push(Arg(LEX_UNDEF, 0));
					break;
				}
				if(u.type == LEX_NUM){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value / t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_NUM, u.value / t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(u.value / strToInt(t.name))));
				}else if(u.type == LEX_BOOL){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value / t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_BOOL, u.value / t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, t.name));
				}else if(u.type == LEX_STR){
					if(t.type == LEX_NUM) args.push(Arg(LEX_STR, to_string(strToInt(u.name) / t.value)));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_STR, u.name));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(strToInt(u.name) / strToInt(t.name))));
				}
				break;
			case LEX_MOD:
				t = args.pop();
				u = args.pop();
				if((t.type == LEX_STR && strToInt(t.name) == 0) || (t.type != LEX_STR && t.value == 0)){
					args.push(Arg(LEX_UNDEF, 0));
					break;
				}
				if(u.type == LEX_NUM){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value / t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_NUM, u.value / t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(u.value / strToInt(t.name))));
				}else if(u.type == LEX_BOOL){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value / t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_BOOL, u.value / t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, t.name));
				}else if(u.type == LEX_STR){
					if(t.type == LEX_NUM) args.push(Arg(LEX_STR, to_string(strToInt(u.name) / t.value)));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_STR, u.name));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(strToInt(u.name) / strToInt(t.name))));
				}
				break;
			case LEX_PLUSD:
				t = args.pop();
				u = args.pop();
				j = u.value;
				if(TID[j].get_type() == LEX_NUM){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, TID[j].get_value() + t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_NUM, TID[j].get_value() + t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(TID[j].get_value()) + t.name));
				}else if(TID[j].get_type() == LEX_BOOL){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, TID[j].get_value() + t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_BOOL, TID[j].get_value() + t.value));
					else if(t.type == LEX_STR){
						if(u.value) args.push(Arg(LEX_STR, "true" + t.name));
						else args.push(Arg(LEX_STR, "false" + t.name));
					}
				}else if(TID[j].get_type() == LEX_STR){
					if(t.type == LEX_NUM) args.push(Arg(LEX_STR, TID[j].get_name() + to_string(t.value)));
					else if(t.type == LEX_BOOL){
						if(t.value) args.push(Arg(LEX_NUM, TID[j].get_value() + "true"));
						else args.push(Arg(LEX_NUM, TID[j].get_value() + "false"));
					}else if(t.type == LEX_STR) args.push(Arg(LEX_STR, TID[j].get_name() + t.name));
				}
				t = args.pop();
				TID[j].put_type(t.type);
				TID[j].put_value(t.value);
				TID[j].put_name(t.name);
				TID[j].put_assign();
				break;
			case LEX_MINUSD:
				t = args.pop();
				u = args.pop();
				j = u.value;
				if(TID[j].get_type() == LEX_NUM){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, TID[j].get_value() - t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_NUM, TID[j].get_value() - t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(TID[j].get_value() - strToInt(t.name))));
				}else if(TID[j].get_type() == LEX_BOOL){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, TID[j].get_value() - t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_BOOL, TID[j].get_value() - t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, t.name));
				}else if(TID[j].get_type() == LEX_STR){
					if(t.type == LEX_NUM) args.push(Arg(LEX_STR, to_string(strToInt(TID[j].get_name()) - t.value)));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_STR, TID[j].get_name()));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(strToInt(TID[j].get_name()) - strToInt(t.name))));
				}
				t = args.pop();
				TID[j].put_type(t.type);
				TID[j].put_value(t.value);
				TID[j].put_name(t.name);
				TID[j].put_assign();
				break;
			case LEX_MULD:
				t = args.pop();
				u = args.pop();
				j = u.value;
				if(TID[j].get_type() == LEX_NUM){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, TID[j].get_value() * t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_NUM, TID[j].get_value() * t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(TID[j].get_value() * strToInt(t.name))));
				}else if(TID[j].get_type() == LEX_BOOL){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, TID[j].get_value() * t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_BOOL, TID[j].get_value() * t.value));
					else if(t.type == LEX_STR){
						if(u.value) args.push(Arg(LEX_STR, t.name));
						else args.push(Arg(LEX_STR, ""));
					}
				}else if(TID[j].get_type() == LEX_STR){
					if(t.type == LEX_NUM) args.push(Arg(LEX_STR, to_string(strToInt(TID[j].get_name()) * t.value)));
					else if(t.type == LEX_BOOL){
						if(t.value) args.push(Arg(LEX_STR, TID[j].get_name()));
						else args.push(Arg(LEX_STR, ""));
					}else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(strToInt(TID[j].get_name()) * strToInt(t.name))));
				}
				t = args.pop();
				TID[j].put_type(t.type);
				TID[j].put_value(t.value);
				TID[j].put_name(t.name);
				TID[j].put_assign();
				break;
			case LEX_DIVD:
				t = args.pop();
				u = args.pop();
				j = u.value;
				if((t.type == LEX_STR && strToInt(t.name) == 0) || (t.type != LEX_STR && t.value == 0)){
					args.push(Arg(LEX_UNDEF, 0));
					break;
				}
				if(TID[j].get_type() == LEX_NUM){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, TID[j].get_value() / t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_NUM, TID[j].get_value() / t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(TID[j].get_value() / strToInt(t.name))));
				}else if(TID[j].get_type() == LEX_BOOL){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, TID[j].get_value() / t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_BOOL, TID[j].get_value() / t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, t.name));
				}else if(TID[j].get_type() == LEX_STR){
					if(t.type == LEX_NUM) args.push(Arg(LEX_STR, to_string(strToInt(TID[j].get_name()) / t.value)));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_STR, TID[j].get_name()));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(strToInt(TID[j].get_name()) / strToInt(t.name))));
				}
				t = args.pop();
				TID[j].put_type(t.type);
				TID[j].put_value(t.value);
				TID[j].put_name(t.name);
				TID[j].put_assign();
				break;
			case LEX_MODD:
				t = args.pop();
				u = args.pop();
				j = u.value;
				if((t.type == LEX_STR && strToInt(t.name) == 0) || (t.type != LEX_STR && t.value == 0)){
					args.push(Arg(LEX_UNDEF, 0));
					break;
				}
				if(TID[j].get_type() == LEX_NUM){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, TID[j].get_value() / t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_NUM, TID[j].get_value() / t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(TID[j].get_value() / strToInt(t.name))));
				}else if(TID[j].get_type() == LEX_BOOL){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, TID[j].get_value() / t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_BOOL, TID[j].get_value() / t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, t.name));
				}else if(TID[j].get_type() == LEX_STR){
					if(t.type == LEX_NUM) args.push(Arg(LEX_STR, to_string(strToInt(TID[j].get_name()) / t.value)));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_STR, TID[j].get_name()));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(strToInt(TID[j].get_name()) / strToInt(t.name))));
				}
				t = args.pop();
				TID[j].put_type(t.type);
				TID[j].put_value(t.value);
				TID[j].put_name(t.name);
				TID[j].put_assign();
				break;
			case LEX_EQ:
				t = args.pop();
				u = args.pop();
				if(u.type == LEX_STR){
					if(u.name == t.name) k = 1;
					else k = 0;
				}else if(u.type == POLIZ_ADDRESS){
					if(TID[u.value].get_type() == LEX_STR){
						if(u.name == t.name) k = 1;
						else k = 0;
					}else k = (u.value == t.value);
				}else k = (u.value == t.value);
				args.push(Arg(LEX_BOOL, k));
				break;
			case LEX_LESS:
				t = args.pop();
				u = args.pop();
				if(u.type == LEX_STR){
					if(u.name < t.name) k = 1;
					else k = 0;
				}else if(u.type == POLIZ_ADDRESS){
					if(TID[u.value].get_type() == LEX_STR){
						if(u.name < t.name) k = 1;
						else k = 0;
					}else k = (u.value < t.value);
				}else k = (u.value < t.value);
				args.push(Arg(LEX_BOOL, k));
				break;
			case LEX_GRT:
				t = args.pop();
				u = args.pop();
				if(u.type == LEX_STR){
					if(u.name > t.name) k = 1;
					else k = 0;
				}else if(u.type == POLIZ_ADDRESS){
					if(TID[u.value].get_type() == LEX_STR){
						if(u.name > t.name) k = 1;
						else k = 0;
					}else k = (u.value > t.value);
				}else k = (u.value > t.value);
				args.push(Arg(LEX_BOOL, k));
				break;
			case LEX_LEQ:
				t = args.pop();
				u = args.pop();
				if(u.type == LEX_STR){
					if(u.name <= t.name) k = 1;
					else k = 0;
				}else if(u.type == POLIZ_ADDRESS){
					if(TID[u.value].get_type() == LEX_STR){
						if(u.name <= t.name) k = 1;
						else k = 0;
					}else k = (u.value <= t.value);
				}else k = (u.value <= t.value);
				args.push(Arg(LEX_BOOL, k));
				break;
			case LEX_GEQ:
				t = args.pop();
				u = args.pop();
				if(u.type == LEX_STR){
					if(u.name >= t.name) k = 1;
					else k = 0;
				}else if(u.type == POLIZ_ADDRESS){
					if(TID[u.value].get_type() == LEX_STR){
						if(u.name >= t.name) k = 1;
						else k = 0;
					}else k = (u.value >= t.value);
				}else k = (u.value >= t.value);
				args.push(Arg(LEX_BOOL, k));
				break;
			case LEX_NEQ:
				t = args.pop();
				u = args.pop();
				if(u.type == LEX_STR){
					if(u.name != t.name) k = 1;
					else k = 0;
				}else if(u.type == POLIZ_ADDRESS){
					if(TID[u.value].get_type() == LEX_STR){
						if(u.name != t.name) k = 1;
						else k = 0;
					}else k = (u.value != t.value);
				}else k = (u.value != t.value);
				args.push(Arg(LEX_BOOL, k));
				break;
			case LEX_DEF:
				t = args.pop();
				u = args.pop();
				j = u.value;
				if(t.type == LEX_NUM){
					TID[j].put_value(t.value);
					TID[j].put_type(LEX_NUM);
				}else if(t.type == LEX_BOOL){
					TID[j].put_value(t.value);
					TID[j].put_type(LEX_BOOL);
				}else if(t.type == LEX_STR){
					TID[j].put_name(t.name);
					TID[j].put_type(LEX_STR);
				}else if(t.type == LEX_UNDEF){
					TID[j].put_type(LEX_UNDEF);
				}
				TID[j].put_assign();
				break;
			default:
				break;
		}
		index++;
	}
}

Arg Executer::get_expr(Poliz& prog){
	Stack <Arg, 1000> args;
	Arg t, u;
	int i, j, k;
	while(index < size){
		pc_el = prog[index];
		switch(pc_el.get_type()){
			case POLIZ_EXPR:
				return args.pop();
			case POLIZ_ADDRESS:
				i = pc_el.get_value();
				if(TID[i].get_type() == LEX_ARR){
					index++;
					t = get_expr(prog);
					j = t.value;
					pc_el = prog[++index];
					k = TID[i][j];
					TID[k].put_type(pc_el.get_type());
					args.push(Arg(POLIZ_ADDRESS, k));
				}else args.push(Arg(POLIZ_ADDRESS, i));
				break;
			case LEX_NUM:
				args.push(Arg(LEX_NUM, pc_el.get_value()));
				break;
			case POLIZ_LABEL:
				args.push(Arg(POLIZ_LABEL, pc_el.get_value()));
				break;
			case LEX_IDENT:
				i = pc_el.get_value();
				args.push(Arg(LEX_IDENT, TID[i].get_value()));
				break;
			case LEX_PLUS:
				t = args.pop();
				u = args.pop();
				if(u.type == LEX_NUM){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value + t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_NUM, u.value + t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(u.value) + t.name));
				}else if(u.type == LEX_BOOL){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value + t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_BOOL, u.value + t.value));
					else if(t.type == LEX_STR){
						if(u.value) args.push(Arg(LEX_STR, "true" + t.name));
						else args.push(Arg(LEX_STR, "false" + t.name));
					}
				}else if(u.type == LEX_STR){
					if(t.type == LEX_NUM) args.push(Arg(LEX_STR, u.name + to_string(t.value)));
					else if(t.type == LEX_BOOL){
						if(t.value) args.push(Arg(LEX_NUM, u.value + "true"));
						else args.push(Arg(LEX_NUM, u.value + "false"));
					}else if(t.type == LEX_STR) args.push(Arg(LEX_STR, u.name + t.name));
				}
				break;
			case LEX_MUL:
				t = args.pop();
				u = args.pop();
				if(u.type == LEX_NUM){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value * t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_NUM, u.value * t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(u.value * strToInt(t.name))));
				}else if(u.type == LEX_BOOL){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value * t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_BOOL, u.value * t.value));
					else if(t.type == LEX_STR){
						if(u.value) args.push(Arg(LEX_STR, t.name));
						else args.push(Arg(LEX_STR, ""));
					}
				}else if(u.type == LEX_STR){
					if(t.type == LEX_NUM) args.push(Arg(LEX_STR, to_string(strToInt(u.name) * t.value)));
					else if(t.type == LEX_BOOL){
						if(t.value) args.push(Arg(LEX_STR, u.name));
						else args.push(Arg(LEX_STR, ""));
					}else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(strToInt(u.name) * strToInt(t.name))));
				}
				break;
			case LEX_MINUS:
				t = args.pop();
				u = args.pop();
				if(u.type == LEX_NUM){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value - t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_NUM, u.value - t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(u.value - strToInt(t.name))));
				}else if(u.type == LEX_BOOL){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value - t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_BOOL, u.value - t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, t.name));
				}else if(u.type == LEX_STR){
					if(t.type == LEX_NUM) args.push(Arg(LEX_STR, to_string(strToInt(u.name) - t.value)));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_STR, u.name));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(strToInt(u.name) - strToInt(t.name))));
				}
				break;
			case LEX_DIV:
				t = args.pop();
				u = args.pop();
				if((t.type == LEX_STR && strToInt(t.name) == 0) || (t.type != LEX_STR && t.value == 0)){
					args.push(Arg(LEX_UNDEF, 0));
					break;
				}
				if(u.type == LEX_NUM){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value / t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_NUM, u.value / t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(u.value / strToInt(t.name))));
				}else if(u.type == LEX_BOOL){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value / t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_BOOL, u.value / t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, t.name));
				}else if(u.type == LEX_STR){
					if(t.type == LEX_NUM) args.push(Arg(LEX_STR, to_string(strToInt(u.name) / t.value)));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_STR, u.name));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(strToInt(u.name) / strToInt(t.name))));
				}
				break;
			case LEX_MOD:
				t = args.pop();
				u = args.pop();
				if((t.type == LEX_STR && strToInt(t.name) == 0) || (t.type != LEX_STR && t.value == 0)){
					args.push(Arg(LEX_UNDEF, 0));
					break;
				}
				if(u.type == LEX_NUM){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value / t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_NUM, u.value / t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(u.value / strToInt(t.name))));
				}else if(u.type == LEX_BOOL){
					if(t.type == LEX_NUM) args.push(Arg(LEX_NUM, u.value / t.value));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_BOOL, u.value / t.value));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, t.name));
				}else if(u.type == LEX_STR){
					if(t.type == LEX_NUM) args.push(Arg(LEX_STR, to_string(strToInt(u.name) / t.value)));
					else if(t.type == LEX_BOOL) args.push(Arg(LEX_STR, u.name));
					else if(t.type == LEX_STR) args.push(Arg(LEX_STR, to_string(strToInt(u.name) / strToInt(t.name))));
				}
				break;
		}
		index++;
	}
	return Arg(LEX_NULL, 0);
}
