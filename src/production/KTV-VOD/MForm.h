//---------------------------------------------------------------------------

#ifndef MFormH
#define MFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "head.h"
#include <ExtCtrls.hpp>
#include <ADODB.hpp>
#include <DB.hpp>
#include "mmsystem.h"
#include "stdio.h"
#include "stdlib.h"
#include <Graphics.hpp>
#include <NMUDP.hpp>
#include "SHDocVw_OCX.h"
#include <OleCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TTimer *Timer1;
        TImage *Image1;
        TTimer *Timer2;
        TImage *Image2;
        TNMUDP *NMUDP1;
        TNMUDP *NMUDP2;
        TPanel *Panel1;
        TCppWebBrowser *CppWebBrowser1;
        TNMUDP *NMUDP3;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall Timer2Timer(TObject *Sender);
        void __fastcall NMUDP1DataReceived(TComponent *Sender,
          int NumberBytes, AnsiString FromIP, int Port);
        void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall NMUDP2DataReceived(TComponent *Sender,
          int NumberBytes, AnsiString FromIP, int Port);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall CppWebBrowser1NewWindow2(TObject *Sender,
          LPDISPATCH *ppDisp, TOLEBOOL *Cancel);
        void __fastcall NMUDP3DataReceived(TComponent *Sender,
          int NumberBytes, AnsiString FromIP, int Port);
private:	// User declarations
        struct ShowPictureStru SPS;
        struct EventStru       ES;
        struct DrawTextStru    TS;
        void __fastcall ShowPicture(int x,int y,char *fname);
        void __fastcall TranShowPicture(int x,int y,char *fname);
        void __fastcall DrawText16(int x,int y,int Color,char *FontName,char *Text);
        void __fastcall DrawText24(int x,int y,int Color,char *FontName,char *Text);
        void __fastcall DrawText32(int x,int y,int Color,char *FontName,char *Text);
        void __fastcall ADrawText(int x,int y,int Color,char *FontName,char *Text,int FontSize);
        void __fastcall BDrawText(int x,int y,int Color,wchar_t *Text,int len);
        struct KeyTabStru  KTS[100];
        int    KeyTabCount;
        struct MapDrive Path[100];
        int    PathCount;
        AnsiString Gswjm,XPath; //公视文件名和X盘的路径
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
        void __fastcall           TDP();
        void __fastcall         TText();
        void __fastcall    SaveRecord();        //保存录音
        void __fastcall   StartRecord();
        bool    CacheData(TMessage Msg);
        char                  *waveBuff;        //录音缓冲区
        FILE                  *RecordFP;        //录音文件
        bool               RecordMusice;
        WAVEFORMATEX         waveFormat;        //声音录制的参数
        WAVEHDR                 waveHdr;        //设置缓冲区的参数
        HWAVEIN                  waveIn;
        unsigned long         bytecount;        //录音的总字节数
        struct WaveHead        wavehead;
        AnsiString         RecordSongNo;
        void __fastcall  BackPlaySong();       //回放录音
        bool                   BackPlay;
        void __fastcall  StopBackPlay();       //停止回放
        AnsiString       BackPlaySongNo;       //录音的歌曲编号
//-----------------------------------------------------------------------------
        void __fastcall ProcessPlaySong();
        int              SongPlayStat;
        bool             BackMusice;
        unsigned char    PlayStream;
        int              SongVolumeSize;
        int              Mute;
        AnsiString __fastcall FindSong(AnsiString No);
        bool __fastcall ProcessDJ(char Key);
        void __fastcall ReadPath(AnsiString mapfile);
        bool PL_ExitWindow(UINT uFlag);
        void __fastcall DelRecordFile();
        AnsiString GetTextWidthText;
        int        GetTextWidthV;
        void __fastcall GetTextWidth();
        void __fastcall ZhGuangGao();
        void __fastcall DrawGuangGao();
        int             RoomStat;
        bool __fastcall CheckComputer();
        void __fastcall MyCopyFile();
        void __fastcall DrawDH1(Graphics::TBitmap *tp,int CurX,int Cury );
        void __fastcall DrawDH2(Graphics::TBitmap *tp,int CurX,int Cury );
        void __fastcall DrawDH3(Graphics::TBitmap *tp,int CurX,int Cury );
        void __fastcall DrawDH4(Graphics::TBitmap *tp,int CurX,int Cury );
        bool  DrawDhFlag;   //画动画标记
        AnsiString __fastcall ReadPara(AnsiString PName);
        int    Gsyldx;
        AnsiString FindSongBuff;
        AnsiString CloseComputerPassword;
        struct SelSongTab PlaySongTab[1000],SelSong[5000]; //播放和待选队列
        int    SongCount,PlaySongCount,PlaySongNo;   //待选歌曲总数,播放歌曲总数,当前播放歌曲编号
        bool ShowTime,ShowNextSong;
        int  MouseOperCode;
        int __fastcall GetMouseOperCode(AnsiString PictName);
        struct MouseTab MTab[2000];
        int    MTabCount;
        struct POPERTab PTab[300];
        int    PTabCount;
        void __fastcall ReadMouseTab();
        //void OperMouseKeyDown(TMessage Msg);
        int  MainThreadStat;
        int  TVAV_STAT;
        int  SyDh;     //使用动画
        int  JiuCheck; //酒水密码检查
        void __fastcall WriteFwhj();
        AnsiString NetPageHome;
        void __fastcall ShowNetPage();  //显示网页操作界面
        void __fastcall CloseNetPage();
        void CloseInternet(TMessage msg);
        int  InternetOper;
        AnsiString SongNameFont;
        int        SongNameFontSize;
        AnsiString SingerFont;   //歌星姓名字体
        int        SingerFontSize; //歌星姓名字体大小
        void __fastcall initSystem();  //初始化系统
        AnsiString hwzt;               //函文字体
        int        hwztdx;             //函文字体大小
        AnsiString zysy;
        int __fastcall SendData(AnsiString RemoteHost,int Port,char *senddata,int sendlen); //往指定的IP发送数据
        int __fastcall WaitRecvData(int LocalPort,char *Recvbuff,int RecvSize,int WaitTime);//超时等待接收数据
        int        Bhdgjm;             //编号点歌界面
        int  zyzt;
        AnsiString RemoteHostIP;
        struct QQDG Send,Recv;
//-----------------------------------------------------------------------------
protected:
        BEGIN_MESSAGE_MAP
                VCL_MESSAGE_HANDLER(MM_WIM_DATA, TMessage, CacheData)
                VCL_MESSAGE_HANDLER(WM_HOTKEY, TMessage, CloseInternet)
        END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
