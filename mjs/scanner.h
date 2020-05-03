#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

using namespace std;
using std::string;
using std::vector;
using std::ostream;

enum type_of_lex{
	LEX_NULL, // 0
	LEX_END, LEX_BEGIN, LEX_WRITE, LEX_LPAR, LEX_RPAR, LEX_DEF, LEX_AND, LEX_OR, // 1-8
	LEX_FIN, LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_DOT, // 9-13
	LEX_LESS, LEX_GRT,	LEX_EQ, LEX_INC, LEX_DEC, LEX_PLUS, LEX_MINUS, // 14-20
	LEX_MUL, LEX_DIV, LEX_PLUSD, LEX_MINUSD, LEX_MULD, LEX_DIVD, // 21-26
	LEX_MOD, LEX_MODD, LEX_NEQ, LEX_GEQ, LEX_LEQ, LEX_EQ2, LEX_NEQ2, // 27-33
	LEX_UNDEF, LEX_BOOL, LEX_NUM, LEX_OBJ, LEX_IDENT, LEX_STR, LEX_NOT, // 34-40
	LEX_ARR, LEX_LSQB, LEX_RSQB, // 41-43
	KEY_DO, KEY_WHILE, KEY_FOR, KEY_IN, KEY_VAR, // 44-48
	KEY_FUNC, KEY_BREAK, KEY_CONT, KEY_RET, // 49-52
	KEY_IF, KEY_ELSE, KEY_TRUE, KEY_FALSE, KEY_READ, KEY_WRITE, // 53-58
	OBJ_RESP, OBJ_ENV, // 59-60
	POLIZ_LABEL, POLIZ_ADDRESS, POLIZ_GO, POLIZ_FGO, POLIZ_CHANGE, POLIZ_EXPR // 61-66
};

class Lex{
	type_of_lex t_lex;
	int v_lex;
	string n_lex;
public:
	Lex(type_of_lex t = LEX_NULL){
		t_lex = t;
		v_lex = 0;
	}
	Lex(type_of_lex t, int v){
		t_lex = t;
		v_lex = v;
	}
	Lex(type_of_lex t, string s){
		t_lex = t;
		v_lex = 0;
		n_lex = s;
	}
	type_of_lex get_type() const{
		return t_lex;
	}
	int get_value() const{
		return v_lex;
	}
	string get_name() const{
		return n_lex;
	}
	friend ostream& operator<<(ostream & s, Lex l){
		s << '(' << l.t_lex << ',' << l.v_lex << ");" ;
		return s;
	}
};

class Ident{
	string name;
	bool declare, assign;
	type_of_lex type;
	int value;
public:
	vector <int> elem;
	Ident(){
		elem.clear();
		declare = false;
		assign = false;
	}
	Ident(const string n){
		name = n;
		elem.clear();
		declare = false;
		assign = false;
	}
	bool operator==(const string& s) const{
		return name == s;
	}
	int operator[](int);
	void put_name(string n){
		name = n;
	}
	string get_name(){
		return name;
	}
	bool get_declare (){
		return declare;
	}
	void put_declare (){
		declare = true;
	}
	type_of_lex get_type(){
		return type;
	}
	void put_type (type_of_lex t){
		type = t;
	}
	bool get_assign(){
		return assign;
	}
	void put_assign(){
		assign = true;
	}
	int get_value(){
		return value;
	}
	void put_value(int v){
		value = v;
	}
};

int put(const string & buf);

class Scanner{
	FILE *fp;
	int c;
	int look(const string & buf, string* list){
		int i = 1;
		while(list[i] != ""){
			if(buf == list[i]) return i;
			i++;
		}
		return 0;
	}
	void gc(){
		c = fgetc(fp);
	}
public:
	static string TW[], TD[];
	static type_of_lex dlms[], kwrd[];
	Scanner(const char*);
	Lex get_lex();
};
