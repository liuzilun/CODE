/*******************************************************************************
*
*   COPYRIGHT   :   (c) 1995-1996 Sigma Designs, Inc. All rights reserved.
*
*   FILENAME    :   FMPPlay.h
*
*   DESCRIPTION :   Header file for FMPPlay.c.
*
*   PROGRAMMER  :   ECY
*
*   COMMENTS    :   You are free to reuse or modify this code.
*
*   NOTES       :   Compiled as a 32-bit application under MSVC 4.0.
*
*   CHANGES     :
*   11/09/95    -   Created.
*    (ECY)
*
*******************************************************************************/


#ifndef __FMPPLAY_H__
#define __FMPPLAY_H__


#define MAXFILENAME   256   // maximum length of file pathname
#define MAXCUSTFILTER 40    // maximum size of custom filter buffer


BOOL FMPPlay_OnCreate( HWND hWnd, CREATESTRUCT FAR *lpCreateStruct );
void FMPPlay_OnDestroy( HWND hWnd );
void FMPPlay_OnCommand( HWND hWnd, int iID, HWND hWndCtl, UINT uiCodeNotify );
void FMPPlay_OnMove( HWND hWnd, int iX, int iY );
void FMPPlay_OnSize( HWND hWnd, UINT uiState, int iCX, int iCY );
void FMPPlay_OnSetFocus( HWND hWnd, HWND hWndOldFocus );
#define FMPPlay_DefWndProc DefWindowProc


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hInstPrev, LPSTR lpstrCmdParam, int iCmdShow );
BOOL InitApplication( HINSTANCE hInstance );
BOOL InitInstance( HINSTANCE hInstance, int iCmdShow );
LRESULT CALLBACK MainWndProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK AboutBoxProc( HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam );


#endif  /* __FMPPLAY_H__ */
