//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <iphlpapi.h>
#include "winsock2.h"
#include "MForm.h"
#include "stdio.h"
#include "stdlib.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
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
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
struct IdMsg Id;
char buff[1000];
DWORD VEAX,VEBX,VECX,VEDX;
asm{
 push eax;
 push ebx;
 push ecx;
 push edx;
 xor eax,eax;
 xor ebx,ebx;
 xor ecx,ecx;
 xor edx,edx;
 mov eax,0;
 cpuid;
 mov VEAX,eax;
 mov VEBX,ebx;
 mov VECX,ecx;
 mov VEDX,edx;
 pop edx;
 pop ecx;
 pop ebx;
 pop eax;
};
memset(&Id,0,sizeof(struct IdMsg));
Id.EAX=VEAX;
Id.EBX=VEBX;
Id.ECX=VECX;
Id.EDX=VEDX;
if(SMB){
sprintf(buff,"EAX=[%d],EBX=[%d],ECX=[%d],EDX=[%d]",VEAX,VEBX,VECX,VEDX);
ShowMessage(buff);}
//GetMsg
GetVolumeInformation("d:\\",
                     Id.VolumeName,
                     40,
                     &Id.VolSerial,
                     &Id.MaxCompant,
                     &Id.FileSystemFlag,
                     Id.FileSystemName,10);
if(SMB){
sprintf(buff,"VolumeName:=[%s],\nVolumeSerial:=[%d],\nFileSystemName:=[%s],\nFileSystemFlag:=[%d],\nMaxCompant:=[%d]",
              Id.VolumeName,Id.VolSerial,
              Id.FileSystemName,Id.FileSystemFlag,Id.MaxCompant);
ShowMessage(buff);}
IP_ADAPTER_INFO Ip;
struct hostent FAR *P;
unsigned long i;
i=sizeof(IP_ADAPTER_INFO);
GetAdaptersInfo(&Ip,&i);
memcpy(Id.NetAddress,Ip.Address,8);
i=40;
GetComputerName(Id.ComputerName,&i);
strcat(Id.AdapterName,Ip.AdapterName);
strcat(Id.Description,Ip.Description);
Id.NetType=Ip.Type;
if(SMB){
sprintf(buff,"NetAddress:=[%x][%x][%x][%x][%x][%x][%x][%x]",
        Ip.Address[0]&0xff,Ip.Address[1]&0xff,Ip.Address[2]&0xff,
        Ip.Address[3]&0xff,Ip.Address[4]&0xff,Ip.Address[5]&0xff,
        Ip.Address[6]&0xff,Ip.Address[7]&0xff);
ShowMessage(buff);

sprintf(buff,"ComputerName:=[%s]\nAdapterName:=[%s]\nDescription:=[%s]\nType:=[%d]",
        Id.ComputerName,Id.AdapterName,Id.Description,Ip.Type);
ShowMessage(buff);
}
FILE *FP;
FP=fopen(Edit1->Text.c_str(),"w+b");
if(FP==NULL)
 {
  MessageDlg("对不起！不能够保存数据！",mtInformation,TMsgDlgButtons()<<mbYes,0);
  return;
 }
 fwrite(&Id,sizeof(struct IdMsg),1,FP);fclose(FP);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
if(Key==112)
  SMB=true;
}
//---------------------------------------------------------------------------

