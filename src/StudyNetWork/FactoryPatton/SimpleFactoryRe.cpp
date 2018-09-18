#include"DynOBJ.h"
#include <iostream>
#include<string>



using namespace std;
class Api{
public:
	virtual void test(string s) = 0;
protected:
	Api(){}
};

class ImplOne :public Api
{
public:
	void test(string s){
		cout << "现在是执行One" << s;
	}
};
class ImplTwo :public Api
{
public:
	void test(string s){
		cout << "现在是执行Two" << s;
	}
};

class Factory{
public:
	static Api* createApi(int type){
		Api* pApi = nullptr;
		if (type == 1)
		{
			pApi = new ImplOne();
		}
		if (type == 2)
		{
			pApi = new ImplTwo();
		}
		return pApi;
	}


};
#define REG_CLASS (class_name) \
class class_name##Helper{ \
public: \
	class_name##Helper(){ \
		CobjectFactory::registerClass(##class_name, class_name##Helper::createObjFunc); \
	} \
	static void* createObjFunc(){ \
		return new class_name##(); \
	} \
}; \
class_name##Helper class_name##helper; 

//REG_CLASS(ImplTwo)

int main(void){
	//ImplTwo* ptwo = static_cast<ImplTwo*> (ImpleTwoHelper::createObjFunc());
	//ptwo->test("two...");
	system("pause");

}