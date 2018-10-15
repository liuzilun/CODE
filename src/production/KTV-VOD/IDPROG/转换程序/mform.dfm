object Form1: TForm1
  Left = 196
  Top = 196
  BorderStyle = bsDialog
  Caption = 'Form1'
  ClientHeight = 295
  ClientWidth = 412
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 19
    Top = 8
    Width = 36
    Height = 12
    Caption = '文件名'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = '宋体'
    Font.Style = []
    ParentFont = False
  end
  object Edit1: TEdit
    Left = 75
    Top = 8
    Width = 236
    Height = 21
    TabOrder = 0
  end
  object Button1: TButton
    Left = 328
    Top = 5
    Width = 75
    Height = 25
    Caption = '转换'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = '宋体'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    OnClick = Button1Click
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = 32
    Width = 193
    Height = 121
    Caption = 'CPU'
    TabOrder = 2
    object Label2: TLabel
      Left = 53
      Top = 17
      Width = 21
      Height = 13
      Caption = 'EAX'
    end
    object Label3: TLabel
      Left = 53
      Top = 38
      Width = 21
      Height = 13
      Caption = 'EBX'
    end
    object Label4: TLabel
      Left = 53
      Top = 61
      Width = 21
      Height = 13
      Caption = 'ECX'
    end
    object Label5: TLabel
      Left = 51
      Top = 80
      Width = 22
      Height = 13
      Caption = 'EDX'
    end
    object Edit2: TEdit
      Left = 75
      Top = 13
      Width = 67
      Height = 21
      TabOrder = 0
    end
    object Edit3: TEdit
      Left = 75
      Top = 34
      Width = 67
      Height = 21
      TabOrder = 1
    end
    object Edit4: TEdit
      Left = 75
      Top = 55
      Width = 67
      Height = 21
      TabOrder = 2
    end
    object Edit5: TEdit
      Left = 75
      Top = 76
      Width = 67
      Height = 21
      TabOrder = 3
    end
  end
  object GroupBox2: TGroupBox
    Left = 200
    Top = 32
    Width = 209
    Height = 121
    Caption = 'Disk'
    TabOrder = 3
    object Label6: TLabel
      Left = 11
      Top = 22
      Width = 63
      Height = 13
      Caption = 'VolumeName'
    end
    object Label7: TLabel
      Left = 12
      Top = 43
      Width = 61
      Height = 13
      Caption = 'VolumeSerial'
    end
    object Edit6: TEdit
      Left = 75
      Top = 20
      Width = 67
      Height = 21
      TabOrder = 0
    end
    object Edit7: TEdit
      Left = 75
      Top = 41
      Width = 67
      Height = 21
      TabOrder = 1
    end
  end
  object GroupBox3: TGroupBox
    Left = 0
    Top = 154
    Width = 409
    Height = 138
    Caption = 'NetAdapter'
    TabOrder = 4
    object Label8: TLabel
      Left = 9
      Top = 16
      Width = 65
      Height = 13
      Caption = 'AdapterName'
    end
    object Label9: TLabel
      Left = 35
      Top = 37
      Width = 39
      Height = 13
      Caption = 'Descript'
    end
    object Label10: TLabel
      Left = 19
      Top = 58
      Width = 55
      Height = 13
      Caption = 'NetAddress'
    end
    object Edit8: TEdit
      Left = 75
      Top = 14
      Width = 330
      Height = 21
      TabOrder = 0
    end
    object Edit9: TEdit
      Left = 75
      Top = 35
      Width = 330
      Height = 21
      TabOrder = 1
    end
    object Edit10: TEdit
      Left = 75
      Top = 56
      Width = 222
      Height = 21
      TabOrder = 2
    end
  end
end
