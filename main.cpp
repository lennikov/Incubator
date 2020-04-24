//---------------------------------------------------------------------------

#include <vcl.h>
#include <DateUtils.hpp>
#include <IniFiles.hpp>
#pragma hdrstop

#include "main.h"
#include "WorkThread.h"
#include "frame.h"
#include "SendEmail.h"
#include "about.h"
#include "DeleteOldFile.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
TFrmIncubator* F[4];
//extern int READFREQUENCY;
//extern int SAVEFREQUENCY;
//------------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner)
{
SuspendEmail = false;
int ind = 0;
if(!FileExists(".\\DATA")) ForceDirectories(".\\DATA");
LoadFromFile();
icnt = 4;
try{icnt = Ed_Incubators_count->Text.ToInt();} catch(...){icnt = 1;}
if(icnt > 9) icnt = 9;
if(icnt < 1) icnt = 1;


for(int i = 0; i < 3; i++)
{
if(ind >= icnt) break;
for(int j = 0; j < 3; j++)
{
	if(ind >= icnt) break;
	F[ind] = new TFrmIncubator(PanelP);
	//F[ind]->Parent = PanelP;
	F[ind]->ID = ind+1;
	F[ind]->Name = "Frm_" + IntToStr(F[ind]->ID);
	F[ind]->PCaption->Caption = "Incubator # " + IntToStr(ind+1);
	F[ind]->Left = 8 + (F[ind]->Width + 10)*j;
	F[ind]->Top = 8 + (F[ind]->Height + 10)*i;
	F[ind]->LoadFromFile();
	if(!FileExists(".\\DATA\\"+F[ind]->Name)) ForceDirectories(".\\DATA\\"+F[ind]->Name);
	ind++;
}
}
if(icnt < 7) Height = Height - F[0]->Height - 10;
if(icnt < 4) Height = Height - F[0]->Height - 10;
if(icnt < 3) Width = Width - F[0]->Width - 10;
if(icnt < 2) Width = Width - F[0]->Width - 10;
PageControl1->ActivePageIndex = 0;

new TDeleteOldFile(Ed_arh_days->Text.ToInt());
}
//------------------------------------------------------------------------------
void __fastcall TForm1::Btn_runClick(TObject *Sender)
{




if(Btn_run->Caption == "Start Monitoring")
  {
	Btn_run->Enabled = false;

	for(int i = 0; i < icnt; i++)
		{
		  F[i]->Start();
		  //break;
		}
//	wt = new TWorkThread(1, Memo1->Lines->Strings[0]);
//	wt->OnTerminate = OnTerminate;

	Btn_run->Caption = "Stop Monitoring";
	Btn_run->Enabled = true;
  }
else
  {
//	Btn_run->Enabled = false;
	Btn_run->Caption = "Start Monitoring";
//	wt->Terminate();
	for(int i = 0; i < icnt; i++)
		{
		 F[i]->Stop();
		}
  }

}
//---------------------------------------------------------------------------
void __fastcall TForm1::OnTerminate(TObject *Sender)
{
 Btn_run->Caption = "Start";
 Btn_run->Enabled = true;

// Edit1->Text = wt->temperature;
// Edit2->Text = wt->CO2;
// Edit3->Text = wt->water;
// Memo2->Text = wt->s1;
// Memo3->Text = wt->s2;
}

//------------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
// SpeedButton1Click(NULL);
SaveToFile();
for(int i = 0; i < icnt; i++)
{
 F[i]->SaveToFile();
}

}
//------------------------------------------------------------------------------

void __fastcall TForm1::SpeedButton1Click(TObject *Sender)
{
Form_about->RichEdit1->Lines->LoadFromFile("about.rtf");
Form_about->Show();
}
//------------------------------------------------------------------------------

void __fastcall TForm1::SpeedButton4Click(TObject *Sender)
{

 TSendEmail* sm = new TSendEmail(true);
 sm->MailHost = Ed_smtp_server->Text;
 sm->MailPort = Ed_smtp_port->Text.ToInt();
 sm->MailUserName = Ed_smtp_login->Text;
 sm->MailPassword = Ed_smtp_password->Text;
 sm->Subject = "Incubator Alarm test";
 sm->MessageText = "This is a drill alarm. No actual issue with incubator(s) is present.";
 sm->MailAddresses = Ed_mailto->Text;
 sm->Resume();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
//if(cb_send_email->Checked)
//{
//  Label_email_timer->Visible = false;
//  SuspendEmail = false;
//}
//else if(SuspendEmail)
//	{
//	  if(Now() >= StopSuspendEmail)
//									{
//									  cb_send_email->Checked = true;
//									  SuspendEmail = false;
//									  Label_email_timer->Visible = false;
//									}
//	 else {
//			  Label_email_timer->Caption = TimeToStr(StopSuspendEmail - Now());
//			  Label_email_timer->Visible = true;
//		  }
//	}
}
//---------------------------------------------------------------------------




void __fastcall TForm1::SaveToFile()
{
TIniFile* f = new TIniFile(".//settings.cfg");
try
{
f->WriteString("common", "Incubators_count",  Ed_Incubators_count->Text);
f->WriteString("common", "READFREQUENCY", Ed_rf->Text);
f->WriteInteger("common", "SAVEFREQUENCY", Ed_sf->ItemIndex);
f->WriteString("common", "arhive_days", Ed_arh_days->Text);

f->WriteString("timer",  "t_low",     Ed_t_low->Text);
f->WriteString("timer",  "t_t_hight", Ed_t_hight->Text);
f->WriteString("timer",  "t_co2_low", Ed_co2_low->Text);
f->WriteString("timer",  "t_w_low", Ed_w_low->Text);
f->WriteString("timer",  "noconect", Ed_noconect->Text);


f->WriteString("Email", "login",       Ed_smtp_login->Text);
f->WriteString("Email", "password",    Ed_smtp_password->Text);
f->WriteString("Email", "smtp_server", Ed_smtp_server->Text);
f->WriteString("Email", "smtp_port",   Ed_smtp_port->Text);
f->WriteString("Email", "mailto",      Ed_mailto->Text);

}__finally{delete f;}

}
//------------------------------------------------------------------------------
void __fastcall TForm1::LoadFromFile()
{
TIniFile* f = new TIniFile(".//settings.cfg");
try
{
Ed_Incubators_count->Text = f->ReadString("common", "Incubators_count", "4");
Ed_rf->Text = f->ReadString("common", "READFREQUENCY", "30");
Ed_sf->ItemIndex = f->ReadInteger("common", "SAVEFREQUENCY", 1);
Ed_arh_days->Text = f->ReadString("common", "arhive_days", "60");

Ed_t_low->Text   = f->ReadString("timer", "t_low",    "15");
Ed_t_hight->Text = f->ReadString("timer", "t_t_hight", "5");
Ed_co2_low->Text = f->ReadString("timer", "t_co2_low", "5");
Ed_w_low->Text 	 = f->ReadString("timer", "t_w_low",   "5");
Ed_noconect->Text = f->ReadString("timer",  "noconect", "5");

Ed_smtp_login->Text = f->ReadString("Email", "login", "");
Ed_smtp_password->Text = f->ReadString("Email", "password", "");
Ed_smtp_server->Text = f->ReadString("Email", "smtp_server", "smtp.gmail.com");
Ed_smtp_port->Text = f->ReadString("Email", "smtp_port", "587");
Ed_mailto->Text = f->ReadString("Email", "mailto", "box1@gmail.com;box2@gmail.com");

}__finally{delete f;}

}

