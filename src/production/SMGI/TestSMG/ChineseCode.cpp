#include "chinesecode.h"
#include <windows.h>
void CChineseCode::UTF_8ToUnicode(wchar_t* pOut,char *pText)
{
	char* uchar = (char *)pOut;

	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

	return;
}

void CChineseCode::UnicodeToUTF_8(char* pOut,wchar_t* pText)
{
	// 注意 WCHAR高低字的顺序,低字节在前，高字节在后
	char* pchar = (char *)pText;

	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));

	return;
}

void CChineseCode::UnicodeToGB2312(char* pOut,wchar_t uData)
{
	WideCharToMultiByte(CP_ACP,NULL,&uData,1,pOut,sizeof(wchar_t),NULL,NULL);
	return;
}     

void CChineseCode::Gb2312ToUnicode(wchar_t* pOut,char *gbBuffer)
{
	::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);
	return ;
}

void CChineseCode::GB2312ToUTF_8(string& pOut,char *pText, int pLen)
{
	char buf[4];
	int nLength = pLen* 3;
	char* rst = new char[nLength];

	memset(buf,0,4);
	memset(rst,0,nLength);

	int i = 0;
	int j = 0;      
	while(i < pLen)
	{
		//如果是英文直接复制就可以
		if( *(pText + i) >= 0)
		{
			rst[j++] = pText[i++];
		}
		else
		{
			wchar_t pbuffer;
			Gb2312ToUnicode(&pbuffer,pText+i);
			UnicodeToUTF_8(buf,&pbuffer);
			unsigned short int tmp = 0;
			tmp = rst[j] = buf[0];
			tmp = rst[j+1] = buf[1];
			tmp = rst[j+2] = buf[2];    

			j += 3;
			i += 2;
		}
	}
	rst[j] = '\0';

	//返回结果
	pOut = rst;             
	delete []rst;   

	return;
}

void CChineseCode::UTF_8ToGB2312(string &pOut, char *pText, int pLen)
{
	char * newBuf = new char[pLen];
	char Ctemp[4];
	memset(Ctemp,0,4);

	int i =0;
	int j = 0;

	while(i < pLen)
	{
		if(pText[i] > 0)
		{
			newBuf[j++] = pText[i++];                       
		}
		else                 
		{
			WCHAR Wtemp;
			UTF_8ToUnicode(&Wtemp,pText + i);
			UnicodeToGB2312(Ctemp,Wtemp);
			newBuf[j] = Ctemp[0];
			newBuf[j + 1] = Ctemp[1];

			i += 3;    
			j += 2;   
		}
	}
	newBuf[j] = '\0';

	pOut = newBuf;
	delete []newBuf;

	return; 
}  


void CChineseCode::BIG52GBK(char *szBuf) 
{
	if(!strcmp(szBuf, ""))
		return; 
	int nStrLen = strlen(szBuf); 
	wchar_t *pws = new wchar_t[nStrLen + 1]; 
	__try 
	{ 
		int nReturn = MultiByteToWideChar(950, 0, szBuf, nStrLen, pws, nStrLen + 1); 
		BOOL bValue = false; 
		nReturn = WideCharToMultiByte(936, 0, pws, nReturn, szBuf, nStrLen + 1, "?", &bValue); szBuf[nReturn] = 0; 
	}
	__finally 
	{ 
		if (pws != NULL)
			delete[] pws;
	} 

}
// GBK => Big5 

void CChineseCode::GBK2BIG5(char *szBuf) 
{ 
	if(!strcmp(szBuf, "")) return ; 
	int nStrLen = strlen(szBuf); 
	wchar_t *pws = new wchar_t[nStrLen + 1]; 
	__try 
	{ 
		MultiByteToWideChar(936, 0, szBuf, nStrLen, pws, nStrLen + 1); 
		BOOL bValue = false; 
		WideCharToMultiByte(950, 0, pws, nStrLen, szBuf, nStrLen + 1, "?", &bValue); 
		szBuf[nStrLen] = 0; 
	} 
	__finally 
	{ 
		delete[] pws; 
	} 
} 


// GB2312 => GBK 
void CChineseCode::GB2GBK(char *szBuf) 
{ 
	if(!strcmp(szBuf, "")) 
		return; 
	int nStrLen = strlen(szBuf); 
	WORD wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC); 
	int nReturn = LCMapString(wLCID, LCMAP_TRADITIONAL_CHINESE, szBuf, nStrLen, NULL, 0); 
	if(!nReturn) 
		return; 
	char *pcBuf = new char[nReturn + 1]; 
	__try 
	{ 
		wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC); 
		LCMapString(wLCID, LCMAP_TRADITIONAL_CHINESE, szBuf, nReturn, pcBuf, nReturn + 1); 
		strncpy(szBuf, pcBuf, nReturn); 
	} 
	__finally 
	{ 
		delete[] pcBuf; 
	} 
} 


// GBK =〉GB2312 

void CChineseCode::GBK2GB(char *szBuf) 
{ 
	if(!strcmp(szBuf, "")) 
		return; 
	int nStrLen = strlen(szBuf); 
	WORD wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_BIG5); 
	int nReturn = LCMapString(wLCID, LCMAP_SIMPLIFIED_CHINESE, szBuf, nStrLen, NULL, 0); 
	if(!nReturn) 
		return; 
	char *pcBuf = new char[nReturn + 1]; 
	__try 
	{ 
		wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_BIG5); 
		LCMapString(wLCID, LCMAP_SIMPLIFIED_CHINESE, szBuf, nReturn, pcBuf, nReturn + 1); 
		strncpy(szBuf, pcBuf, nReturn); 
	} 
	__finally 
	{ 
		delete []pcBuf; 
	} 
} 


void CChineseCode::GB2BIG5(char *szBuf)
{
	GB2GBK(szBuf); 
	GBK2BIG5(szBuf); 
}

void CChineseCode::BIG52GB(char *szbuf)
{
	BIG52GBK(szbuf);
	GBK2GB(szbuf);
}