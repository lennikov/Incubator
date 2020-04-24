//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdExplicitTLSClientServerBase.hpp>
#include <IdMessageClient.hpp>
#include <IdSMTP.hpp>
#include <IdSMTPBase.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>

class TWorkThread;
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TPanel *PanelP;
	TTimer *Timer1;
	TPanel *Panel1;
	TSpeedButton *Btn_run;
	TSpeedButton *SpeedButton1;
	TCheckBox *cb_send_email;
	TPageControl *PageControl2;
	TTabSheet *TabSheet4;
	TTabSheet *TabSheet5;
	TTabSheet *TabSheet6;
	TPanel *Panel9;
	TLabel *Label3;
	TLabel *Label9;
	TLabel *Label11;
	TLabel *Label12;
	TEdit *Ed_smtp_login;
	TEdit *Ed_smtp_password;
	TEdit *Ed_smtp_server;
	TEdit *Ed_smtp_port;
	TLabel *Label1;
	TEdit *Ed_mailto;
	TLabel *Label6;
	TEdit *Ed_Incubators_count;
	TLabel *Label2;
	TEdit *Ed_rf;
	TLabel *Label4;
	TLabel *Label5;
	TEdit *Ed_arh_days;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label10;
	TLabel *Label13;
	TLabel *Label14;
	TEdit *Ed_t_low;
	TEdit *Ed_t_hight;
	TEdit *Ed_co2_low;
	TEdit *Ed_w_low;
	TEdit *Ed_noconect;
	TSpeedButton *SpeedButton4;
	TLabel *Label15;
	TLabel *Label16;
	TLabel *Label17;
	TComboBox *ComboBox1;
	TComboBox *Ed_sf;
	void __fastcall Btn_runClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall SpeedButton1Click(TObject *Sender);
	void __fastcall SpeedButton4Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
private:	// User declarations
	int icnt;
	TWorkThread* wt;
	void __fastcall OnTerminate(TObject *Sender);
	void __fastcall SaveToFile();
	void __fastcall LoadFromFile();
public:		// User declarations
	bool SuspendEmail;
	TDateTime StopSuspendEmail;

	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
