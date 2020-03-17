#include <iostream>
using namespace std;

class Book{
	char* name;
	char* writer;
	static int counter;
	int getLength(const char* s) const{
		int i = 0;
		while(s[i++] != 0);
		return i-1;
	}
public:
	Book(const char* name = "name", const char* writer = "writer"){
		this->name = nullptr;
		this->writer = nullptr;
		setName(name);
		setWriter(writer);
		counter++;
	}
	~Book(){
		delete [] name;
		delete [] writer;
		counter--;
	}
	Book(const Book& book){
		setName(book.getName());
		setWriter(book.getWriter());
	}
	void setName(const char* name){
		if(this->name != nullptr) delete [] this->name;
		int len = getLength(name);
		this->name = new char[len + 1];
		for(int i = 0; i < len; i++){
			this->name[i] = name[i];
		}
		this->name[len] = 0;
	}
	void setWriter(const char* writer){
		if(this->writer != nullptr) delete [] this->writer;
		int len = getLength(writer);
		this->writer = new char[len + 1];
		for(int i = 0; i < len; i++){
			this->writer[i] = writer[i];
		}
		this->writer[len] = 0;
	}
	char* getName() const{
		return name;
	}
	char* getWriter() const{
		return writer;
	}
	int getCounter() const{
		return counter;
	}
};
int Book::counter = 0;
ostream& operator<<(ostream &out, const Book &book){
	cout << "Book: " << book.getName() << ".\n___Writer: " << book.getWriter() << ".";
}

class Library{
	Book *bookArray;
	int size;
public:
	Library(){
		bookArray = new Book[0];
		size = 0;
	}
	virtual ~Library(){
		delete [] bookArray;
	}
	Library(const Library& lib){
		int prevSize = lib.getSize();
		bookArray = new Book[prevSize];
		for(int i = 0; i < prevSize; i++){
			bookArray[i].setName(lib[i].getName());
			bookArray[i].setWriter(lib[i].getWriter());
		}
	}
	Library& operator=(Library& lib){
		delete [] bookArray;
		size = lib.size;
		bookArray = new Book[size];
		for(int i = 0; i < size; i++){
			bookArray[i].setName(lib[i].getName());
			bookArray[i].setWriter(lib[i].getWriter());
		}
		return *this;
	}
	Book& operator[](int index) const{
		return bookArray[index];
	}
	void addBook(Book book){
		Book* newArray = new Book[size + 1];
		for(int i = 0; i < size; i++){
			newArray[i].setName(bookArray[i].getName());
			newArray[i].setWriter(bookArray[i].getWriter());
		}
		newArray[size].setName(book.getName());
		newArray[size].setWriter(book.getWriter());
		delete [] bookArray;
		bookArray = newArray;
		size++;
	}
	int getSize() const{
		return size;
	}
	virtual int getKey() = 0;
};
ostream& operator<<(ostream &out, const Library &lib){
	for(int i = 0; i < lib.getSize(); i++){
		cout << lib[i] << endl;
	}
}

int main(){
	return 0;
}
