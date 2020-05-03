#pragma once
#include "scanner.h"
#include "stack.h"

class Poliz{
	Lex *p;
	int size;
	int free;
public:
	Poliz(int max_size){
		p = new Lex[size = max_size];
		free = 0;
	}
	~Poliz(){
		delete []p;
	}
	void put_lex(Lex l){
		p[free] = l;
		free++;
	}
	void put_lex(Lex l, int place){
		p[place] = l;
	}
	void pop(){
		if(free > 0) free--;
	}
	void blank(){
		free++;
	}
	int get_free(){
		return free;
	}
	Lex& operator[](int index){
		if(index > size) throw "POLIZ: out of array";
		else if(index > free) throw "POLIZ: indefinite element of array";
		else return p[index];
	}
	void print(){
		for(int i = 0; i < free; ++i){
			std::cout << p[i] << std::endl;
		}
	}
};

int transform(int, int);

class Parser{
	Lex cur_lex;
	type_of_lex c_type, var_type = LEX_NULL;
	int c_val;
	bool l_value;
	Scanner scan;
	Stack <int, 128> st_int;
	Stack <type_of_lex, 128> st_lex;
	
	void S(); void SENTENCE(); void FUNC_DEF(); void OPERATOR();
	void BLOCK(); void VAR_DEF(); void COND_OP(); void LOOP_OP();
	void TRANSIT_OP(); void EXPR_OP(); void EXPRESSION();
	void E1(); void E2(); void E3(); void D();
	void OPID(int); void WRITE(); void READ(); void ENV();
	void change_type(type_of_lex, int);
	void change_type(type_of_lex, int, int);
	
	bool is_expression(type_of_lex param){
		return (param == LEX_IDENT) || (param == LEX_NUM) ||
			(param == LEX_STR) ||
			(param == KEY_TRUE) || (param == KEY_FALSE) ||
			(param == LEX_INC) || (param == LEX_DEC);
	}
	bool is_def_operator(){
		return (c_type == LEX_DEF) || (c_type == LEX_PLUSD) || (c_type == LEX_MINUSD) ||
			(c_type == LEX_MULD) || (c_type == LEX_DIVD) || (c_type == LEX_MODD);
	}
	bool is_unary_operator(type_of_lex param){
		return (param == LEX_INC) || (param == LEX_DEC);
	}
	bool is_binary_operator(type_of_lex param){
		return (param == LEX_AND) || (param == LEX_OR) || (param == LEX_DEF) ||
				(param == LEX_DIV) || (param == LEX_DIVD) ||
				(param == LEX_EQ) || (param == LEX_EQ2) ||
				(param == LEX_GEQ) || (param == LEX_GRT) ||
				(param == LEX_LEQ) || (param == LEX_LESS) ||
				(param == LEX_PLUS) || (param == LEX_PLUSD) ||
				(param == LEX_MINUS) || (param == LEX_MINUSD) ||
				(param == LEX_MOD) || (param == LEX_MODD) ||
				(param == LEX_MUL) || (param == LEX_MULD) ||
				(param == LEX_NEQ) || (param == LEX_NEQ2);
	}
	bool is_operand(type_of_lex param){
		return (param == LEX_IDENT) || (param == LEX_NUM) || (param == LEX_STR) ||
				(param == KEY_TRUE) || (param == KEY_FALSE);
	}
	void dec(type_of_lex);
	void dec(type_of_lex, int);
	void check_ident();
	void check_op();
	void check_not();
	void eq_type();
	void eq_bool();
	void check_id_in_read();
	
	void gl(){
		cur_lex = scan.get_lex();
		c_type = cur_lex.get_type();
		c_val = cur_lex.get_value();
	}
public:
	vector <Lex> poliz;
	Poliz prog;
	Parser(const char* program): scan(program), prog(1000) {}
	void analyze();
};
