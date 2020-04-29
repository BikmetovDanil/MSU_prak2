#include "parser.h"

using namespace std;
extern vector<Ident> TID;

void Parser::dec(type_of_lex type){
	int i;
	while(!st_int.is_empty()){
		i = st_int.pop();
		if(TID[i].get_declare()) throw "Twice declare";
		else{
			TID[i].put_declare();
			TID[i].put_type(type);
		}
	}
	var_type = LEX_NULL;
}

void Parser::dec(type_of_lex type, int val){
	if(TID[val].get_declare()) throw "Twice declare";
	else{
		TID[val].put_declare();
		TID[val].put_type(type);
	}
	var_type = LEX_NULL;
}

void Parser::check_ident(){
	if(TID[c_val].get_declare()){
		st_lex.push(TID[c_val].get_type());
		var_type = TID[c_val].get_type();
	}else throw "Not declared";
}

// Операции можно проводить над любыми операндами
void Parser::check_op(){
	
}

// "!" работает только с переменными типа bool
void Parser::check_not(){
	if(st_lex.pop() != LEX_BOOL) throw "Wrong type is in not";
	else st_lex.push(LEX_BOOL);
}

void Parser::eq_type(){
	// num op num -> num
	// str op str -> num
	// str op num -> str
	// str op bool -> str
	// bool op num -> num / bool
	// bool op bool -> num / bool
}

void Parser::eq_bool(){
	
}

void Parser::analyze(){
	S();
	if(c_type != LEX_FIN) throw(cur_lex);
}

void Parser::S(){
	gl();
	while(c_type != LEX_FIN){
		SENTENCE();
	}
}

void Parser::SENTENCE(){
	if(c_type == KEY_FUNC) FUNC_DEF();
	else OPERATOR();
}

void Parser::FUNC_DEF(){
	gl();
	if(c_type != LEX_IDENT) throw(cur_lex);
	gl();
	if(c_type != LEX_LPAR) throw(cur_lex);
	gl();
	while(c_type != LEX_RPAR){
		if(c_type == LEX_IDENT){
			gl();
			if(c_type == LEX_RPAR) break;
			if(c_type != LEX_COMMA) throw(cur_lex);
		}else throw(cur_lex);
		gl();
	}
	gl();
	BLOCK();
}

void Parser::OPERATOR(){
	if(c_type == KEY_VAR) VAR_DEF();
	else if(c_type == LEX_SEMICOLON) gl();
	else if(c_type == LEX_BEGIN) BLOCK();
	else if(c_type == KEY_IF) COND_OP();
	else if(c_type == KEY_DO || c_type == KEY_WHILE || c_type == KEY_FOR) LOOP_OP();
	else if(c_type == KEY_BREAK || c_type == KEY_CONT || c_type == KEY_RET) TRANSIT_OP();
	else if(c_type == KEY_READ){
		READ();
	}else if(c_type == KEY_WRITE){
		WRITE();
	}else if(c_type == LEX_IDENT){
		OPID(0);
	}else if(c_type == LEX_NUM || c_type == LEX_PLUS || c_type == LEX_MINUS){
		EXPRESSION();
	}else if(c_type == LEX_INC){
		gl();
		if(c_type != LEX_IDENT) throw(cur_lex);
		OPID(1);
	}else if(c_type == LEX_DEC){
		gl();
		if(c_type != LEX_IDENT) throw(cur_lex);
		OPID(-1);
	}else if(c_type == OBJ_RESP){
		gl();
		if(c_type != LEX_DOT) throw(cur_lex);
		gl();
		if(c_type != KEY_WRITE) throw(cur_lex);
		WRITE();
	}else if(c_type == OBJ_ENV){
		ENV();
	}else throw(cur_lex);
}

void Parser::BLOCK(){
	if(c_type != LEX_BEGIN) throw(cur_lex);
	gl();
	while(c_type != LEX_END){
		if(c_type == LEX_FIN) throw(cur_lex);
		OPERATOR();
	}
	gl();
}

void Parser::VAR_DEF(){
	st_int.reset();
	do{
		gl();
		if(c_type != LEX_IDENT) throw(cur_lex);
		st_int.push(c_val);
		int prev_val = c_val;
		gl();
		if(c_type == LEX_DEF){
			prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
			gl();
			if(is_expression(c_type)){
				EXPRESSION();
				if(var_type == KEY_TRUE || var_type == KEY_FALSE) var_type = LEX_BOOL;
				dec(var_type);
				prog.put_lex(Lex(LEX_DEF));
			}else if(c_type == LEX_LSQB){
				dec(LEX_ARR, prev_val);
				prog.pop();
				int offset = 0;
				gl();
				while(c_type != LEX_RSQB){
					prog.put_lex(Lex(POLIZ_ADDRESS, transform(prev_val, offset)));
					EXPRESSION();
					prog.put_lex(Lex(LEX_DEF));
					offset++;
					if(c_type == LEX_COMMA) gl();
					else if(c_type != LEX_RSQB) throw(cur_lex);
				}
				gl();
			}else if(c_type == OBJ_ENV){
				ENV();
			}else throw(cur_lex);
		}else dec(LEX_UNDEF);
	}while(c_type == LEX_COMMA);
	if(c_type != LEX_SEMICOLON) throw(cur_lex);
	gl();
}

void Parser::COND_OP(){
	gl();
	if(c_type != LEX_LPAR) throw(cur_lex);
	gl();
	if(is_expression(c_type)) EXPRESSION();
	else throw(cur_lex);
	if(c_type != LEX_RPAR) throw(cur_lex);
	gl();
	OPERATOR();
	if(c_type == KEY_ELSE){
		gl();
		OPERATOR();
	}
}

void Parser::LOOP_OP(){
	if(c_type == KEY_WHILE){
		gl();
		if(c_type != LEX_LPAR) throw(cur_lex);
		gl();
		if(is_expression(c_type)) EXPRESSION();
		else throw(cur_lex);
		if(c_type != LEX_RPAR) throw(cur_lex);
		gl();
		OPERATOR();
	}else if(c_type == KEY_DO){
		gl();
		OPERATOR();
		if(c_type != KEY_WHILE) throw(cur_lex);
		gl();
		if(c_type != LEX_LPAR) throw(cur_lex);
		gl();
		if(is_expression(c_type)) EXPRESSION();
		else throw(cur_lex);
		if(c_type != LEX_RPAR) throw(cur_lex);
		gl();
		if(c_type != LEX_SEMICOLON) throw(cur_lex);
		gl();
	}else if(c_type == KEY_FOR){
		gl();
		if(c_type != LEX_LPAR) throw(cur_lex);
		gl();
		if(is_expression(c_type) || c_type == LEX_SEMICOLON){
			if(is_expression(c_type)) EXPRESSION();
			else if(c_type != LEX_SEMICOLON) throw(cur_lex);
			gl();
			if(is_expression(c_type)) EXPRESSION();
			else if(c_type != LEX_SEMICOLON) throw(cur_lex);
			gl();
			if(is_expression(c_type)) EXPRESSION();
			if(c_type != LEX_RPAR) throw(cur_lex);
			gl();
			OPERATOR();
		}else if(c_type == KEY_VAR){
			if(c_type == KEY_VAR) gl();
			gl();
			if(c_type != KEY_IN) throw(cur_lex);
			gl();
			if(is_expression(c_type)) EXPRESSION();
			else throw(cur_lex);
			if(c_type != LEX_RPAR) throw(cur_lex);
			gl();
			OPERATOR();
		}else throw(cur_lex);
	}
}

void Parser::TRANSIT_OP(){
	if(c_type == KEY_RET){
		gl();
		if(is_expression(c_type)) EXPRESSION();
	}else gl();
	if(c_type != LEX_SEMICOLON) throw(cur_lex);
	gl();
}

void Parser::EXPR_OP(){
	EXPRESSION();
	if(c_type != LEX_SEMICOLON) throw(cur_lex);
	gl();
}

void Parser::EXPRESSION(){
	l_value = true;
	E1();
	if(c_type == LEX_EQ || c_type == LEX_LESS || c_type == LEX_GRT ||
			c_type == LEX_LEQ || c_type == LEX_GEQ || c_type == LEX_NEQ ||
			c_type == LEX_EQ2 || c_type == LEX_NEQ2){
		var_type = LEX_BOOL; // MAY BE CHANGED
		gl();
		E1();
		check_op();
		prog.put_lex(cur_lex); //st_lex.push(c_type);
	}
}

void Parser::E1(){
	E2();
	while(c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_OR){
		gl();
		E2();
		check_op();
		prog.put_lex(cur_lex); //st_lex.push(c_type);
	}
}

void Parser::E2(){
	E3();
	while(c_type == LEX_MUL || c_type == LEX_DIV || c_type == LEX_AND || c_type == LEX_MOD){
		gl();
		E3();
		check_op();
		prog.put_lex(cur_lex); //st_lex.push(c_type);
	}
}

void Parser::E3(){
	if(c_type == LEX_IDENT){
		check_ident();
		if(TID[c_val].get_type() == LEX_ARR){
			int prev_val = c_val;
			gl();
			if(c_type == LEX_LSQB){
				gl();
				if(c_type != LEX_NUM) throw(cur_lex);
				prog.put_lex(Lex(LEX_IDENT, c_val));
				prev_val = transform(prev_val, c_val);
				gl();
				if(c_type != LEX_RSQB) throw(cur_lex);
				gl();
				if(l_value && is_def_operator()){
					prog.pop();
					prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
					D();
				}else if(l_value && (c_type == LEX_INC)){
					prog.pop();
					prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
					prog.put_lex(Lex(LEX_NUM, 1));
					prog.put_lex(Lex(LEX_PLUSD));
					gl();
				}else if(l_value && (c_type == LEX_DEC)){
					prog.pop();
					prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
					prog.put_lex(Lex(LEX_NUM, 1));
					prog.put_lex(Lex(LEX_MINUSD));
					gl();
				}
			}
		}else{
			prog.put_lex(Lex(LEX_IDENT, c_val));
			int prev_val = c_val;
			gl();
			if(l_value && is_def_operator()){
				prog.pop();
				prog.put_lex(POLIZ_ADDRESS, prev_val);
				D();
			}else if(l_value && (c_type == LEX_INC)){
				prog.pop();
				prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
				prog.put_lex(Lex(LEX_NUM, 1));
				prog.put_lex(Lex(LEX_PLUSD));
				gl();
			}else if(l_value && (c_type == LEX_DEC)){
				prog.pop();
				prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
				prog.put_lex(Lex(LEX_NUM, 1));
				prog.put_lex(Lex(LEX_MINUSD));
				gl();
			}
		}
	}else if(c_type == LEX_NUM){
		st_lex.push(LEX_NUM);
		if(var_type == LEX_NUM) var_type = LEX_NUM;
		else if(var_type == LEX_STR) var_type = LEX_STR;
		else if(var_type == LEX_BOOL) var_type = LEX_NUM;
		else var_type = LEX_NUM;
		prog.put_lex(cur_lex);
		gl();
	}else if(c_type == LEX_STR){
		st_lex.push(LEX_STR);
		if(var_type == LEX_NUM) var_type = LEX_STR;
		else if(var_type == LEX_STR) var_type = LEX_STR;
		else if(var_type == LEX_BOOL) var_type = LEX_STR;
		else var_type = LEX_STR;
		prog.put_lex(cur_lex);
		gl();
	}else if(c_type == KEY_TRUE){
		st_lex.push(LEX_BOOL);
		if(var_type == LEX_NUM) var_type = LEX_NUM;
		else if(var_type == LEX_STR) var_type = LEX_STR;
		else if(var_type == LEX_BOOL) var_type = LEX_BOOL;
		else var_type = LEX_BOOL;
		prog.put_lex(Lex(KEY_TRUE, 1));
		gl();
	}else if(c_type == KEY_FALSE){
		st_lex.push(LEX_BOOL);
		if(var_type == LEX_NUM) var_type = LEX_NUM;
		else if(var_type == LEX_STR) var_type = LEX_STR;
		else if(var_type == LEX_BOOL) var_type = LEX_BOOL;
		else var_type = LEX_BOOL;
		prog.put_lex(Lex(KEY_FALSE, 0));
		gl();
	}else if(c_type == LEX_NOT){
		gl();
		E3();
		check_not();
	}else if(c_type == LEX_LPAR){
		gl();
		EXPRESSION();
		if(c_type == LEX_RPAR) gl();
		else throw(cur_lex);
	}else if(c_type == OBJ_ENV){
		l_value = false;
		ENV();
	}else return; // MAY BE CHANGED
	l_value = false;
}

void Parser::D(){
	if(is_def_operator()){
		type_of_lex op = c_type;
		gl();
		EXPRESSION();
		eq_type();
		prog.put_lex(Lex(op));
	}else throw(cur_lex);
}

void Parser::OPID(int flag){
	check_ident();
	if(TID[c_val].get_type() == LEX_ARR){
		int prev_val = c_val;
		gl();
		if(c_type == LEX_LSQB){
			gl();
			if(c_type != LEX_NUM) throw(cur_lex);
			prev_val = transform(prev_val, c_val);
			if(flag != 0){
				prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
				if(flag == -1) prog.put_lex(Lex(LEX_NUM, -1));
				else if(flag == 1) prog.put_lex(Lex(LEX_NUM, 1));
				prog.put_lex(Lex(LEX_PLUSD));
			}
			gl();
			if(c_type != LEX_RSQB) throw(cur_lex);
			gl();
			if(is_def_operator()){
				prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
				D();
			}else if(c_type == LEX_INC){
				prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
				prog.put_lex(Lex(LEX_NUM, 1));
				prog.put_lex(Lex(LEX_PLUSD));
				gl();
			}else if(c_type == LEX_DEC){
				prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
				prog.put_lex(Lex(LEX_NUM, 1));
				prog.put_lex(Lex(LEX_MINUSD));
				gl();
			}else EXPRESSION();
		}else if(c_type == LEX_DEF){
			gl();
		}else EXPRESSION();
	}else{
		int prev_val = c_val;
		if(flag != 0){
			prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
			if(flag == -1) prog.put_lex(Lex(LEX_NUM, -1));
			else if(flag == 1) prog.put_lex(Lex(LEX_NUM, 1));
			prog.put_lex(Lex(LEX_PLUSD));
		}
		gl();
		if(is_def_operator()){
			prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
			D();
		}else if(c_type == LEX_INC){
			prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
			prog.put_lex(Lex(LEX_NUM, 1));
			prog.put_lex(Lex(LEX_PLUSD));
			gl();
		}else if(c_type == LEX_DEC){
			prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
			prog.put_lex(Lex(LEX_NUM, 1));
			prog.put_lex(Lex(LEX_MINUSD));
			gl();
		}else EXPRESSION();
	}
}

void Parser::READ(){
	gl();
	if(c_type != LEX_LPAR) throw(cur_lex);
	gl();
	if(c_type == LEX_IDENT){
		
	}else throw(cur_lex);
	gl();
	if(c_type != LEX_RPAR) throw(cur_lex);
	gl();
}

void Parser::WRITE(){
	gl();
	if(c_type != LEX_LPAR) throw(cur_lex);
	gl();
	if(c_type == LEX_IDENT || c_type == LEX_NUM || c_type == LEX_STR || c_type == LEX_BOOL){
		cout << c_val << endl;
	}else throw(cur_lex);
	gl();
	if(c_type != LEX_RPAR) throw(cur_lex);
	gl();
}

void Parser::ENV(){
	gl();
	cout << cur_lex << endl;
	if(c_type != LEX_DOT) throw(cur_lex);
	gl();
	gl();
}

int transform(int val, int offset){
	return (offset << 16) + val;
}
