// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ENCRYPT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ENCRYPT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ENCRYPT_EXPORTS
#define ENCRYPT_API __declspec(dllexport)
#else
#define ENCRYPT_API __declspec(dllimport)
#endif

// 此类是从 Encrypt.dll 导出的
class ENCRYPT_API CEncrypt {
public:
	CEncrypt(void);
	virtual ~CEncrypt(void);
	bool Encrypt(char*   cSrc,char*   cDest);
	bool Decrypt(char*   cSrc,char*   cDest);   
};

