//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SendEmail.h"
//#include "SendMailError.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TSendEmail::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

//void _fastcall WriteLogFile(AnsiString s);
////////////////////////////////////////////////////////////////////////////////
__fastcall TSendEmail::TSendEmail(bool CreateSuspended)	: TThread(CreateSuspended)
{
// Attachment = new TStringList;
// html = true;
MailPort = 587;
}
//---------------------------------------------------------------------------
void __fastcall TSendEmail::Execute()
{
//WriteLogFile("Start TSendEmail thread");
TIdSSLIOHandlerSocketOpenSSL* io = new TIdSSLIOHandlerSocketOpenSSL(NULL);
TIdSMTP *IdSMTP = new TIdSMTP(NULL);
IdSMTP->IOHandler = io;
IdSMTP->Host= MailHost;// "smtp.gmail.com";
IdSMTP->Port = MailPort; //587; // 465;

//IdSMTP->Host= "smtp.timeweb.ru";
//IdSMTP->Port = 25;


IdSMTP->Username = MailUserName;
//IdSMTP->Username = "tesst";//@centrrp.ru";
IdSMTP->Password = MailPassword;
IdSMTP->UseTLS = utUseRequireTLS;

TIdMessage *Msg = new TIdMessage(IdSMTP);
  Msg->OnInitializeISO = MsgInitializeISO;
  Msg->CharSet = L"Windows-1251";
  Msg->Subject = "INCUBATOR ALARM"; //Subject; //"Test Subject -- Тестовое письмо";
//  Msg->NoEncode = false;

  Msg->ContentTransferEncoding = L"8bit";
  Msg->ContentType = L"text/plain, Windows-1251";
  //Msg->ContentType = L"multipart/mixed";

  Msg->Recipients->EMailAddresses = MailAddresses; //"nickshulga@wildpark.net";
  //Msg->Recipients->EMailAddresses = "nickshulga@gmail.com";

  TIdText *EText = new TIdText(Msg->MessageParts, NULL);
  EText->Body->Text = MessageText; //L"Тело сообщения";
  //if(isHTMLcontext(EText->Body->Text)) EText->ContentType = L"text/html"; else
  EText->ContentType = L"text/plain";

  EText->CharSet = L"Windows-1251";
  EText->ContentTransfer = L"8bit";

  Msg->From->Address = MailUserName;
  Msg->From->Name = "INCUBATOR"; //MailFrom;  //L"Фирма Рога и Копыта";
  Msg->Body->Clear();

//for(int i = 0; i < Attachment->Count; ++i)
//{
//AnsiString fn = Attachment->Strings[i];
//  if(FileExists(fn))
//  {
//  TIdAttachmentFile* af = new TIdAttachmentFile(Msg->MessageParts, fn);
//  af->ContentType = "application/msword";
//  af->ContentDisposition = "attachment";
//  }
//}



//  WriteLogFile("Host: " + IdSMTP->Host);
//  WriteLogFile("Port: " +   IntToStr(IdSMTP->Port));
//  WriteLogFile("username: " +   IdSMTP->Username);
//  WriteLogFile("password: " +   IdSMTP->Password);
//  WriteLogFile("To: " +     Msg->Recipients->EMailAddresses);


  Msg->AfterConstruction();
  try
  {
  IdSMTP->Connect();
  }
  catch(...){
//				WriteLogFile("Connect error. Сервер недоступен или неверен пароль");
//				FrmSendMailError->Memo1->Lines->Add(IdSMTP->Host);
//				FrmSendMailError->Show();
			}
  try
  {
  IdSMTP->Send(Msg);
  }
  catch(...) {
//				WriteLogFile("Отсылка письма не удалась.");
				if(Alias == "") Alias = MailAddresses;
//				FrmSendMailError->Memo1->Lines->Add(Alias);
//				FrmSendMailError->Show();
				IdSMTP->Disconnect();
//				delete Attachment;
//				WriteLogFile("TSendEmail error exit");
				return;
			 }
IdSMTP->Disconnect();
//delete Attachment;
//WriteLogFile("TSendEmail exit");
}
//---------------------------------------------------------------------------

void __fastcall TSendEmail::MsgInitializeISO(wchar_t &VHeaderEncoding, UnicodeString &VCharSet)
{
//  VTransferHeader = bit8;   // в Си такого параметра нет
//  VHeaderEncoding = L'B';
  VHeaderEncoding = L'B';
//   VCharSet = L"Windows-1251";
  VCharSet = L"UTF-8";
////VTransferHeader = bit7;
//  VHeaderEncoding= 'Q';
//  VCharSet = "koi8-r";
}


//---------------------------------------------------------------------------
bool __fastcall TSendEmail::isHTMLcontext(AnsiString s)
{
if(s.Pos("<br") > 0) return true; else return false;  // перевод строки
}
