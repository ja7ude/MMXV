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

/*=============================================================================
  MFSK16の実装	2004/JUL/28に作成開始  by Mako JE3HHT
=============================================================================*/
#ifndef _MFSK_H
#define _MFSK_H

#include "CLX.h"
#include "ComLib.h"

#define	ENABLE_MFSK8	TRUE && DEBUG

#if ENABLE_MFSK8
#define	MFSK_MAXBITS	5			// MFSK8
#define	MFSK_MAXTONES	32			// MFSK8
#define	MFSK_MINSPEED	7.8125		// MFSK8
#else
#define	MFSK_MAXBITS	4			// MFSK16
#define	MFSK_MAXTONES	16			// MFSK16
#define	MFSK_MINSPEED	15.625		// MFSK16
#endif

#define	MFSK_VITERBI_K		7		// Length = 7
#define	MFSK_VITERBI_POLY1	0x006D	// 01101101 (O1 + O3 + O4 + O6 + O7)
#define	MFSK_VITERBI_POLY2	0x004F	// 01001111 (O1 + O2 + O3 + O4 + O7)

typedef enum {
	typMFSK16,
    typMFSK8,
    typMFSK31,
}MFSKTYPE;

int __fastcall MFSK_Tones2Type(int tones);
int __fastcall MFSK_Speed2Type(double speed);
void __fastcall MFSK_SetPara(int type, int *pTones, double *pSpeed, int *pBits);

/*=============================================================================
  CViterbiEncodeクラス
=============================================================================*/
class CViterbiEncode
{
private:
	UINT	m_EncData;

	int		m_Poly1;
    int		m_Poly2;
public:
	CViterbiEncode(void);
	void __fastcall Init(int k, int poly1, int poly2);
    inline void __fastcall Reset(void){m_EncData = 0;};

	BYTE __fastcall Do(BOOL bit);
};

/*=============================================================================
  CViterbiクラス
=============================================================================*/
#define	VITERBI_PATHMAX	64
class CViterbi
{
private:
	int 	m_TraceBack;
	int 	m_BlockSize;
	int 	m_States;
	UINT 	m_CurPtr;

	int 	*m_pPoly;
	int 	*m_pMetrics[VITERBI_PATHMAX];
	int 	*m_pHistory[VITERBI_PATHMAX];
	int 	m_tSeq[VITERBI_PATHMAX];
	int 	m_tMetrics[2][256];
private:
	int __fastcall TraceBack(int *pMetric);

public:
	CViterbi(void);
	inline ~CViterbi(){Free();};
	void __fastcall Free(void);
	void __fastcall Init(int k, int poly1, int poly2);

	BOOL __fastcall SetTraceBack(int traceback);
	BOOL __fastcall SetBlockSize(int blocksize);
	void __fastcall Reset(void);
	int __fastcall Decode(int *pMetric, BYTE s[2]);
};

/*=============================================================================
  CInterLeaverクラス
=============================================================================*/
#define	INTERLEAVER_STAGES	10
class CInterLeaver{
private:
	int		m_BitSize;
    int		m_BitSizeM;
	BYTE	m_tTable[INTERLEAVER_STAGES][MFSK_MAXBITS][MFSK_MAXBITS];
private:
	void __fastcall Bits2Syms(BYTE *pSyms, int b);
	int __fastcall Syms2Bits(BYTE *pSyms);

public:
	CInterLeaver(void);

	void __fastcall Reset(void);
    void __fastcall Init(int bitsize);

	void __fastcall DecodeSyms(BYTE *pSyms);
	void __fastcall EncodeSyms(BYTE *pSyms);
	int __fastcall DecodeBits(int b);
	int __fastcall EncodeBits(int b);
};
#endif
