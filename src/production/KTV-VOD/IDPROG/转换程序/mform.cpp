//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "stdio.h"
#include "stdlib.h"
#include "mform.h"
struct IdMsg
 {
  DWORD VolSerial;
  DWORD MaxCompant;
  DWORD FileSystemFlag;
  DWORD EAX,EBX,ECX,EDX;
  char  NetAddress[10];
  char  ComputerName[40];
  char  VolumeName[30];
  char  FileSystemName[10];
  char  AdapterName[100];
  char  Description[136];
  int   NetType;
 };
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
FILE *FP;
struct IdMsg ID;
FP=fopen(Edit1->Text.c_str(),"rb");
AnsiString Msg;
if(FP==NULL)
 {MessageDlg("Sorry Open File Is Fail!",mtWarning,TMsgDlgButtons()<<mbYes,0);return;}
fread(&ID,sizeof(struct IdMsg),1,FP);fclose(FP);
Edit2->Text=ID.EAX;Edit3->Text=ID.EBX;Edit4->Text=ID.ECX;Edit5->Text=ID.EDX;
Edit6->Text=ID.VolumeName;Edit7->Text=ID.VolSerial;Edit8->Text=ID.AdapterName;
Edit9->Text=ID.Description;
char buff[100];
sprintf(buff,"{0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x}",
        ID.NetAddress[0]&0xff,ID.NetAddress[1]&0xff,ID.NetAddress[2]&0xff,ID.NetAddress[3]&0xff,
        ID.NetAddress[4]&0xff,ID.NetAddress[5]&0xff,ID.NetAddress[6]&0xff,ID.NetAddress[7]&0xff);
Edit10->Text=buff;        
}
//---------------------------------------------------------------------------
