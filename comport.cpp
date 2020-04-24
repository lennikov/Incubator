/*******************************************************************************
Copyright (c)2004 Shulga N.
********************************************************************************
class TctpSerialPort                    Класс для работы с сом-потом и аналоговым момедом
********************************************************************************
Конструктор по умолчанию настраивает работу на скорости 9600, без контроля четности
8 бит, 1 стоп бит на COM1. Без модема. Протокол ошибок не ведется.

SetBaud(DWORD v)                        Установка скорости обмена
										Если порт не открыт то при открытии будет установлена указанная скорость
										если уже открыт то переустанавливает новую скорость

SetByteSize(BYTE v)                     размер байта          \
SetParity(BYTE v)                       установка четности     \---- SEE DCB structure
SetStopBits(BYTE v)                     кол стоп бит           /
SetWriteLog(BOOL v)                     Вкл/Отк(default) запись логов
SetOS_NT(BOOL v)                        true(default) - overlaped; false — no overlaped
void SetReadTotalTimeoutConstant(DWORD timeout)       Изменить время ожидания чтения ReadTotalTimeoutConstant
void SetReadTotalTimeoutMultiplier(DWORD timeout)
void SetReadIntervalTimeout(DWORD timeout)

SetModemCallNum(AnsiString v)           Установка номера дозвона
SetModemInitString(AnsiString v)        Установка строки инициализации модема
char Open(AnsiString vPortName)         Открытие порта и настройка ея
Close()                                 Закрыть порт

char Read(LPVOID buff, DWORD n);
char Write(LPCVOID buff, DWORD n);

void __fastcall PurgeComBuff();         Чистит оба буфера
void __fastcall CancelIO()              Прервать overlaped ожидание
void __fastcall EscapeCommFunction(DWORD dwFunc);   Управляет приборами ( DTR RTS lines)
DWORD __fastcall ClearCommError(void);  SEE  COMSTAT structure
DWORD __fastcall ByteInQueue(void);     Получить кол байт во входной очереди
DWORD __fastcall ByteOutQueue(void);
DWORD __fastcall GetModemStatus(void)   Получить статус модема
void __fastcall ModemClose(void)        Положить трубу
char __fastcall ModemCall()             Позвонить по установленному номеру     */

#include <vcl.h>
#include <windows.h>
#include <stdio.h>
#include "comport.h"
#pragma hdrstop

#pragma package(smart_init)
//------------------------------------------------------------------------------
//-------------   C O N S T R U C T O R   --------------------------------------
// -----------------------------------------------------------------------------
__fastcall TctpSerialPort::TctpSerialPort(void)
{
Opened = false;
//Baud = BAUD_USER; //
// Baud = BAUD_9600; - врут
//Baud = 9600;
Baud = 0x0000e100;//BAUD_57600;
ByteSize = 8;
Parity =  NOPARITY;
StopBits = ONESTOPBIT;

ComPort = INVALID_HANDLE_VALUE;
NewComPort = INVALID_HANDLE_VALUE;

FillMemory(&dcb, sizeof(dcb), 0);


  lpCommTimeouts.ReadIntervalTimeout = 1;
  lpCommTimeouts.ReadTotalTimeoutMultiplier = 1;
  lpCommTimeouts.ReadTotalTimeoutConstant = 20;
  lpCommTimeouts.WriteTotalTimeoutMultiplier = 1;
  lpCommTimeouts.WriteTotalTimeoutConstant = 20;

//if ( SetCommTimeouts( ComPort, &lpCommTimeouts) == 0)
//		{
//		WriteLogDetail("Can't SetCommTimeouts");
//		}

//TimeOut = 500;

CanWriteDetLog = false;

os_NT = true;

PortName = "COM1";
rootDir = ".\\";
Modem = "";
ComPort = INVALID_HANDLE_VALUE;

//ZeroMemory(&ovrlpdRead, sizeof(ovrlpdRead));
//ovrlpdRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
}

// -----------------------------------------------------------------------------
void __fastcall TctpSerialPort::SetBaud(DWORD v)
{
Baud = v;
if(ComPort == INVALID_HANDLE_VALUE) return; // if not open

if(!GetCommState(ComPort, &dcb))
		{
		 WriteLogDetail("Can't read DCB");
		 return;//(0);
		}


dcb.BaudRate = v;

if (!SetCommState(ComPort, &dcb))
		{
		 WriteLogDetail("Can't write DCB");
		 return;//(0);
		}
WriteLogDetail("SetBaud "+ IntToStr((int)v));
}
// -----------------------------------------------------------------------------
void __fastcall TctpSerialPort::SetByteSize(BYTE v)
{
ByteSize = v;
if(ComPort == INVALID_HANDLE_VALUE) return; // if not open
if(!GetCommState(ComPort, &dcb))
		{
		 WriteLogDetail("Can't read DCB");
		 return;//(0);
		}

dcb.ByteSize = v;

if (!SetCommState(ComPort, &dcb))
		{
		 WriteLogDetail("Can't write DCB");
		 return;//(0);
		}
WriteLogDetail("SetByteSize "+ IntToStr((int)v));
}
// -----------------------------------------------------------------------------
void __fastcall TctpSerialPort::SetParity(BYTE v)
{
Parity = v;

if(ComPort == INVALID_HANDLE_VALUE) return; // if not open
if(!GetCommState(ComPort, &dcb))
		{
		 WriteLogDetail("Can't read DCB");
		 return;//(0);
		}

dcb.Parity = v;

if (!SetCommState(ComPort, &dcb))
		{
		 WriteLogDetail("Can't write DCB");
		 return;//(0);
		}
WriteLogDetail("SetParity "+ IntToStr((int)v));
}
// -----------------------------------------------------------------------------
void __fastcall TctpSerialPort::SetStopBits(BYTE v)
{
StopBits = v;
if(ComPort == INVALID_HANDLE_VALUE) return; // if not open
if(!GetCommState(ComPort, &dcb))
		{
		 WriteLogDetail("Can't read DCB");
		 return;//(0);
		}

dcb.StopBits = v;

if (!SetCommState(ComPort, &dcb))
		{
		 WriteLogDetail("Can't write DCB");
		 return;//(0);
		}
WriteLogDetail("SetStopBits "+ IntToStr((int)v));
}
// -----------------------------------------------------------------------------
void __fastcall TctpSerialPort::SetTimeOut(DWORD TotalTimeoutConstant, DWORD ReadTotalTimeoutMultiplier, DWORD ReadIntervalTimeout, DWORD WriteTotalTimeoutConstant, DWORD WriteTotalTimeoutMultiplier)
{
if(!GetCommTimeouts(ComPort, &lpCommTimeouts))
		{
		WriteLogDetail("Can't read ReadTotalTimeoutConstant");
		return;// 0;
		}

lpCommTimeouts.ReadTotalTimeoutConstant = TotalTimeoutConstant;
lpCommTimeouts.ReadTotalTimeoutMultiplier = ReadTotalTimeoutMultiplier;
lpCommTimeouts.ReadIntervalTimeout = ReadIntervalTimeout;

lpCommTimeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier;
lpCommTimeouts.WriteTotalTimeoutConstant = WriteTotalTimeoutConstant;

if(!SetCommTimeouts( ComPort, &lpCommTimeouts))
		{
		WriteLogDetail("Can't write ReadTotalTimeoutConstant");
		return;// 0;
		}

//return 1;
}

//------------------------------------------------------------------------------
void  __fastcall TctpSerialPort::SetReadTotalTimeoutConstant(DWORD timeout)
{
if(!GetCommTimeouts(ComPort, &lpCommTimeouts))
        {
        WriteLogDetail("Can't read ReadTotalTimeoutConstant");
        return;// 0;
        }

lpCommTimeouts.ReadTotalTimeoutConstant = timeout;

if(!SetCommTimeouts( ComPort, &lpCommTimeouts))
        {
        WriteLogDetail("Can't write ReadTotalTimeoutConstant");
        return;// 0;
        }

//return 1;
}
//------------------------------------------------------------------------------
void  __fastcall TctpSerialPort::SetReadTotalTimeoutMultiplier(DWORD timeout)
{
if(!GetCommTimeouts(ComPort, &lpCommTimeouts))
		{
        WriteLogDetail("Can't read ReadTotalTimeoutMultiplier");
        return;
		}

lpCommTimeouts.ReadTotalTimeoutMultiplier = timeout;

if(!SetCommTimeouts( ComPort, &lpCommTimeouts))
        {
        WriteLogDetail("Can't write ReadTotalTimeoutMultiplier");
		return;
        }
}
//------------------------------------------------------------------------------
void  __fastcall TctpSerialPort::SetReadIntervalTimeout(DWORD timeout)
{
if(!GetCommTimeouts(ComPort, &lpCommTimeouts))
        {
        WriteLogDetail("Can't read ReadIntervalTimeout(");
		return;;
		}

lpCommTimeouts.ReadIntervalTimeout = timeout;

if(!SetCommTimeouts( ComPort, &lpCommTimeouts))
        {
        WriteLogDetail("Can't write ReadIntervalTimeout");
        return;// 0;
        }

//return 1;
}


// -----------------------------------------------------------------------------
void __fastcall TctpSerialPort::SetWriteLog(BOOL v)     {CanWriteDetLog = v;}
// -----------------------------------------------------------------------------
void __fastcall TctpSerialPort::SetModemCallNum(AnsiString v)  {Modem = v;}
// -----------------------------------------------------------------------------
void __fastcall TctpSerialPort::SetModemInitString(AnsiString v) {initstring = v;}
// -----------------------------------------------------------------------------
void __fastcall TctpSerialPort::SetRootDir(AnsiString v){rootDir = v;}
// -----------------------------------------------------------------------------
void __fastcall TctpSerialPort::SetOS_NT(BOOL v)       {os_NT = v;}

// -----------------------------------------------------------------------------
//-------------------       L O G     ------------------------------------------
//------------------------------------------------------------------------------
void __fastcall TctpSerialPort::WriteLogDetail(AnsiString s)
{
if (!CanWriteDetLog) return;
FILE *f;


if(s == "Clear")
		{
        f = fopen((rootDir + "serialport.log").c_str(), "w");
        s = "Start";
		}
else    f = fopen((rootDir + "serialport.log").c_str(), "a");

if(f == NULL) return;

if(s == "") s = "\n";
else s = DateTimeToStr(Now()) + "\t" + s + "\n";

fwrite(s.c_str(), s.Length(), 1, f);

fclose(f);
}

//------------------------------------------------------------------------------
char __fastcall TctpSerialPort::Open(AnsiString vPortName)
{
//if(PortName != "")
PortName = vPortName;

if(os_NT)
  NewComPort =CreateFile( UnicodeString(PortName).c_str(),
						  GENERIC_READ |GENERIC_WRITE,
						  0,
						  NULL,
						  OPEN_EXISTING,
						  FILE_FLAG_OVERLAPPED,
						  NULL);
else
 NewComPort =CreateFile( UnicodeString(PortName).c_str(),
						 GENERIC_READ |GENERIC_WRITE,
						 0,
						 NULL,
						 OPEN_EXISTING,
						 0,
						 NULL);

if (NewComPort == INVALID_HANDLE_VALUE)
		{
		WriteLogDetail("Can't open port " + PortName);
		return(0);
		}

ComPort = NewComPort;



if(!GetCommState(ComPort, &dcb))
		{
		 WriteLogDetail("Can't read DCB " + PortName);
		 return(0);
		}


dcb.BaudRate = Baud;
dcb.ByteSize = ByteSize;
dcb.Parity = Parity;
dcb.StopBits = StopBits;


if (!SetCommState(ComPort, &dcb))
		{
		 WriteLogDetail("Can't write DCB");
		 return(0);
        }


if (!GetCommTimeouts( ComPort, &lpCommTimeouts))
		{
		WriteLogDetail("Can't read CommTimeouts " + PortName);
		return(0);
		}

if(Modem == "") // нет модема
  {
  lpCommTimeouts.ReadIntervalTimeout = 300;
  lpCommTimeouts.ReadTotalTimeoutMultiplier = 200;
  lpCommTimeouts.ReadTotalTimeoutConstant = 1000;
  lpCommTimeouts.WriteTotalTimeoutMultiplier = 10;
  lpCommTimeouts.WriteTotalTimeoutConstant = 100;
  }
else
  {
  lpCommTimeouts.ReadIntervalTimeout = 20000;              // Interval =    20 sec
  lpCommTimeouts.ReadTotalTimeoutMultiplier = 2000;        // Total time =  2*130 + 3 =  263 sec  4,5 min
  lpCommTimeouts.ReadTotalTimeoutConstant = 5000;
  lpCommTimeouts.WriteTotalTimeoutMultiplier = 10;
  lpCommTimeouts.WriteTotalTimeoutConstant = 100;
  }


if ( SetCommTimeouts( ComPort, &lpCommTimeouts) == 0)
		{
		WriteLogDetail("Can't SetCommTimeouts " + PortName);
		return(0);
		}

PurgeComm(ComPort,PURGE_TXCLEAR);
PurgeComm(ComPort,PURGE_RXCLEAR);

WriteLogDetail("Port " + vPortName + " open");
Opened = true;
return(1);
}


//------------------------------------------------------------------------------
char __fastcall TctpSerialPort::Write(LPCVOID buff, DWORD n)
{
if(!Opened) return 0;
AnsiString ms;
DWORD nw;
OVERLAPPED Overlapped = {0};
Overlapped.Offset = 0;
Overlapped.OffsetHigh = 0;
Overlapped.hEvent = NULL;
if(os_NT)
 {
 WriteFile(ComPort, buff, n, &nw, &Overlapped );
 if (!GetOverlappedResult( ComPort, &Overlapped, &nw,  true))
  {
   WriteLogDetail(PortName + "-> ERROR write comport");
   return(0);
  }
 }
 else WriteFile(ComPort, buff, n, &nw, NULL );

if(nw < n )
		{
		WriteLogDetail(PortName + "-> Write " + IntToStr((int)nw) + " byte instead " + IntToStr((int)n));
		return(0);
		}
return(1);
}

//------------------------------------------------------------------------------
int __fastcall TctpSerialPort::Read(LPVOID buff, DWORD n)
{
if(!Opened) return 0;
AnsiString ms;
DWORD nw;
OVERLAPPED Overlapped = {0};
Overlapped.Internal = 0;
Overlapped.InternalHigh = 0;
Overlapped.Offset = 0;
Overlapped.OffsetHigh = 0;
Overlapped.InternalHigh = 0;
Overlapped.hEvent = NULL;
if(os_NT)
{

 ReadFile(ComPort, buff, n, &nw, &Overlapped );
// WaitForSingleObject(ComPort, INFINITE);

  if (!GetOverlappedResult( ComPort, &Overlapped, &nw,  true ))
  {
  WriteLogDetail(PortName + "-> ERROR read comport");
  return(0);
  }

}
else ReadFile(ComPort, buff, n, &nw, NULL );

if(nw < n )
		{
		WriteLogDetail(PortName + "-> Read " + IntToStr((int)nw) + " byte instead " + IntToStr((int)n));
		return(0);
		}
return(nw);
}
//------------------------------------------------------------------------------
AnsiString __fastcall TctpSerialPort::ReadLine()
{
if(!Opened) return "";
AnsiString s = "";
char buff;
while(1)
{
int n = Read(&buff,1);
if(n == 0) break;
if((buff == 0x0D) && (s.Length() !=0)) break;
s = s + buff;
}
return Trim(s);
}

//------------------------------------------------------------------------------
void __fastcall TctpSerialPort::Close()
{
if(!Opened) return;
 if(ComPort != INVALID_HANDLE_VALUE)
	{
	WriteLogDetail("Port close");
	CloseHandle(ComPort);
	ComPort = INVALID_HANDLE_VALUE;
	}
Opened = false;
}


//------------------------------------------------------------------------------
BOOL __fastcall TctpSerialPort::SetQueueSize(DWORD dwInQueue, DWORD dwOutQueue)
{
return SetupComm(ComPort, dwInQueue, dwOutQueue);
}



//------------------------------------------------------------------------------

void __fastcall TctpSerialPort::PurgeComBuff()
{
PurgeComm(ComPort,PURGE_TXCLEAR);
PurgeComm(ComPort,PURGE_RXCLEAR);
}
//------------------------------------------------------------------------------
void __fastcall TctpSerialPort::CancelIO()
{
//CancelIO(ComPort);
}
//------------------------------------------------------------------------------
void __fastcall TctpSerialPort::EscapeCommFunction(DWORD dwFunc)
{
::EscapeCommFunction(ComPort, dwFunc);
}
//------------------------------------------------------------------------------
DWORD __fastcall TctpSerialPort::ClearCommError(void)
{
DWORD   dwErrors;
if (::ClearCommError(ComPort, &dwErrors, &comStat))
return(dwErrors);
else return(0);
}
//------------------------------------------------------------------------------

DWORD __fastcall TctpSerialPort::ByteInQueue(void)
{
DWORD   dwErrors;
if (::ClearCommError(ComPort, &dwErrors, &comStat))
return comStat.cbInQue;
else return(0);
}
//------------------------------------------------------------------------------

DWORD __fastcall TctpSerialPort::ByteOutQueue(void)
{
DWORD   dwErrors;
if (::ClearCommError(ComPort, &dwErrors, &comStat))
return comStat.cbOutQue;
else return(0);
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//-----------------     M  O  M  E  D      -------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

DWORD __fastcall TctpSerialPort::GetModemStatus(void)
{
DWORD dwModemStatus;
if (GetCommModemStatus(ComPort, &dwModemStatus))
return dwModemStatus;
else return(0);
}

//------------------------------------------------------------------------------
void __fastcall TctpSerialPort::ModemClose(void)
{
 Write("+++", 3);
 Sleep(2000);
 Write("ATH0\r", 5);
}

//------------------------------------------------------------------------------
char __fastcall TctpSerialPort::ModemCall(void)
{
char b[2];
AnsiString s;
char CallTry = 0;
TDateTime TimeBegin;
DWORD State;
//------------------------------------------------------------------------------
WriteLogDetail("CAllModem " + Modem + " " + initstring);

t:
Application->ProcessMessages();
Sleep(1000);
State = GetModemStatus();
if ((State & MS_CTS_ON) && (State & MS_DSR_ON))
  {
	s = "AT\r";
	if(!Write(s.data(), s.Length()))
		  {
		  WriteLogDetail("Нет связи с модемом");
		  return 0;
		  }
	Application->ProcessMessages();
	Sleep(1000);

	s = initstring + " E0 V0 L0\r";   // Init modem
	Write(s.data(), s.Length());

	Application->ProcessMessages();
	Sleep(2000);

	// - - - - - - - -   Набор номера    - - - - - - - - - - - - - - - - - - - -
	if(Modem == "A") s = "ATA\r";   // Если выделенка
	else               s = "ATDP" + Modem + "\r";

	WriteLogDetail("набираем номер " + Modem);
	Write(s.data(), s.Length());   // Набираем номер
	PurgeComBuff();

	Application->ProcessMessages();
	Sleep(4000);

	TimeBegin = Now();

	while(1)  //  - - - - -    Ожидаем ответа    - - - - - - - - - - - - - - - -
	{
	  Application->ProcessMessages();
	  if(Read(b, 2))
		{
		s = AnsiString(b[0]);
		if((b[1] != 0x13)&&(b[1] != 0x0D)) s += AnsiString(b[1]);

		WriteLogDetail("Ответ модема: " + s);
		if(s == "1")  return 1;
		if(s == "3") {CallTry++; if(CallTry > 3) return 0; else goto t;}
		if(s == "4") {CallTry++; if(CallTry > 3) return 0; else goto t;}
		if(s == "5") return 1;
		if(s == "6") {CallTry++; if(CallTry > 3) return 0; else goto t;}
		if(s == "7") {CallTry++; if(CallTry > 3) return 0; else {Sleep(5000); goto t;}}
		if(s == "8") {CallTry++; if(CallTry > 3) return 0; else goto t;}

		State = GetModemStatus();
		if(State & MS_RLSD_ON) {Sleep(500); return 1;}
		}

	   State = GetModemStatus();

	   if (!((State & MS_CTS_ON) && (State & MS_DSR_ON)))
		 {
		  WriteLogDetail("модем не подключен");
		  return 0;    // modem не подключен
		 }


		if((Now() - TimeBegin) > EncodeTime(0,2,0,0))
			{
			  Write("+++", 3);
			  Sleep(2000);
			  Write("ATH0\r", 5);
			  if(CallTry > 3) return 0;  else goto t;
			}
  }
}
else {WriteLogDetail("модем не подключен"); return 0;}    // modem не подключен

}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//--------------     E  V  E  N  T       ---------------------------------------
//------------------------------------------------------------------------------
////------------------------------------------------------------------------------
//void __fastcall TctpSerialPort::SetCommMask(DWORD EvtMask)
//{
//if(!::SetCommMask(ComPort, EvtMask))WriteLogDetail("Event mask not install");
//}
//
////------------------------------------------------------------------------------
//void __fastcall TctpSerialPort::WaitCommEvent(unsigned long EvtMask, DWORD TimeOut)
//{
//DWORD   dwRes;
//DWORD   BytesTransferred;
//unsigned long dwEventtMask;
//OVERLAPPED Overlapped = {0};
//
//if(!::WaitCommEvent(ComPort, &dwEventtMask, &Overlapped))
//	 if (GetLastError() == ERROR_IO_PENDING)
//		 {
//		 dwRes = WaitForSingleObject(ComPort, TimeOut);
//
//		 switch(dwRes)
//		 {
//			 case WAIT_OBJECT_0:
//				 GetOverlappedResult(ComPort, &Overlapped, &BytesTransferred, FALSE);
//				 break;
//
//			 case WAIT_TIMEOUT:
//				 WriteLogDetail("Timeout wait event");
//				 break;
//		 }
//		 }
//}

////------------------------------------------------------------------------------
//void __fastcall TctpSerialPort::SetBeam(char beam, char type = 0)
//{
//if(type == 0) // potential
//{
//switch(beam - 1)
//        {
//        case 0: EscapeCommFunction(SETDTR);
//                EscapeCommFunction(SETRTS);
//                break;
//
//        case 1: EscapeCommFunction(CLRDTR);
//                EscapeCommFunction(SETRTS);
//                break;
//
//        case 2: EscapeCommFunction(SETDTR);
//                EscapeCommFunction(CLRRTS);
//                break;
//
//        case 3: EscapeCommFunction(CLRDTR);
//                EscapeCommFunction(CLRRTS);
//                break;
//
//        default:EscapeCommFunction(SETDTR);
//                EscapeCommFunction(SETRTS);
//        }
////Sleep((unsigned int)1);
//}
//else //pulse
//{
//EscapeCommFunction(SETDTR);
//EscapeCommFunction(CLRRTS);
//Sleep((unsigned int)1);
//
//EscapeCommFunction(SETRTS);
//EscapeCommFunction(CLRRTS);
//
//for(int i = 0; i < beam; i++)
//        {
//        EscapeCommFunction(CLRDTR);
////        Sleep((unsigned int)30);
//        EscapeCommFunction(SETDTR);
//        }
//Sleep((unsigned int)3);
//}
//}
