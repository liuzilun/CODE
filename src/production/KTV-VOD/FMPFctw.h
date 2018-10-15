#ifndef FMPFCTW
#define FMPFCTW

#ifdef __cplusplus
extern "C"
{
#endif
int FMPOpenDriver(void);
void FMPCloseDriver(void);
DWORD FMPCommand( BYTE Command, BYTE hMPEGStream, WORD Flags, DWORD Value );
//DWORD CALLBACK FMPCallbackProc( BYTE bMsg, BYTE hMPEGStream, DWORD dwValue );
DWORD FMPGetLastError(WORD a);

#ifdef __cplusplus
}
#endif


#endif
