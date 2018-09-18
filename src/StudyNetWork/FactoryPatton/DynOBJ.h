#ifndef OBJDYN_H_
#define OBJDYN_H_
#include<string>
#include<map>

typedef void* (*Constructor)();

class CobjectFactory{
public:
	//sting->key 动态创建类人类名
	static void registerClass(std::string className, Constructor constructor){
		constructors()[className] = constructor;
	}

	static void * creatObject(const std::string& className){
		Constructor constructor = NULL;

		if (constructors().find(className) != constructors().end())
			constructor = constructors().find(className)->second;
		if (constructor == NULL)
			return NULL;

		return(*constructor)();
	}
private:
	inline static std::map<std::string, Constructor>& constructors(){
		static std::map<std::string, Constructor> instance;
		return instance;
	}
};



#endif //!OBJDYN_H