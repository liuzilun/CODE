#pragma once

#ifdef ENCODING_EXPORTS
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif

#include <string>
#include <map>
#include <exception>

#include <windows.h>

// C++ exception specification ignored except to indicate a 
// function is not __declspec(nothrow)
#pragma warning(disable : 4290)

//! base class interface of the encoding
class DLLEXPORT encoding {
public:
	virtual ~encoding() {}

	virtual void encode(const char*, int, char*, int*) = 0;
	virtual void decode(const char*, int, char*, int*) = 0;
};

class DLLEXPORT codeFactory {
public:
	~codeFactory(void) {}

	class bad_creation : public exception {
	public:
		bad_creation(const std::string& info) {
			reason = "cannot create type " + info;
		}
		~bad_creation() throw() { } 
		const char* what(void) const throw() {
			return reason.c_str();
		}
	private:
		std::string reason;
	};

	static encoding* createEncoding(const std::string& id) throw(bad_creation);
protected:
	codeFactory(const std::string& id);
	static void destroy();
	virtual encoding* create(void) = 0;
	static std::map<std::string, codeFactory*>* factories;
};