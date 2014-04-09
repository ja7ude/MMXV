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

//---------------------------------------------------------------------------
#ifndef DSPH
#define DSPH
#include <stdio.h>
#include <math.h>

#include "ComLib.h"
#include "mfsk.h"
#include "fft.h"
class CMMVARICtrl;
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#define	RXMAX	9
//---------------------------------------------------------------------------
#define	MODGAIN		16384.0
#define	LEVELMAX	2048
//---------------------------------------------------------------------------
#define	LOSTMSGTIME	3
//---------------------------------------------------------------------------
enum {
    loopINTERNAL,
    loopEXTERNAL,
};
enum {
	txRX,
    txINTERNAL,
    txEXTERNAL,
};

#define	iniwMETRIC	1
#define	iniwLIMIT	2
#define	iniwBOTH	3
//---------------------------------------------------------------------------
extern double  SAMPFREQ;
extern int g_tBpfTaps[];
extern const DWORD _tBitData[];

enum {
	MODE_GMSK,
    MODE_FSK,
	MODE_FSKW,
    MODE_BPSK,
    MODE_N_BPSK,
    MODE_RTTY,
    MODE_U_RTTY,
    MODE_mfsk_L,
    MODE_mfsk_U,
    MODE_END,
};
//---------------------------------------------------------------------------
BOOL __fastcall IsRTTY(int m);
BOOL __fastcall Is170(int m);
BOOL __fastcall IsBPSK(int m);
BOOL __fastcall IsFSK(int m);
BOOL __fastcall IsMFSK(int m);
#define	MIN_SAMP	5000
#define	MAX_SAMP	51000
//---------------------------------------------------------------------------
#define	CARRIERFREQ		1750
#define	SPEED		31.25
#define	MIN_SPEED	20
#define	MAX_SPEED	300
//---------------------------------------------------------------------------
#define	PSK_OUTFAC	0.70710678118654752440084436210485
//---------------------------------------------------------------------------
#define	MIN_CARRIER	250
//#define	MAX_CARRIER	2700
//---------------------------------------------------------------------------
#define	PI			3.1415926535897932384626433832795
#define	MULI		0.000030517578125
#define	ABS(c)		(((c)<0)?(-(c)):(c))
#define	CIRCULATE(A,B)	((A)%(B))
//--------------------------------------------------------------------------
inline double Limit(double x, double min, double max)
{
	if( x < min ) return min;
    if( x > max ) return max;
    return x;
}
//---------------------------------------------------------------------------
#define	FIFOMAX	256
class CFifo
{
private:
	int		m_rc;
	int		m_wc;
	int		m_cc;
	int		m_bf[FIFOMAX];
	int		m_BusyLimit;

public:
	CFifo(void){
		m_cc = m_rc = m_wc = 0;
		m_BusyLimit = FIFOMAX / 2;
	};
	void __fastcall Clear(void){
		m_cc = m_rc = m_wc = 0;
	};
	void __fastcall PutData(int c){
		m_bf[m_wc] = c;
		m_wc++;
		if( m_wc >= FIFOMAX ) m_wc = 0;
		m_cc++;
	};
	int __fastcall GetData(void){
		int c = m_bf[m_rc];
		m_rc++;
		if( m_rc >= FIFOMAX ) m_rc = 0;
		m_cc--;
		return c;
	};
	int __fastcall GetCount(void){
		return m_cc;
	}
	int __fastcall IsBusy(void){
		return (m_cc < m_BusyLimit) ? FALSE : TRUE;
	}
};
//---------------------------------------------------------------------------
enum {
	ffLPF,
	ffHPF,
	ffBPF,
	ffBEF,
	ffGAUSSIAN,
};
typedef struct {
	int		n;
	int		typ;
	double	fs;
	double	fcl;
	double	fch;
	double	att;
	double	gain;
	double	fc;
	double	*hp;
}FIR;
//---------------------------------------------------------------------------
class CSinTable
{
public:
	double	*m_tSin;
	int		m_Size;
public:
	CSinTable();
    ~CSinTable();
};
//---------------------------------------------------------------------------
class CVARICODE
{
public:
	int		m_TMax;
    int		m_Max;
	DWORD	*m_tEncode;
    BYTE	m_tIndex2Mbcs[128];
public:
	UINT __fastcall Index2Mbcs(int index, BOOL fJA);
	int __fastcall Mbcs2Index(UINT mbcs, BOOL fJA);

public:
	CVARICODE();
    ~CVARICODE();
	void __fastcall Init(void);
	void __fastcall Create(int max);
    DWORD __fastcall Encode(int &n, DWORD d);
    int __fastcall Decode(DWORD d);

	BOOL __fastcall SaveTable(LPCSTR pName);
	void __fastcall SaveSource(LPCSTR pName);
    void __fastcall SaveBin(LPCSTR pName);
	BOOL __fastcall LoadBin(LPCSTR pName);

	int __fastcall DecodeMFSK(DWORD d);
	int __fastcall EncodeMFSK(BYTE c);
};
//---------------------------------------------------------------------------
class CBAUDOT
{
private:
	int		m_OutFig;
    int		m_CodeB4;
private:
	int __fastcall GetOneCode(int &fig, int code);
	int __fastcall DblCode(int code);
public:
	CMMVARICtrl	*m_pMainCtrl;

public:
	CBAUDOT();
	~CBAUDOT();

	int __fastcall GetCode(int &count, int code);
	inline void Reset(void){m_OutFig = -1; m_CodeB4 = 0;};
};
//---------------------------------------------------------------------------
#pragma pack(push, 1)	// パックの指示
typedef struct {
	BYTE	Code;
	BYTE	Fig;
}BCODETBL;
#pragma pack(pop)	// パック解除の指示

class CRTTY
{
private:
	int m_outfig;
	BCODETBL	m_TBL[24*4];

public:
	int	m_CodeSet;
	int	m_fig;
	int	m_uos;

	int	m_txuos;
	CRTTY();
	char ConvAscii(int d);
	int ConvRTTY(char d);
	int ConvRTTY(BYTE *t, LPCSTR p);
	inline int IsChar(int d){
		d &= 0x000000ff;
		if( !d ) return 0;
		if( d == 0x000000ff ) return 0;
		return 1;
	};
	inline void ClearTX(void){m_outfig = 3;};
	inline void ClearRX(void){m_fig = 0;};

	int GetShift(char d);
	char InvShift(char c);
	void SetCodeSet(void);
};

//---------------------------------------------------------------------------
class CBFifo
{
private:
	int		m_WP;
    int		m_RP;
    int		m_CNT;
    int		m_MAX;
    BOOL	*m_pBase;
    BOOL	m_D;
public:
	CBFifo();
	~CBFifo();

    void __fastcall Create(int max);
	void __fastcall PutFlag(BOOL f);
    BOOL __fastcall GetFlag(void);
};


//---------------------------------------------------------------------------
class CVCO
{
private:
	double	m_c1;	// VCOの利得
	double	m_c2;	// フリーランニング周波数
	double	m_z;

	double	m_FreeFreq;
	double	m_SampleFreq;
	int		m_TableSize;
    double	m_TableCOS;
public:
	CVCO();
	~CVCO();
	void __fastcall InitPhase(void);
	void __fastcall SetGain(double gain);
	void __fastcall SetSampleFreq(double f);
	void __fastcall SetFreeFreq(double f);
	double __fastcall Do(void);
	double __fastcall Do(double d);
	double __fastcall DoCos(void);
};

//---------------------------------------------------------------------------
#define	IIRMAX	16
#define	IIRVLIMIT	(1.0e-37)
class CIIR
{
public:
	double	m_rp;
	double	m_A[IIRMAX*2];
	double	m_B[IIRMAX*3];
	int		m_order;
	int		m_bc;
private:
	double	m_Z[IIRMAX*3];
    int		m_orderH;
public:
	CIIR();
	~CIIR();
	void __fastcall Create(int type, double fc, double fs, int order, int bc, double rp);
	double __fastcall Do(double d);
	void __fastcall Clear(void);
};

//---------------------------------------------------------------------------
class CIIRTANK
{
private:
	double	z1, z2;
public:
	double	a0;
	double	b1, b2;

    double	m_Freq;
    double	m_BW;
    double	m_SampleFreq;
private:
	void __fastcall SetFreq(double f, double smp, double bw);
public:
	CIIRTANK();
	inline void SetSampleFreq(double f){m_SampleFreq = f; Create();};
	inline void SetFreq(double f){m_Freq = f; Create();};
	void __fastcall Create(void);
	double __fastcall Do(double d);
};

class CIIRTANK2
{
public:
    double	m_Freq;
    double	m_BW;
    double	m_SampleFreq;
private:
	CIIRTANK	m_Tank1;
    CIIRTANK	m_Tank2;

public:
	CIIRTANK2();
	void __fastcall SetSampleFreq(double f);
	void __fastcall Create(void);
    double __fastcall Do(double d);
    inline CIIRTANK * GetTank1(void){return &m_Tank1;};
};
//---------------------------------------------------------------------------
class CFIR
{
private:
	int		m_Tap;
	double	*m_pZ;
    double	*m_pH;
public:
	CFIR();
    ~CFIR();
	void __fastcall Create(int tap, int type, double fs, double fcl, double fch, double att, double gain);
	double __fastcall Do(double d);
    void __fastcall SaveCoef(LPCSTR pName);
    inline double GetHD(int n){return m_pH[n];};
    inline double * GetHP(void){return m_pH;};
    inline int GetTap(void){return m_Tap;};
};

//---------------------------------------------------------------------------
// ダブルバッファによるFIRフィルタ
class CFIR2
{
private:
	int		m_Tap;
    int		m_TapHalf;
	double	*m_pZ;
    double	*m_pH;
	double	*m_pZP;

    int		m_W;
    double	m_fs;
public:
	CFIR2();
    ~CFIR2();
	void __fastcall Delete(void);
	void __fastcall Clear(void);
	void __fastcall Create(int tap, double fs);
	void __fastcall Create(int tap, int type, double fs, double fcl, double fch, double att, double gain);
	void __fastcall Create(int tap, double fs, double fcl, double fch);
	void __fastcall CreateSamp(int tap, double fs, const double *pSmpFQ, int wDB);
	double __fastcall Do(double d);
	double __fastcall Do(double *hp);
	void __fastcall Do(CLX &z, double d);

    inline double GetHD(int n){return m_pH[n];};
    inline double *GetHP(void){return m_pH;};
    inline int GetTap(void){return m_Tap;};
    inline double GetSampleFreq(void){return m_fs;};
	inline BOOL IsActive(void){return m_pZ!=NULL;};
};
//---------------------------------------------------------------------------
// ダブルバッファによるFIRフィルタ(複素数用)
class CFIRX
{
private:
	int		m_Tap;
    int		m_TapHalf;
	CLX		*m_pZ;
    double	*m_pH;
	CLX		*m_pZP;

    int		m_W;
    double	m_fs;
public:
	CFIRX();
    ~CFIRX();
	void __fastcall Clear(void);
	void __fastcall Create(int tap, int type, double fs, double fcl, double fch, double att, double gain);
	void __fastcall Do(CLX &d);

    inline double GetHD(int n){return m_pH[n];};
    inline double *GetHP(void){return m_pH;};
    inline int GetTap(void){return m_Tap;};
    inline double GetSampleFreq(void){return m_fs;};
};
//---------------------------------------------------------------------------
// スライディングFFT
class CSlideFFT
{
private:
	int 	m_Length;
	int 	m_Base;
    int		m_Tones;

	CLX		m_tData[MFSK_MAXTONES];
	CLX 	m_tWindow[MFSK_MAXTONES];

    CLX		*m_pCur;
    CLX		*m_pEnd;
	CLX		*m_pBase;

	double	m_kWindow;
private:
public:
	CSlideFFT();
	~CSlideFFT();

	void __fastcall Create(int len, int base, int tones);
	CLX* __fastcall Do(const CLX &zIn);
};
//---------------------------------------------------------------------------
// 遅延器
class CDelay
{
private:
	int		m_Delay;
	int		m_CurPnt;
    int		m_Count;
    double	*m_pStg;

public:
	CDelay(void);
    ~CDelay();

	void __fastcall Create(int delay);
	void __fastcall Reset(void);
	double __fastcall Do(const double &d);
	double __fastcall GetData(void);
    inline BOOL IsFull(void){return m_Count >= m_Delay;};
};
//---------------------------------------------------------------------------
// CICフィルタ
class CCIC
{
private:
	int		m_N;
	double	m_K;
	CDelay	m_Com;
    double	m_Z;
public:
	CCIC(void);

	void __fastcall Create(int n);
	double __fastcall Do(double d);
};
//---------------------------------------------------------------------------
// 高次のCICフィルタ
class CCICM
{
private:
	int		m_N;
    CCIC	m_tCIC[16];
public:
	CCICM(void);
	~CCICM();

    void __fastcall Create(int stages, int N);
    double __fastcall Do(double d);
};
//---------------------------------------------------------------------------
class CFAVG
{
private:
    double	m_Sum;
    double	m_Avg;
    double	m_Mul;
	int		m_Max;
    int		m_Cnt;
public:
	CFAVG();
	void __fastcall Reset(void);
	void __fastcall Reset(double d);
    void __fastcall Create(int max);
	double __fastcall DoZ(double d);
	double __fastcall Do(double d);
    inline double GetAvg(void){return m_Avg;};
    inline BOOL IsFull(void){return m_Max == m_Cnt;};
    inline BOOL IsHalf(void){return m_Max <= (m_Cnt*2);};
    inline int GetCount(void){return m_Cnt;};
};
//---------------------------------------------------------------------------
class CAMPCONT
{
private:
	double	m_Max;
    double	m_Cnt;
    double	m_ADD;
    int		m_S;
    int		m_iMax;
public:
	CAMPCONT();
	void __fastcall SetMax(int max);
    void __fastcall Reset(void);
    double __fastcall Do(int d);
};
//---------------------------------------------------------------------------
class CAVG
{
private:
	int		m_Max;
    double	m_Sum;
    double	m_Avg;
    double	*m_pZ;
    int		m_W;
    int		m_Cnt;

public:
	CAVG();
    ~CAVG();
	void __fastcall Create(int max);
	double __fastcall Do(double d);
    inline BOOL IsFull(void){return m_Cnt >= m_Max;};
    void __fastcall Reset(double d);
    void __fastcall Reset(void);
    inline double GetAvg(void){return m_Avg;};
    inline BOOL IsCreate(void){return m_pZ != NULL;};
    inline BOOL IsHalf(void){return m_Max <= (m_Cnt*2);};
};
//---------------------------------------------------------------------------
#define	DECFIR	FALSE
class CDECM2{
private:
    double	m_O;
	int		m_Count;
#if DECFIR
	CFIR2	m_FIR;
#else
    CIIR	m_IIR;
#endif
public:
    int		m_Type;
	double	m_SampleFreq;
	CMMVARICtrl *m_pMain;
public:
	CDECM2();
    void __fastcall SetSampleFreq(int type, double f);
    BOOL __fastcall Do(double d);
    inline double GetOut(void){return m_O;};
#if DECFIR
    inline CFIR2* GetFIR(void){return &m_FIR;};
#else
    inline CIIR* GetIIR(void){return &m_IIR;};
#endif
    inline void SetOut(double d){m_O = d;};
};

//--------------------------------------------------------
// CAGCクラス
class CAGC
{
private:
	double  m_Max;
	double  m_Min;
	double  m_d;
	double  m_agc;

	double	m_fc;
	double	m_Gain;
    CIIR	m_LPF;
//    CIIR	m_Level;
    int		m_Count;
    int		m_TLimit;
    CFAVG	m_AvgOver;
public:
    double	m_LimitGain;
	double	m_MonitorFreq;
	double	m_CarrierFreq;
    double	m_SampleFreq;
public:
	CAGC();
    void __fastcall Create(void);
	inline void SetSampleFreq(double f){m_SampleFreq = f; Create();};
	void __fastcall SetCarrierFreq(double f);
    void __fastcall SetFC(double fc);
    void __fastcall Reset(void);
    double __fastcall Do(double d);
	inline void ResetOver(void){m_AvgOver.Reset(1.0);};
    BOOL __fastcall GetOver(void);
};

#if DEBUG
//---------------------------------------------------------------------------
class CQSB
{
private:
	int		m_vMax, m_vMin;
	BOOL	m_fPhaseError;
    CVCO	m_VCO;
    CVCO	m_VCOP;
    int		m_tap;
    double	m_Z[TAPMAX];
public:
	__fastcall CQSB();
	__fastcall ~CQSB();
    void __fastcall Create(int min, int max, int msec, BOOL perr);
	int __fastcall Do(void);
	double __fastcall Do(double d);
};

//---------------------------------------------------------------------------
class CNoise		// M系列ノイズ N=22 (Tap=1)
{
private:
    double	m_SampleFreq;

	DWORD	m_reg;
	CIIR	m_LPF;
    CIIR	m_HPF;
public:
	__fastcall CNoise();
	void __fastcall SetSampleFreq(double f);
	void __fastcall Create(double f1, double f2);
	DWORD __fastcall Do(void);
    double __fastcall DoLPF(void);
    double __fastcall DoHPF(void);
};
#endif

/*=============================================================================
  CPHASEクラス
=============================================================================*/
#define	CPHASEX_BASEFREQ	1000.0
class CPHASEX
{
private:
//	CFIRX	m_LPF;
	CVCO	m_VCO;
    CFIR2	m_Hilbert;
    CAGC	m_AGC;

    FILE	*m_fp;
public:
    CLX		m_sig;
	CSlideFFT	m_SlideFFT;

    int			m_TONES;
	double		m_SHIFT;
	int			m_BASEPOINT;

	double	m_MixerFreq;
    double	m_SymbolLen;
	double	m_CarrierFreq;
	double	m_SampleFreq;

    double	m_dm;
    double	m_ds;
private:
public:
	CPHASEX();
    void __fastcall Create(void);
    void __fastcall SetSampleFreq(double f);
	void __fastcall SetCarrierFreq(double f);
	inline void __fastcall AddCarrierFreq(double f){
		SetCarrierFreq(m_CarrierFreq + f);
    };
	CLX* __fastcall Do(double d);
    inline CFIR2* __fastcall GetFIR(void){return &m_Hilbert;};

	void __fastcall DoFSK(double d);
    void __fastcall SetShift(double f);

    inline void SetTones(int n){
		m_TONES = n;
        Create();
    }
};

//---------------------------------------------------------------------------
class CCOLLECT
{
public:
	int		m_Max;
    int		m_Cnt;
    long	*m_pZ;
public:
	CCOLLECT();
	~CCOLLECT();
	void __fastcall Create(int max);
	void __fastcall Delete(void);
    inline void Clear(void){m_Cnt = 0;};
    void __fastcall Do(long d);
	inline long * GetZP(void){return m_pZ;};
    inline int GetMax(void){return m_Max;};
    inline int GetCount(void){return m_Cnt;};
    inline BOOL IsFull(void){return m_Max && (m_Cnt >= m_Max);};
};
//---------------------------------------------------------------------------
// CLMSクラス
#define LMSTAP  192
class CLMS
{
private:
	double	Z[LMSTAP+1];		// FIR Z Application
	double	D[LMSTAP+1];		// LMS Delay;

	double	m_lmsADJSC;			// スケール調整値
	double  m_D;
public:
	int		m_Tap;
	int		m_lmsDelay;			// LMS Delay
	int		m_lmsAGC;			// LMS AGC
	double	m_lmsMU2;			// LMS 2μ
	double	m_lmsGM;			// LMS γ
	double	H[LMSTAP+1];		// アプリケーションフィルタの係数
public:
	CLMS();

	double __fastcall Do(double d);
};

class CAFC
{
private:
public:
	BOOL	m_fChange;
    int		m_Count;
    int		m_LCount1;
    int		m_LCount2;

	CAVG	m_Avg;
	double	m_d;
	double	m_min;
    double	m_max;
    double	m_fq;
    CIIR	m_LPF;
public:
	double	m_Max, m_Min;
    double	m_A;
    double	m_B;
	double	m_Speed;
	double	m_SampleFreq;
public:
	CAFC();
	void __fastcall SetPara(double a, double b);
	void __fastcall SetTap(int tap);
	void __fastcall SetTone(double fq);
    BOOL __fastcall Do(double d);
    double __fastcall GetFreq(void){return m_fq;};
};

//---------------------------------------------------------------------------
enum {
    diddleLTR,
    diddleBLK,
};
class CENCODE
{
public:
	DWORD	m_Z[32];

	int		m_Type;
	int		m_Mode;
    int		m_cData;
    int		m_cBCount;
	int		m_out;
	int		m_sync;
    int		m_Idle;
	BOOL	m_fJA;
	BOOL	m_fTWO;

    int		m_Cnt;
	double	m_dCW;
    double	m_dTW;
    double	m_dNow;
    double	m_dNext;

    double	m_Speed;
    double	m_SampleFreq;

    BOOL	m_fCW;
	BOOL	m_Mark;
    BOOL	m_fReqRX;

    int		m_Code;

	int		m_rttyDiddle;
    int		m_rttyCWait;
    int		m_rttyDWait;
    CBAUDOT	m_BAUDOT;

    int		m_Buff[2048];
	int		m_WP;
    int		m_RP;
    int		m_CC;

	CMMVARICtrl	*m_pMainCtrl;
//    int (*m_pFunc)(void);
    CVCO	*m_pVCO;

    BOOL	m_fChar;

    // Added for MFSK
	int		m_MFSK_TONES;
	double	m_MFSK_SPEED;
	int		m_MFSK_BITS;
    UINT	m_MFSK_MASK;

	CViterbiEncode	m_Viterbi;
	UINT	m_MFSK_SHDATA;
	int 	m_MFSK_SHCount;
    CFifo	m_Fifo;
	CInterLeaver	m_InterLeaver;

private:
    int __fastcall GetChar(void);

	void __fastcall MFSKSendBit(BOOL bit);
	void __fastcall MFSKSendPair(BYTE pair);
	void __fastcall MFSKSendSymBit(BOOL bit);
	void __fastcall MFSKSendChar(int c);
	void __fastcall MFSKSendIdle(int n);
	void __fastcall MFSKSendIdle(void);

	void __fastcall MFSKSendIdleChar(void);

public:
	CENCODE();
	inline void SetVCO(CVCO *pVCO){m_pVCO = pVCO;};
	void __fastcall SetSampleFreq(double f){m_SampleFreq = f; Create();};
	void __fastcall Create(void);
	inline void SetSpeed(double b){m_Speed = b; Create();};
    void __fastcall Reset(BOOL fCW);
	void __fastcall ClearBuffer(void);
	void __fastcall SetBufferCount(int cc);
    int __fastcall Do(void);
    void __fastcall SetTmg(double d);
	void __fastcall SetCW(int f);

	void __fastcall PutChar(int c);
    BOOL __fastcall IsBuffFull(void);
	void __fastcall GetSendText(CString &cs);

    inline int GetBuffCount(void){return m_CC;};
    inline void SetType(int type){m_Type = type;};
    inline void SetDiddle(int diddle){m_rttyDiddle = diddle;};
    inline int GetDiddle(void){return m_rttyDiddle;};
    inline void SetMark(void){m_Mark = TRUE;};
    inline void SetWaitC(int n){m_rttyCWait = n;};
    inline void SetWaitD(int n){m_rttyDWait = n;};
	inline int GetWaitC(void){return m_rttyCWait;};
	inline int GetWaitD(void){return m_rttyDWait;};
	void __fastcall SetMFSKType(int type);
};

//---------------------------------------------------------------------------
// CDECFSKクラス
class CDECFSK
{
private:
public:
	int		m_Type;

	int		m_cData;
    int		m_cBCount;
    int		m_cTCount;
    int		m_cMode;
	int		m_s;
	int		m_a;

    double	m_dTW;
    double	m_dTWH;
    double	m_dBTW;
    double	m_dBTWL;
    double	m_dBTWH;
    double	m_dNow;
    double	m_dNext;
    double	m_dFree;

	int		m_cBWave;
	int		m_T;
    int		m_T2;
    int		m_T3;
    int		m_Error;

    int		m_ATCCounter;
    int		m_ATCLimitH;
    int		m_ATCLimitL;

	double	m_Threshold;
	double	m_GainA;
    double	m_GainB;
    CAVG	m_LPF;
    CAVG	m_LPFI;
    double	m_A;

	BOOL	m_fSync;
    CRTTY	m_BAUDOT;
	CFAVG	m_AvgRTTY;

	BOOL	m_fMeasClock;
    int		m_MeasCounter;
    int		m_MeasClock;
	int		m_MeasError;
    int		m_Meas1st;
public:
	int		m_Lock;
	double	m_dTmg3;
	double	m_dTmg2;
	double	m_dTmg;
	int		m_Tmg;
	int		m_Data;
	double	m_dAdj;			// タイミング補正(ppm)
    BOOL	m_fATC;
    int		m_ATCSpeed;
    int		m_ATCLimit;

	double	m_Speed;		// 伝送速度(baud)
    double	m_SampleFreq;	// Hz
private:
	BOOL __fastcall DoBAUDOT(int s, BOOL sq);

public:
	CDECFSK();
    void __fastcall Create(void);
	inline void SetSpeed(double b){m_Speed = b; Create();};
    BOOL __fastcall Do(double d, BOOL sq, BOOL fATC);
    inline void SetSampleFreq(double f){
		m_SampleFreq = f;
        Create();
    };
	void __fastcall ClearLPF(void);
	void __fastcall Reset(void);
	void __fastcall SetMeasClock(BOOL f);
    void __fastcall SetTmg(int ppm);
    inline int GetData(void){int r = m_Data; m_Data = -1; return r;};
    inline int GetTmg(void){ return m_Tmg ? 1 : -1;};
	void __fastcall CalcIntval(double d);
    BOOL __fastcall GetSyncState(void);
	void __fastcall SetATCSpeed(int atcspeed);
	void __fastcall SetATCLimit(int atclimit);

    inline void SetType(int type){m_Type = type;};
	inline void SetUOS(int type){
		if( type == 2 ){
			m_BAUDOT.m_uos = !m_BAUDOT.m_uos;
        }
        else {
			m_BAUDOT.m_uos = type;
        }
    };
    inline BOOL GetUOS(void){return m_BAUDOT.m_uos;};
	inline BOOL IsRTTYTmg(void){return m_AvgRTTY.IsHalf();};
    inline double GetRTTYTmg(void){
		return m_AvgRTTY.GetAvg() * 1000.0 / m_SampleFreq;
    };
    inline void ResetMeasRTTY(void){m_AvgRTTY.Reset();};
};

/*=============================================================================
  CPHASEクラス
=============================================================================*/
#define	MFSK_BASEFREQ	1000.0
class CPHASE
{
private:
	CVCO	m_VCO;
    CFIR2	m_Hilbert;
public:
    CLX		m_sig;
	CSlideFFT	m_SlideFFT;

    int			m_MFSK_TONES;
	double		m_MFSK_SPEED;
	int			m_MFSK_BASEPOINT;

	double	m_MixerFreq;
    double	m_SymbolLen;
	double	m_CarrierFreq;
	double	m_SampleFreq;
private:
public:
	CPHASE();
    void __fastcall Create(void);
    void __fastcall SetSampleFreq(double f);
	void __fastcall SetCarrierFreq(double f);
	inline void AddCarrierFreq(double f){
		SetCarrierFreq(m_CarrierFreq + f);
    };
	CLX* __fastcall Do(double d);
    inline CFIR2* GetFIR(void){return &m_Hilbert;};

	void __fastcall SetMFSKType(int type);
};

/*=============================================================================
  CDecMFSKクラス
=============================================================================*/
#define	AMPSTGMAX	int(9000.0*2.0/MFSK_MINSPEED)	// (MaxDemSamp + ClockAdjust) * 2 / MFSK8_MINSPEED
class CDecMFSK {
private:
	int			m_AmpStgMax;
	int			m_AmpStgPtr;
	double		m_AmpStg[AMPSTGMAX][MFSK_MAXTONES];

	CInterLeaver	m_InterLeaver;
	UINT		m_SHDATA;

	BYTE		m_SymStg[3];

	double		m_AvgMetric;

	double		m_dNow;
    double		m_dTW;
    double		m_dBTW;
    double		m_dBTWL, m_dBTWH;

	CViterbi	m_Viterbi1;
	CViterbi	m_Viterbi2;
	double		m_Metric1;
	double		m_Metric2;
	BYTE		m_ViterbiPair[2];
	UINT 		m_ViterbiPhase;

    BOOL		m_fSQ;
    BOOL		m_fDelaySQ;
	int			m_SQDelayCount;

	CFAVG		m_AvgClock;
    int			m_MeasClock;
    int			m_MeasCounter;
	int			m_MeasStage;

	CFAVG		m_AvgAFC;
    CLX			m_PrevZ;
	double		m_AFCWidth;
    double		m_kAFC;

public:
	BOOL		m_SyncState;
	BOOL		m_Tmg;
    BOOL		m_TmgLock;
	CPHASE		m_Phase;
	CFifo		m_RxData;

    int			m_MFSK_TONES;
	double		m_MFSK_SPEED;
	int			m_MFSK_BITS;
	double		m_MFSK_BW;

	BOOL		m_fLSB;
	double		m_SampleFreq;
	CMMVARICtrl	*m_pMainCtrl;
private:
	double __fastcall GetFactor(void);
	void __fastcall StreamSym(BYTE sym);
	void __fastcall RecSym(const double *pAmp);
	BYTE __fastcall DecodeSym(CLX *pFFT, double *pAmp);
	void __fastcall DoSync(void);
	void __fastcall DoMeas(void);
	void __fastcall DoAFC(CLX *pFFT);
public:
	CDecMFSK(CMMVARICtrl *pMainCtrl);
	void __fastcall SetSampleFreq(double f);
	void __fastcall SetCarrierFreq(double f);

	int __fastcall Do(CLX *pFFT, BOOL fSQ);
	int __fastcall GetData(void);
    void __fastcall Reset(void);
	void __fastcall ResetMeas(void);

    inline BOOL GetSyncState(void){return m_SyncState;};
    inline BOOL GetTmgLock(void){return TRUE;};
    inline BOOL GetTmg(void){return m_Tmg;};
    inline int GetS(void){return m_SymStg[2];};

    int	__fastcall GetClockError(void);
    inline CFIR2* GetFIR(void){return m_Phase.GetFIR();};
    inline BOOL IsMetSQ(void){
		return (m_AvgMetric >= 100);
    };
    int __fastcall GetMetric(int sw);
	void __fastcall SetTmg(int ppm);

	void __fastcall SetMFSKType(int type);
    BOOL __fastcall GetAFCShift(double &fq);
};


//---------------------------------------------------------------------------
// CDEMFSKクラス
#define	DEMAFCLIMIT		(10*11025/2048)
class CDEMFSK
{
#if DEBUG
public:
	int		m_DEBUG;
#endif
//private:
public:
	double	m_d;
	double	m_err;
    double	m_out;
    double	m_PreBPFFC;
	CFIR2	*m_pBPF;
public:
	CFIR2	m_inBPF;
	CIIR	m_LPF1;
//    CIIR	m_LPF2;
    CIIR	m_LoopLPF;
    CIIR	m_OutLPF;
    CVCO	m_VCO;
    CAGC	m_AGC;
	CAFC	m_AFC;
	CDECFSK	m_Decode;
	CFAVG	m_FAVG;
	CIIRTANK	m_TankL;
    CIIRTANK	m_TankH;
	CIIRTANK	m_TankL2;
    CIIRTANK	m_TankH2;
	BOOL	m_fRTTYTANK;
	UINT	m_AFCCount;

    double	m_BPFLimit;
//	CCICM	m_CIC;

	BOOL	m_fRTTYFFT;
	CPHASEX	m_PhaseX;

	BOOL	m_fMFSK;
	CDecMFSK	*m_pMFSK;
	CMMVARICtrl	*m_pMainCtrl;
public:
	int		m_Type;
	double	m_RxFreq;
	int		m_Lock;
	int		m_LockTh;		// 連続キャリア検出のスレッシュホールド
    double	m_FreqErr;
	double	m_DemLevel;

    double	m_Gain;
	double	m_SW;

    BOOL	m_fEnableAFC;
	BOOL	m_fAFC;

	double	m_Speed;
	double	m_CarrierFreq;
    double	m_SampleFreq;
	int		m_PreBPFTaps;
	double	m_RTTYShift;

    int			m_MFSK_TONES;
	double		m_MFSK_SPEED;
	double		m_MFSK_BW;
	int			m_MFSK_BITS;

	BOOL		m_bCollect;
	CCOLLECT	m_Collect[2];
private:
	void __fastcall DoAFC(double d);
	void __fastcall DoAFCPSK(double d);
	void __fastcall DoAFCRTTY(double d);
public:
	CDEMFSK(CMMVARICtrl *pMainCtrl);
	~CDEMFSK();
	inline void SetSampleFreq(double f){
		m_SampleFreq = f; Create();
    };
	inline void SetType(int type){m_Type = type; Create();};
	void __fastcall MakeBPF(int taps);
	void __fastcall UpdateBPF(void);
	void __fastcall SetCarrierFreq(double f);
	void __fastcall SetCollect(BOOL f, int buffersize);
	inline void SetSpeed(double b){m_Speed = b; Create();};
    inline void SetRTTYShift(double s){
    	m_RTTYShift = s;
		if( Is170(m_Type) ) Create();
    };
	void __fastcall Create();
    int __fastcall Do(double);
	int __fastcall Do(double d, BOOL fSQ, BOOL fATC);
	int __fastcall GetData(void);
	int __fastcall GetTmg(void);
	int __fastcall GetTmgLock(void);
	int __fastcall GetS(void);
	void __fastcall ResetMFSK(void);
	void __fastcall ResetMeasMFSK(void);
	BOOL __fastcall GetSyncState(void);
	int __fastcall GetClockError(void);
	void __fastcall SetTmg(int ppm);

	double __fastcall GetFreqErr(void);
#if 0
    inline double GetMFSKAFC(void){
		if( m_pMFSK && m_pMFSK->m_AvgAFC.IsFull() ){
			return m_pMFSK->m_AvgAFC.GetAvg();
        }
        else {
			return 0;
        }
    };
    inline void ClearMFSKAFC(void){
		if( m_pMFSK ){
			m_pMFSK->m_AvgAFC.Reset();
        }
    }

#endif
	inline CFIR2* GetMFSKHIL(void){
		if( m_pMFSK ){
			return m_pMFSK->GetFIR();
        }
        else {
			return NULL;
        }
    };
    inline BOOL IsMFSKSQ(void){
		if( m_pMFSK ){
			return m_pMFSK->IsMetSQ();
        }
        else {
			return FALSE;
        }
    }
    inline int GetMFSKMetric(int sw){
		if( m_pMFSK ){
			return m_pMFSK->GetMetric(sw);
        }
        else {
			return 0;
        }
    }

	void __fastcall SetMFSKType(int type);
};

#define	MODTABLEMAX	(MAX_SAMP/MIN_SPEED)
/*
enum {
    MOD_CZ2P,
    MOD_VZ2P,
    MOD_RZ2P,
    MOD_LZ2P,
};
*/
enum {
    MOD_P2M,
    MOD_M2P,
    MOD_Z2P,
    MOD_Z2M,
};
//---------------------------------------------------------------------------
// CDEMFSKクラス
class CMODFSK
{
#if DEBUG
public:
	int		m_DEBUG;
#endif
private:
	int		m_s;
    int		m_Cnt;
    double	m_d;
    double	*m_pTbl;
    double	m_Tbl[4][MODTABLEMAX];
    CFIR2	m_BPF;
	CAMPCONT	m_AMPCW;
    CAMPCONT	m_AMPSIG;
public:
	int		m_OutVol;
	int		m_Type;
	CENCODE	m_Encode;
    CVCO	m_Carrier;
	int		m_CWSpeed;
    BOOL	m_fRTTYLPF;
	double	m_Speed;
    double	m_RTTYShift;
	double	m_CarrierFreq;
    double	m_SampleFreq;

	BOOL		m_fMFSK;
    int			m_MFSK_TONES;
	double		m_MFSK_SPEED;
	double		m_MFSK_BW;

	double		m_MFSK_K;
	CMMVARICtrl	*m_pMainCtrl;
private:
	void __fastcall CreateGMSK(void);
	void __fastcall CreatePSK(void);
	void __fastcall CreateRTTY(void);

public:
	CMODFSK();
	~CMODFSK();
	inline void SetSampleFreq(double f){
		m_SampleFreq = f; Create();
    };
	void __fastcall SetCarrierFreq(double f);
	void __fastcall SetCWSpeed(int d);
    void __fastcall Reset(void);
	inline int GetCWSpeed(void){return m_CWSpeed;};
	inline void SetSpeed(double b){ m_Speed = b; Create();};
	inline void SetType(int type){m_Type = type; Create();};
    inline void SetRTTYShift(double s){
    	m_RTTYShift = s;
		if( Is170(m_Type) ) Create();
    };
	void __fastcall Create(void);
//    inline void Reset(void){m_Encode.Reset(FALSE);};

    double __fastcall Do(void);
    double __fastcall DoCarrier(void);

    inline CFIR2* GetFIR(void){return &m_BPF;};
	void __fastcall SetMFSKType(int type);
};

//---------------------------------------------------------------------------
class CClock
{
public:
	int			m_Width;
    int			*m_pData;
    double		m_dNow;
    double		m_dAdd;

    double		m_ToneFreq;
	double		m_SampleFreq;
    CIIRTANK2	m_BPF;
	CIIR		m_LPF;
public:
	CClock();
	~CClock();
	void __fastcall Delete(void);
    void __fastcall Create(int max);
	void __fastcall SetSampleFreq(double f);
	void __fastcall SetToneFreq(double f);
    BOOL __fastcall Do(short ds);
    inline int GetData(int n){return m_pData[n];};
	inline int *GetData(void){return m_pData;};
	inline BOOL IsActive(void){return m_pData!=NULL;};
};
/*=============================================================================
  CPlayBackクラス
=============================================================================*/
class CPlayBack
{
private:
	int		m_StgMax;
    int		m_StgWidth;

    int		m_StgCnt;
    int		m_StgW;

	int		m_WTimer;
	int		m_StgRCnt;
    int		m_StgR;
	short	*m_pStg;
public:
	CPlayBack();
	~CPlayBack();
	void __fastcall Delete(void);
	void __fastcall Init(int wSize, int SampBase);
    void __fastcall Clear(void);
	void __fastcall Write(const short *p);
	BOOL __fastcall Read(short *p);
    void __fastcall StopPlaying(void);
	BOOL __fastcall StartPlaying(int s);

    BOOL __fastcall IsActive(void){return m_StgMax;};
    BOOL __fastcall IsPlaying(void){return m_StgRCnt;};
};
//---------------------------------------------------------------------------
double __fastcall DoFIR(double *hp, double *zp, double d, int tap);
void __fastcall MakeHilbert(double *H, int N, double fs, double fc1, double fc2);
void __fastcall MakeGaussian(double *H, int N, double fc, double fs, double B);
void __fastcall AddGaussian(short *pData, int n, double gain);
void __fastcall DoAvg(double &av, double in, double factor);
int __fastcall GetRTTYBW(int taps);
int __fastcall GetMFSKBW(int taps);
LPCSTR GetVariType(BYTE charset, int mode);
extern CVARICODE	g_VariCode;

//---------------------------------------------------------------------------
class CRxSet {
public:
	int				m_nIndex;
	int				m_Mode;
	BOOL			m_fJA;
	BOOL			m_fTWO;
    int				m_fMBCS;
	int				m_CarrierFreq;
	CMBCS			m_MBCS;
	short			m_Charset;

	BOOL			m_fAFC;
    BOOL			m_SQ;			// スケルチの状態
    int				m_SQLevel;
    int				m_SQTimer;
	double			m_Speed;

    int				m_AFCTimerW;	// 広帯域AFCのガード時間
    int				m_AFCTimerN;	// 高精度AFCのガード時間
	int				m_AFCTimerW2;
    UINT			m_AFCTimerPSK;
	int				m_AFCFQ;
    BOOL			m_AFCSQ;

	CFAVG			m_AvgAFC;

    BOOL			m_fATC;
	BOOL			m_fRTTYFFT;

    int				m_PeakSig;
    CFAVG			m_AvgSig;
	FFTSTG			m_StgFFT;
    CDEMFSK			*m_pDem;
    CFFT			*m_pFFT;
	CMMVARICtrl		*m_pMain;

    int				m_X;
    int				m_Y;
    int				m_WaterW;		// for the sub channel

    int				m_cAutoTS1;		// 自動タイムスタンプカウンタ
    int				m_cAutoTS2;		// 自動タイムスタンプカウンタ

    int				m_MFSK_TYPE;
    UINT			m_AFCTimerMFSK;

	long			m_Timing;

	int				m_PFTimer;
	double			m_RxShift;
	BOOL			m_fCollectScope;

	BOOL			m_fOver;
public:
	CRxSet();
    ~CRxSet();
	void __fastcall SetMFSKType(int type);
	void __fastcall SetSpeed(double b);
	void __fastcall SetMode(int mode);
	void __fastcall SetCarrierFreq(double f);
	void __fastcall Create(BOOL fSub, CMMVARICtrl *pMain);
	void __fastcall Delete(void);
	void __fastcall InitStgFFT(void);
	void __fastcall SetSampleFreq(double f);
	void __fastcall ClearWindow(void);
	double __fastcall GetBandWidth(void);
    double __fastcall GetSpeed(void);
	void __fastcall SetRxShift(double shift);
	void __fastcall SetCollect(BOOL f);
	BOOL __fastcall IsFreqErr(double d);
	void __fastcall SetRTTYFFT(BOOL f);
    inline BOOL IsActive(void){return m_pDem != NULL;};
    inline BOOL IsRTTY(void){return ::IsRTTY(m_Mode);};
    inline BOOL Is170(void){return ::Is170(m_Mode);};
    inline BOOL IsBPSK(void){return ::IsBPSK(m_Mode);};
    inline BOOL IsFSK(void){return ::IsFSK(m_Mode);};
    inline BOOL IsMFSK(void){return ::IsMFSK(m_Mode);};
};
/*=============================================================================
  CNotchesクラス
=============================================================================*/
typedef struct {
	int		Freq;
}NOTCHCTR;

class CNotches
{
public:
	int		m_Count;
    int		m_Max;
    NOTCHCTR	*m_pBase;

    CFIR2	m_FIR;

    int		m_NotchWidth;
	int		m_nBaseTaps;
	int		m_nTaps;
	double	m_SampleFreq;
private:
	void __fastcall Alloc(int nIndex);

public:
	CNotches(void);
    ~CNotches();
	void __fastcall Delete(void);
	void __fastcall Create(void);
	int __fastcall Find(int Freq);
	void __fastcall Add(int Freq, BOOL bFouce);
	void __fastcall SetFreq(int nIndex, int Freq);
	void __fastcall Delete(int nIndex);
};

#endif
