object FrmIncubator: TFrmIncubator
  Left = 0
  Top = 0
  Width = 425
  Height = 208
  Color = 14612478
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'Tahoma'
  Font.Style = []
  ParentBackground = False
  ParentColor = False
  ParentFont = False
  TabOrder = 0
  object Label1: TLabel
    Left = 8
    Top = 44
    Width = 54
    Height = 16
    Caption = 'COM port'
  end
  object Label2: TLabel
    Left = 165
    Top = 44
    Width = 28
    Height = 16
    Caption = 'Baud'
  end
  object Label3: TLabel
    Left = 1
    Top = 87
    Width = 94
    Height = 16
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'Temperature '#176'C'
  end
  object Label4: TLabel
    Left = 1
    Top = 115
    Width = 94
    Height = 16
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'CO2 %'
  end
  object Label5: TLabel
    Left = 1
    Top = 143
    Width = 94
    Height = 16
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'Water'
  end
  object Label6: TLabel
    Left = 250
    Top = 140
    Width = 109
    Height = 16
    Caption = 'Door open too long'
    Visible = False
  end
  object Label7: TLabel
    Left = 1
    Top = 173
    Width = 94
    Height = 16
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'Connect'
  end
  object Label8: TLabel
    Left = 174
    Top = 87
    Width = 246
    Height = 16
    Caption = 'Alarm low                '#176'C  high                '#176'C'
  end
  object Label9: TLabel
    Left = 174
    Top = 115
    Width = 246
    Height = 16
    Caption = 'Alarm low                %  high                 %'
  end
  object SpeedButton1: TSpeedButton
    Left = 356
    Top = 176
    Width = 62
    Height = 24
    Caption = 'Chart'
    OnClick = SpeedButton1Click
  end
  object PCaption: TPanel
    Left = 0
    Top = 0
    Width = 425
    Height = 31
    Align = alTop
    BevelOuter = bvNone
    Color = 13816530
    ParentBackground = False
    TabOrder = 0
    OnDblClick = PCaptionDblClick
  end
  object CB_baud: TComboBox
    Left = 199
    Top = 41
    Width = 68
    Height = 24
    Style = csDropDownList
    ItemIndex = 5
    TabOrder = 1
    Text = '57600'
    Items.Strings = (
      '9600'
      '14400'
      '19200'
      '38400'
      '56000'
      '57600'
      '115200'
      '128000')
  end
  object PanelT: TPanel
    Left = 101
    Top = 83
    Width = 67
    Height = 25
    BevelOuter = bvNone
    BorderStyle = bsSingle
    Caption = '+000.00'
    Ctl3D = False
    ParentCtl3D = False
    TabOrder = 2
  end
  object PanelCO2: TPanel
    Left = 101
    Top = 111
    Width = 67
    Height = 25
    BevelOuter = bvNone
    BorderStyle = bsSingle
    Caption = '+000.00'
    Ctl3D = False
    ParentCtl3D = False
    TabOrder = 3
  end
  object PanelW: TPanel
    Left = 101
    Top = 139
    Width = 67
    Height = 25
    BevelOuter = bvNone
    BorderStyle = bsSingle
    Caption = 'OK'
    Color = clSilver
    Ctl3D = False
    ParentBackground = False
    ParentCtl3D = False
    TabOrder = 4
  end
  object PanelD: TPanel
    Left = 365
    Top = 137
    Width = 39
    Height = 25
    BevelOuter = bvNone
    BorderStyle = bsSingle
    Color = clSilver
    Ctl3D = False
    ParentBackground = False
    ParentCtl3D = False
    TabOrder = 5
    Visible = False
  end
  object CBport: TComboBox
    Left = 71
    Top = 41
    Width = 73
    Height = 24
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 6
    Text = 'COM1'
    Items.Strings = (
      'COM1'
      'COM2'
      'COM3'
      'COM4'
      'COM5'
      'COM6'
      'COM7'
      'COM8'
      'COM9'
      'COM10'
      'COM11'
      'COM12'
      'COM13'
      'COM14'
      'COM15'
      'COM16'
      'COM17'
      'COM18'
      'COM19'
      'COM20')
  end
  object PanelC: TPanel
    Left = 101
    Top = 169
    Width = 67
    Height = 25
    BevelOuter = bvNone
    BorderStyle = bsSingle
    Caption = 'OK'
    Color = clSilver
    Ctl3D = False
    ParentBackground = False
    ParentCtl3D = False
    TabOrder = 7
  end
  object Ed_tt_low: TEdit
    Left = 236
    Top = 84
    Width = 57
    Height = 24
    TabOrder = 8
    Text = '+000.00'
  end
  object Ed_tt_max: TEdit
    Left = 346
    Top = 84
    Width = 57
    Height = 24
    TabOrder = 9
    Text = '+000.00'
  end
  object Ed_tco_max: TEdit
    Left = 346
    Top = 112
    Width = 57
    Height = 24
    TabOrder = 10
    Text = '+000.00'
  end
  object Ed_tco_low: TEdit
    Left = 236
    Top = 112
    Width = 57
    Height = 24
    TabOrder = 11
    Text = '+000.00'
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 192
    Top = 144
  end
end
