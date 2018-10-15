//---------------------------------------------------------------------------

#ifndef NetMsgH
#define NetMsgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <NMUDP.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm2 : public TForm
{
__published:	// IDE-managed Components
        TNMUDP *NMUDP1;
        TTimer *Timer1;
        void __fastcall NMUDP1DataReceived(TComponent *Sender,
          int NumberBytes, AnsiString FromIP, int Port);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations

public:		// User declarations
        __fastcall TForm2(TComponent* Owner);
        HANDLE  NetMsgEventHandle;
        AnsiString FindSongBuff;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
//---------------------------------------------------------------------------
#endif
