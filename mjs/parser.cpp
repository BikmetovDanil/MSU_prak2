#include "parser.h"

using namespace std;
extern vector<Ident> TID;

void Parser::dec(type_of_lex type, int val){
	if(TID[val].get_declare()) throw "Twice declare";
	else{
		TID[val].put_declare();
		change_type(type, val);
	}
	var_type = LEX_UNDEF;
}

void Parser::check_ident(){
	if(TID[c_val].get_declare()){
		st_lex.push(TID[c_val].get_type());
		var_type = TID[c_val].get_type();
	}else throw "Not declared";
}

// "!" работает только с переменными типа bool
void Parser::check_not(){
	if(st_lex.pop() != LEX_BOOL) throw "Wrong type is in not";
	else st_lex.push(LEX_BOOL);
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
				dec(var_type, prev_val);
				prog.put_lex(Lex(LEX_DEF));
			}else if(c_type == LEX_LSQB){
				int pl0 = prog[prog.get_free() - 1].get_value();
				prog.put_lex(Lex(LEX_NUM, 0));
				prog.put_lex(Lex(POLIZ_EXPR));
				dec(LEX_ARR, prev_val);
				int offset = 0;
				gl();
				while(c_type != LEX_RSQB){
					prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
					prog.put_lex(Lex(LEX_NUM, offset));
					prog.put_lex(Lex(POLIZ_EXPR));
					int pl1 = prog.get_free();
					prog.blank();
					EXPRESSION();
					prog.put_lex(Lex(var_type), pl1);
					prog.put_lex(Lex(LEX_DEF));
					offset++;
					if(c_type == LEX_COMMA) gl();
					else if(c_type != LEX_RSQB) throw(cur_lex);
				}
				gl();
			}else if(c_type == OBJ_ENV){
				ENV();
				prog.put_lex(Lex(LEX_DEF));
			}else throw(cur_lex, prev_val);
		}else dec(LEX_UNDEF, prev_val);
	}while(c_type == LEX_COMMA);
	if(c_type != LEX_SEMICOLON) throw(cur_lex);
	gl();
}

void Parser::COND_OP(){
	int pl0, pl1;
	gl();
	if(c_type != LEX_LPAR) throw(cur_lex);
	gl();
	if(is_expression(c_type)) EXPRESSION();
	else throw(cur_lex);
	eq_bool();
	if(c_type != LEX_RPAR) throw(cur_lex);
	gl();
	pl0 = prog.get_free();
	prog.blank();
	prog.put_lex(Lex(POLIZ_FGO));
	OPERATOR();
	pl1 = prog.get_free();
	prog.blank();
	prog.put_lex(Lex(POLIZ_GO));
	if(c_type == KEY_ELSE){
		prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl0);
		gl();
		OPERATOR();
		prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl1);
	}else{
		prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl0);
		prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl1);
	}
}

void Parser::LOOP_OP(){
	int pl0, pl1, pl2, pl3, pl4;
	if(c_type == KEY_WHILE){
		gl();
		if(c_type != LEX_LPAR) throw(cur_lex);
		gl();
		pl0 = prog.get_free();
		if(is_expression(c_type)) EXPRESSION();
		else throw(cur_lex);
		eq_bool();
		pl1 = prog.get_free();
		prog.blank();
		prog.put_lex(Lex(POLIZ_FGO));
		if(c_type != LEX_RPAR) throw(cur_lex);
		gl();
		OPERATOR();
		prog.put_lex(Lex(POLIZ_LABEL, pl0));
		prog.put_lex(Lex(POLIZ_GO));
		prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl1);
	}else if(c_type == KEY_DO){
		pl0 = prog.get_free();
		gl();
		OPERATOR();
		if(c_type != KEY_WHILE) throw(cur_lex);
		gl();
		if(c_type != LEX_LPAR) throw(cur_lex);
		gl();
		if(is_expression(c_type)) EXPRESSION();
		else throw(cur_lex);
		eq_bool();
		pl1 = prog.get_free();
		prog.blank();
		prog.put_lex(Lex(POLIZ_FGO));
		prog.put_lex(Lex(POLIZ_LABEL, pl0));
		prog.put_lex(Lex(POLIZ_GO));
		if(c_type != LEX_RPAR) throw(cur_lex);
		gl();
		if(c_type != LEX_SEMICOLON) throw(cur_lex);
		gl();
		pl2 = prog.get_free();
		prog.put_lex(Lex(POLIZ_LABEL, pl2), pl1);
	}else if(c_type == KEY_FOR){
		gl();
		if(c_type != LEX_LPAR) throw(cur_lex);
		gl();
		if(is_expression(c_type) || c_type == LEX_SEMICOLON){
			if(is_expression(c_type)) EXPRESSION();
			else if(c_type != LEX_SEMICOLON) throw(cur_lex);
			gl();
			pl0 = prog.get_free();
			if(is_expression(c_type)) EXPRESSION();
			else if(c_type != LEX_SEMICOLON) throw(cur_lex);
			eq_bool();
			pl1 = prog.get_free();
			prog.blank();
			prog.put_lex(Lex(POLIZ_FGO));
			prog.blank();
			prog.put_lex(Lex(POLIZ_GO));
			gl();
			pl2 = prog.get_free();
			if(is_expression(c_type)) EXPRESSION();
			if(c_type != LEX_RPAR) throw(cur_lex);
			prog.put_lex(Lex(POLIZ_LABEL, pl0));
			prog.put_lex(Lex(POLIZ_GO));
			gl();
			pl3 = prog.get_free();
			prog.put_lex(Lex(POLIZ_LABEL, pl3), pl1 + 2);
			OPERATOR();
			prog.put_lex(Lex(POLIZ_LABEL, pl2));
			prog.put_lex(Lex(POLIZ_GO));
			pl4 = prog.get_free();
			prog.put_lex(Lex(POLIZ_LABEL, pl4), pl1);
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
	var_type = LEX_UNDEF;
	E1();
	if(c_type == LEX_EQ || c_type == LEX_LESS || c_type == LEX_GRT ||
			c_type == LEX_LEQ || c_type == LEX_GEQ || c_type == LEX_NEQ ||
			c_type == LEX_EQ2 || c_type == LEX_NEQ2){
		var_type = LEX_BOOL; // MAY BE CHANGED
		type_of_lex prev_type = c_type;
		gl();
		E1();
		prog.put_lex(prev_type); //st_lex.push(prev_type);
	}
}

void Parser::E1(){
	E2();
	while(c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_OR){
		type_of_lex prev_type = c_type;
		gl();
		E2();
		prog.put_lex(prev_type); //st_lex.push(prev_type);
	}
}

void Parser::E2(){
	E3();
	if(c_type == LEX_DOT){
		l_value = false;
		DOT();
	}
	while(c_type == LEX_MUL || c_type == LEX_DIV || c_type == LEX_AND || c_type == LEX_MOD){
		type_of_lex prev_type = c_type;
		gl();
		E3();
		prog.put_lex(prev_type); //st_lex.push(prev_type);
	}
}

void Parser::E3(){
	if(c_type == LEX_IDENT){
		check_ident();
		int prev_val = c_val;
		if(TID[c_val].get_type() == LEX_ARR){
			gl();
			if(c_type == LEX_LSQB){
				gl();
				int pl0 = prog.get_free();
				prog.put_lex(Lex(LEX_IDENT, prev_val));
				EXPRESSION();
				prog.put_lex(Lex(POLIZ_EXPR));
				int pl1 = prog.get_free();
				prog.blank();
				if(c_type != LEX_RSQB) throw(cur_lex);
				gl();
				if(l_value && is_def_operator()){
					type_of_lex op = c_type;
					prog.put_lex(Lex(POLIZ_ADDRESS, prev_val), pl0);
					gl();
					EXPRESSION();
					prog.put_lex(Lex(var_type), pl1);
					prog.put_lex(Lex(op));
				}else if(l_value && (c_type == LEX_INC)){
					prog.put_lex(Lex(POLIZ_ADDRESS, prev_val), pl0);
					prog.put_lex(Lex(LEX_NUM), pl1);
					prog.put_lex(Lex(LEX_NUM, 1));
					prog.put_lex(Lex(LEX_PLUSD));
					gl();
				}else if(l_value && (c_type == LEX_DEC)){
					prog.put_lex(Lex(POLIZ_ADDRESS, prev_val), pl0);
					prog.put_lex(Lex(LEX_NUM), pl1);
					prog.put_lex(Lex(LEX_NUM, 1));
					prog.put_lex(Lex(LEX_MINUSD));
					gl();
				}
			}
		}else{
			prog.put_lex(Lex(LEX_IDENT, prev_val));
			gl();
			if(l_value && is_def_operator()){
				prog.pop();
				prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
				var_type = TID[prev_val].get_type();
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
		if(var_type != LEX_STR) var_type = LEX_NUM;
		prog.put_lex(cur_lex);
		gl();
	}else if(c_type == LEX_STR){
		st_lex.push(LEX_STR);
		var_type = LEX_STR;
		prog.put_lex(cur_lex);
		gl();
	}else if(c_type == KEY_TRUE){
		st_lex.push(LEX_BOOL);
		if(var_type == LEX_NUM) var_type = LEX_NUM;
		else if(var_type == LEX_STR) var_type = LEX_STR;
		else var_type = LEX_BOOL;
		prog.put_lex(Lex(KEY_TRUE, 1));
		gl();
	}else if(c_type == KEY_FALSE){
		st_lex.push(LEX_BOOL);
		if(var_type == LEX_NUM) var_type = LEX_NUM;
		else if(var_type == LEX_STR) var_type = LEX_STR;
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
		int pl0 = prog[prog.get_free() - 1].get_value();
		type_of_lex op = c_type;
		gl();
		EXPRESSION();
		change_type(var_type, pl0);
		prog.put_lex(Lex(op));
	}else throw(cur_lex);
}

void Parser::OPID(int flag){
	check_ident();
	int prev_val = c_val;
	if(TID[c_val].get_type() == LEX_ARR){
		gl();
		if(c_type == LEX_LSQB){
			gl();
			int pl0 = prog.get_free();
			prog.put_lex(Lex(LEX_IDENT, c_val));
			EXPRESSION();
			prog.put_lex(Lex(POLIZ_EXPR));
			int pl1 = prog.get_free();
			prog.blank();
			if(flag != 0){
				prog.put_lex(Lex(POLIZ_ADDRESS, prev_val), pl0);
				prog.put_lex(Lex(LEX_NUM), pl1);
				prog.put_lex(Lex(LEX_NUM, 1));
				if(flag == -1) prog.put_lex(Lex(LEX_MINUSD));
				else if(flag == 1) prog.put_lex(Lex(LEX_PLUSD));
			}
			if(c_type != LEX_RSQB) throw(cur_lex);
			gl();
			if(is_def_operator()){
				type_of_lex op = c_type;
				prog.put_lex(Lex(POLIZ_ADDRESS, prev_val), pl0);
				gl();
				EXPRESSION();
				prog.put_lex(Lex(var_type), pl1);
				prog.put_lex(Lex(op));
			}else if(c_type == LEX_INC){
				prog.put_lex(Lex(POLIZ_ADDRESS, prev_val), pl0);
				prog.put_lex(Lex(LEX_NUM), pl1);
				prog.put_lex(Lex(LEX_NUM, 1));
				prog.put_lex(Lex(LEX_PLUSD));
				gl();
			}else if(c_type == LEX_DEC){
				prog.put_lex(Lex(POLIZ_ADDRESS, prev_val), pl0);
				prog.put_lex(Lex(LEX_NUM), pl1);
				prog.put_lex(Lex(LEX_NUM, 1));
				prog.put_lex(Lex(LEX_MINUSD));
				gl();
			}else EXPRESSION();
		}else if(c_type == LEX_DEF){
			gl();
		}else EXPRESSION();
	}else{
		if(flag != 0){
			prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
			prog.put_lex(Lex(LEX_NUM, 1));
			if(flag == -1) prog.put_lex(Lex(LEX_MINUSD));
			else if(flag == 1) prog.put_lex(Lex(LEX_PLUSD));
		}
		gl();
		if(is_def_operator()){
			prog.put_lex(Lex(POLIZ_ADDRESS, prev_val));
			var_type = TID[prev_val].get_type();
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
	if(c_type == LEX_IDENT)	prog.put_lex(Lex(KEY_READ, c_val));
	else throw(cur_lex);
	gl();
	if(c_type != LEX_RPAR) throw(cur_lex);
	gl();
}

void Parser::WRITE(){
	gl();
	if(c_type != LEX_LPAR) throw(cur_lex);
	gl();
	while(c_type != LEX_RPAR){
		prog.put_lex(Lex(KEY_WRITE));
		if(c_type == OBJ_ENV){
			ENV();
		}else{
			prog.put_lex(cur_lex);
			gl();
		}
		if(c_type == LEX_LSQB){
			gl();
			EXPRESSION();
			prog.put_lex(Lex(POLIZ_EXPR));
			if(c_type != LEX_RSQB) throw(cur_lex);
			gl();
		}
		if(c_type == LEX_RPAR) break;
		else if(c_type != LEX_COMMA) throw(cur_lex);
		gl();
	}
	gl();
}

void Parser::ENV(){
	prog.put_lex(Lex(OBJ_ENV));
	gl();
	if(c_type != LEX_DOT) throw(cur_lex);
	gl();
	string action = TID[c_val].get_name();
	prog.put_lex(Lex(LEX_STR, action));
	TID.erase(--TID.end());
	gl();
}

void Parser::DOT(){
	prog.put_lex(Lex(LEX_DOT));
	gl();
	string action = TID[c_val].get_name();
	prog.put_lex(Lex(LEX_STR, action));
	if(action == "toString"){
		var_type = LEX_STR;
		gl();
		gl();
	}else if(action == "MAX_VALUE") var_type = LEX_NUM;
	else if(action == "MIN_VALUE") var_type = LEX_NUM;
	else if(action == "length") var_type = LEX_NUM;
	TID.erase(--TID.end());
	gl();
}

void Parser::change_type(type_of_lex type, int index){
	prog.put_lex(Lex(POLIZ_CHANGE));
	prog.put_lex(Lex(type, index));
	TID[index].put_type(type);
}

void Parser::change_type(type_of_lex type, int index, int offset){
	prog.put_lex(Lex(POLIZ_CHANGE));
	int i = TID[index][offset];
	prog.put_lex(Lex(type, i));
	TID[i].put_type(type);
}
