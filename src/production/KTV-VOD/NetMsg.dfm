object Form2: TForm2
  Left = 232
  Top = 238
  BorderStyle = bsNone
  Caption = 'Form2'
  ClientHeight = 78
  ClientWidth = 131
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object NMUDP1: TNMUDP
    RemotePort = 0
    LocalPort = 8301
    ReportLevel = 1
    OnDataReceived = NMUDP1DataReceived
    Left = 64
    Top = 24
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 24
    Top = 16
  end
end
