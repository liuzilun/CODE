//
//
//	Buffer Class
//
//	auth: cgc
//	date: 2003-08-02
//

#pragma once

#include <memory.h>
#include <string>
#include "sync.h"

#pragma warning (disable : 4103)
#pragma pack (push, 1)

struct buffer
{
	char* 	buf;
	unsigned long 	len;
	
	buffer();
    buffer(char* b, unsigned long l) : buf(b), len(l) {}

	void	clear();							// set buf = NULL, len = 0.
	void 	assign(char value, unsigned long count = 0);
	void	fillzero();
	void	go_ahead(int step);

    std::string get_hex();
    std::string get_format_hex();
};

class buffer_block
{
public:
	buffer_block(unsigned long buf_len = 20480, char default_value = 0);
	virtual ~buffer_block();
	
	bool 	apply(unsigned long, buffer&);		// return true, if apply success, or return false.
	void 	erase(unsigned long len);			// erase buffer, from begginning to len.
	void 	clear();							// clear the buffer, and fill the buffer with default_value.
	buffer&	get_buffer();						// get the whole block.
    void	get_buffer(buffer& buff);
	char*	get();								// get the buffer pointer.
	int		len();
	bool	append(buffer& buf);
	
//	void 	increase(unsigned long);			// resize the buffer.
	void	fillzero();							// fill the buffer with zero.
	
private:
	unsigned long	_applied_len;
	char			_default;
	buffer			_data;
};

//
//	MACRO BUFFER
//
#define BUFFER(buf_name, buf_size)				\
	char buf_name[buf_size];					\
	unsigned long len_##buf_name = buf_size;	\
	unsigned long applied_##buf_name = 0

#define BUFFER_FILLZERO(buf_name)				\
	memset(buf_name, 0, len_##buf_name)

#define BUFFER_CAN_APPLY(buf_name, buf_size)	\
	(len_##buf_name - applied_##buf_name > buf_size > 0)

#define BUFFER_APPLY(buf_name, buf_size, des)	\
	des.buf = buf_name + applied_##buf_name;	\
	des.len = buf_size;							\
	applied_##buf_name += buf_size

#define BUFFER_LEN(buf_name)					\
	(applied_##buf_name)

#define BUFFER_GET(buf_name)					\
	(buf_name)

//
// usage:
//		BUFFER(app_buf, 1024);
//		BUFFER_FILLZERO(app_buf);
//		buffer tmp_buf;
//		BUFFER_APPLY(app_buf, 100, tmp_buf); 
//

//
//	added at 2003-12-17
//
class critical_buffer_block : private buffer_block
{
public:
	critical_buffer_block(unsigned long buf_len = 20480, char default_value = 0)
		: buffer_block(buf_len, default_value)
	{}
	~critical_buffer_block() {}
	
	bool 	apply(unsigned long len, buffer& buff)
	{
		_cs.Lock();
		bool ret = buffer_block::apply(len, buff);
		_cs.Unlock();
		return ret;
	}
	void 	erase(unsigned long len)			// erase buffer, from begginning to len.
	{
		_cs.Lock();
		buffer_block::erase(len);
		_cs.Unlock();
	}
	void 	clear()							// clear the buffer, and fill the buffer with default_value.
	{
		_cs.Lock();
		buffer_block::clear();
		_cs.Unlock();
	}
	buffer&	get_buffer()						// get the whole block.
 	{
		return buffer_block::get_buffer();
	}
	void	get_buffer(buffer& buff)
	{
		_cs.Lock();
		buffer_block::get_buffer(buff);
		_cs.Unlock();
	}
	char*	get() 
	{ 
		return buffer_block::get(); 
	}
	int		len()
	{
		_cs.Lock();
		int ret = buffer_block::len();
		_cs.Unlock();
		return ret;
	}
	bool	append(buffer& buf)
	{
		_cs.Lock();
		bool ret = buffer_block::append(buf);
		_cs.Unlock();
		return ret;
	}
	
//	void 	increase(unsigned long);			// resize the buffer.
	void	fillzero()							// fill the buffer with zero.
	{
		_cs.Lock();
		buffer_block::fillzero();
		_cs.Unlock();
	}

private:
	Process::CCriticalSection _cs;
};
#pragma pack (pop, 1)