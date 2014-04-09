//Copyright+LGPL

//-----------------------------------------------------------------------------------------------------------------------------------------------
// Copyright 1999-2014 Makoto Mori, JE3HHT
//-----------------------------------------------------------------------------------------------------------------------------------------------
// This file is part of MMVARI.ocx.

// MMVARI.ocx is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// MMVARI.ocx is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License along with MMVARI.ocx. If not, see
// <http://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------------------------------------------------------------------------

#ifndef ComLibH
#define ComLibH
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mbstring.h>

//#define VERBETA "A"
//#define	VERNO	"0.39"
#define	INI_SCHEMA	3

#define	SHOWERRCOUNT	FALSE

#if SHOWERRCOUNT
#define	EPHASE(c)	sys.m_ErrPhase = c
enum {
	P_NULL,
	P_DEMBPF,
    P_DEMAGC,
	P_DEMLPF,
    P_DEMAFC,
    P_DEMLOCK,
    P_DEMVCO,
    P_DEMOUT,
    P_SUBCREATE,
    P_SUBVIEW,
};
#else
#define	EPHASE(c)
#endif

#define	VERID	"beta ver "VERNO
#define	VERTTL2	"MMVARI "VERID VERBETA
#define	VERTTL  VERTTL2" (C) JE3HHT 2004."

#define	WM_WAVE		WM_USER+400
#define CM_CRADIO   WM_USER+400
#define	CM_CMML		WM_USER+401
#define	CM_CMMR		WM_USER+402

enum {
	waveIN,
    waveOUT,
    waveCloseFileEdit,
    waveCodeView,
    wavePlayDlg,
    waveCloseRxView,
    waveSwapRxView,
    waveClockAdj,
    waveSeekMacro,
    waveDoMacro,
    waveLoadMacro,
#if DEBUG
    waveRepeatMacro = 1024,
#endif
};

enum {
	macOnTimer,
    macOnPTT,
    macOnQSO,
    macOnFind,
	macOnBand,
    macOnStart,
    macOnExit,
    macOnMode,
    macOnSpeed,
    macOnClick,
    macOnFFTScale,
    macOnEnd,
};
//extern  char    g_MMLogDir[256];

extern	const char	MONN[];
extern	const char	MONU[];
extern	const LPCSTR g_tLogModeTable[];
extern	const LPCSTR g_tDispModeTable[];
extern	const LPCSTR g_tMacEvent[];
extern	const LPCSTR g_tSoundCH[];
extern	const LPCSTR g_tOnOff[];

#ifndef LPCUSTR
typedef const unsigned char *	LPCUSTR;
typedef unsigned char *	LPUSTR;
#endif

#define ABS(c)	(((c)<0)?(-(c)):(c))
#define AN(p)	(sizeof(p)/sizeof(*(p)))
#define	CR		0x0d
#define	LF		0x0a
#define	TAB		'\t'

enum {
	kkTX,
    kkTXOFF,
    kkEOF,
};

typedef struct {
	int			m_DefaultMode;
    int			m_MsgEng;

	LCID		m_LCID;
    WORD		m_wLang;
	BOOL		m_fBaseMBCS;
	DWORD		m_dwVersion;
	BOOL		m_WinNT;
	BYTE		m_Charset;
	BYTE		m_BaseCharset;

    char		m_BgnDir[256];

#if SHOWERRCOUNT
    int			m_ErrPhase;
#endif
}SYSSET;
extern SYSSET   sys;

typedef struct {
	int			WMax;
	int			Max;
    int			Sum;
    int			dBSum;
    int			dBMax;
    int			dBWMax;

    int			Timer;
    int         VW;
	int			LimitL, LimitH;
    int			Sig;
    int			DispSig;
}FFTSTG;

///---------------------------------------------------------
int __fastcall FindStringTableStrictly(const LPCSTR _tt[], LPCSTR pName, int max);
int __fastcall FindStringTable(const LPCSTR _tt[], LPCSTR pName, int max);
int __fastcall GetModeIndex(LPCSTR pName);
int __fastcall IsFile(LPCSTR pName);
BOOL __fastcall StrWindowsVer(LPSTR t);

//---------------------------------------------------------------------------
int __fastcall SetTimeOffsetInfo(int &Hour, int &Min);
void __fastcall AddjustOffset(SYSTEMTIME *tp);
void __fastcall GetUTC(SYSTEMTIME *tp);
void __fastcall GetLocal(SYSTEMTIME *tp);
LPSTR __fastcall StrDupe(LPCSTR s);

LPUSTR __fastcall jstrupr(LPUSTR s);
LPUSTR __fastcall jstrlwr(LPUSTR s);
inline LPSTR __fastcall jstrupr(LPSTR s){return (LPSTR)jstrupr(LPUSTR(s));};
inline LPSTR __fastcall jstrlwr(LPSTR s){return (LPSTR)jstrlwr(LPUSTR(s));};
LPCSTR __fastcall ConvAndChar(LPSTR t, LPCSTR p);

void __fastcall OnWave(void);
int __fastcall SetTimeOffsetInfo(int &Hour, int &Min);
WORD __fastcall AdjustRolTimeUTC(WORD tim, char c);
//void __fastcall DrawMessage(CDC *pDC, int XW, int YW, LPCSTR p, int Pos);
char *__fastcall lastp(char *p);
char *__fastcall clipsp(char *s);
LPCSTR __fastcall _strdmcpy(LPSTR t, LPCSTR p, char c);
const char *__fastcall StrDlmCpy(char *t, const char *p, char Dlm, int len);
const char *__fastcall StrDlmCpyK(char *t, const char *p, char Dlm, int len);
void __fastcall StrCopy(LPSTR t, LPCSTR s, int n);
char __fastcall LastC(LPCSTR p);
LPCSTR __fastcall GetEXT(LPCSTR Fname);
void __fastcall SetEXT(LPSTR pName, LPSTR pExt);
void __fastcall SetCurDir(LPSTR t, int size);
void __fastcall SetDirName(LPSTR t, LPCSTR pName);
void __fastcall LimitInt(int *pInt, int min, int max);
void __fastcall LimitDbl(double *pInt, double min, double max);
LPCSTR __fastcall StrDbl(LPSTR bf, double d);
void __fastcall ClipLF(LPSTR sp);
void __fastcall DeleteComment(LPSTR bf);
LPSTR __fastcall FillSpace(LPSTR s, int n);
LPCSTR __fastcall SkipToValue(LPCSTR sp);
LPSTR __fastcall SkipSpace(LPSTR sp);
LPCSTR __fastcall SkipSpace(LPCSTR sp);
LPSTR __fastcall DelLastSpace(LPSTR t);
LPSTR __fastcall StrDlm(LPSTR &t, LPSTR p);
LPSTR __fastcall StrDlm(LPSTR &t, LPSTR p, char c);
void __fastcall ChgString(LPSTR t, char a, char b);
void __fastcall DelChar(LPSTR t, char a);
int __fastcall atoin(const char *p, int n);
int __fastcall htoin(const char *p, int n);

void InfoMB(LPCSTR fmt, ...);
void ErrorMB(LPCSTR fmt, ...);
void WarningMB(LPCSTR fmt, ...);
int YesNoMB(LPCSTR fmt, ...);
int YesNoCancelMB(LPCSTR fmt, ...);
int OkCancelMB(LPCSTR fmt, ...);

UCOL __fastcall GetGrade2(UCOL s[2], int x, int xw);
void __fastcall SetMBCP(BYTE charset);

void __fastcall NumCopy(LPSTR t, LPCSTR p);
int __fastcall IsNumbs(LPCSTR p);
int __fastcall IsNumbAll(LPCSTR p);
int __fastcall IsAlphas(LPCSTR p);
int __fastcall IsAlphaAll(LPCSTR p);
int __fastcall IsRST(LPCSTR p);
int __fastcall IsCallChar(char c);
int __fastcall IsCall(LPCSTR p);
int __fastcall IsJA(const char *s);
LPCSTR __fastcall ClipCall(LPCSTR s);
LPCSTR __fastcall ClipCC(LPCSTR s);

//inline LPUSTR __fastcall StrDlm(LPUSTR &t, LPUSTR p, char c){return (LPUSTR)StrDlm(LPSTR(t), LPSTR(p), c);};
//inline LPUSTR __fastcall StrDlm(LPUSTR &t, LPUSTR p){return (LPUSTR)StrDlm(LPSTR(t), LPSTR(p));};
//inline LPUSTR __fastcall StrDlm(LPUSTR &t, LPSTR p, char c){return (LPUSTR)StrDlm(LPSTR(t), p, c);};
//inline LPUSTR __fastcall StrDlm(LPUSTR &t, LPSTR p){return (LPUSTR)StrDlm(LPSTR(t), p);};

void __fastcall GPS2SystemTime(ULONG gps, SYSTEMTIME *sp);
ULONG __fastcall SystemTime2GPS(SYSTEMTIME *sp);
void __fastcall DrawMessage(CDC *pDC, int XW, int YW, LPCSTR p, int Pos);

//---------------------------------------------------------------------------
#pragma pack(push, 1)	// パックの指示
typedef struct {
	WORD	cMBCS;
    WORD	cASCII;
}CONVALPHA;
#pragma pack(pop)		// パック解除の指示

class CMBCS
{
private:
	BYTE	*m_pLead;
    int		m_Charset;
public:
	CMBCS();
    ~CMBCS();
	void __fastcall Create(BYTE charset);
	void __fastcall Create(void);
    BOOL __fastcall IsLead(BYTE c);
    BOOL __fastcall IsLead(const unsigned char *p);
	int __fastcall ConvAlpha(int code);

    inline BOOL __fastcall IsCreate(void){return m_pLead != NULL;};
    inline void __fastcall SetCharset(BYTE charset){m_Charset = charset;};
};

//---------------------------------------------------------------------------
// CLIBLクラス
typedef struct {
	LPSTR	pName;
	HINSTANCE  hLib;
}LIBD;

typedef void (__stdcall *tmmMacro)(HWND hWnd, LPSTR t, LPCSTR p);
class CLIBL {
public:
	int     m_AMax;
	int     m_Count;
	LIBD    *m_pBase;
private:
	void __fastcall Alloc(void);
	void __fastcall Add(LPCSTR pName, HINSTANCE hLib);
public:
	CLIBL(){
		m_pBase = NULL;
		Delete();
	};
	~CLIBL(){
		Delete();
	};
	void __fastcall Delete(void);
	HINSTANCE __fastcall LoadLibrary(LPCSTR pName);
	void __fastcall DeleteLibrary(HINSTANCE hLib);
};
#endif
