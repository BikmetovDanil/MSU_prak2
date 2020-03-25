#include <iostream>
#include <cmath>
#define CLEARSCREEN "\033[2J\033[1;1H"
#define PI 3.14159
using namespace std;

class Shape{
	double x, y;
	static int counter;
public:
	Shape(double x = 0, double y = 0){
		setPos(x, y);
		counter++;
	}
	virtual ~Shape(){
		counter--;
	}
	int getCounter(){
		return counter;
	}
	void setX(double x){
		this->x = x;
	}
	void setY(double y){
		this->y = y;
	}
	void setPos(double x, double y){
		setX(x);
		setY(y);
	}
	double getX() const{
		return x;
	}
	double getY() const{
		return y;
	}
	virtual void print(){
		cout << "Фигура: ("<< getX() << ", " << getY() << ")";
	}
	virtual double getArea() const = 0;
};
ostream& operator<<(ostream& out, const Shape& shape){
	cout << "Фигура: ("<< shape.getX() << ", " << shape.getY() << ")";
	return out;
}
int Shape::counter = 0;

class Rectangle: public Shape{
	double width, height;
public:
	Rectangle(double x = 0, double y = 0, double width = 1, double height = 1): Shape(x, y){
		setSize(width, height);
	}
	void setWidth(double width){
		this->width = width;
	}
	void setHeight(double height){
		this->height = height;
	}
	void setSize(double width, double height){
		setWidth(width);
		setHeight(height);
	}
	double getWidth() const{
		return width;
	}
	double getHeight() const{
		return height;
	}
	double getArea() const{
		return width*height;
	}
	void print(){
		cout << "Прямоугольник: ("<< getX() << ", " << getY() << ")";
		cout << " : [" << getWidth() << ", " << getHeight() << "]";
	}
	Rectangle& operator=(const Rectangle& rect){
		setPos(rect.getX(), rect.getY());
		setSize(rect.getWidth(), rect.getHeight());
		return *this;
	}
};
ostream& operator<<(ostream& out, const Rectangle& rect){
	cout << "Прямоугольник: ("<< rect.getX() << ", " << rect.getY() << ")";
	cout << " : [" << rect.getWidth() << ", " << rect.getHeight() << "]";
	return out;
}

class Circle: public Shape{
	double radius;
public:
	Circle(double x = 0, double y = 0, double radius = 1): Shape(x, y){
		setRadius(radius);
	}
	void setRadius(double radius){
		this->radius = radius;
	}
	double getRadius() const{
		return radius;
	}
	double getArea() const{
		return PI*radius*radius;
	}
	void print(){
		cout << "Круг: ("<< getX() << ", " << getY() << ")";
		cout << " : [" << getRadius() << "]";
	}
	Circle& operator=(const Circle& circ){
		setPos(circ.getX(), circ.getY());
		setRadius(circ.getRadius());
		return *this;
	}
};
ostream& operator<<(ostream& out, const Circle& circ){
	cout << "Круг: ("<< circ.getX() << ", " << circ.getY() << ")";
	cout << " : [" << circ.getRadius() << "]";
	return out;
}

class Square: public Shape{
	double side;
public:
	Square(double x = 0, double y = 0, double side = 1): Shape(x, y){
		setSide(side);
	}
	void setSide(double side){
		this->side = side;
	}
	double getSide() const{
		return side;
	}
	double getArea() const{
		return side*side;
	}
	void print(){
		cout << "Квадрат: ("<< getX() << ", " << getY() << ")";
		cout << " : [" << getSide() << "]";
	}
	Square& operator=(const Square& sqr){
		setPos(sqr.getX(), sqr.getY());
		setSide(sqr.getSide());
		return *this;
	}
};
ostream& operator<<(ostream& out, const Square& sqr){
	cout << "Квадрат: ("<< sqr.getX() << ", " << sqr.getY() << ")";
	cout << " : [" << sqr.getSide() << "]";
	return out;
}

class Triangle: public Shape{
	double a, b, c;
public:
	Triangle(double x = 0, double y = 0, double a = 1, double b = 1, double c = 1): Shape(x, y){
		setSize(a, b, c);
	}
	void setSize(double a, double b, double c){
		this->a = a;
		this->b = b;
		this->c = c;
	}
	double getA() const{
		return a;
	}
	double getB() const{
		return b;
	}
	double getC() const{
		return c;
	}
	double getPerimeter() const{
		return a + b + c;
	}
	double getArea() const{
		double p = getPerimeter()/2;
		return sqrt(p*(p-a)*(p-b)*(p-c));
	}
	void print(){
		cout << "Треугольник: ("<< getX() << ", " << getY() << ")";
		cout << " : [" << getA() << ", " << getB() << ", " << getC() << "]";
	}
	Triangle& operator=(const Triangle& tri){
		setPos(tri.getX(), tri.getY());
		setSize(tri.getA(), tri.getB(), tri.getC());
		return *this;
	}
};
ostream& operator<<(ostream& out, const Triangle& tri){
	cout << "Треугольник: ("<< tri.getX() << ", " << tri.getY() << ")";
	cout << " : [" << tri.getA() << ", " << tri.getB() << ", " << tri.getC() << "]";
	return out;
}

typedef Shape* Object;
class Scene{
	Object *obj;
	int size;
public:
	Scene(){
		obj = new Object[0];
		size = 0;
	}
	~Scene(){
		for(int i = 0; i < size; i++){
			delete obj[i];
		}
		delete [] obj;
	}
	void clear(){
		for(int i = 0; i < size; i++){
			delete obj[i];
		}
		delete [] obj;
		obj = new Object[0];
		size = 0;
	}
	void addObject(Rectangle rect){
		Object *newObj = new Object[size + 1];
		for(int i = 0; i < size; i++){
			newObj[i] = obj[i];
		}
		newObj[size] = new Rectangle(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
		delete [] obj;
		obj = newObj;
		size++;
	}
	void addObject(Circle circ){
		Object *newObj = new Object[size + 1];
		for(int i = 0; i < size; i++){
			newObj[i] = obj[i];
		}
		newObj[size] = new Circle(circ.getX(), circ.getY(), circ.getRadius());
		delete [] obj;
		obj = newObj;
		size++;
	}
	void addObject(Square sqr){
		Object *newObj = new Object[size + 1];
		for(int i = 0; i < size; i++){
			newObj[i] = obj[i];
		}
		newObj[size] = new Square(sqr.getX(), sqr.getY(), sqr.getSide());
		delete [] obj;
		obj = newObj;
		size++;
	}
	void addObject(Triangle tri){
		Object *newObj = new Object[size + 1];
		for(int i = 0; i < size; i++){
			newObj[i] = obj[i];
		}
		newObj[size] = new Triangle(tri.getX(), tri.getY(), tri.getA(), tri.getB(), tri.getC());
		delete [] obj;
		obj = newObj;
		size++;
	}
	void deleteObject(int index){
		Object *newObj = new Object[size - 1];
		for(int i = 0; i < index; i++){
			newObj[i] = obj[i];
		}
		delete obj[index];
		for(int i = index + 1; i < size; i++){
			newObj[i-1] = obj[i];
		}
		delete [] obj;
		obj = newObj;
		size--;
	}
	double getSumOfArea(){
		double sum = 0;
		for(int i = 0; i < size; i++){
			sum += obj[i]->getArea();
		}
		return sum;
	}
	Object& operator[](int index){
		return obj[index];
	}
	int getSize(){
		return size;
	}
};

Scene scn;
void drawMenu();
void drawShapeMenu();
void drawAddMenu();
void drawDelMenu();

int main(){
	bool isExited = false;
	int op;
	cout << CLEARSCREEN;
	while(!isExited){
		drawMenu();
		cin >> op;
		cout << CLEARSCREEN;
		if(op == 1){
			drawShapeMenu();
		}else if(op == 2){
			cout << "Всего фигур: " << scn.getSize() << endl;
			cout << "Суммарная площадь: " << scn.getSumOfArea() << endl;
			cout << "\n***\n" << endl;
		}else if(op == 3){
			drawAddMenu();
		}else if(op == 4){
			drawDelMenu();
		}else if(op == 5){
			scn.clear();
			cout << "Все фигуры были удалены!" << endl;			
			cout << "\n***\n" << endl;
		}else{
			isExited = true;
		}
	}
	cout << CLEARSCREEN << "\nДо новых встреч! Не болейте!\n" << endl;
	return 0;
}

void drawMenu(){
	cout << "1: Посмотреть коллекцию фигур" << endl;
	cout << "2: Найти суммарную площадь всех фигур" << endl;
	cout << "3: Добавить новую фигуру" << endl;
	cout << "4: Удалить фигуру" << endl;
	cout << "5: Удалить все фигуры" << endl;
	cout << "0: Завершить программу" << endl;
	cout << "\n";
}

void drawShapeMenu(){
	cout << "Всего фигур: " << scn.getSize() << endl;
	for(int i = 0; i < scn.getSize(); i++){
		cout << (i+1) << ". ";
		scn[i]->print();
		cout << endl;
	}
	cout << "\n***\n" << endl;
}

void drawAddMenu(){
	int op;
	cout << "Какую фигуру вы хотите добавить?\n" << endl;
	cout << "1: Добавить прямоугольник" << endl;
	cout << "2: Добавить круг" << endl;
	cout << "3: Добавить квадрат" << endl;
	cout << "4: Добавить треугольник" << endl;
	cout << "0: Вернуться в меню" << endl;
	cout << endl;
	cin >> op;
	cout << CLEARSCREEN;
	if(op == 1){
		double x, y, w, h;
		cout << "Введите прямоугольник в формате [x, y, ширина, высота]:" << endl;
		cin >> x;
		cin >> y;
		cin >> w;
		cin >> h;
		scn.addObject(Rectangle(x, y, w, h));
	}else if(op == 2){
		double x, y, r;
		cout << "Введите круг в формате [x, y, радиус]:" << endl;
		cin >> x;
		cin >> y;
		cin >> r;
		scn.addObject(Circle(x, y, r));
	}else if(op == 3){
		double x, y, s;
		cout << "Введите квадрат в формате [x, y, сторона]:" << endl;
		cin >> x;
		cin >> y;
		cin >> s;
		scn.addObject(Square(x, y, s));
	}else if(op == 4){
		double x, y, a, b, c;
		cout << "Введите треугольник в формате [x, y, a, b, c]:" << endl;
		cin >> x;
		cin >> y;
		cin >> a;
		cin >> b;
		cin >> c;
		if(a >= (b + c) || b >= (a + c) || c >= (a + b)){
			cout << "Не выполнено неравенство треугольника!" << endl;
			cout << "Некорректные данные" << endl;
			cout << "\n***\n" << endl;
			return;
		}
		scn.addObject(Triangle(x, y, a, b, c));
	}else return;
	cout << "Фигура успешно добавлена!\n\n***\n" << endl;
}

void drawDelMenu(){
	int op;
	cout << "Какую фигуру вы хотите удалить? (Введите порядковый номер)\n" << endl;
	for(int i = 0; i < scn.getSize(); i++){
		cout << (i+1) << ". ";
		scn[i]->print();
		cout << endl;
	}
	cout << "0: Вернуться в меню\n" << endl;
	cin >> op;
	if(op == 0){
		cout << CLEARSCREEN;
		return;
	}
	else if(op > 0 && op <= scn.getSize()){
		scn.deleteObject(op - 1);
		cout << "Фигура успешно удалена" << endl;
	}else cout << "Некорректный ввод" << endl;
	cout << "\n***\n" << endl;
}

