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

enum type_of_lex{
	LEX_NULL,
	LEX_AND, LEX_BEGIN, LEX_WRITE, //... 
	LEX_FIN,
	LEX_SEMICOLON, LEX_COMMA, LEX_STR, LEX_GEQ, //... 
	LEX_NUM,
	LEX_IDENT,
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
	int get_value ( ) const{
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
	int look(const string & buf, string* list) {
		int i = 0;
		while(list[i] != "") {
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
	Scanner(const char* program){
		fp = fopen(program, "r");
		if(fp == NULL){
			cerr << "Не удалось открыть программу" << endl;
			exit(1);
		}
	}
	Lex get_lex();
};

string Scanner::TW[] = {"", "and", "begin", "bool", "do", "else", "end",
					"if", "false", "int", "not", "or", "program", "read",
					"then", "true", "var", "while", "write"};
					
string Scanner::TD[] = {"", ";", "@", ",", ":", ":=", "(", ")",
					"=", "<", ">", "+", "-", "*", "/", "<=", ">="};
					
type_of_lex dlms[25];
					
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
					buf.push_back(c);
					CS = IDENT;
				}else if(isdigit(c)){
					d = 0;
					buf.push_back(c);
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
				}else if(c == '<' || c == '>' || c == '*' || c == '%'){
					buf.push_back(c);
					gc();
					CS = AL1;
				}else if(c == '=' || c == '!'){
					buf.push_back(c);
					gc();
					CS = AL2;
				}else if(c == '{'){
					gc();
					Lex(LEX_BEGIN);
					CS = H;
				}else if(c == '}'){
					gc();
					Lex(LEX_END);
					CS = H;
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
					Lex(LEX_IDENT, buf);
					CS = H;
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
					Lex(LEX_NUM, d);
					CS = H;
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
					Lex(LEX_NUM, d);
					CS = H;
				}
				break;
			case E:
				if(c == '-'){
					s = 0;
					CS = EMINUS;
				}else if(isdigit(c)){
					s = 0;
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
					Lex(LEX_NUM, d);
					CS = H;
				}
				break;
			case EMINUS:
				if(isdigit(c)){
					s = s*10 + (c-'0');
					gc();
					CS = EMINUS;
				}else{
					for(int i = 0; i < s; i++) d /= 10;
					Lex(LEX_NUM, d);
					CS = H;
				}
				break;
			case STR1:
				if(c == '\\'){
					gc();
					CS = ECR1;
				}
				else if(c == '@'){
					CS = ERR;			
				}
				else if(c == '\''){
					gc();
					Lex(LEX_STR, buf);
					CS = H;
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
					gc();
					Lex(LEX_STR, buf);
					CS = H;
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
					Lex(dlms[j], j);
					gc();
					CS = H;
				}else{
					j = look(buf, TD);
					Lex(dlms[j], j);
					gc();
					CS = H;
				}
				break;
			case MINUS:
				if(c == '=' || c == '-'){
					buf.push_back(c);
					j = look(buf, TD);
					Lex(dlms[j], j);
					gc();
					CS = H;
				}else{
					j = look(buf, TD);
					Lex(dlms[j], j);
					gc();
					CS = H;
				}
				break;
			case AMP:
				if(c == '&'){
					buf.push_back(c);
					j = look(buf, TD);
					Lex(dlms[j], j);
					gc();
					CS = H;
				}else{
					CS = ERR;
				}
				break;
			case STICK:
				if(c == '|'){
					buf.push_back(c);
					j = look(buf, TD);
					Lex(dlms[j], j);
					gc();
					CS = H;
				}else{
					CS = ERR;
				}
				break;
			case SLASH:
				if(c == '='){
					buf.push_back(c);
					j = look(buf, TD);
					Lex(dlms[j], j);
					gc();
					CS = H;
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
					Lex(dlms[j], j);
					gc();
					CS = H;
				}
				break;
			case COM1:
				if(c == '\n'){
					gc();
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
					gc();
					CS = H;
				}else{
					CS = COM2;
				}
				break;
			case AL1:
				if(c == '='){
					buf.push_back(c);
					j = look(buf, TD);
					Lex(dlms[j], j);
					gc();
					CS = H;
				}else{
					j = look(buf, TD);
					Lex(dlms[j], j);
					CS = H;
				}
				break;
			case AL2:
				if(c == '='){
					buf.push_back(c);
					gc();
					CS = AL3;
				}else{
					j = look(buf, TD);
					Lex(dlms[j], j);
					CS = H;
				}
				break;
			case AL3:
				if(c == '='){
					buf.push_back(c);
					gc();
					CS = AL3;
				}else{
					j = look(buf, TD);
					Lex(dlms[j], j);
					CS = H;
				}
				break;
			case OTHER:
				if(c == ':' || c == ',' || c == '(' || c == ')' || c == '[' || c == ']' || c == '.'){
					buf.push_back(c);
					gc();
					j = look(buf, TD);
					Lex(dlms[j], j);
					CS = H;
				}else{
					CS = ERR;
				}
				break;
			case ERR:
				throw(c);
				break;
			case FIN:
				cout << "Программа верна" << endl;
				exit(0);
		}
	}
}

int main(int argc, char* argv[]){
	if(argc < 2){
		cerr << "Введите файл с программой" << endl;
		exit(1);
	}
	Scanner scan(argv[1]);
	try{
		scan.get_lex();
	}catch(int c){
		if(c == EOF) cout << "Ошибка: неожиданный конец файла" << endl;
		else cout << "Ошибка в символе: " << (int)c << endl;
		exit(2);
	}
	return 0;
}
