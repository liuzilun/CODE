//---------------------------------------------------------------------------

#ifndef mformH
#define mformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TStringGrid *StringGrid1;
        TPanel *Panel1;
        TButton *Button1;
        TButton *Button2;
        TButton *Button3;
        TButton *Button4;
        TButton *Button5;
        TButton *Button6;
        TOpenDialog *OpenDialog1;
        TButton *Button7;
        TSaveDialog *SaveDialog1;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button5Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall Button4Click(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall Button6Click(TObject *Sender);
        void __fastcall StringGrid1KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall StringGrid1KeyPress(TObject *Sender, char &Key);
        void __fastcall Button7Click(TObject *Sender);
private:	// User declarations
        int Stat;
        int PathCount;
        int EditStat;
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
