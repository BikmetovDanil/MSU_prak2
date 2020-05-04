#include "parser.h"

using namespace std;
extern vector<Ident> TID;

struct Arg{
	type_of_lex type;
	int value;
	string name;
	Arg(){
		type = LEX_NULL;
		value = 0;
		name.clear();
	}
	Arg(type_of_lex t, int v){
		type = t;
		value = v;
		name = to_string(v);
	}
	Arg(type_of_lex t, string n){
		type = t;
		value = 0;
		name = n;
	}
	Arg(type_of_lex t, int v, string n){
		type = t;
		value = v;
		name = n;
	}
};

class Executer{
	Lex pc_el;
	int index, size;
	Arg get_expr(Poliz&);
public:
	void execute(Poliz& prog);
};
