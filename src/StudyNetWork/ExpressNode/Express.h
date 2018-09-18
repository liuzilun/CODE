#include"ExpressNode.h"
#include<iostream>

using namespace std;

class Express{
public:
	Express(int);//ValueNode
	Express(char, const Express);
	Express(char, const Express, const Express);
private:
	class ExpressNode* p;  //
};