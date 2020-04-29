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
using std::string;
using std::vector;
using std::ostream;

int main(int argc, char* argv[]){
	if(argc < 2){
		std::cerr << "Введите файл с программой" << std::endl;
		exit(1);
	}
	
	Scanner scan(argv[1]);
	try{
		Lex cur_lex = LEX_NULL;
		while(cur_lex.get_type() != LEX_FIN){
			cur_lex = scan.get_lex();
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
	}catch(const char* s){
		cout << s << endl;
		exit(4);
	}
	
	/*for(vector<Ident>::iterator it = TID.begin(); it != TID.end(); it++){
		cout << "var " << (*it).get_name() << " = " << (*it).get_value() << ", type: " << (*it).get_type() << endl;
	}*/
	return 0;
}
