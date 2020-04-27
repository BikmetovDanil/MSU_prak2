#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <stack>
#include <algorithm>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

using namespace std;

enum type_of_lex{
	LEX_NULL,
	LEX_END, LEX_BEGIN, LEX_WRITE, LEX_LPAR, LEX_RPAR, LEX_DEF, LEX_AND, LEX_OR,
	LEX_FIN, LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_STR,
	LEX_LESS, LEX_GRT,	LEX_EQ, LEX_INC, LEX_DEC, LEX_PLUS, LEX_MINUS,
	LEX_MUL, LEX_DIV, LEX_PLUSD, LEX_MINUSD, LEX_MULD, LEX_DIVD,
	LEX_MOD, LEX_MODD, LEX_LEX_EQ, LEX_NEQ, LEX_GEQ, LEX_LEQ, LEX_EQ2, LEX_NEQ2,
	LEX_NUM, LEX_IDENT,
	KEY_DO, KEY_WHILE, KEY_FOR, KEY_IN, KEY_VAR,
	KEY_FUNC, KEY_BREAK, KEY_CONT, KEY_RET,
	KEY_IF, KEY_ELSE, KEY_TRUE, KEY_FALSE, KEY_READ, KEY_WRITE,
	POLIZ_LABEL,
	POLIZ_ADDRESS,
	POLIZ_GO,
	POLIZ_FGO
};

class Lex{
	type_of_lex t_lex;
	int v_lex;
public:
	Lex(type_of_lex t = LEX_NULL){
		t_lex = t;
	}
	Lex(type_of_lex t, int v){
		t_lex = t;
		v_lex = v;
	}
	Lex(type_of_lex t, string s){
		t_lex = t;
		v_lex = 0; // Временно
	}
	type_of_lex get_type() const{
		return t_lex;
	}
	int get_value() const{
		return v_lex;
	}
	friend ostream& operator<<(ostream & s, Lex l){
		s << '(' << l.t_lex << ',' << l.v_lex << ");" ;
		return s;
	}
};

class Ident{
	string name;
	bool declare;
	type_of_lex type;
	bool assign;
	int value;
public:
	Ident(){
		declare = false;
		assign = false;
	}
	Ident(const string n){
		name = n;
		declare = false;
		assign = false;
	}
	bool operator==(const string& s) const{
		return name == s;
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

vector <Ident> TID;

int put(const string & buf){
	vector<Ident> :: iterator k;
	if((k = find(TID.begin(), TID.end(), buf)) != TID.end()){
		return k - TID.begin();
	}
	TID.push_back(Ident(buf));
	return TID.size() - 1;
}

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
	Scanner(const char* program){
		fp = fopen(program, "r");
		if(fp == NULL){
			cerr << "Не удалось открыть программу" << endl;
			exit(1);
		}
	}
	Lex get_lex();
};

string Scanner::TW[] = {"", "do", "while", "for", "in", "var",
					"function", "break", "continue", "return",
					"if", "else", "true", "false", "read", "write"};
					
type_of_lex Scanner::kwrd[] = {LEX_NULL, KEY_DO, KEY_WHILE, KEY_FOR, KEY_IN, KEY_VAR,
					KEY_FUNC, KEY_BREAK, KEY_CONT, KEY_RET,
					KEY_IF, KEY_ELSE, KEY_TRUE, KEY_FALSE, KEY_READ, KEY_WRITE};
					
string Scanner::TD[] = {"", ";", ",", ":", "(", ")",
					"=", "<", ">", "+", "-", "*", "/", "<=", ">=",
					"==", "!=", "===", "!==", "+=", "++", "-=", "--",
					"*=", "/=", "%", "%=", "&&", "||"};
					
type_of_lex Scanner::dlms[] = {LEX_NULL, LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_LPAR, LEX_RPAR,
					LEX_DEF, LEX_LESS, LEX_GRT, LEX_PLUS, LEX_MINUS, LEX_MUL, LEX_DIV, LEX_LEQ, LEX_GEQ,
					LEX_EQ, LEX_NEQ, LEX_EQ2, LEX_NEQ2, LEX_PLUSD, LEX_INC, LEX_MINUSD, LEX_DEC,
					LEX_MULD, LEX_DIVD, LEX_MOD, LEX_MODD, LEX_AND, LEX_OR};
					
Lex Scanner::get_lex(){
	enum state{H, ERR, IDENT, NUMB, FLOAT, E, EPLUS, EMINUS, STR1, STR2, ECR1, ECR2, PLUS, MINUS,
				AMP, STICK, SLASH, COM1, COM2, COM3, AL1, AL2, AL3, OTHER, FIN};
	state CS = H;
	string buf;
	int n, s, j;
	double d;
	while(true){
		switch(CS){
			case H:
				gc();
				if(c == ' ' || c == '\n' || c == '\r' || c == '\t'){
					CS = H;
				}else if(isalpha(c) || c == '_'){
					CS = IDENT;
				}else if(isdigit(c)){
					d = 0;
					CS = NUMB;
				}else if(c == '\''){
					gc();
					CS = STR1;
				}else if(c == '\"'){
					gc();
					CS = STR2;
				}else if(c == '+'){
					buf.push_back(c);
					gc();
					CS = PLUS;
				}else if(c == '-'){
					buf.push_back(c);
					gc();
					CS = MINUS;
				}else if(c == '&'){
					buf.push_back(c);
					gc();
					CS = AMP;
				}else if(c == '|'){
					buf.push_back(c);
					gc();
					CS = STICK;
				}else if(c == '/'){
					buf.push_back(c);
					gc();
					CS = SLASH;
				}else if(c == '#'){
					CS = COM1;
					gc();
				}else if(c == '<' || c == '>' || c == '*' || c == '%'){
					buf.push_back(c);
					gc();
					CS = AL1;
				}else if(c == '=' || c == '!'){
					buf.push_back(c);
					gc();
					CS = AL2;
				}else if(c == '{'){
					return Lex(LEX_BEGIN);
				}else if(c == '}'){
					return Lex(LEX_END);
				}else if(c == EOF){
					CS = FIN;
				}else{
					CS = OTHER;
				}
				break;
			case IDENT:
				if(isalpha(c) || isdigit(c) || c == '_'){
					buf.push_back(c);
					gc();
					CS = IDENT;
				}else{
					fseek(fp, -1, SEEK_CUR);
					j = look(buf, TW);
					if(j > 0) return Lex(kwrd[j], j);
					return Lex(LEX_IDENT, buf);
				}
				break;
			case NUMB:
				if(isdigit(c)){
					d = d*10 + (c-'0');
					gc();
					CS = NUMB;
				}else if(c == '.'){
					gc();
					n = 10;
					CS = FLOAT;
				}else if(c == 'e' || c == 'E'){
					gc();
					CS = E;
				}else{
					fseek(fp, -1, SEEK_CUR);
					return Lex(LEX_NUM, d);
				}
				break;
			case FLOAT:
				if(isdigit(c)){
					d = d + (c-'0')/n;
					n *= 10;
					gc();
					CS = FLOAT;
				}else if(c == 'e' || c == 'E'){
					gc();
					CS = E;
				}else{
					fseek(fp, -1, SEEK_CUR);
					return Lex(LEX_NUM, d);
				}
				break;
			case E:
				s = 0;
				if(c == '-'){
					gc();
					if(isdigit(c)) CS = EMINUS;
					else CS = ERR;
				}else if(isdigit(c)){
					CS = EPLUS;
				}else{
					CS = ERR;
				}
				break;
			case EPLUS:
				if(isdigit(c)){
					s = s*10 + (c-'0');
					gc();
					CS = EPLUS;
				}else{
					for(int i = 0; i < s; i++) d *= 10;
					fseek(fp, -1, SEEK_CUR);
					return Lex(LEX_NUM, d);
				}
				break;
			case EMINUS:
				if(isdigit(c)){
					s = s*10 + (c-'0');
					gc();
					CS = EMINUS;
				}else{
					for(int i = 0; i < s; i++) d /= 10;
					fseek(fp, -1, SEEK_CUR);
					return Lex(LEX_NUM, d);
				}
				break;
			case STR1:
				if(c == '\\'){
					gc();
					CS = ECR1;
				}
				else if(c == EOF){
					CS = ERR;
				}
				else if(c == '\''){
					return Lex(LEX_STR, buf);
				}
				else{
					gc();
					buf.push_back(c);
				}
				break;
			case STR2:
				if(c == '\\'){
					gc();
					CS = ECR2;
				}
				else if(c == EOF){
					CS = ERR;
				}
				else if(c == '\"'){
					return Lex(LEX_STR, buf);
				}
				else{
					gc();
					buf.push_back(c);
				}
				break;
			case ECR1:
				if(c == '\\'){
					gc();
					buf.push_back('\\');
					CS = STR1;
				}
				else if(c == EOF){
					CS = ERR;
				}
				else if(c == '\''){
					gc();
					buf.push_back('\'');
					CS = STR1;
				}
				else{
					CS = ERR;
				}
				break;
			case ECR2:
				if(c == '\\'){
					gc();
					buf.push_back('\\');
					CS = STR2;
				}
				else if(c == EOF){
					CS = ERR;
				}
				else if(c == '\"'){
					gc();
					buf.push_back('\"');
					CS = STR2;
				}
				else{
					CS = ERR;
				}
				break;
			case PLUS:
				if(c == '=' || c == '+'){
					buf.push_back(c);
					j = look(buf, TD);
					return Lex(dlms[j], j);
				}else{
					j = look(buf, TD);
					fseek(fp, -1, SEEK_CUR);
					return Lex(dlms[j], j);
				}
				break;
			case MINUS:
				if(c == '=' || c == '-'){
					buf.push_back(c);
					j = look(buf, TD);
					return Lex(dlms[j], j);
				}else{
					j = look(buf, TD);
					fseek(fp, -1, SEEK_CUR);
					return Lex(dlms[j], j);
				}
				break;
			case AMP:
				if(c == '&'){
					buf.push_back(c);
					j = look(buf, TD);
					return Lex(dlms[j], j);
				}else{
					CS = ERR;
				}
				break;
			case STICK:
				if(c == '|'){
					buf.push_back(c);
					j = look(buf, TD);
					return Lex(dlms[j], j);
				}else{
					CS = ERR;
				}
				break;
			case SLASH:
				if(c == '='){
					buf.push_back(c);
					j = look(buf, TD);
					return Lex(dlms[j], j);
				}else if(c == '/'){
					buf.pop_back();
					gc();
					CS = COM1;
				}else if(c == '*'){
					buf.pop_back();
					gc();
					CS = COM2;
				}else{
					j = look(buf, TD);
					fseek(fp, -1, SEEK_CUR);
					return Lex(dlms[j], j);
				}
				break;
			case COM1:
				if(c == '\n'){
					CS = H;
				}else{
					gc();
					CS = COM1;
				}
				break;
			case COM2:
				if(c == '*'){
					gc();
					CS = COM3;
				}else if(c == EOF){
					CS = ERR;
				}else{
					gc();
					CS = COM2;
				}
				break;
			case COM3:
				if(c == '/'){
					CS = H;
				}else{
					CS = COM2;
				}
				break;
			case AL1:
				if(c == '='){
					buf.push_back(c);
					j = look(buf, TD);
					return Lex(dlms[j], j);
				}else{
					j = look(buf, TD);
					fseek(fp, -1, SEEK_CUR);
					return Lex(dlms[j], j);
				}
				break;
			case AL2:
				if(c == '='){
					buf.push_back(c);
					gc();
					CS = AL3;
				}else{
					j = look(buf, TD);
					fseek(fp, -1, SEEK_CUR);
					return Lex(dlms[j], j);
				}
				break;
			case AL3:
				if(c == '='){
					buf.push_back(c);
					CS = AL3;
				}else{
					j = look(buf, TD);
					fseek(fp, -1, SEEK_CUR);
					return Lex(dlms[j], j);
				}
				break;
			case OTHER:
				if(c == ':' || c == ',' || c == '(' || c == ')' || c == '[' || c == ']' || c == '.' || c == ';'){
					buf.push_back(c);
					j = look(buf, TD);
					return Lex(dlms[j], j);
				}else{
					CS = ERR;
				}
				break;
			case ERR:
				throw(c);
				break;
			case FIN:
				return Lex(LEX_FIN);
		}
	}
}

class Parser{
	Lex cur_lex;
	type_of_lex c_type;
	int c_val;
	Scanner scan;
	stack <int> st_int;
	stack <type_of_lex> st_lex;
	void S();
	void SENTENCE();
	void FUNC_DEF();
	void OPERATOR();
	void BLOCK();
	void VAR_DEF();
	void COND_OP();
	void LOOP_OP();
	void TRANSIT_OP();
	void EXPR_OP();
	void gl(){
		cur_lex = scan.get_lex();
		c_type = cur_lex.get_type();
		c_val = cur_lex.get_value();
	}
	bool is_expression(type_of_lex param){
		return (param == LEX_IDENT) || (param == LEX_NUM) ||
			(param == LEX_STR) ||
			(param == KEY_TRUE) || (param == KEY_FALSE) ||
			(param == LEX_INC) || (param == LEX_DEC);
	}
public:
	vector <Lex> poliz;
	Parser(const char* program): scan(program){}
	void analyze();
};

void Parser::analyze(){
	S();
	if(c_type != LEX_FIN) throw(cur_lex);
}

void Parser::S(){
	gl();
	if(c_type != LEX_FIN){
		SENTENCE();
		S();
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
	else if(is_expression(c_type)) EXPR_OP();
	else throw(cur_lex);
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
	do{
		gl();
		if(c_type != LEX_IDENT) throw(cur_lex);
		gl();
		if(c_type == LEX_DEF){
			gl();
			if(is_expression(c_type)) EXPR_OP();
			else throw(cur_lex);
		}
	}while(c_type == LEX_COMMA);
	if(c_type != LEX_SEMICOLON) throw(cur_lex);
	gl();
}

void Parser::COND_OP(){
	gl();
	if(c_type != LEX_LPAR) throw(cur_lex);
	gl();
	if(is_expression(c_type)) EXPR_OP();
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
		if(is_expression(c_type)) EXPR_OP();
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
		if(is_expression(c_type)) EXPR_OP();
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
			if(is_expression(c_type)) EXPR_OP();
			else if(c_type != LEX_SEMICOLON) throw(cur_lex);
			gl();
			if(is_expression(c_type)) EXPR_OP();
			else if(c_type != LEX_SEMICOLON) throw(cur_lex);
			gl();
			if(is_expression(c_type)) EXPR_OP();
			if(c_type != LEX_RPAR) throw(cur_lex);
			gl();
			OPERATOR();
		}else if(c_type == KEY_VAR){
			if(c_type == KEY_VAR) gl();
			gl();
			if(c_type != KEY_IN) throw(cur_lex);
			gl();
			if(is_expression(c_type)) EXPR_OP();
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
		if(is_expression(c_type)) EXPR_OP();
	}else gl();
	if(c_type != LEX_SEMICOLON) throw(cur_lex);
	gl();
}

void Parser::EXPR_OP(){
	// Сделать <expression> !!!
	gl();
}

int main(int argc, char* argv[]){
	if(argc < 2){
		cerr << "Введите файл с программой" << endl;
		exit(1);
	}
	
	Scanner scan(argv[1]);
	try{
		Lex cur_lex = LEX_NULL;
		while(cur_lex.get_type() != LEX_FIN){
			cur_lex = scan.get_lex();
			cout << cur_lex << endl;
		}
		cout << "Лексически программа верна" << endl;
	}catch(int c){
		if(c == EOF) cout << "Ошибка: неожиданный конец файла" << endl;
		else cout << "Ошибка в символе: " << (char)c << endl;
		exit(2);
	}
	
	Parser pars(argv[1]);
	try{
		pars.analyze();
		cout << "Синтаксически программа верна" << endl;
	}catch(Lex lex){
		cout << "Ошибка в лексеме: " << lex << endl;
		exit(3);
	}
	return 0;
}
