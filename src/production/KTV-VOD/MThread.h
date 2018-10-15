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
        void __fastcall Process1();  //�������ƴ�����
        void __fastcall ShowPicture(int x,int y,AnsiString Fname);     //��ʾͼƬ
        void __fastcall TranShowPicture(int x,int y,AnsiString Fname); //��͸��ʾͼƬ
        void __fastcall DrawText16(int x,int y,int color,char *Fontname,char *Text);
        void __fastcall DrawText24(int x,int y,int color,char *Fontname,char *Text);
        void __fastcall DrawText32(int x,int y,int color,char *Fontname,char *Text);
        int  __fastcall ReadSongTab(AnsiString Field,AnsiString Value,int Compare);  //��ȡ��������
        void __fastcall DisplayMsg(AnsiString Msg);  //��ʾ��ʾ��Ϣ
        void __fastcall ProcessGQ();               //�������
        void __fastcall ShowSelSong(int PageNo,int SelNo,int stat=0);
        void __fastcall Process2();                //�����������
        void __fastcall Process3();                //������ǵ��
        int __fastcall ReadSinger(int fno,AnsiString value);//��ȡ���ǿ�
        struct SingerTab Singer[1000];
        int    SingerCount;
        void __fastcall ShowSinger(int pageno,int selno);
        void __fastcall Process4();   //���ѡ��
        void __fastcall Process5();   //����ѡ��
        void __fastcall Process6();
        void __fastcall ProcessBh();  //�����ŵ��
        void __fastcall SongAddClick(int rno);
        void __fastcall Process8();
        void __fastcall ShowPlaySong(int PageNo,int SelNo);  //��ʾ��ѡ����
        int  __fastcall FilterPy(char Py);    //���˸�����ƴ��
        int  __fastcall FilterZs(char zs);    //��������
        char __fastcall ReadZs();
        char __fastcall ReadPy();
        int  __fastcall SingerFilterPy(char py);  //����ƴ��
        void __fastcall Process7();
        void __fastcall ShowXf(int PageNo);
        void __fastcall ReadXf();  //��ȡ��������
        char ComputerName[1024];   //�����������
        struct Xf  XfArray[MAXXFRECCOUNT];  //�������������
        int    XfCount;            //���Ѽ�¼����
        void __fastcall ShowNoSong(int pageno,int selno);
        void __fastcall ProcessDJ(char Key);
        int           SongPlayStat;   //��������״̬,0��ʾ�������� 1��ʾ��� 2��ʾ����
        int           SongVolumeSize; //����������С
        int           Mute;
        unsigned char PlayStream;     //����������

        bool          BackMusice;
        void       __fastcall BackPlaySong(int Rno);
        bool       BackPlay;  //�ط�
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
        struct MyLoveFile MLF;   //�ҵ���е��ļ�
        void __fastcall VIPMyLove();
        void __fastcall DrawMyLoveEdit(int stat,int operno);
        void __fastcall MyLoveKey(int Key,int operno);
        void __fastcall ProcessMyLoveReg();
        void __fastcall ProcessMyLoveLogin();  //�����ҵ����¼
        void __fastcall ProcessMyLoveClick(AnsiString yh);  //�����ҵ������
        bool __fastcall MyLoveAdd(int rno);    //�ڼ�ѡ�������ҵ����½
        void __fastcall ProcessVIP();

        struct shoplb   shoplbTab[100];
        int    shoplbcount;
        void __fastcall ProcessShowJiuLb();   //�����ˮ����
        void __fastcall ShowShopLb(int pno,int pc);

        struct Shop     shoptab[500];
        int    shopcount;
        void __fastcall ProcessJiu(int ShopLb);
        int  __fastcall ReadShop(int ShopLbRno);  //��ȡָ��������Ʒ
        void __fastcall ShowShop(int Pno,int Pc,AnsiString ShopLb);        //��ʾ��Ʒ
        void __fastcall AddJiu(int rno);  //����һ�ξ�ˮ���
        bool __fastcall AddJiuNum(struct Dan DBuffer);
        bool __fastcall checkUser(AnsiString Password,AnsiString No);
        bool __fastcall GetGh();
        bool            DrawPicture;   ///�Ƿ񻭽���
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
