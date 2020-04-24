//---------------------------------------------------------------------------

#include <System.hpp>
#include <DateUtils.hpp>
#include <stdio.h>
#include <System.SysUtils.hpp>
#pragma hdrstop
#include "WorkThread.h"
#include "comport.h"
#include "frame.h"
#include "main.h"
#include "SendEmail.h"
#include "chart.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TWorkThread::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

int TIMEOUT = 1000*3; // 3 cек

//void _fastcall WriteLog(char* c, int len)
//{
//FILE *f;
//f = fopen(AnsiString(".\\LOG\\work_1.log").c_str(), "a");
//if(f == NULL) return;
//try
// {
//
//	fwrite(c, len, 1, f);
// }
//__finally {
//			fclose(f);
//		  }
//}

//------------------------------------------------------------------------------
void _fastcall TWorkThread::WriteArhiveFile(AnsiString s)
{
WORD Y;
WORD M;
WORD D;
DecodeDate(Now(), Y, M, D);
AnsiString fn = IntToStr(Y) + "-";
if(M < 10) fn = fn + "0";
fn = fn + IntToStr(M) + "-";
if(D < 10) fn = fn + "0";
fn = fn + IntToStr(D) + ".csv";
fn = ".\\DATA\\Frm_" + IntToStr(Incubator->ID) + "\\" + fn;

FILE *f;
f = fopen(AnsiString(fn).c_str(), "a");
if(f == NULL) return;
try
 {
  if(s == "") s = "\n";
  else s = s + "\n";
  fwrite(s.c_str(), s.Length(), 1, f);
 }
__finally {
			fclose(f);
		  }
}

//------------------------------------------------------------------------------
void _fastcall TWorkThread::WriteLogFile(AnsiString s)
{
if(!FileExists(".\\LOG")) ForceDirectories(".\\LOG");
FILE *f;
f = fopen(AnsiString(".\\LOG\\work_" + IntToStr(Incubator->ID) + ".log").c_str(), "a");
if(f == NULL) return;
try
 {
  if(s == "") s = "\n";
  else s = DateTimeToStr(Now()) + "\n" + s+ "\n"; // временно уберем
  fwrite(s.c_str(), s.Length(), 1, f);
 }
__finally {
			fclose(f);
		  }
}

//------------------------------------------------------------------------------
void _fastcall TWorkThread::WriteAlarmOrder(AnsiString s)
{
if(!FileExists(".\\LOG")) ForceDirectories(".\\LOG");
FILE *f;
f = fopen(AnsiString(".\\LOG\\alarm_" + IntToStr(Incubator->ID) + ".txt").c_str(), "a");
if(f == NULL) return;
try
 {
  if(s == "") s = "\n";
  else s = DateTimeToStr(Now()) + "\n" + s+ "\n"; // временно уберем
  fwrite(s.c_str(), s.Length(), 1, f);
 }
__finally {
			fclose(f);
		  }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
__fastcall TWorkThread::TWorkThread(TFrmIncubator* incubator) : TThread(false)
{
this->FreeOnTerminate = true;
Incubator = incubator;
baud = 57600;
try{baud = Incubator->CB_baud->Text.ToInt();} catch(...){baud = 57600;}

//baud = Baud;
Connected = false;
//cnterror = 0;
AnsiString s = Incubator->CBport->Text;
s = s.SubString(4, s.Length()-3);
if(s.Length() > 1) s = "\\\\.\\" + Incubator->CBport->Text;
else s = Incubator->CBport->Text;
COMPORT = s;

try{READFREQUENCY = Form1->Ed_rf->Text.ToInt();} catch(...) {READFREQUENCY = 30;}
try{SAVEFREQUENCY = Form1->Ed_sf->Text.ToInt();} catch(...) {SAVEFREQUENCY = 5;}


try{T_low_wait     = StrToInt(Form1->Ed_t_low->Text);}   catch(...) {T_low_wait = 15;}
try{T_hight_wait   = StrToInt(Form1->Ed_t_hight->Text);} catch(...) {T_hight_wait = 5;}
try{CO2_low_wait   = StrToInt(Form1->Ed_co2_low->Text);} catch(...) {CO2_low_wait = 5;}
try{water_low_wait = StrToInt(Form1->Ed_w_low->Text);}   catch(...) {water_low_wait = 5;}
try{T_noconect = StrToInt(Form1->Ed_noconect->Text);}   catch(...) {T_noconect = 5;}


try{T_E_low   = myStrToFloat(Incubator->Ed_tt_low->Text);} catch(...) {T_E_low = 36.0;}
try{T_E_hight = myStrToFloat(Incubator->Ed_tt_max->Text);} catch(...) {T_E_hight = 37.5;}
try{CO2_E_low = myStrToFloat(Incubator->Ed_tco_low->Text);} catch(...){CO2_E_low = 4.0;}
try{CO2_E_hight = myStrToFloat(Incubator->Ed_tco_max->Text);} catch(...){CO2_E_hight = 5.0;}
W_E_low = 0.01; // !!!!!!!!!!!!!!!!   нет воды наверно 000.00 будет

A_T   = false;
A_CO2 = false;
A_W   = true;
A_C   = true;

send_A1 = false;
send_A2 = false;
send_A3 = false;
send_A4 = false;
}
//------------------------------------------------------------------------------
void __fastcall TWorkThread::Execute()
{
Sleep(1000*(Incubator->ID-1)*5);  //певый без задержки оттальные по 5 сек увеличивают задержку
Connected = false;
WriteLogFile("- - - S T A R T - - - ID = " + Incubator->ID);

com = new TctpSerialPort;
com->Open(COMPORT);
if(!com->Opened) {
					  WriteLogFile("ѕорт не открываетс€ " + COMPORT);
					  WriteLogFile("STOP");
					  com = NULL;
					  return;
					}
com->SetBaud(baud);


AnsiString Result = "";
TDateTime lastread = 0;
last_save = 0;
while(!Terminated)
{

if(SecondsBetween(Now(), lastread) < READFREQUENCY) {Sleep(100); continue;}
lastread = Now();
Connected = ReadTemperature();
Connected = ReadCO2();
Connected = ReadWater();

if(!Connected)
{
 if(!A_C)	{					// если еще не взвели флажек - взводим
			 A_C = true;
			 start_C = Now();
			}
   else		{
			 if(MinutesBetween(Now(), start_C) >= T_noconect) // прошло достаточно времени вызываем аварию
			   {
				 if(!send_A4) SendAlarm("Lost connect");
			   }
			}
}
else {
		A_C = false;  send_A4 = false;
		SaveValue();
//		if((FrmChart->Active)&&
		if(FrmChart->ID == IntToStr(Incubator->ID))
		{
			FrmChart->AddToGraph(Time(), temperature, CO2);
		}
	 }
}

if(com)
	{
	  com->Close();
	  delete com;
	}


WriteLogFile("STOP");
}
//---------------------------------------------------------------------------
char __fastcall TWorkThread::bintohex(char a)
{
char hexa[16] = "0123456789abcdef";
if(a < 0) return 0;
if(a > 15) return 'F';
return hexa[a];
}
//---------------------------------------------------------------------------
char __fastcall TWorkThread::ahex(char a)
{
char hexa[16] = "0123456789abcdef";
for (int i = 0; i < 16; i++)
if (a == hexa[i]) return (i);
return 0;
}

//---------------------------------------------------------------------------
void __fastcall TWorkThread::Parce(AnsiString s)
{
s = Trim(s);
if(s == "") return;
while((s.Length() > 0)&&(s[1] != '!')) s = s.Delete(1,1);
if(s == "") return;
int pos = s.Pos(":");
s = s.Delete(1,pos);
pos = s.Pos(":");
AnsiString adr = s.SubString(1, pos-1);
if(!((adr == "2300")||(adr == "2301"))) return;
s = s.Delete(1,pos);
pos = s.Pos(":");
AnsiString ss = s.SubString(1, pos-1);
int len = 0;
if(ss.Length() == 2)
{
len = ahex(ss[1])*16 + ahex(ss[2]);
}
int dp = (len)/22;
s = s.Delete(1,pos);

while(dp >0)
{
pos = 22;
ss = s.SubString(1, pos);
Parce2(ss);
s = s.Delete(1,pos);
dp--;
}

}
//-----------------------------------------------------------------------------
void __fastcall TWorkThread::Parce2(AnsiString s)
{
if(s.Length() != 22) return;
AnsiString type = s.SubString(1,2);
AnsiString val = s.SubString(19,4);
AnsiString ss = s;
if(type == "00") { ss = ss + " Temperature control loop -> ";
				  if(val == "0002") ss = ss + "Actual value high";
				  if(val == "0004") ss = ss + "Actual value low";
				}

else if(type == "01") { ss = ss + " CO2 control loop -> ";
				  if(val == "0002") ss = ss + "Actual value high";
				  if(val == "0004") ss = ss + "Actual value low";
				}
else if(type == "02") ss = ss + " O2 control loop";

else if(type == "07") { ss = ss + " Water level -> ";
				  if(val == "0001") ss = ss + "No water";
				}
else if(type == "08") {
						ss = "General device status -> ";
						if(val == "0002")
						{
						  ss = ss + "Device door open too long";
						  //DOTL = "ALR";
						}
					  }

WriteLogFile(ss);
}

//------------------------------------------------------------------------------
bool __fastcall TWorkThread::ReadTemperature()
{
char str[15]={0};
unsigned char bcc = 0x0FF;
unsigned char bcc2 = 0;
strncpy (&str[0], "?:2010:00::a6\r", 14);
////bcc = (str[0]^str[1]);
//for (int i = 0; i < 11; i ++)
//{
//bcc = ~(bcc^str[i]);
////bcc2 = (bcc2+(str[i]));
//}
//
//str[11] = bintohex(bcc/16);
//str[12] = bintohex(bcc%16);

com->PurgeComBuff();
if(!com->Write(str, 14)) {WriteLogFile("can't WRITE"); return false;}
WriteLogFile("WRITE: " + AnsiString((char*)str));

AnsiString result = com->ReadLine();
// result = "!:2010:17:+037.00;+036.70;+036.86:5d";
WriteLogFile("READ: " + result);
if(result == "") {temperature = "?"; return false;} //{cnterror++; if(cnterror > 5) Connected = false;}
//else {cnterror = 0; Connected = true;}
if(result != "")
{
int pos = result.Pos("2010:");
result = result.Delete(1, pos+4);
pos = result.Pos(":");
result = result.Delete(1, pos);
pos = result.Pos(";");
result = result.Delete(1, pos);
pos = result.Pos(";");
temperature = result.SubString(1, pos-1);
temperature = myStrToStr(temperature);
float n = -1;
try{ n = temperature.ToDouble();} catch(...){n = -1;}
if((n >= 0)&&(n < T_E_low)) // за допустимыми пределами
 {
  if(!A_T)	{					// если еще не взвели флажек - взводим
			 A_T = true;
			 start_T = Now();
			}
   else		{
			 if(MinutesBetween(Now(), start_T) >= T_low_wait) // прошло достаточно времени вызываем аварию
			   {
				 if(!send_A1) SendAlarm("Actual temperature low");
			   }

			}
 }
else if((n >= 0)&&(n > T_E_hight)) // за допустимыми пределами
 {
  if(!A_T)	{					// если еще не взвели флажек - взводим
			 A_T = true;
			 start_T = Now();
			}
   else		{
			 if(MinutesBetween(Now(), start_T) >= T_hight_wait) // прошло достаточно времени вызываем аварию
			   {
				 if(!send_A1) SendAlarm("Actual temperature high");
			   }

			}
 }
else { A_T = false;  send_A1 = false;} // пришло в норму
}
//else temperature = "?";
return true;
}
//------------------------------------------------------------------------------
bool __fastcall TWorkThread::ReadCO2()
{
char str[15]={0};
unsigned char bcc = 0xff;
//CO2 = "?";
strncpy (&str[0], "?:2020:00::c0\r", 14);
//for (int i = 0; i < 11; i ++)
//{
//bcc = (bcc^str[i]);
////bcc = (bcc+str[i]);
//}
//
//str[11] = bintohex(bcc/16);
//str[12] = bintohex(bcc%16);

com->PurgeComBuff();
if(!com->Write(str, 14)) {WriteLogFile("can't WRITE"); return false;}
WriteLogFile("WRITE: " + AnsiString(str));


AnsiString result = com->ReadLine();
// result = "!:2020:0f:+004.00;+004.05:e1";
WriteLogFile("READ: " + result);
//CO2 = result;
if(result == "") {CO2 = "?"; return false;}//{cnterror++; if(cnterror > 5) Connected = false;}
//else {cnterror = 0; Connected = true;}

if(result != "")
{
int pos = result.Pos("2020:");
result = result.Delete(1, pos+4);
pos = result.Pos(":");
result = result.Delete(1, pos);
pos = result.Pos(";");
result = result.Delete(1, pos);
pos = result.Pos(":");
CO2 = result.SubString(1, pos-1);
CO2 = myStrToStr(CO2);
float n = -1;
try{ n = CO2.ToDouble();} catch(...){n = -1;}
if((n >= 0)&&(n < CO2_E_low)) // за допустимыми пределами
 {
  if(!A_CO2){					// если еще не взвели флажек - взводим
			 A_CO2 = true;
			 start_CO2 = Now();
			}
   else		{
			 if(MinutesBetween(Now(), start_CO2) >= CO2_low_wait) // прошло достаточно времени вызываем аварию
			   {
				 if(!send_A2) SendAlarm("Actual CO2 low");
			   }
			}
 }
else if((n >= 0)&&(n > CO2_E_hight)) // за допустимыми пределами
 {
  if(!A_CO2){					// если еще не взвели флажек - взводим
			 A_CO2 = true;
			 start_CO2 = Now();
			}
   else		{
			 if(MinutesBetween(Now(), start_CO2) >= CO2_low_wait) // прошло достаточно времени вызываем аварию
			   {
				 if(!send_A2) SendAlarm("Actual CO2 hight");
			   }
			}
 }
else {A_CO2 = false;  send_A2 = false;} // пришло в норму
}
else CO2 = "?";


}
//------------------------------------------------------------------------------
bool __fastcall TWorkThread::ReadWater()
{
char str[15]={0};
unsigned char bcc = 0xff;

strncpy (&str[0], "?:204a:00::cd\r", 14);
//for (int i = 0; i < 11; i ++)
//{
//bcc = (bcc^str[i]);
////bcc = (bcc+str[i]);
//}
//
//str[11] = bintohex(bcc/16);
//str[12] = bintohex(bcc%16);

com->PurgeComBuff();
if(!com->Write(str, 14)) {WriteLogFile("can't WRITE"); return false;}
WriteLogFile("WRITE: " + AnsiString(str));


AnsiString result = com->ReadLine();
result = "!:204a:07:+001.00:t6";
WriteLogFile("READ: " + result);

if(result == ""){ water = "?"; return false;} //{cnterror++; if(cnterror > 5) Connected = false;}
//else {cnterror = 0; Connected = true;}

if(result != "")
{
int pos = result.Pos("204a:");
result = result.Delete(1, pos+4);
pos = result.Pos(":");
result = result.Delete(1, pos);
pos = result.Pos(":");
AnsiString w = result.SubString(1, pos-1);
float n = -1;
try{ n = myStrToFloat(w);} catch(...){n = -1;}
if((n >= 0)&&(n < W_E_low)) // за допустимыми пределами
 {
  if(!A_W){					// если еще не взвели флажек - взводим
			 A_W = true;
			 start_W = Now();
			}
   else		{
			 if(MinutesBetween(Now(), start_W) >= water_low_wait) // прошло достаточно времени вызываем аварию
			   {
				 if(!send_A3) SendAlarm("No water");
			   }
			}
  water = "low";
 }
else {water = "OK"; A_W = false; send_A3 = false;} // пришло в норму


}
else water = "?";
return true;
}
//------------------------------------------------------------------------------
bool __fastcall TWorkThread::ReadErrorMemory()
{
char str[15]={0};
unsigned char bcc = 0;//0xFF;
s1 = "?";
strncpy (&str[0], "?:2300:00::4c\r", 14);
//for (int i = 0; i < 11; i ++)
//{
////bcc = (bcc^str[i]);
//bcc = (bcc+str[i]);
//}

//str[11] = bintohex(bcc/16);
//str[12] = bintohex(bcc%16);

com->PurgeComBuff();
if(!com->Write(str, 14)) {WriteLogFile("can't WRITE"); return false;}
WriteLogFile("WRITE: " + AnsiString(str));


AnsiString result = com->ReadLine();
WriteLogFile("READ: " + result);
s1 = result;
if(result == "") {return false;} // {cnterror++; if(cnterror > 5) Connected = false;}
// else {cnterror = 0; Connected = true; Parce(s1);}



bcc = 0; // 0xFF;
s2 = "?";
strncpy (&str[0], "?:2301:00::65\r", 14);
//for (int i = 0; i < 11; i ++)
//{
////bcc = (bcc^str[i]);
//bcc = (bcc+str[i]);
//}
//
//str[11] = bintohex(bcc/16);
//str[12] = bintohex(bcc%16);

com->PurgeComBuff();
if(!com->Write(str, 14)) {WriteLogFile("can't WRITE"); return false;}
WriteLogFile("WRITE: " + AnsiString(str));


result = com->ReadLine();
// result = "!:2301:f2:07010214003b370000010007010214010011000001000701021401001a00000100070102140101100000010007010214010117000001000701021401011c000001000701021401013600000100070102140102030000010007010214010221000001000701021401042700000100070102140c061f00000100:d6";
WriteLogFile("READ: " + result);
s2 = result;

if(result == "") return false; //{cnterror++; if(cnterror > 5) Connected = false;}
//else {cnterror = 0; Connected = true; Parce(s2);}

}

AnsiString __fastcall TWorkThread::myStrToStr(AnsiString s)
{
s = Trim(s);
if(s == "") return "";

if(s[1] == '+') s = s.Delete(1,1);
while(s.Length()>0)
{
 if(s[1] == '.') {s = "0" + s;
				  for(int i = 1; i < s.Length(); ++i) if(s[i] == '.') {s[i] = FormatSettings.DecimalSeparator; if(s[i] == ',') s[i] = FormatSettings.DecimalSeparator;}
				  return s.ToDouble();
				 }
 if(s[1] == '0') s = s.Delete(1,1); else break;
}

for(int i = 1; i < s.Length(); ++i) if(s[i] == '.') {s[i] = FormatSettings.DecimalSeparator; if(s[i] == ',') s[i] = FormatSettings.DecimalSeparator;}
return s;
}
//------------------------------------------------------------------------------
float __fastcall TWorkThread::myStrToFloat(AnsiString s)
{
s = myStrToStr(s);
if(s != "") return s.ToDouble();
else return 0;
}

//------------------------------------------------------------------------------
void __fastcall TWorkThread::SendAlarm(AnsiString s)
{
if(!Form1->cb_send_email->Checked) return;

UnicodeString fn = "";
if(s == "Actual temperature low")       { send_A1 = true; fn = "1L.wav";}
else if(s == "Actual temperature high") { send_A1 = true; fn = "1H.wav";}
else if(s == "Actual CO2 low") 			{ send_A2 = true; fn = "2L.wav"; }
else if(s == "Actual CO2 hight")        { send_A2 = true; fn = "2H.wav"; }
else if(s == "No water") 				{ send_A3 = true; fn = "3.wav"; }
else if(s == "Lost connect") 			{ send_A4 = true; fn = "4.wav"; }

AnsiString mailtext = Incubator->PCaption->Caption + ": ";
if(s == "Actual temperature low")        mailtext = mailtext + "Temperature is low (check settings, check door)";
else if(s == "Actual temperature high")  mailtext = mailtext + "Temperature is high (check settings)";
else if(s == "Actual CO2 low")           mailtext = mailtext + "CO2 level is low (check the gas cylinder, check door)";
else if(s == "Actual CO2 hight")         mailtext = mailtext + "CO2 level is high (check settings)";
else if(s == "No water")                 mailtext = mailtext + "Water level is low (add water)";
else if(s == "Lost connect")             mailtext = mailtext + "Connection with the incubator is lost (check power, check cables)";

WriteLogFile(mailtext);

WriteAlarmOrder(mailtext);

// Form1->cb_send_email->Checked = false;
// Form1->StopSuspendEmail = IncMinute(Now(), 60);
// Form1->SuspendEmail = true;



 TSendEmail* sm = new TSendEmail(true);
 sm->MailHost = Form1->Ed_smtp_server->Text;
 sm->MailPort = Form1->Ed_smtp_port->Text.ToInt();
 sm->MailUserName = Form1->Ed_smtp_login->Text;
 sm->MailPassword = Form1->Ed_smtp_password->Text;
 sm->Subject = "Incubator Alarm";
 sm->MessageText = mailtext;
 sm->MailAddresses = Form1->Ed_mailto->Text;
 sm->Resume();

PlaySound(fn.c_str(), NULL, SND_FILENAME|SND_ASYNC);
}

//------------------------------------------------------------------------------
void __fastcall TWorkThread::SaveValue()
{
if(MinutesBetween(Now(), last_save) < SAVEFREQUENCY) return;
int m = MinuteOf(Now());
int h = HourOf(Now());

int mr = m/SAVEFREQUENCY;
mr = mr * SAVEFREQUENCY;
if(mr == m)
{
AnsiString s = "";
if(h < 10) s = s + "0";
s = s + IntToStr(h) + ":";
if(m < 10) s = s + "0";
s = s + IntToStr(m) + ";";
s = s + temperature + ";" + CO2;
last_save = Now();
WriteArhiveFile(s);
}
}
