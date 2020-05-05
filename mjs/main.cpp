#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "executer.h"

using namespace std;
using std::string;
using std::vector;
using std::ostream;

int transform(int, int);
bool write_poliz;

class Interpretator{
	Parser pars;
	Executer E;
public:
	Interpretator(char* program): pars(program) {};
	void interpretate();
};

void Interpretator::interpretate(){
	pars.analyze();
	if(write_poliz) pars.prog.print();
	cout << "Выполняю программу" << endl;
	E.execute(pars.prog);
}

int main(int argc, char* argv[]){
	if(argc < 2){
		std::cerr << "Введите файл с программой" << std::endl;
		exit(1);
	}
	if(argc > 2 && !strcmp(argv[2], "poliz")) write_poliz = true;
	else write_poliz = false;

	Interpretator I(argv[1]);
	try{
		I.interpretate();
	}catch(int c){
		if(c == EOF) cout << "Ошибка: неожиданный конец файла" << endl;
		else cout << "Ошибка в символе: " << (char)c << endl;
		exit(2);
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

int transform(int val, int offset){
	return (offset << 16) + val;
}
