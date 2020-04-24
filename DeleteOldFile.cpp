//---------------------------------------------------------------------------

#include <System.hpp>
#include <DateUtils.hpp>
#pragma hdrstop

#include "DeleteOldFile.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TDeleteOldFile::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TDeleteOldFile::TDeleteOldFile(int days)	: TThread(false)
{
D =  days;
dt = IncDay(Now(), -D);
this->FreeOnTerminate = true;
fl = new TStringList;
}
//---------------------------------------------------------------------------
void __fastcall TDeleteOldFile::Execute()
{

AnsiString path = ".\\DATA\\";
FindFile(path);
// fl->SaveToFile("______fl.txt");

for(int i = 0; i < fl->Count; ++i)
{
  AnsiString s = fl->Strings[i];
  if(filematch(s)) DeleteFile(s);
}

if(fl) delete fl;
}
//---------------------------------------------------------------------------
void __fastcall TDeleteOldFile::FindFile(AnsiString path)
{
TSearchRec sr;
int iAttributes = 0;
iAttributes |= faAnyFile;
AnsiString s = path + "*.*";
if (FindFirst(s, iAttributes, sr) == 0)
  {
	do
	{
	 // if ((sr.Attr & iAttributes) == sr.Attr)
	  {
		if(sr.Name == ".") continue;
		if(sr.Name == "..") continue;
		if(sr.Attr == faDirectory) FindFile(path + sr.Name + "\\");
	   else fl->Add(path + sr.Name);
	  }
	} while (FindNext(sr) == 0);
	FindClose(sr);
  }
}


//---------------------------------------------------------------------------
bool __fastcall TDeleteOldFile::filematch(AnsiString fn)
{
AnsiString s = ExtractFileName(fn);
WORD Y;
WORD M;
WORD D;
AnsiString ss = s.SubString(1,4);
Y = ss.ToInt();
ss = s.SubString(6,2);
M = ss.ToInt();
ss = s.SubString(9,2);
D = ss.ToInt();
TDateTime fd = EncodeDate(Y, M, D);

if(fd < dt) return true;
return false;
}
