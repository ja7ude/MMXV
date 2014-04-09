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

#include "stdafx.h"
#include "MMVARI.h"
///----------------------------------------------------------------

#include "ComLib.h"
#include "Dsp.h"
//---------------------------------------------------------------------------
//char    g_MMLogDir[256];

SYSSET  	sys;		// システム共通データ

//const LPCSTR g_tLogModeTable[]={"GMSK", "FSK", "FSK", "PSK", "PSK", "RTTY", "RTTY", "MFSK", "MFSK", NULL};
const LPCSTR g_tDispModeTable[]={"GMSK", "FSK", "FSK-W", "BPSK", "bpsk", "rtty-L", "rtty-U", "mfsk-L", "mfsk-U", NULL};
//const LPCSTR g_tSoundCH[]={"MONO","LEFT","RIGHT"};
//---------------------------------------------------------------------------
void __fastcall OnWave(void)
{
//	MainVARI->OnWave();
}
//---------------------------------------------------------------------------
int __fastcall FindStringTableStrictly(const LPCSTR _tt[], LPCSTR pName, int max)
{
	int n;
    for( n = 0; (n < max) && (_tt[n]!=NULL); n++ ){
		if( !strcmp(_tt[n], pName) ) return n;
    }
    return -1;
}
//---------------------------------------------------------------------------
int __fastcall FindStringTable(const LPCSTR _tt[], LPCSTR pName, int max)
{
	int n;
    for( n = 0; (n < max) && (_tt[n]!=NULL); n++ ){
		if( !strcmpi(_tt[n], pName) ) return n;
    }
    return -1;
}
//---------------------------------------------------------------------------
int __fastcall GetModeIndex(LPCSTR pName)
{
	int m;
	for( m = 0; g_tDispModeTable[m] && (m < MODE_END); m++ ){
		if( !strcmp(g_tDispModeTable[m], pName) ) return m;
    }
    if( sscanf(pName, "%d", &m) == 1 ){
		if( m < 0 ) m = 0;
        if( m >= MODE_END ) m = MODE_END - 1;
    }
    else {
		m = 0;
    }
    return m;
}

//---------------------------------------------------------------------------
int __fastcall IsFile(LPCSTR pName)
{
	if( !pName || !*pName ) return 0;
	FILE *fp;
	fp = fopen(pName, "rb");
	if( fp != NULL ){
		fclose(fp);
		return 1;
	}
	else {
		return 0;
	}
}
//---------------------------------------------------------------------------
int __fastcall SetTimeOffsetInfo(int &Hour, int &Min)
{
	TIME_ZONE_INFORMATION tz;

	int off;
	switch(GetTimeZoneInformation(&tz)){
		case TIME_ZONE_ID_STANDARD:
			off = tz.StandardBias;
			break;
		case TIME_ZONE_ID_DAYLIGHT:
			off = tz.DaylightBias;
			break;
		default:
			return FALSE;
	}
	if( off >= 0 ){
		Hour = -(off / 60);
		Min = (off % 60);
	}
	else {
		off = -off;
		Hour = off / 60;
		Min = off % 60;
	}
	return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall LimitInt(int *pInt, int min, int max)
{
	if( *pInt < min ) *pInt = min;
    if( *pInt > max ) *pInt = max;
}
//---------------------------------------------------------------------------
void __fastcall LimitDbl(double *pInt, double min, double max)
{
	if( *pInt < min ) *pInt = min;
    if( *pInt > max ) *pInt = max;
}
//---------------------------------------------------------------------------
LPCSTR __fastcall StrDbl(LPSTR bf, double d)
{
	LPSTR	p;

	sprintf(bf, "%lf", d);
	for( p = &bf[strlen(bf)-1]; p >= bf; p-- ){
		if( (*p == '0')&&(*(p-1)!='.') ){
			*p = 0;
		}
		else {
			break;
		}
	}
	return bf;
}
//---------------------------------------------------------------------------
LPSTR __fastcall StrDupe(LPCSTR s)
{
	LPSTR p = new char[strlen(s)+1];
	strcpy(p, s);
	return p;
}


LPUSTR __fastcall jstrupr(LPUSTR s)
{
	LPUSTR	p = s;
	int kf;

	for( kf = 0; *p; p++ ){
		if( kf ){
			kf = 0;
		}
		else if( _mbsbtype((unsigned char *)p, 0) == _MBC_LEAD ){
			kf = 1;
		}
		else {
			*p = (unsigned char)toupper(*p);
		}
	}
	return s;
}
LPUSTR __fastcall jstrlwr(LPUSTR s)
{
	LPUSTR	p = s;
	int kf;

	for( kf = 0; *p; p++ ){
		if( kf ){
			kf = 0;
		}
		else if( _mbsbtype((unsigned char *)p, 0) == _MBC_LEAD ){
			kf = 1;
		}
		else {
			*p = (unsigned char)tolower(*p);
		}
	}
	return s;
}
//---------------------------------------------------------------------------
LPCSTR __fastcall ConvAndChar(LPSTR t, LPCSTR p)
{
	LPSTR s = t;

    while(*p){
		if( *p == '&' ){
			*t++ = *p;
        }
		*t++ = *p++;
    }
    *t = 0;
    return s;
}
/*#$%
===============================================================
	時刻を調整する
---------------------------------------------------------------
	t : 時刻(UTC)
	c : 時差コード
---------------------------------------------------------------
	ローカルタイム
---------------------------------------------------------------
	A-Z
	a-z 	+30min
===============================================================
*/
WORD __fastcall AdjustRolTimeUTC(WORD tim, char c)
{
	const	char	tdf[]={
/*		A B C D E F G H I J K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z	*/
		1,2,3,4,5,6,7,8,9,9,10,11,12,23,22,21,20,19,18,17,16,15,14,13,12,0
	};
	int	cc;

	cc = toupper(c);
	if( (cc >= 'A')&&(cc <= 'Z') ){
		// JST to UTC
#if 0
		if( tim >= (9*60*30) ){
			tim -= WORD(9 * 60 * 30);
		}
		else {
			tim += WORD(15 * 60 * 30);
		}
#endif
		tim /= WORD(30);
		tim += WORD(tdf[cc-'A'] * 60);
		if( c >= 'a' ) tim += WORD(30);
		while( tim >= (24 * 60) ){
			tim -= WORD(24 * 60);
		}
		tim *= WORD(30);
		if( !tim ) tim++;
	}
	else {
		tim = 0;
	}
	return tim;
}
/*#$%
========================================================
	最後の文字コードを返す
--------------------------------------------------------
	p : 文字列のポインタ
--------------------------------------------------------
	文字コード
--------------------------------------------------------
========================================================
*/
char *__fastcall lastp(char *p)
{
	if( *p ){
		for( ; *p; p++ );
		p--;
		return(p);
	}
	else {
		return(p);
	}
}
/*#$%
========================================================
	末尾のスペースとＴＡＢを取る
--------------------------------------------------------
	s : 文字列のポインタ
--------------------------------------------------------
	文字列のポインタ
--------------------------------------------------------
========================================================
*/
char *__fastcall clipsp(char *s)
{
	char	*p;

	if( *s ){
		for( p = lastp(s); p >= s; p-- ){
			if( (*p == ' ')||(*p == 0x09) ){
				*p = 0;
			}
			else {
				break;
			}
		}
	}
	return(s);
}
//---------------------------------------------------------------------------
LPCSTR __fastcall _strdmcpy(LPSTR t, LPCSTR p, char c)
{
	for( ; *p && (*p != c); p++, t++ ) *t = *p;
	*t = 0;
	if( *p == c ) p++;
	return(p);
}
const char *__fastcall StrDlmCpy(char *t, const char *p, char Dlm, int len)
{
	const char _tt1[]="[{(｢<";
	const char _tt2[]="]})｣>";
	const char	*pp;
	int			r = FALSE;

	char	Key;
	if( (pp = strchr(_tt2, Dlm))!=NULL ){
		Key = _tt1[pp - _tt2];
	}
	else {
		Key = 0;
	}
	int	f, k;
	for( f = k = 0; *p;  p++ ){
		if( k ){															// 漢字２バイト目
			k = 0;
		}
		else if(  _mbsbtype((const unsigned char *)p, 0) == _MBC_LEAD ){	// 漢字１バイト目
			k = 1;
		}
		else if( *p == Key ){
			f++;
		}
		else if( *p == Dlm ){
			if( f ){
				f--;
			}
			else {
				r = TRUE;
				p++;
				break;
			}
		}
		if( len ){
			*t++ = *p;
			len--;
		}
	}
	*t = 0;
	return (r == TRUE) ? p : NULL;
}

const char *__fastcall StrDlmCpyK(char *t, const char *p, char Dlm, int len)
{
	const char _tt1[]="[{(｢<";
	const char _tt2[]="]})｣>";
	const char	*pp;
	int			r = FALSE;

	char	Key;
	if( (pp = strchr(_tt2, Dlm))!=NULL ){
		Key = _tt1[pp - _tt2];
	}
	else {
		Key = 0;
	}
	int	f, k;
	for( f = k = 0; *p;  p++ ){
		if( k ){															// 漢字２バイト目
			k = 0;
		}
		else if(  _mbsbtype((const unsigned char *)p, 0) == _MBC_LEAD ){	// 漢字１バイト目
			k = 1;
		}
		else if( (pp = strchr(_tt1, *p))!=NULL ){
			Key = _tt2[pp - _tt1];
			f++;
		}
		else if( f && (*p == Key) ){
			f--;
			Key = 0;
		}
		else if( *p == Dlm ){
			if( !f ){
				r = TRUE;
				p++;
				break;
			}
		}
		if( len ){
			*t++ = *p;
			len--;
		}
	}
	*t = 0;
	return (r == TRUE) ? p : NULL;
}
//---------------------------------------------------------------------------
void __fastcall StrCopy(LPSTR t, LPCSTR s, int n)
{
	for( ; *s && n; n--, s++, t++){
		*t = *s;
	}
	*t = 0;
}
//---------------------------------------------------------------------------
char __fastcall LastC(LPCSTR p)
{
	char c = 0;

	if( *p ){
		c = *(p + strlen(p) - 1);
	}
	return c;
}

/*#$%
========================================================
	拡張子を得る
--------------------------------------------------------
	p : 文字列のポインタ
--------------------------------------------------------
	文字コード
--------------------------------------------------------
========================================================
*/
LPCSTR __fastcall GetEXT(LPCSTR Fname)
{
	if( *Fname ){
		LPCSTR p = Fname + (strlen(Fname) - 1);
		for( ; p > Fname; p-- ){
			if( *p == '.' ) return p+1;
		}
	}
	return "";
}

void __fastcall SetEXT(LPSTR pName, LPSTR pExt)
{
	if( *pName ){
		LPSTR	p;

		for( p = &pName[strlen(pName)-1]; p >= pName; p-- ){
			if( *p == '.' ){
				strcpy(p, pExt);
				return;
			}
		}
		strcat(pName, pExt);
	}
}

void __fastcall SetCurDir(LPSTR t, int size)
{
	if( !::GetCurrentDirectory(size-1, t) ){
		*t = 0;
	}
	else {
		if( LastC(t) != '\\' ){
			strcat(t, "\\");
		}
	}
}

void __fastcall SetDirName(LPSTR t, LPCSTR pName)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char name[_MAX_FNAME];
	char ext[_MAX_EXT];
	CString	Dir;

	::_splitpath( pName, drive, dir, name, ext );
	Dir = drive;
	Dir += dir;
	strncpy(t, Dir, 128);
}

///----------------------------------------------------------------
///  CR/LFを削除
///
void __fastcall ClipLF(LPSTR sp)
{
	for( ; *sp; sp++ ){
		if( (*sp == LF)||(*sp == CR) ){
			*sp = 0;
			break;
		}
	}
}
///----------------------------------------------------------------
///  コメントを削除（スペースおよびＴＡＢも削除）
///
void __fastcall DeleteComment(LPSTR bf)
{
	LPSTR sp, tp;

	for( sp = tp = bf; *sp; sp++ ){
		if( (*sp == ';')||(*sp == CR)||(*sp == LF) ){
			break;
		}
		else if( (*sp!=' ')&&(*sp!=TAB) ){
			*tp++ = *sp;
		}
	}
	*tp = 0;
}

LPSTR __fastcall FillSpace(LPSTR s, int n)
{
	LPSTR p = s;
	int i = 0;
	for( ; *p && (i < n); i++, p++ );
	for( ; i < n; i++, p++ ){
		*p = ' ';
	}
	*p = 0;
	return s;
}
///----------------------------------------------------------------
LPCSTR __fastcall SkipToValue(LPCSTR sp)
{
	while(*sp){
		if( isdigit(*sp) ) break;
        if( (*sp == '+') || (*sp == '-') || (*sp == '.') ) break;
        sp++;
    }
    return sp;
}
///----------------------------------------------------------------
///  ホワイトスペースのスキップ
///
LPSTR __fastcall SkipSpace(LPSTR sp)
{
	for( ; *sp; sp++ ){
		if( (*sp != ' ')&&(*sp != TAB) ) break;
	}
	return sp;
}

LPCSTR __fastcall SkipSpace(LPCSTR sp)
{
	for( ; *sp; sp++ ){
		if( (*sp != ' ')&&(*sp != TAB) ) break;
	}
	return sp;
}
///----------------------------------------------------------------
LPSTR __fastcall DelLastSpace(LPSTR t)
{
	int l = strlen(t);
	if( l ){
		LPSTR p;
		for( p = t + l - 1; p >= t; p-- ){
			if( (*p == ' ') || (*p == TAB) ){
				*p = 0;
			}
			else {
				break;
			}
		}
	}
	return t;
}
///----------------------------------------------------------------
///  デリミッタ分解を行う
///
LPSTR __fastcall StrDlm(LPSTR &t, LPSTR p)
{
	return StrDlm(t, p, ',');
}

///----------------------------------------------------------------
///  デリミッタ分解を行う
///
LPSTR __fastcall StrDlm(LPSTR &t, LPSTR p, char c)
{
	int		f, k;
	LPSTR	d1=NULL;
	LPSTR	d2=NULL;

	t = p;
	f = k = 0;
	while(*p){
		if( k ){															// 漢字２バイト目
			if( (c == -1) && !sys.m_MsgEng ){
				k |= *p & 0x00ff;
                if( k == 0x8140 ){
					p--;
					*p = 0;
					p+=2;
					break;
                }
            }
			k = 0;
		}
		else if(  _mbsbtype((const unsigned char *)p, 0) == _MBC_LEAD ){	// 漢字１バイト目
			k = *p & 0x00ff;
            k = k << 8;
		}
		else if( *p == 0x22 ){
			if( !f ){
				if( d1 == NULL ) d1 = p+1;
				f++;
			}
			else {
				d2 = p;
				f--;
			}
		}
		else if( !f && ((*p == c) || ((c == -1) && strchr(" /,;;.", *p))) ){
			*p = 0;
			p++;
			break;
		}
		p++;
	}
	if( (d1!=NULL)&&(d2!=NULL) ){
		if( ((t+1)==d1) && ( ((p-2)==d2)||((p-1)==d2) ) ){
			t = d1;
			*d2 = 0;
		}
	}
	return(p);
}

///----------------------------------------------------------------
///
void __fastcall ChgString(LPSTR t, char a, char b)
{
	for( ; *t; t++ ){
		if( *t == a ) *t = b;
	}
}

///----------------------------------------------------------------
///
void __fastcall DelChar(LPSTR t, char a)
{
	for( ; *t; t++ ){
		if( *t == a ){
			strcpy(t, t+1);
			t--;
		}
	}
}

/*#$%
========================================================
	デシマルアスキーを数値に変換する
--------------------------------------------------------
	p : 文字列のポインタ
	n : 変換桁数
--------------------------------------------------------
	数値
--------------------------------------------------------
========================================================
*/
int __fastcall atoin(const char *p, int n)
{
	int	d;

	for( d = 0; *p && n; p++, n-- ){
		d *= 10;
		d += (*p & 0x0f);
	}
	return(d);
}
/*#$%
========================================================
	１６進アスキーを数値に変換する
--------------------------------------------------------
	p : 文字列のポインタ
	n : 変換桁数
--------------------------------------------------------
	数値
--------------------------------------------------------
========================================================
*/
int __fastcall htoin(const char *p, int n)
{
	if( *p == 'x' ) return 0;
	int	d;

	for( d = 0; *p && n; p++, n-- ){
		d = d << 4;
		d += (*p & 0x0f);
		if( *p >= 'A' ) d += 9;
	}
	return(d);
}
//---------------------------------------------------------------------------
void __fastcall SetMBCP(BYTE charset)
{
	sys.m_Charset = charset;
	UINT cp;
	switch(charset){
		case SHIFTJIS_CHARSET:
			cp = 932;
			break;
		case HANGEUL_CHARSET:
			cp = 949;
			break;
        case JOHAB_CHARSET:
			cp = 1361;
        	break;
		case CHINESEBIG5_CHARSET:   //
			cp = 950;
			break;
		case 134:       // 簡略
			cp = 936;
			break;
        case ANSI_CHARSET:
        case SYMBOL_CHARSET:
        	cp = 1252;
            break;
		default:
			cp = _MB_CP_ANSI;
			break;
	}
	if( cp != _MB_CP_ANSI ){
		CPINFO info;
		if( GetCPInfo(cp, &info) != TRUE ){
			cp = _MB_CP_ANSI;
		}
	}
	_setmbcp(cp);
}

//---------------------------------------------------------------------------
UCOL __fastcall GetGrade2(UCOL s[2], int x, int xw)
{
	if( x < 0 ) x = 0;
	if( x > xw ) x = xw;
	UCOL    r;
	r.c = 0;
	int c = s[0].b.b + ((int(s[1].b.b) - int(s[0].b.b)) * x / xw);
	if( c < 0 ) c = 0;
	if( c > 255 ) c = 255;
	r.b.b = BYTE(c);
	c = s[0].b.g + ((int(s[1].b.g) - int(s[0].b.g)) * x / xw);
	if( c < 0 ) c = 0;
	if( c > 255 ) c = 255;
	r.b.g = BYTE(c);
	c = s[0].b.r + ((int(s[1].b.r) - int(s[0].b.r)) * x / xw);
	if( c < 0 ) c = 0;
	if( c > 255 ) c = 255;
	r.b.r = BYTE(c);
	return r;
}
///----------------------------------------------------------------
void __fastcall NumCopy(LPSTR t, LPCSTR p)
{
	p = SkipSpace(p);
	for( ; *p; p++ ){
		if( isdigit(*p) ){
			*t++ = *p;
		}
		else {
			break;
		}
	}
	*t = 0;
}
///----------------------------------------------------------------
///  数字が含まれるかどうか調べる
///
int __fastcall IsNumbs(LPCSTR p)
{
	for( ; *p; p++ ){
		if( isdigit(*p) ) return 1;
	}
	return 0;
}
///----------------------------------------------------------------
///  数字が含まれるかどうか調べる
///
int __fastcall IsNumbAll(LPCSTR p)
{
	for( ; *p; p++ ){
		if( !isdigit(*p) ) return 0;
	}
	return 1;
}
///----------------------------------------------------------------
///  数字が含まれるかどうか調べる
///
int __fastcall IsAlphas(LPCSTR p)
{
	for( ; *p; p++ ){
		if( isalpha(*p) ) return 1;
	}
	return 0;
}
///----------------------------------------------------------------
int __fastcall IsAlphaAll(LPCSTR p)
{
	for( ; *p; p++ ){
		if( !isalpha(*p) ) return FALSE;
	}
	return TRUE;
}
///----------------------------------------------------------------
///  RSTかどうか調べる
///
int __fastcall IsRST(LPCSTR p)
{
	if( strlen(p) < 3 ) return 0;
	if( (*p < '1') || (*p > '5') ) return 0;
	p++;
	if( (*p < '1') || (*p > '9') ) return 0;
	p++;
	if( (*p < '1') || (*p > '9') ) return 0;
	return 1;
}
///----------------------------------------------------------------
int __fastcall IsCallChar(char c)
{
	if( !isalpha(c) && !isdigit(c) && (c != '/') ) return 0;
	if( islower(c) ) return 0;
	return 1;
}
///----------------------------------------------------------------
///  コールサインかどうか調べる
///
int __fastcall IsCall(LPCSTR p)
{
	int l = strlen(p);
	if( l > 16 ) return 0;
	if( l < 3 ) return 0;
	if( isdigit(*p) ){					// 先頭が数字
		if( l <= 3 ) return 0;				// 3文字以下の時NG
		if( isdigit(*(p+1)) ) return 0;		// 2文字目が数字の時NG
	}
	if( isdigit(LastC(p)) ){			// 最後が数字
		if( l <= 4 ) return 0;				// ４文字以下の時NG
//		if( !strchr(p, '/')==NULL ) return 0;	// /が含まれていない時NG
//		if( p[l-2] != '/' ) return 0;		// 最後の１つ前が/以外の時NG
	}
	int i;
	for( i = 0; i < l-1; i++, p++ ){
		if( isdigit(*p) ) return 1;		// 数字が含まれていればOK
	}
	return 0;
}
/*#$%
======================================================
	ポータブル表記を調べる
------------------------------------------------------
	p : コールサイン文字列のポインタ
------------------------------------------------------
	数値のポインタ
------------------------------------------------------
======================================================
*/
static LPSTR __fastcall chkptb(LPSTR p)
{
	if( *p ){
		p = lastp(p);
		if( isdigit(*p) && ((*(p-1))=='/') ){
			return(p);
		}
	}
	return(NULL);
}

/*#$%
======================================================
	ポータブル表記の入れ替え
------------------------------------------------------
	s : コールサイン文字列のポインタ
------------------------------------------------------
------------------------------------------------------
	元の文字列を破壊する
======================================================
*/
void __fastcall chgptb(LPSTR s)
{
	LPSTR	p, t;

	if( (p = chkptb(s))!=NULL ){
		t = p;
		for( p--; p > s; p-- ){
			if( isdigit(*p) ){
				*p = *t;
				t--;
				*t = 0;
				break;
			}
		}
	}
}


//---------------------------------------------------------------------------
void __fastcall KeyEvent(const short *p)
{
	for( ; *p; p++ ){
		keybd_event(BYTE(*p), 0, *p&0x8000 ? KEYEVENTF_KEYUP : 0, 0);
	}
}
//---------------------------------------------------------------------------
BOOL __fastcall StrWindowsVer(LPSTR t)
{
	*t = 0;
	OSVERSIONINFO osvi;

	memset(&osvi, 0, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if( !GetVersionEx(&osvi) ) return FALSE;

	switch (osvi.dwPlatformId){
		case VER_PLATFORM_WIN32_NT:		// NT, 2000, XP
			if( osvi.dwMajorVersion <= 4 ){
    	        strcat(t, "NT");
			}
	        else if( osvi.dwMajorVersion == 5 ){
			    if( osvi.dwMinorVersion == 0 ){
					strcat(t, "2000");
				}
				else if( osvi.dwMinorVersion == 1 ){
					strcat(t, "XP");
				}
			}
			break;
		case VER_PLATFORM_WIN32_WINDOWS:	// 95, 98, 98SE, ME
			if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0){
				strcat(t, "95");
				if( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' ){
					strcat(t, " OSR2" );
				}
			}
			else if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10){
				strcat(t, "98");
				if( osvi.szCSDVersion[1] == 'A' ){
	                strcat(t, "SE" );
    			}
            }
            else if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90){
				strcat(t, "ME");
            }
			break;
		default:
      		break;
   }
   return TRUE;
}
//---------------------------------------------------------------------------
CMBCS::CMBCS()
{
	m_pLead = NULL;
	m_Charset = ANSI_CHARSET;
}
//---------------------------------------------------------------------------
CMBCS::~CMBCS()
{
	if( m_pLead ) delete m_pLead;
}
//---------------------------------------------------------------------------
void __fastcall CMBCS::Create(void)
{
//	FILE *fp = fopen("DbgLog.txt", "wt");
	if( !m_pLead ) m_pLead = new BYTE[256];
    for( int i = 0; i < 256; i++ ){
		m_pLead[i] = (_mbsbtype((unsigned char *)&i, 0) == _MBC_LEAD);
//        fprintf(fp, "%02X:%d\n", i, m_pLead[i]);
    }
//    fclose(fp);
}
//---------------------------------------------------------------------------
void __fastcall CMBCS::Create(BYTE charset)
{
	BYTE bak_charset = sys.m_Charset;
	m_Charset = charset;
	SetMBCP(charset);
	Create();
    SetMBCP(bak_charset);
}
//---------------------------------------------------------------------------
BOOL __fastcall CMBCS::IsLead(BYTE c)
{
	if( !m_pLead ) return FALSE;
    return m_pLead[c];
}
//---------------------------------------------------------------------------
BOOL __fastcall CMBCS::IsLead(const unsigned char *p)
{
	return IsLead(*p);
}
//---------------------------------------------------------------------------
#define _SWAP(c) WORD((c<<8)|(c>>8))
const CONVALPHA	_tConvAlphaJA[]={
	{_SWAP('　'), ' '},
//	{_SWAP('！'), '!'},
//	{_SWAP('”'), '"'},
//	{_SWAP('＃'), '#'},
//	{_SWAP('＄'), '$'},
//	{_SWAP('％'), '%'},
//	{_SWAP('＆'), '&'},
//	{_SWAP('’'), 0x27},
//	{_SWAP('（'), '('},
//	{_SWAP('）'), ')'},
//	{_SWAP('＝'), '='},
//	{_SWAP('－'), '-'},
//	{_SWAP('＠'), '@'},
//	{_SWAP('＋'), '+'},
//	{_SWAP('＊'), '*'},
//	{_SWAP('；'), ';'},
//	{_SWAP('：'), ':'},
//	{_SWAP('＜'), '<'},
//	{_SWAP('＞'), '>'},
	{_SWAP('，'), ','},
	{_SWAP('．'), '.'},
//	{_SWAP('？'), '?'},
	{_SWAP('／'), '/'},
    {0, 0},
};
const CONVALPHA	_tConvAlphaBV[]={
	{0xa140, ' '},
	{0xa14d, ','},
	{0xa14f, '.'},
//	{0xa151, ';'},
//	{0xa152, ':'},
//	{0xa153, '?'},
//	{0xa248, '%'},
//	{0xa249, '@'},
    {0, 0},
};
const char _tInhibit[]="!\x22#$%&'()=-^~|\\@`[{;+*:}]<>?_";
//---------------------------------------------------------------------------
static int __fastcall ConvChar(const CONVALPHA *pAlpha, int code)
{
	for( ; pAlpha->cMBCS; pAlpha++ ){
		if( pAlpha->cMBCS == code ) return pAlpha->cASCII;
    }
    return code;
}
//---------------------------------------------------------------------------
int __fastcall CMBCS::ConvAlpha(int code)
{
	switch(m_Charset){
		case SHIFTJIS_CHARSET:		// JA
			if( (code >= 0x8140) && (code <= 0x81ff) ){
				code = ConvChar(_tConvAlphaJA, code);
            }
            else if( (code >= 0x824f) && (code <= 0x8258) ){
				code -= (0x824f - 0x30);
            }
            else if( (code >= 0x8260) && (code <= 0x8279) ){
				code -= (0x8260 - 0x41);
            }
            else if( (code >= 0x8281) && (code <= 0x829a) ){
				code -= (0x8281 - 0x61);
            }
        	break;
		case HANGEUL_CHARSET:		// HL
		case 134:       			// BY
			if( code == 0xa1a1 ){
				code = 0x20;
            }
            else if( (code >= 0xa3a1) && (code <= 0xa3ff) ){
				int c = code - (0xa3a1 - 0x21);
				if( !strchr(_tInhibit, c) ) code = c;
            }
			break;
		case CHINESEBIG5_CHARSET:   // BV
			if( (code >= 0xa140) && (code <= 0xa249) ){
				code = ConvChar(_tConvAlphaBV, code);
            }
			else if( (code >= 0xa2af) && (code <= 0xa2b9) ){
				code -= (0xa2af - 0x30);
            }
            else if( (code >= 0xa2cf) && (code <= 0xa2e8) ){
				code -= (0xa2cf - 0x41);
            }
            else if( (code >= 0xa2e9) && (code <= 0xa2fe) ){
				code -= (0xa2cf - 0x61);
            }
            else if( (code >= 0xa340) && (code <= 0xa343) ){
				code -= (0xa340 - 'w');
            }
            break;
        default:
        	break;
    }
	return code;
}


short	mend[14] = {31,31,28,31,30,31,30,31,31,30,31,30,31,31} ;
short	summ[13] = {0,0,31,59,90,120,151,181,212,243,273,304,334};

static int cleap(int yy)
{
	if ((yy % 4) != 0)
		return 0;
	else if (((yy % 100) == 0) && ((yy % 400) != 0))
		return 0;
	else
		return 1;
}

static int mcleap(int y, int m)
{
	if (m == 2)
		return cleap(y) ;
	else
		return 0 ;
}

void __fastcall GPS2SystemTime(ULONG gps, SYSTEMTIME *sp)
{
	int 	s,yr,m;
	long	d;

	d = (gps / 86400) + 6 ;
	yr = 1980 ;
	while ((d > 0) && (yr <= 2060))
	{
		s = cleap(yr) + 365;
		d -= s;
		yr++ ;
	}
	if (yr >= 2060)
	{
		memset(sp, 0, sizeof(SYSTEMTIME));
		return;
	}
	else
	{
		d += s ;
		yr-- ;
	}
	m = 1 ;
	while(d > 0)
	{
		d = d - (mend[m] + mcleap(yr,m));
		m++ ;
	}
	m-- ;
	d = d + (mend[m] + mcleap(yr,m));

/*
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
*/
	sp->wYear = WORD(yr);
	sp->wMonth = WORD(m);
	sp->wDay = WORD(d);
	sp->wHour = WORD((gps % 86400) / 3600);
	sp->wMinute = WORD((gps % 3600) / 60);
	sp->wSecond = WORD(gps % 60);
}

ULONG __fastcall SystemTime2GPS(SYSTEMTIME *sp)
{
	int 	dd;
	ULONG	gps;
/*
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
*/

	if (sp->wYear < 1980){
		return 0;
	}
	if ((sp->wYear == 1980) && (sp->wMonth == 1) && (sp->wDay < 6)){
		return 0;
	}

	dd = (sp->wYear - 1980) * 365 + (sp->wYear - 1977) / 4 + summ[sp->wMonth]
		+ (sp->wMonth + 9) /12 * cleap(sp->wYear) + sp->wDay - 6;
	gps = dd * 86400 + (ULONG)sp->wHour * 3600 + sp->wMinute * 60 + sp->wSecond;

	return gps;
}
//***************************************************************************
//CLIBLクラス
//***************************************************************************
//
//
//---------------------------------------------------------------------------
void __fastcall CLIBL::Alloc(void)
{
	int am = m_AMax ? (m_AMax * 2) : 16;
	LIBD *pN = new LIBD[am];
	if( m_pBase != NULL ){
		memcpy(pN, m_pBase, sizeof(LIBD)*m_Count);
		delete m_pBase;
	}
	m_pBase = pN;
	m_AMax = am;
}
//---------------------------------------------------------------------------
void __fastcall CLIBL::Delete(void)
{
	if( m_pBase != NULL ){
		LIBD *cp = m_pBase;
		for( int i = 0; i < m_Count; i++, cp++ ){
			delete cp->pName;
			::FreeLibrary(cp->hLib);
		}
		delete m_pBase;
	}
	m_pBase = NULL;
	m_AMax = 0;
	m_Count = 0;
}
//---------------------------------------------------------------------------
void __fastcall CLIBL::Add(LPCSTR pName, HINSTANCE hLib)
{
	if( m_Count >= m_AMax ) Alloc();
	LIBD *cp = &m_pBase[m_Count];
	cp->pName = StrDupe(pName);
	cp->hLib = hLib;
	m_Count++;
}
//---------------------------------------------------------------------------
HINSTANCE __fastcall CLIBL::LoadLibrary(LPCSTR pName)
{
	LIBD *cp = m_pBase;
	for( int i = 0; i < m_Count; i++, cp++ ){
		if( !strcmpi(cp->pName, pName) ){
			if( cp->hLib == NULL ){
				::SetCurrentDirectory(sys.m_BgnDir);
				cp->hLib = ::LoadLibrary(pName);
			}
			return cp->hLib;
		}
	}
	HINSTANCE hLib = ::LoadLibrary(pName);
	if( hLib != NULL ) Add(pName, hLib);
	return hLib;
}
//---------------------------------------------------------------------------
void __fastcall CLIBL::DeleteLibrary(HINSTANCE hLib)
{
	if( hLib == NULL ) return;

	LIBD *cp = m_pBase;
	for( int i = 0; i < m_Count; i++, cp++ ){
		if( cp->hLib == hLib ){
			::FreeLibrary(hLib);
			cp->hLib = NULL;
			break;
		}
	}
}
//---------------------------------------------------------------------------
// DCへのメッセージの描画
void __fastcall DrawMessage(CDC *pDC, int XW, int YW, LPCSTR p, int Pos)
{
	if( !*p ) return;

	int xr, xl, vc;
	CSize fs = pDC->GetTextExtent(p);
	switch(Pos){
		case 0:     // 中央
			xr = fs.cx;
			xl = (XW - xr)/2;
			xr += xl;
			vc = YW / 2;
			break;
		case 1:     // 左上
			xl = 6;
			xr = xl + fs.cx;
			vc = fs.cy*2/3 + 1;
			break;
        case 2:		// 右上
        	xr = XW - 8;
			xl = xr - fs.cx;
			vc = fs.cy*2/3 + 1;
            break;
        case 3:		// 右下
			xr = XW - 8;
            xl = xr - fs.cx;
            vc = YW - fs.cy + 2;
            break;
		case 4:     // 左下
			xl = 6;
			xr = xl + fs.cx;
            vc = YW - fs.cy + 2;
			break;
        default:
        	xl = Pos - fs.cx/2;
            xr = xl + fs.cx;
            vc = fs.cy*2/3;
        	break;
	}
	CPen penb(PS_SOLID, 1, RGB(0,0,0));
	CBrush brushw(RGB(255,255,255));
	CPen *pOldPen = pDC->SelectObject(&penb);
	CBrush *pOldBrush = pDC->SelectObject(&brushw);
    int yo = fs.cy * 2/3;
	pDC->RoundRect(xl-5, vc-yo, xr+5, vc+yo, 10, 10);
	pDC->SetTextColor(RGB(0,0,0));
	pDC->TextOut(xl, vc-fs.cy/2, p);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	penb.DeleteObject();
	brushw.DeleteObject();
}
//---------------------------------------------------------------------------
