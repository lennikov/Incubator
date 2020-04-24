//---------------------------------------------------------------------------

#ifndef SendEmailH
#define SendEmailH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdExplicitTLSClientServerBase.hpp>
#include <IdMessageClient.hpp>
#include <IdSMTP.hpp>
#include <IdSMTPBase.hpp>
#include <IdText.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <IdSSL.hpp>
#include <IdSSLOpenSSL.hpp>
#include <IdAttachmentFile.hpp>


//---------------------------------------------------------------------------
class TSendEmail : public TThread
{
private:
	bool __fastcall isHTMLcontext(AnsiString s);
	void __fastcall MsgInitializeISO(wchar_t &VHeaderEncoding, UnicodeString &VCharSet);
protected:
	void __fastcall Execute();
public:
//    bool html;
	AnsiString Alias;
	AnsiString MailHost;
	WORD	   MailPort;
	AnsiString MailUserName;
	AnsiString MailPassword;
	AnsiString MailFrom;
	AnsiString Subject;
	AnsiString MessageText;
	AnsiString MailAddresses;
//	TStringList* Attachment;
	__fastcall TSendEmail(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
