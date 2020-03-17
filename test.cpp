#include <iostream>
using namespace std;

struct B{
	int b;
	B(int x){
		b = x;
	}
	virtual void func(){
		cout << "f()_B\n";
	}
	virtual void h(double) = 0;
};

struct T{
	int a;
	T(int x = 1){
		a = x;
	}
};

struct D: public B{
	D(): B(0) {}
	virtual void func(){
		cout << "f()_D\n";
	}
	void h(double d){}
};

struct C: public B{
	C(): B(0) {}
	void h(double d){}
};

struct P{
	T * f;
	P(T * a){
		if(a != NULL) f = a;
		else f = new T(1);
	}
	T & operator*(){
		return *f;
	}
};

int main(){
	D d;
	C c;
	B *bp = &d, *bp1 = &c;
	bp -> func();
	bp1 -> func();
	T t;
	P p(&t);
	(*p).a = 28;
	cout << t.a << endl;
	P p1(NULL);
	cout << (*p1).a << endl;
	return 0;
}
