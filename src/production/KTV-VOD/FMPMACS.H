#ifndef __FMPMACS_H
#define __FMPMACS_H

// some macros for an easier writing of calls to the driver

#define FMPOpen(Flags,Filename)	FMPCommand(FMP_OPEN,0,Flags,Filename)
#define FMPClose(hStream)	FMPCommand(FMP_CLOSE,hStream,0,0)
#define FMPSet(hStream,Index,Value)	FMPCommand(FMP_SET,hStream,Index,Value)
#define FMPGet(hStream,Index)	FMPCommand(FMP_GET,hStream,Index,0)
#define FMPPlay(hStream,Flags,Position)	FMPCommand(FMP_PLAY,hStream,Flags,Position)
#define FMPStep(hStream,Flags,Position)	FMPCommand(FMP_STEP,hStream,Flags,Position)
#define FMPCapture(hStream,Index,Position)	FMPCommand(FMP_CAPTURE,hStream,Index,Position)
#define FMPSeek(hStream,Flags,Position)	FMPCommand(FMP_SEEK,hStream,Flags,Position)
#define FMPSignal(hStream,Flags,Position)	FMPCommand(FMP_SIGNAL,hStream,Flags,Position)
#define FMPPause(hStream)	FMPCommand(FMP_PAUSE,hStream,0,0)
#define FMPStop(hStream)	FMPCommand(FMP_STOP,hStream,0,0)
#define FMPGroup(hStream,Flags,Value)	FMPCommand(FMP_GROUP,hStream,Flags,Value)
#define FMPCallback(hStream,Value)	FMPCommand(FMP_CALLBACK,hStream,FMPF_C,Value)
#define FMPFreeze(hStream,Flags,Value)	FMPCommand(FMP_FREEZE,hStream,Flags,Value)
#endif
