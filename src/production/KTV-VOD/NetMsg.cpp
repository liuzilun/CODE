//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "head.h"
#include "NetMsg.h"
#include "MForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
        : TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TForm2::NMUDP1DataReceived(TComponent *Sender,
      int NumberBytes, AnsiString FromIP, int Port)
{
 AnsiString msg;
 struct QQDG Recv;
 int l=sizeof(struct QQDG);
 memset((void *)&Recv,0,sizeof(struct QQDG));
 NMUDP1->ReadBuffer((char *)&Recv,sizeof(struct QQDG),l);
 Form1->FindSongBuff=Recv.Song;
 Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::FormShow(TObject *Sender)
{
Top=0;Left=0;Width=1;Height=1;
Form1->FindSongBuff="";
Timer1->Enabled=true;
Tag=0;
}
//---------------------------------------------------------------------------
void __fastcall TForm2::Timer1Timer(TObject *Sender)
{
Tag++;
if(Tag>=2)Close();        
}
//---------------------------------------------------------------------------

void __fastcall TForm2::FormClose(TObject *Sender, TCloseAction &Action)
{
 Timer1->Enabled=false;
 Tag=0;        
}
//---------------------------------------------------------------------------

