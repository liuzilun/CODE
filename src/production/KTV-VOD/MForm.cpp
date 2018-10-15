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
#define MAXCOMPUTER     1     //�ܹ�ӵ�еļ��������
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
 RecordMusice=false;  //û��¼��
 BackMusice=false;     //û�б�������
 BackPlay=false;      //û�лط�
 Width=800;
 Height=600;
 Top=0;
 Left=0;
 ReadPath(MAPTABPATH);      //��ȡ������·��
 DelRecordFile(); //ɾ��¼���ļ�
 ZhGuangGao();  //ת�����
 MyCopyFile();  //��������
 OpenDrive();     //��������

 ReadMouseTab();  //��ȡ���̲�����
 MainThreadStat=0;  //Ĭ���̴߳��ڻ�ӭ����
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
 //����Ļ����ʾһ��ͼƬ
 struct ffblk fb;
 TRect R,R1;
 int i;
 i=GetMouseOperCode(fname);
 if(i!=-1)MouseOperCode=i;   //������Ǹ��������ȡ����Ӧ�Ĵ�ϵͳ����
 Graphics::TBitmap *TB;
 TB=new Graphics::TBitmap();
 if(findfirst(fname,&fb,FA_ARCH|FA_RDONLY)!=0)return; //û���ҵ������������κδ���
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
   case 0:  //û�ж���
       R.top=y;
       R.left=x;
       R.right=x+TB->Width-1;
       R.bottom=y+TB->Height-1;
       R1.top=0;
       R1.left=0;
       R1.right=TB->Width;
       R1.bottom=TB->Height;
       Canvas->CopyRect(R,TB->Canvas,R1);break;
   case 1: //��ʾ����鶯��
       DrawDH2(TB,x,y);break;
   case 2: //��ʾ��������
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
 //����Ļ����ʾһ��ͼƬ,�Ǵ�͸��ʾ
 struct ffblk fb;
 char *B1,c1,c2,c3,*B2;
 TRect R,R1;
 Graphics::TBitmap *TB,*TB2;
 TB=new Graphics::TBitmap();
 if(findfirst(fname,&fb,FA_ARCH|FA_RDONLY)!=0)return; //û���ҵ������������κδ���
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
//��ʾ����
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
 //��ʾ����
 switch(this->TS.fonttype)
  {
   case 0:  //ʮ������
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
//��ʾ����,��ʾ��������
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
//��ʾ����
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
//��ʾ����
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
  if(atoi(KTS[l].KeyDown)==Key)  //��̬�ǽ������Ž��д���
  {
    ES.KeyValue=int(KTS[l].KeyChar[0]&0xff);
    if(ES.KeyValue>='a'&&ES.KeyValue<='j')ES.KeyType=-4;  //��ݼ�
    found=true;
    break;
  }
 }
 if(found==false)return;     //���Ǽ��̱��ϵ����ݣ������κδ���c
 if(RoomStat==1)
 {
  if(ProcessDJ(ES.KeyValue))return;  //DJ���ƴ���
 }
 if(ES.KeyValueCount>0)return;
 ES.KeyValueCount=1;
 SetEvent(ES.KeyHandle);     //���������������ź�
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TDP()
{
//��Զ���߳�ʹ�õ���ʾͼƬ����
 switch(SPS.type)
  {
   case 0:    ShowPicture(SPS.x,SPS.y,SPS.fname);    break;//
   case 1:TranShowPicture(SPS.x,SPS.y,SPS.fname);    break;//
  }
}

void __fastcall TForm1::FormPaint(TObject *Sender)
{
  //ˢ�½���
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
 if(waveInOpen(&waveIn,WAVE_MAPPER,&waveFormat,(DWORD)Handle,0,CALLBACK_WINDOW))  //�豸��ʧ��
  {
   RecordFP=NULL;
   RecordMusice=false;
   return;
  }
 //��������¼��������ʼ¼��
 RF="d:\\rm\\";
 RF=RF+RecordSongNo.Trim();
 RF=RF+".wav";
 RecordFP=fopen(RF.c_str(),"w+b");
 if(RecordFP==NULL)return;
 memset((void *)&waveHdr,0,sizeof(WAVEHDR));
 memset((void *)&wavehead,0,sizeof(struct WaveHead));
 fwrite((void *)&wavehead,sizeof(struct WaveHead),1,RecordFP);  //д���ļ�ͷ
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
   } //����¼��������ͷʧ��
 if(waveInAddBuffer(waveIn,&waveHdr,sizeof(WAVEHDR)))
   {
    fclose(RecordFP);
    RecordFP=NULL;
    RecordMusice=false;
    return ;
   }//���뻺����ʧ��
 if(waveInStart(waveIn))
   {
     fclose(RecordFP);
     RecordFP=NULL;
     RecordMusice=false;
     return ;
   }  //��ʼ¼��ʧ��
 RecordMusice=true;
}
void __fastcall TForm1::SaveRecord()   //����¼��
{
 //����¼���ļ�
 if(RecordMusice)
 {
  waveInStop(waveIn);
  waveInReset(waveIn); //ˢ��һ�βɼ�����ֹ����
  waveInUnprepareHeader(waveIn,&waveHdr,sizeof(WAVEHDR));//�ͷ���Դ
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
    fwrite((void *)&wavehead,sizeof(struct WaveHead),1,RecordFP);  //����д��ͷ����
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
  fwrite((void *)waveBuff,sizeof(char),waveHdr.dwBytesRecorded,RecordFP);//д��Ӳ��
  waveInAddBuffer(waveIn,&waveHdr,sizeof(WAVEHDR));
  return true;
}

void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    if(ReadPara("�ػ��Ͽ�X��")=="1")WNetCancelConnection("x:",true);
    if(Tag>0)  //��������óɹػ��ź�
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
 //�طŸ���
 struct ffblk fb;
 int i;
 char  wjm[1024],buff[1024];
 AnsiString command;
 if(RecordMusice)
  {
   SaveRecord();
   bytecount=0;
   RecordMusice=false;
  } //�����ǰ��¼�����򱣴�¼������
 mciSendString("stop mysound",buff,1024,NULL);
 mciSendString("close mysound",buff,1024,NULL);
 Timer1->Enabled=false;
 Timer2->Enabled=false;
 command=FindSong(BackPlaySongNo.Trim());
 Timer1->Enabled=true;
 Timer2->Enabled=true;
 if(command=="")return; //���ܻط�
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
//ֹͣ����¼��
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
 if(BackMusice==false) //����ڲ��������ĸ���
   {
     if(SongPlayStat==1)Forward(PlayStream,1);  //���
     if(SongPlayStat==2)Backward(PlayStream,1); //����
     if(PlayStream!=0&&GetState(PlayStream)==FMPF_PLAYING) return; //������ڲ��ţ����˳�
     if(PlayStream!=0&&GetState(PlayStream)==FMPF_PAUSED) return;   //��ͣҲ����
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
 if( PlaySongCount> PlaySongNo)  //����и������Բ���
  {
   memset(buff,0,100);
   strncpy(buff, PlaySongTab[ PlaySongNo].A.BH,10);
   Timer1->Enabled=false;
   Timer2->Enabled=false;
   wjm=FindSong(buff);
   Timer1->Enabled=true;
   Timer2->Enabled=true;
   PlaySongTab[ PlaySongNo].Stat=1;          //����Ϊ���ڲ���
   if(BackPlay)StopBackPlay();   //��ǰ�ڻطţ���ֹͣ�ط�
   if(RecordMusice)SaveRecord(); //��ǰ��¼������ֹͣ¼��
   if(wjm!="")
   {
    OpenVideo(wjm.c_str(),PlayStream);SelectTV(TVAV_STAT,PlayStream);
    memset(buff,0,100);
    if( PlaySongTab[ PlaySongNo].A.BCSD[0]=='1')SAudio(PlayStream);
    if( PlaySongTab[ PlaySongNo].A.BCSD[0]=='2'){SAudio(PlayStream);SAudio(PlayStream);}  //��������
    strncpy(buff, PlaySongTab[ PlaySongNo].A.YLDX,2);
    SetVolume(PlayStream,atoi(buff)*2);     //����������С
    SongVolumeSize=atoi(buff);
    SongPlayStat=0;
    memset(buff,0,100);
    strncpy(buff, PlaySongTab[ PlaySongNo].A.BH,10);
    RecordSongNo=buff;
    RecordSongNo=RecordSongNo.Trim();
    StartRecord();  //��ʼ¼��
    }
   BackMusice=false;
   SongPlayStat=0;
   MainTh->PlaySongEnd();
   PlaySongNo++;
  }
 else                         //���򲥷ű�������
  {
   if(PlayStream!=0&&GetState(PlayStream)==FMPF_PLAYING) return; //������ڲ��ţ����˳�
   if(PlayStream!=0&&GetState(PlayStream)==FMPF_PAUSED) return;   //��ͣҲ����
   if(BackPlay)StopBackPlay();   //��ǰ�ڻطţ���ֹͣ�ط�
   if(RecordMusice)SaveRecord(); //��ǰ��¼������ֹͣ¼��
   OpenVideo(Gswjm.c_str(),PlayStream);SelectTV(TVAV_STAT,PlayStream);
   SetVolume(PlayStream,Gsyldx);
   SongVolumeSize=Gsyldx;BackMusice=true;
  }
}

AnsiString __fastcall TForm1::FindSong(AnsiString No)
{
  //���ݱ����������
  //����ҵ��򷵻ظ����ľ���·����
int Rj;
static int PackNo=0;
if(zysy=="1")  //ʹ�����⼼��
{
  Timer1->Enabled=false;
  Timer2->Enabled=false;
  //��������D������ĸ���
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
if(zysy=="0"||zysy=="") //ʹ�÷ֲ�ʽϵͳ
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
 {113,509,'O',"m_1.bmp",true},  //�˵�
 {173,505,'M',"m_2.bmp",true},  //����
 {244,503,'Q',"m_3.bmp",true},  //����
 {326,498,'H',"m_4.bmp",true},  //���
 {398,501,'I',"m_5.bmp",true},  //����
 {469,501,'L',"m_6.bmp",true},  //��ͣ
 {555,504,'N',"m_7.bmp",true},  //����
 {624,509,'P',"m_8.bmp",true},  //�ط�
 {4  ,561,'K',"m_9.bmp",true},  //��С
 {87 ,559,'J',"m_10.bmp",true}, //����
 {668,549,'B',"m_11.bmp",false}, //�Ϸ�
 {732,546,'A',"m_12.bmp",false}  //�·�
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
    founddj=sz[i].dj;    //��ȡ��ť����
    break;
   }
 }
switch(Key)
 {
   case 'H'://         72        ���
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
   case 'I'://         73        ����
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
   case 'J'://         74        ����
         if(BackPlay)break;
         if((SongVolumeSize+1)<50)
           {
             SongVolumeSize++;
             SetVolume(PlayStream,SongVolumeSize*2);
           }
         break;
   case 'K'://         75        ��С
         if(BackPlay)break;
         if(SongVolumeSize>0)
           {
             SongVolumeSize--;
             SetVolume(PlayStream,SongVolumeSize*2);
           }
         break;
   case 'L'://         76        ��ͣ
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
   case 'M'://         77        ����
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
   case 'N'://         78        ����
         if(BackPlay)break;
         SAudio(PlayStream);
         break;
   case 'O'://         79        �˵�
         SongPlayStat=0;
         CloseVideo(PlayStream);
         BackMusice=true;        //����Ϊ���ű�������
         if(BackPlay)StopBackPlay();
         break;
   case 'P'://         80        �ط�
         if(BackPlay)break;
         RePlay(PlayStream);
         SetVolume(PlayStream,SongVolumeSize*2);
         break;
   case 'Q'://         81        ����
         if(RoomStat==1) WriteFwhj();   //����ǽ���״̬
         if(ES.KeyValueCount>0) return false;
         ES.KeyValueCount=1;ES.KeyValue='Q';
         SetEvent(ES.KeyHandle);     //���������������ź�
         return false;
   case 'R'://         82        TV_AV�л�
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
//------------------�����Ƕ�ʱ����һ�δ�����Ϣ
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
//-----------------�����ǽ���״̬��ѯ
  if(Tag>0)                //����йػ��źţ�����������
  {
   Tag++;
   if(Tag>=4)Close();
   return;
  }
  if(ES.KeyType==-4)return;
  ES.KeyType=0;
  if(RoomStat==1)ProcessPlaySong();              //ÿ�����Ӳ�ѯһ�θ�������
  if(RoomStat==1&&SyDh==1)
   {
    if(ES.KeyValueCount>0)return;
    ES.KeyValue='!';
    ES.KeyValueCount=1;
    SetEvent(ES.KeyHandle);
   } //��ʱ����ʱ�䰴ť
  if(RoomStat==0)
   {
    if(ES.KeyValueCount>0)return;
    ES.KeyValue='$';
    ES.KeyType=-2;
    ES.KeyValueCount=1;
    SetEvent(ES.KeyHandle);
   }
//----------------�����Ƕ�ʱ�������ײ��ź�ʱ��
  if(ShowTime)
     {
      TranShowPicture(559,66,"d:\\vod\\picture\\a\\systime.bmp");
      DrawText16(631,74,int(RGB(0,0,0)),"����",Now().FormatString("hh:nn").c_str());
      DrawText16(633,76,int(RGB(255,255,255)),"����",Now().FormatString("hh:nn").c_str());
      DrawText16(632,75,int(RGB(0,0,255)),"����",Now().FormatString("hh:nn").c_str());
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
        DrawText16(156,75,int(RGB(0,0,0)),"����_GB2312",buff);;
        DrawText16(158,77,int(RGB(255,255,255)),"����_GB2312",buff);;
        DrawText16(157,76,int(RGB(0,0,255)),"����_GB2312",buff);;
       }
      }
     else
      {
       TranShowPicture(146,64,"d:\\vod\\picture\\a\\nextsong.bmp");
       DrawText16(157,76,int(RGB(255,255,255)),"����_GB2312","û�и������Բ���");;
       DrawText16(155,74,int(RGB(0,0,0)),"����_GB2312","û�и������Բ���");;
       DrawText16(156,75,int(RGB(0,0,255)),"����_GB2312","û�и������Բ���");;
      }
     }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ReadPath(AnsiString MAPFILE)
{
 //��ȡ·��
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
//��һ�����ˢ�³������������IMAGE
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
 Image1->Canvas->TextOutA(200,0,"�������!");
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
  if(Sz.zd1[0]=='3') //����Ŀ¼
  {
     mkdir(Sz.sou);
     continue;
  }
  if(Sz.zd1[0]=='1') //��������
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
  if(Sz.zd1[0]=='2') //ɾ���ļ�
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
//��ʾ��Ļ����
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
//��ʾ��Ļ����
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
//��ʾ��Ļ����
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
//��ȡ�����������
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
 //������ͼƬ�������������Ĵ���
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
 return -1;   ///���ɹ����ظ�ֵ����������
}
void __fastcall TForm1::ReadMouseTab()
{
 //��ȡ����Ӧ�ļ��̱�
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
//���������Ҫ��������갴�¶�Ӧ�Ŀ���
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
  if(X>=atoi(x)&&X<=atoi(x1)&&Y>=atoi(y)&&Y<=atoi(y1))   //������Ч����
     {
       bool oper=false;
       if(MTab[i].PICT[0]!=' ') //�����Ҫ�����ж�
        {
         Graphics::TBitmap *TB;
         TB=new Graphics::TBitmap();   //���жϷ�ͼʱ����Ҫ������ͼ������ɫ�ж�
         memset(buff,0,200);
         strncpy(buff,MTab[i].PICT,200);
         TB->LoadFromFile(buff);
         int ax,ay;
         ax=X-atoi(x);if(ax<0)ax=0;if(ax>=TB->Width)ax=TB->Width-2;
         ay=Y-atoi(y);if(ay<0)ay=0;if(ay>=TB->Height)ax=TB->Height-1;
         if(TB->Canvas->Pixels[ax][ay]!=TB->Canvas->Pixels[0][0])oper=true; //�������������
         delete TB;
        }
       else
         oper=true;  //�������Ҫ�����ж�
       if(oper==false)return;  //���������Ч�ģ��򷵻�
       if((MTab[i].KEYCOUNT&0xff)=='2') //��˫���������⴦��
       {
        unsigned short key,key1;
        char buff[10],buff1[10];
        memset(buff,0,10);memset(buff1,0,10);
        strncpy(buff,MTab[i].KEYDOWN1,4);
        strncpy(buff1,MTab[i].KEYDOWN2,4);
        key=atoi(buff);key1=atoi(buff1);
        if(ES.KeyValueCount>0)return; //��������������δ��ʱ
        ES.KeyValue=key1;  //��ֵ1��
        ES.KeyValue1=key;  //��ֵ2
        ES.KeyValueCount=2;
        ES.KeyType=0;
        SetEvent(ES.KeyHandle);
        return;
       }
       else  //�Ե���ֱ�ӽ��д���
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
//�����������ݰ�
 int l;
 struct QQDG P;
 NMUDP2->ReadBuffer((char *)&P,sizeof(struct QQDG),l);
 if(P.KD==AUTOPLAY) //�Զ����Ÿ���
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
  if(P.KD==QUITPLAY) //��ղ��Ŷ���
  {PlaySongCount=0;PlaySongNo=0;}
 if(P.KD==CLOSEPROG) //�رճ���
  {
    if(ES.KeyValueCount>0)return;
    Timer2->Enabled=false;
    ES.KeyValue=0xfffe;
    ES.KeyValueCount=1;
    SetEvent(ES.KeyHandle);     //���������������ź�
    QuitVideo(PlayStream);
    Close();
  }
 if(P.KD==PLAYONEFILE)   //����ָ���ļ�
  {
   CloseVideo(PlayStream);OpenVideo(P.SongName,PlayStream);
  } //����ָ���ļ�
 if(P.KD==SYSTEMRESET)  //���³�ʼ��������Ĳ���
  {
    MyCopyFile();       //���¿����ļ�
    initSystem();       //���³�ʼ��ϵͳ
  }
 if(P.KD==FORWARD)     {ProcessDJ('H');} //���
 if(P.KD==BACKWARD)    {ProcessDJ('I');} //����
 if(P.KD==MUSICECHANGE){ProcessDJ('N');} //�����л�
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
        SetEvent(ES.KeyHandle);     //���������������ź�
        PL_ExitWindow(EWX_REBOOT|EWX_LOGOFF);
  }
 if(P.KD==CLOSECOMPUTER)
  {
        QuitVideo(PlayStream);
        if(ES.KeyValueCount>0)return;
        Timer2->Enabled=false;
        ES.KeyValue=0xfffe;
        ES.KeyValueCount=1;
        SetEvent(ES.KeyHandle);     //���������������ź�
        PL_ExitWindow(EWX_POWEROFF|EWX_SHUTDOWN);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::WriteFwhj()
{
//д��������
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
 //��ʾ��ҳ��������
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
  //�ر���ҳ��ʾ
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
 SetEvent(ES.KeyHandle);     //���������������ź�
 UnregisterHotKey(Handle,0);
}

void __fastcall TForm1::initSystem()
{
 Bhdgjm=atoi(ReadPara("��ŵ�跽ʽ").c_str());
 zysy=ReadPara("��Դʹ��");
 SongNameFont=ReadPara("��������");
 SongNameFontSize=atoi(ReadPara("���������С").c_str());
 SingerFont= ReadPara("��������");
 SingerFontSize=atoi(ReadPara("���������С").c_str());
 InternetOper=atoi(ReadPara("�Ƿ��������").c_str());
 TVAV_STAT=atoi(ReadPara("TVAV��ʼ").c_str());
 SyDh=atoi(ReadPara("ʹ�ö���").c_str());
 JiuCheck=atoi(ReadPara("��ˮ���").c_str());
 RoomStat=atoi(ReadPara("�����ʼ״̬").c_str());  //Ĭ�Ϸ����ǽ�����
 RemoteHostIP=ReadPara("������IP��ַ") ;
 if(RemoteHostIP=="")RemoteHostIP="127.0.0.1";
 Gsyldx=atoi(ReadPara("��������").c_str());
 XPath=ReadPara("����������X��");
 WNetAddConnection(XPath.c_str(),NULL,"X:");
 Gswjm=ReadPara("�����ļ���");
 hwzt=ReadPara("��������");if(hwzt=="")hwzt="Gulim";
 hwztdx=atoi(ReadPara("���������С").c_str());
 if(Gswjm=="")Gswjm="lfw.vob";
 CloseComputerPassword=ReadPara("�ػ�����");
 if(CloseComputerPassword=="") CloseComputerPassword="13873970053";
 PlaySongCount=0;
 PlaySongNo=0;
 PlayStream=0;
 zyzt=0;
}
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
zyzt++;
if(zyzt>=2)   //����ʮ����û���ҵ�������������Ϊϵͳ��Դʹ��MAP�ļ���
{
 zysy="0";  //�л����ֲ�ʽϵͳ
 ReadPath(MAPTABPATH);    //���¶�ȡ����վ��·��
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
//��ʾ����,��ʾ��������
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
   if(zyzt>=2){ReadPath(MAPTABPATH);zysy="1";}      //��ȡ������·��
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
if(rt==SOCKET_ERROR)  //�������ݴ���
{
 closesocket(S);
 WSACleanup();
 return 3;
}
 closesocket(S);
 WSACleanup();
 return 0;
}

