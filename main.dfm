object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'incubator controller'
  ClientHeight = 720
  ClientWidth = 1316
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 0
    Top = 27
    Width = 1316
    Height = 693
    ActivePage = TabSheet1
    Align = alClient
    TabOrder = 0
    TabWidth = 100
    object TabSheet1: TTabSheet
      Caption = 'Incubators'
      object PanelP: TPanel
        Left = 0
        Top = 0
        Width = 1308
        Height = 665
        Align = alClient
        BevelOuter = bvNone
        Color = clGray
        ParentBackground = False
        TabOrder = 0
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Settings'
      ImageIndex = 1
      object PageControl2: TPageControl
        Left = 0
        Top = 0
        Width = 1308
        Height = 665
        ActivePage = TabSheet4
        Align = alClient
        TabOrder = 0
        TabWidth = 110
        object TabSheet4: TTabSheet
          Caption = 'Basic'
          object Label6: TLabel
            Left = 4
            Top = 16
            Width = 107
            Height = 13
            Caption = 'Incubators to monitor '
          end
          object Label2: TLabel
            Left = 4
            Top = 41
            Width = 125
            Height = 13
            Caption = 'Monitoring frequency, sec'
          end
          object Label4: TLabel
            Left = 4
            Top = 67
            Width = 99
            Height = 13
            Caption = 'Save frequency, min'
          end
          object Label5: TLabel
            Left = 4
            Top = 93
            Width = 117
            Height = 13
            Caption = 'Keep data archive, days'
          end
          object Ed_Incubators_count: TEdit
            Left = 140
            Top = 13
            Width = 45
            Height = 21
            TabOrder = 0
            Text = 'Ed_Incubators_count'
          end
          object Ed_rf: TEdit
            Left = 140
            Top = 38
            Width = 45
            Height = 21
            TabOrder = 1
            Text = 'Ed_rf'
          end
          object Ed_arh_days: TEdit
            Left = 140
            Top = 90
            Width = 45
            Height = 21
            TabOrder = 2
            Text = 'Ed_rf'
          end
          object Ed_sf: TComboBox
            Left = 140
            Top = 64
            Width = 45
            Height = 21
            Style = csDropDownList
            ItemIndex = 1
            TabOrder = 3
            Text = '10'
            Items.Strings = (
              '5'
              '10'
              '20'
              '30'
              '60')
          end
        end
        object TabSheet5: TTabSheet
          Caption = 'Alarm waiting time'
          ImageIndex = 1
          object Label7: TLabel
            Left = 4
            Top = 47
            Width = 159
            Height = 13
            Caption = 'Low-temp alarm waiting time, min'
          end
          object Label8: TLabel
            Left = 4
            Top = 70
            Width = 164
            Height = 13
            Caption = 'High-temp alarm waiting time, min '
          end
          object Label10: TLabel
            Left = 4
            Top = 93
            Width = 183
            Height = 13
            Caption = 'Low CO2 level alarm waiting time, min '
          end
          object Label13: TLabel
            Left = 4
            Top = 116
            Width = 159
            Height = 13
            Caption = 'No water alarm waiting time, min '
          end
          object Label14: TLabel
            Left = 3
            Top = 138
            Width = 181
            Height = 13
            Caption = 'Lost connection to the incubator, min '
          end
          object Label16: TLabel
            Left = 4
            Top = 8
            Width = 349
            Height = 29
            AutoSize = False
            Caption = 
              'The condition above or below threshold must persist for the set ' +
              'number of minutes to trigger the email notification. '
            WordWrap = True
          end
          object Ed_t_low: TEdit
            Left = 198
            Top = 44
            Width = 45
            Height = 21
            TabOrder = 0
            Text = 'Ed_rf'
          end
          object Ed_t_hight: TEdit
            Left = 199
            Top = 67
            Width = 45
            Height = 21
            TabOrder = 1
            Text = 'Ed_rf'
          end
          object Ed_co2_low: TEdit
            Left = 199
            Top = 90
            Width = 45
            Height = 21
            TabOrder = 2
            Text = 'Ed_rf'
          end
          object Ed_w_low: TEdit
            Left = 199
            Top = 113
            Width = 45
            Height = 21
            TabOrder = 3
            Text = 'Ed_rf'
          end
          object Ed_noconect: TEdit
            Left = 198
            Top = 135
            Width = 45
            Height = 21
            TabOrder = 4
            Text = 'Ed_rf'
          end
        end
        object TabSheet6: TTabSheet
          Caption = 'EMail'
          ImageIndex = 2
          object Label1: TLabel
            Left = 2
            Top = 110
            Width = 412
            Height = 13
            Caption = 
              'Email address list (separator '#8220';'#8221' without space i.e. email1@mail' +
              '.com;email2@mail.com)'
          end
          object SpeedButton4: TSpeedButton
            Left = 289
            Top = 3
            Width = 91
            Height = 22
            Caption = 'Test EMail'
            OnClick = SpeedButton4Click
          end
          object Label15: TLabel
            Left = 271
            Top = 33
            Width = 143
            Height = 39
            Caption = 
              'Tip: Check your spam folder; alarm notification may initially be' +
              ' recognized as spam. '
            WordWrap = True
          end
          object Label17: TLabel
            Left = 3
            Top = 150
            Width = 427
            Height = 24
            AutoSize = False
            Caption = 
              'Tip: You can use your cellphone provider email to text service t' +
              'o send a text notification to your cellphone. '
            WordWrap = True
          end
          object Panel9: TPanel
            Left = 0
            Top = 0
            Width = 254
            Height = 107
            Alignment = taLeftJustify
            BevelOuter = bvNone
            BorderStyle = bsSingle
            Caption = ' Email account'
            Ctl3D = False
            ParentCtl3D = False
            TabOrder = 0
            VerticalAlignment = taAlignTop
            object Label3: TLabel
              Left = 4
              Top = 19
              Width = 25
              Height = 13
              Caption = 'Login'
            end
            object Label9: TLabel
              Left = 4
              Top = 41
              Width = 46
              Height = 13
              Caption = 'Password'
            end
            object Label11: TLabel
              Left = 4
              Top = 63
              Width = 57
              Height = 13
              Caption = 'smtp server'
            end
            object Label12: TLabel
              Left = 4
              Top = 85
              Width = 40
              Height = 13
              Caption = 'TSL port'
            end
            object Ed_smtp_login: TEdit
              Left = 67
              Top = 17
              Width = 177
              Height = 19
              TabOrder = 0
            end
            object Ed_smtp_password: TEdit
              Left = 67
              Top = 39
              Width = 177
              Height = 19
              TabOrder = 1
            end
            object Ed_smtp_server: TEdit
              Left = 67
              Top = 61
              Width = 177
              Height = 19
              TabOrder = 2
              Text = 'smtp.gmail.com'
            end
            object Ed_smtp_port: TEdit
              Left = 67
              Top = 83
              Width = 69
              Height = 19
              TabOrder = 3
              Text = '587'
            end
          end
          object Ed_mailto: TEdit
            Left = 2
            Top = 125
            Width = 412
            Height = 21
            TabOrder = 1
            Text = 'Ed_mailto'
          end
          object ComboBox1: TComboBox
            Left = 3
            Top = 179
            Width = 417
            Height = 21
            ItemIndex = 0
            TabOrder = 2
            Text = 'AT&T'#9'10digitphonenumber@txt.att.net'
            Items.Strings = (
              'AT&T'#9'10digitphonenumber@txt.att.net'
              'Carolina West Wireless'#9'10digit10digitnumber@cwwsms.com'
              'Cellular One'#9'10digitphonenumber@mobile.celloneusa.com'
              'Illinois Valley Cellular'#9'10digitphonenumber@ivctext.com'
              'Inland Cellular Telephone'#9'10digitphonenumber@inlandlink.com'
              'Sprint'#9'10digitphonenumber@messaging.sprintpcs.com'
              'T-Mobile'#9'10digitphonenumber@tmomail.net'
              'US Cellular'#9'10digitphonenumber@email.uscc.net'
              'Verizon'#9'10digitphonenumber@vtext.com'
              'Virgin Mobile'#9'10digitphonenumber@vmobl.com')
          end
        end
      end
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 1316
    Height = 27
    Align = alTop
    BevelOuter = bvNone
    Color = clSilver
    Ctl3D = False
    ParentBackground = False
    ParentCtl3D = False
    TabOrder = 1
    DesignSize = (
      1316
      27)
    object Btn_run: TSpeedButton
      Left = 7
      Top = 1
      Width = 100
      Height = 24
      Caption = 'Start Monitoring'
      OnClick = Btn_runClick
    end
    object SpeedButton1: TSpeedButton
      Left = 1289
      Top = 1
      Width = 24
      Height = 24
      Anchors = [akTop, akRight]
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000130B0000130B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
        333333333333333FF3FF3333333333CC30003333333333773777333333333C33
        3000333FF33337F33777339933333C3333333377F33337F3333F339933333C33
        33003377333337F33377333333333C333300333F333337F33377339333333C33
        3333337FF3333733333F33993333C33333003377FF33733333773339933C3333
        330033377FF73F33337733339933C33333333FF377F373F3333F993399333C33
        330077F377F337F33377993399333C33330077FF773337F33377399993333C33
        33333777733337F333FF333333333C33300033333333373FF7773333333333CC
        3000333333333377377733333333333333333333333333333333}
      NumGlyphs = 4
      OnClick = SpeedButton1Click
    end
    object cb_send_email: TCheckBox
      Left = 126
      Top = 5
      Width = 133
      Height = 17
      Caption = 'Enable e-mail alarms'
      Checked = True
      State = cbChecked
      TabOrder = 0
    end
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 28
    Top = 427
  end
end
