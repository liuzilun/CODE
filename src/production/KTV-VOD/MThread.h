//---------------------------------------------------------------------------

#ifndef MThreadH
#define MThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "head.h"
#define MAXXFRECCOUNT 300
//---------------------------------------------------------------------------
class MainThread : public TThread
{
private:
protected:
        void __fastcall Execute();
        struct ShowPictureStru *SPS;
        struct EventStru *ES;
        struct DrawTextStru *DTS;
        int __fastcall ReadKey();
        void __fastcall Process1();  //处理组合拼音点歌
        void __fastcall ShowPicture(int x,int y,AnsiString Fname);     //显示图片
        void __fastcall TranShowPicture(int x,int y,AnsiString Fname); //穿透显示图片
        void __fastcall DrawText16(int x,int y,int color,char *Fontname,char *Text);
        void __fastcall DrawText24(int x,int y,int color,char *Fontname,char *Text);
        void __fastcall DrawText32(int x,int y,int color,char *Fontname,char *Text);
        int  __fastcall ReadSongTab(AnsiString Field,AnsiString Value,int Compare);  //读取歌曲数据
        void __fastcall DisplayMsg(AnsiString Msg);  //显示提示信息
        void __fastcall ProcessGQ();               //处理歌曲
        void __fastcall ShowSelSong(int PageNo,int SelNo,int stat=0);
        void __fastcall Process2();                //处理字数点歌
        void __fastcall Process3();                //处理歌星点歌
        int __fastcall ReadSinger(int fno,AnsiString value);//读取歌星库
        struct SingerTab Singer[1000];
        int    SingerCount;
        void __fastcall ShowSinger(int pageno,int selno);
        void __fastcall Process4();   //编号选曲
        void __fastcall Process5();   //语种选曲
        void __fastcall Process6();
        void __fastcall ProcessBh();  //处理点号点歌
        void __fastcall SongAddClick(int rno);
        void __fastcall Process8();
        void __fastcall ShowPlaySong(int PageNo,int SelNo);  //显示己选歌曲
        int  __fastcall FilterPy(char Py);    //过滤歌曲的拼音
        int  __fastcall FilterZs(char zs);    //过滤字数
        char __fastcall ReadZs();
        char __fastcall ReadPy();
        int  __fastcall SingerFilterPy(char py);  //过滤拼音
        void __fastcall Process7();
        void __fastcall ShowXf(int PageNo);
        void __fastcall ReadXf();  //读取消费数据
        char ComputerName[1024];   //本计算机名字
        struct Xf  XfArray[MAXXFRECCOUNT];  //最多三百项消费
        int    XfCount;            //消费记录总数
        void __fastcall ShowNoSong(int pageno,int selno);
        void __fastcall ProcessDJ(char Key);
        int           SongPlayStat;   //歌曲播放状态,0表示正常播放 1表示快进 2表示快退
        int           SongVolumeSize; //歌曲音量大小
        int           Mute;
        unsigned char PlayStream;     //歌曲播放流

        bool          BackMusice;
        void       __fastcall BackPlaySong(int Rno);
        bool       BackPlay;  //回放
        bool __fastcall CloseComputer();
public:
        __fastcall MainThread(bool CreateSuspended,
                              struct EventStru *P1,
                              struct ShowPictureStru *P2,
                              struct DrawTextStru *P3);
        void __fastcall ProcessPlaySong();
        void __fastcall PlaySongEnd();
        char vipuser[100];
        char vippass[100];
        char myloveuser[100];
        char mylovepass[100];
        char mylovepassv[100];
        struct MyLoveFile MLF;   //我的最爱中的文件
        void __fastcall VIPMyLove();
        void __fastcall DrawMyLoveEdit(int stat,int operno);
        void __fastcall MyLoveKey(int Key,int operno);
        void __fastcall ProcessMyLoveReg();
        void __fastcall ProcessMyLoveLogin();  //处理我的最爱登录
        void __fastcall ProcessMyLoveClick(AnsiString yh);  //处理我的最爱歌曲
        bool __fastcall MyLoveAdd(int rno);    //在己选歌曲中我的最爱登陆
        void __fastcall ProcessVIP();

        struct shoplb   shoplbTab[100];
        int    shoplbcount;
        void __fastcall ProcessShowJiuLb();   //处理酒水界面
        void __fastcall ShowShopLb(int pno,int pc);

        struct Shop     shoptab[500];
        int    shopcount;
        void __fastcall ProcessJiu(int ShopLb);
        int  __fastcall ReadShop(int ShopLbRno);  //读取指定类别的商品
        void __fastcall ShowShop(int Pno,int Pc,AnsiString ShopLb);        //显示商品
        void __fastcall AddJiu(int rno);  //增加一次酒水点击
        bool __fastcall AddJiuNum(struct Dan DBuffer);
        bool __fastcall checkUser(AnsiString Password,AnsiString No);
        bool __fastcall GetGh();
        bool            DrawPicture;   ///是否画界面
        AnsiString  GhValue;
        void __fastcall InterNet();
        struct InterNet IArray[1000];
        int    IArrayCount;
        void __fastcall ShowHomePage(int PageNo,int PageCount);
        void  __fastcall DrawText(int x,
                                  int y,
                                  int color,
                                  AnsiString Fontname,
                                  char * Text,
                                  int FontSize,
                                  char yz='0');
};
//---------------------------------------------------------------------------
#endif
