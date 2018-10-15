object Form1: TForm1
  Left = 192
  Top = 107
  BorderStyle = bsDialog
  Caption = #36335#24452#32534#36753#36719#20214
  ClientHeight = 453
  ClientWidth = 634
  Color = clBtnFace
  Font.Charset = GB2312_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #23435#20307
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object StringGrid1: TStringGrid
    Left = 0
    Top = 41
    Width = 634
    Height = 412
    Align = alClient
    ColCount = 3
    DefaultRowHeight = 16
    RowCount = 2
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing, goAlwaysShowEditor]
    TabOrder = 0
    OnKeyDown = StringGrid1KeyDown
    OnKeyPress = StringGrid1KeyPress
    ColWidths = (
      30
      414
      119)
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 634
    Height = 41
    Align = alTop
    TabOrder = 1
    object Button1: TButton
      Left = 6
      Top = 8
      Width = 85
      Height = 25
      Caption = '&1.'#26032#24314#25991#20214
      TabOrder = 0
      OnClick = Button1Click
    end
    object Button2: TButton
      Left = 95
      Top = 8
      Width = 85
      Height = 25
      Caption = '&2.'#25171#24320#25991#20214
      TabOrder = 1
      OnClick = Button2Click
    end
    object Button3: TButton
      Left = 273
      Top = 8
      Width = 85
      Height = 25
      Caption = '&4.'#21024#38500#36335#24452
      TabOrder = 2
      OnClick = Button3Click
    end
    object Button4: TButton
      Left = 184
      Top = 8
      Width = 85
      Height = 25
      Caption = '&3.'#22686#21152#36335#24452
      TabOrder = 3
      OnClick = Button4Click
    end
    object Button5: TButton
      Left = 451
      Top = 8
      Width = 85
      Height = 25
      Caption = '&6.'#20445#23384#24182#20851#38381
      TabOrder = 4
      OnClick = Button5Click
    end
    object Button6: TButton
      Left = 362
      Top = 8
      Width = 85
      Height = 25
      Caption = '&5.'#20445#23384#25968#25454
      TabOrder = 5
      OnClick = Button6Click
    end
    object Button7: TButton
      Left = 540
      Top = 8
      Width = 85
      Height = 25
      Caption = '&7.'#36864#20986
      TabOrder = 6
      OnClick = Button7Click
    end
  end
  object OpenDialog1: TOpenDialog
    Filter = '*.lfw|*.lfw'
    Left = 184
    Top = 120
  end
  object SaveDialog1: TSaveDialog
    Left = 336
    Top = 224
  end
end
