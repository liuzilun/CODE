
#include "vcl.h"
#include "windows.h"
#include "fmpbuf.h"
#include "winfmp.h"
#include "mmsystem.h"
#include "stdio.h"
#include "stdlib.h"
extern "C"{
static DWORD VideoModel=2,VideoType=0,VideoSelect;
static BYTE TTStream;
static int AudioSelect;
static BYTE AudioState=1,AState=0;
static DWORD AudioCh[10];
static DWORD AudioNum=0;
static HDRVR hReelDrv;
static BYTE	TemStream;
}
bool OpenDrive(void)   //打开一个神龙卡设备，如果成功，返回非0值
{
//入口参数
  hReelDrv=(HDRVR)OpenDriver(L"reeldrv",L"Drivers32",0L);//(LPCWSTR)TEXT("Reeldrv"),(LPCWSTR)TEXT("Driver32"),0L);
  if(hReelDrv!=0) return true;
  return false;
}

DWORD GetState(BYTE ghMPEGStream )
{
//入口参数:播放的流编号
//出口参数:如果流存在,返回流的状态,否则返回停止状态
  if (ghMPEGStream)
  {
    return FMPGet(ghMPEGStream,FMPI_STM_MODE);
  }
  return FMPF_STOPPED;
}

DWORD FMPCommand( BYTE Command, BYTE hMPEGStream, WORD Flags, DWORD Value )  //向打开的设备指定的流发送命令
{
//入口参数:
//  COMMAND表示要发送的命令
//  hMPEGStream表示要针对的流
//  Flags要发送的命令的状态
//  Value要发送的命令的变量值
	Sleep(10);
	return (DWORD)SendDriverMessage(hReelDrv,DRV_USER+1,
	MAKELPARAM(MAKEWORD(Command,hMPEGStream),Flags),Value);
}

void QuitVideo(BYTE &ghMPEGStream)  //本函数用来退出视频，关闭设备
{
//入口参数:
//   ghMPEGStream表示要退出的流
  if( ghMPEGStream )
  {
    FMPStop( ghMPEGStream );
    FMPClose( ghMPEGStream );
    ghMPEGStream = 0;
  }
  //CloseDriver(hReelDrv,0L,0L);
  hReelDrv = 0;
}

void CloseVideo(BYTE &ghMPEGStream)         //关闭打开的文件
{
//入口参数:
//     ghMPEGStream表示要停止播放的流
  if( ghMPEGStream )
  {
    FMPStop( ghMPEGStream );
    FMPClose( ghMPEGStream );
    ghMPEGStream=0;
  }
}

void SelectTV(int Choice,BYTE ghMPEGStream)   //对设备进行TV/AV切换
{
//入口参数:
//   Choice为0表示切换到AV,如果为1表示切换到TV
//   ghMPEGStream表示要切换的流
  if (Choice==0)
  {
    if (ghMPEGStream)
    {
      FMPSet(ghMPEGStream,0x41b,0);
    }
  }
  else
  {
    if (ghMPEGStream)
    {
      FMPSet(ghMPEGStream,0x41b,1);
    }
  }
}

//=============================================================================
void MuteAudio(BYTE ghMPEGStream)   //静音操作
{
//入口参数:
//    ghMPEGStream 要静音或者是开音的流
	DWORD LevelLevel;
	if (!ghMPEGStream) return;
	FMPSet( ghMPEGStream, FMPI_AUD_VOLUME , 0);

}

void PauseVideo(BYTE ghMPEGStream,int PauseSign)   //暂停流或者是恢复播放
{
//入口参数:
//       ghMPEGStream表示要暂停的流
//       PauseSign 为0表示要暂停,为1表示恢复暂停
	if (!ghMPEGStream) return;
	if (PauseSign==0)
	{
		FMPPause (ghMPEGStream);
	}
	else
	{
		FMPPlay( ghMPEGStream, FMPF_POS_END | FMPF_END_STOP, 0L );
	}
}

void StopVideo(BYTE &ghMPEGStream)        //停止一个流的播放
{
//入口参数:
//      ghMPEGStream表示要停止播放的流
  if( ghMPEGStream )
  {
    FMPStop( ghMPEGStream );
    FMPClose( ghMPEGStream );
	ghMPEGStream=0;
  }
}

void RePlay(BYTE ghMPEGStream)   //重放流
{
//入口参数:
//    ghMPEGStream表示要重放的流
  if (ghMPEGStream)
  {
    FMPPause( ghMPEGStream );
    FMPSeek(ghMPEGStream,FMPF_POS_START,0L);
    FMPPlay( ghMPEGStream, FMPF_POS_START | FMPF_END_STOP, 0L );
  }
}
//
void SetVolume(BYTE ghMPEGStream,int VolumeLevel)   //设置声音音量
{
	DWORD LevelLevel;
        int TmpVolumeLevel;
	if (!ghMPEGStream) return;
        TmpVolumeLevel=VolumeLevel;
	LevelLevel =(TmpVolumeLevel<<16) ;
	LevelLevel = LevelLevel + TmpVolumeLevel;
	FMPSet( ghMPEGStream, FMPI_AUD_VOLUME , LevelLevel);
}

void Forward(BYTE ghMPEGStream,int ForwardBackward)  //快进
{
//入口参数:
//     ghMPEGStream表示要向前播放的流
//     ForwardBackward为0表示不快进,恢复以前的播放
//                    为1表示快进
    if (ghMPEGStream)
        {

		if (ForwardBackward ==1)
		{
			FMPSeek(ghMPEGStream,FMPF_POS_CUR | FMPF_END_PAUSE,50L);
		}
		else
			FMPPlay( ghMPEGStream, FMPF_POS_END | FMPF_END_STOP, 0L );
        }
}
void Backward(BYTE ghMPEGStream,int ForwardBackward)   //快退
{
//入口参数:   ghMPEGStream表示要快退的流
//            ForwardBackward为1表示要快退为0表示不快退
    if (ghMPEGStream)
    {

		if (ForwardBackward ==1)
		{
			DWORD H = FMPGet(ghMPEGStream,FMPI_STM_POSITION);
			if (H>200)
			{
				H -= 50;
				FMPSeek(ghMPEGStream,FMPF_POS_SET | FMPF_END_PAUSE ,H);
			}
		}
		else
			FMPPlay( ghMPEGStream, FMPF_POS_END | FMPF_END_STOP, 0L );
    }
}

void SAudio(DWORD ghMPEGStream)  //设置原声切换
{

	if (!ghMPEGStream) return;

	if ((VideoModel==2)&&(VideoType<=8))         //如果是DVD歌曲
	{
		switch (AudioState)
		{
			case 0:
				FMPSet( ghMPEGStream, FMPI_AUD_BAL_L,0x64);  //打开所有的立体声
				FMPSet( ghMPEGStream, FMPI_AUD_BAL_R,0x64);  //打开所有的立体声
				AState=0;
				AudioState=1;
				break;
			case 1:
				FMPSet( ghMPEGStream, FMPI_AUD_BAL_L,0x640000);  //去掉所有的立体声
				FMPSet( ghMPEGStream, FMPI_AUD_BAL_R,0x640000);  //去掉所有的立体声
				AState=1;
				AudioState=2;
				break;
			case 2:
				FMPSet( ghMPEGStream, FMPI_AUD_BAL_L,0x64);      //去掉左立体声
				FMPSet( ghMPEGStream, FMPI_AUD_BAL_R,0x640000);  //打开原声
				AState=2;
				AudioState=0;
				break;
		}
	}
	else  //DVD歌曲
	{
		if (VideoModel==2)   //如果只有两个流,则有八种配音方案
		{
			if (AudioSelect == 7) AudioSelect = 0;  //共有八个声道,八种配音方案,来回切换
			else AudioSelect ++;
			FMPSet( TTStream, FMPI_AUD_CHANNELS,(DWORD)AudioSelect);  //选择音左右两迹切换
			return;
		}
		if (AudioNum==VideoModel) return;  //如果不止两个流,且当前的音频流是最后一个流,则将直接退出
		FMPGroup(ghMPEGStream,FMPF_SELECT | FMPF_INDEX ,AudioCh[VideoSelect-1]);  //选择音频流,来回切换
		VideoSelect++;
		if (VideoSelect==AudioNum+1) VideoSelect=1;       //声道来回切换
	}
}

int OpenVideo(char *VideoName,BYTE &ghMPEGStream)  //播放一个文件
{
//入口参数:
//   VideoName表示要播放的文件
//   ghMPEGStream用来接收返回的流的值
DWORD i,H,K;
	//QuitVideo(ghMPEGStream);
        CloseVideo(ghMPEGStream);
        ghMPEGStream = (BYTE)FMPOpen( FMPF_FILE, (DWORD)VideoName);
 	if(ghMPEGStream==0 )return 0;
	FMPSet( ghMPEGStream, FMPI_STM_TIME_FMT, FMPF_FRAMES );
        SetVolume(ghMPEGStream,0);
        FMPPlay( ghMPEGStream, FMPF_POS_END | FMPF_END_STOP, 0L );
	//K = VideoType;
	VideoModel=FMPGet(ghMPEGStream,FMPI_GRP_NB);
	if (VideoModel==2)
	{
		for (i=1;i<=VideoModel;i++)
		{
			TemStream = (unsigned char)FMPGroup(ghMPEGStream,FMPF_GET | FMPF_INDEX,i);
			H=FMPGet(TemStream,FMPI_STM_TYPE);
			if (H==FMPF_AUDIO)
			{
				VideoType = FMPGet(TemStream,FMPI_AUD_TYPE);
				TTStream = TemStream;
				AudioSelect = 7;
			}
		}
	}
	else
		VideoType = FMPF_AUD_DOLBY_AC3;
	AudioState=1;
	AState=0;
	if (VideoType<=8)        //VCD歌曲
	{
		FMPSet( ghMPEGStream, FMPI_AUD_BAL_L,0x64);  //设置立体声
		FMPSet( ghMPEGStream, FMPI_AUD_BAL_R,0x64);  //设置立体声
	}
	else  //DVD歌曲
	{
		if (VideoModel>2)  //如果流超过两个则将要进行声道选择
		{
			K = 0;
			for (i=1;i<=VideoModel;i++)          //首先获取音频流
			{
				TemStream = (unsigned char)FMPGroup(ghMPEGStream,FMPF_GET | FMPF_INDEX,i);
				H=FMPGet(TemStream,FMPI_STM_TYPE);
				if (H==FMPF_AUDIO)   //如果是音频流,则保存音频流的编号
				{
					AudioCh[K]=i;
					K++;
				}
			}
			AudioNum = K;
			if (AudioNum!=VideoModel)
			{
				VideoSelect=AudioNum;
				FMPGroup(ghMPEGStream,FMPF_SELECT | FMPF_INDEX ,AudioCh[VideoSelect-1]);   //选择一个播放的声迹
				VideoSelect++;
				if (VideoSelect==AudioNum+1) VideoSelect=1;
			}
		}
	}
	FMPPlay( ghMPEGStream, FMPF_POS_START | FMPF_END_STOP, 0L );
	return 1;
}
//}   //End extern "C"
