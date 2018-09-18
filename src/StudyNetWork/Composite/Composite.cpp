#include<iostream>
#include<string>

#include<list>
#include<vector>

using namespace std;

class IFile{
public:
	virtual int add(IFile* file) = 0;
	virtual int remove(IFile* ifile) = 0;
	virtual void display() = 0;

	virtual list<IFile*>* getChild() = 0;


};


class File :public IFile{
public:
	File(string name) :m_name(name){}

	virtual void display(){
		cout << m_name << endl;
	}
	virtual int add(IFile* file){
		return -1;
	}
	virtual int remove(IFile* file){
		return -1;
	}
	virtual list<IFile*>* getChild(){
		return NULL;
	}


private:
	string m_name;
};

class Dir :public IFile{
public:
	Dir(string name) :m_name(name){
		m_list = new list<IFile*>;


	}



	virtual int add(IFile* file){
		m_list->push_back(file);
		return 0;
	}

	virtual int remove(IFile* file){
		m_list->remove(file);
		return 0;
	}
	virtual list<IFile*>* getChild(){
		return m_list;
	}

	virtual void display(){
		cout << m_name << endl;
	}
private:
	string m_name;

	list<IFile*> *m_list;
};


void showTree(IFile* root, int level){
	if (root == NULL)
	{
		return;
	}
	int i;
	for (i = 0; i < level; i++){
		printf("\t");
	}

	root->display();
	list<IFile*> *list1 = root->getChild();
	if (list1 != NULL){
		for (list<IFile*>::iterator iter = list1->begin(); iter != list1->end(); iter++)
		{
			if ((*iter)->getChild() == NULL){
				for (i = 0; i < level; i++){
					printf("\t");
				}
				(*iter)->display();
			}
			else
			{
				showTree(*iter, level + 1);
			}
		}
	}
}


int main(){
	
	    Dir* root = new Dir("C:");
		Dir* dir1 = new Dir("Program File");
		Dir* dir2 = new Dir("windows");
		Dir* dir3 = new Dir("system");


		File* file1 = new File("a.txt");
		File* file2 = new File("b.txt");
	
		root->add(dir1);
		root->add(file1);

		dir1->add(file2);

		dir1->add(dir2);

		list<IFile*>* list1 = root->getChild();
		showTree(root, 0);
	
	system("pause");
	return 0;

}




























//#include<iostream>
//#include<string>
//#include<list>
//
//using namespace std;
//
//class Leaf
//{
//public:
//
//public:
//	Leaf(string name):m_name(name){};
//	void showStruct(string preStr){
//		cout << preStr << "-" << m_name << endl;
//	}
//
//private:
//	string m_name;
//
//};
//
////文件夹
//class Composite{
//public:
//	Composite(string name) :m_name(name){};
//	//加入组合对象
//	void addComposite(Composite* pCom){
//		childComposite.push_back(pCom);
//	}
//
//	//加入叶子
//	void addLeaf(Leaf* pLeaf)
//	{
//		childLeaf.push_back(pLeaf);
//	}
//
//	void showStruct(string preStr){
//		cout << preStr << "+" << m_name << endl;
//		preStr += " ";
//		for (list<Leaf*>::iterator iter = childLeaf.begin(); iter != childLeaf.end(); iter++)
//		{
//			(*iter)->showStruct(preStr);
//		}
//		for (list<Composite*>::iterator iter = childComposite.begin(); iter != childComposite.end(); iter++)
//		{
//			(*iter)->showStruct(preStr);
//		}
//	}
//private:
//	string m_name;
//	list<Leaf*> childLeaf;
//	list<Composite*> childComposite;
//};
//
//
//int main()
//{
//	//Composite* pRoot = new Composite("C:\\");
//	//Composite* pC1 = new Composite("Program Files");
//	//Composite* pC2 = new Composite("Windows");
//
//	//Leaf * pLeaf1 = new Leaf("a.txt");
//	//Leaf * pLeaf2 = new Leaf("b.txt");
//
//	//Leaf * pLeaf3 = new Leaf("1.txt");
//	//Leaf * pLeaf4 = new Leaf("2.txt");
//
//
//	//pRoot->addComposite(pC1);
//	//pRoot->addComposite(pC2);
//
//	//pC1->addLeaf(pLeaf1);
//	//pC1->addLeaf(pLeaf1);
//
//	//pC2->addLeaf(pLeaf3);
//	//pC2->addLeaf(pLeaf4);
//
//	//pRoot->showStruct("");
//
//	//system("pause");
//	return 0;
//}