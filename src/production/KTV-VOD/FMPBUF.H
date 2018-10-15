/*******************************************************************************
*
*   COPYRIGHT   :   (c) 1995-1996 Sigma Designs, Inc. All rights reserved.
*
*   FILENAME    :   FMPBuf.h
*
*   DESCRIPTION :   Header file for FMPBuf1.c
*
*   PROGRAMMER  :   ECY
*
*   COMMENTS    :   You are free to reuse or modify this code.
*
*   NOTES       :   Compiled as a 32-bit application under MSVC 4.0.
*
*   CHANGES     :
*   11/30/95    -   Created.
*    (ECY)
*
*******************************************************************************/


#ifndef __FMPBUF_H__
#define __FMPBUF_H__


#define MAXFILENAME    256      // maximum length of file pathname
#define MAXCUSTFILTER  40       // maximum size of custom filter buffer


#define MAX_BUF_SIZE   1024*32	// maximum buffer size (< 65534)(multiple of 1024)
#define MAX_BUF_NUMBER 2        // maximum number of buffers to use


struct TBuf
{
	HANDLE hFile;
	DWORD dwSize;
	WORD wIndex;
	char *Buffer[MAX_BUF_NUMBER];
	DWORD BufferIndex[MAX_BUF_NUMBER];
};


#define FMPBuf1_DefWndProc DefWindowProc

DWORD CALLBACK FMPCallbackProc( BYTE bMsg, BYTE hMPEGStream, DWORD dwValue );


#endif  /* __FMPBUF_H__ */
