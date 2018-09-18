#include<iostream>
using namespace std;


CRITICAL_SECTION g_cs;

class Singleton{
private:
	Singleton(){
		m_singer = NULL;
		cout << "构造一个Singleton对象" << endl;
	}
public:
	static Singleton* getInstance(){
		if (m_singer == NULL)
		{
			m_singer = new Singleton;
		}
		return m_singer;
	}
private:
	static Singleton* m_singer;

};

Singleton* Singleton::m_singer = NULL;

int main(void){
	Singleton* p1 = Singleton::getInstance();
	Singleton* p2 = Singleton::getInstance();


	printf("p1= %x  p2 = %x ", p1, p2);
	system("pause");
}