#ifndef STRUHEAD__
#define STRUHEAD__
#include "windows.h"
#define MYRUN        true   //����Ǳ������У���ĳ�TRUE �����ǳ�Ʒ�ģ���ĳ�FALSE
#define YS           false
#define PROGRUNPATH  "d:\\VOD\\"
#define GXPATH       "d:\\VOD\\gx.dbf"
#define GQPATH       "d:\\VOD\\gq.dbf"
#define XFKPATH      "d:\\VOD\\x\\xfk.dbf"
#define DANPATH      "d:\\VOD\\X\\dan\\"
#define VIPTABPATH   "d:\\VOD\\X\\mylove\\viptab.dbf"
#define SHOPLBPATH   "x:\\shoplb.dbf"
#define SHOPPATH     "x:\\shop.dbf"
#define KEYTABPATH   "d:\\VOD\\keytab.dbf"
#define MAPTABPATH   "d:\\VOD\\map.dbf"
#define SERMAPPATH   "d:\\vod\\sermap.dbf"    //��������MAP�ļ�
#define PARATABPATH  "d:\\VOD\\paratab.dbf"
#define COPYPATH     "x:\\COPYDATA\\"
#define POPERPATH    "d:\\VOD\\poper.dbf"
#define MOUSETABPATH "d:\\VOD\\mousetab.dbf"
#define FWHJPATH     "x:\\fwhj.dbf"
#define INTERPATH    "d:\\vod\\internet.dbf"
struct EventStru
   {
     HANDLE KeyHandle;
     int    KeyValue;
     int    KeyValue1;   //˫��ֵʱʹ��
     int    KeyValueCount; //��ʾ��ֵ�Ķ��ٸ�
     int    KeyType;
   };
struct ShowPictureStru
   {
     int x;
     int y;
     int type;
     char fname[1024];
   };
struct DrawTextStru
   {
     int x;
     int y;
     int Color;
     char Text[1024];
     char FontName[1024];  //��������
     int  fonttype;        //0��ʾʮ��λ   1 ��ʾ24λ    2��ʾ32����  3.��ʾ��ѡ
     int  FontSize;        //��ѡʱ�������С
     int  Len;
   };
#define KeyTabHead   129   //KeyTab.dbf�ļ���ͷ�ļ���С
struct KeyTabStru
   {
     char delflag;
     char KeyChar[10];  //��Ӧ���ַ�
     char KeyDown[10];  //��Ӧ���ַ�����ֵ
     char common[20];   //ע��
   };
#define SongTabHead   417
struct SongTab
   {
     char delflag;
     char GM[40];
     char BH[10];
     char ZHPY[4];
     char GX[10];
     char LB[10];
     char YZ[10];
     char YSSD[2];
     char BCSD[2];
     char ZS[4];
     char DCCS[10];
     char YLDX[2];
     char VIP[1];
   };
struct SelSongTab      //��ѡ�������кͲ��Ÿ�������
 {
   struct SongTab A;
   int    Stat;        //״̬,���ڴ�ѡ����,1��ʾѡ��,���ڼ�ѡ����,1��ʾ���ڲ���,2��ʾ�������
   unsigned long PlayOff;  //���ŵ�ƫ����
   int    RecNo;       //��¼��
 };
#define SingerHeadSize 193
struct SingerTab
 {
   char delflag;
   char xm[24];
   char xb[1];
   char sp[1];
   char qp[12];
   char dccs[8];
 };

struct ph     //�������н����Ľṹ
 {
   unsigned int rno;   //�����ļ�¼��
   unsigned int cs;    //�㳪����
 };
struct Xf
 {
   char shopname[100];   //��Ʒ����
   char shopdw[8];       //��Ʒ��λ
   char shopdj[10];      //��Ʒ����
   char shopsl[10];      //��Ʒ����
   char shopzj[10];      //��Ʒ�ܼ�
   char shopzt[4];       //��Ʒ״̬
   char shoprq[10];      //��Ʒ��ȡ����
   char shopsj[10];      //��Ʒ��ȡʱ��
   char shopczy[10];     //��Ʒ��ȡ�Ĳ���Ա���
 };
struct WaveHead   //WAVE�����ļ���ͷ����
  {
   char  RIFF[4];
   DWORD BLOCKSIZE;
   char  WAVE[4];
   char  fmt[4];
   DWORD SIZE16;
   WORD  FormatFlag;
   WORD  Chanel;
   DWORD SamplesPerSec;
   DWORD AvgBytesPerSec;
   WORD  BlockAlign;
   WORD  BitsPerSample;
   char  datastr[4];
   DWORD DataSize;
  };
#define MapDriveHead 161
struct MapDrive
   {
    char DelFlag;
    char RemoteIP[48];
    char RemotePath[100];
    char MapDrive[2];
    char MapOk;
   };
#if YS
#define MYLOVEMAX  2
#else
#define MYLOVEMAX  200
#endif
struct MyLoveFile
   {
    char Password[100];                //����
    int  Rcount;                       //��¼����
    struct SongTab A[MYLOVEMAX];       //��������׸���
    int  RecNo[MYLOVEMAX];             //��¼��
    unsigned long PlayOff[MYLOVEMAX];  //����ƫ��
   };
#define VIPTABHeadSize 353
 struct VIPTAB
  {
    char delfag;
    char bh[10];
    char mm[10];
    char sm[40];
    char bl1[10];
    char bl2[20];
    char bl3[30];
    char bl4[2];
    char bl5[1];
    char bl6[3];
    char bl7[5];
  };
#define SHOPLBHEADSIZE 129
struct shoplb
  {
   char delfalg;
   char bh[10];
   char sm[20];
   char pict[100];
  };
#define ShopHeadSize 289
   struct Shop   //��Ʒ��
   {
     char DelFlag;
     char ShopName[40];  //��Ʒ����
     char ShopBh[5];     //��Ʒ���
     char ShopDj[8];     //��Ʒ����
     char ShopDz[3];     //��Ʒ������
     char ShopSm[40];    //��Ʒ˵��
     char ShopKc[10];    //��Ʒ�������
     char ShopLb[10];    //��Ʒ���
     char ShopDw[4];     //��Ʒ��λ
   };
#define DanHeadSize 449
   struct Dan            //�û��㵥�����ݿ�
   {
     char DelFlag;       //ɾ����־
     char RoomName[10];  //��������
     char ShopName[20];  //��Ʒ����
     char ShopDj[8];     //��Ʒ����
     char ShopDz[3];     //��Ʒ����
     char ShopDw[4];     //��Ʒ��λ
     char ShopSl[5];     //��Ʒ����
     char ShopZj[10];    //��Ʒ�ܼ�
     char Gh[10];        //����
     char bl1[3];
     char bl2[6];
     char bl3[9];
     char bl4[10];
     char bl5[10];
   };
#define XfkHeadSize 449
   struct Xfk
   {
     char DelFlag;
     char JiuName[20];  //��ˮ����
     char Sl[3];        //����
     char Dw[4];        //��ˮ��λ
     char Dj[7];        //��ˮ����
     char Zj[7];        //������ֵ
     char Zt[4];        //��ˮ״̬
     char Fm[10];       //��������
     char Sj[20];       //����ʱ��
     char Czy[20];      //����Ա��Ҳ��������ƷԱ��
     char Sfjz;         //�Ƿ����
     char Zdh[10];      //�ʵ���
     char dzl[3];       //������
     char fwfzk[3];     //������ۿ�
   };
//���������е�������������
struct QQDG              //������ĸ�����
   {
      char SongName[100];   //����Ҫ��ĸ����ı��
      unsigned long BH;     //����İ��ı��
      int  KD;              //����״̬���ɷ�������Ӧ��NOPACK/OKPACK/WAITPACK
      char Song[100];       //Զ�̸�����ȫ������������Ӧ������KD=OKPACKʱ����ʹ��
   };
struct SearchPath         //����·���ṹ
   {
    char Path[40];          //����·����
    int MaxClick;           //��·�����ĵ㲥��
   };
struct LoginPack
   {
    char RemoteName[20];   //Զ���û�������"\\888lfw\"
    int  DriveCount;       //ӵ������·���ĸ���
    struct SearchPath Sp[24]; //���24������·��
   };
#define ParaTabHeadSize 97
struct ParaTab
   {
    char delflag;
    char sm[20];
    char v1[100];
   };
#define MouseTabHeadSize 353
struct MouseTab
   {
    char delflag;
    char X[3];        //ͼƬX����
    char Y[3];        //ͼƬY����
    char X1[3];       //ͼƬ�ұ�����
    char Y1[3];       //ͼƬ���½�����
    char PICT[200];   //��Ӧ���Ǵ�͸ͼƬ,�������Ҫ��͸ͼƬ�жϣ���˴�Ϊ��
    char KEYDOWN1[4]; //��Ӧ�ļ�ֵ1
    char KEYDOWN2[4]; //��Ӧ�ļ�ֵ2
    char KEYCOUNT;    //��Ӧ�ļ�ֵ����
    char OPERCODE[3]; //���ڵڼ����������
    char SM[20];
   };
#define POPERTabeHeadSize 193
struct POPERTab
   {
    char delfalg;
    char pictname[200];
    char opercode[3];
    char bl1[5];
    char bl2[10];
    char bl3[5];
   };
#define JIUYHHeadSize  257
  struct JiuYhStru
   {
    char delflag;
    char gh[10];
    char mm[10];
    char bl1[10];
    char bl2[10];
    char bl3[10];
    char bl4[10];
    char bl5[10];
   };
#define AUTOPLAY 3   //�Զ�����
#define QUITPLAY 4   //ֹͣ���ţ����������
#define CLOSEPROG 5  //�رճ���
#define PLAYONEFILE 6  //����ָ���ļ�
#define SYSTEMRESET 7  //���³�ʼ��ϵͳ����
#define FORWARD     8  //���
#define BACKWARD    9  //����
#define MUSICECHANGE 10 //�����л�
#define TVAVCHANGE   11 //TVAV�л�
#define ENDPLAY      12 //��ֹ�����ļ�
#define REPLAYSONG   13 //�طŸ���
#define PAUSESONG    14 //��ͣ����
#define VOLUMEUP     15 //����
#define VOLUMEDOWN   16 //��С
#define GETMSG       17 //��ȡԶ����Ϣ
#define RESET        18 //Զ�̼������������
#define CLOSECOMPUTER 19 //Զ�̼�����ػ�

#define FWHJHEADSIZE  513
struct fwhj
{
 char delflag;
 char room[10];
 char hjcs[3];
 char s1[10];
 char s2[10];
 char s3[10];
 char s4[10];
 char s5[10];
 char s6[10];
 char s7[10];
 char s8[10];
 char s9[10];
 char s10[10];
 char s11[10];
 char s12[10];
 char s13[10];
};
#define INTERNETHEAD 193
struct InterNet
 {
  char delflag;
  char sm[40];
  char wz[200];
  char bl1[10];
  char bl2[20];
  char bl3[50];
 };
#endif
