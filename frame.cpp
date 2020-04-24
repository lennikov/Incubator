//---------------------------------------------------------------------------

#include <vcl.h>
#include <IniFiles.hpp>
#pragma hdrstop

#include "frame.h"
#include "WorkThread.h"
#include "chart.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFrmIncubator *FrmIncubator;
//---------------------------------------------------------------------------
__fastcall TFrmIncubator::TFrmIncubator(TComponent* Owner) : TFrame(Owner)
{
Parent = (TWinControl*)Owner;
wt = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFrmIncubator::PCaptionDblClick(TObject *Sender)
{
UnicodeString s = PCaption->Caption;
if(InputQuery(L"New name", L"Name", s))
{
PCaption->Caption = s;
SaveToFile();
}

}
//------------------------------------------------------------------------------
void __fastcall TFrmIncubator::SaveToFile()
{
TIniFile* f = new TIniFile(".//" + Name + ".cfg");
try
{
f->WriteString("common", "Caption", PCaption->Caption);
f->WriteInteger("common", "comport", CBport->ItemIndex);
f->WriteInteger("common", "baud", CB_baud->ItemIndex);
f->WriteString("Thereslold", "TemperatureLow",  Ed_tt_low->Text);
f->WriteString("Thereslold", "TemperatureMax",  Ed_tt_max->Text);
f->WriteString("Thereslold", "CO2Low",  Ed_tco_low->Text);
f->WriteString("Thereslold", "CO2Max",  Ed_tco_max->Text);

}__finally{delete f;}

}
//------------------------------------------------------------------------------
void __fastcall TFrmIncubator::LoadFromFile()
{
TIniFile* f = new TIniFile(".//" + Name + ".cfg");
try
{
PCaption->Caption = f->ReadString("common", "Caption", Name);
CBport->ItemIndex = f->ReadInteger("common", "comport", 0);
CB_baud->ItemIndex = f->ReadInteger("common", "baud", 5);
Ed_tt_low->Text = f->ReadString("Thereslold", "TemperatureLow", "");
Ed_tt_max->Text = f->ReadString("Thereslold", "TemperatureMax",  "");
Ed_tco_low->Text = f->ReadString("Thereslold", "CO2Low", "");
Ed_tco_max->Text = f->ReadString("Thereslold", "CO2Max",  "");

}__finally{delete f;}

}
void __fastcall TFrmIncubator::Timer1Timer(TObject *Sender)
{
if(wt)
{
if(wt->Connected) {PanelC->Color = clGreen; PanelC->Caption = "OK";}
			else  {PanelC->Color = clRed;   PanelC->Caption = "Error";}
PanelT->Caption = wt->temperature;
PanelCO2->Caption = wt->CO2;
PanelW->Caption = wt->water;
if(wt->A_T) PanelT->Font->Color = clRed; else PanelT->Font->Color = clBlack;
if(wt->A_CO2) PanelCO2->Font->Color = clRed; else PanelCO2->Font->Color = clBlack;
if(wt->A_W) PanelW->Color = clRed; else PanelW->Color = clGreen;
}
else {PanelC->Color = clRed; PanelC->Caption = "Error";}
}
//------------------------------------------------------------------------------
void __fastcall TFrmIncubator::Start()
{
if(!wt) {
//		  int baud = 57600;
//		  try{baud = CB_baud->Text.ToInt();} catch(...){baud = 57600;}
		  wt = new TWorkThread(this);
		  wt->OnTerminate = OnTerminate;
		}
}
//------------------------------------------------------------------------------
void __fastcall TFrmIncubator::OnTerminate(TObject *Sender)
{
wt = NULL;
}
//------------------------------------------------------------------------------
void __fastcall TFrmIncubator::Stop()
{
if(wt) wt->Terminate();
}
//------------------------------------------------------------------------------
void __fastcall TFrmIncubator::SpeedButton1Click(TObject *Sender)
{
FrmChart->Caption = PCaption->Caption;
FrmChart->ID = IntToStr(this->ID);
FrmChart->ShowGraph(FrmChart->DatePicker->Date);
FrmChart->Show();
}
//---------------------------------------------------------------------------

