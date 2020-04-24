//---------------------------------------------------------------------------

#ifndef frameH
#define frameH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class TWorkThread;
class TFrmIncubator : public TFrame
{
__published:	// IDE-managed Components
	TPanel *PCaption;
	TLabel *Label1;
	TComboBox *CB_baud;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TPanel *PanelT;
	TPanel *PanelCO2;
	TPanel *PanelW;
	TLabel *Label6;
	TPanel *PanelD;
	TComboBox *CBport;
	TLabel *Label7;
	TPanel *PanelC;
	TLabel *Label8;
	TEdit *Ed_tt_low;
	TEdit *Ed_tt_max;
	TLabel *Label9;
	TEdit *Ed_tco_max;
	TEdit *Ed_tco_low;
	TTimer *Timer1;
	TSpeedButton *SpeedButton1;
	void __fastcall PCaptionDblClick(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall SpeedButton1Click(TObject *Sender);
private:	// User declarations
	TWorkThread* wt;
	void __fastcall OnTerminate(TObject *Sender);
public:		// User declarations
	int ID;
	__fastcall TFrmIncubator(TComponent* Owner);
	void __fastcall SaveToFile();
	void __fastcall LoadFromFile();
	void __fastcall Start();
	void __fastcall Stop();

};
//---------------------------------------------------------------------------
extern PACKAGE TFrmIncubator *FrmIncubator;
//---------------------------------------------------------------------------
#endif
