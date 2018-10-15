//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PrevView.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "WMPLib_OCX"
#pragma resource "*.dfm"
TForm3 *Form3;
//---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm3::FormShow(TObject *Sender)
{
Top=0;Left=0;Height=600;Width=800;
AWindowsMediaPlayer1->Controls->Visible=false;
}
//---------------------------------------------------------------------------
