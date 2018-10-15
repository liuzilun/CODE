//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "stdio.h"
#include "stdlib.h"
#include "mform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
  struct SearchPath         //搜索路径结构
   {
    char Path[40];          //搜索路径名
    int MaxClick;           //此路径最大的点播数
   };
  struct LoginPack
   {
    char RemoteName[20];   //远程用户名称如"\\888lfw\"
    int  DriveCount;       //拥有搜索路径的个数
    struct SearchPath Sp[24]; //最多24个搜索路径
   };
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
 Stat=0;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button1Click(TObject *Sender)
{
 if(Stat!=0&&EditStat==1)
 {
  int i=MessageDlg("对不起,数据没有保存,你要保存数据,或者是放弃?",mtWarning,TMsgDlgButtons()<<mbYes<<mbNo,0);
  if(i==mrYes){Button5Click(this);}
 }
 Stat=1;  //设置为新建项目
 Caption="路径编辑程序 - 新建";
 for(int i=1;i<StringGrid1->RowCount;i++)
   StringGrid1->Rows[i]->Clear();
 AnsiString InputString;
 InputString = InputBox("路径数", "请你输入路径总条数", "0");
 if(atoi(InputString.c_str())<2)
   StringGrid1->RowCount=2;
 else
   StringGrid1->RowCount=atoi(InputString.c_str())+1;
 EditStat=1;
 PathCount=atoi(InputString.c_str());
 StringGrid1->Cells[0][0]="编号";
 StringGrid1->Cells[1][0]="路径名";
 StringGrid1->Cells[2][0]="最大限点";
 StringGrid1->EditorMode=true;
 for(int i=1;i<StringGrid1->RowCount;i++)
  {
   StringGrid1->Cells[0][i]=IntToStr(i);
  }
 Button3->Enabled=true;
 Button4->Enabled=true;
 Button5->Enabled=true;
 Button6->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button5Click(TObject *Sender)
{
//保存数据
FILE *FP;
struct LoginPack P;
memset((void *)&P,0,sizeof(struct LoginPack));
if(Stat==1)  //是新建的
 {
  if(SaveDialog1->Execute())
   {
    FP=fopen(SaveDialog1->FileName.c_str(),"w+b");
    if(FP==NULL)
     {
      MessageDlg("对不起,数据不能保存!",mtWarning,TMsgDlgButtons()<<mbYes,0);return;
     }
    else
     {
      P.DriveCount=PathCount;
      for(int i=0;i<PathCount&&i<24;i++)
       {
        AnsiString st;
        st=StringGrid1->Cells[1][i+1];
        strcpy(P.Sp[i].Path,st.c_str());
        st=StringGrid1->Cells[2][i+1];
        P.Sp[i].MaxClick=atoi(st.c_str());
       }
     }
    fwrite((void *)&P,sizeof(struct LoginPack),1,FP);fclose(FP);Stat=0;
    for(int i=1;i<StringGrid1->RowCount;i++)StringGrid1->Rows[i]->Clear();StringGrid1->RowCount=2;
    Button3->Enabled=false;Button4->Enabled=false;Button5->Enabled=false;Button6->Enabled=false;
    MessageDlg("数据保存成功!",mtWarning,TMsgDlgButtons()<<mbYes,0);
    return;
   }
 }
if(Stat==2)  //是打开的文件
 {
    FP=fopen(OpenDialog1->FileName.c_str(),"w+b");
    if(FP==NULL)
     {
      MessageDlg("对不起,数据不能保存!",mtWarning,TMsgDlgButtons()<<mbYes,0);return;
     }
    else
     {
      P.DriveCount=PathCount;
      for(int i=0;i<PathCount&&i<24;i++)
       {
        AnsiString st;
        st=StringGrid1->Cells[1][i+1];
        strcpy(P.Sp[i].Path,st.c_str());
        st=StringGrid1->Cells[2][i+1];
        P.Sp[i].MaxClick=atoi(st.c_str());
       }
     }
    fwrite((void *)&P,sizeof(struct LoginPack),1,FP);fclose(FP);Stat=0;
    Button3->Enabled=false;Button4->Enabled=false;Button5->Enabled=false;Button6->Enabled=false;
    for(int i=1;i<StringGrid1->RowCount;i++)StringGrid1->Rows[i]->Clear();StringGrid1->RowCount=2;
    MessageDlg("数据保存成功!",mtWarning,TMsgDlgButtons()<<mbYes,0);
    return;
 }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
 if(Stat!=0&&EditStat==1)  //编辑过数据
  {
  int i=MessageDlg("对不起,数据没有保存,你要保存数据,或者是放弃?",mtWarning,TMsgDlgButtons()<<mbYes<<mbNo,0);
  if(i==mrYes){Button5Click(this);Stat=0;}
  }
 for(int i=1;i<StringGrid1->RowCount;i++)
  {
   StringGrid1->Rows[i]->Clear();
  }
 StringGrid1->RowCount=2;
 StringGrid1->Cells[0][0]="编号";
 StringGrid1->Cells[1][0]="路径名";
 StringGrid1->Cells[2][0]="最大限点";
 StringGrid1->EditorMode=true;
 if(OpenDialog1->Execute())
  {
   FILE *FP;
   FP=fopen(OpenDialog1->FileName.c_str(),"rb");
   if(FP==NULL)
    {
     MessageDlg("对不起,文件打开失败!",mtWarning,TMsgDlgButtons()<<mbYes<<mbNo,0);
     Stat=0;
     Caption="路径编辑";
     return;
    }
   else
    {
     struct LoginPack P;
     fread((void *)&P,sizeof(struct LoginPack),1,FP);
     fclose(FP);
     Caption="路径编辑 - "+OpenDialog1->FileName;
     PathCount=P.DriveCount;
     if(P.DriveCount<2)StringGrid1->RowCount=2;
     else StringGrid1->RowCount=P.DriveCount+1;
     for(int i=0;i<P.DriveCount;i++)
      {
       StringGrid1->Cells[0][i+1]=IntToStr(i+1);
       StringGrid1->Cells[1][i+1]=P.Sp[i].Path;
       StringGrid1->Cells[2][i+1]=IntToStr(P.Sp[i].MaxClick);
      }
     Button3->Enabled=true;
     Button4->Enabled=true;
     Button5->Enabled=true;
     Button6->Enabled=true;
     Stat=2;
    }

  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormShow(TObject *Sender)
{
 Button3->Enabled=false;
 Button4->Enabled=false;
 Button5->Enabled=false;
 Button6->Enabled=false;
 PathCount=0;
 Stat=0;EditStat=0;        
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button4Click(TObject *Sender)
{
 StringGrid1->RowCount++;
 StringGrid1->Cells[0][StringGrid1->RowCount-1]=IntToStr(StringGrid1->RowCount);
 StringGrid1->Row=StringGrid1->RowCount-1;StringGrid1->EditorMode=true;
 PathCount++;
 EditStat=1;
 Button6->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender)
{
 int i;
 if(PathCount==0)return;
 i=MessageDlg("你真的要删除这条路径吗?",mtInformation,TMsgDlgButtons()<<mbYes<<mbNo,0);
 if(i==mrYes)
  {
   StringGrid1->Rows[StringGrid1->Row]->Clear();
   PathCount--;
   if(StringGrid1->RowCount==2)return;
   for(int i=StringGrid1->Row+1;i<StringGrid1->RowCount;i++)
    {
     StringGrid1->Cells[1][i-1]=StringGrid1->Cells[1][i];
    }
   StringGrid1->Rows[StringGrid1->RowCount-1]->Clear();
   StringGrid1->RowCount--;
   for(int i=1;i<StringGrid1->RowCount;i++)
    StringGrid1->Cells[0][i]=IntToStr(i);
  }
 EditStat=1;
 Button6->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button6Click(TObject *Sender)
{
//保存数据
FILE *FP;
struct LoginPack P;
memset((void *)&P,0,sizeof(struct LoginPack));
if(Stat==1)  //是新建的
 {
  if(SaveDialog1->Execute())
   {
    FP=fopen(SaveDialog1->FileName.c_str(),"w+b");
    if(FP==NULL)
     {MessageDlg("对不起,数据不能保存!",mtWarning,TMsgDlgButtons()<<mbYes,0);return;}
    else
     {
      P.DriveCount=PathCount;
      for(int i=0;i<PathCount&&i<24;i++)
       {
        AnsiString st;st=StringGrid1->Cells[1][i+1];
        strcpy(P.Sp[i].Path,st.UpperCase().c_str());
        st=StringGrid1->Cells[2][i+1];P.Sp[i].MaxClick=atoi(st.c_str());
       }
     }
    fwrite((void *)&P,sizeof(struct LoginPack),1,FP);fclose(FP);Stat=2;EditStat=0;
    Button5->Enabled=false;Button6->Enabled=false;
    MessageDlg("数据保存成功!",mtWarning,TMsgDlgButtons()<<mbYes,0);
    OpenDialog1->FileName=SaveDialog1->FileName;return;
   }
 }
if(Stat==2)  //是打开的文件
 {
    FP=fopen(OpenDialog1->FileName.c_str(),"r+b");
    if(FP==NULL)
     {
      MessageDlg("对不起,数据不能保存!",mtWarning,TMsgDlgButtons()<<mbYes,0);return;
     }
    else
     {
      P.DriveCount=PathCount;
      for(int i=0;i<PathCount&&i<24;i++)
       {
        AnsiString st;
        st=StringGrid1->Cells[1][i+1];
        strcpy(P.Sp[i].Path,st.c_str());
        st=StringGrid1->Cells[2][i+1];
        P.Sp[i].MaxClick=atoi(st.c_str());
       }
     }
    fwrite((void *)&P,sizeof(struct LoginPack),1,FP);fclose(FP);Stat=2;EditStat=0;
    Button6->Enabled=false;Button5->Enabled=false;    
    MessageDlg("数据保存成功!",mtWarning,TMsgDlgButtons()<<mbYes,0);return;
 }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::StringGrid1KeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
 if(PathCount==0){Key=0;return ;}
EditStat=1;
Button3->Enabled=true;
Button4->Enabled=true;
Button5->Enabled=true;
Button6->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::StringGrid1KeyPress(TObject *Sender, char &Key)
{
 if(PathCount==0){Key=0;return;}
EditStat=1;
Button3->Enabled=true;
Button4->Enabled=true;
Button5->Enabled=true;
Button6->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button7Click(TObject *Sender)
{
 if(Stat!=0&&EditStat==1)
  {
   int i=MessageDlg("有数据被编辑,你需要保存吗?",mtWarning,TMsgDlgButtons()<<mbYes<<mbNo,0);
   if(i==mrYes)
    {
     Button5Click(this);
    }
  }
Close();
}
//---------------------------------------------------------------------------
