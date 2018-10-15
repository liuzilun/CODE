#include <stdio.h> //sprintf 
#include "buf.h"

//
//
//
buffer::buffer()
{
	clear();
}
void buffer::clear()
{
	buf = 0;
	len = 0;
}
void buffer::assign(char value, unsigned long count)
{
	if(count == 0) memset(buf, value, len);
	else memset(buf, value, count);
}
void buffer::fillzero()
{
	memset(buf, 0, len);
}
void buffer::go_ahead(int step)
{
	buf += step;
	len -= step;
}
std::string buffer::get_hex()
{
    if(len <= 0) return "";
	char* ch = new char[len*3 + 1];
    for(unsigned int i = 0; i < len; ++i)
    {
    	sprintf(ch + 3*i, "%02x ", (unsigned char)buf[i]);
    }
    ch[len*3] = 0;
    std::string ret = ch;
    delete [] ch;
    return ret;
}
std::string buffer::get_format_hex()
{
    if(len <= 0) return "";
    std::string ret;
	char ch[4], ch1[17], ch2[12];

    for(unsigned int i = 0; i < len; ++i)
    {
    	memset(ch, 0, 4);
    	sprintf(ch, "%02x ", (unsigned char)buf[i]);
        if(i % 16 == 0)
        {
            sprintf(ch2, "%08xh: ", i);
            ret += ch2;
        }
        if( i % 16 == 8)
        	ret += "- ";
        ret += ch;
        if(i % 16 == 15)
        {
        	ret += "; ";
            char c;
            for(int j = 0; j < 16; ++j)
            {
                c = buf[i-15+j];
            	if( c > 31 && c < 127 )
                	sprintf(&ch1[j], "%c", c);
                else
                	sprintf(&ch1[j], "%c", '.');
            }
            ch1[16] = 0;    
            ret += ch1;
        	ret += "\n";
        }
    }
    if(len % 16 ==0) return ret;
    for(int j = len % 16; j < 16; ++j)
    {
    	if( j % 16 == 8)
        	ret += "  ";
        ret += "   ";
    }
    ret += "; ";
    char c;
    memset(ch1, 0, 17);
    for(unsigned int i = len / 16 * 16, k = 0; i < len; ++i, ++k)
    {
        c = buf[i];
        if( c > 31 && c < 127 )
            sprintf(&ch1[k], "%c", c);
        else
            sprintf(&ch1[k], "%c", '.');
    }
    ret += ch1;
    return ret;
}
//
//
//
buffer_block::buffer_block(unsigned long buf_len, char default_value)
{
	if (buf_len)
	{
		_data.buf = new char[buf_len];
        _data.len = buf_len;
		_default = default_value;
        clear();
	}
	else
	{
		_data.clear();
	}
}
buffer_block::~buffer_block()
{
	if ((_data.len) && (_data.buf != NULL))
	{
		delete []_data.buf;
		_data.buf = NULL;
		_data.len = 0;
	}
}
	
bool buffer_block::apply(unsigned long len, buffer& buff)
{
	if(_applied_len + len > _data.len) { return false; }

    buff.buf = _data.buf + _applied_len;
	buff.len = len;
    _applied_len += len;
    
    return true;
}
void buffer_block::erase(unsigned long len)
{
	if(len && len == _applied_len)
	{
		clear();
		return;
	}
	if(len && len < _applied_len)
	{
        unsigned long move_len = _applied_len - len;
		memcpy(_data.buf, _data.buf + len, move_len);
		memset(_data.buf + move_len, _default, _data.len - move_len);
		_applied_len -= len;
	}
}
void buffer_block::clear()
{
	_applied_len = 0;
	if(_data.len) memset(_data.buf, _default, _data.len);
}
buffer&	buffer_block::get_buffer()
{
	return _data;
}
void buffer_block::get_buffer(buffer& buff)
{
	buff.buf = _data.buf;
    buff.len = _applied_len;
}
char* buffer_block::get()
{
	return _data.buf;
}
/*void buffer_block::resize(unsigned long len)
{
	if(len)
	buffer tmp_buff;
	tmp_buff.buf = 
}*/
void buffer_block::fillzero()
{
	if(_data.len) _data.fillzero();
}

int buffer_block::len()
{
	return _applied_len;
}
bool buffer_block::append(buffer& buf)
{
	bool result = (_data.len >= _applied_len + buf.len);
	if(result)
	{
		memcpy(&_data.buf[_applied_len], buf.buf, buf.len);
		_applied_len += buf.len;
	}
	return result;
}
