//---------------------------------------------------------------------------

#ifndef DeleteOldFileH
#define DeleteOldFileH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class TDeleteOldFile : public TThread
{
private:
	int D;
	TStringList* fl;
	TDateTime dt;
	void __fastcall FindFile(AnsiString path);
	bool __fastcall filematch(AnsiString fn);
protected:
	void __fastcall Execute();
public:
	__fastcall TDeleteOldFile(int days);
};
//---------------------------------------------------------------------------
#endif
