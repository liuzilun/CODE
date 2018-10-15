//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USEFORM("MForm.cpp", Form1);
USEFORM("D:\123\tiao.cpp", impo);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TForm1), &Form1);
                 Application->CreateForm(__classid(Timpo), &impo);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
