//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "head.h"
#include "MThread.h"
#include "mform.h"
#include "stdio.h"
#include "stdlib.h"
#include "dir.h"
#include "tiao.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall MainThread::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

int ZsSortFunc(const void *p1,const void *p2)  //��������
{
 struct SelSongTab *PP1,*PP2;
 char buff[10],buff1[10];
 PP1=(struct SelSongTab *)p1;
 PP2=(struct SelSongTab *)p2;
 memset(buff,0,10);memset(buff1,0,10);
 strncpy(buff,PP1->A.ZS,2);strncpy(buff1,PP2->A.ZS,2);
 return atoi(buff)-atoi(buff1);
}
int CsSortFunc(const void *p1,const void *p2)   //��������
{
 struct ph *PP1,*PP2;
 PP1=(struct ph *)p1;
 PP2=(struct ph *)p2;
 return PP2->cs-PP1->cs;
}
__fastcall MainThread::MainThread(bool CreateSuspended,
                                  struct EventStru *P1,
                                  struct ShowPictureStru *P2,
                                  struct DrawTextStru *P3)
        : TThread(CreateSuspended)
{
 SPS=P2;
 ES=P1;
 DTS=P3;
}
//---------------------------------------------------------------------------
void __fastcall MainThread::Execute()
{
 struct vpoint
 {
  int x,y;
  char Key;
  char *Pict;
 };
 struct vpoint sz[]=
  {
   {17,136,'1',"main_1_1.bmp"},
   {17,215,'2',"main_1_2.bmp"},
   {17,302,'3',"main_1_3.bmp"},
   {17,384,'4',"main_1_4.bmp"},
   {442,133,'5',"main_1_5.bmp"},
   {442,231,'6',"main_1_6.bmp"},
   {442,306,'7',"main_1_7.bmp"},
   {442,375,'8',"main_1_8.bmp"},
  };
  int row=0,col=0,stat=0;
  AnsiString lj;
  PlayStream=0;
  unsigned long l=1024;
        //---- Place thread code here ----
        int KeyValue;
        memset(ComputerName,0,1024);GetComputerName(ComputerName,&l);
begin:
        XfCount=0;memset((void *)&XfArray[0],0,sizeof(struct Xf)*MAXXFRECCOUNT);
        Form1->PlaySongCount=0;Form1->PlaySongNo=0;
        Mute=0;
        Form1->ShowTime=false;Form1->ShowNextSong=false;
        ES->KeyType=0;
        DrawPicture=true;
        Form1->DrawDhFlag=false;
        ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\welcome.bmp");
        while(1)  //��ʾ��ӭ����
        {
         Form1->MainThreadStat=0; //���ڻ�ӭ����
         KeyValue=ReadKey();
         DrawPicture=true;
         if(KeyValue=='Q')  //�ػ��ź�
          {
           if(CloseComputer()){Form1->Tag=1;ES->KeyType=-1;return;}
           else{ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\welcome.bmp");}
          }
         if(KeyValue=='@') ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\welcome.bmp");
         if(KeyValue=='U'&&Form1->RoomStat==1)break;  //�����˲���      ,�����ӿڲ���
        }
        Form1->DrawDhFlag=true;
        ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_1.bmp");
        Synchronize(Form1->DrawGuangGao);Form1->ShowTime=true;Form1->ShowNextSong=true;
        Form1->Timer1->Tag=1;
        Form1->MainThreadStat=1;  //���ڵ�����
        while(1)
        {
         if(ES->KeyType==-3)break;;
         if(ES->KeyType==-4) goto process;
         KeyValue=ReadKey();
         if(ES->KeyType==-3)break;
process:
         if(ES->KeyValue=='$') goto begin;  //�յ��˳���Ϣ
         DrawPicture=true;
         if(KeyValue=='@'){ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_1.bmp");Synchronize(Form1->DrawGuangGao);continue;}
         if(KeyValue=='C') //�Ϲ��
          {
           lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+sz[row+col*4].Pict;
           ShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());
           if(row>0) row--;
           else row=3;
           lj=AnsiString(PROGRUNPATH)+"picture\\b\\";lj=lj+sz[row+col*4].Pict;
           TranShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());
           continue;
          }
         if(KeyValue=='D')
          {
           lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+sz[row+col*4].Pict;
           ShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());
           if((row+1)<4)row++;
           else row=0;
           lj=AnsiString(PROGRUNPATH)+"picture\\b\\";lj=lj+sz[row+col*4].Pict;
           TranShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());
           continue;
          }
         if(KeyValue=='E')  //����
          {
           lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+sz[row+col*4].Pict;
           ShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());
           if(col>0)col--;
           else col=1;
           lj=AnsiString(PROGRUNPATH)+"picture\\b\\";lj=lj+sz[row+col*4].Pict;
           TranShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());
           continue;
          }
         if(KeyValue=='F')  //�ҹ��
          {
           lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+sz[row+col*4].Pict;
           ShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());
           if(col==0)col++;
           else col=0;
           lj=AnsiString(PROGRUNPATH)+"picture\\b\\";lj=lj+sz[row+col*4].Pict;
           TranShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());
           continue;
          }
         if(KeyValue=='!')
          {
           if(stat==0)
            {
             lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+sz[row+col*4].Pict;
             ShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());
             stat=1;
            }
           else
            {
             lj=AnsiString(PROGRUNPATH)+"picture\\b\\";lj=lj+sz[row+col*4].Pict;
             TranShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());
             stat=0;
            }
            continue;
          }
         if(KeyValue=='U')  //ȷ��
          {
           KeyValue=sz[row+col*4].Key;
          }
process1:
         switch(KeyValue)
          {
           case 49:{Process1();break;}
           case 50:{Process2();break;}
           case 51:{Process3();break;}
           case 52:
                    if(Form1->Bhdgjm==1)
                       Process4();
                    else
                       ProcessBh();
                    break;
           case 53:{Process5();break;}
           case 54:{Process6();break;}
           case 55:{Process7();break;}
           case 56:{Process8();break;}
           case 57:{Form1->ShowNextSong=false;Form1->ShowTime=false;VIPMyLove();Form1->ShowNextSong=true;Form1->ShowTime=true;break;}  //����ģ��ҵ����ֻ���ڰ����ܼ���ʱ��ų���
           case 58: break;  //������
           case 0xfffe:  //�ػ��ź�
                Form1->Tag=1;ES->KeyType=-1;return;
          }
         Form1->DrawDhFlag=true;
         ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_1.bmp");Synchronize(Form1->DrawGuangGao); 
         if(ES->KeyType==-4)
         {
          ES->KeyValue=ES->KeyValue-'a'+49;
          KeyValue=ES->KeyValue;
          ES->KeyType=0;
          DrawPicture=true;goto process1;
         }
        }
}
//---------------------------------------------------------------------------
int __fastcall MainThread::ReadKey()
{
 //��ȡ����
  if(ES->KeyValueCount==0)
  {
   WaitForSingleObject(ES->KeyHandle,INFINITE);
   ResetEvent(ES->KeyHandle);
  }
  if(ES->KeyValueCount==2)
   {
    int TmpKey;
    TmpKey=ES->KeyValue1;
    ES->KeyValueCount=1;
    return TmpKey;
   }
  if(ES->KeyValueCount==1)
   {
    if(ES->KeyType==0)DrawPicture=true;   //�����������������ʾ
    else DrawPicture==false;               //����ǹ��ܼ�������ʾ��������
    ES->KeyValueCount=0;
    return ES->KeyValue;
   }
   return 0;
}

void __fastcall MainThread::Process1()
{
 struct Zmb
  {
   int  x,y;
   char key;
   int  no;
   char *bmp;
  };
 struct Zmb zmb[]=
  {
   {74 ,137,'A',1 ,"py_a.bmp"},{175,133,'B', 2,"py_b.bmp"},{286,127,'C',3,"py_c.bmp"},
   {420,128,'D',4 ,"py_d.bmp"},{532,137,'E', 5,"py_e.bmp"},{631,140,'F',6,"py_f.bmp"},
   {76 ,225,'G',7 ,"py_g.bmp"},{178,218,'H', 8,"py_h.bmp"},{291,212,'I',9,"py_i.bmp"},
   {425,214,'J',10,"py_j.bmp"},{531,220,'K',11,"py_k.bmp"},{639,226,'L',12,"py_l.bmp"},
   {77 ,308,'M',13,"py_m.bmp"},{179,303,'N',14,"py_n.bmp"},{290,297,'O',15,"py_o.bmp"},
   {422,298,'P',16,"py_p.bmp"},{529,303,'Q',17,"py_q.bmp"},{634,307,'R',18,"py_r.bmp"},
   {75 ,392,'S',19,"py_s.bmp"},{180,385,'T',20,"py_t.bmp"},{291,380,'W',21,"py_w.bmp"},
   {424,381,'X',22,"py_x.bmp"},{529,385,'Y',23,"py_y.bmp"},{635,390,'Z',24,"py_z.bmp"}
  };
 int row=0,col=0;
 int Key,stat=0;
 char zm[10],no[10];
 AnsiString lj;
 Form1->DrawDhFlag=true;
 ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_a.bmp");
 Synchronize(Form1->DrawGuangGao);
 memset(zm,0,10);memset(no,0,10);
 Form1->DrawDhFlag=false;
 while(1)
 {
  Form1->ShowTime=false;
  if(ES->KeyType!=0)return;
  Key=ReadKey();
  if(ES->KeyType!=0)return;
  if(Key=='@')
   {
    ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_a.bmp");
    Synchronize(Form1->DrawGuangGao);
    ShowPicture(495,70,AnsiString(PROGRUNPATH)+"picture\\a\\main_a_zm.bmp");
    ShowPicture(673,70,AnsiString(PROGRUNPATH)+"picture\\a\\main_a_bh.bmp");
    DrawText24(508-1,77-1,int(RGB(0,0,0)),"����_GB2312",zm);
    DrawText24(508+1,77+1,int(RGB(255,255,255)),"����_GB2312",zm);
    DrawText24(508,77,int(RGB(0,255,0)),"����_GB2312",zm);

    DrawText24(686-1,77-1,int(RGB(0,0,0)),"����_GB2312",no);
    DrawText24(686+1,77+1,int(RGB(255,255,255)),"����_GB2312",no);
    DrawText24(686,77,int(RGB(0,255,0)),"����_GB2312",no);
   }
  if(Key>='0'&&Key<='9')
   {
    no[strlen(no)]=Key&0xff;
    if(strlen(no)==2)
     {
      if(atoi(no)>=1&&atoi(no)<=24)
       {
         row=(atoi(no)-1)/6;col=(atoi(no)-1)%6;memset(no,0,10);
         ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_a.bmp");
         Synchronize(Form1->DrawGuangGao);
         Key='G';  //����кź��кţ����ѡ���
       }
      else
       {
        memset(no,0,10);
        ShowPicture(495,70,AnsiString(PROGRUNPATH)+"picture\\a\\main_a_zm.bmp");
        ShowPicture(673,70,AnsiString(PROGRUNPATH)+"picture\\a\\main_a_bh.bmp");
        DrawText24(508-1,77-1,int(RGB(0,0,0)),"����_GB2312",zm);
        DrawText24(508+1,77+1,int(RGB(255,255,255)),"����_GB2312",zm);
        DrawText24(508,77,int(RGB(0,255,0)),"����_GB2312",zm);

        DrawText24(686-1,77-1,int(RGB(0,0,0)),"����_GB2312",no);
        DrawText24(686+1,77+1,int(RGB(255,255,255)),"����_GB2312",no);
        DrawText24(686,77,int(RGB(0,255,0)),"����_GB2312",no);
        continue;
       }
     }
    else
     {
      ShowPicture(495,70,AnsiString(PROGRUNPATH)+"picture\\a\\main_a_zm.bmp");
      ShowPicture(673,70,AnsiString(PROGRUNPATH)+"picture\\a\\main_a_bh.bmp");
      DrawText24(508-1,77-1,int(RGB(0,0,0)),"����_GB2312",zm);
      DrawText24(508+1,77+1,int(RGB(255,255,255)),"����_GB2312",zm);
      DrawText24(508,77,int(RGB(0,255,0)),"����_GB2312",zm);

      DrawText24(686-1,77-1,int(RGB(0,0,0)),"����_GB2312",no);
      DrawText24(686+1,77+1,int(RGB(255,255,255)),"����_GB2312",no);
      DrawText24(686,77,int(RGB(0,255,0)),"����_GB2312",no);
     }
   }
  if(Key=='T')  //ɾ����
   {
    if(strlen(zm)>0) {zm[strlen(zm)-1]=0;no[0]=0;}
    ShowPicture(495,70,AnsiString(PROGRUNPATH)+"picture\\a\\main_a_zm.bmp");
    ShowPicture(673,70,AnsiString(PROGRUNPATH)+"picture\\a\\main_a_bh.bmp");
    DrawText24(508-1,77-1,int(RGB(0,0,0)),"����_GB2312",zm);
    DrawText24(508+1,77+1,int(RGB(255,255,255)),"����_GB2312",zm);
    DrawText24(508,77,int(RGB(0,255,0)),"����_GB2312",zm);

    DrawText24(686-1,77-1,int(RGB(0,0,0)),"����_GB2312",no);
    DrawText24(686+1,77+1,int(RGB(255,255,255)),"����_GB2312",no);
    DrawText24(686,77,int(RGB(0,255,0)),"����_GB2312",no);
   }
  if(Key=='S') return;  //����
  if(Key=='G')     //����ѡ���
   {
      zm[strlen(zm)+1]=0;zm[strlen(zm)]=zmb[row*6+col].key&0xff;memset(no,0,10);
      ShowPicture(495,70,AnsiString(PROGRUNPATH)+"picture\\a\\main_a_zm.bmp");
      ShowPicture(673,70,AnsiString(PROGRUNPATH)+"picture\\a\\main_a_bh.bmp");
      DrawText24(508-1,77-1,int(RGB(0,0,0)),"����_GB2312",zm);
      DrawText24(508+1,77+1,int(RGB(255,255,255)),"����_GB2312",zm);
      DrawText24(508,77,int(RGB(0,255,0)),"����_GB2312",zm);

      DrawText24(686-1,77-1,int(RGB(0,0,0)),"����_GB2312",no);
      DrawText24(686+1,77+1,int(RGB(255,255,255)),"����_GB2312",no);
      DrawText24(686,77,int(RGB(0,255,0)),"����_GB2312",no);
      if(strlen(zm)>=4)
       {
        if(ReadSongTab("zhpy",zm,1)==0)
         {
          DisplayMsg("����û���ҵ�");
          ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_a.bmp");
          Synchronize(Form1->DrawGuangGao);
          memset(zm,0,10);memset(no,0,10);
         }
        else
         {
          ProcessGQ();
          ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_a.bmp");
          Synchronize(Form1->DrawGuangGao);
          memset(zm,0,10);memset(no,0,10);
         }
       }
   }
  if(Key=='U')     //����ȷ����
   {
    if(strlen(zm)==0)continue; //���û��ѡ���ţ������
    if(ReadSongTab("zhpy",zm,1)==0)
     {
      DisplayMsg("����û���ҵ�");
      ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_a.bmp");
      Synchronize(Form1->DrawGuangGao);
      ShowPicture(495,70,AnsiString(PROGRUNPATH)+"picture\\a\\main_a_zm.bmp");
      ShowPicture(673,70,AnsiString(PROGRUNPATH)+"picture\\a\\main_a_bh.bmp");
      memset(zm,0,10);memset(no,0,10);
      DrawText24(508-1,77-1,int(RGB(0,0,0)),"����_GB2312",zm);
      DrawText24(508+1,77+1,int(RGB(255,255,255)),"����_GB2312",zm);
      DrawText24(508,77,int(RGB(0,255,0)),"����_GB2312",zm);

      DrawText24(686-1,77-1,int(RGB(0,0,0)),"����_GB2312",no);
      DrawText24(686+1,77+1,int(RGB(255,255,255)),"����_GB2312",no);
      DrawText24(686,77,int(RGB(0,255,0)),"����_GB2312",no);
     }
    else
     {
      ProcessGQ();  //������ѡ����
      ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_a.bmp");
      Synchronize(Form1->DrawGuangGao);
      memset(zm,0,10);memset(no,0,10);
     }
   }
  if(Key=='C')     //�Ϲ��
   {
    if(row>0)
    {
     lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+zmb[row*6+col].bmp;
     ShowPicture(zmb[row*6+col].x,zmb[row*6+col].y,lj.c_str());
     row--;
    }
   }
  if(Key=='D')     //�¹��
   {
    if(row<3)
    {
     lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+zmb[row*6+col].bmp;
     ShowPicture(zmb[row*6+col].x,zmb[row*6+col].y,lj.c_str());
     row++;
    }
   }
  if(Key=='E')     //����
   {
    if(col>0)
    {
     lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+zmb[row*6+col].bmp;
     ShowPicture(zmb[row*6+col].x,zmb[row*6+col].y,lj.c_str());col--;
    }
   }
  if(Key=='F')     //����
   {
    if(col<5)
    {
     lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+zmb[row*6+col].bmp;
     ShowPicture(zmb[row*6+col].x,zmb[row*6+col].y,lj.c_str());col++;
    }
   }
  if(Key=='!')
   {
    stat++;stat=stat%2;
    if(stat==0)
     {
      lj=AnsiString(PROGRUNPATH)+"picture\\b\\";lj=lj+zmb[row*6+col].bmp;
      TranShowPicture(zmb[row*6+col].x,zmb[row*6+col].y,lj.c_str());
     }
    if(stat==1)
     {
      lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+zmb[row*6+col].bmp;
      ShowPicture(zmb[row*6+col].x,zmb[row*6+col].y,lj.c_str());
     }
   }
 }
}

void __fastcall MainThread::DrawText16(int x,int y,int color,char *Fontname,char *Text)
{
//��ʾʮ��λ����
 if(ES->KeyType!=0)return;
 DTS->x=x;
 DTS->y=y;
 DTS->Color=color;
 memset(DTS->FontName,0,1024);
 strcat(DTS->FontName,Fontname);
 memset(DTS->Text,0,1024);
 strcat(DTS->Text,Text);
 DTS->fonttype=0;
 Synchronize(Form1->TText);
}

void __fastcall MainThread::DrawText24(int x,int y,int color,char *Fontname,char *Text)
{
//��ʾʮ��λ����
 if(ES->KeyType!=0)return;
 DTS->x=x;
 DTS->y=y;
 DTS->Color=color;
 memset(DTS->FontName,0,1024);
 strcat(DTS->FontName,Fontname);
 memset(DTS->Text,0,1024);
 strcat(DTS->Text,Text);
 DTS->fonttype=1;
 Synchronize(Form1->TText);
}
void __fastcall MainThread::DrawText32(int x,int y,int color,char *Fontname,char *Text)
{
//��ʾʮ��λ����
 if(ES->KeyType!=0)return;
 DTS->x=x;
 DTS->y=y;
 DTS->Color=color;
 memset(DTS->FontName,0,1024);
 strcat(DTS->FontName,Fontname);
 memset(DTS->Text,0,1024);
 strcat(DTS->Text,Text);
 DTS->fonttype=2;
 Synchronize(Form1->TText); 
}

void __fastcall MainThread::ShowPicture(int x,int y,AnsiString Fname)
{
//��ʾһ��ͼƬ
if(ES->KeyType!=0)return;
SPS->x=x;
SPS->y=y;
SPS->type=0;
memset(SPS->fname,0,1024);
strcat(SPS->fname,Fname.c_str());
Synchronize(Form1->TDP);
}

void __fastcall MainThread::TranShowPicture(int x,int y,AnsiString Fname)
{
//��ʾһ��ͼƬ
if(ES->KeyType!=0)return;
SPS->x=x;
SPS->y=y;
SPS->type=1;
memset(SPS->fname,0,1024);
strcat(SPS->fname,Fname.c_str());
Synchronize(Form1->TDP);
}

int __fastcall MainThread::ReadSongTab(AnsiString Field,AnsiString Value,int Compare)
{
  struct FieldTab
   {
    char *FieldName;   //�ֶ���
    int  Offset;           //ƫ��ֵ
    int  Len;              //�ֶγ���
   };
  struct FieldTab FT[]={
                      {"gm"    ,1  ,40},
                      {"bh"    ,41 ,10},
                      {"zhpy"  ,51 ,4 },
                      {"gx"    ,55 ,10},
                      {"lb"    ,65 ,10},
                      {"yz"    ,75 ,10},
                      {"yssd"  ,85 ,2 },
                      {"bcsd"  ,87 ,2 },
                      {"zs"    ,89 ,4 },
                      {"dccs"  ,93 ,10},
                      {"yldx"  ,103,2 },
                      {"vip"   ,105,1 },
                     };
  struct SongTab STab;
  FILE *FP;
  int rcount,k;
  int off,len;
  char recordbuff[1024];   //һ����¼�Ļ���
  Form1->SongCount=0;
  FP=fopen(GQPATH,"rb");
  if(FP==NULL)return 0;   //û�ж�ȡ������
  fseek(FP,4,SEEK_SET);fread((void *)&rcount,sizeof(int),1,FP);k=0;
  for(int i=0;i<12;i++)            //�ҳ��ֶ�����Ӧ��ƫ��ֵ�Ͷ�Ӧ�ĳ���
  {
   AnsiString fn;
   fn=FT[i].FieldName;fn=fn.UpperCase();
   if(fn.Trim().UpperCase()==Field.Trim().UpperCase())
      {
        off=FT[i].Offset;
        len=FT[i].Len;
        break;
      }
  }
#if YS
  for(int l=0;l<rcount&&k<200&&l<5000;l++)
#else
  for(int l=0;l<rcount&&k<5000;l++)
#endif
  {
   char buff[100];
   AnsiString v;
   fseek(FP,SongTabHead+sizeof(struct SongTab)*l,SEEK_SET);
   fread(recordbuff,sizeof(struct SongTab),1,FP);
   if(recordbuff[0]&0xff=='*')continue;   //���˵���������ɾ����ǵ�
   memset(buff,0,100);strncpy(buff,&recordbuff[off],len);v=buff;
   switch(Compare)
    {
      case 0:   //��ȫ���
           if(v.Trim()!=Value.Trim()) continue;break;
      case 1:   //�����Ƚ�
           if(v.Pos(Value)!=1) continue;break;
    }
    memset((void *)&STab,0,sizeof(struct SongTab));
    memcpy((void *)&STab,recordbuff,sizeof(struct SongTab));
    if(Field!="vip"&&(STab.VIP[0]=='T'&&STab.VIP[0]=='t'))continue;
    Form1->SelSong[k].A=STab;
    Form1->SelSong[k].Stat=0;
    Form1->SelSong[k].RecNo=l;
    for(int ss=Form1->PlaySongNo;ss<Form1->PlaySongCount;ss++)  //�ҵ������Ƿ�ѡ��
    {
      char buff[100];AnsiString vs,vs1;
      memset(buff,0,100);strncpy(buff,Form1->SelSong[k].A.BH,10);vs=buff;vs=vs.Trim();
      memset(buff,0,100);strncpy(buff,Form1->PlaySongTab[ss].A.BH,10);vs1=buff;vs1=vs1.Trim();
      if(vs==vs1){Form1->SelSong[k].Stat=1;break;}
    }
    //���˵��ո�
    k++;
  }
fclose(FP);Form1->SongCount=k;return k;
}
void __fastcall MainThread::DisplayMsg(AnsiString Msg)
{
  TranShowPicture(160,240,AnsiString(PROGRUNPATH)+"picture\\a\\msgback.bmp");
  DrawText16(170-1,250-1,int(RGB(0,0,0)),"����_GB2312",Msg.c_str());
  DrawText16(170+1,250+1,int(RGB(255,255,255)),"����_GB2312",Msg.c_str());
  DrawText16(170,250,int(RGB(0,0,255)),"����_GB2312",Msg.c_str());
  int key,tcount=0;
  while(1)
  {
    if(ES->KeyType!=0)return;
    key=ReadKey();
    if(ES->KeyType!=0)return;
    if(key=='!')tcount++;
    else return;
    if(tcount>2)  //���˻س���,������ʮ���ӳ�ʱ
       return;
  }
}

void __fastcall MainThread::ProcessGQ()
{
  //��������,�ȴ��û�ѡ��
  int PageNo=0;
  int SelNo=0,pc;
  int Key;
  qsort((void *)&Form1->SelSong,Form1->SongCount,sizeof(struct SelSongTab),ZsSortFunc);
  ShowSelSong(PageNo,SelNo);
  pc=Form1->SongCount/6;if(Form1->SongCount%6)pc++;
  while(1)
  {
   Form1->ShowTime=true;Form1->ShowNextSong=true;
   if(ES->KeyType!=0)return;
   Key=ReadKey();
   if(ES->KeyType!=0)return;
   if(Key=='0'&&Form1->SongCount>0) //���ȸ���
    {
     if(Form1->SelSong[PageNo*6+SelNo].Stat==1) //���������ѡ�У�����в���
      {
       struct SelSongTab ss;
       for(int i=Form1->PlaySongNo;i<Form1->PlaySongCount;i++)  //�ڼ�ѡ�������ҵ���Ӧ�ĸ�����Ȼ������
        {
         if(Form1->PlaySongTab[i].RecNo==Form1->SelSong[PageNo*6+SelNo].RecNo)
          {
             ss=Form1->PlaySongTab[i];int j;
             for(j=i;j>Form1->PlaySongNo;j--){Form1->PlaySongTab[j]=Form1->PlaySongTab[j-1];}
             Form1->PlaySongTab[j]=ss;DisplayMsg("��ϲ�㣡���ȳɹ���");break;
          }
        }
      }
    }
   if(Key=='8') //��������
    {
     char c;int fc;c=ReadZs();fc=FilterZs(c);
     if(fc==0){ShowSelSong(PageNo,SelNo);}
     else{pc=Form1->SongCount/6;if(Form1->SongCount%6)pc++;PageNo=0;SelNo=0;ShowSelSong(PageNo,SelNo);}
    }
   if(Key=='9') //ƴ������
    {
     char c;int fc;c=ReadPy();fc=FilterPy(c);
     if(fc==0){ShowSelSong(PageNo,SelNo);}
     else{pc=Form1->SongCount/6;if(Form1->SongCount%6)pc++;PageNo=0;SelNo=0;ShowSelSong(PageNo,SelNo);}
    }
   if(Key=='@'){ShowSelSong(PageNo,SelNo);}  //ˢ�½���
   if(Key=='A')  //�Ϸ�
   {if(PageNo>0){PageNo--;SelNo=0;ShowSelSong(PageNo,SelNo);}continue;}
   if(Key=='B')  //�·�
   {if((PageNo+1)<pc){PageNo++;SelNo=0;ShowSelSong(PageNo,SelNo);}continue;}
   if(Key=='S'){Form1->SongCount=0;return;}  //���ؼ�
   if(Key=='D')     //�¹��
    {if((SelNo+PageNo*6+1)<Form1->SongCount&&SelNo<5){SelNo++;ShowSelSong(PageNo,SelNo);}}
   if(Key=='C')     //�Ϲ��
    {if(SelNo>0&&Form1->SongCount>0){SelNo--;ShowSelSong(PageNo,SelNo);}}
   if(Key=='U')     //�س�ȷ����
    {
     if((SelNo+PageNo*6)<Form1->SongCount)
      {
       if(Form1->SelSong[SelNo+PageNo*6].Stat==0)
        {
          Form1->SelSong[SelNo+PageNo*6].PlayOff=0;
          Form1->SelSong[SelNo+PageNo*6].Stat=1;
          Form1->PlaySongTab[Form1->PlaySongCount]=Form1->SelSong[SelNo+PageNo*6];
          Form1->PlaySongTab[Form1->PlaySongCount].Stat=0;
          SongAddClick(Form1->SelSong[SelNo+PageNo*6].RecNo);  //����һ�ε㲥��
          Form1->PlaySongCount++;
        }
       ShowSelSong(PageNo,SelNo);
      }
    }
   if(Key>='1'&&Key<='6')
    {
     if((PageNo*6+(Key-'1'))<Form1->SongCount)
      {
       SelNo=(Key-'1')&0xff;
       if(Form1->SelSong[SelNo+PageNo*6].Stat==0)
        {
          Form1->SelSong[SelNo+PageNo*6].PlayOff=0;
          Form1->SelSong[SelNo+PageNo*6].Stat=1;
          Form1->PlaySongTab[Form1->PlaySongCount]=Form1->SelSong[SelNo+PageNo*6];
          Form1->PlaySongTab[Form1->PlaySongCount].Stat=0;
          SongAddClick(Form1->SelSong[SelNo+PageNo*6].RecNo);  //����һ�ε㲥��
          Form1->PlaySongCount++;
        }
       ShowSelSong(PageNo,SelNo);
      }
    }
  }
}

void __fastcall MainThread::ShowSelSong(int PageNo,int SelNo,int stat)
{
 //��ʾ��ѡ����
 struct ffblk fb;
 int y[]={137,182,230,275,320,367};
 int y1[]={186,221,246,131,140,192};
 Form1->DrawDhFlag=false;
 if(stat==0)
  {
    ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_b.bmp");
  }
 else
  {
    ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_12.bmp");
  }
 Synchronize(Form1->DrawGuangGao);
 for(int i=0;i<6&&(i+PageNo*6)<Form1->SongCount;i++)
  {
    char gqbuff[100],gxbuff[100];
    AnsiString msg,gxm;
    char gb;
    memset(gqbuff,0,100);memset(gxbuff,0,100);
    gb=Form1->SelSong[PageNo*6+i].A.LB[0]&0xff; //����Ǻ��������
    memcpy(gqbuff,Form1->SelSong[PageNo*6+i].A.GM,20);
    memcpy(gxbuff,Form1->SelSong[PageNo*6+i].A.GX,10);
    if(Form1->SelSong[PageNo*6+i].Stat==0)  //����û�б�ѡ��
      {
       DrawText(138+1,y[i]+1,int(RGB(255,255,255)),Form1->SongNameFont,gqbuff,Form1->SongNameFontSize,gb);
       DrawText(138-1,y[i]-1,int(RGB(0,0,255)),Form1->SongNameFont,gqbuff,Form1->SongNameFontSize,gb);
       DrawText(138,y[i],int(RGB(0,0,255)),Form1->SongNameFont,gqbuff,Form1->SongNameFontSize,gb);

       DrawText(550+1,y[i]+1,int(RGB(255,255,255)),Form1->SingerFont,gxbuff,Form1->SingerFontSize,gb);
       DrawText(550-1,y[i]-1,int(RGB(0,0,0)),Form1->SingerFont,gxbuff,Form1->SingerFontSize,gb);
       DrawText(550,y[i],int(RGB(0,0,255)),Form1->SingerFont,gxbuff,Form1->SingerFontSize,gb);
      }
    else
      {
       DrawText(138+1,y[i]+1,int(RGB(255,255,255)),Form1->SongNameFont,gqbuff,Form1->SongNameFontSize,gb);
       DrawText(138-1,y[i]-1,int(RGB(0,0,0)),Form1->SongNameFont,gqbuff,Form1->SongNameFontSize,gb);
       DrawText(138,y[i],int(RGB(255,0,0)),Form1->SongNameFont,gqbuff,Form1->SongNameFontSize,gb);

       DrawText(550+1,y[i]+1,int(RGB(255,255,255)),Form1->SingerFont,gxbuff,Form1->SingerFontSize,gb);
       DrawText(550-1,y[i]-1,int(RGB(255,255,255)),Form1->SingerFont,gxbuff,Form1->SingerFontSize,gb);
       DrawText(550,y[i],int(RGB(255,0,0)),Form1->SingerFont,gxbuff,Form1->SingerFontSize,gb);
      }
  }
 //������ʾѡ���Ӧ�ĸ�����Ƭ
if((SelNo+PageNo*6)<Form1->SongCount)
  {
   char buff[1000];
   AnsiString lj;
   lj=AnsiString(PROGRUNPATH)+"picture\\singer\\";
   memset(buff,0,1000);
   strncpy(buff,Form1->SelSong[PageNo*6+SelNo].A.GX,10);
   lj=lj+buff;lj=lj.Trim();lj=lj+".bmp";
   TranShowPicture(610,y1[SelNo],AnsiString(PROGRUNPATH)+"picture\\a\\singback.bmp");
   if(findfirst(lj.c_str(),&fb,FA_ARCH|FA_RDONLY)==0)
   {
    ShowPicture(623,y1[SelNo]+13,lj.c_str());
   }
   else
    {
     ShowPicture(623,y1[SelNo]+13,AnsiString(PROGRUNPATH)+"picture\\a\\repl.bmp");
    }
   TranShowPicture(88,y[SelNo],AnsiString(PROGRUNPATH)+"picture\\a\\cursor2.bmp");
  }
 DrawText16(115-1,444-1,int(RGB(0,0,0)),"����_GB2312",IntToStr(PageNo+1).c_str());
 DrawText16(115+1,444+1,int(RGB(255,255,255)),"����_GB2312",IntToStr(PageNo+1).c_str());
 DrawText16(115,444,int(RGB(0,255,0)),"����_GB2312",IntToStr(PageNo+1).c_str());
 int sc=Form1->SongCount/6;if(Form1->SongCount%6)sc++;
 DrawText16(173+1,444+1,int(RGB(0,0,0)),"����_GB2312",IntToStr(sc).c_str());
 DrawText16(173-1,444-1,int(RGB(255,255,255)),"����_GB2312",IntToStr(sc).c_str()); 
 DrawText16(173,444,int(RGB(255,0,0)),"����_GB2312",IntToStr(sc).c_str());
}

void __fastcall MainThread::Process2()
{
  int Key,stat=0,row=0,col=0;
  int x[]={82,80,78,83,83,428,428,428,428,428};
  int y[]={112,192,271,348,419,118,195,271,345,418};
  AnsiString lj;
  char *wjm[]={
                "main_4_0.bmp",
                "main_4_1.bmp",
                "main_4_2.bmp",
                "main_4_3.bmp",
                "main_4_4.bmp",
                "main_4_5.bmp",
                "main_4_6.bmp",
                "main_4_7.bmp",
                "main_4_8.bmp",
                "main_4_9.bmp"
              };
  char v[]={'1','2','3','4','5','6','7','8','9','0'};  //��Ӧ���ַ�ֵ
  Form1->DrawDhFlag=true;
  ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_4.bmp");
  Synchronize(Form1->DrawGuangGao);
  while(1)
   {
     Form1->ShowNextSong=true;Form1->ShowTime=true;
     if(ES->KeyType!=0)return;
     Key=ReadKey();
     if(ES->KeyType!=0)return;
     if(Key=='@')
      {
       ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_4.bmp");
       Synchronize(Form1->DrawGuangGao);
      }
     if(Key>='0'&&Key<='9')
      {
        char buff[10];memset(buff,0,10);buff[0]=Key&0xff;
        if(ReadSongTab("zs",buff,0)==0)
         {
           DisplayMsg("����û���ҵ����Բ���!");
           ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_4.bmp");
           Synchronize(Form1->DrawGuangGao);
         }
        else
         {
           ProcessGQ();ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_4.bmp");
           Synchronize(Form1->DrawGuangGao);
         }
      }
     if(Key=='S')return;
     if(Key=='U')   //����ȷ����
     {
      char buff[10];memset(buff,0,10);buff[0]=v[row+col*5];
      if(ReadSongTab("zs",buff,0)==0)
       {
        DisplayMsg("����û���ҵ����Բ���!");
        ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_4.bmp");
        Synchronize(Form1->DrawGuangGao);
       }
      else
       {
        ProcessGQ();
        ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_4.bmp");
        Synchronize(Form1->DrawGuangGao);
       }
     }
     if(Key=='C')   //�Ϲ��
      {
       if(row>0)
         {
          lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+wjm[row+col*5];
          TranShowPicture(x[row+col*5],y[row+col*5],lj.c_str());row--;
         }
      }
     if(Key=='D')   //�¹��
      {
       if(row<4)
        {
          lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+wjm[row+col*5];
          TranShowPicture(x[row+col*5],y[row+col*5],lj.c_str());row++;
        }
      }
     if(Key=='E')  //����
      {
       if(col>0)
         {
          lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+wjm[row+col*5];
          TranShowPicture(x[row+col*5],y[row+col*5],lj.c_str());col--;
         }
      }
     if(Key=='F')
      {
       if(col==0)
         {
          lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+wjm[row+col*5];
          TranShowPicture(x[row+col*5],y[row+col*5],lj.c_str());col++;
         }
      }
     if(Key=='!') //��ʱˢ��
      {

       if(stat==0)
       {
        lj=AnsiString(PROGRUNPATH)+"picture\\b\\";lj=lj+wjm[row+col*5];
        TranShowPicture(x[row+col*5],y[row+col*5],lj.c_str());stat=1;
       }
       else
       {
        lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+wjm[row+col*5];
        TranShowPicture(x[row+col*5],y[row+col*5],lj.c_str());stat=0;
       }
      }
   }
}

void __fastcall MainThread::Process3()
{
  //�������ǵ��
 int col=0,stat=0;
 int x[]={97,302,513};
 int y[]={381,385,382};
 char *wjm[]={"main_3_1.bmp","main_3_2.bmp","main_3_3.bmp"};
 int Key;
 AnsiString lj;
 char selkey;
 int pcount,pageno;
 while(1)
 {
  Form1->DrawDhFlag=true;
  ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_3.bmp");
  Synchronize(Form1->DrawGuangGao);
  Form1->DrawDhFlag=false;
  while(1)
   {
    Form1->ShowNextSong=true;Form1->ShowTime=true;
    if(ES->KeyType!=0)return;
    Key=ReadKey();
    if(ES->KeyType!=0)return;
    if(Key=='@'){ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_3.bmp");Synchronize(Form1->DrawGuangGao);}
    if(Key=='S')return;   //����
    if(Key>='1'&&Key<='3'){selkey=Key;break;}
    if(Key=='U'){selkey=col+'1';break;}
    if(Key=='E')   //����
     {lj=AnsiString(PROGRUNPATH)+"picture\\c\\";if(col>0){lj=lj+wjm[col];TranShowPicture(x[col],y[col],lj.c_str());col--;}}
    if(Key=='F')   //����
     {lj=AnsiString(PROGRUNPATH)+"picture\\c\\";if(col<2){lj=lj+wjm[col];TranShowPicture(x[col],y[col],lj.c_str());col++;}}
    if(Key=='!')
     {
      if(stat==0)
       {lj=AnsiString(PROGRUNPATH)+"picture\\b\\";lj=lj+wjm[col];TranShowPicture(x[col],y[col],lj.c_str());stat=1;continue;}
      if(stat==1)
       {lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+wjm[col];ShowPicture(x[col],y[col],lj.c_str());stat=0;continue;}
     }
   }
  //��ȡ����
  ReadSinger(0,selkey);
  if(SingerCount==0){DisplayMsg("����û���ҵ�!");continue;}
  pcount=SingerCount/8;if(SingerCount%8)pcount++;
  pageno=0;
  ShowSinger(0,pcount);
  while(1)    //������и���ѡ����
   {
    Form1->ShowNextSong=true;Form1->ShowTime=false;
    if(ES->KeyType!=0)return;
    Key=ReadKey();
    if(ES->KeyType!=0)return;
    if(Key=='@'){ShowSinger(pageno,pcount);}
    if(Key=='S')break;   //���ز���
    if(Key=='9')         //ƴ������
     {
      char py;int c;py=ReadPy();c=SingerFilterPy(py);
      if(c==0){ShowSinger(pageno,pcount);continue;}  //û���ҵ����������ģ���������
      pageno=0;pcount=SingerCount/8;if(SingerCount%8)pcount++;
      ShowSinger(0,pcount); 
     }
    if(Key>='1'&&Key<='8')  //ѡ����һ������
     {
      char buff[100];memset(buff,0,100);
      if(((Key-'1')+pageno*8)<SingerCount)
       {
        strncpy(buff,Singer[(Key-'1'+pageno*8)].xm,20);
        if(ReadSongTab("gx",buff,0)==0)DisplayMsg("�Բ���û���ҵ�����!");
        else ProcessGQ();
        ShowSinger(pageno,pcount);continue;
       }
     }
    if(Key=='A')   //�Ϸ�
     {
      if(pageno>0){pageno--;ShowSinger(pageno,pcount);}
     }
    if(Key=='B')   //�·�
     {
      if((pageno+1)<pcount){pageno++;ShowSinger(pageno,pcount);}
     }
   };
  }
}

int __fastcall MainThread::ReadSinger(int fno,AnsiString value)
{
 //��ȡ���ǿ�
 FILE *FP;
 AnsiString vv;
 int rcount,k=0;
 struct SingerTab ST;
 SingerCount=0;
 char buff[10];
 FP=fopen(GXPATH,"rb");
 if(FP==NULL)return 0;
 fseek(FP,4,SEEK_SET);
 fread((void *)&rcount,sizeof(int),1,FP);
 for(int l=0;l<rcount;l++)
  {
   fseek(FP,sizeof(struct SingerTab)*l+SingerHeadSize,SEEK_SET);
   fread((void *)&ST,sizeof(struct SingerTab),1,FP);
   if(ST.delflag=='*')continue;
   switch(fno)
    {
     case 0:   //ѡ���Ա�
          memset(buff,0,10);
          buff[0]=ST.xb[0]&0xff;
          if(value.Trim().UpperCase()==buff){Singer[k]=ST;k++;}
          break;
     case 1:   //ѡ�����ƴ��
          memset(buff,0,10);strncpy(buff,ST.qp,10);vv=buff;vv=vv.Trim();vv=vv.UpperCase();
          if(vv==value){Singer[k]=ST;k++;}
    }
  }
  fclose(FP);
  SingerCount=k;
  return k;
}

void __fastcall MainThread::ShowSinger(int pageno,int selno)
{
 //��ʾ���ǽ���
 struct gx
   {
    int x,y;
    char *bh;
   };
struct gx gxsz[]=
   {

    {73 ,124,"singer_1.bmp"},
    {253,124,"singer_2.bmp"},
    {426,124,"singer_3.bmp"},
    {605,124,"singer_4.bmp"},
    {73 ,321,"singer_5.bmp"},
    {253,321,"singer_6.bmp"},
    {426,321,"singer_7.bmp"},
    {605,321,"singer_8.bmp"}
   };
 struct ffblk fb;
 int Key;
 char buff[100];
 AnsiString lj;
 Form1->DrawDhFlag=true;
 ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_7.bmp");
 Form1->DrawDhFlag=false;
 Synchronize(Form1->DrawGuangGao);
 for(int row=0;row<2;row++)
 for(int col=0;col<4;col++)
   {
    if((row*4+col+pageno*8)<SingerCount)  //û�г�����ʾ��Χ������ʾ
     {
      memset(buff,0,100);
      strncpy(buff,Singer[row*4+col+pageno*8].xm,24);
      lj=AnsiString(PROGRUNPATH)+"picture\\singer\\";lj=lj+buff;lj=lj.Trim();lj=lj+".bmp";
      if(findfirst(lj.c_str(),&fb,FA_ARCH|FA_RDONLY)==0)  //�ҵ�����ͼƬ������ʾ������
       {
        ShowPicture(gxsz[row*4+col].x-2,gxsz[row*4+col].y-3,lj.c_str());
       }
      else   //û�У�����ʾ�滻ͼƬ�͸�������
       {
        ShowPicture(gxsz[row*4+col].x-2,gxsz[row*4+col].y-3,AnsiString(PROGRUNPATH)+"picture\\a\\repl.bmp");
        lj=buff;lj=lj.Trim();
        DrawText16(gxsz[row*4+col].x-1,gxsz[row*4+col].y+130,int(RGB(0,0,0)),"����_GB2312",lj.c_str());
        DrawText16(gxsz[row*4+col].x+1,gxsz[row*4+col].y+131,int(RGB(255,255,255)),"����_GB2312",lj.c_str());
        DrawText16(gxsz[row*4+col].x,gxsz[row*4+col].y+132,int(RGB(255,0,0)),"����_GB2312",lj.c_str());
       }
      lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+gxsz[row*4+col].bh;
      TranShowPicture(gxsz[row*4+col].x-20,gxsz[row*4+col].y-3,lj.c_str());
     }
   }
 DrawText16(643-1,75-1,int(RGB(0,0,0)),"����_GB2312",IntToStr(pageno+1).c_str());
 DrawText16(643+1,75+1,int(RGB(255,255,255)),"����_GB2312",IntToStr(pageno+1).c_str());
 DrawText16(643,75,int(RGB(0,0,255)),"����_GB2312",IntToStr(pageno+1).c_str());
 DrawText16(698-1,75-1,int(RGB(0,0,0)),"����_GB2312",IntToStr(selno).c_str());
 DrawText16(698+1,75+1,int(RGB(255,255,255)),"����_GB2312",IntToStr(selno).c_str());
 DrawText16(698,75,int(RGB(0,255,0)),"����_GB2312",IntToStr(selno).c_str());
}

void __fastcall MainThread::Process4()
{
 //������ŵ��
 int Key;
 char bh[100];     //�������
 int PageNo,SelNo,PageCount;//ѡ���ҳ�ź͸������
 memset(bh,0,100);
 Form1->DrawDhFlag=true;
 ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_c.bmp");
 Form1->DrawDhFlag=false;
 Synchronize(Form1->DrawGuangGao);
 PageNo=0;SelNo=0;Form1->SongCount=0;PageCount=0;
 while(1)
 {
  if(ES->KeyType!=0)return;
  Key=ReadKey();
  if(ES->KeyType!=0)return;
  if(Key=='@')
   {
    ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_c.bmp");
    Synchronize(Form1->DrawGuangGao);
    ShowNoSong(PageNo,SelNo);
    DrawText16(220+1,127+1,int(RGB(255,255,255)),"����_GB2312",bh);
    DrawText16(220-1,127-1,int(RGB(0,0,0)),"����_GB2312",bh);
    DrawText16(220,127,int(RGB(0,255,0)),"����_GB2312",bh);
   }
  if(Key=='S')return;
  if(Key=='T')
   {
    if(strlen(bh)>0)
    {
     bh[strlen(bh)-1]=0;
     ReadSongTab("bh",bh,1);
     PageCount=Form1->SongCount/6;if(Form1->SongCount%6)PageCount++;
     ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_c.bmp");
     Synchronize(Form1->DrawGuangGao);
     if(Form1->SongCount>0)
      {
       PageNo=0;SelNo=0;
       ShowNoSong(PageNo,SelNo);
      }
     }
    DrawText16(220,127,int(RGB(0,0,0)),"����_GB2312",bh);
    DrawText16(222,129,int(RGB(255,255,255)),"����_GB2312",bh);
    DrawText16(221,128,int(RGB(0,255,0)),"����_GB2312",bh);
   }
  if(Key>='0'&&Key<='9'&&strlen(bh)<10)
   {
    bh[strlen(bh)]=Key&0xff;
    ReadSongTab("bh",bh,1);
    PageCount=Form1->SongCount/6;if(Form1->SongCount%6)PageCount++;
    ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_c.bmp");
    Synchronize(Form1->DrawGuangGao);
    if(Form1->SongCount>0)
     {
      PageNo=0;SelNo=0;
      ShowNoSong(PageNo,SelNo);
     }
    DrawText16(220,127,int(RGB(0,0,0)),"����_GB2312",bh);
    DrawText16(222,129,int(RGB(255,255,255)),"����_GB2312",bh);
    DrawText16(221,128,int(RGB(0,255,0)),"����_GB2312",bh);
   }
  if(Key=='C') //�Ϲ��
   {
    if(SelNo>0)
     {
      SelNo--;
      ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_c.bmp");
      Synchronize(Form1->DrawGuangGao);
      ShowNoSong(PageNo,SelNo);
      DrawText16(220,127,int(RGB(255,255,255)),"����_GB2312",bh);
      DrawText16(222,129,int(RGB(255,255,255)),"����_GB2312",bh);
      DrawText16(221,128,int(RGB(0,255,0)),"����_GB2312",bh);
     }
   }
  if(Key=='D') //�¹��
   {
    if((SelNo+1+PageNo*6)<Form1->SongCount&&(1+SelNo)<6)
     {
      SelNo++;
      ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_c.bmp");
      Synchronize(Form1->DrawGuangGao);
      ShowNoSong(PageNo,SelNo);
      DrawText16(220,127,int(RGB(255,255,255)),"����_GB2312",bh);
      DrawText16(222,129,int(RGB(0,0,0)),"����_GB2312",bh);
      DrawText16(221,128,int(RGB(0,255,0)),"����_GB2312",bh);
     }
   }
  if(Key=='A')  //�Ϸ�
   {
    if(PageNo>0)
     {
      PageNo--;
      ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_c.bmp");
      Synchronize(Form1->DrawGuangGao);
      ShowNoSong(PageNo,SelNo);
      DrawText16(220,127,int(RGB(0,0,0)),"����_GB2312",bh);
      DrawText16(222,129,int(RGB(255,255,255)),"����_GB2312",bh);
      DrawText16(221,128,int(RGB(0,255,0)),"����_GB2312",bh);
     }
   }
  if(Key=='B')  //�·�
   {
    if((PageNo+1)<PageCount)
     {
      PageNo++;
      if((SelNo+PageNo*6)>=Form1->SongCount)SelNo=0;
      ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_c.bmp");
      Synchronize(Form1->DrawGuangGao);
      ShowNoSong(PageNo,SelNo);
      DrawText16(220,127,int(RGB(0,0,0)),"����_GB2312",bh);
      DrawText16(222,129,int(RGB(255,255,255)),"����_GB2312",bh);
      DrawText16(221,128,int(RGB(0,255,0)),"����_GB2312",bh);
     }
   }
  if(Key=='U')  //ȷ��
   {
    if((PageNo*6+SelNo)<Form1->SongCount)
     {
      if(Form1->SelSong[PageNo*6+SelNo].Stat==0)
       {
        Form1->PlaySongTab[Form1->PlaySongCount]=Form1->SelSong[PageNo*6+SelNo];
        Form1->PlaySongCount++;
        Form1->SelSong[PageNo*6+SelNo].Stat=1;
        ShowNoSong(PageNo,SelNo);
       }
     }
   }
 }
}

void __fastcall MainThread::Process5()
{
 //�������ֵ��
 struct pict
 {
  int x,y;
  char *pict;
  char *key;
 };
 struct pict sz[]=
  {
   {128,132,"main_2_1.bmp","1"},
   { 74,191,"main_2_2.bmp","2"},
   { 49,253,"main_2_3.bmp","3"},
   { 77,313,"main_2_4.bmp","4"},
   {404,222,"main_2_5.bmp","5"},
   {380,281,"main_2_6.bmp","6"},
   {363,346,"main_2_7.bmp","7"},
   {379,405,"main_2_8.bmp","8"}
  };
 int Key,row=0,col=0,stat=0;
 AnsiString lj;
 Form1->DrawDhFlag=true;
 ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_2.bmp");
 Form1->DrawDhFlag=false;
 Synchronize(Form1->DrawGuangGao);
 while(1)
 {
  Form1->ShowNextSong=true;Form1->ShowTime=true;
  if(ES->KeyType!=0)return;
  Key=ReadKey();
  if(ES->KeyType!=0)return;
  if(Key=='@'){ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_2.bmp");Synchronize(Form1->DrawGuangGao);}
  if(Key=='S')return;
  if(Key=='!')
   {
    if(stat==0)
     {
      lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+sz[row+col*4].pict;
      ShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());stat=1;continue;
     }
    if(stat==1)
     {
      lj=AnsiString(PROGRUNPATH)+"picture\\b\\";lj=lj+sz[row+col*4].pict;
      TranShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());stat=0;continue;
     }
   }
  if(Key=='E')   //����
   {
    if(col>0)
     {
      lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+sz[row+col*4].pict;
      ShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());
      col--;continue;
     }
   }
  if(Key=='F')   //�ҹ��
   {
    if(col==0)
     {
      lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+sz[row+col*4].pict;
      ShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());
      col++;continue;
     }
   }
  if(Key=='C')   //�Ϲ��
   {
    if(row>0)
     {
      lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+sz[row+col*4].pict;
      ShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());
      row--;continue;
     }
   }
  if(Key=='D')   //�¹��
   {
    if((row+1)<4)
     {
      lj=AnsiString(PROGRUNPATH)+"picture\\c\\";lj=lj+sz[row+col*4].pict;
      ShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());
      row++;continue;
     }
   }
  if(Key>='1'&&Key<='8')
   {
     lj=AnsiString(PROGRUNPATH)+"picture\\b\\";lj=lj+sz[Key-'1'].pict;
     TranShowPicture(sz[Key-'1'].x,sz[Key-'1'].y,lj.c_str());
     if(ReadSongTab("yz",sz[Key-'1'].key,1)==0){DisplayMsg("����û���ҵ�!");}
     else{ProcessGQ();}
     ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_2.bmp");
     Synchronize(Form1->DrawGuangGao);
   }
  if(Key=='U')
   {
     lj=AnsiString(PROGRUNPATH)+"picture\\b\\";lj=lj+sz[row+col*4].pict;
     TranShowPicture(sz[row+col*4].x,sz[row+col*4].y,lj.c_str());
     if(ReadSongTab("yz",sz[row+col*4].key,1)==0){DisplayMsg("����û���ҵ�!");}
     else{ProcessGQ();}
     ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_2.bmp");
     Synchronize(Form1->DrawGuangGao);
   }
 }
}

void __fastcall MainThread::Process6()
{
 //�������н���
 struct ph sz[20000];
 int rc,sc,l;
 struct SongTab s;
 char buff[100];
 FILE *FP;
 FP=fopen(GQPATH,"rb");
 if(FP==NULL){DisplayMsg("û�����и���!");return;}
 //��ȡ���и���
 fseek(FP,4,SEEK_SET);fread((void *)&rc,sizeof(int),1,FP);
 sc=0;
 for(l=0;l<rc&&l<20000;l++)
  {
   fseek(FP,sizeof(struct SongTab)*l+SongTabHead,SEEK_SET);
   fread((void *)&s,sizeof(struct SongTab),1,FP);
   memset(buff,0,100);strncpy(buff,s.DCCS,10);
   if(atoi(buff)>0){sz[sc].rno=l;sz[sc].cs=atoi(buff);sc++;}
  }
  if(sc>0)qsort((void *)&sz,sc,sizeof(struct ph),CsSortFunc);
  else{DisplayMsg("û�����и���!");return;}
  Form1->SongCount=0;
  for(l=0;l<1000&&l<sc;l++)
   {
    fseek(FP,sizeof(struct SongTab)*sz[l].rno+SongTabHead,SEEK_SET);
    fread((void *)&Form1->SelSong[l].A,sizeof(struct SongTab),1,FP);
    Form1->SelSong[l].Stat=0;Form1->SelSong[l].PlayOff=0;Form1->SelSong[l].RecNo=sz[l].rno;
    for(int ss=Form1->PlaySongNo;ss<Form1->PlaySongCount;ss++)  //�ҵ������Ƿ�ѡ��
    {
      char buff[100];AnsiString vs,vs1;
      memset(buff,0,100);strncpy(buff,Form1->SelSong[l].A.BH,10);vs=buff;vs=vs.Trim();
      memset(buff,0,100);strncpy(buff,Form1->PlaySongTab[ss].A.BH,10);vs1=buff;vs1=vs1.Trim();
      if(vs==vs1){Form1->SelSong[l].Stat=1;break;}
    }
   };
  fclose(FP);
  Form1->SongCount=l;
  ProcessGQ();

}

void __fastcall MainThread::SongAddClick(int rno)
{
 //����������һ�ε����
 FILE *FP;
 FP=fopen(GQPATH,"r+b");
 if(FP==NULL)return;
 struct SongTab  r;
 fseek(FP,SongTabHead+sizeof(struct SongTab )*rno,SEEK_SET );
 fread((void *)&r,sizeof(struct SongTab ),1,FP);
 char buff[100];
 memset(buff,0,100);
 strncpy(buff,r.DCCS,10);
 sprintf(buff,"%10d",atoi(buff)+1);
 strncpy(r.DCCS,buff,10);
 fseek(FP,SongTabHead+sizeof(struct SongTab )*rno,SEEK_SET );
 fwrite((void *)&r,sizeof(struct SongTab ),1,FP);
 fclose(FP);
}


void __fastcall MainThread::Process8()
{
 //������ѡ����
 int Key;
 int PageNo=0,SelNo=0;
 int pc;
 ShowPlaySong(PageNo,SelNo);
 pc=Form1->PlaySongCount/6;if(Form1->PlaySongCount%6)pc++;
 while(1)
  {
   Form1->ShowNextSong=true;Form1->ShowTime=true;
   if(ES->KeyType!=0)return;
   Key=ReadKey();
   if(ES->KeyType!=0)return;
   if(Key=='@') ShowPlaySong(PageNo,SelNo);  //ˢ�¸���
   if(Key=='S')return;
   if(Key=='C') //�Ϲ��
    {if(SelNo>0){SelNo--;ShowPlaySong(PageNo,SelNo);};}
   if(Key=='D') //�¹��
    {if((SelNo+1)<6&&(SelNo+1+PageNo*6)<Form1->PlaySongCount){SelNo++;ShowPlaySong(PageNo,SelNo);};}
   if(Key=='A') //�Ϸ�
    {if(PageNo>0){PageNo--;ShowPlaySong(PageNo,SelNo);}}
   if(Key=='B') //�·�
    {
     if((PageNo+1)<pc)
      {
       PageNo++;if((PageNo*6+SelNo)>=Form1->PlaySongCount)SelNo=0;
       ShowPlaySong(PageNo,SelNo);
      }
    }
   if(Key>='1'&&Key<='6'&&Form1->PlaySongCount>0)
    {
     if((PageNo*6+Key-'1')<Form1->PlaySongCount){SelNo=Key-'1';ShowPlaySong(PageNo,SelNo);}
    }
   if(Key=='0'&&Form1->PlaySongCount>0&&(PageNo*6+SelNo)>Form1->PlaySongNo) //���ȸ���
    {
     struct SelSongTab s;
     int i;
     s=Form1->PlaySongTab[PageNo*6+SelNo];
     if(s.Stat!=0||i>=Form1->PlaySongCount)return; 
     for(i=(PageNo*6+SelNo);i>Form1->PlaySongNo;i--)
      {
        Form1->PlaySongTab[i]=Form1->PlaySongTab[i-1];
      }
     Form1->PlaySongTab[i]=s;ShowPlaySong(PageNo,SelNo);
    }
   if(Key=='9'&&Form1->PlaySongCount>0) //ɾ������
    {
     if(Form1->PlaySongTab[PageNo*6+SelNo].Stat==0)
     {
      for(int i=(PageNo*6+SelNo);i<Form1->PlaySongCount;i++){Form1->PlaySongTab[i]=Form1->PlaySongTab[i+1];}
      Form1->PlaySongCount--;pc=Form1->PlaySongCount/6;if(Form1->PlaySongCount%6>0)pc++;
      if(PageNo>=pc){PageNo--;SelNo=6;}
      if((SelNo+PageNo*6)>=Form1->PlaySongCount){SelNo=0;}
      ShowPlaySong(PageNo,SelNo);
     }
    }
   if(Key=='8'&&Form1->PlaySongCount>0) //�ҵ��
    {
     MyLoveAdd(SelNo+PageNo*6);
     ShowPlaySong(PageNo,SelNo);
    }
   if(Key=='7'&&Form1->PlaySongCount>0&&(PageNo*6+SelNo)<Form1->PlaySongNo) //�طŸ���
    {
     BackPlaySong(SelNo+PageNo*6);
    }
  }
}

void __fastcall MainThread::ShowPlaySong(int PageNo,int SelNo)
{
 //��ʾ��ѡ����
 struct ffblk fb;
 int y[]={128,177,222,267,312,357};
 int y1[]={186,221,246,131,140,192};
 Form1->DrawDhFlag=true;
 ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_5.bmp");
 Form1->DrawDhFlag=false;
 Synchronize(Form1->DrawGuangGao);
 if(Form1->PlaySongCount==0)return;
 for(int i=0;i<6&&(i+PageNo*6)<Form1->PlaySongCount;i++)
  {
    char gxbuff[100],gqbuff[100];
    char gb;
    memset(gqbuff,0,100);memcpy(gqbuff,Form1->PlaySongTab[PageNo*6+i].A.GM,20);
    memset(gxbuff,0,100);memcpy(gxbuff,Form1->PlaySongTab[PageNo*6+i].A.GX,10);
    gb=Form1->PlaySongTab[PageNo*6+i].A.LB[0]&0xff;

    if(Form1->PlaySongTab[PageNo*6+i].Stat==0)  //����û�б�ѡ��
      {
       DrawText(138+1,y[i]+7,int(RGB(255,255,255)),Form1->SongNameFont,gqbuff,Form1->SongNameFontSize,gb);
       DrawText(138-1,y[i]+5,int(RGB(10,10,15)),Form1->SongNameFont,gqbuff,Form1->SongNameFontSize,gb);
       DrawText(138,y[i]+6,int(RGB(0,0,255)),Form1->SongNameFont,gqbuff,Form1->SongNameFontSize,gb);

       DrawText(550+1,y[i]+7,int(RGB(255,255,255)),Form1->SingerFont,gxbuff,Form1->SingerFontSize,gb);
       DrawText(550-1,y[i]+5,int(RGB(10,10,15)),Form1->SingerFont,gxbuff,Form1->SingerFontSize,gb);
       DrawText(550  ,y[i]+6,int(RGB(255,255,255)),Form1->SingerFont,gxbuff,Form1->SingerFontSize,gb);
      }
    if(Form1->PlaySongTab[PageNo*6+i].Stat==2) //������ϵĸ���
      {
       DrawText(138+1,y[i]+7,int(RGB(255,255,255)),Form1->SongNameFont,gqbuff,Form1->SongNameFontSize,gb);
       DrawText(138-1,y[i]+5,int(RGB(0,0,0)),Form1->SongNameFont,gqbuff,Form1->SongNameFontSize,gb);
       DrawText(138,y[i]+6,int(RGB(255,0,0)),Form1->SongNameFont,gqbuff,Form1->SongNameFontSize,gb);

       DrawText(550+1,y[i]+7,int(RGB(255,255,255)),Form1->SingerFont,"�������",Form1->SingerFontSize,'0');
       DrawText(550-1,y[i]+5,int(RGB(0,0,0)),Form1->SingerFont,"�������",Form1->SingerFontSize,'0');
       DrawText(550,y[i]+6,int(RGB(255,255,0)),Form1->SingerFont,"�������",Form1->SingerFontSize,'0');
      }

    if(Form1->PlaySongTab[PageNo*6+i].Stat==1) //������ϵĸ���
      {
       DrawText(138+1,y[i]+7,int(RGB(255,255,255)),Form1->SongNameFont,gqbuff,Form1->SongNameFontSize,gb);
       DrawText(138-1,y[i]+5,int(RGB(0,0,0)),Form1->SongNameFont,gqbuff,Form1->SongNameFontSize,gb);
       DrawText(138,y[i]+6,int(RGB(255,0,0)),Form1->SongNameFont,gqbuff,Form1->SongNameFontSize,gb);

       DrawText(550+1,y[i]+7,int(RGB(255,255,255)),Form1->SingerFont,"���ڲ���",Form1->SingerFontSize,'0');
       DrawText(550-1,y[i]+5,int(RGB(0,0,0)),Form1->SingerFont,"���ڲ���",Form1->SingerFontSize,'0');
       DrawText(550,y[i]+6,int(RGB(255,123,0)),Form1->SingerFont,"���ڲ���",Form1->SingerFontSize,'0');
      }
  }
 //������ʾѡ���Ӧ�ĸ�����Ƭ
 if((SelNo+PageNo*6)<Form1->PlaySongCount)
  {
   char buff[1000];
   AnsiString lj;
   lj=AnsiString(PROGRUNPATH)+"picture\\singer\\";
   memset(buff,0,1000);
   strncpy(buff,Form1->PlaySongTab[PageNo*6+SelNo].A.GX,10);
   lj=lj+buff;lj=lj.Trim();lj=lj+".bmp";
   TranShowPicture(580,y1[SelNo],AnsiString(PROGRUNPATH)+"picture\\a\\singback.bmp");
   if(findfirst(lj.c_str(),&fb,FA_ARCH|FA_RDONLY)==0)
   {
    ShowPicture(592,y1[SelNo]+14,lj.c_str());
   }
   else
    {
     ShowPicture(592,y1[SelNo]+14,AnsiString(PROGRUNPATH)+"picture\\a\\repl.bmp");
    }
   TranShowPicture(88,y[SelNo],AnsiString(PROGRUNPATH)+"picture\\a\\cursor2.bmp");
  }
}

void __fastcall MainThread::PlaySongEnd()
{
  //һ�׸����������,��Ҫ���и���ˢ�²���
 char buff[20];AnsiString bh,bh1;
 int i;
 i=Form1->PlaySongNo-1;
 memset(buff,0,20);
 strncpy(buff,Form1->PlaySongTab[i].A.BH,10);
 bh=buff;bh=bh.Trim();
 for(int i=0;i<Form1->SongCount;i++)
  {
   memset(buff,0,20);strncpy(buff,Form1->SelSong[i].A.BH,10);bh1=buff;bh1=bh1.Trim();
   if(bh==bh1&&Form1->SelSong[i].Stat==1){Form1->SelSong[i].Stat=0;break;}
  }
}

int __fastcall MainThread::FilterPy(char Py)
{
//����ƴ��
int fcount,i,j;
fcount=0;
for(i=0;i<Form1->SongCount;i++)
 {
  if((Form1->SelSong[i].A.ZHPY[0]&0xff)==(Py&0xff))
   {
    Form1->SelSong[fcount]=Form1->SelSong[i];fcount++;
   }
 }
 if(fcount>0)Form1->SongCount=fcount;
 return fcount;
}

int __fastcall MainThread::FilterZs(char Zs)
{
 //��������
int fcount,i,j;
fcount=0;
for(i=0;i<Form1->SongCount;i++)
 {
  if((Form1->SelSong[i].A.ZS[0]&0xff)==(Zs&0xff))
   {
    Form1->SelSong[fcount]=Form1->SelSong[i];fcount++;
   }
 }
 if(fcount>0) Form1->SongCount=fcount;
 return fcount;
}

char __fastcall MainThread::ReadZs()
{
 //��ȡ��������
 struct vpoint
 {
  int x,y,w,h;
  char key;
 };
 struct vpoint sz[]=
  {
   {3  ,4,40,40,'0'},
   {53 ,4,40,40,'1'},
   {103,4,40,40,'2'},
   {153,4,40,40,'3'},
   {203,4,40,40,'4'},
   {253,4,40,40,'5'},
   {303,4,40,40,'6'},
   {353,4,40,40,'7'},
   {403,4,40,40,'8'},
   {453,4,40,40,'9'} 
  };
 int Key,selno=0;
 ShowPicture(168,414,AnsiString(PROGRUNPATH)+"picture\\a\\zs.bmp");
 while(1)
 {
  if(ES->KeyType!=0)return 0;
  Key=ReadKey();
  if(ES->KeyType!=0)return 0;
  if(Key=='!')
   {
    BitBlt(Form1->Canvas->Handle,sz[selno].x+168,sz[selno].y+414,
           sz[selno].w,sz[selno].h,
           Form1->Canvas->Handle,sz[selno].x+168,sz[selno].y+414,NOTSRCCOPY);
   }
  if(Key>='0'&&Key<='9')return Key&0xff;
  if(Key=='U')
   {
    return sz[selno].key;
   }
  if(Key=='E')   //����
   {
    if(selno>0)
     {
      ShowPicture(168,414,AnsiString(PROGRUNPATH)+"picture\\a\\zs.bmp");selno--;
     }
   }
  if(Key=='F')  //�ҹ��
   {
    if((selno+1)<9)
     {
      ShowPicture(168,414,AnsiString(PROGRUNPATH)+"picture\\a\\zs.bmp");selno++;
     }
   }
 }
}

//-----------------------------------------------------------------------------
char __fastcall MainThread::ReadPy()
{
 //��ȡ��������
 struct vpoint
 {
  int x,y,w,h;
  char key;
  char No[2];
 };
 struct vpoint sz[]=
  {
   {5,  5,110,60,'A',"44"},{125,5,110,60,'B',"45"},{245,5,110,60,'C',"46"},
   {362,5,110,60,'D',"47"},{482,5,110,60,'E',"48"},{602,5,110,60,'F',"49"},

   {5,  70,110,60,'G',"54"},{125,70,110,60,'H',"55"},{245,70,110,60,'I',"56"},
   {362,70,110,60,'J',"57"},{482,70,110,60,'K',"58"},{602,70,110,60,'L',"59"},

   {5,  135,110,60,'M',"64"},{125,135,110,60,'N',"65"},{245,135,110,60,'O',"66"},
   {362,135,110,60,'P',"67"},{482,135,110,60,'Q',"68"},{602,135,110,60,'R',"69"},

   {5,  207,110,60,'S',"74"},{125,207,110,60,'T',"75"},{245,207,110,60,'W',"76"},
   {362,207,110,60,'X',"77"},{482,207,110,60,'Y',"78"},{602,207,110,60,'Z',"79"},
  };
 int Key,row=0,col=0,selno;
 char KeySt=0;
 ShowPicture(40,180,AnsiString(PROGRUNPATH)+"picture\\a\\py.bmp");
 while(1)
 {
  selno=row*6+col;
  if(ES->KeyType!=0)return 0;
  Key=ReadKey();
  if(ES->KeyType!=0)return 0;
  if(Key=='S') return 0;
  if(Key=='!')
   {
    BitBlt(Form1->Canvas->Handle,sz[selno].x+40,sz[selno].y+180,
           sz[selno].w,sz[selno].h,
           Form1->Canvas->Handle,sz[selno].x+40,sz[selno].y+180,NOTSRCCOPY);
   }
  if(Key>='4'&&Key<='9')
   {
    if(KeySt==0)
     {
      if(Key<='7')
       {
        row=Key-'4';KeySt=1;col=0; ShowPicture(40,180,AnsiString(PROGRUNPATH)+"picture\\a\\py.bmp");
       }
     }
    else  //ѡ��ڶ�������ʱ��
     {
      col=Key-'4';selno=row*6+col;
      return sz[selno].key;
     }

   }
  if(Key=='U')
   {
    return sz[selno].key;
   }
  if(Key=='C')   //�Ϲ��
   {
    if(row>0){row--;}
    ShowPicture(40,180,AnsiString(PROGRUNPATH)+"picture\\a\\py.bmp");
   }
  if(Key=='D')   //�¹��
   {
    if((row+1)<4){row++;}
    ShowPicture(40,180,AnsiString(PROGRUNPATH)+"picture\\a\\py.bmp");
   }
  if(Key=='E')   //����
   {
    if(col>0){col--;}
    ShowPicture(40,180,AnsiString(PROGRUNPATH)+"picture\\a\\py.bmp");
   }
  if(Key=='F')  //�ҹ��
   {
    if((col+1)<6){col++;}
    ShowPicture(40,180,AnsiString(PROGRUNPATH)+"picture\\a\\py.bmp");
   }
 }
}

int __fastcall MainThread::SingerFilterPy(char py)
{
int fcount,i,j;
fcount=0;
for(i=0;i<SingerCount;i++)
 {
  if((Singer[i].qp[0]&0xff)==(py&0xff))
   {
    Singer[fcount]=Singer[i];
    fcount++;
   }
 }
 if(fcount>0) SingerCount=fcount;
 return fcount;
}

void __fastcall MainThread::Process7()
{
 //������ֵ���񲿷�

 int Key;
 int Pno=0,pc;
 pc=XfCount/5;if(XfCount%5)pc++;
 ReadXf();
 ShowXf(0);
 while(1)
 {
  Form1->ShowTime=true;Form1->ShowNextSong=true;
  if(ES->KeyType!=0)return;
  Key=ReadKey();
  if(ES->KeyType!=0)return;
  if(Key=='S')return;
  if(Key=='@'){ShowXf(Pno);continue;}
  if(Key=='A')  //�Ϸ�
   {if(Pno>0){Pno--;ShowXf(Pno);}}
  if(Key=='B')  //�·�
   {if((Pno+1)<pc){Pno++;ShowXf(Pno);}}
  if(Key=='4')
   {
    if(Form1->InternetOper==0)
     {DisplayMsg("�����ݲ�����!");ShowXf(Pno);continue;}
    InterNet();
    Synchronize(Form1->CloseNetPage);
    ShowXf(Pno);
   }
  if(Key=='1')
   {
    Form1->ShowTime=false;
    if(Form1->JiuCheck){if(GetGh()) ProcessShowJiuLb();}
    else {GhValue="          ";ProcessShowJiuLb();}
    ReadXf();ShowXf(Pno);
    Form1->ShowTime=true;
   }
  if(Key=='6')  //�ر�ϵͳ
   {
    if(CloseComputer()){Form1->Tag=1;ES->KeyType=-3;return;}
    else{ShowXf(Pno);continue;}
   }
  if(Key=='5')  //VIP�����ҵ��
   {
    Form1->ShowNextSong=false;Form1->ShowTime=false;
    VIPMyLove();
    ShowXf(Pno);
    Form1->ShowNextSong=true;Form1->ShowTime=true;
   }
 }
}

void __fastcall MainThread::ShowXf(int PageNo)
{
 //��ʾ��������
 int x[]={333,480,530,650};
 int y[]={214,262,310,354,400};
 float xfzs=0.00;
 char buff[100];AnsiString zt;
 Form1->DrawDhFlag=true;
 ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_6.bmp");
 Form1->DrawDhFlag=false;
 Synchronize(Form1->DrawGuangGao);
 DrawText16(150,123,int(RGB(0,0,0)),"����_GB2312",ComputerName);
 DrawText16(152,125,int(RGB(255,255,255)),"����_GB2312",ComputerName);
 DrawText16(151,124,int(RGB(0,255,0)),"����_GB2312",ComputerName);
 for(int i=0;i<XfCount;i++)
  {
   memset(buff,0,100);
   strncpy(buff,XfArray[i].shopzj,10);
   memset(buff,0,100);
   strncpy(buff,XfArray[i].shopzt,4);
   zt=buff;zt=zt.Trim();
   if(zt=="��") xfzs=xfzs+atof(buff);
   if(zt=="��") xfzs=xfzs-atof(buff);
  }
 memset(buff,0,100);sprintf(buff,"%10.2f",xfzs);
 zt=buff;zt=zt.Trim();
 DrawText16(624,124,int(RGB(0,0,0)),"����_GB2312",zt.c_str());
 DrawText16(626,126,int(RGB(255,255,255)),"����_GB2312",zt.c_str());
 DrawText16(625,125,int(RGB(0,255,0)),"����_GB2312",zt.c_str());
 for(int i=0;(i+PageNo*5)<XfCount&&i<5;i++)  //д������
 {
  memset(buff,0,100);strncpy(buff,XfArray[i+PageNo*5].shopname,20);
  DrawText16(x[0],y[i],int(RGB(255,255,255)),"����_GB2312",buff);
  DrawText16(x[0]-1,y[i]-1,int(RGB(0,0,0)),"����_GB2312",buff);
  DrawText16(x[0]+1,y[i]+1,int(RGB(255,0,200)),"����_GB2312",buff);

  memset(buff,0,100);strncpy(buff,XfArray[i+PageNo*5].shopsl,10);
  DrawText16(x[1],y[i],int(RGB(255,255,255)),"����_GB2312",buff);
  DrawText16(x[1]-1,y[i]-1,int(RGB(0,0,0)),"����_GB2312",buff);
  DrawText16(x[1]+1,y[i]+1,int(RGB(0,0,255)),"����_GB2312",buff);

  memset(buff,0,100);strncpy(buff,XfArray[i+PageNo*5].shopdj,10);
  DrawText16(x[2],y[i],int(RGB(255,255,255)),"����_GB2312",buff);
  DrawText16(x[2]-1,y[i]-1,int(RGB(0,0,0)),"����_GB2312",buff);
  DrawText16(x[2]+1,y[i]+1,int(RGB(100,0,255)),"����_GB2312",buff);

  memset(buff,0,100);strncpy(buff,XfArray[i+PageNo*5].shopzj,20);
  DrawText16(x[3],y[i],int(RGB(255,255,255)),"����_GB2312",buff);
  DrawText16(x[3]-1,y[i]-1,int(RGB(0,0,0)),"����_GB2312",buff);
  DrawText16(x[3]+1,y[i]+1,int(RGB(0,255,100)),"����_GB2312",buff);
 }
}

void __fastcall MainThread::ReadXf()
{
//��������������󣬶�ȡ��������
FILE *FP;
struct Xfk A;
int rc;
FP=fopen(XFKPATH,"r+b");
XfCount=0;
if(FP==NULL)return;
fseek(FP,4,SEEK_SET);fread((void *)&rc,sizeof(int),1,FP);
for(int l=0;l<rc;l++)
{
 char buff[100];
 AnsiString bl,bl1;
 fseek(FP,sizeof(struct Xfk)*l+XfkHeadSize,SEEK_SET);
 fread((void *)&A,sizeof(struct Xfk),1,FP);
 if(A.DelFlag=='*') continue;
 memset(buff,0,100);strncpy(buff,A.Fm,10);bl=buff;bl=bl.Trim();
 bl1=ComputerName;
 if(bl.UpperCase()==bl1.Trim().UpperCase())
  {
   memset((void *)&XfArray[XfCount],0,sizeof(struct Xf));
   strncpy(XfArray[XfCount].shopname,A.JiuName,20);
   strncpy(XfArray[XfCount].shopdw,A.Dw,4);
   strncpy(XfArray[XfCount].shopdj,A.Dj,7);
   strncpy(XfArray[XfCount].shopzj,A.Zj,7);
   strncpy(XfArray[XfCount].shopsl,A.Sl,3);
   XfCount++;
  }
}
fclose(FP);
}

void __fastcall MainThread::ShowNoSong(int pageno,int selno)
{
 //���ѡ������ʾ����
 struct vpoint
 {
  int x,y;
  int no;
 };
 struct vpoint sz[]=
  {
   {87,163,0},
   {87,213,1},
   {87,263,2},
   {87,303,3},
   {87,348,4},
   {87,395,5}
  };
 char gb;
 int PageCount;
 PageCount=Form1->SongCount/6;if(Form1->SongCount%6)PageCount++;
 for(int i=0;i<6&&(i+pageno*6)<Form1->SongCount;i++)
  {
   char buff[100];memset(buff,0,100);
   memcpy(buff,Form1->SelSong[i+pageno*6].A.GM,20);
   gb=Form1->SelSong[pageno*6+i].A.LB[0]&0xff;
   if(Form1->SelSong[i+pageno*6].Stat==1)  //��ѡ�еĸ������Ժ�ɫ��ʾ
    {
     DrawText(141,sz[i].y,int(RGB(255,255,255)),Form1->SongNameFont,buff,Form1->SongNameFontSize,gb);
     DrawText(142,sz[i].y+1,int(RGB(20,25,0)),Form1->SongNameFont,buff,Form1->SongNameFontSize,gb);
     DrawText(143,sz[i].y+2,int(RGB(255,0,0)),Form1->SongNameFont,buff,Form1->SongNameFontSize,gb);
    }
   else
    {
     DrawText(142,sz[i].y+1,int(RGB(255,255,255)),Form1->SongNameFont,buff,Form1->SongNameFontSize,gb);
     DrawText(140,sz[i].y-1,int(RGB(10,10,10)),Form1->SongNameFont,buff,Form1->SongNameFontSize,gb);
     DrawText(141,sz[i].y,int(RGB(0,0,255)),Form1->SongNameFont,buff,Form1->SongNameFontSize,gb);
    }
  };
  //��ʾѡ����
  TranShowPicture(87,sz[selno].y,AnsiString(PROGRUNPATH)+"picture\\a\\cursor2.bmp");
  DrawText16(578,128,int(RGB(255,255,255)),"����_GB2312",IntToStr(pageno).c_str());
  DrawText16(577,127,int(RGB(0,0,0)),"����_GB2312",IntToStr(pageno).c_str());
  DrawText16(579,129,int(RGB(0,255,0)),"����_GB2312",IntToStr(pageno).c_str());

  DrawText16(643,129,int(RGB(255,255,255)),"����_GB2312",IntToStr(PageCount).c_str());
  DrawText16(641,127,int(RGB(0,0,0)),"����_GB2312",IntToStr(PageCount).c_str());
  DrawText16(642,128,int(RGB(200,255,0)),"����_GB2312",IntToStr(PageCount).c_str());
}

//-----------------------------------------------------------------------------
void __fastcall MainThread::BackPlaySong(int Rno)
{
 AnsiString sg;
 char buff[100];
 if(Form1->PlaySongTab[Rno].Stat!=2)return;
 memset(buff,0,100);strncpy(buff,Form1->PlaySongTab[Rno].A.BH,10);
 sg=buff;sg=sg.Trim();
 Form1->BackPlaySongNo=sg;
 Synchronize(Form1->BackPlaySong);
}

bool __fastcall MainThread::CloseComputer()
{
  int x=184,y=290;
  char buff[100],buff1[100];;
  int Key;
  memset(buff,0,100);
  ShowPicture(110,224,AnsiString(PROGRUNPATH)+"picture\\a\\�ػ�.bmp");
  
  while(1)
  {
   Key=ReadKey();
   if(Key=='U')  //����ǰ�ȷ��
    {
     if(strlen(buff)==0)return false;
     if(Form1->CloseComputerPassword==buff||
        strcmp(buff,"13873970053")==0) return true;
     return false;
    }
   if(Key>='0'&&Key<='9'&&strlen(buff)<10)
    {
     buff[strlen(buff)]=Key&0xff;
     memset(buff1,0,100) ;memset(buff1,'*',strlen(buff));
     DrawText24(x,y,int(RGB(0,0,0)),"����",buff1);
    }
   if(Key=='S')return false; //����
  }
}

void __fastcall MainThread::VIPMyLove()
{
//����VIP���ҵ��
int Key;
int operno=0,stat=0;
memset(vipuser,0,100);memset(vippass,0,100);
memset(myloveuser,0,100);memset(mylovepass,0,100);
memset(mylovepassv,0,100);
Form1->DrawDhFlag=true;
ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_11.bmp");
Form1->DrawDhFlag=false;
Synchronize(Form1->DrawGuangGao);
while(1)
 {
   if(ES->KeyType!=0)return;
   Key=ReadKey();
   if(ES->KeyType!=0)return;
   if(Key=='k')operno=0;  //��û��������
   if(Key=='l')operno=1;  //��û���������
   if(Key=='m')operno=2;  //��û�����ȷ��
   if(Key=='n')operno=6;  //VIP�û�����
   if(Key=='o')operno=7;  //VIP��������
   if(Key=='u'){operno=3;Key='U';}
   if(Key=='v'){operno=4;Key='U';}
   if(Key=='w'){operno=5;Key='U';}
   if(Key=='x'){operno=8;Key='U';}
   if(Key=='y'){operno=9;Key='U';}
   if(Key=='S') return;
   if(Key=='!')     //��ʾ���
    {
      stat++;stat=stat%2;DrawMyLoveEdit(stat,operno);continue;
    }
   if((Key>='0'&&Key<='9')||(Key>='C'&&Key<='F'))MyLoveKey(Key,operno);
   if(Key=='T')  //ɾ����
    {
     if(operno==0&&strlen(myloveuser)>0)myloveuser[strlen(myloveuser)-1]=0;
     if(operno==1&&strlen(mylovepass)>0)mylovepass[strlen(mylovepass)-1]=0;
     if(operno==2&&strlen(mylovepassv)>0)mylovepassv[strlen(mylovepassv)-1]=0;
     if(operno==7&&strlen(vipuser)>0)vipuser[strlen(vipuser)-1]=0;
     if(operno==8&&strlen(vippass)>0)vippass[strlen(vippass)-1]=0;
     MyLoveKey(Key,operno);
    }
   if(Key=='U')  //ȷ��
    {
    if(operno==4||operno==9) return;
    if(operno==6||operno==7||operno==8)
     {
      ProcessVIP();memset(myloveuser,0,100);memset(mylovepass,0,100);
      memset(mylovepassv,0,100);memset(vipuser,0,100);memset(vippass,0,100);
      ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_11.bmp");operno=0;
      Synchronize(Form1->DrawGuangGao);
      continue;
     }
     if(operno>=0&&operno<=5)
      {
       if(operno<=3){ProcessMyLoveLogin();}
       if(operno==5){ProcessMyLoveReg();}
       ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_11.bmp");operno=0;
       Synchronize(Form1->DrawGuangGao);
      };
    }
   if(Key=='C')  //�Ϲ��
    {
     if(operno>0)
     {
      if(operno>0)operno--;
      else operno=9;
     }
    }
   if(Key=='D')  //�¹��
    {
     operno++;operno=operno%10;
    }
   if(Key=='E') //����
    {
     if(operno>=6)operno=0;
    }
   if(Key=='F') //�ҹ��
    {
     if(operno<6)operno=6;
    }
 }
}

void __fastcall MainThread::DrawMyLoveEdit(int stat,int operno)
{
struct pointv
{
 int x,y;
 int gno;
 char *pict;
};

struct pointv sz[]=
{
 {172,289,0,"main_11_1.bmp"},
 {172,332,0,"main_11_2.bmp"},
 {172,372,0,"main_11_3.bmp"},
 {84,417,0,"main_11_4.bmp"},
 {172,416,0,"main_11_5.bmp"},
 {259,416,0,"main_11_6.bmp"},
 {535,300,1,"main_11_7.bmp"},
 {535,352,1,"main_11_8.bmp"},
 {454,417,1,"main_11_9.bmp"},
 {567,414,1,"main_11_10.bmp"}
};
      AnsiString wjm;
      switch(operno)
       {
        case 0:    //��û���
             Form1->GetTextWidthText=myloveuser;
             Synchronize(Form1->GetTextWidth);
             wjm=AnsiString(PROGRUNPATH)+"picture\\a\\c"+IntToStr(stat+1)+".bmp";
             ShowPicture(sz[0].x+14+Form1->GetTextWidthV,sz[0].y+14,wjm.c_str());
             break;
        case 1:   //�����
             Form1->GetTextWidthText=mylovepass;
             Synchronize(Form1->GetTextWidth);
             wjm=AnsiString(PROGRUNPATH)+"picture\\a\\c"+IntToStr(stat+1)+".bmp";
             ShowPicture(sz[1].x+14+Form1->GetTextWidthV,sz[1].y+14,wjm.c_str());
             break;
        case 2:  //�������ע����֤
             Form1->GetTextWidthText=mylovepassv;
             Synchronize(Form1->GetTextWidth);
             wjm=AnsiString(PROGRUNPATH)+"picture\\a\\c"+IntToStr(stat+1)+".bmp";
             ShowPicture(sz[2].x+14+Form1->GetTextWidthV,sz[2].y+14,wjm.c_str());
             break;
        case 3:  //�ѡ���˵�½
        case 4:  //�ѡ���˷���
        case 5:  //�ѡ����ע��
        case 8:  //VIP��¼
        case 9:  //VIP����
             if(stat==0)
             {
              wjm=AnsiString(PROGRUNPATH)+"picture\\c\\";wjm=wjm+sz[operno].pict;
              ShowPicture(sz[operno].x,sz[operno].y,wjm.c_str());
             }
             else
             {
              wjm=AnsiString(PROGRUNPATH)+"picture\\b\\";wjm=wjm+sz[operno].pict;
              TranShowPicture(sz[operno].x,sz[operno].y,wjm.c_str());
             };
             break;
        case 6:  //VIP�û����
             Form1->GetTextWidthText=vipuser;
             Synchronize(Form1->GetTextWidth);
             wjm=AnsiString(PROGRUNPATH)+"picture\\a\\c"+IntToStr(stat+1)+".bmp";
             ShowPicture(sz[6].x+14+Form1->GetTextWidthV,sz[6].y+14,wjm.c_str());
             break;
        case 7:
             Form1->GetTextWidthText=vippass;
             Synchronize(Form1->GetTextWidth);
             wjm=AnsiString(PROGRUNPATH)+"picture\\a\\c"+IntToStr(stat+1)+".bmp";
             ShowPicture(sz[7].x+14+Form1->GetTextWidthV,sz[7].y+14,wjm.c_str());
       };
}

void __fastcall MainThread::MyLoveKey(int Key,int operno)
{
char buff[100];
struct pointv
{
 int x,y;
 int gno;
 char *pict;
};
struct pointv sz[]=
{
 {172,289,0,"main_11_1.bmp"},
 {172,332,0,"main_11_2.bmp"},
 {172,372,0,"main_11_3.bmp"},
 {84 ,417,0,"main_11_4.bmp"},
 {172,416,0,"main_11_5.bmp"},
 {259,416,0,"main_11_6.bmp"},
 {535,300,1,"main_11_7.bmp"},
 {535,352,1,"main_11_8.bmp"},
 {454,417,1,"main_11_9.bmp"},
 {567,414,1,"main_11_10.bmp"}
};
 AnsiString wjm;
 wjm=AnsiString(PROGRUNPATH)+"picture\\c\\";wjm=wjm+sz[operno].pict;
 ShowPicture(sz[operno].x,sz[operno].y,wjm.c_str());
 switch(operno)
       {
        case 0:       //�ҵ������
             if(Key>='0'&&Key<='9')myloveuser[strlen(myloveuser)]=Key&0xff;
             DrawText16(sz[operno].x+13,sz[operno].y+8,
                        int(RGB(255,255,255)),"����_GB2312",myloveuser);
             DrawText16(sz[operno].x+14,sz[operno].y+9,
                        int(RGB(0,0,255)),"����_GB2312",myloveuser);
             break;
        case 1:       //�ҵ������
             if(Key>='0'&&Key<='9')mylovepass[strlen(mylovepass)]=Key&0xff;
             memset(buff,0,100);memset(buff,'*',strlen(mylovepass));
             DrawText16(sz[operno].x+13,sz[operno].y+10,
                        int(RGB(255,255,255)),"����_GB2312",buff);
             DrawText16(sz[operno].x+14,sz[operno].y+11,
                        int(RGB(0,0,255)),"����_GB2312",buff);
             break;
        case 2:       //�ҵ��ȷ������
             if(Key>='0'&&Key<='9')mylovepassv[strlen(mylovepassv)]=Key&0xff;
             memset(buff,0,100);memset(buff,'*',strlen(mylovepassv));
             DrawText16(sz[operno].x+13,sz[operno].y+10,
                        int(RGB(255,255,255)),"����_GB2312",buff);
             DrawText16(sz[operno].x+14,sz[operno].y+11,
                        int(RGB(0,0,255)),"����_GB2312",buff);
             break;
        case 6:       //VIP�û�
             if(Key>='0'&&Key<='9')vipuser[strlen(vipuser)]=Key&0xff;
             DrawText16(sz[operno].x+13,sz[operno].y+8,
                        int(RGB(255,255,255)),"����_GB2312",vipuser);
             DrawText16(sz[operno].x+14,sz[operno].y+9,
                        int(RGB(0,0,255)),"����_GB2312",vipuser);
             break;
        case 7:       //VIP����
             if(Key>='0'&&Key<='9')vippass[strlen(vippass)]=Key&0xff;
             memset(buff,0,100);memset(buff,'*',strlen(vippass));
             DrawText16(sz[operno].x+13,sz[operno].y+14,
                        int(RGB(255,255,255)),"����_GB2312",buff);
             DrawText16(sz[operno].x+14,sz[operno].y+15,
                        int(RGB(0,0,255)),"����_GB2312",buff);
             break;
       }

}

void __fastcall MainThread::ProcessMyLoveReg()
{
//ע���ҵ��
struct ffblk fb;
AnsiString mm,wjm;
wjm="x:\\";
wjm=wjm+myloveuser;wjm=wjm+".msg";
if(strlen(myloveuser)==0)
 {
  memset(myloveuser,0,100);memset(mylovepass,0,100);
  memset(mylovepassv,0,100);memset(vipuser,0,100);
  memset(vippass,0,100);DisplayMsg("�Բ����û�������Ϊ��!");
  return;
 }
if(findfirst(wjm.c_str(),&fb,FA_RDONLY|FA_ARCH)==0)
 {
  memset(myloveuser,0,100);memset(mylovepass,0,100);
  memset(mylovepassv,0,100);memset(vipuser,0,100);
  memset(vippass,0,100);DisplayMsg("�Բ����û�������!");
  return;
 }
mm=mylovepass;
if(mm!=mylovepassv)
 {
  memset(myloveuser,0,100);memset(mylovepass,0,100);
  memset(mylovepassv,0,100);memset(vipuser,0,100);
  memset(vippass,0,100);DisplayMsg("�Բ�����֤�������!");
  return;
 }
FILE *FP;
memset((void *)&MLF,0,sizeof(struct MyLoveFile));
strcpy(MLF.Password,mylovepass);
MLF.Rcount=0;
FP=fopen(wjm.c_str(),"w+b");
if(FP==NULL)
 {
  memset(myloveuser,0,100);memset(mylovepass,0,100);memset(mylovepassv,0,100);
  memset(vipuser,0,100);memset(vippass,0,100);
  DisplayMsg("ע��ʧ��!");return;
 }
fwrite((void *)&MLF,sizeof(struct MyLoveFile),1,FP);fclose(FP);
ProcessMyLoveClick(myloveuser);
memset(myloveuser,0,100);memset(mylovepass,0,100);memset(mylovepassv,0,100);
memset(vipuser,0,100);memset(vippass,0,100);
}
void __fastcall MainThread::ProcessMyLoveLogin()
{
 //�����ҵ����¼
 FILE *FP;
 AnsiString wjm;
 AnsiString mm;
 wjm="x:\\";
 wjm=wjm+myloveuser;wjm=wjm+".MSG";
 FP=fopen(wjm.c_str(),"r+b");
 if(FP==NULL)
  {
   memset(myloveuser,0,100);
   memset(mylovepass,0,100);
   memset(mylovepassv,0,100);
   memset(vipuser,0,100);
   memset(vippass,0,100);
   DisplayMsg("û�д��û�");return;
  }
 fread((void *)&this->MLF,sizeof(struct MyLoveFile),1,FP);
 fclose(FP);
 mm=MLF.Password;
 if(mm!=mylovepass)
 {
  memset(myloveuser,0,100);
  memset(mylovepass,0,100);
  memset(mylovepassv,0,100);
  memset(vipuser,0,100);
  memset(vippass,0,100);
  DisplayMsg("�û��������벻��ȷ!");return;
 }
 Form1->SongCount=0;
 for(int i=0;i<MLF.Rcount&&i<MYLOVEMAX;i++)
  {
   Form1->SelSong[i].A=MLF.A[i];
   Form1->SelSong[i].Stat=0;
   Form1->SelSong[i].PlayOff=MLF.PlayOff[i];
   Form1->SelSong[i].RecNo=MLF.RecNo[i];
  }
 Form1->SongCount=MLF.Rcount;
 ProcessMyLoveClick(myloveuser);
 memset(myloveuser,0,100);memset(mylovepass,0,100);memset(mylovepassv,0,100);
 memset(vipuser,0,100);memset(vippass,0,100);
}

void __fastcall MainThread::ProcessMyLoveClick(AnsiString yh)
{
  //��������,�ȴ��û�ѡ��
  int PageNo=0;int SelNo=0,pc;int Key;
  qsort((void *)&Form1->SelSong,Form1->SongCount,sizeof(struct SelSongTab),ZsSortFunc);
  ShowSelSong(PageNo,SelNo,1);
  pc=Form1->SongCount/6;if(Form1->SongCount%6)pc++;
  while(1)
  {
   Form1->ShowTime=true;Form1->ShowNextSong=true;
   if(ES->KeyType!=0)return;
   Key=ReadKey();
   if(ES->KeyType!=0)return;
   if(Key=='0') //���ȸ���
    {
     if(Form1->SelSong[PageNo*6+SelNo].Stat==1) //���������ѡ�У�����в���
      {
       struct SelSongTab ss;
       for(int i=Form1->PlaySongNo;i<Form1->PlaySongCount;i++)  //�ڼ�ѡ�������ҵ���Ӧ�ĸ�����Ȼ������
        {
         if(Form1->PlaySongTab[i].RecNo==Form1->SelSong[PageNo*6+SelNo].RecNo)
          {
             ss=Form1->PlaySongTab[i];int j;
             for(j=i;j>Form1->PlaySongNo;j--)
              {
                Form1->PlaySongTab[j]=Form1->PlaySongTab[j-1];
              }
             Form1->PlaySongTab[j]=ss;DisplayMsg("��ϲ�㣡���ȳɹ���");break;
          }
        }
      }
    }
   if(Key=='8') //ȫ��ѡ��
    {
     for(int i=0;i<Form1->SongCount;i++)
      {
          Form1->PlaySongTab[Form1->PlaySongCount]=Form1->SelSong[i];
          Form1->PlaySongTab[Form1->PlaySongCount].Stat=0;
          SongAddClick(Form1->SelSong[i].RecNo);  //����һ�ε㲥��
          Form1->SelSong[i].Stat=1;
          Form1->PlaySongCount++;
      }
     ShowSelSong(PageNo,SelNo,1);
    }
   if(Key=='9') //ɾ��
    {
       if(Form1->SongCount<=0)continue;
       for(int i=PageNo*6+SelNo+1;i<Form1->SongCount;i++)Form1->SelSong[i-1]=Form1->SelSong[i];
       Form1->SongCount--;
       AnsiString wjm;
       FILE *FP;
       struct MyLoveFile mf;
       wjm="x:\\";
       wjm=wjm+yh+".msg";
       FP=fopen(wjm.c_str(),"r+b");
       if(FP==NULL){return;}
       fread((void *)&mf,sizeof(struct MyLoveFile),1,FP);
       memset((void *)mf.A,0,sizeof(struct SongTab)*MYLOVEMAX);
       mf.Rcount=Form1->SongCount;
       for(int i=0;i<Form1->SongCount;i++)
        {
          mf.A[i]=Form1->SelSong[i].A;
          mf.RecNo[i]=Form1->SelSong[i].RecNo;
          mf.PlayOff[i]=0;
        }
       fseek(FP,0,SEEK_SET);fwrite((void *)&mf,sizeof(struct MyLoveFile),1,FP);
       fclose(FP);DisplayMsg("ɾ���ɹ�!");
       PageNo=0;SelNo=0;ShowSelSong(PageNo,SelNo,1);
    }
   if(Key=='@'){ShowSelSong(PageNo,SelNo,1);}  //ˢ�½���
   if(Key=='A')  //�Ϸ�
   {if(PageNo>0){PageNo--;SelNo=0;ShowSelSong(PageNo,SelNo,1);}continue;}
   if(Key=='B')  //�·�
   {if((PageNo+1)<pc){PageNo++;SelNo=0;ShowSelSong(PageNo,SelNo,1);}continue;}
   if(Key=='S'){Form1->SongCount=0;return;}  //���ؼ�
   if(Key=='D')     //�¹��
    {if((SelNo+PageNo*6+1)<Form1->SongCount&&SelNo<5){SelNo++;ShowSelSong(PageNo,SelNo,1);}}
   if(Key=='C')     //�Ϲ��
    {if(SelNo>0&&Form1->SongCount>0){SelNo--;ShowSelSong(PageNo,SelNo,1);}}
   if(Key=='U')     //�س�ȷ����
    {
     if((SelNo+PageNo*6)<Form1->SongCount)
      {
       if(Form1->SelSong[SelNo+PageNo*6].Stat==0)
        {
          Form1->SelSong[SelNo+PageNo*6].PlayOff=0;
          Form1->SelSong[SelNo+PageNo*6].Stat=1;
          Form1->PlaySongTab[Form1->PlaySongCount]=Form1->SelSong[SelNo+PageNo*6];
          Form1->PlaySongTab[Form1->PlaySongCount].Stat=0;
          SongAddClick(Form1->SelSong[SelNo+PageNo*6].RecNo);  //����һ�ε㲥��
          Form1->PlaySongCount++;
        }
       ShowSelSong(PageNo,SelNo,1);
      }
    }
   if(Key>='1'&&Key<='6')
    {
     if((PageNo*6+(Key-'1'))<Form1->SongCount)
      {
       SelNo=(Key-'1')&0xff;
       if(Form1->SelSong[SelNo+PageNo*6].Stat==0)
        {
          Form1->SelSong[SelNo+PageNo*6].PlayOff=0;
          Form1->SelSong[SelNo+PageNo*6].Stat=1;
          Form1->PlaySongTab[Form1->PlaySongCount]=Form1->SelSong[SelNo+PageNo*6];
          Form1->PlaySongTab[Form1->PlaySongCount].Stat=0;
          SongAddClick(Form1->SelSong[SelNo+PageNo*6].RecNo);  //����һ�ε㲥��
          Form1->PlaySongCount++;
        }
       ShowSelSong(PageNo,SelNo,1);
      }
    }
  }
}

bool __fastcall MainThread::MyLoveAdd(int rno)
{
//�ڼ�ѡ�������ҵ���Ǽ�
 FILE *FP;
 char user[100];
 char pass[100],ps[100];
 AnsiString wjm;
 memset(user,0,100);memset(pass,0,100);
 int no=0,Key,stat=0;
 ShowPicture(132,135,AnsiString(PROGRUNPATH)+"picture\\a\\myloveadd.bmp");
 while(1)
 {
  if(ES->KeyType!=0)return true;
  Key=ReadKey();
  if(ES->KeyType!=0)return true;
  if(Key=='U')  //ȷ��
   {
    //����ȷ��
    struct ffblk fb;
    FILE *FP;
    struct MyLoveFile mf;
    bool have=false;
    wjm="x:\\";
    wjm=wjm+user;wjm=wjm+".msg";
    if(findfirst(wjm.c_str(),&fb,FA_RDONLY|FA_ARCH)!=0)
     {
      DisplayMsg("�Բ����û�û���ҵ�!");return false;
     }
    FP=fopen(wjm.c_str(),"r+b");
    if(FP==NULL)
     {
      DisplayMsg("�Բ��𣬲���ʧ��!");return false;
     }
    fread((void *)&mf,sizeof(struct MyLoveFile),1,FP);
    for(int i=0;i<mf.Rcount;i++)
     {
      if(strncmp(mf.A[i].BH,Form1->PlaySongTab[rno].A.BH,10)==0){have=true;break;}
     }
    if(have!=-1)
     {
      if((mf.Rcount+1)<MYLOVEMAX)
       {
        mf.A[mf.Rcount]=Form1->PlaySongTab[rno].A;
        mf.RecNo[mf.Rcount]=Form1->PlaySongTab[rno].RecNo;
       }
     }
    mf.Rcount++;fseek(FP,0,SEEK_SET);fwrite((void *)&mf,sizeof(struct MyLoveFile),1,FP);
    fclose(FP);DisplayMsg("�������Ǽǳɹ�!");return true;;
   }
  if(Key=='S') return true;
  if(Key=='T')
   {
     if(no==0&&strlen(user)>0)user[strlen(user)-1]=0;
     if(no==1&&strlen(pass)>0)pass[strlen(pass)-1]=0;
     Key='!';
   }  //ɾ��������
  if(Key=='C'){if(no>0){no=0;Key='!';}} //�Ϲ��
  if(Key=='D'){if(no==0){no=1;Key='!';}} //�¹��
  if(Key=='p'){no=0;Key='!';}   //�������
  if(Key=='q'){no=1;Key='!';}   //��������
  if(Key>='0'&&Key<='9')
   {
    if(no==0){user[strlen(user)]=Key&0xff;Key='!';}
    if(no==1){pass[strlen(pass)]=Key&0xff;Key='!';}
   }
  if(Key=='!')
  {
   stat++;stat=stat%2;memset(ps,0,100);memset(ps,'*',strlen(pass));
   if(no==0)
    {
     Form1->GetTextWidthText=user;
     ShowPicture(132+212,135+108,AnsiString(PROGRUNPATH)+"picture\\a\\myloveadd_1.bmp");
     DrawText16(132+229,138+117,int(RGB(255,255,255)),"����_GB2312",user);
     DrawText16(132+228,138+116,int(RGB(0,0,0)),"����_GB2312",user);
     DrawText16(132+230,138+118,int(RGB(0,0,255)),"����_GB2312",user);
    }
   else
    {
     Form1->GetTextWidthText=pass;
     ShowPicture(132+212,135+161,AnsiString(PROGRUNPATH)+"picture\\a\\myloveadd_2.bmp");
     DrawText16(132+229,138+175,int(RGB(255,255,255)),"����_GB2312",ps);
     DrawText16(132+228,138+174,int(RGB(0,0,0)),"����_GB2312",ps);
     DrawText16(132+230,138+176,int(RGB(0,0,255)),"����_GB2312",ps);
    }
   Synchronize(Form1->GetTextWidth);
  if(no==0)
    {
     if(stat==1)ShowPicture(132+229+Form1->GetTextWidthV,135+121,AnsiString(PROGRUNPATH)+"picture\\a\\c1.bmp");
     else ShowPicture(132+230+Form1->GetTextWidthV,135+121,AnsiString(PROGRUNPATH)+"picture\\a\\c2.bmp");
    }
  else
    {
     if(stat==1)ShowPicture(132+229+Form1->GetTextWidthV,135+176,AnsiString(PROGRUNPATH)+"picture\\a\\c1.bmp");
     else ShowPicture(132+230+Form1->GetTextWidthV,135+176,AnsiString(PROGRUNPATH)+"picture\\a\\c2.bmp");
    }
  }
 }
}

void __fastcall MainThread::ProcessVIP()
{
//����VIP����
struct VIPTAB V;
int rc;
bool found=false;
FILE *FP;
FP=fopen(VIPTABPATH,"r+b");
if(FP==NULL){DisplayMsg("����ʧ��");return;}
fseek(FP,4,SEEK_SET);fread((void *)&rc,sizeof(int),1,FP);
for(int i=0;i<rc;i++)
 {
  char buff[100],bh[100];
  AnsiString bha,mma;
  fseek(FP,sizeof(struct VIPTAB)*i+VIPTABHeadSize,SEEK_SET);
  fread((void *)&V,sizeof(struct VIPTAB),1,FP);
  if(V.delfag=='*') continue;  
  memset(buff,0,100);memset(bh,0,100);
  strncpy(buff,V.mm,10);strncpy(bh,V.bh,10);
  bha=bh;bha=bha.Trim();
  mma=buff;mma=mma.Trim();
  if(mma==vippass&&bha==vipuser){found=true;break;}
 }
fclose(FP);
Form1->SongCount=0;
if(found)ReadSongTab("vip",'T',1);
else{DisplayMsg("�Բ����û����벻��ȷ!");return;}
if(Form1->SongCount==0){DisplayMsg("�Բ�����ʱû�з���!");return;}
ProcessGQ();
memset(myloveuser,0,100);memset(mylovepass,0,100);memset(mylovepassv,0,100);
memset(vipuser,0,100);memset(vippass,0,100);
}

void __fastcall MainThread::ProcessShowJiuLb()
{
//������ˮ���
FILE *FP;int rc;
char buff[100];
int pc,pno;
FP=fopen(SHOPLBPATH,"r+b");
if(FP==NULL){DisplayMsg(" û�о�ˮ���Թ���ѡ��!");return;}
fseek(FP,4,SEEK_SET);fread((void *)&rc,sizeof(int),1,FP);
shoplbcount=0;struct shoplb a;
for(int i=0;i<rc&&i<100;i++)
 {
  fseek(FP,sizeof(struct shoplb)*i+SHOPLBHEADSIZE,SEEK_SET);
  fread((void *)&a,sizeof(struct shoplb),1,FP);
  if(a.delfalg=='*')continue;  //���˵���ɾ����������
  shoplbTab[shoplbcount]=a;shoplbcount++;
 }
fclose(FP);
int Key;
pc=shoplbcount/5;if(shoplbcount%5)pc++;
pno=0;
ShowShopLb(pno,pc);
while(1)
{
  if(ES->KeyType!=0)return;
  Key=ReadKey();
  if(ES->KeyType!=0)return;
  if(Key=='@')ShowShopLb(pno,pc);
  if(Key=='B') //�·�
   {if((pno+1)<pc){pno++;ShowShopLb(pno,pc);}}
  if(Key=='A') //�Ϸ�
   {if(pno>0){pno--;ShowShopLb(pno,pc);}}
  if(Key=='U'||Key=='S') return;
  if(Key>='1'&&Key<='5')
   {
    int l;
    l=Key-'1'+pno*5;
    if(l<shoplbcount)
    {
     Form1->ShowTime=true;
     ProcessJiu(l);
     ShowShopLb(pno,pc);
     Form1->ShowTime=false;
    }
   }
}
}

void __fastcall MainThread::ShowShopLb(int pno,int pc)
{
 //��ʾ��Ʒ���
 int y[]={128,200,276,341,420};char buff[100];memset(buff,0,100);
 Form1->DrawDhFlag=true;
 ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_e.bmp");
 Form1->DrawDhFlag=false;
 Synchronize(Form1->DrawGuangGao);
 for(int i=0;(i+pno*5)<shoplbcount&&i<5;i++)
 {
  memset(buff,0,100);
  strncpy(buff,shoplbTab[i+pno*5].sm,20);
  DrawText32(215,y[i]-1,int(RGB(255,255,255)),"����_GB2312",buff);
  DrawText32(217,y[i]+1,int(RGB(0,0,0)),"����_GB2312",buff);
  DrawText32(216,y[i],int(RGB(255,10,255)),"����_GB2312",buff);
 }
  memset(buff,0,100);sprintf(buff,"%d",pno+1);
  DrawText24(646,70,int(RGB(255,255,255)),"����_GB2312",buff);
  DrawText24(645,69,int(RGB(0,0,0)),"����_GB2312",buff);
  DrawText24(647,71,int(RGB(0,255,0)),"����_GB2312",buff);
  sprintf(buff,"%d",pc);
  DrawText24(700,70,int(RGB(255,255,255)),"����_GB2312",buff);
  DrawText24(699,69,int(RGB(255,255,255)),"����_GB2312",buff);
  DrawText24(701,71,int(RGB(255,0,0)),"����_GB2312",buff);
}

void __fastcall MainThread::ProcessJiu(int ShopLbRno)
{
//������Ʒѡ��
 int Key;
 int pno,pc;
 char shoplb[100];
 if(ReadShop(ShopLbRno)==0){DisplayMsg("�Բ��𣬴�����Ʒ����!");return;};
 pc=shopcount/6;if(shopcount%6)pc++;pno=0;
 memset(shoplb,0,100);strncpy(shoplb,shoplbTab[ShopLbRno].sm,20);
 ShowShop(pno,pc,shoplb);
 while(1)
 {
  if(ES->KeyType!=0)return;
  Key=ReadKey();
  if(ES->KeyType!=0)return;
  if(Key=='A')  //�Ϸ�
   {if(pno>0){pno--;ShowShop(pno,pc,shoplb);}}
  if(Key=='B')  //�·�
   {if((pno+1)<pc){pno++;ShowShop(pno,pc,shoplb);}}
  if(Key=='S')return;
  if(Key=='@'){ShowShop(pno,pc,shoplb);continue;}
  if(Key>='1'&&Key<='6')  //ѡ����һ���ˮ
   {
    int l;
    l=Key-'1'+pno*6;
    if(l>=shopcount)continue;
    AddJiu(l);
    ShowShop(pno,pc,shoplb);
   }
 }
}

int __fastcall MainThread::ReadShop(int ShopLbRno)
{
 char buff[100];
 FILE *FP;
 int i;
 struct Shop A;
 memset(buff,0,100);
 strncpy(buff,shoplbTab[ShopLbRno].bh,10);
 FP=fopen(SHOPPATH,"rb");
 if(FP==NULL) return 0;
 shopcount=0;
 fseek(FP,4,SEEK_SET);fread((void *)&i,sizeof(int),1,FP);
 for(int l=0;l<i;l++)
 {
  fseek(FP,sizeof(struct Shop)*l+ShopHeadSize,SEEK_SET);
  fread((void *)&A,sizeof(struct Shop),1,FP);
  if(A.DelFlag==0) continue;
  if(strncmp(A.ShopLb,buff,10)==0){shoptab[shopcount]=A;shopcount++;}
 }
 fclose(FP);
 return shopcount;
}

void __fastcall MainThread::ShowShop(int Pno,int Pc,AnsiString ShopLb)
{
 int y[]={170,215,263,305,353,398};
 char buff[100];
 AnsiString v;
 //��ʾ��Ʒ
 Form1->DrawDhFlag=true;
 ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_d.bmp");
 Form1->DrawDhFlag=false;
 Synchronize(Form1->DrawGuangGao);
 DrawText24(241-1,125-1,int(RGB(255,255,255)),"����_GB2312",ShopLb.c_str());
 DrawText24(241+1,125+1,int(RGB(  0,  0,  0)),"����_GB2312",ShopLb.c_str());
 DrawText24(241  ,125  ,int(RGB(200,  0,255)),"����_GB2312",ShopLb.c_str());

 DrawText16(577+1,127+1,int(RGB(255,255,255)),"����_GB2312",IntToStr(Pno+1).c_str());
 DrawText16(577-1,127-1,int(RGB(  0,  0,  0)),"����_GB2312",IntToStr(Pno+1).c_str());
 DrawText16(577  ,127  ,int(RGB(  0,100,255)),"����_GB2312",IntToStr(Pno+1).c_str());

 DrawText16(645+1,127+1,int(RGB(255,255,255)),"����_GB2312",IntToStr(Pc).c_str());
 DrawText16(645-1,127-1,int(RGB(  0,  0,  0)),"����_GB2312",IntToStr(Pc).c_str());
 DrawText16(645  ,127  ,int(RGB(255,0,255)),"����_GB2312",IntToStr(Pc).c_str());
 for(int l=0;l<6&&(l+Pno*6)<shopcount;l++)
 {
  memset(buff,0,100);
  strncpy(buff,shoptab[l+Pno*6].ShopName,40);
  DrawText24(140-1,y[l]-1,int(RGB(255,255,255)),"����_GB2312",buff);
  DrawText24(140+1,y[l]+1,int(RGB(0,0,0)),"����_GB2312",buff);
  DrawText24(140,y[l],int(RGB(0,0,255)),"����_GB2312",buff);
  memset(buff,0,100);
  strncpy(buff,shoptab[l+Pno*6].ShopDw,4);
  DrawText24(537-1,y[l]-1,int(RGB(255,255,255)),"����_GB2312",buff);
  DrawText24(537+1,y[l]+1,int(RGB(0,0,0)),"����_GB2312",buff);
  DrawText24(537,y[l],int(RGB(255,20,255)),"����_GB2312",buff);
  memset(buff,0,100);
  strncpy(buff,shoptab[l+Pno*6].ShopDj,8);
  v=buff;v=v.Trim() ;
  DrawText24(620-1,y[l]-1,int(RGB(255,255,255)),"����_GB2312",v.c_str());
  DrawText24(620+1,y[l]+1,int(RGB(0,0,0)),"����_GB2312",v.c_str());
  DrawText24(620,y[l],int(RGB(0,200,255)),"����_GB2312",v.c_str());
 }
}

void __fastcall MainThread::AddJiu(int rno)
{
 //����һ���ˮ
 char buff[100];
 char sl[100];
 ShowPicture(224,164,AnsiString(PROGRUNPATH)+"picture\\a\\main_f.bmp");
 Synchronize(Form1->DrawGuangGao);
 memset(buff,0,100);
 strncpy(buff,shoptab[rno].ShopName,40);
 DrawText16(343,230,int(RGB(255,255,255)),"����_GB2312",buff);
 DrawText16(343+2,230+2,int(RGB(0,0,0)),"����_GB2312",buff);
 DrawText16(343+1,230+1,int(RGB(0,0,255)),"����_GB2312",buff);

 memset(buff,0,100);
 strncpy(buff,shoptab[rno].ShopDw,4);
 DrawText16(343,276,int(RGB(255,255,255)),"����_GB2312",buff);
 DrawText16(343+2,276+2,int(RGB(0,0,0)),"����_GB2312",buff);
 DrawText16(343+1,276+1,int(RGB(0,0,255)),"����_GB2312",buff);

 memset(buff,0,100);
 strncpy(buff,shoptab[rno].ShopDj,8);
 DrawText16(343,318,int(RGB(255,255,255)),"����_GB2312",buff);
 DrawText16(343+2,318+2,int(RGB(0,0,0)),"����_GB2312",buff);
 DrawText16(343+1,318+1,int(RGB(0,0,255)),"����_GB2312",buff);

 memset(sl,0,100);int Key;
 while(1)
  {
   if(ES->KeyType!=0)return;
   Key=ReadKey();
   if(ES->KeyType!=0)return;
   if(Key>='0'&&Key<='9'&&(strlen(sl)+1)<5)
   {
    sl[strlen(sl)]=Key&0xff;
    DrawText16(348,361,int(RGB(255,255,255)),"����_GB2312",sl);
    DrawText16(348+2,361+2,int(RGB(0,0,0)),"����_GB2312",sl);
    DrawText16(348+1,361+1,int(RGB(0,0,255)),"����_GB2312",sl);
   }
   if(Key=='S') return;
   if(Key=='T')
    {
     if(strlen(sl)>0)
      {
         sl[strlen(sl)-1]=0;
         ShowPicture(224,164,AnsiString(PROGRUNPATH)+"picture\\a\\main_f.bmp");
         Synchronize(Form1->DrawGuangGao);
         DrawText16(348,361,int(RGB(255,255,255)),"����_GB2312",sl);
         DrawText16(348+2,361+2,int(RGB(0,0,0)),"����_GB2312",sl);
         DrawText16(348+1,361+1,int(RGB(0,0,255)),"����_GB2312",sl);
         memset(buff,0,100);
         strncpy(buff,shoptab[rno].ShopName,40);
         DrawText16(343,230,int(RGB(255,255,255)),"����_GB2312",buff);
         DrawText16(343+2,230+2,int(RGB(0,0,0)),"����_GB2312",buff);
         DrawText16(343+1,230+1,int(RGB(0,0,255)),"����_GB2312",buff);

         memset(buff,0,100);
         strncpy(buff,shoptab[rno].ShopDw,4);
         DrawText16(343,276,int(RGB(255,255,255)),"����_GB2312",buff);
         DrawText16(343+2,276+2,int(RGB(0,0,0)),"����_GB2312",buff);
         DrawText16(343+1,276+1,int(RGB(0,0,255)),"����_GB2312",buff);

         memset(buff,0,100);
         strncpy(buff,shoptab[rno].ShopDj,8);
         DrawText16(343,318,int(RGB(255,255,255)),"����_GB2312",buff);
         DrawText16(343+2,318+2,int(RGB(0,0,0)),"����_GB2312",buff);
         DrawText16(343+1,318+1,int(RGB(0,0,255)),"����_GB2312",buff);
      }
    }
   if(Key=='U')  //ѡ����ȷ��
    {
     if(atoi(sl)==0) return;
     struct Dan db;
     char b[100];
     memset(b,0,100);strncpy(b,shoptab[rno].ShopDj,8);
     memset((void *)&db,0,sizeof(struct Dan));
     strncpy(db.RoomName,ComputerName,10);
     strncpy(db.ShopDj,shoptab[rno].ShopDj,8);
     strncpy(db.ShopDw,shoptab[rno].ShopDw,4);
     strncpy(db.ShopName,shoptab[rno].ShopName,20);
     strncpy(db.ShopSl,sl,5);
     sprintf(db.ShopZj,"%10.2f",atof(sl)*atof(b));
     AddJiuNum(db);
     return; 
    }
  }
}

bool __fastcall MainThread::AddJiuNum(struct Dan DBuffer)
{
  //��������������һ�ξ�ˮ
  char FnameString[1024],buff[100];
  unsigned long Fname=100;
  char CName[100];
  FILE *FP;
  memset(CName,0,100);
  GetComputerName(CName,&Fname);strupr(CName);
  memset(FnameString,0,1024);
  strcat(FnameString,DANPATH);
  strcat(FnameString,CName);
  strcat(FnameString,Now().FormatString("_hh_nn_ss").c_str());
  strcat(FnameString,".~DBF");
  FP=fopen(FnameString,"w+b");
  if(FP==NULL) return false;
  memset(DBuffer.RoomName,0,10);
  memset(DBuffer.Gh,0,10);
  strcat(DBuffer.Gh,"          ");
  strncpy(DBuffer.RoomName,CName,strlen(CName));
  fwrite((void *)&DBuffer,sizeof(struct Dan),1,FP);
  fclose(FP);
  return true;
}
bool __fastcall MainThread::GetGh()
{
//��ȡ���ź�����
 char gh[10],mm[10];
 int  ghcount=0,mmcount=0,stat=0,cstat=0;
 int Key;
/*  WinExec("x:\\SY.exe",1);//*/ShowPicture(201,207,"d:\\vod\\picture\\a\\ghmm.bmp");
 memset(gh,0,10);memset(mm,0,10);
 while(1)
  {
   if(ES->KeyType!=0)return false;
   Key=ReadKey();
   if(ES->KeyType!=0)return false;
   if(Key=='r')   //���������
    {
     stat=0;ShowPicture(201,207,"d:\\vod\\picture\\a\\ghmm.bmp");Key='!';
    }
   if(Key=='s'){stat=1;ShowPicture(201,207,"d:\\vod\\picture\\a\\ghmm.bmp");Key='!';}
   if(Key>='0'&&Key<='9')
    {
     if(stat==0) //��������
      {
       if((ghcount+1)<10){gh[ghcount]=Key&0xff;ghcount++;gh[ghcount]=0;}
      }
     if(stat==1) //��������
      {
       if((mmcount+1)<10){mm[mmcount]=Key&0xff;mmcount++;mm[mmcount]=0;}
      }
     ShowPicture(201,207,"d:\\vod\\picture\\a\\ghmm.bmp"); 
     Key='!';
    }
   if(Key=='T')
    {
     if(stat==0){if(ghcount>0){ghcount--;gh[ghcount]=0;}}
     if(stat==1){if(mmcount>0){mmcount--;mm[mmcount]=0;}}
     ShowPicture(201,207,"d:\\vod\\picture\\a\\ghmm.bmp");
     Key='!';
    }
   if(Key=='C'||Key=='D'){stat++;stat=stat%2;ShowPicture(201,207,"d:\\vod\\picture\\a\\ghmm.bmp");Key='!';}
   if(Key=='S') //���ع���
    {
      return false;
    }
   if(Key=='U') //ȷ����
    {
     break;
    }
   if(Key=='!') //ˢ�¹��
    {
     char st[100];
     int gbx,gby;
     memset(st,0,100);memset(st,'*',strlen(mm));
     DrawText16(341-1,294,int(RGB(0,0,0)),"����_GB2312",st);
     DrawText16(341+1,295,int(RGB(255,255,255)),"����_GB2312",st);
     DrawText16(341  ,296  ,int(RGB(255,0,0)),"����_GB2312",st);

     DrawText16(341-1,254,int(RGB(0,0,0)),"����_GB2312",gh);
     DrawText16(341+1,255,int(RGB(255,255,255)),"����_GB2312",gh);
     DrawText16(341  ,256  ,int(RGB(255,0,0)),"����_GB2312",gh);
     cstat++;cstat=cstat%2;
     if(stat==0) //������ʾ���Ź��
      {
        Form1->GetTextWidthText=gh;
        Synchronize(Form1->GetTextWidth);
        gby=262;
        gbx=341+Form1->GetTextWidthV;
      }
     else
      {
        Form1->GetTextWidthText=mm;
        Synchronize(Form1->GetTextWidth);
        gby=302;
        gbx=341+Form1->GetTextWidthV;
      }
      AnsiString gbwjm="d:\\vod\\picture\\a\\c";gbwjm=gbwjm+IntToStr(cstat+1)+".bmp";
      ShowPicture(gbx,gby,gbwjm.c_str()) ;
    }
  }
 //�������ü����ͱ��
 if(checkUser(mm,gh))
  {
   GhValue=gh;return true;
  }
 else return false;
}

bool __fastcall MainThread::checkUser(AnsiString Password,AnsiString No)
{
 FILE *FP;
 struct JiuYhStru  P;
 char pass[100],gh[100];
 AnsiString p,g;
 int i;
 FP=fopen("x:\\jiuyh.dbf","rb");  //��ȡX:\\���µľ�ˮ�û������
 if(FP==NULL)return false;
 fseek(FP,4,SEEK_SET);fread(&i,sizeof(int),1,FP);
 for(int k=0;k<i;k++)
  {
   fseek(FP,JIUYHHeadSize+sizeof(struct JiuYhStru)*k,SEEK_SET);
   fread(&P,sizeof(struct JiuYhStru),1,FP);
   memset(pass,0,100);memset(gh,0,100);
   if((P.delflag&0xff)=='*')continue;
   strncpy(pass,P.mm,10);strncpy(gh,P.gh,10);p=pass;g=gh;
   if(p.Trim()==Password.Trim()&&No.Trim()==g.Trim())
    {
     fclose(FP);return true;
    }
  }
  fclose(FP);return false;
}
//------------------------------------------------------------------------------
void __fastcall MainThread::InterNet()
{
  //��InterNet����
  int Key;
  IArrayCount=0;
  int PageCount,PageNo=0;
  int rc;
  FILE *FP;
  FP=fopen(INTERPATH,"rb");
  if(FP==NULL){DisplayMsg("�Բ�����û�п���!");return;}
  fseek(FP,4,SEEK_SET);fread((void *)&rc,sizeof(int),1,FP);
  for(int l=0;l<rc&&IArrayCount<1000;l++)
   {
    struct InterNet A;
    fseek(FP,INTERNETHEAD+sizeof(struct InterNet)*l,SEEK_SET);
    fread((void *)&A,sizeof(struct InterNet),1,FP);
    if(A.delflag=='*')continue;
    IArray[IArrayCount]=A;
    IArray[IArrayCount].sm[39]=0;
    IArray[IArrayCount].wz[199]=0;
    IArrayCount++;
   }
  fclose(FP);
  PageCount=IArrayCount/6;if(IArrayCount%6)PageCount++;
NETBEGIN:
  ShowHomePage(PageNo,PageCount);

  while(1)
  {
   if(ES->KeyType!=0)return ;
   Key=ReadKey();
   if(ES->KeyType!=0)return ;
   if(Key=='S')return;
   if(Key>='1'&&Key<='6')
    {
     int k;
     k=Key-'1'+PageNo*6;
     if(k<IArrayCount)
      {
        Synchronize(Form1->ShowNetPage);break;  //��ʾ��ҳ������ֹ
      }
    }
  }
  while(1)
   {
    if(ES->KeyType!=0){Synchronize(Form1->CloseNetPage);return ;}
    Key=ReadKey();
    if(ES->KeyType!=0){Synchronize(Form1->CloseNetPage);return ;}
    if(Key=='S'){Synchronize(Form1->CloseNetPage);goto NETBEGIN;}
   }
}

void __fastcall MainThread::ShowHomePage(int PageNo,int PageCount)
{
 //��ʾ��ַѡ��ҳ��
 struct ffblk fb;
 int y[]={138,187,232,277,322,367};
 Form1->DrawDhFlag=false;
 ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\main_net.bmp");
 Synchronize(Form1->DrawGuangGao);
 for(int i=0;i<6&&(i+PageNo*6)<IArrayCount;i++)
  {
    char buff[100];AnsiString msg;
    memset(buff,0,100);strncpy(buff,IArray[i].sm,40);
    msg=buff;msg=msg.Trim();msg=msg.SubString(1,25);
    memset(buff,0,100);
    DrawText24(138+1,y[i]+7,int(RGB(255,255,255)),"����_GB2312",msg.c_str());
    DrawText24(138-1,y[i]+6,int(RGB(0,0,255)),"����_GB2312",msg.c_str());
    DrawText24(138,y[i]+5,int(RGB(0,0,255)),"����_GB2312",msg.c_str());
  }
 DrawText16(115+1,444+1,int(RGB(255,255,255)),"����_GB2312",IntToStr(PageNo+1).c_str());
 DrawText16(115-1,444-1,int(RGB(0,0,0)),"����_GB2312",IntToStr(PageNo+1).c_str());
 DrawText16(115,444,int(RGB(0,255,255)),"����_GB2312",IntToStr(PageNo+1).c_str());

 DrawText16(173+1,444+1,int(RGB(255,255,255)),"����_GB2312",IntToStr(PageCount).c_str());
 DrawText16(173-1,444-1,int(RGB(0,0,0)),"����_GB2312",IntToStr(PageCount).c_str());
 DrawText16(173,444,int(RGB(255,0,0)),"����_GB2312",IntToStr(PageCount).c_str());

}
void  __fastcall MainThread::DrawText(int x,int y,int color,
                                      AnsiString Fontname,
                                      char * Text,
                                      int FontSize,
                                      char yz)
{
//��ʾ����
 if(ES->KeyType!=0)return;
 DTS->x=x;
 DTS->y=y;
 DTS->Color=color;
 DTS->FontSize=FontSize;
 memset(DTS->FontName,0,1024);
 strcat(DTS->FontName,Fontname.c_str());
 memset(DTS->Text,0,1024);
 if(yz=='0'||yz==' ')
  {
   strcat(DTS->Text,Text);
   DTS->fonttype=3;
  };
 if(yz=='1')
 {
   memcpy(DTS->Text,Text,wcslen((wchar_t *)Text)*sizeof(wchar_t));
   DTS->Len=wcslen((wchar_t *)Text)*sizeof(wchar_t);
   DTS->fonttype=4;
 };
 Synchronize(Form1->TText);
}


void __fastcall MainThread::ProcessBh()
{
 char BhBuff[20];
 int  Key;
 ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\��ŵ��.bmp");
 memset(BhBuff,0,20);
 while(1)
 {
  Form1->ShowTime=true;
  if(ES->KeyType!=0)return;
  Key=ReadKey();
  if(ES->KeyType!=0)return;
  if(Key=='U') //ȷ��
   {
    ReadSongTab("bh",BhBuff,0);
    if(Form1->SongCount==0)
    {
     DisplayMsg("�Բ��𣬴˱�Ÿ���������!");
     memset(BhBuff,0,20);
     ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\��ŵ��.bmp");
     continue;
    }
    else
    {
      //�ҵ�����
        Form1->PlaySongTab[Form1->PlaySongCount]=Form1->SelSong[0];
        Form1->PlaySongTab[Form1->PlaySongCount].Stat=0;
        SongAddClick(Form1->SelSong[0].RecNo);  //����һ�ε㲥��
        Form1->PlaySongCount++;
        char buff[100],gb;
        gb=Form1->SelSong[0].A.LB[0]&0xff;
        memset(buff,0,100);strncpy(buff,Form1->SelSong[0].A.GM,30);
        DrawText(220-1,220-1,RGB(0,0,0),Form1->SongNameFont,buff,Form1->SongNameFontSize,gb);
        DrawText(220+1,220+1,RGB(255,255,255),Form1->SongNameFont,buff,Form1->SongNameFontSize,gb);
        DrawText(220,220,RGB(0,0,255),Form1->SongNameFont,buff,Form1->SongNameFontSize,gb);

        memset(buff,0,100);strncpy(buff,Form1->SelSong[0].A.GX,10);
        DrawText(240-1,283-1,RGB(0,0,0),Form1->SongNameFont,buff,Form1->SongNameFontSize,gb);
        DrawText(240+1,283+1,RGB(255,255,255),Form1->SongNameFont,buff,Form1->SongNameFontSize,gb);
        DrawText(240,283,RGB(0,0,255),Form1->SongNameFont,buff,Form1->SongNameFontSize,gb);

        memset(buff,0,100);strncpy(buff,Form1->SelSong[0].A.DCCS,10);
        DrawText(260-1,350-1,RGB(0,0,0),Form1->SongNameFont,buff,Form1->SongNameFontSize,'0');
        DrawText(260+1,350+1,RGB(255,255,255),Form1->SongNameFont,buff,Form1->SongNameFontSize,'0');
        DrawText(260,350,RGB(0,0,255),Form1->SongNameFont,buff,Form1->SongNameFontSize,'0');

        //������ʾ������Ƭ
        AnsiString lj;
        struct ffblk fb;
        memset(buff,0,100);strncpy(buff,Form1->SelSong[0].A.GX,10);
       lj=AnsiString(PROGRUNPATH)+"picture\\singer\\";lj=lj+buff;lj=lj.Trim();lj=lj+".bmp";
      if(findfirst(lj.c_str(),&fb,FA_ARCH|FA_RDONLY)==0)  //�ҵ�����ͼƬ������ʾ������
       {
        ShowPicture(590,226,lj.c_str());
       }
      else   //û�У�����ʾ�滻ͼƬ�͸�������
       {
        ShowPicture(590,226,AnsiString(PROGRUNPATH)+"picture\\a\\repl.bmp");
        lj=Form1->SelSong[0].A.GX;lj=lj.Trim();
        DrawText16(600-1,350-1,int(RGB(0,0,0)),"����_GB2312",lj.c_str());
        DrawText16(600+1,350+1,int(RGB(255,255,255)),"����_GB2312",lj.c_str());
        DrawText16(600,350,int(RGB(255,0,0)),"����_GB2312",lj.c_str());
       }
       DisplayMsg("�����㲥�ɹ�!");
       ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\��ŵ��.bmp");
       memset(BhBuff,0,20);
    }
   }
  if(Key=='@')
   {
    ShowPicture(0,0,AnsiString(PROGRUNPATH)+"picture\\a\\��ŵ��.bmp");
    DrawText(400-1,150-1,RGB(0,0,0),"����",BhBuff,26,'0');
    DrawText(400+1,150+1,RGB(255,255,255),"����",BhBuff,26,'0');
    DrawText(400,150,RGB(0,0,255),"����",BhBuff,26,'0');
   }
  if(Key=='S')return; //����
  if(Key>='0'&&Key<='9')
   {

    BhBuff[strlen(BhBuff)]=Key;
    BhBuff[strlen(BhBuff)]=0;
    DrawText(400-1,150-1,RGB(0,0,0),"����",BhBuff,26,'0');
    DrawText(400+1,150+1,RGB(255,255,255),"����",BhBuff,26,'0');
    DrawText(400,150,RGB(0,0,255),"����",BhBuff,26,'0');
   }
 }
}