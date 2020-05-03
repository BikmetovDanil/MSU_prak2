#include "scanner.h"

using namespace std;

vector <Ident> TID;

int put(const string & buf){
	vector<Ident> :: iterator k;
	if((k = find(TID.begin(), TID.end(), buf)) != TID.end()){
		return k - TID.begin();
	}
	TID.push_back(Ident(buf));
	return TID.size() - 1;
}

int Ident::operator[](int index){
	int size = elem.size() - 1;
	while(index > size){
		Ident ident;
		ident.put_type(LEX_UNDEF);
		ident.put_declare();
		TID.push_back(ident);
		elem.push_back(TID.size() - 1);
		size++;
	}
	return elem[index];
}

Scanner::Scanner(const char* program){
	fp = fopen(program, "r");
	TID.reserve(1000); // Very important :)
	if(fp == NULL){
		std::cerr << "Не удалось открыть программу" << std::endl;
		exit(1);
	}
}

string Scanner::TW[] = {"", "do", "while", "for", "in", "var",
					"function", "break", "continue", "return",
					"if", "else", "true", "false", "read", "write",
					"Response", "Environment"};
					
type_of_lex Scanner::kwrd[] = {LEX_NULL, KEY_DO, KEY_WHILE, KEY_FOR, KEY_IN, KEY_VAR,
					KEY_FUNC, KEY_BREAK, KEY_CONT, KEY_RET,	KEY_IF, KEY_ELSE, 
					KEY_TRUE, KEY_FALSE, KEY_READ, KEY_WRITE, OBJ_RESP, OBJ_ENV};
					
string Scanner::TD[] = {"", ";", ",", ".", ":", "(", ")",
					"=", "<", ">", "+", "-", "*", "/", "<=", ">=",
					"==", "!=", "===", "!==", "+=", "++", "-=", "--",
					"*=", "/=", "%", "%=", "&&", "||", "!", "[", "]"};
					
type_of_lex Scanner::dlms[] = {LEX_NULL, LEX_SEMICOLON, LEX_COMMA, LEX_DOT, LEX_COLON, LEX_LPAR, LEX_RPAR,
					LEX_DEF, LEX_LESS, LEX_GRT, LEX_PLUS, LEX_MINUS, LEX_MUL, LEX_DIV, LEX_LEQ, LEX_GEQ,
					LEX_EQ, LEX_NEQ, LEX_EQ2, LEX_NEQ2, LEX_PLUSD, LEX_INC, LEX_MINUSD, LEX_DEC,
					LEX_MULD, LEX_DIVD, LEX_MOD, LEX_MODD, LEX_AND, LEX_OR, LEX_NOT, LEX_LSQB, LEX_RSQB};
					
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
					if(j > 0){
						return Lex(kwrd[j], j);
					}else{
						j = put(buf);
						return Lex(LEX_IDENT, j);
					}
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
				}else if(c == EOF){
					CS = ERR;
				}else if(c == '\''){
					return Lex(LEX_STR, buf);
				}else{
					buf.push_back(c);
					gc();
				}
				break;
			case STR2:
				if(c == '\\'){
					gc();
					CS = ECR2;
				}else if(c == EOF){
					CS = ERR;
				}else if(c == '\"'){
					return Lex(LEX_STR, buf);
				}else{
					buf.push_back(c);
					gc();
				}
				break;
			case ECR1:
				if(c == '\\'){
					buf.push_back('\\');
					gc();
					CS = STR1;
				}else if(c == EOF){
					CS = ERR;
				}else if(c == '\''){
					buf.push_back('\'');
					gc();
					CS = STR1;
				}else if(c == 'n'){
					buf.push_back('\n');
					gc();
					CS = STR1;
				}else{
					CS = ERR;
				}
				break;
			case ECR2:
				if(c == '\\'){
					buf.push_back('\\');
					gc();
					CS = STR2;
				}else if(c == EOF){
					CS = ERR;
				}else if(c == '\"'){
					buf.push_back('\"');
					gc();
					CS = STR2;
				}else if(c == 'n'){
					buf.push_back('\n');
					gc();
					CS = STR2;
				}else{
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
