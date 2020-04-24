//---------------------------------------------------------------------------

#ifndef WorkThreadH
#define WorkThreadH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
class TctpSerialPort;
class TFrmIncubator;
//---------------------------------------------------------------------------
class TWorkThread : public TThread
{
private:
//	int ID;
	bool send_A1;
	bool send_A2;
	bool send_A3;
	bool send_A4;

	int READFREQUENCY; // 30 cек темп опроса
	int SAVEFREQUENCY; // мин

	AnsiString COMPORT;
	int baud;
	TctpSerialPort* com;

	TDateTime start_T;

	TDateTime start_CO2;

	TDateTime start_W;
	bool A_C;
	TDateTime start_C;
	TDateTime last_save;
	float T_E_low;
	float T_E_hight;
	float CO2_E_low;
	float CO2_E_hight;
	float W_E_low;

	int T_low_wait;  	// время срабатывания аварии при температуре меньше нижнего порога
	int T_hight_wait;     // время срабатывания аварии при температуре больше верхнего порога
	int CO2_low_wait;  // // время срабатывания аварии при % CO2 меньше порога
	int water_low_wait;
	int T_noconect;

	TFrmIncubator* Incubator;
	void _fastcall WriteArhiveFile(AnsiString s);
	void _fastcall WriteAlarmOrder(AnsiString s);
	float __fastcall myStrToFloat(AnsiString s);
	AnsiString __fastcall myStrToStr(AnsiString s);
	void _fastcall WriteLogFile(AnsiString s);
	char __fastcall ahex(char a);
	char __fastcall bintohex(char a);
	void __fastcall Parce(AnsiString s);
	void __fastcall Parce2(AnsiString s);

	bool __fastcall ReadTemperature();
	bool __fastcall ReadCO2();
	bool __fastcall ReadWater();
	bool __fastcall ReadErrorMemory();
	void __fastcall SendAlarm(AnsiString);
	void __fastcall SaveValue();
//	int cnterror;
protected:
	void __fastcall Execute();
public:
	AnsiString temperature;
	AnsiString CO2;
	AnsiString water;
	AnsiString DOTL;
	AnsiString s1;
	AnsiString s2;
    bool A_T;
	bool A_CO2;
	bool A_W;
	bool Connected;
	__fastcall TWorkThread(TFrmIncubator* Incubator);
};
//---------------------------------------------------------------------------
#endif
