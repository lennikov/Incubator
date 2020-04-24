//---------------------------------------------------------------------------

#include <vcl.h>
#include <DateUtils.hpp>
#pragma hdrstop

#include "chart.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFrmChart *FrmChart;
//---------------------------------------------------------------------------
__fastcall TFrmChart::TFrmChart(TComponent* Owner) : TForm(Owner)
{
DatePicker->Date = Date();
DatePicker->Time = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFrmChart::FormResize(TObject *Sender)
{
ChartT->Height = (ClientHeight - PanelTop->Height)/2-4;
}
//---------------------------------------------------------------------------
void __fastcall TFrmChart::SpeedButton3Click(TObject *Sender)
{
ShowGraph(DatePicker->Date);
}
//---------------------------------------------------------------------------
void __fastcall TFrmChart::ShowGraph(TDateTime dt)
{
WORD Y;
WORD M;
WORD D;
DecodeDate(dt, Y, M, D);
AnsiString fn = IntToStr(Y) + "-";
if(M < 10) fn = fn + "0";
fn = fn + IntToStr(M) + "-";
if(D < 10) fn = fn + "0";
fn = fn + IntToStr(D) + ".csv";
fn = ".\\DATA\\Frm_" + ID + "\\" + fn;

ChartT->Series[0]->Clear();
ChartCO2->Series[0]->Clear();

if(!FileExists(fn)) return;
TStringList* sl = new TStringList;
sl->LoadFromFile(fn);
for(int i = 0; i < sl->Count; ++i)
{
AnsiString s = Trim(sl->Strings[i]);
if(s == "") continue;
int pos = s.Pos(":");
AnsiString ss = s.SubString(1, pos-1);
s = s.Delete(1, pos);
int h = 0;
try{h = ss.ToInt();}catch(...){h = 0;}

pos = s.Pos(";");
ss = s.SubString(1, pos-1);
s = s.Delete(1, pos);
int m = 0;
try{m = ss.ToInt();}catch(...){m = 0;}

pos = s.Pos(";");
ss = s.SubString(1, pos-1);
s = s.Delete(1, pos);
float v = 0;
try{v = ss.ToDouble();}catch(...){v = 0;}

ss = s;
float v2 = 0;
try{v2 = ss.ToDouble();}catch(...){v2 = 0;}

if(v > 0)  ChartT->Series[0]->AddXY(h+float(m)/60.0, v);
if(v2 > 0) ChartCO2->Series[0]->AddXY(h+float(m)/60.0, v2);
}

}

//---------------------------------------------------------------------------
void __fastcall TFrmChart::SpeedButton2Click(TObject *Sender)
{
DatePicker->Date = IncDay(DatePicker->Date, 1);
ShowGraph(DatePicker->Date);
}
//---------------------------------------------------------------------------

void __fastcall TFrmChart::SpeedButton1Click(TObject *Sender)
{
DatePicker->Date = IncDay(DatePicker->Date, -1);
ShowGraph(DatePicker->Date);
}
//---------------------------------------------------------------------------

void __fastcall TFrmChart::DatePickerChange(TObject *Sender)
{
ShowGraph(DatePicker->Date);
}
//---------------------------------------------------------------------------
void __fastcall TFrmChart::AddToGraph(TDateTime dt, AnsiString V1, AnsiString V2)
{
if(!this->Showing) return;
if( DatePicker->Date != Date()) return; // только если выбраны текущие сутки
int h = HourOf(dt);
int m = MinuteOf(dt);

float v1 = 0;
try{v1 = V1.ToDouble();}catch(...){v1 = 0;}


float v2 = 0;
try{v2 = V2.ToDouble();}catch(...){v2 = 0;}

if(v1 > 0) ChartT->Series[0]->AddXY(h+float(m)/60.0, v1);
if(v2 > 0) ChartCO2->Series[0]->AddXY(h+float(m)/60.0, v2);

}
