//---------------------------------------------------------------------------

#ifndef mformH
#define mformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TEdit *Edit1;
        TLabel *Label1;
        TButton *Button1;
        TGroupBox *GroupBox1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TEdit *Edit2;
        TEdit *Edit3;
        TEdit *Edit4;
        TEdit *Edit5;
        TGroupBox *GroupBox2;
        TLabel *Label6;
        TEdit *Edit6;
        TLabel *Label7;
        TEdit *Edit7;
        TGroupBox *GroupBox3;
        TLabel *Label8;
        TEdit *Edit8;
        TEdit *Edit9;
        TLabel *Label9;
        TEdit *Edit10;
        TLabel *Label10;
        void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
