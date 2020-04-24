//---------------------------------------------------------------------------

#ifndef comportH
#define comportH
//---------------------------------------------------------------------------
#include <Classes.hpp>

class TctpSerialPort
{
private:
DWORD           Baud;
BYTE            ByteSize;
BYTE            Parity;
BYTE            StopBits;

HANDLE  	ComPort;
HANDLE  	NewComPort;

DCB             dcb;
COMMTIMEOUTS 	lpCommTimeouts;
DWORD           ModemStatus;
COMSTAT         comStat;

BOOL            CanWriteDetLog;
BOOL 			os_NT;

AnsiString      rootDir;
AnsiString      Modem;
AnsiString      initstring;
//DWORD           TimeOut;

//OVERLAPPED	ovrlpdRead;

void __fastcall WriteLogDetail(AnsiString s);

public:
AnsiString      PortName;
bool Opened;
__fastcall TctpSerialPort(void);
void __fastcall SetBaud(DWORD v);
void __fastcall SetByteSize(BYTE v);
void __fastcall SetParity(BYTE v);
void __fastcall SetStopBits(BYTE v);
void __fastcall SetTimeOut(DWORD TotalTimeoutConstant, DWORD ReadTotalTimeoutMultiplier, DWORD ReadIntervalTimeout, DWORD WriteTotalTimeoutConstant, DWORD WriteTotalTimeoutMultiplier);
void __fastcall SetWriteLog(BOOL v);
void __fastcall SetOS_NT(BOOL v);
void __fastcall SetModemCallNum(AnsiString v);
void __fastcall SetModemInitString(AnsiString v);
BOOL __fastcall SetQueueSize(DWORD dwInQueue, DWORD dwOutQueue);

int __fastcall Read(LPVOID buff, DWORD n);
AnsiString __fastcall ReadLine();
char __fastcall Write(LPCVOID buff, DWORD n);
char __fastcall Open(AnsiString vPortName);
void __fastcall Close();
//void __fastcall SetComTimeOut(DWORD timeout);
void __fastcall SetReadTotalTimeoutConstant(DWORD timeout);
void __fastcall SetReadTotalTimeoutMultiplier(DWORD timeout);
void __fastcall SetReadIntervalTimeout(DWORD timeout);

void __fastcall PurgeComBuff();
void __fastcall CancelIO();
void __fastcall EscapeCommFunction(DWORD dwFunc);
DWORD __fastcall GetModemStatus(void);
DWORD __fastcall ClearCommError(void);
DWORD __fastcall ByteInQueue(void);
DWORD __fastcall ByteOutQueue(void);
void __fastcall ModemClose(void);
char __fastcall ModemCall();
void __fastcall SetRootDir(AnsiString v);
//void __fastcall SetCommMask(DWORD EvtMask);
//void __fastcall WaitCommEvent(unsigned long EvtMask, DWORD TimeOut);
//void __fastcall SetBeam(char n, char type);
};

#endif
