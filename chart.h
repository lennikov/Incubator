//---------------------------------------------------------------------------

#ifndef chartH
#define chartH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFrmChart : public TForm
{
__published:	// IDE-managed Components
	TChart *ChartT;
	TLineSeries *LineSeries23;
	TPanel *PanelTop;
	TDateTimePicker *DatePicker;
	TLabel *Label1;
	TSpeedButton *SpeedButton1;
	TSpeedButton *SpeedButton2;
	TChart *ChartCO2;
	TLineSeries *LineSeries1;
	void __fastcall FormResize(TObject *Sender);
	void __fastcall SpeedButton3Click(TObject *Sender);
	void __fastcall SpeedButton2Click(TObject *Sender);
	void __fastcall SpeedButton1Click(TObject *Sender);
	void __fastcall DatePickerChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
	AnsiString ID;
	void __fastcall ShowGraph(TDateTime dt);
	void __fastcall AddToGraph(TDateTime dt, AnsiString v1, AnsiString v2);
	__fastcall TFrmChart(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrmChart *FrmChart;
//---------------------------------------------------------------------------
#endif
