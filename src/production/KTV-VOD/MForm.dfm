object Form1: TForm1
  Left = 93
  Top = 37
  BorderStyle = bsNone
  Caption = 'Form1'
  ClientHeight = 385
  ClientWidth = 511
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Visible = True
  OnCloseQuery = FormCloseQuery
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnMouseDown = FormMouseDown
  OnPaint = FormPaint
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Image1: TImage
    Left = 0
    Top = 0
    Width = 800
    Height = 56
    Visible = False
  end
  object Image2: TImage
    Left = 80
    Top = 360
    Width = 516
    Height = 42
    Visible = False
  end
  object Panel1: TPanel
    Left = 0
    Top = 72
    Width = 800
    Height = 393
    Caption = 'Panel1'
    TabOrder = 0
    object CppWebBrowser1: TCppWebBrowser
      Left = 1
      Top = 1
      Width = 798
      Height = 391
      Align = alClient
      TabOrder = 0
      ControlData = {
        4C0000007A520000692800000000000000000000000000000000000000000000
        000000004C000000000000000000000001000000E0D057007335CF11AE690800
        2B2E126208000000000000004C0000000114020000000000C000000000000046
        8000000000000000000000000000000000000000000000000000000000000000
        00000000000000000100000000000000000000000000000000000000}
    end
  end
  object Timer1: TTimer
    Interval = 5000
    OnTimer = Timer1Timer
    Left = 144
    Top = 64
  end
  object Timer2: TTimer
    OnTimer = Timer2Timer
    Left = 144
    Top = 216
  end
  object NMUDP1: TNMUDP
    RemotePort = 0
    LocalPort = 12345
    ReportLevel = 1
    OnDataReceived = NMUDP1DataReceived
    Left = 368
    Top = 96
  end
  object NMUDP2: TNMUDP
    RemotePort = 0
    LocalPort = 6688
    ReportLevel = 1
    OnDataReceived = NMUDP2DataReceived
    Left = 392
    Top = 144
  end
  object NMUDP3: TNMUDP
    RemotePort = 0
    LocalPort = 5555
    ReportLevel = 1
    OnDataReceived = NMUDP3DataReceived
    Left = 224
    Top = 232
  end
end
