//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "MThread.h"
#include "MForm.h"
#include "head.h"
#include "dir.h"
#include "stdio.h"
#include "stdlib.h"
#include "newdvd.cpp"
#include "pdata.cpp"
#include "iphlpapi.h"


#include "netmsg.h"
#define TMPWAVEBUFFSIZE 1024*512
#define MAXCOMPUTER     1     //总共拥有的计算机总数
#define RemoteLoginPort 8300
#define RemotePortNo    8304
#define RecvNetMsgPort  8301
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "SHDocVw_OCX"
#pragma resource "*.dfm"
USEFORM("tiao.cpp", impo);
TForm1 *Form1;
MainThread *MainTh;
//==============================================
DWORD VolserialArray[]={
626266605,
};
char *AdapterNameArray[]={
"{CBA0E79E-C85B-434C-8F2F-02EB932E1D69}",
};
char Id[][8]={
{0x0,0x90,0xf5,0x29,0xbe,0x3,0x0,0x0},
};

//==============================================
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
 initSystem();
 FILE *FP;
 KeyTabCount=0;
 RecordFP=NULL;
 RecordMusice=false;
 waveBuff=(char *)malloc(sizeof(char)*TMPWAVEBUFFSIZE);waveIn=NULL;  
 memset((void *)&KTS,0,sizeof(struct KeyTabStru)*100);
 FP=fopen(KEYTABPATH,"rb");
 if(FP!=NULL)
 {
   fseek(FP,4,SEEK_SET);
   fread((void *)&KeyTabCount,sizeof(int),1,FP);
   for(int k=0;k<KeyTabCount;k++)
    {
     fseek(FP,KeyTabHead+sizeof(struct KeyTabStru)*k,SEEK_SET);
     fread((void *)&KTS[k],sizeof(struct KeyTabStru),1,FP);
     for(int l=9;l>0;l--)
     {
      if(KTS[k].KeyChar[l]==' ')
        KTS[k].KeyChar[l]=0;
      if(KTS[k].KeyDown[l]==' ')
        KTS[k].KeyDown[l]=0;
     }
    }
   fclose(FP);
 }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormShow(TObject *Sender)
{
 Timer1->Enabled=false;
 Timer2->Enabled=false;
 Panel1->Visible=false;
 MouseOperCode=-1;
 RecordMusice=false;  //没有录音
 BackMusice=false;     //没有背景音乐
 BackPlay=false;      //没有回放
 Width=800;
 Height=600;
 Top=0;
 Left=0;
 ReadPath(MAPTABPATH);      //读取服务器路径
 DelRecordFile(); //删除录音文件
 ZhGuangGao();  //转换广告
 MyCopyFile();  //拷贝数据
 OpenDrive();     //打开神龙卡

 ReadMouseTab();  //读取键盘操作表
 MainThreadStat=0;  //默认线程处于欢迎界面
#if MYRUN
 ES.KeyHandle=CreateEvent(NULL,false,false,NULL);
 memset((void *)&SPS,0,sizeof(struct ShowPictureStru));
 MainTh=new MainThread(false,&ES,&SPS,&TS);
#else
 if(CheckComputer()==false) return;
#endif

 Timer1->Enabled=true;
 Timer2->Enabled=true;

}
//---------------------------------------------------------------------------
void __fastcall TForm1::ShowPicture(int x,int y,char *fname)
{
 //在屏幕上显示一副图片
 struct ffblk fb;
 TRect R,R1;
 int i;
 i=GetMouseOperCode(fname);
 if(i!=-1)MouseOperCode=i;   //如果不是负数，则获取鼠标对应的大系统坐标
 Graphics::TBitmap *TB;
 TB=new Graphics::TBitmap();
 if(findfirst(fname,&fb,FA_ARCH|FA_RDONLY)!=0)return; //没有找到歌曲，则不做任何处理
 TB->LoadFromFile(fname);
 if(x!=0||y!=0)
 {
  R.top=y;
  R.left=x;
  R.right=x+TB->Width-1;
  R.bottom=y+TB->Height-1;
  R1.top=0;
  R1.left=0;
  R1.right=TB->Width;
  R1.bottom=TB->Height;
  Canvas->CopyRect(R,TB->Canvas,R1);
 }
 else
 {
  int dh;
  //if(DrawDhFlag)dh=rand()%5;
  //dh=0;
  dh=0;
  switch(dh)
  {
   case 0:  //没有动画
       R.top=y;
       R.left=x;
       R.right=x+TB->Width-1;
       R.bottom=y+TB->Height-1;
       R1.top=0;
       R1.left=0;
       R1.right=TB->Width;
       R1.bottom=TB->Height;
       Canvas->CopyRect(R,TB->Canvas,R1);break;
   case 1: //显示随机块动画
       DrawDH2(TB,x,y);break;
   case 2: //显示拉帘动画
       DrawDH3(TB,x,y );break;
   case 3:
       DrawDH3(TB,x,y);break;
   case 4:
       DrawDH4(TB,x,y);break;
  }
 }
 delete TB;
}

void __fastcall TForm1::TranShowPicture(int x,int y,char *fname)
{
 //在屏幕上显示一副图片,是穿透显示
 struct ffblk fb;
 char *B1,c1,c2,c3,*B2;
 TRect R,R1;
 Graphics::TBitmap *TB,*TB2;
 TB=new Graphics::TBitmap();
 if(findfirst(fname,&fb,FA_ARCH|FA_RDONLY)!=0)return; //没有找到歌曲，则不做任何处理
 TB->PixelFormat=pf24bit;
 TB->LoadFromFile(fname);
 TB2=new Graphics::TBitmap();
 TB2->PixelFormat=pf24bit;
 TB2->Width=TB->Width;
 TB2->Height=TB->Height;
 R.top=y;
 R.left=x;
 R.bottom=y+TB->Height-1;
 R.right=x+TB->Width-1;
 R1.top=0;
 R1.left=0;
 R1.bottom=TB->Height-1;
 R1.right=TB->Width-1;
 TB2->Canvas->CopyRect(R1,Canvas,R);
 B1=(char *)TB->ScanLine[0];
 c1=B1[0];
 c2=B1[1];
 c3=B1[2];
 for(int i=0;i<TB->Height;i++)
 {
  B1=(char *)TB->ScanLine[i];
  B2=(char *)TB2->ScanLine[i];
  for(int j=0;j<TB->Width;j++)
   {
    if((B1[j*3]&0xff)==(c1&0xff)&&
       (B1[j*3+1]&0xff)==(c2&0xff)&&
       (B1[j*3+2]&0xff)==(c3&0xff))
       {
        continue;
       }
       B2[j*3]=B1[j*3];
       B2[j*3+1]=B1[j*3+1];
       B2[j*3+2]=B1[j*3+2];
   }
 }
 R.top=y;
 R.left=x;
 R.right=x+TB2->Width-1;
 R.bottom=y+TB2->Height-1;
 R1.top=0;
 R1.left=0;
 R1.right=TB2->Width-1;
 R1.bottom=TB2->Height-1;
 Canvas->CopyRect(R,TB2->Canvas,R1);
 delete TB;
 delete TB2;
}

void __fastcall TForm1::DrawText24(int x,int y,int Color,char *FontName,char *Text)
{
//显示字体
Canvas->Brush->Style=bsClear;
Canvas->Pen->Style=psClear;
Canvas->Font->Size=24;
Canvas->Font->Charset=GB2312_CHARSET;
Canvas->Font->Color=TColor(Color);
Canvas->Font->Name=FontName;
Canvas->Font->Style=TFontStyles()<<fsBold;
Canvas->TextOutA(x,y,Text);
}

void __fastcall TForm1::TText()
{
 //显示字体
 switch(this->TS.fonttype)
  {
   case 0:  //十六点阵
        DrawText16(TS.x,TS.y,TS.Color ,TS.FontName,TS.Text);break;
   case 1:
        DrawText24(TS.x,TS.y,TS.Color ,TS.FontName,TS.Text);break;
   case 2:
        DrawText32(TS.x,TS.y,TS.Color ,TS.FontName,TS.Text);break;
   case 3:
        ADrawText(TS.x,TS.y,TS.Color ,TS.FontName,TS.Text,TS.FontSize);break;
   case 4:
        BDrawText(TS.x,TS.y,TS.Color ,(wchar_t *)TS.Text,TS.Len);break;
  }
}

void __fastcall TForm1::ADrawText(int x,int y,int Color,char *FontName,char *Text,int FontSize)
{
//显示字体,显示中文字体
Canvas->Brush->Style=bsClear;
Canvas->Pen->Style=psClear;
Canvas->Font->Size=FontSize;
Canvas->Font->Color=TColor(Color);
Canvas->Font->Name =FontName;
Canvas->Font->Charset=GB2312_CHARSET;
Canvas->TextOutA(x,y,Text);
}
void __fastcall TForm1::DrawText32(int x,int y,int Color,char *FontName,char *Text)
{
//显示字体
Canvas->Brush->Style=bsClear;
Canvas->Pen->Style=psClear;
Canvas->Font->Size=32;
Canvas->Font->Color=TColor(Color);
Canvas->Font->Charset=GB2312_CHARSET;
Canvas->Font->Name =FontName;
Canvas->TextOutA(x,y,Text);
}
void __fastcall TForm1::DrawText16(int x,int y,int Color,char *FontName,char *Text)
{
//显示字体
Canvas->Brush->Style=bsClear;
Canvas->Font->Style=TFontStyles()<<fsBold;
Canvas->Pen->Style=psClear;
Canvas->Font->Charset=GB2312_CHARSET;
Canvas->Font->Size=20;
Canvas->Font->Color=TColor(Color);
Canvas->Font->Name =FontName;
Canvas->TextOutA(x,y,Text);
}

void __fastcall TForm1::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
 if(Tag>0)return;
 bool found=false;
 ES.KeyValue=Key;
 ES.KeyType=0;
 for(int l=0;l<KeyTabCount;l++)
 {
  if(atoi(KTS[l].KeyDown)==Key)  //房态是进场，才进行处理
  {
    ES.KeyValue=int(KTS[l].KeyChar[0]&0xff);
    if(ES.KeyValue>='a'&&ES.KeyValue<='j')ES.KeyType=-4;  //快捷键
    found=true;
    break;
  }
 }
 if(found==false)return;     //不是键盘表上的数据，则不做任何处理c
 if(RoomStat==1)
 {
  if(ProcessDJ(ES.KeyValue))return;  //DJ控制处理
 }
 if(ES.KeyValueCount>0)return;
 ES.KeyValueCount=1;
 SetEvent(ES.KeyHandle);     //其它处理，则设置信号
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TDP()
{
//供远程线程使用的显示图片程序
 switch(SPS.type)
  {
   case 0:    ShowPicture(SPS.x,SPS.y,SPS.fname);    break;//
   case 1:TranShowPicture(SPS.x,SPS.y,SPS.fname);    break;//
  }
}

void __fastcall TForm1::FormPaint(TObject *Sender)
{
  //刷新界面
  if(ES.KeyValueCount>0)return;
  ES.KeyType=0;
  ES.KeyValue='@';
  ES.KeyValueCount=1;
  SetEvent(ES.KeyHandle);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::StartRecord()
{
 AnsiString RF;
 char buff[1024];
 waveFormat.wFormatTag=WAVE_FORMAT_PCM;
 waveFormat.nChannels=2;
 waveFormat.nSamplesPerSec=11025*4;
 waveFormat.nAvgBytesPerSec=11025*4;
 waveFormat.nBlockAlign=1;
 waveFormat.wBitsPerSample=16;
 waveFormat.cbSize=0;
 bytecount=0;
 if(waveInOpen(&waveIn,WAVE_MAPPER,&waveFormat,(DWORD)Handle,0,CALLBACK_WINDOW))  //设备打开失败
  {
   RecordFP=NULL;
   RecordMusice=false;
   return;
  }
 //下面设置录音区，开始录音
 RF="d:\\rm\\";
 RF=RF+RecordSongNo.Trim();
 RF=RF+".wav";
 RecordFP=fopen(RF.c_str(),"w+b");
 if(RecordFP==NULL)return;
 memset((void *)&waveHdr,0,sizeof(WAVEHDR));
 memset((void *)&wavehead,0,sizeof(struct WaveHead));
 fwrite((void *)&wavehead,sizeof(struct WaveHead),1,RecordFP);  //写入文件头
 waveHdr.lpData=waveBuff;
 waveHdr.dwBufferLength=TMPWAVEBUFFSIZE;
 waveHdr.dwBytesRecorded=0;
 waveHdr.dwUser=0;waveHdr.dwFlags=0;
 waveHdr.dwLoops=1;
 waveHdr.lpNext=NULL;
 waveHdr.reserved=0;
 if(waveInPrepareHeader(waveIn,&waveHdr,sizeof(WAVEHDR)))
   {
    fclose(RecordFP);
    RecordFP=NULL;
    RecordMusice=false;
    return;
   } //建立录音缓冲区头失败
 if(waveInAddBuffer(waveIn,&waveHdr,sizeof(WAVEHDR)))
   {
    fclose(RecordFP);
    RecordFP=NULL;
    RecordMusice=false;
    return ;
   }//加入缓冲区失败
 if(waveInStart(waveIn))
   {
     fclose(RecordFP);
     RecordFP=NULL;
     RecordMusice=false;
     return ;
   }  //开始录音失败
 RecordMusice=true;
}
void __fastcall TForm1::SaveRecord()   //保存录音
{
 //保存录音文件
 if(RecordMusice)
 {
  waveInStop(waveIn);
  waveInReset(waveIn); //刷新一次采集，终止数据
  waveInUnprepareHeader(waveIn,&waveHdr,sizeof(WAVEHDR));//释放资源
  waveInClose(waveIn);
  struct WaveHead wavehead;
  memset((void *)&wavehead,0,sizeof(struct WaveHead));
  strncpy(wavehead.RIFF,"RIFF",4);
  strncpy(wavehead.WAVE,"WAVE",4);
  strncpy(wavehead.fmt,"fmt ",4);
  strncpy(wavehead.datastr,"data",4);
  wavehead.BLOCKSIZE=bytecount-8+sizeof(struct WaveHead);
  wavehead.SIZE16=16;
  wavehead.FormatFlag=WAVE_FORMAT_PCM;
  wavehead.Chanel=2;
  wavehead.SamplesPerSec=11025*4;
  wavehead.AvgBytesPerSec=11025*4;
  wavehead.DataSize=bytecount;
  wavehead.BlockAlign=1;
  wavehead.BitsPerSample=16;
  if(RecordFP!=NULL)
  {
    fseek(RecordFP,0,SEEK_SET);
    fwrite((void *)&wavehead,sizeof(struct WaveHead),1,RecordFP);  //重新写入头数据
    fclose(RecordFP);RecordFP=NULL;
  }
 }
 bytecount=0;
 RecordMusice=false;
}

bool TForm1::CacheData(TMessage Msg)
{
        //TODO: Add your source code here
  bytecount=bytecount+waveHdr.dwBytesRecorded;
  if(RecordFP!=NULL)
  fwrite((void *)waveBuff,sizeof(char),waveHdr.dwBytesRecorded,RecordFP);//写进硬盘
  waveInAddBuffer(waveIn,&waveHdr,sizeof(WAVEHDR));
  return true;
}

void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    if(ReadPara("关机断开X盘")=="1")WNetCancelConnection("x:",true);
    if(Tag>0)  //如果被设置成关机信号
    {
       StopBackPlay();
       SaveRecord();
       QuitVideo(PlayStream);
       PL_ExitWindow(EWX_POWEROFF|EWX_SHUTDOWN|EWX_LOGOFF);
       return;
    }
    else
    {
    Timer1->Enabled=false;
    if(ES.KeyValueCount>0){Timer1->Enabled=true;return;}
    ES.KeyType=-3;
    ES.KeyValue='\0';
    ES.KeyValueCount=1;
    SetEvent(ES.KeyHandle);
    QuitVideo(PlayStream);
    StopBackPlay();
    SaveRecord();
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BackPlaySong()
{
 //回放歌曲
 struct ffblk fb;
 int i;
 char  wjm[1024],buff[1024];
 AnsiString command;
 if(RecordMusice)
  {
   SaveRecord();
   bytecount=0;
   RecordMusice=false;
  } //如果以前在录音，则保存录音数据
 mciSendString("stop mysound",buff,1024,NULL);
 mciSendString("close mysound",buff,1024,NULL);
 Timer1->Enabled=false;
 Timer2->Enabled=false;
 command=FindSong(BackPlaySongNo.Trim());
 Timer1->Enabled=true;
 Timer2->Enabled=true;
 if(command=="")return; //不能回放
 OpenVideo(command.c_str(),PlayStream);
 SelectTV(TVAV_STAT,PlayStream);
 SetVolume(PlayStream,0);
 command="open d:\\rm\\";
 command=command+BackPlaySongNo.Trim()+".wav alias mysound";
 mciSendString(command.c_str(),buff,1024,NULL);
 mciSendString("play mysound",buff,1024,NULL);
 BackPlay=true;
}
//-------------------------------------------------------------------------
void __fastcall TForm1::StopBackPlay()
{
//停止播放录音
 if(BackPlay)
 {
 char buff[1024];
 memset(buff,0,1024);
 mciSendString("stop mysound",buff,1024,NULL);
 mciSendString("close mysound",buff,1024,NULL);
 BackPlay=false;
 }
}

void __fastcall TForm1::ProcessPlaySong()
{
  AnsiString wjm;
  char buff[100];
 if(BackMusice==false) //如果在播放正常的歌曲
   {
     if(SongPlayStat==1)Forward(PlayStream,1);  //快进
     if(SongPlayStat==2)Backward(PlayStream,1); //快退
     if(PlayStream!=0&&GetState(PlayStream)==FMPF_PLAYING) return; //如果正在播放，则将退出
     if(PlayStream!=0&&GetState(PlayStream)==FMPF_PAUSED) return;   //暂停也操作
   }
 if( PlaySongNo>0)
  {
   if(PlaySongTab[PlaySongNo-1].Stat==1)
    {
     SongPlayStat=0;
     MainTh->PlaySongEnd();
     if(ES.KeyValueCount>0)return;
     ES.KeyType=0;
     ES.KeyValue='@';
     ES.KeyValueCount=1;
     SetEvent(ES.KeyHandle);
    }
   PlaySongTab[ PlaySongNo-1].Stat=2;
  }
 if( PlaySongCount> PlaySongNo)  //如果有歌曲可以播放
  {
   memset(buff,0,100);
   strncpy(buff, PlaySongTab[ PlaySongNo].A.BH,10);
   Timer1->Enabled=false;
   Timer2->Enabled=false;
   wjm=FindSong(buff);
   Timer1->Enabled=true;
   Timer2->Enabled=true;
   PlaySongTab[ PlaySongNo].Stat=1;          //设置为正在播放
   if(BackPlay)StopBackPlay();   //以前在回放，则停止回放
   if(RecordMusice)SaveRecord(); //以前在录音，则停止录音
   if(wjm!="")
   {
    OpenVideo(wjm.c_str(),PlayStream);SelectTV(TVAV_STAT,PlayStream);
    memset(buff,0,100);
    if( PlaySongTab[ PlaySongNo].A.BCSD[0]=='1')SAudio(PlayStream);
    if( PlaySongTab[ PlaySongNo].A.BCSD[0]=='2'){SAudio(PlayStream);SAudio(PlayStream);}  //设置声道
    strncpy(buff, PlaySongTab[ PlaySongNo].A.YLDX,2);
    SetVolume(PlayStream,atoi(buff)*2);     //设置音量大小
    SongVolumeSize=atoi(buff);
    SongPlayStat=0;
    memset(buff,0,100);
    strncpy(buff, PlaySongTab[ PlaySongNo].A.BH,10);
    RecordSongNo=buff;
    RecordSongNo=RecordSongNo.Trim();
    StartRecord();  //开始录音
    }
   BackMusice=false;
   SongPlayStat=0;
   MainTh->PlaySongEnd();
   PlaySongNo++;
  }
 else                         //否则播放背景音乐
  {
   if(PlayStream!=0&&GetState(PlayStream)==FMPF_PLAYING) return; //如果正在播放，则将退出
   if(PlayStream!=0&&GetState(PlayStream)==FMPF_PAUSED) return;   //暂停也操作
   if(BackPlay)StopBackPlay();   //以前在回放，则停止回放
   if(RecordMusice)SaveRecord(); //以前在录音，则停止录音
   OpenVideo(Gswjm.c_str(),PlayStream);SelectTV(TVAV_STAT,PlayStream);
   SetVolume(PlayStream,Gsyldx);
   SongVolumeSize=Gsyldx;BackMusice=true;
  }
}

AnsiString __fastcall TForm1::FindSong(AnsiString No)
{
  //根据编号搜索歌曲
  //如果找到则返回歌曲的绝对路径名
int Rj;
static int PackNo=0;
if(zysy=="1")  //使用虚拟技术
{
  Timer1->Enabled=false;
  Timer2->Enabled=false;
  //首先搜索D盘下面的歌曲
  struct ffblk fb;
  char buff[1024];
  AnsiString msg;
  for(char i='C';i<'Z';i++)
  {
    AnsiString wjm;
    memset(buff,0,1024);
    buff[0]=i&0xff;
    buff[1]=':';
    buff[2]='\\';
    buff[3]=0;
    if(GetDriveType(buff)!=DRIVE_FIXED)continue;
    strcat(buff,No.Trim().c_str());
    strcat(buff,".*");
    if(findfirst(buff,&fb,FA_RDONLY|FA_ARCH)==0)
    {
      memset(buff,0,1024);
      buff[0]=i&0xff;
      buff[1]=':';
      buff[2]='\\';
      buff[3]=0;
      strcat(buff,fb.ff_name);
      wjm=buff;
      return wjm;
    }
  }
  FindSongBuff="";
  memset(&Send,0,sizeof(struct QQDG));
  memset(&Recv,0,sizeof(struct QQDG));
  strcat(Send.SongName,No.Trim().c_str());
  Send.BH=PackNo;
  PackNo++;
  Send.KD=-1;
  SendData(RemoteHostIP,RemotePortNo,(char *)&Send,sizeof(struct QQDG));
  memset((void *)&Recv,0,sizeof(struct QQDG));
  WaitRecvData(RecvNetMsgPort,(char *)&Recv,sizeof(struct QQDG),1000);
  FindSongBuff=Recv.Song;
  Timer1->Enabled=true;
  Timer2->Enabled=true;
  return FindSongBuff;
}
if(zysy=="0"||zysy=="") //使用分布式系统
{
  struct ffblk fb;
  char buff[1024];
  AnsiString msg;
  for(int i=0;i<PathCount;i++)
  {
    AnsiString wjm;
    memset(buff,0,1024);
    strcat(buff,Path[i].RemotePath);

    wjm=buff;
    wjm=wjm.Trim();
    wjm=wjm+No.Trim()+".*";
    if(findfirst(wjm.c_str(),&fb,FA_RDONLY|FA_ARCH)==0)
    {
      memset(buff,0,1024);
      strcat(buff,Path[i].RemotePath);
      msg=buff;
      msg=msg.Trim();
      msg=msg+fb.ff_name;
      return msg;
    }
  }
  msg="";
  return msg;
 }
return "";
}
//--------------------------------------------------------------------------
bool __fastcall TForm1::ProcessDJ(char Key)
{
struct vpoint
{
 int x,y;
 char Key;
 char *pict;
 bool dj;
};
struct vpoint sz[]=
{
 {113,509,'O',"m_1.bmp",true},  //退碟
 {173,505,'M',"m_2.bmp",true},  //静音
 {244,503,'Q',"m_3.bmp",true},  //服务
 {326,498,'H',"m_4.bmp",true},  //快进
 {398,501,'I',"m_5.bmp",true},  //快退
 {469,501,'L',"m_6.bmp",true},  //暂停
 {555,504,'N',"m_7.bmp",true},  //声道
 {624,509,'P',"m_8.bmp",true},  //重放
 {4  ,561,'K',"m_9.bmp",true},  //音小
 {87 ,559,'J',"m_10.bmp",true}, //音大
 {668,549,'B',"m_11.bmp",false}, //上翻
 {732,546,'A',"m_12.bmp",false}  //下翻
};
bool founddj=false;
for(int i=0;i<12;i++)
 {
  if(Key==sz[i].Key)
   {
    if(MainThreadStat==1)
    {
      AnsiString lj;
      lj=AnsiString(PROGRUNPATH)+"\\picture\\b\\";
      lj=lj+sz[i].pict;
      TranShowPicture(sz[i].x,sz[i].y,lj.c_str());
      Sleep(100);
      lj=AnsiString(PROGRUNPATH)+"\\picture\\c\\";
      lj=lj+sz[i].pict;
      ShowPicture(sz[i].x,sz[i].y,lj.c_str());
    }
    founddj=sz[i].dj;    //获取按钮属性
    break;
   }
 }
switch(Key)
 {
   case 'H'://         72        快进
         if(BackPlay)break;
         if(SongPlayStat!=1)
            {
              SongPlayStat=1;
              Forward(PlayStream,1);
            }  //
         else
            {
              SongPlayStat=0;
              Forward(PlayStream,0);
            }  //
         break;
   case 'I'://         73        快退
         if(BackPlay)break;
         if(SongPlayStat!=2)
           {
             SongPlayStat=2;
             Backward(PlayStream,1);
           }
         else
           {
             SongPlayStat=0;
             Backward(PlayStream,0);
           }
         break;
   case 'J'://         74        音大
         if(BackPlay)break;
         if((SongVolumeSize+1)<50)
           {
             SongVolumeSize++;
             SetVolume(PlayStream,SongVolumeSize*2);
           }
         break;
   case 'K'://         75        音小
         if(BackPlay)break;
         if(SongVolumeSize>0)
           {
             SongVolumeSize--;
             SetVolume(PlayStream,SongVolumeSize*2);
           }
         break;
   case 'L'://         76        暂停
         if(BackPlay)break;
         if(SongPlayStat!=3)
           {
            SongPlayStat=3;
            PauseVideo(PlayStream,1);
           }
         else
           {
            SongPlayStat=0;
            PauseVideo(PlayStream,0);
           }
         break;
   case 'M'://         77        静音
         if(BackPlay)break;
         if(Mute==0)
           {
             MuteAudio(PlayStream);
             Mute=1;
           }
         else
           {
             SetVolume(PlayStream,SongVolumeSize*2);
             Mute=0;
           }
         break;
   case 'N'://         78        声道
         if(BackPlay)break;
         SAudio(PlayStream);
         break;
   case 'O'://         79        退碟
         SongPlayStat=0;
         CloseVideo(PlayStream);
         BackMusice=true;        //设置为播放背景音乐
         if(BackPlay)StopBackPlay();
         break;
   case 'P'://         80        重放
         if(BackPlay)break;
         RePlay(PlayStream);
         SetVolume(PlayStream,SongVolumeSize*2);
         break;
   case 'Q'://         81        服务
         if(RoomStat==1) WriteFwhj();   //如果是进场状态
         if(ES.KeyValueCount>0) return false;
         ES.KeyValueCount=1;ES.KeyValue='Q';
         SetEvent(ES.KeyHandle);     //其它处理，则设置信号
         return false;
   case 'R'://         82        TV_AV切换
         break;
 }
 return founddj;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormDestroy(TObject *Sender)
{
  delete MainTh;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer2Timer(TObject *Sender)
{
//------------------下面是定时发送一次存在消息
static int TimeCount=0;
TimeCount++;
if(TimeCount>=5)
{
  char findname[100],cname[100];
  struct ffblk fb;
  struct LoginPack P;
  FILE *FP;
  int i;
  unsigned long l=100;
  GetComputerName(cname,&l);
  strcat(cname,".sg");
  FP=fopen(cname,"rb");
  if(FP!=NULL)
  {
    fread(&P,sizeof(struct LoginPack),1,FP);
    NMUDP1->RemoteHost=RemoteHostIP;
    NMUDP1->RemotePort=RemoteLoginPort;
    NMUDP1->SendBuffer((char *)&P,sizeof(struct LoginPack),sizeof(struct LoginPack));
    fclose(FP);
  }
  TimeCount=0;
}
//-----------------下面是进行状态查询
  if(Tag>0)                //如果有关机信号，则不正做处理
  {
   Tag++;
   if(Tag>=4)Close();
   return;
  }
  if(ES.KeyType==-4)return;
  ES.KeyType=0;
  if(RoomStat==1)ProcessPlaySong();              //每两秒钟查询一次歌曲播放
  if(RoomStat==1&&SyDh==1)
   {
    if(ES.KeyValueCount>0)return;
    ES.KeyValue='!';
    ES.KeyValueCount=1;
    SetEvent(ES.KeyHandle);
   } //定时发送时间按钮
  if(RoomStat==0)
   {
    if(ES.KeyValueCount>0)return;
    ES.KeyValue='$';
    ES.KeyType=-2;
    ES.KeyValueCount=1;
    SetEvent(ES.KeyHandle);
   }
//----------------下面是定时出现下首播放和时间
  if(ShowTime)
     {
      TranShowPicture(559,66,"d:\\vod\\picture\\a\\systime.bmp");
      DrawText16(631,74,int(RGB(0,0,0)),"宋体",Now().FormatString("hh:nn").c_str());
      DrawText16(633,76,int(RGB(255,255,255)),"宋体",Now().FormatString("hh:nn").c_str());
      DrawText16(632,75,int(RGB(0,0,255)),"宋体",Now().FormatString("hh:nn").c_str());
     }
  if(ShowNextSong)
     {
      TranShowPicture(146,64,"d:\\vod\\picture\\a\\nextsong.bmp");
      char buff[100];
      memset(buff,0,100);
      if(PlaySongNo<PlaySongCount)
      {
       strncpy(buff,PlaySongTab[PlaySongNo].A.GM,20);
       if(PlaySongTab[PlaySongNo].A.LB[0]=='1')
       {
        BDrawText(156,75,int(RGB(0,0,0)),(wchar_t *)buff,20);
        BDrawText(158,77,int(RGB(255,255,255)),(wchar_t *)buff,20);
        BDrawText(157,76,int(RGB(0,0,255)),(wchar_t *)buff,20);
       }
       else
       {
        DrawText16(156,75,int(RGB(0,0,0)),"楷体_GB2312",buff);;
        DrawText16(158,77,int(RGB(255,255,255)),"楷体_GB2312",buff);;
        DrawText16(157,76,int(RGB(0,0,255)),"楷体_GB2312",buff);;
       }
      }
     else
      {
       TranShowPicture(146,64,"d:\\vod\\picture\\a\\nextsong.bmp");
       DrawText16(157,76,int(RGB(255,255,255)),"楷体_GB2312","没有歌曲可以播放");;
       DrawText16(155,74,int(RGB(0,0,0)),"楷体_GB2312","没有歌曲可以播放");;
       DrawText16(156,75,int(RGB(0,0,255)),"楷体_GB2312","没有歌曲可以播放");;
      }
     }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ReadPath(AnsiString MAPFILE)
{
 //读取路径
 FILE *FP;
 PathCount=0;
 FP=fopen(MAPFILE.c_str(),"rb");
 if(FP==NULL)return;
 memset((void *)&Path,0,sizeof(struct MapDrive)*100);
 fseek(FP,4,SEEK_SET);
 fread((void *)&PathCount,sizeof(int),1,FP);
 for(int i=0;i<PathCount;i++)
  {
   fseek(FP,sizeof(struct MapDrive)*i+MapDriveHead,SEEK_SET);
   fread((void *)&Path[i],sizeof(struct MapDrive),1,FP);
   Path[i].RemotePath[99]=0;
  }
  fclose(FP);
}

bool TForm1::PL_ExitWindow(UINT uFlag)
{
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES tkp;

	if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		if(LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid))
		{
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if(AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0))
			{
				return ::ExitWindowsEx(uFlag, 0);
			}
		}
	}

	return ::ExitWindowsEx(uFlag, 0);
}

void __fastcall TForm1::DelRecordFile()
{
 struct ffblk fb;
 int i;
 i=findfirst("d:\\rm\\*.wav",&fb,faArchive|faReadOnly);
 while(i==0)
 {
  char buff[1024];
  memset(buff,0,1024);
  strcat(buff,"d:\\rm\\");
  strcat(buff,fb.ff_name);
  remove(buff);
  i=findnext(&fb);
 }
}

void __fastcall TForm1::GetTextWidth()
{
 GetTextWidthV=Canvas->TextWidth(GetTextWidthText);
}
//---------------------------------------------------------
void __fastcall TForm1::ZhGuangGao()
{
//将一个广告刷新程序里面的两个IMAGE
Image1->Height=TopPictureHeight;
Image1->Width=TopPictureWidth;
for(int l=0;l<TopPictureHeight;l++)
 for(int j=0;j<TopPictureWidth;j++)
  {
   Image1->Canvas->Pixels[j][l]=TColor(int(TopPictData[l*TopPictureWidth+j])^773377);
  }
Image2->Height=BottomPictureHeight;
Image2->Width=BottomPictureWidth;
for(int l=0;l<BottomPictureHeight;l++)
 for(int j=0;j<BottomPictureWidth;j++)
  {
   Image2->Canvas->Pixels[j][l]=TColor(int(BottomPictData[l*BottomPictureWidth+j])^337733);
  }
}

void __fastcall TForm1::DrawGuangGao()
{
  TRect Dest,Sour;
  Dest.top=557;
  Dest.left=142;
  Dest.right=Dest.left+Image2->Width;
  Dest.bottom=Dest.top+Image2->Height;
  Sour.top=0;
  Sour.left=0;
  Sour.bottom=Image2->Height;
  Sour.right=Image2->Width;
  Canvas->CopyRect(Dest,Image2->Canvas,Sour);
  //Dest.top=0;Dest.left=128;Dest.right=Image1->Width+128;
  Dest.top=0;
  Dest.left=0;
  Dest.right=Image1->Width;
  Dest.bottom=Image1->Height;
  Sour.top=0;
  Sour.left=0;
  Sour.bottom=Image1->Height;
  Sour.right=Image1->Width;
  Canvas->CopyRect(Dest,Image1->Canvas,Sour);
}

void __fastcall TForm1::NMUDP1DataReceived(TComponent *Sender,
      int NumberBytes, AnsiString FromIP, int Port)
{
 int l=sizeof(int);
 NMUDP1->ReadBuffer((char *)&RoomStat,sizeof(int ),l);
}
//---------------------------------------------------------------------------
bool __fastcall TForm1::CheckComputer()
{
 IP_ADAPTER_INFO IP;
 DWORD VEAX,VEBX,VECX,VEDX,Volserial;
 AnsiString AdapterName;
 unsigned long l=sizeof(IP_ADAPTER_INFO);
 unsigned long Flen=0,FFlag;
 memset(&IP,0,sizeof(IP_ADAPTER_INFO));
 GetAdaptersInfo(&IP,&l);
 char VolumeName[1024],FileSystem[1024];
 GetVolumeInformation("c:\\",VolumeName,1024,&Volserial,&Flen,&FFlag,FileSystem,1024);
 AdapterName=IP.AdapterName;
 int i;
 for(i=0;i<MAXCOMPUTER;i++)
 {
   if(memicmp(Id[i],IP.Address,6)==0&&
      AdapterName==AdapterNameArray[i]&&
      Volserial==VolserialArray[i])
      {
       ES.KeyHandle=CreateEvent(NULL,false,false,NULL);
       memset((void *)&SPS,0,sizeof(struct ShowPictureStru));
       MainTh=new MainThread(false,&ES,&SPS,&TS);
       return true;
      }
  };
 Image1->Canvas->Rectangle(0,0,Image1->Width,Image1->Height);
 Image1->Canvas->Font->Size=26;
 Image1->Canvas->Font->Color=clRed;
 Image1->Left=0;Image1->Top=200;
 Image1->Canvas->TextOutA(200,0,"打击盗版!");
 Image1->Visible =true;
 return false;
}

void __fastcall TForm1::MyCopyFile()
{
 struct COPYTAB
  {
   char delfalg;
   char sou[100];
   char des[50];
   char zd1[3];
   char zd2[10];
   char zd3[20];
   char zd4[30];
   char zd5[5];
  }Sz;
 char FilePath[1024],cname[1024];
 int CopyTabHead=257;
 int CopyRecCount;
 unsigned long l=1024;
 GetComputerName(cname,&l);
 memset(FilePath,0,1024);
 strcat(FilePath,"x:\\CopyData\\");
 strcat(FilePath,cname);
 strcat(FilePath,".dbf");
 FILE *FP;
 FP=fopen(FilePath,"rb");
 if(FP==NULL) return;
 fseek(FP,4,SEEK_SET);
 fread(&CopyRecCount,sizeof(int),1,FP);
 for(int k=0;k<CopyRecCount;k++)
 {
  fseek(FP,CopyTabHead+sizeof(struct COPYTAB)*k,SEEK_SET);
  fread((void *)&Sz,sizeof(struct COPYTAB),1,FP);
  for(int l=99;l>0;l--)
    {
      if(Sz.sou[l]==' ')Sz.sou[l]=0;
    }
  for(int l=49;l>0;l--)
    {
      if(Sz.des[l]==' ')Sz.des[l]=0;
    }
  if(Sz.zd1[0]=='3') //建立目录
  {
     mkdir(Sz.sou);
     continue;
  }
  if(Sz.zd1[0]=='1') //拷贝数据
  {
    struct ffblk fb;
    int i;
    i=findfirst(Sz.sou,&fb,FA_ARCH|FA_RDONLY);
    while(i==0)
    {
     AnsiString FN,DestPath;
     FN=ExtractFilePath(Sz.sou);
     DestPath=ExtractFilePath(Sz.des);
     FN=FN+fb.ff_name;
     DestPath=DestPath+fb.ff_name;
     SetFileAttributes(DestPath.c_str(),FILE_ATTRIBUTE_NORMAL);
     CopyFile(FN.c_str(),DestPath.c_str(),FALSE);
     i=findnext(&fb);
    }
    continue;
  }
  if(Sz.zd1[0]=='2') //删除文件
  {
    struct ffblk fb;
    int i;
    i=findfirst(Sz.sou,&fb,FA_ARCH|FA_RDONLY);
    while(i==0)
    {
     AnsiString FN;
     FN=ExtractFilePath(Sz.sou)+fb.ff_name;
     SetFileAttributes(FN.c_str(),FILE_ATTRIBUTE_NORMAL);
     DeleteFile(FN.c_str());i=findnext(&fb);
    }
    continue;
  }
 };
 fclose(FP);
}
//-------------------------------------------------------------------
void __fastcall TForm1::DrawDH1(Graphics::TBitmap *tp,int CurX,int CurY )
{
 TRect DDR,SDR;
 for(int i=0;i<tp->Width/4;i++)
  {
   int w,h;
   int xoff,yoff;
   srand(GetTickCount());
   w=rand()%100;
   h=rand()%100;
   xoff=rand()%(tp->Width-w);
   yoff=rand()%(tp->Height-h);
   DDR.left=xoff+CurX;
   DDR.right=xoff+w+CurX;
   DDR.top=yoff+CurY;
   DDR.bottom=yoff+h+CurY;
   SDR.left=xoff;
   SDR.right=xoff+w;
   SDR.top=yoff;
   SDR.bottom=yoff+h;
   Canvas->CopyRect(DDR,tp->Canvas,SDR);
   Sleep(1);
  }
 SDR.left=0;
 SDR.right=tp->Width;
 SDR.top=0;
 SDR.bottom=tp->Height;
 DDR.left=CurX;
 DDR.right=DDR.left+tp->Width;
 DDR.top=CurY;
 DDR.bottom=DDR.top+tp->Height;
 Canvas->CopyRect(DDR,tp->Canvas,SDR);
}
//------------------------------------------------------------------------
void __fastcall TForm1::DrawDH2(Graphics::TBitmap *tp,int CurX,int CurY  )
{
//显示拉幕动画
 TRect DDR,SDR;
for(int j=0;j<=tp->Width/40;j++)
 {
 for(int i=0;i<40;i++)
  {
   int w;
   w=tp->Width/40*i;
   DDR.left=CurX+w+j-1;
   DDR.right=w+CurX+j;
   DDR.top=CurY;
   DDR.bottom=CurY+tp->Height;
   SDR.left=w;
   SDR.right=w+j;
   SDR.top=0;
   SDR.bottom=tp->Height;
   Canvas->CopyRect(DDR,tp->Canvas,SDR);
  }
 }
 SDR.left=0;
 SDR.right=tp->Width;
 SDR.top=0;
 SDR.bottom=tp->Height;
 DDR.left=CurX;
 DDR.right=DDR.left+tp->Width;
 DDR.top=CurY;
 DDR.bottom=DDR.top+tp->Height;
 Canvas->CopyRect(DDR,tp->Canvas,SDR);
}
//------------------------------------------------------------------------
void __fastcall TForm1::DrawDH3(Graphics::TBitmap *tp,int CurX,int CurY  )
{
//显示闭幕动画
 TRect DDR,SDR;
for(int j=0;j<=tp->Width/2;j++)
 {
   int w;
   DDR.left=CurX+j;
   DDR.right=CurX+j+1;
   DDR.top=CurY;
   DDR.bottom=CurY+tp->Height;
   SDR.left=j;
   SDR.right=j+1;
   SDR.top=0;
   SDR.bottom=tp->Height;
   Canvas->CopyRect(DDR,tp->Canvas,SDR);
   DDR.left=CurX+tp->Width-j;
   DDR.right=CurX+tp->Width-j-1;
   DDR.top=CurY;
   DDR.bottom=CurY+tp->Height;
   SDR.left=tp->Width-j;
   SDR.right=tp->Width-j-1;
   SDR.top=0;
   SDR.bottom=tp->Height;
   Canvas->CopyRect(DDR,tp->Canvas,SDR);
 }
}
//------------------------------------------------------------------------
void __fastcall TForm1::DrawDH4(Graphics::TBitmap *tp,int CurX,int CurY  )
{
//显示闭幕动画
TRect DDR,SDR;
for(int j=0;j<=tp->Height/2;j+=4)
 {
   int w;
   DDR.left=CurX;
   DDR.right=CurX+tp->Width;
   DDR.top=CurY;
   DDR.bottom=j+4+CurY;
   SDR.left=CurX;
   SDR.right=CurX+tp->Width;
   SDR.top=0;
   SDR.bottom=j+4;
   Canvas->CopyRect(DDR,tp->Canvas,SDR);

   DDR.left=CurX;
   DDR.right=CurX+tp->Width;
   DDR.top=CurY+(tp->Height-j-4);
   DDR.bottom=CurY+(tp->Height-j);
   SDR.left=CurX;
   SDR.right=CurX+tp->Width;
   SDR.top=tp->Height-j;
   SDR.bottom=tp->Height-j-4;
   Canvas->CopyRect(DDR,tp->Canvas,SDR);
 }
 SDR.left=0;
 SDR.right=tp->Width;
 SDR.top=0;
 SDR.bottom=tp->Height;
 DDR.left=CurX;
 DDR.right=DDR.left+tp->Width;
 DDR.top=CurY;
 DDR.bottom=DDR.top+tp->Height;
 Canvas->CopyRect(DDR,tp->Canvas,SDR);
}

AnsiString __fastcall TForm1::ReadPara(AnsiString PName)
{
//读取参数表的数据
struct ParaTab A;
FILE *FP;
int r;
char buff[200];
AnsiString st;
FP=fopen(PARATABPATH,"rb");
if(FP==NULL)return "";
fseek(FP,4,SEEK_SET);
fread((void *)&r,sizeof(int),1,FP);
for(int i=0;i<r;i++)
 {
  memset(buff,0,200);st="";
  fseek(FP,ParaTabHeadSize+sizeof(struct ParaTab)*i,SEEK_SET);
  fread((void *)&A,sizeof(struct ParaTab),1,FP);
  if(A.delflag=='*')continue;
  strncpy(buff,A.sm,20);
  st=buff;
  st=st.Trim();
  if(st==PName.Trim())
   {
    memset(buff,0,200);
    strncpy(buff,A.v1,100);
    fclose(FP);
    st=buff;
    st=st.Trim();
    break;
   }
 }
fclose(FP);
return st;
}
int __fastcall TForm1::GetMouseOperCode(AnsiString PictName)
{
 //根据主图片，返回鼠标操作的代码
 char buff[1024];
 AnsiString ABuff,BBuff;
 ABuff=ExtractFileName(PictName);

 for(int i=0;i<PTabCount;i++)
  {
   memset(buff,0,1024);
   strncpy(buff,PTab[i].pictname,200);
   BBuff=buff;BBuff=BBuff.Trim();
   if(BBuff.Trim().UpperCase()==ABuff.Trim().UpperCase())
    {
     memset(buff,0,1024);
     strncpy(buff,PTab[i].opercode,3);
     return atoi(buff);
    }
  }
 return -1;   ///不成功返回负值，则不做处理
}
void __fastcall TForm1::ReadMouseTab()
{
 //读取鼠标对应的键盘表
 FILE *FP;
 struct MouseTab T1;
 struct POPERTab P1;
 int i,j;
 MTabCount=0;
 PTabCount=0;
 FP=fopen(POPERPATH,"rb");
 if(FP==NULL)return;
 fseek(FP,4,SEEK_SET);
 fread((void *)&i,sizeof(int),1,FP);
 for(j=0;j<i;j++)
  {
   fseek(FP,POPERTabeHeadSize+sizeof(struct POPERTab)*j,SEEK_SET);
   fread((void *)&P1,sizeof(struct POPERTab),1,FP);
   PTab[PTabCount]=P1;
   PTabCount++;
  }
 fclose(FP);
 FP=fopen(MOUSETABPATH,"rb");
 if(FP==NULL)return;
 fseek(FP,4,SEEK_SET);fread((void *)&i,sizeof(int),1,FP);
 for(j=0;j<i;j++)
  {
   fseek(FP,MouseTabHeadSize+sizeof(struct MouseTab)*j,SEEK_SET);
   fread((void *)&T1,sizeof(struct MouseTab),1,FP);
   T1.PICT[199]=0;
   MTab[MTabCount]=T1;
   MTabCount++;
  }
 fclose(FP);
}
void __fastcall TForm1::FormMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
//下面根据需要，产生鼠标按下对应的控制
char buff[1024];
if(Button==mbRight) {unsigned short K=8;FormKeyDown(this,K,TShiftState());return;}
for(int i=0;i<MTabCount;i++)
 {
  char x[10],x1[10],y[10],y1[10],bh[10];
  memset(x,0,10);
  strncpy(x,MTab[i].X,3);
  memset(x1,0,10);
  strncpy(x1,MTab[i].X1,3);
  memset(y,0,10);
  strncpy(y,MTab[i].Y,3);
  memset(y1,0,10);
  strncpy(y1,MTab[i].Y1,3);
  memset(bh,0,10);
  strncpy(bh,MTab[i].OPERCODE,3);
  if(atoi(bh)!=MouseOperCode)continue;
  if(X>=atoi(x)&&X<=atoi(x1)&&Y>=atoi(y)&&Y<=atoi(y1))   //落在有效区域
     {
       bool oper=false;
       if(MTab[i].PICT[0]!=' ') //如果需要区域判断
        {
         Graphics::TBitmap *TB;
         TB=new Graphics::TBitmap();   //当判断反图时，则要做出反图区域颜色判断
         memset(buff,0,200);
         strncpy(buff,MTab[i].PICT,200);
         TB->LoadFromFile(buff);
         int ax,ay;
         ax=X-atoi(x);if(ax<0)ax=0;if(ax>=TB->Width)ax=TB->Width-2;
         ay=Y-atoi(y);if(ay<0)ay=0;if(ay>=TB->Height)ax=TB->Height-1;
         if(TB->Canvas->Pixels[ax][ay]!=TB->Canvas->Pixels[0][0])oper=true; //如果落在区域内
         delete TB;
        }
       else
         oper=true;  //如果不需要区域判断
       if(oper==false)return;  //如果不是有效的，则返回
       if((MTab[i].KEYCOUNT&0xff)=='2') //对双键进行特殊处理
       {
        unsigned short key,key1;
        char buff[10],buff1[10];
        memset(buff,0,10);memset(buff1,0,10);
        strncpy(buff,MTab[i].KEYDOWN1,4);
        strncpy(buff1,MTab[i].KEYDOWN2,4);
        key=atoi(buff);key1=atoi(buff1);
        if(ES.KeyValueCount>0)return; //不做处理，缓冲区未空时
        ES.KeyValue=key1;  //键值1，
        ES.KeyValue1=key;  //键值2
        ES.KeyValueCount=2;
        ES.KeyType=0;
        SetEvent(ES.KeyHandle);
        return;
       }
       else  //对单键直接进行处理
       {
         if(MTab[i].KEYDOWN1[0]!=' ')
          {
           unsigned short key;
           strncpy(buff,MTab[i].KEYDOWN1,4);
           key=atoi(buff);
           FormKeyDown(this,key,TShiftState());
           return;
          }
       }
     }
 }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMUDP2DataReceived(TComponent *Sender,
      int NumberBytes, AnsiString FromIP, int Port)
{
//接收网络数据包
 int l;
 struct QQDG P;
 NMUDP2->ReadBuffer((char *)&P,sizeof(struct QQDG),l);
 if(P.KD==AUTOPLAY) //自动播放歌曲
  {
     FILE *FP;int i,j,k;
     struct SongTab Tmp;
     FP=fopen(GQPATH,"rb");
     if(FP==NULL)return;
     PlaySongCount=0;PlaySongNo=0;
     randomize();
     fseek(FP,4,SEEK_SET);fread(&j,sizeof(int),1,FP);
     for(i=0;i<400;i++)
      {
       k=rand()%j;
       fseek(FP,SongTabHead+sizeof(struct SongTab)*k,SEEK_SET);
       fread(&Tmp,sizeof(struct SongTab),1,FP);
       for(int i=9;i>0;i--)if(Tmp.BH[i]==' ')Tmp.BH[i]=0;
       for(int i=39;i>0;i--)
        {
         if(Tmp.GM[i]==' ')Tmp.GM[i]=0;else break;
        }
       PlaySongTab[PlaySongCount].A    =Tmp;
       PlaySongTab[PlaySongCount].Stat =0;
       PlaySongTab[PlaySongCount].RecNo=k;
       PlaySongCount++;
      }
  }
  if(P.KD==QUITPLAY) //清空播放队列
  {PlaySongCount=0;PlaySongNo=0;}
 if(P.KD==CLOSEPROG) //关闭程序
  {
    if(ES.KeyValueCount>0)return;
    Timer2->Enabled=false;
    ES.KeyValue=0xfffe;
    ES.KeyValueCount=1;
    SetEvent(ES.KeyHandle);     //其它处理，则设置信号
    QuitVideo(PlayStream);
    Close();
  }
 if(P.KD==PLAYONEFILE)   //播放指定文件
  {
   CloseVideo(PlayStream);OpenVideo(P.SongName,PlayStream);
  } //播放指定文件
 if(P.KD==SYSTEMRESET)  //重新初始化点歌程序的参数
  {
    MyCopyFile();       //重新拷贝文件
    initSystem();       //重新初始化系统
  }
 if(P.KD==FORWARD)     {ProcessDJ('H');} //快进
 if(P.KD==BACKWARD)    {ProcessDJ('I');} //快退
 if(P.KD==MUSICECHANGE){ProcessDJ('N');} //声道切换
 if(P.KD==ENDPLAY)     {ProcessDJ('O');} //exit
 if(P.KD==REPLAYSONG)  {ProcessDJ('P');};//replay
 if(P.KD==PAUSESONG)   {ProcessDJ('L');};//PAUSE
 if(P.KD==VOLUMEUP)    {ProcessDJ('J');};//VOLUMEUP
 if(P.KD==VOLUMEDOWN)  {ProcessDJ('K');};//VOLUMEUP
 if(P.KD==RESET)
 {
        QuitVideo(PlayStream);
        if(ES.KeyValueCount>0)return;
        Timer2->Enabled=false;
        ES.KeyValue=0xfffe;
        ES.KeyValueCount=1;
        SetEvent(ES.KeyHandle);     //其它处理，则设置信号
        PL_ExitWindow(EWX_REBOOT|EWX_LOGOFF);
  }
 if(P.KD==CLOSECOMPUTER)
  {
        QuitVideo(PlayStream);
        if(ES.KeyValueCount>0)return;
        Timer2->Enabled=false;
        ES.KeyValue=0xfffe;
        ES.KeyValueCount=1;
        SetEvent(ES.KeyHandle);     //其它处理，则设置信号
        PL_ExitWindow(EWX_POWEROFF|EWX_SHUTDOWN);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::WriteFwhj()
{
//写入服务呼叫
FILE *FP;
struct fwhj A;
int rccount;
char cname[1024];
unsigned long k=1024;
GetComputerName(cname,&k);
FP=fopen(FWHJPATH,"r+b");
if(FP==NULL)return;
fseek(FP,4,SEEK_SET);
fread((void *)&rccount,sizeof(int),1,FP);

for(int l=0;l<rccount;l++)
 {
  char buff[100];
  AnsiString fm,fm1;
  fm1=cname;
  fseek(FP,FWHJHEADSIZE+sizeof(struct fwhj)*l,SEEK_SET);
  fread((void *)&A,sizeof(struct fwhj),1,FP);
  memset(buff,0,100);strncpy(buff,A.room,10);fm=buff;fm=fm.Trim();

  if(fm1.UpperCase()==fm.UpperCase())
  {
   A.hjcs[0]='1'&0xff;A.hjcs[1]=' ';A.hjcs[2]=' ';
   fseek(FP,513+sizeof(struct fwhj)*l,SEEK_SET);
   fwrite((void *)&A,sizeof(struct fwhj),1,FP);
   break;
  }
 }
 fclose(FP);
}

void __fastcall TForm1::ShowNetPage()
{
 //显示网页操作界面
 wchar_t buff[1024];
 RegisterHotKey(Handle,0,0,VK_BACK);
 NetPageHome.WideChar(buff,1024);
 Panel1->Visible=true;
 Panel1->Left=0;Panel1->Top=60;
 Panel1->Width=800;Panel1->Height=440;
 CppWebBrowser1->Navigate(buff);
 CppWebBrowser1->SetFocus();
}
void __fastcall TForm1::CloseNetPage()
{
  //关闭网页显示
  Panel1->Visible=false;
  try{CppWebBrowser1->Stop();}
  catch(...){};
}


void TForm1::CloseInternet(TMessage msg)
{
        //TODO: Add your source code here
 if(ES.KeyValueCount>0)return;
 ES.KeyValue='S';
 ES.KeyValueCount=1;
 SetEvent(ES.KeyHandle);     //其它处理，则设置信号
 UnregisterHotKey(Handle,0);
}

void __fastcall TForm1::initSystem()
{
 Bhdgjm=atoi(ReadPara("编号点歌方式").c_str());
 zysy=ReadPara("资源使用");
 SongNameFont=ReadPara("歌曲字体");
 SongNameFontSize=atoi(ReadPara("歌曲字体大小").c_str());
 SingerFont= ReadPara("歌星字体");
 SingerFontSize=atoi(ReadPara("歌星字体大小").c_str());
 InternetOper=atoi(ReadPara("是否可以上网").c_str());
 TVAV_STAT=atoi(ReadPara("TVAV初始").c_str());
 SyDh=atoi(ReadPara("使用动画").c_str());
 JiuCheck=atoi(ReadPara("酒水检查").c_str());
 RoomStat=atoi(ReadPara("房间初始状态").c_str());  //默认房进是进场的
 RemoteHostIP=ReadPara("服务器IP地址") ;
 if(RemoteHostIP=="")RemoteHostIP="127.0.0.1";
 Gsyldx=atoi(ReadPara("公视音量").c_str());
 XPath=ReadPara("收银服务器X盘");
 WNetAddConnection(XPath.c_str(),NULL,"X:");
 Gswjm=ReadPara("公视文件名");
 hwzt=ReadPara("韩文字体");if(hwzt=="")hwzt="Gulim";
 hwztdx=atoi(ReadPara("韩文字体大小").c_str());
 if(Gswjm=="")Gswjm="lfw.vob";
 CloseComputerPassword=ReadPara("关机密码");
 if(CloseComputerPassword=="") CloseComputerPassword="13873970053";
 PlaySongCount=0;
 PlaySongNo=0;
 PlayStream=0;
 zyzt=0;
}
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
zyzt++;
if(zyzt>=2)   //超过十秒钟没有找到服务器，则认为系统资源使用MAP文件！
{
 zysy="0";  //切换到分布式系统
 ReadPath(MAPTABPATH);    //重新读取工作站的路径
}
#if YS
 static int playcount=0;
 playcount++;
 if(playcount>7200)
 {
  Timer2->Enabled=false;
  Timer1->Enabled=false;
  Close();
  }
#endif
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BDrawText(int x,int y,int Color,wchar_t *Text,int len)
{
//显示字体,显示韩文字体
Canvas->Brush->Style=bsClear;
Canvas->Pen->Style=psClear;
Canvas->Font->Size=hwztdx;
Canvas->Font->Color=TColor(Color);
Canvas->Font->Name =hwzt;
Canvas->Font->Charset=HANGEUL_CHARSET;
TextOutW(Canvas->Handle,x,y,Text,len);
}
void __fastcall TForm1::CppWebBrowser1NewWindow2(TObject *Sender,
      LPDISPATCH *ppDisp, TOLEBOOL *Cancel)
{
*Cancel=(TOLEBOOL)true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMUDP3DataReceived(TComponent *Sender,
      int NumberBytes, AnsiString FromIP, int Port)
{
   /*int zs;
   int l=0;
   NMUDP3->ReadBuffer((char *)&zs,sizeof(int),l);
   if(zyzt>=2){ReadPath(MAPTABPATH);zysy="1";}      //读取服务器路径
   zyzt=0;*/
}
//---------------------------------------------------------------------------
int __fastcall TForm1::SendData(AnsiString RemoteHost,int Port,char *senddata,int sendlen)
{
//
WSAData P;
int rt;
rt=WSAStartup(MAKEWORD(2,2),&P);
if(rt==SOCKET_ERROR){return 1;}
SOCKET S;
SOCKADDR_IN SENDST;
S=socket(AF_INET,SOCK_DGRAM,0);
if(S==INVALID_SOCKET){WSACleanup();return 2;}
memset((void *)&SENDST,0,sizeof(SOCKADDR_IN));
SENDST.sin_family=AF_INET;
SENDST.sin_port=htons(Port);
SENDST.sin_addr.s_addr=inet_addr(RemoteHost.c_str());
rt=sendto(S,senddata,sendlen,0,(sockaddr *)&SENDST,sizeof(sockaddr));
if(rt==SOCKET_ERROR)
{
 return 3;
}
closesocket(S);
WSACleanup();
return 0;
}
int __fastcall TForm1::WaitRecvData(int LocalPort,char *Recvbuff,int RecvSize,int WaitTime)
{
//
WSAData P;
int rt,l;
rt=WSAStartup(MAKEWORD(2,2),&P);
if(rt==SOCKET_ERROR){return 1;}
SOCKET S;

SOCKADDR_IN RECVST,SENDST;
S=socket(AF_INET,SOCK_DGRAM,0);
if(S==INVALID_SOCKET){WSACleanup();return 2;}
memset((void *)&RECVST,0,sizeof(SOCKADDR_IN));
memset((void *)&SENDST,0,sizeof(SOCKADDR_IN));
RECVST.sin_family=AF_INET;
RECVST.sin_port=htons(LocalPort);
RECVST.sin_addr.s_addr=htonl(INADDR_ANY);
if(WaitTime!=-1)
{
 setsockopt(S,SOL_SOCKET,SO_RCVTIMEO,(char *)&WaitTime,sizeof(WaitTime));
}
rt=bind(S,(sockaddr *)&RECVST,sizeof(sockaddr));
if(rt==SOCKET_ERROR)
{
  closesocket(S);
  WSACleanup();
  return 10;
}
l=RecvSize;
rt=recvfrom(S,Recvbuff,RecvSize,0,(sockaddr *)&SENDST,&l);
if(rt==SOCKET_ERROR)  //接收数据错误
{
 closesocket(S);
 WSACleanup();
 return 3;
}
 closesocket(S);
 WSACleanup();
 return 0;
}

