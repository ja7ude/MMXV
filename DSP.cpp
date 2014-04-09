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
#include "MMVARICtl.h"
///----------------------------------------------------------------
#include <stdlib.h>

#include "DSP.h"
#include "ComLib.h"
double	SAMPFREQ=11025;		// default

CVARICODE	g_VariCode;
CSinTable	g_SinTable;
int 	g_tBpfTaps[]={64, 80, 128, 256};
//---------------------------------------------------------------------------

const DWORD _tBitData[]={
   	0x00000001, 0x00000002, 0x00000004, 0x00000008,
    0x00000010, 0x00000020, 0x00000040, 0x00000080,
    0x00000100, 0x00000200, 0x00000400, 0x00000800,
    0x00001000, 0x00002000, 0x00004000, 0x00008000,
    0x00010000, 0x00020000, 0x00040000, 0x00080000,
    0x00100000, 0x00200000, 0x00400000, 0x00800000,
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
};

//---------------------------------------------------------------------------
BOOL __fastcall IsRTTY(int m)
{
	return (m==MODE_RTTY)||(m==MODE_U_RTTY);
}
//---------------------------------------------------------------------------
BOOL __fastcall Is170(int m)
{
	return (m==MODE_FSKW)||IsRTTY(m);
}
//---------------------------------------------------------------------------
BOOL __fastcall IsBPSK(int m)
{
	return (m==MODE_BPSK)||(m==MODE_N_BPSK);
}
//---------------------------------------------------------------------------
BOOL __fastcall IsFSK(int m)
{
	return (m==MODE_GMSK)||(m==MODE_FSK);
}
//---------------------------------------------------------------------------
BOOL __fastcall IsMFSK(int m)
{
	return (m==MODE_mfsk_L)||(m==MODE_mfsk_U);
}
//---------------------------------------------------------------------------
// �e�h�q�t�B���^�̂��������݉��Z
double __fastcall DoFIR(double *hp, double *zp, double d, int tap)
{
	memcpy(zp, &zp[1], sizeof(double)*tap);
	zp[tap] = d;
	d = 0.0;
	for( int i = 0; i <= tap; i++, hp++, zp++ ){
		d += (*zp) * (*hp);
	}
	return d;
}
//---------------------------------------------------------------------------
//�x�b�Z���֐�
static double __fastcall I0(double x)
{
	double	sum, xj;
	int		j;

	sum = 1.0;
	xj = 1.0;
	j = 1;
	while(1){
		xj *= ((0.5 * x) / (double)j);
		sum += (xj*xj);
		j++;
		if( ((0.00000001 * sum) - (xj*xj)) > 0 ) break;
	}
	return(sum);
}
//---------------------------------------------------------------------------
//�e�h�q�t�B���^�̐݌v
void __fastcall MakeFilter(double *HP, FIR *fp)
{
	if( fp->n <= 1 ){
		HP[0] = 1.0;
        return;
    }
    if( fp->typ == ffGAUSSIAN ){
		MakeGaussian(HP, fp->n, fp->fcl, fp->fs, fp->gain);
		return;
    }
	int		j, m;
	double	alpha, win, fm, w0, sum;
	double	*hp;

	if( fp->typ == ffHPF ){
		fp->fc = 0.5*fp->fs - fp->fcl;
	}
	else if( fp->typ != ffLPF ){
		fp->fc = (fp->fch - fp->fcl)/2.0;
	}
	else {
		fp->fc = fp->fcl;
	}
	if( fp->att >= 50.0 ){
		alpha = 0.1102 * (fp->att - 8.7);
	}
	else if( fp->att >= 21.0 ){
		alpha = (0.5842 * pow(fp->att - 21.0, 0.4)) + (0.07886 * (fp->att - 21.0));
	}
	else {
		alpha = 0.0;
	}

	hp = fp->hp;
	sum = PI*2.0*fp->fc/fp->fs;
	if( fp->att >= 21 ){		// �C���p���X�����Ƒ��֐����v�Z
		for( j = 0; j <= (fp->n/2); j++, hp++ ){
			fm = (double)(2 * j)/(double)fp->n;
			win = I0(alpha * sqrt(1.0-(fm*fm)))/I0(alpha);
			if( !j ){
				*hp = fp->fc * 2.0/fp->fs;
			}
			else {
				*hp = (1.0/(PI*(double)j))*sin((double)j*sum)*win;
			}
		}
	}
	else {						// �C���p���X�����̂݌v�Z
		for( j = 0; j <= (fp->n/2); j++, hp++ ){
			if( !j ){
				*hp = fp->fc * 2.0/fp->fs;
			}
			else {
				*hp = (1.0/(PI*(double)j))*sin((double)j*sum);
			}
		}
	}
	hp = fp->hp;
	sum = *hp++;
	for( j = 1; j <= (fp->n/2); j++, hp++ ) sum += 2.0 * (*hp);
	hp = fp->hp;
	if( sum > 0.0 ){
		for( j = 0; j <= (fp->n/2); j++, hp++ ) (*hp) /= sum;
	}

	// ���g���ϊ�

	if( fp->typ == ffHPF ){
		hp = fp->hp;
		for( j = 0; j <= (fp->n/2); j++, hp++ ) (*hp) *= cos((double)j*PI);
	}
	else if( fp->typ != ffLPF ){
		w0 = PI * (fp->fcl + fp->fch) / fp->fs;
		if( fp->typ == ffBPF ){
			hp = fp->hp;
			for( j = 0; j <= (fp->n/2); j++, hp++ ) (*hp) *= 2.0*cos((double)j*w0);
		}
		else {
			hp = fp->hp;
			*hp = 1.0 - (2.0 * (*hp));
			for( hp++, j = 1; j <= (fp->n/2); j++, hp++ ) (*hp) *= -2.0*cos((double)j*w0);
		}
	}
	for( m = fp->n/2, hp = &fp->hp[m], j = m; j >= 0; j--, hp-- ){
		*HP++ = (*hp) * fp->gain;
	}
	for( hp = &fp->hp[1], j = 1; j <= (fp->n/2); j++, hp++ ){
		*HP++ = (*hp) * fp->gain;
	}
}
//---------------------------------------------------------------------------
//�e�h�q�t�B���^�̐݌v
void __fastcall MakeFilter(double *HP, int tap, int type, double fs, double fcl, double fch, double att, double gain)
{
	FIR	fir;

	fir.typ = type;
	fir.n = tap;
	fir.fs = fs;
	fir.fcl = fcl;
	fir.fch = fch;
	fir.att = att;
	fir.gain = gain;
	fir.hp = new double[tap];
	MakeFilter(HP, &fir);
	delete fir.hp;
}
//---------------------------------------------------------------------------
//�e�h�q�t�B���^�i�q���x���g�ϊ��t�B���^�j�̐݌v
//
void __fastcall MakeHilbert(double *H, int N, double fs, double fc1, double fc2)
{
	int L = N / 2;
    double T = 1.0 / fs;

	double W1 = 2 * PI * fc1;
	double W2 = 2 * PI * fc2;

	// 2*fc2*T*SA((n-L)*W2*T) - 2*fc1*T*SA((n-L)*W1*T)

	double w;
	int n;
	double x1, x2;
	for( n = 0; n <= N; n++ ){
		if( n == L ){
			x1 = x2 = 0.0;
		}
		else if( (n - L) ){
			x1 = ((n - L) * W1 * T);
			x1 = cos(x1) / x1;
			x2 = ((n - L) * W2 * T);
			x2 = cos(x2) / x2;
		}
		else {
			x1 = x2 = 1.0;
		}
		w = 0.54 - 0.46 * cos(2*PI*n/(N));
		H[n] = -(2 * fc2 * T * x2 - 2 * fc1 * T * x1) * w;
	}

	if( N < 8 ){
		w = 0;
    	for( n = 0; n <= N; n++ ){
			w += fabs(H[n]);
    	}
		if( w ){
			w = 1.0 / w;
			for( n = 0; n <= N; n++ ){
				H[n] *= w;
			}
    	}
	}
}
//---------------------------------------------------------------------
void __fastcall MakeGaussian(double *H, int N, double fc, double fs, double B)
{
	double W = fc;
    double T = 1.0 / fs;
    double k = B * T * W;

	double	LN = 2.0/log(2.0);
	int	L = N / 2;
    int i, n;
    double sum = 0;
	for( i = 0; i <= N; i++ ){
    	n = i - L;
        if( n < 0 ) n = -n;
		double d = -sqrt(LN) * PI * k * n;
		H[i] = sqrt(LN*PI) * k * exp(d);
        sum += H[i];
    }
	if( sum ){
		sum = 1.0 / sum;
	    for( i = 0; i <= N; i++ ){
			H[i] *= sum;
    	}
    }
}
//---------------------------------------------------------------------------
// CSinTable�N���X
CSinTable::CSinTable()
{
	m_Size = 48000;
    m_tSin = new double[m_Size];
	double pi2t = 2 * PI / double(m_Size);
	for( int i = 0; i < m_Size; i++ ){
		m_tSin[i] = sin(double(i) * pi2t);
	}
}
//---------------------------------------------------------------------------
CSinTable::~CSinTable()
{
	delete m_tSin;
}
//---------------------------------------------------------------------------
// VCO�N���X
CVCO::CVCO()
{
	m_SampleFreq = SAMPFREQ;
	m_FreeFreq = 2000.0;
	m_TableSize = g_SinTable.m_Size;
    m_TableCOS = m_TableSize / 4;
	m_c1 = 0.0;
	m_c2 = double(m_TableSize) * m_FreeFreq / m_SampleFreq;
	m_z = 0.0;
}

CVCO::~CVCO()
{
}

void __fastcall CVCO::SetGain(double gain)
{
	m_c1 = double(m_TableSize) * gain / m_SampleFreq;
}

void __fastcall CVCO::SetSampleFreq(double f)
{
	m_SampleFreq = f;
	SetFreeFreq(m_FreeFreq);
}

void __fastcall CVCO::SetFreeFreq(double f)
{
	m_FreeFreq = f;
	m_c2 = double(m_TableSize) * m_FreeFreq / m_SampleFreq;
	if( f < 1.0 ) m_z = 0;
}

void __fastcall CVCO::InitPhase(void)
{
	m_z = 0.0;
}
//------------------------------------------------------------------
double __fastcall CVCO::Do(void)
{
	m_z += m_c2;
	if( m_z >= m_TableSize ){
		m_z -= m_TableSize;
	}
	if( m_z < 0 ){
		m_z += m_TableSize;
	}
	return g_SinTable.m_tSin[int(m_z)];
}
//------------------------------------------------------------------
double __fastcall CVCO::Do(double d)
{
#if DEBUG
	if( ABS(d) >= 2.0 ) Application->MainForm->Caption = "VCO over range";
#endif
	// -1 to 1
	m_z += (d * m_c1 + m_c2);
	while( m_z >= m_TableSize ){
		m_z -= m_TableSize;
	}
	while( m_z < 0 ){
		m_z += m_TableSize;
	}
	return g_SinTable.m_tSin[int(m_z)];
}
//------------------------------------------------------------------
double __fastcall CVCO::DoCos(void)
{
	double z = m_z + m_TableCOS;
    if(z >= m_TableSize ) z -= m_TableSize;
	return g_SinTable.m_tSin[int(z)];
}

//------------------------------------------------------------------
static double __fastcall asinh(double x)
{
	return log(x + sqrt(x*x+1.0));
}
//------------------------------------------------------------------
// bc : 0-�o�^�[���[�X, 1-�`�F�r�V�t
// rp : �ʉ߈�̃��b�v��
void __fastcall MakeIIR(int type, double *A, double *B, double fc, double fs, int order, int bc, double rp)
{
	double	w0, wa, u, zt, x;
	int		j, n;

	if( bc ){		// �`�F�r�V�t
		u = 1.0/double(order) * asinh(1.0/sqrt(pow(10.0,0.1*rp)-1.0));
	}
	wa = tan(PI*fc/fs);
	w0 = 1.0;
	n = (order & 1) + 1;
	double *pA = A;
	double *pB = B;
	double s, d, d1, d2;
	for( j = 1; j <= order/2; j++, pA+=2, pB+=3 ){
		if( bc ){	// �`�F�r�V�t
			d1 = sinh(u)*cos(n*PI/(2*order));
			d2 = cosh(u)*sin(n*PI/(2*order));
			w0 = sqrt(d1 * d1 + d2 * d2);
			zt = sinh(u)*cos(n*PI/(2*order))/w0;
		}
		else {		// �o�^�[���[�X
			w0 = 1.0;
			zt = cos(n*PI/(2*order));
		}
    	switch(type){
        	case ffLPF:
				s = wa * w0;
				d = 1 + s*2*zt + s*s;
				pA[0] = -2 * (s*s - 1)/d;
        		pA[1] = -(1.0 - s*2*zt + s*s)/d;
        		pB[0] = s*s / d;
        		pB[1] = 2*pB[0];
	        	pB[2] = pB[0];
                break;
            case ffHPF:
				s = wa / w0;
				d = 1 + s*2*zt + s*s;
				pA[0] = -2 * (s*s - 1)/d;
		        pA[1] = -(1.0 - s*2*zt + s*s)/d;
		        pB[0] = 1.0 / d;
		        pB[1] = -2*pB[0];
		        pB[2] = pB[0];
                break;
        }
		n += 2;
	}
	if( bc && !(order & 1) ){
		x = pow( 1.0/pow(10.0,rp/20.0), 1.0/double(order/2) );
		pB = B;
		for( j = 1; j <= order/2; j++, pB+=3 ){
			pB[0] *= x;
			pB[1] *= x;
            pB[2] *= x;
		}
	}
	if( order & 1 ){
		if( bc ) w0 = sinh(u);
		j = (order / 2);
		pA = A + (j*2);
		pB = B + (j*3);
        switch(type){
        	case ffLPF:
				s = wa * w0;
				d = 1 + s;
				pA[0] = -(s - 1)/d;
				pB[0] = wa*w0/d;
				pB[1] = pB[0];
                break;
            case ffHPF:
				s = wa / w0;
				d = 1 + s;
				pA[0] = -(s - 1)/d;
				pB[0] = 1.0/d;
				pB[1] = -pB[0];
            	break;
        }
	}
}

//---------------------------------------------------------------------------
CIIR::CIIR()
{
	m_order = 0;
	memset(m_A, 0, sizeof(m_A));
	memset(m_B, 0, sizeof(m_B));
	memset(m_Z, 0, sizeof(m_Z));
}

CIIR::~CIIR()
{
}

void __fastcall CIIR::Clear(void)
{
	memset(m_Z, 0, sizeof(m_Z));
}

void __fastcall CIIR::Create(int type, double fc, double fs, int order, int bc, double rp)
{
#if DEBUG
	if( order > IIRMAX ){
		Application->MainForm->Caption = "Over tap in CIIR";
        order = IIRMAX;
    }
#endif
	m_bc = bc;
	m_rp = rp;
	m_order = order;
    m_orderH = order/2;
	::MakeIIR(type, m_A, m_B, fc, fs, order, bc, rp);
}

double __fastcall CIIR::Do(double d)
{
	double *pA = m_A;
	double *pB = m_B;
	double *pZ = m_Z;
	double o;
	for( int i = 0; i < m_orderH; i++, pA+=2, pB+=3, pZ+=2 ){
		d += pZ[0] * pA[0] + pZ[1] * pA[1];
		o = d * pB[0] + pZ[0] * pB[1] + pZ[1] * pB[2];
		pZ[1] = pZ[0];
		if( d < 0 ){
			if( d > -IIRVLIMIT ) d = 0.0;
        }
        else if( d < IIRVLIMIT ){
        	d = 0.0;
        }
		pZ[0] = d;
		d = o;
	}
	if( m_order & 1 ){
		d += pZ[0] * pA[0];
		o = d * pB[0] + pZ[0] * pB[1];
		if( d < 0 ){
			if( d > -IIRVLIMIT ) d = 0.0;
        }
        else if( d < IIRVLIMIT ){
        	d = 0.0;
        }
		pZ[0] = d;
		d = o;
	}
	return d;
}


//---------------------------------------------------------------------------
CIIRTANK::CIIRTANK()
{
	b1 = b2 = a0 = z1 = z2 = 0;
	m_SampleFreq = SAMPFREQ;
    m_Freq = CARRIERFREQ;
    m_BW = 100;
    Create();
}
//---------------------------------------------------------------------------
void __fastcall CIIRTANK::Create(void)
{
	SetFreq(m_Freq, m_SampleFreq, m_BW);
}
//---------------------------------------------------------------------------
void __fastcall CIIRTANK::SetFreq(double f, double smp, double bw)
{
	double lb1, lb2, la0;
	lb1 = 2 * exp(-PI * bw/smp) * cos(2 * PI * f / smp);
	lb2 = -exp(-2*PI*bw/smp);
	if( bw ){
		la0 = sin(2 * PI * f/smp) / ((smp/(PI*2.0)) / bw);
	}
	else {
		la0 = sin(2 * PI * f/smp);
	}
	b1 = lb1; b2 = lb2; a0 = la0;
}
//---------------------------------------------------------------------------
double __fastcall CIIRTANK::Do(double d)
{
	d *= a0;
	d += (z1 * b1);
	d += (z2 * b2);
	z2 = z1;
	if( d < 0 ){
		if( d > -IIRVLIMIT ) d = 0.0;
    }
    else if( d < IIRVLIMIT ){
       	d = 0.0;
    }
	z1 = d;
	return d;
}

//---------------------------------------------------------------------------
CIIRTANK2::CIIRTANK2()
{
	m_SampleFreq = SAMPFREQ;
    m_Freq = CARRIERFREQ;
    m_BW = 100;
    Create();
}
//---------------------------------------------------------------------------
void __fastcall CIIRTANK2::SetSampleFreq(double f)
{
	m_SampleFreq = f;
    Create();
}
//---------------------------------------------------------------------------
void __fastcall CIIRTANK2::Create(void)
{
	m_Tank1.m_SampleFreq = m_SampleFreq;
    m_Tank1.m_Freq = m_Freq;
    m_Tank1.m_BW = m_BW;
	m_Tank2.m_SampleFreq = m_SampleFreq;
    m_Tank2.m_Freq = m_Freq;
    m_Tank2.m_BW = m_BW;
    m_Tank1.Create();
    m_Tank2.Create();
}
//---------------------------------------------------------------------------
double __fastcall CIIRTANK2::Do(double d)
{
	return m_Tank1.Do(m_Tank2.Do(d));
}

//---------------------------------------------------------------------------
// CFIR�N���X
CFIR::CFIR()
{
	m_pZ = NULL;
    m_pH = NULL;
    m_Tap = 0;
}

//---------------------------------------------------------------------------
CFIR::~CFIR()
{
	if( m_pZ ) delete m_pZ;
    if( m_pH ) delete m_pH;
}
//---------------------------------------------------------------------------
void __fastcall CFIR::Create(int tap, int type, double fs, double fcl, double fch, double att, double gain)
{
	m_Tap = tap;
	if( m_pZ ) delete m_pZ;
    if( m_pH ) delete m_pH;
	m_pZ = new double[tap+1];
	m_pH = new double[tap+1];
    memset(m_pZ, 0, sizeof(double)*(tap+1));
	::MakeFilter(m_pH, tap, type, fs, fcl, fch, att, gain);
}
//---------------------------------------------------------------------------
double __fastcall CFIR::Do(double d)
{
	return DoFIR(m_pH, m_pZ, d, m_Tap);
}
//---------------------------------------------------------------------------
void __fastcall CFIR::SaveCoef(LPCSTR pName)
{
	FILE *fp;
    if( (fp = fopen(pName, "wt")) != NULL ){
		int i;
		for( i = 0; i <= m_Tap; i++ ){
			fprintf(fp, "H[%u]=%lf\n", i, m_pH[i]);
        }
		fclose(fp);
    }
}

//---------------------------------------------------------------------------
// CFIR2�N���X
CFIR2::CFIR2()
{
	m_pZ = NULL;
    m_pH = NULL;
    m_pZP = NULL;
    m_W = 0;
    m_Tap = 0;
    m_fs = 0;
}

//---------------------------------------------------------------------------
CFIR2::~CFIR2()
{
	if( m_pZ ) delete m_pZ;
    if( m_pH ) delete m_pH;
}
//---------------------------------------------------------------------------
void __fastcall CFIR2::Delete(void)
{
	if( m_pZ ) delete m_pZ;
    if( m_pH ) delete m_pH;
	m_pZ = NULL;
    m_pH = NULL;
    m_pZP = NULL;
    m_W = 0;
    m_Tap = 0;
    m_fs = 0;
}
//---------------------------------------------------------------------------
void __fastcall CFIR2::Create(int tap, double fs)
{
	if( (m_Tap != tap) || !m_pZ || !m_pH ){
		if( m_pZ ) delete m_pZ;
		m_pZ = new double[(tap+1)*2];
    	memset(m_pZ, 0, sizeof(double)*(tap+1)*2);
	    if( m_pH ) delete m_pH;
		m_pH = new double[tap+1];
		m_W = 0;
    }
	m_Tap = tap;
    m_TapHalf = tap/2;
	m_fs = fs;
}
//---------------------------------------------------------------------------
void __fastcall CFIR2::Create(int tap, int type, double fs, double fcl, double fch, double att, double gain)
{
	if( (m_Tap != tap) || !m_pZ || !m_pH ){
		if( m_pZ ) delete m_pZ;
		m_pZ = new double[(tap+1)*2];
    	memset(m_pZ, 0, sizeof(double)*(tap+1)*2);
	    if( m_pH ) delete m_pH;
		m_pH = new double[tap+1];
		m_W = 0;
    }
	m_Tap = tap;
    m_TapHalf = tap/2;
    m_fs = fs;
	::MakeFilter(m_pH, tap, type, fs, fcl, fch, att, gain);
}
//---------------------------------------------------------------------------
void __fastcall CFIR2::Create(int tap, double fs, double fcl, double fch)
{
	if( (m_Tap != tap) || !m_pZ || !m_pH ){
		if( m_pZ ) delete m_pZ;
		m_pZ = new double[(tap+1)*2];
    	memset(m_pZ, 0, sizeof(double)*(tap+1)*2);
	    if( m_pH ) delete m_pH;
		m_pH = new double[tap+1];
		m_W = 0;
    }
	m_Tap = tap;
    m_TapHalf = tap/2;
    m_fs = fs;
	::MakeHilbert(m_pH, tap, fs, fcl, fch);
}
//---------------------------------------------------------------------------
void __fastcall CFIR2::CreateSamp(int tap, double fs, const double *pSmpFQ, int wDB)
{
	if( (m_Tap != tap) || !m_pZ || !m_pH ){
		if( m_pZ ) delete m_pZ;
		m_pZ = new double[(tap+1)*2];
    	memset(m_pZ, 0, sizeof(double)*(tap+1)*2);
	    if( m_pH ) delete m_pH;
		m_pH = new double[tap+1];
        m_W = 0;
    }

    int htap = tap/2;
    int i, j;
    double *pSamp = new double[tap+1];
    memcpy(pSamp, pSmpFQ, sizeof(double)*(tap/2));
	double maxAmp = 0;
	double *dp = pSamp;
    for( i = 0; i < tap/2; i++, dp++ ){
		pSamp[tap-i] = *dp;
		if( maxAmp < *dp ) maxAmp = *dp;
    }
    pSamp[tap/2] = pSamp[tap/2 - 1];
    double *pH = new double[tap+1];
    double re, fm;

	for( i = 0; i <= htap; i++ ){
		re = 0.0;
		dp = pSamp;
		for( j = 0; j < tap; j++ ){
			fm = 2.0 * PI * double((i*j)%tap)/double(tap);
			re += (*dp++) * cos(fm);
		}
		pH[i] = re / tap;
    }
	if( wDB >= 21 ){
		double alpha, att;
		att = double(wDB);
		if( att >= 50.0 ){
			alpha = 0.1102 * (att - 8.7); 
		}
		else {
			alpha = (0.5842*pow(att - 21.0, 0.4)) + (0.07886 * (att - 21.0));
		}
		dp = pH;
		for( i = 0; i <= htap; i++, dp++ ){
			double fm = double(2 * i)/double(tap);
			double win = I0(alpha * sqrt(1.0-(fm*fm)))/I0(alpha);
			*dp *= win;
		}
		fm = 0.000001;
		double f = 0.0;
		double g;
		double pi2t = 2.0 * PI / fs;
		int i, k;
		double fk = fs * 0.5 / tap;
		for( i = 0; i < tap; i++, f += fk ){
			g = pH[0];
			for( k = 1; k <= htap; k++ ){
				g += 2.0 * pH[k] * cos(pi2t*f*double(k));
			}
			if( fm < g ) fm = g;
		}
		fm = maxAmp / fm;
		dp = pH;
		for( i = 0; i <= htap; i++, dp++ ){
			*dp *= fm;
		}
	}
	for( i = 0; i <= htap; i++ ) m_pH[htap-i] = pH[i];
	for( i = 0; i < htap; i++ ) m_pH[tap-i] = m_pH[i];
    delete pH;
    delete pSamp;

	m_Tap = tap;
    m_TapHalf = htap;
    m_fs = fs;
}
//---------------------------------------------------------------------------
void __fastcall CFIR2::Clear(void)
{
	if( m_pZ ) memset(m_pZ, 0, sizeof(double)*(m_Tap+1)*2);
    m_W = 0;
}
//---------------------------------------------------------------------------
double __fastcall CFIR2::Do(double d)
{
	double *dp1 = &m_pZ[m_W+m_Tap+1];
	m_pZP = dp1;
	*dp1 = d;
    m_pZ[m_W] = d;
    d = 0;
    double *hp = m_pH;
    for( int i = 0; i <= m_Tap; i++ ){
		d += (*dp1--) * (*hp++);
    }
    m_W++;
	if( m_W > m_Tap ) m_W = 0;
    return d;
}
//---------------------------------------------------------------------------
double __fastcall CFIR2::Do(double *hp)
{
    double d = 0;
    double *dp = m_pZP;
    for( int i = 0; i <= m_Tap; i++ ){
		d += (*dp--) * (*hp++);
    }
    return d;
}
//---------------------------------------------------------------------------
void __fastcall CFIR2::Do(CLX &z, double d)
{
	double *dp1 = &m_pZ[m_W+m_Tap+1];
	m_pZP = dp1;
	*dp1 = d;
    m_pZ[m_W] = d;
    d = 0;
    double *hp = m_pH;
    for( int i = 0; i <= m_Tap; i++ ){
		d += (*dp1--) * (*hp++);
    }
    z.j = d;
    z.r = m_pZ[m_W+m_TapHalf+1];
    m_W++;
	if( m_W > m_Tap ) m_W = 0;
}


//---------------------------------------------------------------------------
// CFIRX�N���X
CFIRX::CFIRX()
{
	m_pZ = NULL;
    m_pH = NULL;
    m_pZP = NULL;
    m_W = 0;
    m_Tap = 0;
    m_fs = 0;
}

//---------------------------------------------------------------------------
CFIRX::~CFIRX()
{
	if( m_pZ ) delete m_pZ;
    if( m_pH ) delete m_pH;
}
//---------------------------------------------------------------------------
void __fastcall CFIRX::Create(int tap, int type, double fs, double fcl, double fch, double att, double gain)
{
	if( (m_Tap != tap) || !m_pZ || !m_pH ){
		if( m_pZ ) delete m_pZ;
		m_pZ = new CLX[(tap+1)*2];
    	memset(m_pZ, 0, sizeof(CLX)*(tap+1)*2);
	    if( m_pH ) delete m_pH;
		m_pH = new double[tap+1];
		m_W = 0;
    }
	m_Tap = tap;
    m_TapHalf = tap/2;
    m_fs = fs;
	::MakeFilter(m_pH, tap, type, fs, fcl, fch, att, gain);
}
//---------------------------------------------------------------------------
void __fastcall CFIRX::Clear(void)
{
	if( m_pZ ) memset(m_pZ, 0, sizeof(CLX)*(m_Tap+1)*2);
    m_W = 0;
}
//---------------------------------------------------------------------------
void __fastcall CFIRX::Do(CLX &d)
{
	CLX *dp1 = &m_pZ[m_W+m_Tap+1];
	m_pZP = dp1;
	*dp1 = d;
    m_pZ[m_W] = d;
	CLX z = 0;
    double *hp = m_pH;
    for( int i = 0; i <= m_Tap; i++, dp1-- ){
		z.r += dp1->r * (*hp);
		z.j += dp1->j * (*hp++);
    }
    m_W++;
	if( m_W > m_Tap ) m_W = 0;
    d = z;
}


/*=============================================================================
  CSlideFFT�N���X  �X���C�f�B���O�@FFT
=============================================================================*/
#define	SLIDE_WINDOW_COEFF		0.9999
CSlideFFT::CSlideFFT(void)
{
	m_Length = 0;
	m_Base = 0;
	m_Tones = 0;
	m_kWindow = 0;
	m_pBase = NULL;
    m_pCur = m_pEnd = NULL;
}

//--------------------------------------------------------------------------
CSlideFFT::~CSlideFFT()
{
   	if( m_pBase ) delete m_pBase;
}

//--------------------------------------------------------------------------
void __fastcall CSlideFFT::Create(int len, int base, int tones)
{
	if( !m_pBase || (len != m_Length) ){
	   	if( m_pBase ) delete m_pBase;
		m_pBase = new CLX[len];
    }

#if DEBUG
	if( tones > MFSK_MAXTONES ){
		Application->MainForm->Caption = "Tones overflow in CSlideFFT";
        tones = MFSK_MAXTONES;
    }
#endif

    memset(m_pBase, 0, sizeof(CLX)*len);
    memset(m_tWindow, 0, sizeof(m_tWindow));
    memset(m_tData, 0, sizeof(m_tData));

	m_Length = len;
	m_Base = base;
    m_Tones = tones;

	double k = 2.0 * PI / double(len);
	for(int i = 0; i < tones; i++){
		m_tWindow[i].r = cos((i+base) * k) * SLIDE_WINDOW_COEFF;
		m_tWindow[i].j = sin((i+base) * k) * SLIDE_WINDOW_COEFF;
	}
	m_kWindow = pow(SLIDE_WINDOW_COEFF, len);
    m_pCur = m_pBase;
    m_pEnd = &m_pBase[m_Length];
}

//--------------------------------------------------------------------------
CLX* __fastcall CSlideFFT::Do(const CLX &zIn)
{
	CLX z;

	if( m_pCur >= m_pEnd ) m_pCur = m_pBase;
	z = *m_pCur;
	*m_pCur = zIn;
    m_pCur++;
	z *= m_kWindow;

	CLX *pData = m_tData;
	CLX *pWindow = m_tWindow;
	for( int i = 0; i < m_Tones; i++, pData++ ){
		*pData -= z;
		*pData += zIn;
		*pData *= *pWindow++;
	}
	return m_tData;
}

#if DEBUG
//--------------------------------------------------------
// CQSB�N���X
__fastcall CQSB::CQSB()
{
    m_vMin = 16384/32;
    m_vMax = 16384/4;
    m_fPhaseError = FALSE;
}

__fastcall CQSB::~CQSB()
{
}

void __fastcall CQSB::Create(int min, int max, int msec, BOOL perr)
{
	m_vMin = min;
    m_vMax = max;

	m_VCO.SetSampleFreq(SAMPFREQ);
    m_VCO.SetFreeFreq(1000.0 / double(msec));
	m_VCOP.SetSampleFreq(SAMPFREQ);
    m_VCOP.SetFreeFreq(1000.0 / 10000.0);
    m_tap = 200;
    memset(m_Z, 0, sizeof(m_Z));

    m_fPhaseError = perr;
}

int __fastcall CQSB::Do(void)
{
    int d = m_VCO.Do() * 16384.0;
	BOOL fm = d < 0;
	d = ABS(d);
	d = m_vMin + ((m_vMax - m_vMin) * d / 16384);
    if( d > 16384 ) d = 16384;
    if( fm && m_fPhaseError ) d *= -1.0;
	return d;
}

double __fastcall CQSB::Do(double d)
{
    double ds = m_VCO.Do();
	if( m_fPhaseError ){
    	double dc = m_VCO.DoCos();
		m_Z[m_tap] = d;
    	memcpy(m_Z, &m_Z[1], m_tap*sizeof(double));
        int n = m_tap * (0.6 + m_VCOP.Do(0.0)*0.4);
        if( n < 0 ) n = 0;
        if( n > m_tap ) n = m_tap;
	    d = m_Z[0] * ds + m_Z[n] * dc;
    }
	if( m_vMin != m_vMax ){
		ds = m_vMin + ((m_vMax - m_vMin) * ABS(ds));
	    if( d > 16384 ) d = 16384;
    }
    else {
		ds = m_vMax;
    }
	return d * ds;
}
//--------------------------------------------------------
// CNoise�N���X
__fastcall CNoise::CNoise()
{
	m_SampleFreq = SAMPFREQ;
//	m_reg = 0x12345;
	m_reg = 0x11111;
    Create(600, 2700);
};

void __fastcall CNoise::SetSampleFreq(double f)
{
	m_SampleFreq = f;
	Create(100, 2700);
}

void __fastcall CNoise::Create(double f1, double f2)
{
	m_HPF.Create(ffHPF, f1, m_SampleFreq, 3, 1, 0.3);
    m_LPF.Create(ffLPF, f2, m_SampleFreq, 7, 1, 0.3);
}

DWORD __fastcall CNoise::Do(void)
{
	DWORD r = m_reg >> 1;
	if( (m_reg ^ r) & 1 ){
		r |= 0xffe00000;
	}
	else {
		r &= 0x001fffff;
	}
	m_reg = r;
    return r >> 19;
}

double __fastcall CNoise::DoLPF(void)
{
	return m_LPF.Do(Do());
}

double __fastcall CNoise::DoHPF(void)
{
    return m_LPF.Do(m_HPF.Do(Do()));
}


void __fastcall AddGaussian(short *pData, int n, double gain)
{
	int i = 0;
	double rad;
	double r;
	double u1;
	double u2;
	while( i < n ){
		do {
			u1 = 1.0 - 2.0 * (double)rand()/(double)RAND_MAX ;
			u2 = 1.0 - 2.0 * (double)rand()/(double)RAND_MAX ;
			r = u1*u1 + u2*u2;
		} while(r >= 1.0 || r == 0.0);
		rad = sqrt(-2.0*log(r)/r);
		pData[i++] += (gain*u1*rad);
		pData[i++] += (gain*u2*rad);
	}
}
#endif

//*********************************************************************
// CDECM2	1/N�f�V���[�^�����N���X
//
/*
	Clock	������	FFT
	6000Hz	6000Hz	6000Hz	1024
	8000Hz	8000Hz	8000Hz	2048
	11025Hz	5513Hz	11025Hz	2048
	12000Hz	6000Hz	6000Hz	1024
	16000Hz	5333Hz	10667Hz	2048
	18000Hz	6000Hz	6000Hz	1024
	22050Hz	5513Hz	11025Hz	2048
	24000Hz	6000Hz	6000Hz	1024
	44100Hz	6300Hz	6300Hz	1024
	48000Hz	6000Hz	6000Hz	1024
	50000Hz	6250Hz	6250Hz	1024
*/
CDECM2::CDECM2()
{
	m_pMain = NULL;
	m_Type = 1;
	m_Count = 0;
    SetSampleFreq(m_Type, SAMPFREQ);
}

void __fastcall CDECM2::SetSampleFreq(int type, double f)
{
	m_Type = type;
	m_SampleFreq = f;
#if DECFIR
	switch(m_Type){
		case 2:			// 16000, 18000
		case 3:         // 22050, 24000
		case 4:
		case 5:
			m_FIR.Create(40, ffLPF, m_SampleFreq, 2900, 2900, 40, 1.0);
        	break;
		case 6:			// 44100,
		case 7:			// 48000, 50000
			m_FIR.Create(44, ffLPF, m_SampleFreq, 3000, 3000, 40, 1.0);
        	break;
		default:		// 11025, 12000
			m_FIR.Create(36, ffLPF, m_SampleFreq, 2700, 2700, 40, 1.0);
        	break;
    }
#else
	m_IIR.Create(ffLPF, m_pMain ? m_pMain->m_DecCutOff : 2700, m_SampleFreq, 10, 1, 0.1);
#endif
}

BOOL __fastcall CDECM2::Do(double d)
{
#if DECFIR
	m_O = m_FIR.Do(d);
#else
	m_O = m_IIR.Do(d);
#endif
	if( !m_Count ){
		m_Count = m_Type;
        return TRUE;
    }
    else {
		m_Count--;
        return FALSE;
    }
}

//--------------------------------------------------------
// CAGC�N���X
CAGC::CAGC()
{
	m_fc = 1000.0;
	m_MonitorFreq = SAMPFREQ/2048;
    m_SampleFreq = SAMPFREQ;
	m_CarrierFreq = CARRIERFREQ;
    m_LimitGain = 0.005;
    m_AvgOver.Create(4);
	Create();
}
//--------------------------------------------------------
void __fastcall CAGC::Create(void)
{
	m_Count = 0;
	m_Max = -1.0;
	m_Min = 1.0;
	m_d = 0;
	m_agc = 1.0;
//    m_Level.Create(ffLPF, 0.3, m_MonitorFreq, 3, 0, 0);
	SetCarrierFreq(m_CarrierFreq);
}
//--------------------------------------------------------
void __fastcall CAGC::SetCarrierFreq(double f)
{
	m_CarrierFreq = f;
	if( m_CarrierFreq >= 1000.0 ){
	    m_Gain = (m_CarrierFreq / 1000.0);

/*
500		0.003		333
1000	0.0035		285		1.0		1.0
1500	0.005		200		1.4		1.5
1800	0.006		166		1.71
2000	0.008		125     2.3		2.0
2100	0.01		100		2.85	2.1
2200	0.012       83      3.4		2.2
2300	0.015		66      4.3     2.3
2400	0.025       40      7.1		2.4
2500	0.028		36      7.9		2.5
2600	0.030       33      8.6		2.6
*/
        if( m_Gain >= 2.35 ){
			m_Gain *= 7.1 / 2.4;
        }
		else if( m_Gain >= 2.25 ){
			m_Gain *= 4.3 / 2.3;
        }
        else if( m_Gain >= 2.15 ){
        	m_Gain *= 3.4 / 2.2;
        }
		else if( m_Gain >= 2.05 ){
			m_Gain *= 2.9 / 2.1;
        }
        else if( m_Gain >= 1.8 ){
			m_Gain *= 2.3 / 2.2;
        }
    }
    else {
		m_Gain = 1.0;
    }
    SetFC(m_fc);
}
//--------------------------------------------------------
void __fastcall CAGC::SetFC(double fc)
{
	m_fc = fc;
    if( fc > m_CarrierFreq*0.45 ) fc = m_CarrierFreq*0.45;
    m_LPF.Create(ffLPF, fc, m_CarrierFreq, 1, 0, 0);
    m_d = 0;
    m_TLimit = int(m_SampleFreq*0.8/m_CarrierFreq);
    m_AvgOver.Reset(1.0);
}
//--------------------------------------------------------
void __fastcall CAGC::Reset(void)
{
	m_Max = -1.0;
	m_Min = 1.0;
	m_agc = 1.0;
    m_d = 0;
    m_Count = 0;
    for( int i = 0; i < 12; i++ ){
		m_LPF.Do(1.0);
//    	m_Level.Do(0);
    }
    m_AvgOver.Reset(1.0);
}
//--------------------------------------------------------
double __fastcall CAGC::Do(double d)
{
	if( m_Max < d ) m_Max = d;
	if( m_Min > d ) m_Min = d;
	if( (d >= 0) && (m_d < 0) && (m_Count >= m_TLimit) ){
		double amp = m_Max - m_Min;
        if( amp > 0.1 ){
		    m_agc = m_LPF.Do(5.0/amp);
    	    if( m_agc >= 1.0 ) m_agc = 1.0;
			m_Max = -32768.0;
			m_Min = 32768.0;
        }
        m_Count = 0;
	}
    m_Count++;
	m_d = d;
	d *= m_agc;
    if( d > 2.5 ){ d = 2.5; } else if( d < -2.5 ){ d = -2.5; }
    return d;
}
//--------------------------------------------------------
BOOL __fastcall CAGC::GetOver(void)
{
// 5.0 / (65536 * 0.666)
	return (m_AvgOver.Do(m_agc) < 0.0001146);
//	return (m_agc < 0.0001146);
}

//--------------------------------------------------------
CAFC::CAFC()
{
	m_Speed = SPEED;
	m_SampleFreq = SAMPFREQ;
    m_A = m_SampleFreq / (2 * PI);
    m_B = 0.0;
    SetTap(32);
	SetTone(CARRIERFREQ);
    m_LPF.Create(ffLPF, SPEED, m_SampleFreq, 1, 0, 0);
    m_d = 0;
    m_min = 65536.0;
    m_max = -65536.0;
	m_Max = m_Min = 0.0;
	m_Count = 0;
	m_LCount1 = int(m_SampleFreq * 1.8 / m_Speed);
    m_LCount2 = int(m_SampleFreq * 31.8 / m_Speed);
}

void __fastcall CAFC::SetPara(double a, double b)
{
	m_A = a;
    m_B = b;
}

void __fastcall CAFC::SetTap(int tap)
{
	m_Avg.Create(tap);
}

void __fastcall CAFC::SetTone(double fq)
{
	m_fq = fq;
	m_Avg.Reset(fq);
    m_fChange = FALSE;
}

BOOL __fastcall CAFC::Do(double d)
{
	BOOL f = FALSE;
	d = m_LPF.Do(d);
    if( m_min > d ) m_min = d;
    if( m_max < d ) m_max = d;
    if( ((m_d < 0.0) && (d >= 0.0) && (m_Count > m_LCount1)) ||
		(m_Count > m_LCount2)
    ){
    	m_Max = m_max; m_Min = m_min;
		double dd = (m_max + m_min) * 0.5;
	    m_min = m_max = d;
        dd = dd * m_A + m_B;

		dd = m_Avg.Do(dd);
		if( (dd >= m_B-100) && (dd <= m_B+100) ){
	        m_fq = dd;
			f = TRUE;
        }
		m_Count = 0;
    }
    m_d = d;
    m_Count++;
	return f;
}


//---------------------------------------------------------------------------
// CLMS�N���X
CLMS::CLMS()
{
	m_Tap = 48;
    m_lmsDelay = 32;

	memset(Z, 0, sizeof(Z));
	memset(H, 0, sizeof(H));
	memset(D, 0, sizeof(D));

	m_lmsADJSC = 1.0 / double(32768 * 32768);	// �X�P�[�������l

	m_lmsMU2 = 0.003;			// LMS 2��
	m_lmsGM = 0.9999;			// LMS ��
}

//-------------------------------------------------
// �K���t�B���^�̉��Z
double __fastcall CLMS::Do(double d)
{
	double a = 0.0;
	int i;
	double *zp = Z;
	double *hp = H;

	// �g�����X�o�[�T���t�B���^
	memcpy(Z, &Z[1], sizeof(double)*m_Tap);
	Z[m_Tap] = D[0];
	for( i = 0; i <= m_Tap; i++, zp++, hp++ ){
		a += (*zp) * (*hp);
	}
	// �덷�v�Z
	double err = d - a;
	double merr = err * m_lmsMU2 * m_lmsADJSC;	// lmsADJSC = 1/(32768 * 32768) �X�P�[�����O�����l

	// �x����̈ړ�
	if( m_lmsDelay ) memcpy(D, &D[1], sizeof(double)*m_lmsDelay);
	D[m_lmsDelay] = d;
	// �W���X�V
	zp = Z;
	hp = H;
	double sum = 0;
	for( i = 0; i <= m_Tap; i++, zp++, hp++ ){
		*hp = (merr * (*zp)) + (*hp * m_lmsGM);
        sum += ABS(*hp);
	}
    if( sum ){
		sum = 1.0 / sum;
        hp = H;
	    for( i = 0; i <= m_Tap; i++, hp++ ){
			*hp *= sum;
    	}
    }
	return a;
}

//---------------------------------------------------------------------------
// CDEMFSK�N���X
CDEMFSK::CDEMFSK(CMMVARICtrl *pMainCtrl)
{
	m_pMainCtrl = pMainCtrl;
    m_MFSK_TONES = 16;
	m_MFSK_SPEED = 15.625;
	m_MFSK_BW = (m_MFSK_SPEED * (m_MFSK_TONES-1));
	m_MFSK_BITS = 4;

	m_fRTTYFFT = FALSE;

	m_Type = MODE_GMSK;
	m_pBPF = NULL;
	m_Speed = SPEED;
	m_SampleFreq = SAMPFREQ/2;
//    m_LoopFc = SPEED*6;
//    m_Gain = 1.0;
    m_Gain = 0.8;
	m_CarrierFreq = CARRIERFREQ;
    m_PreBPFTaps = 64;
    m_fAFC = TRUE;
	m_fEnableAFC = FALSE;
    m_out = m_err = 0.0;
	m_RTTYShift = 170.0;
    m_fRTTYTANK = TRUE;
    m_DemLevel = 1.0;
    m_SW = m_Speed * 0.5;
    m_pMFSK = NULL;
    m_fMFSK = FALSE;
    m_BPFLimit = 4.0;
	m_bCollect = FALSE;
    Create();
}
//---------------------------------------------------------------------------
CDEMFSK::~CDEMFSK()
{
	if( m_pMFSK ) delete m_pMFSK;
}
//---------------------------------------------------------------------------
void __fastcall CDEMFSK::SetMFSKType(int type)
{
	int tones;
    MFSK_SetPara(type, &tones, &m_MFSK_SPEED, &m_MFSK_BITS);
    if( tones == m_MFSK_TONES ) return;
    m_MFSK_TONES = tones;
	m_MFSK_BW = (m_MFSK_SPEED * (m_MFSK_TONES-1));
	if( m_pMFSK ) Create();
}

//---------------------------------------------------------------------------
#define	AFC_LPFORDER	3
#define	OUT_LPFORDER	3
void __fastcall CDEMFSK::Create()
{
	m_fMFSK = IsMFSK(m_Type);
	m_fRTTYTANK = (m_Speed < m_RTTYShift);
	m_LPF1.Create(ffLPF, m_Speed, m_SampleFreq, AFC_LPFORDER, 0, 0);
//	m_LPF2.Create(ffLPF, m_Speed, m_SampleFreq, AFC_LPFORDER, 0, 0);
    m_LPF1.Clear();
//    m_LPF2.Clear();
	double fc = m_Speed * 6.0;
    if( fc > m_SampleFreq * 0.45 ) fc = m_SampleFreq * 0.45;
	m_LoopLPF.Create(ffLPF, fc, m_SampleFreq, 1, 0, 0);
	if( Is170(m_Type) ){
		m_FAVG.Create(int(128*45.45/m_Speed));
    }
	else if( IsFSK(m_Type) ){
		m_FAVG.Create(16);
    }
    else {
		m_FAVG.Create(int(8192*31.25/m_Speed));
    }

//	m_CIC.Create(4, m_SampleFreq*0.5/m_Speed);
    m_OutLPF.Create(ffLPF, m_Speed*0.5, m_SampleFreq, OUT_LPFORDER, 0, 0);
    m_VCO.SetSampleFreq(m_SampleFreq);

	m_AFC.m_Speed = m_Speed;
	m_AFC.SetTap(int(16*m_SampleFreq/11025));

    m_Decode.SetSampleFreq(m_SampleFreq);
	m_Decode.SetType(m_Type);
    m_Decode.SetSpeed(m_Speed);
	m_Decode.Reset();

	m_AGC.SetSampleFreq(m_SampleFreq);
    m_TankL.m_BW = m_TankH.m_BW = 60.0;
    m_TankL.SetSampleFreq(m_SampleFreq);
    m_TankH.SetSampleFreq(m_SampleFreq);
    m_TankL2.m_BW = m_TankH2.m_BW = 60.0;
    m_TankL2.SetSampleFreq(m_SampleFreq);
    m_TankH2.SetSampleFreq(m_SampleFreq);
	m_PhaseX.SetSampleFreq(m_SampleFreq);
    SetCarrierFreq(m_CarrierFreq);

	m_fEnableAFC = FALSE;
    m_DemLevel = 1.0;
/*
	31.25	88
    45.45	66
    62.5	53
    93.75	37
    125		30
*/
    if( m_fMFSK ){
		if( !m_pMFSK ) m_pMFSK = new CDecMFSK(m_pMainCtrl);
		m_pMFSK->SetMFSKType(MFSK_Tones2Type(m_MFSK_TONES));
		m_pMFSK->SetSampleFreq(m_SampleFreq);
    }
    else {
		delete m_pMFSK;
        m_pMFSK = NULL;
    }
	double spd = m_Speed;
    if( spd < 30 ) spd = 30.0;
	switch(m_Type){
		case MODE_N_BPSK:
		case MODE_BPSK:
			m_LockTh = int((2750*2/3)/spd);
			m_AGC.SetFC(500);
            break;
		case MODE_GMSK:
			m_LockTh = int((2750*1/2)/spd);
			m_AGC.SetFC(500);
            break;
        case MODE_FSKW:
        case MODE_FSK:
			m_LockTh = int((2750*1/3)/spd);
			m_AGC.SetFC(500);
        	break;
        case MODE_RTTY:
        case MODE_U_RTTY:
			m_LockTh = int((2750*1/3)/spd);
			m_AGC.SetFC(500);
        	break;
        case MODE_mfsk_L:
			m_pMFSK->m_fLSB = TRUE;
	        m_pMFSK->SetCarrierFreq(m_CarrierFreq);
            m_Lock = TRUE;
        	break;
        case MODE_mfsk_U:
        	m_pMFSK->m_fLSB = FALSE;
	        m_pMFSK->SetCarrierFreq(m_CarrierFreq);
            m_Lock = TRUE;
            break;
	}
    m_out = m_err = 0.0;
}
//---------------------------------------------------------------------------
void __fastcall CDEMFSK::SetCollect(BOOL f, int buffersize)
{
	if( f ){
		switch(m_Type){
		case MODE_FSKW:
		case MODE_RTTY:
		case MODE_U_RTTY:
			m_TankL2.SetFreq(m_CarrierFreq - (m_RTTYShift * 0.5));
	        m_TankH2.SetFreq(m_CarrierFreq + (m_RTTYShift * 0.5));
			m_Collect[0].Create(buffersize);
			m_Collect[1].Create(buffersize);
			m_bCollect = TRUE;
			break;
		default:
			m_Collect[0].Delete();
			m_Collect[1].Delete();
			m_bCollect = FALSE;
			break;
		}
	}
	else {
		m_Collect[0].Delete();
		m_Collect[1].Delete();
		m_bCollect = FALSE;
	}
}

//---------------------------------------------------------------------------
int __fastcall GetRTTYBW(int taps)
{
	int bw = 180;
	if( taps == g_tBpfTaps[1] ){
		bw = 120;
    }
	else if( taps == g_tBpfTaps[2] ){
		bw = 70;
	}
	else if( taps == g_tBpfTaps[3] ){
		bw = 30;
	}
	return bw;
}

//---------------------------------------------------------------------------
int __fastcall GetMFSKBW(int taps)
{
	int bw = 200;
	if( taps == g_tBpfTaps[1] ){
		bw = 150;
    }
	else if( taps == g_tBpfTaps[2] ){
		bw = 100;
	}
	else if( taps == g_tBpfTaps[3] ){
		bw = 50;
	}
	return bw;
}

//---------------------------------------------------------------------------
void __fastcall CDEMFSK::MakeBPF(int taps)
{
	int bw;
	m_PreBPFTaps = taps;
	m_PreBPFFC = m_CarrierFreq;
	switch(m_Type){
		case MODE_N_BPSK:
		case MODE_BPSK:
		case MODE_GMSK:
		    m_inBPF.Create(m_PreBPFTaps, ffBPF, m_SampleFreq, m_PreBPFFC - m_Speed, m_PreBPFFC + m_Speed, 60, 1.0);
		    m_BPFLimit = m_Speed * 0.125;
            break;
        case MODE_FSK:
		    m_inBPF.Create(m_PreBPFTaps, ffBPF, m_SampleFreq, m_PreBPFFC - m_Speed*2, m_PreBPFFC + m_Speed*2, 60, 1.0);
		    m_BPFLimit = m_Speed * 0.125;
            break;
        case MODE_FSKW:
        case MODE_RTTY:
        case MODE_U_RTTY:
			bw = GetRTTYBW(taps);
		    m_inBPF.Create(m_PreBPFTaps, ffBPF, m_SampleFreq, m_PreBPFFC - m_RTTYShift*0.5 - bw, m_PreBPFFC + m_RTTYShift*0.5 + bw, 60, 1.0);
			m_BPFLimit = 5.0;
            break;
        case MODE_mfsk_L:
			bw = GetMFSKBW(taps);
			if( m_pMainCtrl->m_MFSK_Center ){
			    m_inBPF.Create(m_PreBPFTaps, ffBPF, m_SampleFreq, m_PreBPFFC - m_MFSK_BW*0.5 - bw, m_PreBPFFC + m_MFSK_BW*0.5 + bw, 60, 1.0);
            }
            else {
			    m_inBPF.Create(m_PreBPFTaps, ffBPF, m_SampleFreq, m_PreBPFFC - m_MFSK_BW - bw, m_PreBPFFC + bw, 60, 1.0);
            }
            m_BPFLimit = 2.0;
            break;
        case MODE_mfsk_U:
			bw = GetMFSKBW(taps);
			if( m_pMainCtrl->m_MFSK_Center ){
			    m_inBPF.Create(m_PreBPFTaps, ffBPF, m_SampleFreq, m_PreBPFFC - m_MFSK_BW*0.5 - bw, m_PreBPFFC + m_MFSK_BW*0.5 + bw, 60, 1.0);
            }
            else {
			    m_inBPF.Create(m_PreBPFTaps, ffBPF, m_SampleFreq, m_PreBPFFC - bw, m_PreBPFFC + m_MFSK_BW + bw, 60, 1.0);
            }
            m_BPFLimit = 2.0;
            break;
	}
}

//---------------------------------------------------------------------------
void __fastcall CDEMFSK::SetCarrierFreq(double f)
{
	m_CarrierFreq = f;
	m_RxFreq = m_CarrierFreq;
	MakeBPF(m_PreBPFTaps);
    m_VCO.SetFreeFreq(m_CarrierFreq);
	switch(m_Type){
		case MODE_N_BPSK:
		case MODE_BPSK:
		case MODE_GMSK:
		    m_VCO.SetGain(m_Speed * 0.5 * m_Gain);
			m_AFC.SetPara(m_Speed * 0.5 * m_Gain, m_CarrierFreq);
            m_SW = m_Speed*0.5;
            break;
        case MODE_FSK:
		    m_VCO.SetGain(m_Speed * 1.0 * m_Gain);
			m_AFC.SetPara(m_Speed * 1.0 * m_Gain, m_CarrierFreq);
            m_SW = m_Speed;
        	break;
        case MODE_FSKW:
        case MODE_RTTY:
        case MODE_U_RTTY:
			m_TankL.SetFreq(m_CarrierFreq - (m_RTTYShift * 0.5));
            m_TankH.SetFreq(m_CarrierFreq + (m_RTTYShift * 0.5));
			if( m_bCollect ){
				m_TankL2.SetFreq(m_CarrierFreq - (m_RTTYShift * 0.5));
		        m_TankH2.SetFreq(m_CarrierFreq + (m_RTTYShift * 0.5));
			}
    		m_VCO.SetGain(m_RTTYShift * 1.0 * m_Gain);
			m_AFC.SetPara(m_RTTYShift * 1.0 * m_Gain, m_CarrierFreq);
            m_PhaseX.SetCarrierFreq(m_CarrierFreq - (m_RTTYShift * 0.5));
            m_PhaseX.SetShift(m_RTTYShift);
			m_SW = m_RTTYShift;
        	break;
        case MODE_mfsk_L:
        case MODE_mfsk_U:
			if( m_pMFSK ){
				m_pMFSK->SetCarrierFreq(m_CarrierFreq);
            }
        	break;
    }
    m_AFC.SetTone(m_CarrierFreq);
	m_AGC.SetCarrierFreq(m_CarrierFreq);
	if( IsFSK(m_Type) ) m_FAVG.Reset(m_CarrierFreq);
	m_fEnableAFC = FALSE;
    m_AFCCount = 0;
}
//---------------------------------------------------------------------------
#define	countBPFUPDATE	(3*11025/2048)
void __fastcall CDEMFSK::UpdateBPF(void)
{
	double a = fabs(m_PreBPFFC - m_CarrierFreq);
    if( a >= m_BPFLimit ){
		m_AGC.SetCarrierFreq(m_CarrierFreq);
		MakeBPF(m_PreBPFTaps);
    }
    m_AFCCount++;
//    Application->MainForm->Caption = m_AFCCount;
}
//---------------------------------------------------------------------------
double __fastcall CDEMFSK::GetFreqErr(void)
{
	if( IsBPSK(m_Type) ){
		return m_FAVG.GetAvg();
    }
	else if( m_fMFSK ){
		return (m_RxFreq - m_CarrierFreq) / m_MFSK_SPEED;
    }
    else {
		return (m_RxFreq - m_CarrierFreq) / m_SW;
    }
}
//---------------------------------------------------------------------------
void __fastcall CDEMFSK::DoAFC(double d)
{
	if( m_AFC.Do(d) ){
		if( !m_fEnableAFC ) return;
		if( m_AFCCount < DEMAFCLIMIT ){
			m_RxFreq = m_AFC.GetFreq();
        }
        else {
			m_RxFreq = m_FAVG.Do(m_AFC.GetFreq());
    	}
        if( !m_fAFC ) return;

        double a = fabs(m_CarrierFreq - m_RxFreq);
        if( (a >= 0.1) && ( a <= (m_Speed*1.5)) ){
        	m_CarrierFreq = m_RxFreq;
			if( m_AFCCount < DEMAFCLIMIT ) m_FAVG.Reset(m_RxFreq);
		    m_VCO.SetFreeFreq(m_CarrierFreq);
			switch(m_Type){
				case MODE_GMSK:
		    		m_VCO.SetGain(m_Speed * 0.5 * m_Gain);
					m_AFC.SetPara(m_Speed * 0.5 * m_Gain, m_CarrierFreq);
		            break;
		        case MODE_FSK:
		    		m_VCO.SetGain(m_Speed * 1.0 * m_Gain);
					m_AFC.SetPara(m_Speed * 1.0 * m_Gain, m_CarrierFreq);
		        	break;
                case MODE_FSKW:
                case MODE_RTTY:
                case MODE_U_RTTY:
//					m_TankL.SetFreq(m_CarrierFreq - (m_RTTYShift * 0.5));
//        		    m_TankH.SetFreq(m_CarrierFreq + (m_RTTYShift * 0.5));
		    		m_VCO.SetGain(m_RTTYShift * 1.0 * m_Gain);
					m_AFC.SetPara(m_RTTYShift * 1.0 * m_Gain, m_CarrierFreq);
                	break;
		    }
            m_AFC.SetTone(m_CarrierFreq);
        }
	}
}
//---------------------------------------------------------------------------
void __fastcall CDEMFSK::DoAFCPSK(double d)
{
	d = m_FAVG.DoZ(d);
	if( !m_fEnableAFC ) return;
	m_RxFreq = m_CarrierFreq + d * m_Speed * 0.5;
    if( !m_fAFC ) return;

    double a = fabs(m_CarrierFreq - m_RxFreq);
    if( (a >= 0.1) && ( a <= (m_Speed*1.5)) ){
       	m_CarrierFreq = m_RxFreq;
	    m_VCO.SetFreeFreq(m_CarrierFreq);
		switch(m_Type){
			case MODE_N_BPSK:
			case MODE_BPSK:
			case MODE_GMSK:
	    		m_VCO.SetGain(m_Speed * 0.5 * m_Gain);
				m_AFC.SetPara(m_Speed * 0.5 * m_Gain, m_CarrierFreq);
	            break;
	        case MODE_FSK:
	    		m_VCO.SetGain(m_Speed * 1.0 * m_Gain);
				m_AFC.SetPara(m_Speed * 1.0 * m_Gain, m_CarrierFreq);
	        	break;
	    }
        m_AFC.SetTone(m_CarrierFreq);
	    m_out = 0;
    	m_FAVG.Reset();
	}
}
//---------------------------------------------------------------------------
void __fastcall CDEMFSK::DoAFCRTTY(double d)
{
	if( m_AFC.Do(d) ){
		if( !m_fEnableAFC ) return;
		m_RxFreq = m_AFC.GetFreq();
        if( !m_fAFC ) return;

        double a = fabs(m_CarrierFreq - m_RxFreq);
        if( (a >= 1.0) && ( a <= (150.0)) ){
			a *= 0.5;
			if( m_CarrierFreq < m_RxFreq ){
				m_CarrierFreq += a;
            }
            else {
				m_CarrierFreq -= a;
            }
		    m_VCO.SetFreeFreq(m_CarrierFreq);
			m_TankL.SetFreq(m_CarrierFreq - (m_RTTYShift * 0.5));
   	        m_TankH.SetFreq(m_CarrierFreq + (m_RTTYShift * 0.5));
			if( m_bCollect ){
				m_TankL2.SetFreq(m_CarrierFreq - (m_RTTYShift * 0.5));
   		        m_TankH2.SetFreq(m_CarrierFreq + (m_RTTYShift * 0.5));
			}
   			m_VCO.SetGain(m_RTTYShift * 1.0 * m_Gain);
			m_AFC.SetPara(m_RTTYShift * 1.0 * m_Gain, m_CarrierFreq);
            m_AFC.SetTone(m_CarrierFreq);
			m_PhaseX.SetCarrierFreq(m_CarrierFreq - (m_RTTYShift * 0.5));
        }
	}
}
//---------------------------------------------------------------------------
int __fastcall CDEMFSK::Do(double d)
{
	EPHASE(P_DEMBPF);
	if( m_pBPF ){
		d = m_pBPF->Do(m_inBPF.GetHP());
    }
    else {
		d = m_inBPF.Do(d);
    }
	m_d = d;
	EPHASE(P_DEMAGC);
	d = m_AGC.Do(d);
    switch(m_Type){
    	case MODE_GMSK:				// �������g
        case MODE_FSK:
			EPHASE(P_DEMLPF);
			// Loop Filter
			m_out = m_LoopLPF.Do(m_err);
			EPHASE(P_DEMAFC);
		    DoAFC(m_out);
			if( m_out > 1.5 ){ m_out = 1.5; } else if( m_out < -1.5 ){ m_out = -1.5;}
			EPHASE(P_DEMLOCK);
			m_Lock = int((m_AFC.m_Max - m_AFC.m_Min) * 100);
		    m_Lock = (m_Lock > m_LockTh);
//		    m_FreqErr = (m_AFC.m_Max + m_AFC.m_Min) * 0.5;
			// VCO
			// �ʑ���r
			EPHASE(P_DEMVCO);
			m_err = m_VCO.Do(m_out) * d;
			EPHASE(P_DEMOUT);
		    d = m_OutLPF.Do(m_out);
			EPHASE(P_NULL);
			return int(d * 32768.0);
		case MODE_BPSK:
        case MODE_N_BPSK:			// �������g
			{
				EPHASE(P_NULL);
				m_Lock = TRUE;
    	    	double ds = d * m_VCO.Do(m_out);
        	    double dc = d * m_VCO.DoCos();
				m_out = m_LPF1.Do(ABS(ds)-ABS(dc));
	   	        DoAFCPSK(m_out);
        	    return int(m_OutLPF.Do(ds - dc) * 32768.0);
            }
		case MODE_FSKW:
    	case MODE_RTTY:
        case MODE_U_RTTY:
			if( m_fRTTYTANK ){		// �N�I�h���`�����g
				EPHASE(P_DEMLPF);
				// Loop Filter
				m_out = m_LoopLPF.Do(m_err);
				EPHASE(P_DEMAFC);
			    DoAFCRTTY(m_out);
				if( m_out > 1.5 ){ m_out = 1.5; } else if( m_out < -1.5 ){ m_out = -1.5;}
				EPHASE(P_DEMLOCK);
				m_Lock = int((m_AFC.m_Max - m_AFC.m_Min) * 100);
			    m_Lock = (m_Lock > m_LockTh) && (m_DemLevel >= 0.5);
//		    	m_FreqErr = (m_AFC.m_Max + m_AFC.m_Min) * 0.5;
				// VCO
				// �ʑ���r
				EPHASE(P_DEMVCO);
				m_err = m_VCO.Do(m_out) * d;
				EPHASE(P_NULL);

				if( m_bCollect && m_Collect[0].GetMax() ){
					m_Collect[0].Do(long(m_TankH2.Do(d)*8192.0));
					m_Collect[1].Do(long(m_TankL2.Do(d)*8182.0));
				}

				if( m_fRTTYFFT ){
					m_PhaseX.DoFSK(d);
					double ds = m_PhaseX.m_ds;
	    	        double dm = m_PhaseX.m_dm;
					d = m_OutLPF.Do(m_LPF1.Do(ABS(ds)-ABS(dm)));
	                m_DemLevel = m_FAVG.Do(ABS(d)) * 4.0;
		            if( m_DemLevel >= 1.0 ) m_DemLevel = 1.0;
					if( m_Type == MODE_RTTY ) d = -d;
	        	    return int(d * 32768.0);
                }
				else {
					d *= 32.0;
	                if(	d > 1.0 ) d = 1.0;
					if( d < -1.0 ) d = -1.0;
					double ds = m_TankH.Do(d);
	    	        double dm = m_TankL.Do(d);
					d = m_OutLPF.Do(m_LPF1.Do(ABS(ds)-ABS(dm)));
	                m_DemLevel = m_FAVG.Do(ABS(d)) * 4.0;
		            if( m_DemLevel >= 1.0 ) m_DemLevel = 1.0;
					if( m_Type == MODE_RTTY ) d = -d;
	        	    return int(d * 32768.0);
				}
			}
            else {					// �������g
				EPHASE(P_DEMLPF);
				// Loop Filter
				m_out = m_LoopLPF.Do(m_err);
				EPHASE(P_DEMAFC);
			    DoAFC(m_out);
				if( m_out > 1.5 ){ m_out = 1.5; } else if( m_out < -1.5 ){ m_out = -1.5;}
				EPHASE(P_DEMLOCK);
				m_Lock = int((m_AFC.m_Max - m_AFC.m_Min) * 100);
			    m_Lock = (m_Lock > m_LockTh);
				// VCO
				// �ʑ���r
				EPHASE(P_DEMVCO);
				m_err = m_VCO.Do(m_out) * d;
				EPHASE(P_DEMOUT);
			    d = m_OutLPF.Do(m_out);
				EPHASE(P_NULL);
				if( m_Type == MODE_RTTY ) d = -d;
				return int(d * 32768.0);
            }
    }
   	return 0;
}
//---------------------------------------------------------------------------
int __fastcall CDEMFSK::Do(double d, BOOL fSQ, BOOL fATC)
{
	if( m_fMFSK ){
		if( m_pBPF ){
			d = m_pBPF->Do(m_inBPF.GetHP());
	    }
    	else {
			d = m_inBPF.Do(d);
	    }
		m_d = d;
		m_AGC.Do(d);
		return m_pMFSK->Do(m_pMFSK->m_Phase.Do(d), fSQ);
    }
    else {
		return m_Decode.Do(Do(d), fSQ && m_Lock, fATC );
    }
}
//---------------------------------------------------------------------------
int __fastcall CDEMFSK::GetData(void)
{
	if( m_fMFSK ){
		if( !m_pMFSK ) return 0;
		return m_pMFSK->GetData();
    }
    else {
		return m_Decode.GetData();
    }
}
//---------------------------------------------------------------------------
int __fastcall CDEMFSK::GetTmg(void)
{
	if( m_fMFSK ){
		return m_pMFSK->GetTmg() ? 1 : -1;
    }
    else {
		return m_Decode.GetTmg();
    }
}
//---------------------------------------------------------------------------
int __fastcall CDEMFSK::GetTmgLock(void)
{
	if( m_fMFSK ){
		return m_pMFSK->GetTmgLock();
    }
    else {
		return m_Decode.m_Lock;
    }
}
//---------------------------------------------------------------------------
int __fastcall CDEMFSK::GetS(void)
{
	if( m_fMFSK ){
		return (m_pMFSK->GetS() - (m_MFSK_TONES/2)) * 32768/m_MFSK_TONES;
    }
    else {
		return int(m_Decode.m_s ? 16384 : -16384);
    }
}
//---------------------------------------------------------------------------
void __fastcall CDEMFSK::ResetMFSK(void)
{
	if( m_pMFSK ){
		m_pMFSK->Reset();
    }
}
//---------------------------------------------------------------------------
void __fastcall CDEMFSK::ResetMeasMFSK(void)
{
	if( m_pMFSK ){
		m_pMFSK->ResetMeas();
    }
}
//---------------------------------------------------------------------------
BOOL __fastcall CDEMFSK::GetSyncState(void)
{
	if( m_fMFSK ){
		if( !m_pMFSK ) return FALSE;
		return m_pMFSK->GetSyncState();
    }
    else {
		return m_Decode.GetSyncState();
    }
}
//---------------------------------------------------------------------------
int __fastcall CDEMFSK::GetClockError(void)
{
	if( m_fMFSK ){
		if( !m_pMFSK ) return 0;
		return m_pMFSK->GetClockError();
    }
    else {
		double d = m_Decode.m_dTmg2;
        if( d >= 0 ){d += 0.5;} else { d -= 0.5; }
		return int(d);
    }
}
//---------------------------------------------------------------------------
void __fastcall CDEMFSK::SetTmg(int ppm)
{
	if( m_fMFSK ){
		m_pMFSK->SetTmg(ppm);
    }
    else if( !IsRTTY(m_Type) ){
		m_Decode.SetTmg(ppm);
    }
}
//---------------------------------------------------------------------------
CMODFSK::CMODFSK()
{
	m_pMainCtrl = NULL;

    m_MFSK_TONES = 16;
	m_MFSK_SPEED = 15.625;
	m_MFSK_BW = (m_MFSK_SPEED * (m_MFSK_TONES-1));

	m_MFSK_K = 1.0 / double(m_MFSK_TONES);
	m_fMFSK = FALSE;

	m_Type = 0;
	m_Speed = SPEED;
    m_CWSpeed = 20;
	m_SampleFreq = SAMPFREQ;
	m_CarrierFreq = CARRIERFREQ;
    m_Encode.SetVCO(&m_Carrier);
    m_OutVol = 1;
    m_RTTYShift = 170.0;
	Create();
}
//---------------------------------------------------------------------------
CMODFSK::~CMODFSK()
{
}
//---------------------------------------------------------------------------
void __fastcall CMODFSK::SetMFSKType(int type)
{
	MFSK_SetPara(type, &m_MFSK_TONES, &m_MFSK_SPEED, NULL);
	m_MFSK_BW = (m_MFSK_SPEED * (m_MFSK_TONES-1));
	m_MFSK_K = 1.0 / double(m_MFSK_TONES);
    m_Encode.SetMFSKType(type);
    SetCarrierFreq(m_CarrierFreq);
}
//---------------------------------------------------------------------------
void __fastcall CMODFSK::Reset(void)
{
	SetCWSpeed(m_CWSpeed);
    m_OutVol = 1;
    m_AMPCW.Reset();
    m_AMPSIG.Reset();
    m_BPF.Clear();
}

//---------------------------------------------------------------------------
void __fastcall CMODFSK::CreateGMSK(void)
{
	// Gaussian�����̃e�[�u�����쐬���� (1.0 to -1.0)
	int i;
    double w = m_SampleFreq/m_Speed;
	double BT = 1.0;
    double LN2 = sqrt(2.0/log(2.0));
    double LN2P = sqrt(2.0*PI/log(2.0));
    double cd = 0;
    for( i = 0; i < MODTABLEMAX; i++ ){
//		double d = LN2 * PI * BT * double(i)/w;
//		d = BT * exp(-(d * d));
//        d = 1.0 - d;
		double d = LN2 * PI * BT * double(i-w*0.5)/w;
		d = LN2P * BT * exp(-(d * d));
        cd += (d / w);
		// d = 0 to 1
		m_Tbl[MOD_Z2P][i] = cd;					// 0 to 1
		m_Tbl[MOD_Z2M][i] = -cd;					// 0 to -1
        m_Tbl[MOD_P2M][i] = 1.0 - cd * 2;		// 1 to -1
        m_Tbl[MOD_M2P][i] = cd * 2 - 1.0;		// -1 to 1
    }
}

//---------------------------------------------------------------------------
void __fastcall CMODFSK::CreatePSK(void)
{
//FILE *fp = fopen("test.txt", "wt");
	// Cos�����̃e�[�u�����쐬���� (1.0 to -1.0)
	int i;
    double k = PI * m_Speed / m_SampleFreq;
    for( i = 0; i < MODTABLEMAX; i++ ){
		double cd = cos(double(i)*k);
		if( i >= (m_SampleFreq / m_Speed) ) cd = -1.0;
		m_Tbl[MOD_Z2P][i] = -cd;			  	// 0 to 1
		m_Tbl[MOD_Z2M][i] = cd;					// 0 to -1
        m_Tbl[MOD_P2M][i] = cd;					// 1 to -1
        m_Tbl[MOD_M2P][i] = -cd;				// -1 to 1
//		fprintf(fp, "%4u: %.4lf\n", i, cd);
    }
//fclose(fp);
}

//---------------------------------------------------------------------------
void __fastcall CMODFSK::CreateRTTY(void)
{
#if 0
	// Gaussian�����̃e�[�u�����쐬���� (1.0 to -1.0)
	int i;
    double w = m_SampleFreq/m_Speed;
	double BT = 0.5;
    double LN2 = sqrt(2.0/log(2.0));
    double LN2P = sqrt(2.0*PI/log(2.0));
    double cd = 0;
    for( i = 0; i < MODTABLEMAX; i++ ){
//		double d = LN2 * PI * BT * double(i)/w;
//		d = BT * exp(-(d * d));
//        d = 1.0 - d;
		double d = LN2 * PI * BT * double(i-w*0.5)/w;
		d = LN2P * BT * exp(-(d * d));
        cd += (d / w);
		// d = 0 to 1
		m_Tbl[MOD_Z2P][i] = cd;					// 0 to 1
		m_Tbl[MOD_Z2M][i] = -cd;					// 0 to -1
        m_Tbl[MOD_P2M][i] = 1.0 - cd * 2;		// 1 to -1
        m_Tbl[MOD_M2P][i] = cd * 2 - 1.0;		// -1 to 1
    }
#else
	int i;
    for( i = 0; i < MODTABLEMAX; i++ ){
		m_Tbl[MOD_Z2P][i] = 1;		// 0 to 1
		m_Tbl[MOD_Z2M][i] = -1;		// 0 to -1
        m_Tbl[MOD_P2M][i] = -1;		// 1 to -1
        m_Tbl[MOD_M2P][i] = 1;		// -1 to 1
    }
#endif
}

//---------------------------------------------------------------------------
void __fastcall CMODFSK::Create(void)
{
	m_fMFSK = IsMFSK(m_Type);
	m_Encode.SetType(m_Type);
	if( m_fMFSK ){
	    m_Encode.SetMFSKType(MFSK_Tones2Type(m_MFSK_TONES));
    }
	m_s = 0;
	m_Carrier.SetSampleFreq(m_SampleFreq);

    SetCarrierFreq(m_CarrierFreq);
	m_Encode.SetSampleFreq(m_SampleFreq);
	m_Encode.SetSpeed(m_Speed);
	SetCWSpeed(m_CWSpeed);
	m_AMPSIG.SetMax(int(m_SampleFreq/m_Speed));

    switch(m_Type){
		case MODE_GMSK:
        case MODE_FSK:
        case MODE_FSKW:
			m_Encode.m_fTWO = FALSE;
		    CreateGMSK();
        	break;
		case MODE_BPSK:
			m_Encode.m_fTWO = FALSE;
			CreatePSK();
			break;
        case MODE_N_BPSK:
			m_Encode.m_fTWO = TRUE;
			CreatePSK();
        	break;
        case MODE_RTTY:
        case MODE_U_RTTY:
			m_Encode.m_fTWO = FALSE;
			if( m_Speed >= m_RTTYShift ){
			    CreateGMSK();
            }
            else {
			    CreateRTTY();
            }
        	break;
        case MODE_mfsk_L:
        case MODE_mfsk_U:
			m_Encode.m_fTWO = TRUE;
			CreateRTTY();	// dummy
        	break;
	}
    m_pTbl = NULL;
    m_d = 0.0;
}

//---------------------------------------------------------------------------
void __fastcall CMODFSK::SetCWSpeed(int d)
{
	m_CWSpeed = d;
    int dCW = int(m_SampleFreq/m_CWSpeed);
	if( dCW < 2 ) dCW = 2;
	m_AMPCW.SetMax(int(dCW*0.25));
	m_Encode.SetCW(d);
}

//---------------------------------------------------------------------------
void __fastcall CMODFSK::SetCarrierFreq(double f)
{
	m_CarrierFreq = f;
	m_Carrier.SetFreeFreq(m_CarrierFreq);
	switch(m_Type){
		case MODE_BPSK:
        case MODE_N_BPSK:
		case MODE_GMSK:
		    m_Carrier.SetGain((m_Speed * 0.5) * 0.5);	// fm = 0.5
            break;
        case MODE_FSK:
		    m_Carrier.SetGain((m_Speed * 0.5) * 1.0);	// fm = 1.0
			break;
		case MODE_FSKW:
        case MODE_RTTY:
        case MODE_U_RTTY:
		    m_Carrier.SetGain(m_RTTYShift * 0.5);
			break;
        case MODE_mfsk_L:
		    m_Carrier.SetGain(-250);
			if( m_pMainCtrl && m_pMainCtrl->m_MFSK_Center ){
				m_Carrier.SetFreeFreq(m_CarrierFreq+m_MFSK_BW*0.5);
            }
			break;
        case MODE_mfsk_U:
		    m_Carrier.SetGain(250);
			if( m_pMainCtrl && m_pMainCtrl->m_MFSK_Center ){
				m_Carrier.SetFreeFreq(m_CarrierFreq-m_MFSK_BW*0.5);
            }
			break;
    }
	double fl, fh;
    int fHigh = (m_SampleFreq >= 15000.0);
    int type = ffBPF;
    double dATT = 60;
    if( fHigh ) dATT -= ((m_SampleFreq - 15000.0)/1750);
	if( Is170(m_Type) ){
		double hw = m_RTTYShift * 0.5;
	    fl = m_CarrierFreq - hw - (fHigh ? (m_SampleFreq*0.1) : 200);
    	fh = m_CarrierFreq + hw + (fHigh ? (m_SampleFreq*0.1) : 200);
    }
	else if( m_fMFSK ){
		double hw = m_MFSK_SPEED;
		if( m_Type == MODE_mfsk_U ){
		    fl = m_CarrierFreq - hw - (fHigh ? (m_SampleFreq*0.1) : 200);
    		fh = m_CarrierFreq + m_MFSK_BW + hw + (fHigh ? (m_SampleFreq*0.1) : 200);
        }
        else {
		    fl = m_CarrierFreq - m_MFSK_BW - hw - (fHigh ? (m_SampleFreq*0.1) : 200);
    		fh = m_CarrierFreq + hw + (fHigh ? (m_SampleFreq*0.1) : 200);
        }
    }
    else {
	    fl = m_CarrierFreq-m_Speed - (fHigh ? (m_SampleFreq*0.1) : 200);
    	fh = m_CarrierFreq+m_Speed + (fHigh ? (m_SampleFreq*0.1) : 200);
    }
    if( fl < (fHigh ? 800 : 200) ){
		type = ffLPF;
        fl = fh;
    }
	m_BPF.Create(24, type, m_SampleFreq, fl, fh, dATT, 1.0);
}

//--------------------------------------------------------
double __fastcall CMODFSK::Do(void)
{
	int s = m_Encode.Do();
	if( (s == 128)||(s == 129) ){
		s -= 128;			// s = 0 or 1
		return m_BPF.Do(m_Carrier.Do() * m_AMPCW.Do(s));
    }
	else if( m_fMFSK ){
		return m_BPF.Do(m_Carrier.Do(double(s)*m_MFSK_K)*m_AMPSIG.Do(m_OutVol));
    }
    int cnt = m_Encode.m_Cnt;
	if( !cnt ){
		if( s > 0 ){
			if( m_s > 0 ){					// 1 to 1
               	m_d = 1.0; m_pTbl = NULL;
            }
            else if( m_s < 0 ){            	// -1 to 1
				m_pTbl = m_Tbl[MOD_M2P];
            }
            else {		                	// 0 to 1
				m_pTbl = m_Tbl[MOD_Z2P];
            }
        }
        else if( s < 0 ){
			if( m_s > 0 ){                	// 1 to -1
				m_pTbl = m_Tbl[MOD_P2M];
            }
            else if( m_s < 0 ){            	// -1 to -1
				m_d = -1; m_pTbl = NULL;
            }
            else {		                	// 0 to -1
				m_pTbl = m_Tbl[MOD_Z2M];
            }
        }
        else {		            			// 0
			m_d = 0; m_pTbl = NULL;
        }
        m_s = s;
    }
	if( cnt > MODTABLEMAX ) cnt = MODTABLEMAX - 1;
	if( m_pTbl ){
		m_d = m_pTbl[cnt];
    }
	double d;
	switch(m_Type){
		case MODE_GMSK:			// ���g���ϒ�
        case MODE_FSK:
        case MODE_FSKW:
		case MODE_RTTY:
			d = m_Carrier.Do(m_d);
            break;
        case MODE_BPSK:
        case MODE_N_BPSK:		// ����ϒ�
			d = m_Carrier.Do() * m_d;
			d += m_Carrier.DoCos() * -m_d;
			d *= PSK_OUTFAC;
			break;
        case MODE_U_RTTY:
			d = m_Carrier.Do(-m_d);
            break;
        default:
        	d = 0.0;
            break;
    }
	return m_BPF.Do(d*m_AMPSIG.Do(m_OutVol));
}

double __fastcall CMODFSK::DoCarrier(void)
{
	return m_BPF.Do(m_Carrier.Do()*m_AMPSIG.Do(m_OutVol));
}

//--------------------------------------------------------
CDECFSK::CDECFSK()
{
	m_Tmg = 0;
    m_cMode = 0;
    m_SampleFreq = SAMPFREQ;
//	m_SampleFreq = 11100;
    m_Speed = SPEED;
    m_Lock = 0;
    m_dAdj = 0;
	m_dTmg3 = 0;
	m_dTmg2 = 0;
	m_dTmg = 0;
    m_fATC = TRUE;
    Create();
    m_T = 0;
    m_T2 = 0;
    m_T3 = 0;
	m_cBWave = 15;
    m_ATCSpeed = 0;
    m_ATCLimit = 25000;

    m_ATCCounter = 0;
    m_Type = MODE_GMSK;
    m_AvgRTTY.Create(48);
}

#define	INI_T2	64
#define	LPFA	20
#define	LPFB	16
void __fastcall CDECFSK::Create(void)
{
	m_dBTW = m_dTW = m_SampleFreq / m_Speed;
	SetATCLimit(m_ATCLimit);
	SetTmg(int(m_dAdj));
    m_LPF.Create(int(LPFA*m_SampleFreq/11025));
    m_LPFI.Create(int(LPFB*m_SampleFreq/11025));
    m_cMode = 0;
	for( int i = 0; i < 16; i++ ){
    	m_LPF.Do(0);
    	m_LPFI.Do(0);
    }
    m_GainA = 60.0 / m_Speed;
	SetATCSpeed(m_ATCSpeed);
	m_dNow = 0;
    m_dNext = m_dTW;
    m_dFree = m_dTW;
    m_T = 0;
    m_T2 = INI_T2;
    m_T3 = 0;
    m_Threshold = 128.0;
    m_A = 0;
    if( m_Speed >= 70.0 ) m_Threshold *= 0.7;
    m_a = 0;

    m_ATCCounter = 0;
    m_ATCLimitH = int(32 * m_SampleFreq / m_Speed);
    m_ATCLimitL = int(0.5 * m_SampleFreq / m_Speed);
}

void __fastcall CDECFSK::SetATCSpeed(int atcspeed)
{
	m_ATCSpeed = atcspeed;
//    m_GainB = 0.002 * 60.0 / m_Speed;
    m_GainB = 0.002 * 60.0 / 31.25;
	if( m_ATCSpeed ){
		m_GainB *= ((m_ATCSpeed * 0.5) + 1.0);
    }
}

void __fastcall CDECFSK::SetATCLimit(int atclimit)
{
	m_ATCLimit = atclimit;
	double d = double(atclimit) * 1.0e-6;
    m_dBTWL = m_dBTW * (1.0-d);
    m_dBTWH = m_dBTW * (1.0+d);
}

void __fastcall CDECFSK::ClearLPF(void)
{
	for( int i = 0; i < 16; i++ ){
    	m_LPF.Do(0);
    	m_LPFI.Do(0);
    }
}

void __fastcall CDECFSK::Reset(void)
{
	m_dTW = m_SampleFreq / m_Speed;
    m_dTWH = m_dTW * 0.5;
	m_dNow = 0;
    m_dNext = m_dTW;
    m_dFree = m_dTW;
    m_T = 0;
    ClearLPF();
    m_T2 = INI_T2;
    m_T3 = 0;

	m_dTmg3 = 0;
	m_dTmg2 = 0;
	m_dTmg = 0;

    m_ATCCounter = 0;
    m_ATCLimitH = int(32 * m_SampleFreq / m_Speed);
    m_ATCLimitL = int(0.5 * m_SampleFreq / m_Speed);

	m_fSync = FALSE;
    m_BAUDOT.ClearRX();

	m_fMeasClock = FALSE;
	m_MeasError = m_MeasClock = m_MeasCounter = 0;
    m_Meas1st = TRUE;
    m_AvgRTTY.Reset();
}

void __fastcall CDECFSK::SetTmg(int ppm)
{
	if( (ppm < -50000) || (ppm > 50000) ) return;

	m_dAdj = ppm;
	m_dTW = m_SampleFreq / m_Speed;
	m_dTW += m_dTW * m_dAdj * 1.0e-6;
//    m_dBTW = m_dTW;
    m_dTWH = m_dTW * 0.5;
    m_T2 = INI_T2;
}

void __fastcall CDECFSK::SetMeasClock(BOOL f)
{
	if( IsRTTY(m_Type) ) return;

	m_fMeasClock = f;
	if( !f ) m_MeasClock = m_MeasCounter = 0;
}

BOOL __fastcall CDECFSK::GetSyncState(void)
{
	if( IsRTTY(m_Type) ){
		return m_fSync;
    }
	else if( m_T && (m_T3 > 16) ){
		return (fabs(m_dTmg) >= (0.2*60.0/m_Speed)) ? FALSE : TRUE;
    }
    else {
		return FALSE;
    }
}

BOOL __fastcall CDECFSK::Do(double d, BOOL sq, BOOL fATC)
{
	BOOL r = FALSE;
    int s = m_s;
	if( ABS(d) > m_Threshold ){
		s = (d > 0) ? 1 : 0;
    }
	if( IsRTTY(m_Type) ){
		return DoBAUDOT(s, sq);
    }
	int u;
	m_ATCCounter++;
	m_dNow += 1.0;
	if( m_dNow >= m_dFree ) m_dFree += m_dTW;
	if( s != m_s ){
		m_s = s;
		if( (m_ATCCounter >= m_ATCLimitL) && (m_ATCCounter < m_ATCLimitH) ){
			BOOL f1st = sq && !m_T;
			d = m_dFree - m_dNow;
	        if( d >= m_dTWH ) d -= m_dTW;
    	    d /= m_dBTW;
        	if( (m_dTmg >= 0.3) && (d < -0.1) ) d += 1.0;
	        if( (m_dTmg < -0.3) && (d > 0.1) ) d -= 1.0;
			if( !sq ) d *= 0.1;
			if( sq && !m_T ){				// �ŏ���1���
				m_dFree -= d * m_dBTW;
    	    }
   			d = m_LPF.Do(d * m_GainA);
	        m_dTmg3 = d - m_A;
    	    m_A = d;
        	m_dTmg = d;
			if( m_T ){
				if( m_T3 < 256 ){
					d *= 2.0;
            	}
	            else if( !fATC ){
					d *= 0.5;
        	    }
//			if( !fATC ) d *= (m_T3 > 256) ? 0.25 : 2.0;
				m_dFree -= d;
    	    }

			if( sq ){
				m_T = int(m_Speed);
				m_T3++;
	        }
    	    else if( !m_T ){
				d = (m_dTW - m_dBTW) * 20.0;
		    	m_T2 = INI_T2;
	            m_T3 = 0;
                m_MeasClock = m_MeasCounter = 0;
    	   	}
       		else {
				m_T--;
                m_MeasClock = m_MeasCounter = 0;
	       	}
    	    d = m_LPFI.Do(d);
			if( (!m_T2 && fATC) || !m_T  ){
				if( m_fATC ){
			   	    m_dTW -= d * m_GainB;
					if( m_dTW < m_dBTWL ){ m_dTW = m_dBTWL; }
        	        if( m_dTW > m_dBTWH ){ m_dTW = m_dBTWH; }
				   	m_dTWH = m_dTW * 0.5;
	            }
    	    }
        	else if( m_T2 ){
				m_T2--;
    	    }
            m_dTmg2 = ((m_dTW / m_dBTW) - 1.0) * 1e6;
            if( GetSyncState() ){
				m_dNext = m_dFree - m_dTW*0.5;
				if( m_dNext > (m_dNow + m_dTW) ){
					m_dNext -= m_dTW;
	        	}
	    	    if( m_dNext < m_dNow ) m_dNext += m_dTW;
            }
			else if( f1st ){
				m_dNext = m_dNow + m_dTWH;
                m_MeasClock = m_MeasCounter = 0;
            }
            else {
                d = m_dNext - m_dNow;
                m_dNext = m_dNow + (d + m_dTWH) * 0.5;
//                m_dNext = m_dNow + (d * 0.25 + m_dTWH * 0.75);
			}
		}
        m_ATCCounter = 0;
    }
	if( m_MeasClock ) m_MeasCounter++;
    switch(m_cMode){
		case 0:
			if( m_dNext <= m_dNow ){
				m_MeasClock++;
				if( m_MeasClock >= 256 ){
					if( sq && fATC && !GetSyncState() && m_fMeasClock ){
						d = double(m_MeasCounter) / double(m_MeasClock-1);
                        m_dTW += (d - m_dTW) * 0.5;
						if( m_dTW < m_dBTWL ){ m_dTW = m_dBTWL; }
    	    	        if( m_dTW > m_dBTWH ){ m_dTW = m_dBTWH; }
					   	m_dTWH = m_dTW * 0.5;
                    }
	                m_MeasClock = 1;
                    m_MeasCounter = 0;
                }
				m_Tmg = !m_Tmg;
				m_dNext += m_dTW;
				u = (s != m_a) ? 0 : 1;
                m_a = s;
                m_cData = m_cData << 1;
				if( u && sq ){
					if( !m_Lock ) m_Tmg = 1;
                	m_cData = 1;
					m_cBCount = 1;
                    m_cMode++;
                    m_Lock = TRUE;
                }
                else {
					if( !m_cBCount ){
						m_Lock = TRUE;
                    }
                    else if( m_cBCount >= m_cBWave ){
						m_cBCount = -1;
						m_Lock = FALSE;
                    }
                    m_cBCount++;
                }
			}
        	break;
		case 1:
			if( m_dNext <= m_dNow ){
				m_MeasClock++;
				m_Tmg = !m_Tmg;
				m_dNext += m_dTW;
				u = (s != m_a) ? 0 : 1;
                m_a = s;
                m_cData = m_cData << 1;
				m_cBCount++;
				if( u ){
                	m_cData |= 1;
                }
                else if( m_cBCount >= 3){
					if( !(m_cData & 3) ){
                        m_Data = g_VariCode.Decode(m_cData >> 2);
   	                    r = TRUE;
                        m_cMode--;
	                    m_Lock = FALSE;
                    }
                }
			}
        	break;
        default:
        	m_cMode = 0;
            break;
    }
	if( m_dNow >= 1000.0 ){
		m_dNow -= 1000.0;
        m_dNext -= 1000.0;
        m_dFree -= 1000.0;
    }
	return r;
}

//---------------------------------------------------------------------------
BOOL __fastcall CDECFSK::DoBAUDOT(int s, BOOL sq)
{
	if( s != m_s ){
		m_s = s;
        if( sq ){
			if( !m_cMode && !s ){
				if( m_MeasClock ){
					int tw = m_MeasCounter - m_MeasClock;
                    int stw = int(m_SampleFreq * 6.5 / m_Speed);
					if( tw < stw ){
						m_MeasError++;
						if( m_MeasError >= 3 ){
							m_MeasError = m_MeasCounter = m_MeasClock = 0;
       	                    m_AvgRTTY.Reset();
   	        	        }
                    }
					else if( m_AvgRTTY.IsHalf() ){
						int atw = int(m_AvgRTTY.GetAvg());
       	        	    if( ABS(atw-tw) >= (m_dTW*0.2) ){
							m_MeasError++;
							if( m_MeasError >= 3 ){
								m_MeasError = m_MeasCounter = m_MeasClock = 0;
        	                    m_AvgRTTY.Reset();
       	        	        }
           		        }
	               	    else {
							m_AvgRTTY.Do(tw);
	   	                    m_MeasError = 0;
        	            }
       	    	    }
           	    	else {
						m_AvgRTTY.Do(tw);
                    }
                }
				if( m_Meas1st ){
                	m_AvgRTTY.Reset();
                }
                else {
		            m_MeasClock = m_MeasCounter = 1;
                }
            }
        }
        else {
			m_MeasError = m_MeasCounter = m_MeasClock = 0;
		    m_Meas1st = TRUE;
        }
    }
	BOOL r = FALSE;
	m_MeasCounter++;
	m_dNow += 1.0;
	switch(m_cMode){
        case 0:				// Start bits?
			if( !s && sq ){
                m_Lock = TRUE;
				m_Tmg = 0;
				m_dNext = m_dNow + m_dTW*0.5;
                m_cData = 0;
                m_cBCount = 0;
                m_cMode++;
            }
        	break;
		case 1:
			if( m_dNext <= m_dNow ){
				m_dNext += m_dTW;
				m_Tmg = !m_Tmg;
                m_cMode++;
			}
            break;
        case 2:
			if( m_dNext <= m_dNow ){
				m_dNext += m_dTW;
				m_Tmg = !m_Tmg;
				m_cData = m_cData << 1;
                if( s ) m_cData |= 0x01;
                m_cBCount++;
                if( m_cBCount >= 5 ){
					m_cMode++;
                }
			}
            break;
        case 3:							// Stop bits
        	if( m_dNext <= m_dNow ){
				m_dNext += m_dTW;
				m_Tmg = !m_Tmg;
                m_fSync = s;
				if( s ){
					m_Meas1st = FALSE;
					r = TRUE;
					m_cMode = 0;
                    m_Data = m_BAUDOT.ConvAscii(m_cData);
                }
                else {					// �t���[�~���O�G���[
					m_cMode++;
					m_MeasClock = 0;
                }
                m_Lock = FALSE;
            }
            break;
        case 4:
        	if( m_dNext <= m_dNow ){
				m_dNext += m_dTW;
            }
			if( s ){
				m_cMode = 0;
            }
            break;
        default:
			m_cMode = 0;
        	break;
    }
	if( m_dNow >= 1000.0 ){
		m_dNow -= 1000.0;
        m_dNext -= 1000.0;
    }
    return r;
}
//--------------------------------------------------------------------------
void __fastcall DoAvg(double &av, double in, double factor)
{
	av = av * (1.0 - factor) + (in * factor);
}
//---------------------------------------------------------------------------
CFAVG::CFAVG()
{
	Create(16);
}
//---------------------------------------------------------------------------
void __fastcall CFAVG::Reset(void)
{
	m_Cnt = 0;
    m_Sum = 0;
    m_Avg = 0;
}
//---------------------------------------------------------------------------
void __fastcall CFAVG::Reset(double d)
{
	m_Cnt = m_Max;
    m_Sum = d * m_Max;
    m_Avg = d;
}
//---------------------------------------------------------------------------
void __fastcall CFAVG::Create(int max)
{
	m_Max = max;
    m_Mul = 1.0 / m_Max;
    Reset();
}
//---------------------------------------------------------------------------
double __fastcall CFAVG::DoZ(double d)
{
	m_Sum += d;
    if( m_Cnt < m_Max ){
		m_Cnt++;
    }
    else {
		m_Sum -= m_Avg;
    }
    m_Avg = m_Sum * m_Mul;
    return m_Avg;
}
//---------------------------------------------------------------------------
double __fastcall CFAVG::Do(double d)
{
	m_Sum += d;
    if( m_Cnt < m_Max ){
		m_Cnt++;
	    m_Avg = m_Sum / m_Cnt;
    }
    else {
		m_Sum -= m_Avg;
	    m_Avg = m_Sum * m_Mul;
    }
    return m_Avg;
}
//---------------------------------------------------------------------------
CAVG::CAVG()
{
	m_Max = 0;
    m_pZ = NULL;
    m_Sum = 0.0;
    m_Avg = 0;
    m_Cnt = m_W = 0;
}

CAVG::~CAVG()
{
	if( m_pZ ) delete m_pZ;
}

void __fastcall CAVG::Create(int max)
{
	if( max == m_Max ) return;

	m_Max = max;
	if( m_pZ ) delete m_pZ;
    m_pZ = new double[max];
    Reset();
}

void __fastcall CAVG::Reset(void)
{
	m_Cnt = m_W = 0;
    m_Sum = 0.0;
    m_Avg = 0.0;
}

void __fastcall CAVG::Reset(double d)
{
	if( !m_pZ ) return;

    double *dp = m_pZ;
    for( int i = 0; i < m_Max; i++ ){
		*dp++ = d;
    }
	m_Avg = d;
    m_Sum = d * m_Max;
    m_W = 0;
    m_Cnt = m_Max;
}

double __fastcall CAVG::Do(double d)
{
	if( !m_Max ) return d;

	double *dp = &m_pZ[m_W];
	if( m_Cnt >= m_Max ){
		m_Sum -= *dp;
    }
    else {
		m_Cnt++;
    }
    m_W++;
	if( m_W >= m_Max ) m_W = 0;
    *dp = d;
    m_Sum += d;
    m_Avg = m_Sum / m_Cnt;
    return m_Avg;
}

//--------------------------------------------------------
CENCODE::CENCODE()
{
	m_MFSK_TONES = 16;
	m_MFSK_SPEED = 15.625;
	m_MFSK_BITS = 4;
    m_MFSK_MASK = m_MFSK_TONES - 1;

	m_pMainCtrl = NULL;
	m_Speed = SPEED;
    m_SampleFreq = SAMPFREQ;
    m_Code = 0;
    Create();
    m_RP = m_WP = m_CC = 0;
    m_Idle = FALSE;
    m_pVCO = NULL;
    m_fJA = TRUE;
    m_fTWO = FALSE;
    m_fCW = FALSE;
    m_Type = MODE_GMSK;
    m_BAUDOT.Reset();
    m_rttyDiddle = diddleLTR;
    m_rttyCWait = m_rttyDWait = 0;
    m_Mark = FALSE;

	m_Viterbi.Init(MFSK_VITERBI_K, MFSK_VITERBI_POLY1, MFSK_VITERBI_POLY2);
	m_MFSK_SHDATA = 0;
	m_MFSK_SHCount = 0;
}
//--------------------------------------------------------
void __fastcall CENCODE::SetMFSKType(int type)
{
	MFSK_SetPara(type, &m_MFSK_TONES, &m_MFSK_SPEED, &m_MFSK_BITS);
    m_MFSK_MASK = m_MFSK_TONES - 1;
    Create();
}
//--------------------------------------------------------
void __fastcall CENCODE::Create(void)
{
	m_Mode = 0;
	m_dCW = m_SampleFreq / 20.0;
	if( IsMFSK(m_Type) ){
	    m_dTW = m_SampleFreq / m_MFSK_SPEED;
    }
    else {
	    m_dTW = m_SampleFreq / m_Speed;
    }
    m_out = 0;
    m_sync = 0;
    m_Idle = FALSE;
    m_fCW = FALSE;
    memset(m_Z, 0, sizeof(m_Z));
}
void __fastcall CENCODE::SetTmg(double d)
{
	if( IsMFSK(m_Type) ){
		m_dTW = m_SampleFreq / m_MFSK_SPEED;
    }
    else {
		m_dTW = m_SampleFreq / m_Speed;
    }
	m_dTW += m_dTW * d / 1e6;
}
void __fastcall CENCODE::ClearBuffer(void)
{
    m_RP = m_WP = m_CC = 0;
}
void __fastcall CENCODE::Reset(BOOL fCW)
{
	m_fCW = fCW;
	m_Code = 0;
    m_Mode = 0;
    m_Idle = FALSE;
    m_Mark = FALSE;
}
//--------------------------------------------------------
void __fastcall CENCODE::SetBufferCount(int cc)
{
	if( cc <= 0 ){
		ClearBuffer();
	}
	else if( cc < m_CC ){
		int n = m_CC - cc;
		m_CC -= n;
		m_WP -= n;
		if( m_WP < 0 ) m_WP += AN(m_Buff);
	}
}
//--------------------------------------------------------
void __fastcall CENCODE::GetSendText(CString &cs)
{
	int rp = m_RP;
	int c, h;
	for( int i = 0; i < m_CC; i++ ){
		c = m_Buff[rp++];
		if( rp >= AN(m_Buff) ) rp = 0;
		h = c & 0x00ff00;
		if( h >= 0x8000 ){
			cs += char(c >> 8);
			cs += char(c);
		}
		else {
			cs += char(c);
		}
	}
}
//--------------------------------------------------------
void __fastcall CENCODE::SetCW(int f)
{
	m_dCW = m_SampleFreq / f;
}
//--------------------------------------------------------
static int __fastcall GetCWCode(int &count, int code)
{
	const USHORT _tbl[]={
		// 0       1       2       3        4       5       6       7
		0x0005, 0x8005, 0xc005, 0xe005, 0xf005, 0xf805, 0x7805, 0x3805, // 0-7
		// 8       9       :        ;       <       =       >       ?
		0x1805, 0x0805, 0xe806, 0xA805, 0x0000, 0x7005, 0x0000, 0xcc06, // 8
		// @        A       B       C       D       E       F       G
		0xb805, 0x8002, 0x7004, 0x5004, 0x6003, 0x8001, 0xd004, 0x2003, // @-G
		// H        I       J       K       L       M       N       O
		0xf004, 0xc002, 0x8004, 0x4003, 0xb004, 0x0002, 0x4002, 0x0003, // H-O
		// P        Q       R       S       T       U       V       W
		0x9004, 0x2004, 0xa003, 0xe003, 0x0001, 0xc003, 0xe004, 0x8003, // P-W
		// X        Y       Z       [	    \	    ]
		0x6004, 0x4004, 0x3004, 0x0000, 0x0000, 0x4805, 0x0000, 0x0000, // X-Z
	};
	if( code == '/' ){
		code = 0x6805;
        count = 5;
	}
	else if( (code >= 0x30) && (code <= 0x7f) ){
		if( code >= 0x60 ) code -= 0x20;
        code -= 0x30;
        code = _tbl[code];
		count = code & 0x000f;
    }
    else {
		code = 0;
        count = 0;
    }
    return code;
}
//--------------------------------------------------------------------------
void __fastcall CENCODE::MFSKSendBit(BOOL bit)
{
	MFSKSendPair(m_Viterbi.Do(bit));
}
//--------------------------------------------------------------------------
void __fastcall CENCODE::MFSKSendPair(BYTE pair)
{
	MFSKSendSymBit(pair & 2);
	MFSKSendSymBit(pair & 1);
}
//--------------------------------------------------------------------------
//
void __fastcall CENCODE::MFSKSendSymBit(BOOL bit)
{
	const BYTE _tGray2Bin[]={	//Encode
		0x00,0x01,0x03,0x02,0x07,0x06,0x04,0x05,
		0x0F,0x0E,0x0C,0x0D,0x08,0x09,0x0B,0x0A,
		0x1F,0x1E,0x1C,0x1D,0x18,0x19,0x1B,0x1A,
		0x10,0x11,0x13,0x12,0x17,0x16,0x14,0x15,
	};

	m_MFSK_SHDATA = m_MFSK_SHDATA << 1;
    if( bit ) m_MFSK_SHDATA |= 1;

	m_MFSK_SHCount++;
	if( m_MFSK_SHCount >= m_MFSK_BITS ){
		m_Fifo.PutData(_tGray2Bin[m_InterLeaver.EncodeBits(m_MFSK_SHDATA) & m_MFSK_MASK]);
		m_MFSK_SHDATA = 0;
        m_MFSK_SHCount = 0;
	}
}
//--------------------------------------------------------------------------
void __fastcall CENCODE::MFSKSendChar(int c)
{
	int code = g_VariCode.EncodeMFSK(BYTE(c));
    int n = (code & 0x0f000) >> 12;
    for( n--; n >= 0; n-- ){
		MFSKSendBit(_tBitData[n] & code);
    }
}
//--------------------------------------------------------------------------
void __fastcall CENCODE::MFSKSendIdle(void)
{
	MFSKSendIdle(m_MFSK_TONES);
}
//--------------------------------------------------------------------------
void __fastcall CENCODE::MFSKSendIdle(int n)
{
	for(int i = 0; i < n; i++){
		MFSKSendBit(0);
	}
}
//--------------------------------------------------------------------------
void __fastcall CENCODE::MFSKSendIdleChar(void)
{
	MFSKSendChar(0);
    MFSKSendIdle();
	m_fChar = FALSE;
}
//--------------------------------------------------------
int __fastcall CENCODE::Do(void)
{
	m_Cnt++;
	m_dNow += 1.0;
	switch(m_Mode){
		case 0:				// �G���R�[�h�̊J�n
			m_fReqRX = FALSE;
			m_fChar = FALSE;
		    if( m_pVCO ) m_pVCO->InitPhase();
			if( IsMFSK(m_Type) ){
				m_Fifo.Clear();
                m_InterLeaver.Init(m_MFSK_BITS);
				m_Viterbi.Reset();
				m_MFSK_SHDATA = 0;
				m_MFSK_SHCount = 0;
            }

			if( m_fCW ){
				m_out = 128;
				m_Mode = IsMFSK(m_Type) ? 130 : 2;
            }
			else if( IsRTTY(m_Type) ){
			    m_Mark = FALSE;
			    m_BAUDOT.Reset();
				m_out = -1;
   	    	    m_Mode = 64;
            }
			else if( IsMFSK(m_Type) ){
				m_out = 0;
                m_Mode = 128;
            }
            else {
				m_out = IsBPSK(m_Type) ? 1 : 0;
     	        m_Mode++;
            }
   	        m_Cnt = 0;
			m_dNext = m_dTW * 2;
       	    m_dNow = 0.0;
   	        m_cBCount = 0;
   	        m_Idle = FALSE;
            break;
        case 1:
            if( m_dNext <= m_dNow ){
				m_out = (m_out >= 0) ? -1 : 1;
				m_dNext += m_dTW;
                m_cBCount++;
                if( m_cBCount >= (0.64 * m_Speed) ){
					m_cBCount = 0;
	                m_Mode++;
                }
                m_Cnt = 0;
            }
            break;
        case 2:
            if( m_dNext <= m_dNow ){
				m_dNext += m_dTW;
				if( !m_cBCount ){
					if( m_CC ){
						m_cData = GetChar();
                    }
                	else if( m_pMainCtrl ){
						m_cData = m_pMainCtrl->OnGetChar();
						if( (m_cData < 0) && m_CC ) m_cData = GetChar();
                    }
                    else {
						m_cData = GetChar();
                    }
					if( (m_cData >= 0x100) && (m_cData < 0x0200) ){
                        m_Idle = FALSE;
						m_cData -= 0x0100;
                        m_cData = GetCWCode(m_cBCount, m_cData);
						m_dNext -= m_dTW;
                        m_dNext += m_fCW ? m_dCW : m_dCW*3;
						m_out = 128;
                        m_Mode = 16;
                        m_fCW = TRUE;
                        break;
                    }
                    else if( m_cData == 0x200 ){	// Idle
						m_fCW = FALSE;
                        m_Idle = FALSE;
						m_cData = 0;
                        m_cBCount = 20;
                    }
                    else if( m_cData >= 0 ){
						m_fCW = FALSE;
                        m_Idle = FALSE;
						if( m_fTWO && (m_cData >= 0x8140) && (m_cData <= 0xfeff) ){
							int c1 = (m_cData >> 8) & 0x00ff;
                            int c2 = m_cData & 0x00ff;
							c1 = g_VariCode.Mbcs2Index(c1, FALSE);
							c2 = g_VariCode.Mbcs2Index(c2, FALSE);
                            int b1, b2;
							int d1 = g_VariCode.Encode(b1, c1) << 2;
							int d2 = g_VariCode.Encode(b2, c2) << 2;
							m_cData = (d1 << (b2+2)) | d2;
                            m_cBCount = b1 + b2 + 4;
                        }
                        else {
							m_cData = g_VariCode.Mbcs2Index(m_cData, m_fTWO ? FALSE : m_fJA);
	                        if( m_cData >= 0 ){
								m_cData = g_VariCode.Encode(m_cBCount, m_cData) << 2;
								m_cBCount += 2;
            	            }
                	        else {
								m_cData = 0;
    	                	    m_cBCount = 1;
                        	}
                        }
                    }
					else if( m_fCW ){
						m_out = 128;
                        m_Idle = TRUE;
                        break;
                    }
                    else {
						m_Idle = TRUE;
						m_cData = 0;
                        m_cBCount = 1;
                    }
                }
                m_cBCount--;
				if( !(m_cData & _tBitData[m_cBCount]) ){
					m_out = (m_out > 0) ? -1 : 1;
                }
                m_Cnt = 0;
            }
            break;
		// CW output
        case 16:
            if( m_dNext <= m_dNow ){
				if( m_cBCount ){
	                m_dNext += m_cData & 0x8000 ? m_dCW : m_dCW*3;
    	            m_out = 129;
        	        m_Mode++;
                }
                else {
					m_out = 128;
                    m_dNext += m_dCW*6;
                    m_Mode = IsMFSK(m_Type) ? 130 : 2;
                }
			}
            break;
        case 17:			// �X�y�[�X
        	if( m_dNext <= m_dNow ){
				m_dNext += m_dCW;
                m_out = 128;
                m_cData = m_cData << 1;
                m_cBCount--;
				if( !m_cBCount ){
					m_dNext += m_dCW;
					m_Mode = IsMFSK(m_Type) ? 130 : 2;
                }
                else {
					m_Mode--;
                }
            }
            break;
        case 64:						// BAUDOT
            if( m_dNext <= m_dNow ){
				m_dNext += m_dTW;
                m_cBCount++;
                if( m_cBCount >= (0.25 * m_Speed) ){
					m_cBCount = 0;
	                m_Mode++;
                }
                m_Cnt = 0;
            }
            break;
        case 65:						// BAUDOT
            if( m_dNext <= m_dNow ){
				m_dNext += m_dTW*0.5;
				if( !m_cBCount ){
					if( m_CC ){
						m_cData = GetChar();
                    }
                	else if( m_pMainCtrl ){
						m_cData = m_pMainCtrl->OnGetChar();
						if( (m_cData < 0) && m_CC ) m_cData = GetChar();
                    }
                    else {
						m_cData = GetChar();
                    }
					if( (m_cData >= 0x100) && (m_cData < 0x0200) ){
                        m_Idle = FALSE;
						m_cData -= 0x0100;
                        m_cData = GetCWCode(m_cBCount, m_cData);
						m_dNext -= m_dTW*0.5;
                        m_dNext += m_fCW ? m_dCW : m_dCW*3;
						m_out = 128;
                        m_Mode = 16;
                        m_fCW = TRUE;
                        break;
                    }
                    else if( m_cData == 0x200 ){	// Idle
						m_BAUDOT.Reset();
						m_fCW = FALSE;
						m_Idle = FALSE;
                        if( m_fChar ){
	                        m_cData = (m_rttyDiddle == diddleLTR) ? 0x7ffc : 0x7000;
                        }
                        else {
	                        m_cData = 0xffff;
                        }
                        m_cBCount = 15;
                    }
                    else if( m_cData >= 0 ){
						m_fChar = TRUE;
						m_fCW = FALSE;
                        m_Idle = FALSE;
						m_cData = m_BAUDOT.GetCode(m_cBCount, m_cData);
                        if( m_cData < 0 ){
	                        m_cData = 0x7000;	// BLK
                            m_cBCount = 15;
                        }
                    }
					else if( m_fCW ){
						m_BAUDOT.Reset();
						m_out = 128;
                        m_Idle = TRUE;
                        break;
                    }
					else if( m_Mark || m_fReqRX ){
						m_fChar = FALSE;
						m_out = -1;
                        m_Idle = TRUE;
                        break;
                    }
                    else {						// Diddle
						m_BAUDOT.Reset();
						m_Idle = TRUE;
                        m_cData = (m_rttyDiddle == diddleLTR) ? 0x7ffc : 0x7000;
                        m_cBCount = 15;
                    }
                }
                m_cBCount--;
				if( !(m_cData & 0x0001) ){
					m_out = 1;
                }
                else {
					m_out = -1;
                }
                m_cData = m_cData >> 1;
				m_Cnt = 0;
				if( !m_cBCount || (m_cBCount == 15) ){
					if( m_Idle ){
						if( m_rttyDWait ) m_Mode++;
                    }
                    else {
						if( m_rttyCWait ) m_Mode++;
                    }
                }
            }
            break;
        case 66:						// BAUDOT-wait
            if( m_dNext <= m_dNow ){
				m_dNext += m_dTW * 0.1 * (m_Idle ? m_rttyDWait : m_rttyCWait);
                m_Mode = 65;
			}
            m_Cnt = 0;
			break;

		// MFSK
		case 128:
			m_cBCount = (m_MFSK_TONES == 8) ? 32 : 16;
            m_dNext = m_dNow + m_dTW;
            m_Mode++;
        case 129:
            if( m_dNext <= m_dNow ){
				m_dNext += m_dTW;
            	m_cBCount--;
                if( !m_cBCount ){
					if( m_MFSK_TONES == 8 ){	// �Q�r�^�r�̃��g���b�N�X�������̂���
						MFSKSendChar(0);
						MFSKSendChar(0);
                    }
					m_Mode++;
                }
            }
        	break;
        case 130:
        case 131:
            if( m_dNext <= m_dNow ){
				m_dNext += m_dTW;
                if( !m_Fifo.GetCount() ){
					if( m_CC ){
						m_cData = GetChar();
                    }
                	else if( m_pMainCtrl ){
						m_cData = m_pMainCtrl->OnGetChar();
						if( (m_cData < 0) && m_CC ) m_cData = GetChar();
                    }
                    else {
						m_cData = GetChar();
                    }
					if( (m_cData >= 0x100) && (m_cData < 0x0200) ){
                        m_Idle = FALSE;
						m_cData -= 0x0100;
                        m_cData = GetCWCode(m_cBCount, m_cData);
						if( m_fCW ){
							m_dNext -= m_dTW;
	                        m_dNext += m_fCW ? m_dCW : m_dCW*3;
							m_out = 128;
        	                m_Mode = 16;
            	            m_fCW = TRUE;
                        }
                        else {
	                        MFSKSendIdleChar();
    	                    MFSKSendIdleChar();
    	                    MFSKSendIdleChar();
							m_Mode = 132;
							if( m_Fifo.GetCount() ) m_out = m_Fifo.GetData();
                        }
                        break;
                    }
                    else if( m_cData == 0x200 ){	// Idle
                        m_Mode = 130;
						m_fCW = FALSE;
                        m_Idle = FALSE;
//                        MFSKSendIdleChar();
						if( m_fChar ){
	                        MFSKSendIdleChar();
                        }
                        else {
	                        MFSKSendIdle();
                        }
                        MFSKSendIdle();
                    }
                    else if( m_cData >= 0 ){
						m_fChar = TRUE;
                        m_Mode = 130;
						m_fCW = FALSE;
                        m_Idle = FALSE;
						if( m_fTWO && (m_cData >= 0x8140) && (m_cData <= 0xfeff) ){
                            MFSKSendChar((m_cData >> 8) & 0x00ff);
                            MFSKSendChar(m_cData & 0x00ff);
                        }
                        else {
                            MFSKSendChar(m_cData & 0x00ff);
                        }
                    }
					else if( m_fCW ){
						m_out = 128;
                        m_Idle = TRUE;
                        break;
                    }
					else if( m_Mode == 130 ){
						m_Idle = FALSE;
						MFSKSendIdleChar();
                        m_cBCount = 3;
                        m_Mode++;
                    }
					else if( m_Idle ){
						m_cBCount--;
                        if( !m_cBCount ){
                            m_cBCount = (rand() % 4) + 8;
							switch(m_MFSK_TONES){
								case 8:
									m_cBCount *= 2;
                                	break;
                                case 32:
									m_cBCount -= 4;
                                	break;
                            }
	                      	MFSKSendIdleChar();
                        }
                        else {
							MFSKSendIdle();
                        }
                    }
                    else {
						if( m_fReqRX ){
//                        	MFSKSendIdleChar();
							MFSKSendIdle(m_MFSK_TONES+12);
                        }
                        else {
							MFSKSendIdle();
                        }
						if( m_cBCount ){
							m_cBCount--;
                            if( !m_cBCount ){
								m_Idle = TRUE;
                                m_cBCount = (rand() % 4) + 4;
								switch(m_MFSK_TONES){
									case 8:
                                    	m_cBCount *= 2;
                                    	break;
                                    case 32:
                                    	m_cBCount /= 2;
                                    	break;
                                }
                            }
                        }
                    }
                }
				if( m_Fifo.GetCount() ){
					m_out = m_Fifo.GetData();
                }
                else {
//					goto _try;
                }
            }
            break;
        case 132:
            if( m_dNext <= m_dNow ){
				m_dNext += m_dTW;
				if( m_Fifo.GetCount() ){
					m_out = m_Fifo.GetData();
                }
                else {
                    m_dNext += m_fCW ? m_dCW : m_dCW*3;
					m_out = 128;
                    m_Mode = 16;
                    m_fCW = TRUE;
                }
            }
        	break;
    }
	if( m_dNow >= 8192.0 ){
		m_dNow -= 8192.0;
        m_dNext -= 8192.0;
    }
    return m_out;
}

//--------------------------------------------------------
BOOL __fastcall CENCODE::IsBuffFull(void)
{
	return m_CC >= AN(m_Buff);
}

//--------------------------------------------------------
void __fastcall CENCODE::PutChar(int c)
{
	if( !IsBuffFull() ){
		m_Buff[m_WP] = c;
    	m_WP++;
    	if( m_WP >= AN(m_Buff) ){
			m_WP = 0;
    	}
	    m_CC++;
    }
}

//--------------------------------------------------------
int __fastcall CENCODE::GetChar(void)
{
	if( !m_CC ) return -1;
	int c = m_Buff[m_RP];
    m_RP++;
    if( m_RP >= AN(m_Buff) ){
		m_RP = 0;
    }
    m_CC--;
    return c;
}

//--------------------------------------------------------
CCOLLECT::CCOLLECT()
{
	m_Max = 0;
    m_Cnt = 0;
    m_pZ = NULL;
}

CCOLLECT::~CCOLLECT()
{
	Delete();
}

void __fastcall CCOLLECT::Delete(void)
{
	m_Max = 0;
	if( m_pZ ){
		delete m_pZ;
		m_pZ = NULL;
	}
}

void __fastcall CCOLLECT::Create(int max)
{
	if( m_Max == max ) return;

    m_Max = max;
    m_Cnt = 0;
	if( m_pZ ) delete m_pZ;
    m_pZ = new long[m_Max];
    memset(m_pZ, 0, sizeof(long)*m_Max);
}

void __fastcall CCOLLECT::Do(long d)
{
	if( m_Cnt >= m_Max ) return;

    m_pZ[m_Cnt] = d;
    m_Cnt++;
}


//---------------------------------------------------------------------------
const BYTE _tMbcs2Index[]={			// PSK31 index converter
	0x58,0x62,0x66,0x76,0x65,0x71,0x67,0x6B,	/*00*/
	0x6C,0x30,0x0A,0x74,0x63,0x0B,0x75,0x7A,	/*08*/
	0x69,0x68,0x7B,0x7C,0x6F,0x72,0x73,0x6E,	/*10*/
	0x77,0x78,0x7E,0x6D,0x70,0x7F,0x6A,0x79,	/*18*/
	0x00,0x57,0x3A,0x53,0x4D,0x60,0x5D,0x42,	/*20*/
	0x33,0x32,0x3D,0x4F,0x1C,0x0F,0x15,0x45,	/*28*/
	0x25,0x27,0x2F,0x35,0x3F,0x38,0x3B,0x44,	/*30*/
	0x43,0x47,0x31,0x49,0x51,0x14,0x4C,0x5A,	/*38*/
	0x5E,0x1F,0x2E,0x22,0x24,0x1D,0x2B,0x34,	/*40*/
	0x36,0x20,0x56,0x41,0x2A,0x26,0x2C,0x21,	/*48*/
	0x29,0x4E,0x23,0x1B,0x1A,0x37,0x46,0x39,	/*50*/
	0x3E,0x40,0x59,0x54,0x52,0x55,0x5F,0x3C,	/*58*/
	0x64,0x04,0x18,0x0E,0x0D,0x01,0x12,0x16,	/*60*/
	0x0C,0x05,0x50,0x28,0x09,0x11,0x06,0x03,	/*68*/
	0x13,0x4A,0x07,0x08,0x02,0x10,0x1E,0x19,	/*70*/
	0x2D,0x17,0x4B,0x5C,0x48,0x5B,0x61,0x7D,
};

//---------------------------------------------------------------------------
CVARICODE::CVARICODE()
{
    m_tEncode = NULL;
    m_Max = m_TMax = 0;
}

CVARICODE::~CVARICODE()
{
	if( m_tEncode ) delete m_tEncode;
}

void __fastcall CVARICODE::Init(void)
{
    if( !LoadBin("VariCode.tbl") ){
	    Create(256+(192*126));
    }
	for( int i = 0; i < 128; i++ ){
		m_tIndex2Mbcs[_tMbcs2Index[i]] = BYTE(i);
    }
}

void __fastcall CVARICODE::Create(int max)
{
	if( m_tEncode ) delete m_tEncode;
	m_TMax = max;
	DWORD *tEncode = new DWORD[m_TMax];
	m_Max = 0;
    int i;
    int b, db;
	tEncode[m_Max++] = 0x01000001;
    tEncode[m_Max++] = 0x02000003;
    int bmax = 3;
    int bm = 2;
    int bl = 0;
	while(m_Max < m_TMax){
		for( b = bl; b < bm; b++ ){
            BOOL f = TRUE;
			db = b;
			for( i = 3; i < bmax; i++ ){
				if( !(db & 1) && !(db & 2) ){
                	f = FALSE;
                    break;
                }
                db = db >> 1;
            }
			if( f ){
				db = b << 1;
    	        db |= (1 | (bm<<1));
	            db = db | (bmax << 24);
				tEncode[m_Max++] = db;
                if( m_Max >= m_TMax ) break;
            }
        }
		bl = bl << 1;
        if( !(bl & 2) ) bl |= 1;
        bm = bm << 1;
        bmax++;
        if( bmax >= 24 ) break;
    }
    m_tEncode = tEncode;
    SaveBin("VariCode.tbl");
}

static int __fastcall SwapIndex(int index)
{
    if( index <= 0x00d3 ){								// $80-$D3
		index += 0x21f - 0x0080;	// $80-$D3 to Hiragana
    }
	else if( index <= 0x12a ){							// $D4-$12A
		index += 0x280 - 0x00d4;	// $D4-$12A to Katakana
    }
    else if( (index >= 0x021f) && (index <= 0x0272) ){	// Hiragana
		index -= 0x21f - 0x0080;	// Hiragana to $80-$D3
    }
    else if( (index >= 0x0280) && (index <= 0x02d6) ){	// Katakana
		index -= 0x280 - 0x00d4;	// Katakana to $D4-$12A
    }
   	else if( (index >= 0x1840) && (index <= 0x2f7f) ){
		index += 0x4840 - 0x1840;
    }
	else if( (index >= 0x4840) && (index <= 0x5f7f) ){
		index -= 0x4840 - 0x1840;
    }

    return index;
}

UINT __fastcall CVARICODE::Index2Mbcs(int index, BOOL fJA)
{
    if( fJA && (index >= 0x4840) ){	// �Â��o�[�W�����Ƃ̌݊����p
		index -= 0x4840 - 0x1840;
    }
    if( index <= 0x007f ){
		index = m_tIndex2Mbcs[index];
    }
	else if( index >= m_Max ){
		return 0;
    }
    else if( fJA ){
		index = SwapIndex(index);
    }

	UINT mbcs;
    int m, b;
	if( index >= 0x0100 ){
		index -= 0x0100;
		m = index % 192;
		b = index / 192;
		mbcs = 0x8140 + m + b * 256;
	}
    else {
		mbcs = index;
    }
    return mbcs;
}

int __fastcall CVARICODE::Mbcs2Index(UINT mbcs, BOOL fJA)
{
	int index, m, b;
	if( mbcs >= 0x8100 ){
		if( (mbcs & 0x00ff) >= 0x0040 ){
			mbcs -= 0x8100;
			m = mbcs % 256;
			b = mbcs / 256;
			index = 0x100 - 0x40 + m + b * 192;
		}
		else {
			return -1;
		}
	}
    else {
		index = mbcs;
    }
	if( index >= m_Max ){
		index = 0;
    }
    else if( index <= 0x007f ){
		index = _tMbcs2Index[index];
    }
    else if( fJA ){
		index = SwapIndex(index);
    }
    return index;
}

BOOL __fastcall CVARICODE::SaveTable(LPCSTR pName)
{
	BOOL r = FALSE;
	CWaitCursor w;
	FILE *fp;
    if( (fp = fopen(pName, "wt")) != NULL ){
		fprintf(fp, "Index ANSI  JA    HL/BV/BY Bits\tVaricode\n");
    	int i, j;
		for( i = 0; i < m_Max; i++ ){
        	DWORD b = m_tEncode[i];
            int n = b >> 24;
            b &= 0x00ffffff;
			int m = Index2Mbcs(i, FALSE);
            j = Index2Mbcs(i, TRUE);
            if( i >= 0x2f80 ){
				fprintf(fp, "%04X              %04X     %-4u\t", i, m, n);
            }
			else if( i < 0x100 ){
				fprintf(fp, "%04X  %04X  %04X  %04X     %-4u\t", i, m, j, m, n);
            }
            else {
				fprintf(fp, "%04X        %04X  %04X     %-4u\t", i, j, m, n);
            }
            DWORD bm = _tBitData[n-1];
			for( j = 0; j < n; j++, bm = bm >> 1){
				fprintf(fp, "%c", (b & bm) ? '1' : '0');
            }
            fprintf(fp, "\n");
            if( !(i & 0xff) ) OnWave();
        }
		r = !fclose(fp);
    }
	return r;
}

void __fastcall CVARICODE::SaveSource(LPCSTR pName)
{
	FILE *fp;
    if( (fp = fopen(pName, "wt")) != NULL ){
		fprintf(fp, "const DWORD g_tVariCode[%u]={\n", m_TMax);
    	int i;
		for( i = 0; i < m_Max; i++ ){
        	DWORD b = m_tEncode[i];
			if( !(i % 8) ) fprintf(fp, "\n\t");
			fprintf(fp, "0x%X,", b);
        }
        fprintf(fp, "};\n");
		fclose(fp);
    }
}

void __fastcall CVARICODE::SaveBin(LPCSTR pName)
{
	CWaitCursor w;
	char name[256];
    sprintf(name, "%s%s", sys.m_BgnDir, pName);
	FILE *fp;
    if( (fp = fopen(name, "wb")) != NULL ){
		fwrite(m_tEncode, sizeof(DWORD), m_Max, fp);
        fclose(fp);
    }
}

BOOL __fastcall CVARICODE::LoadBin(LPCSTR pName)
{
	BOOL r = FALSE;
	char name[256];
    sprintf(name, "%s%s", sys.m_BgnDir, pName);
	FILE *fp;
    if( (fp = fopen(name, "rb")) != NULL ){
    	int len = 256 + (126*192);
		if( m_tEncode ) delete m_tEncode;
		m_tEncode = new DWORD[len];
		fread(m_tEncode, sizeof(DWORD), len, fp);
        fclose(fp);
        m_TMax = m_Max = len;
        r = TRUE;
    }
    return r;
}

DWORD __fastcall CVARICODE::Encode(int &n, DWORD d)
{
	if( d < DWORD(m_Max) ){
		d = m_tEncode[d];
		n = d >> 24;
    }
    else {
		d = 0;
        n = 1;
    }
    return d;
}

int __fastcall CVARICODE::Decode(DWORD d)
{
	// VARICODE��񕪒T���T�[�`�ŃC���f�b�N�X�ɕϊ�
	int l, h, m;
    l = 0;
    h = m_Max - 1;
    while(l <= h){
		m = (l + h)/2;
		DWORD b = m_tEncode[m] & 0x00ffffff;
        if( d < b ){
			h = m - 1;
        }
        else if( d > b ){
			l = m + 1;
        }
        else {
			return m;
        }
    }
	return -1;
}
//---------------------------------------------------------------------------
const WORD g_tMFSKVariCode[]={			// MFSK VARICODE �C���f�b�N�X��
	0x3004,0x4008,0x400C,0x5010,0x5014,0x5018,0x501C,0x6020,0x6028,0x602C,0x6030,0x6034,0x6038,0x603C,0x7040,0x7050,	//00
	0x7054,0x7058,0x705C,0x7060,0x7068,0x706C,0x7070,0x7074,0x7078,0x707C,0x8080,0x80A0,0x80A8,0x80AC,0x80B0,0x80B4,	//10
	0x80B8,0x80BC,0x80C0,0x80D0,0x80D4,0x80D8,0x80DC,0x80E0,0x80E8,0x80EC,0x80F0,0x80F4,0x80F8,0x80FC,0x9100,0x9140,	//20
	0x9150,0x9154,0x9158,0x915C,0x9160,0x9168,0x916C,0x9170,0x9174,0x9178,0x917C,0x9180,0x91A0,0x91A8,0x91AC,0x91B0,	//30
	0x91B4,0x91B8,0x91BC,0x91C0,0x91D0,0x91D4,0x91D8,0x91DC,0x91E0,0x91E8,0x91EC,0x91F0,0x91F4,0x91F8,0x91FC,0xA200,	//40
	0xA280,0xA2A0,0xA2A8,0xA2AC,0xA2B0,0xA2B4,0xA2B8,0xA2BC,0xA2C0,0xA2D0,0xA2D4,0xA2D8,0xA2DC,0xA2E0,0xA2E8,0xA2EC,	//50
	0xA2F0,0xA2F4,0xA2F8,0xA2FC,0xA300,0xA340,0xA350,0xA354,0xA358,0xA35C,0xA360,0xA368,0xA36C,0xA370,0xA374,0xA378,	//60
	0xA37C,0xA380,0xA3A0,0xA3A8,0xA3AC,0xA3B0,0xA3B4,0xA3B8,0xA3BC,0xA3C0,0xA3D0,0xA3D4,0xA3D8,0xA3DC,0xA3E0,0xA3E8,	//70
	0xA3EC,0xA3F0,0xA3F4,0xA3F8,0xA3FC,0xB400,0xB500,0xB540,0xB550,0xB554,0xB558,0xB55C,0xB560,0xB568,0xB56C,0xB570,	//80
	0xB574,0xB578,0xB57C,0xB580,0xB5A0,0xB5A8,0xB5AC,0xB5B0,0xB5B4,0xB5B8,0xB5BC,0xB5C0,0xB5D0,0xB5D4,0xB5D8,0xB5DC,	//90
	0xB5E0,0xB5E8,0xB5EC,0xB5F0,0xB5F4,0xB5F8,0xB5FC,0xB600,0xB680,0xB6A0,0xB6A8,0xB6AC,0xB6B0,0xB6B4,0xB6B8,0xB6BC,	//A0
	0xB6C0,0xB6D0,0xB6D4,0xB6D8,0xB6DC,0xB6E0,0xB6E8,0xB6EC,0xB6F0,0xB6F4,0xB6F8,0xB6FC,0xB700,0xB740,0xB750,0xB754,	//B0
	0xB758,0xB75C,0xB760,0xB768,0xB76C,0xB770,0xB774,0xB778,0xB77C,0xB780,0xB7A0,0xB7A8,0xB7AC,0xB7B0,0xB7B4,0xB7B8,	//C0
	0xB7BC,0xB7C0,0xB7D0,0xB7D4,0xB7D8,0xB7DC,0xB7E0,0xB7E8,0xB7EC,0xB7F0,0xB7F4,0xB7F8,0xB7FC,0xC800,0xCA00,0xCA80,	//D0
	0xCAA0,0xCAA8,0xCAAC,0xCAB0,0xCAB4,0xCAB8,0xCABC,0xCAC0,0xCAD0,0xCAD4,0xCAD8,0xCADC,0xCAE0,0xCAE8,0xCAEC,0xCAF0,	//E0
	0xCAF4,0xCAF8,0xCAFC,0xCB00,0xCB40,0xCB50,0xCB54,0xCB58,0xCB5C,0xCB60,0xCB68,0xCB6C,0xCB70,0xCB74,0xCB78,0xCB7C,
};
const BYTE g_tMFSKIndex2Ascii[]={		// MFSK Index -> Ascii
	0x20,0x65,0x74,0x6F,0x61,0x69,0x6E,0x72,0x73,0x6C,0x68,0x64,0x63,0x75,0x6D,0x66,	//00
	0x70,0x67,0x79,0x62,0x77,0x76,0x6B,0x78,0x71,0x7A,0x6A,0x2C,0x08,0x0D,0x54,0x53,	//10
	0x45,0x41,0x49,0x4F,0x43,0x52,0x44,0x30,0x4D,0x50,0x31,0x4C,0x46,0x4E,0x42,0x32,	//20
	0x47,0x33,0x48,0x55,0x35,0x57,0x36,0x58,0x34,0x59,0x4B,0x38,0x37,0x56,0x39,0x51,	//30
	0x4A,0x5A,0x27,0x21,0x3F,0x2E,0x2D,0x3D,0x2B,0x2F,0x3A,0x29,0x28,0x3B,0x22,0x26,	//40
	0x40,0x25,0x24,0x60,0x5F,0x2A,0x7C,0x3E,0x3C,0x5C,0x5E,0x23,0x7B,0x7D,0x5B,0x5D,	//50
	0x7E,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,	//60
	0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,	//70
	0xBF,0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,	//80
	0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,	//90
	0xDF,0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,	//A0
	0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,	//B0
	0xFF,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x09,0x0A,0x0B,0x0C,0x0E,0x0F,0x10,	//C0
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x7F,	//D0
	0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,	//E0
	0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
};
const BYTE g_tMFSKAscii2Index[]={		// MFSK Ascii -> Index
	0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0x1C,0xC9,0xCA,0xCB,0xCC,0x1D,0xCD,0xCE,	//00
	0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,	//10
	0x00,0x43,0x4E,0x5B,0x52,0x51,0x4F,0x42,0x4C,0x4B,0x55,0x48,0x1B,0x46,0x45,0x49,	//20
	0x27,0x2A,0x2F,0x31,0x38,0x34,0x36,0x3C,0x3B,0x3E,0x4A,0x4D,0x58,0x47,0x57,0x44,	//30
	0x50,0x21,0x2E,0x24,0x26,0x20,0x2C,0x30,0x32,0x22,0x40,0x3A,0x2B,0x28,0x2D,0x23,	//40
	0x29,0x3F,0x25,0x1F,0x1E,0x33,0x3D,0x35,0x37,0x39,0x41,0x5E,0x59,0x5F,0x5A,0x54,	//50
	0x53,0x04,0x13,0x0C,0x0B,0x01,0x0F,0x11,0x0A,0x05,0x1A,0x16,0x09,0x0E,0x06,0x03,	//60
	0x10,0x18,0x07,0x08,0x02,0x0D,0x15,0x14,0x17,0x12,0x19,0x5C,0x56,0x5D,0x60,0xDF,	//70
	0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,	//80
	0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,	//90
	0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,	//A0
	0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,0x80,	//B0
	0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,	//C0
	0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,0xA0,	//D0
	0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,	//E0
	0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,0xC0,
};
//---------------------------------------------------------------------------
int __fastcall CVARICODE::DecodeMFSK(DWORD d)
{
	// VARICODE��񕪒T���T�[�`�ŃC���f�b�N�X�ɕϊ�
	int l, h, m;
    l = 0;
    h = 256 - 1;
    while(l <= h){
		m = (l + h)/2;
		DWORD b = g_tMFSKVariCode[m] & 0x00fff;
        if( d < b ){
			h = m - 1;
        }
        else if( d > b ){
			l = m + 1;
        }
        else {
			return g_tMFSKIndex2Ascii[m];
        }
    }
	return -1;
}

//---------------------------------------------------------------------------
int __fastcall CVARICODE::EncodeMFSK(BYTE c)
{
	return g_tMFSKVariCode[g_tMFSKAscii2Index[c]];
}

//---------------------------------------------------------------------------
BCODETBL _TTY[]={			// S-BELL
		// 0x20 - 0x7f
		{0x04, 2}, {0x16, 1}, {0x11, 1}, {0x00, 2}, //  !"#
		{0x12, 1}, {0x00, 2}, {0x0b, 1}, {0x1a, 1}, // $%&'
		{0x1e, 1}, {0x09, 1}, {0x00, 2}, {0x00, 2}, // ()*+
		{0x06, 1}, {0x18, 1}, {0x07, 1}, {0x17, 1}, // ,-./
		{0x0d, 1}, {0x1d, 1}, {0x19, 1}, {0x10, 1},	// 0123	30-33
		{0x0a, 1}, {0x01, 1}, {0x15, 1}, {0x1c, 1},	// 4567	34-37
		{0x0c, 1}, {0x03, 1}, {0x0e, 1}, {0x0f, 1},	// 89	38-3b
		{0x00, 2}, {0x00, 2}, {0x00, 2}, {0x13, 1},	// <=>?	3c-3f
		{0x00, 2}, {0x18, 0}, {0x13, 0}, {0x0e, 0},	// @ABC	40-43
		{0x12, 0}, {0x10, 0}, {0x16, 0}, {0x0b, 0},	// DEFG
		{0x05, 0}, {0x0c, 0}, {0x1a, 0}, {0x1e, 0},	// HIJK
		{0x09, 0}, {0x07, 0}, {0x06, 0}, {0x03, 0},	// LMNO
		{0x0d, 0}, {0x1d, 0}, {0x0a, 0}, {0x14, 0},	// PQRS
		{0x01, 0}, {0x1c, 0}, {0x0f, 0}, {0x19, 0},	// TUVW
		{0x17, 0}, {0x15, 0}, {0x11, 0}, {0x00, 2},	// XYZ[ 58-5b
		{0x00, 2}, {0x00, 2}, {0x00, 2}, {0x00, 2}, // \]^_
		{0x00, 2}, {0x18, 0}, {0x13, 0}, {0x0e, 0},	// @ABC	60-63
		{0x12, 0}, {0x10, 0}, {0x16, 0}, {0x0b, 0},	// DEFG
		{0x05, 0}, {0x0c, 0}, {0x1a, 0}, {0x1e, 0},	// HIJK
		{0x09, 0}, {0x07, 0}, {0x06, 0}, {0x03, 0},	// LMNO
		{0x0d, 0}, {0x1d, 0}, {0x0a, 0}, {0x14, 0},	// PQRS
		{0x01, 0}, {0x1c, 0}, {0x0f, 0}, {0x19, 0},	// TUVW
		{0x17, 0}, {0x15, 0}, {0x11, 0}, {0x00, 2},	// XYZ[ 78-7b
		{0x00, 2}, {0x00, 2}, {0x00, 2}, {0x00, 2},	// |{~
};

//---------------------------------------------------------------------------
CBAUDOT::CBAUDOT()
{
	m_pMainCtrl = NULL;
	Reset();
}
//---------------------------------------------------------------------------
CBAUDOT::~CBAUDOT()
{
}
//---------------------------------------------------------------------------
int __fastcall CBAUDOT::GetOneCode(int &fig, int code)
{
	int r;
	switch(code){
		case '\n':
			r = 0x08;
            fig = 2;
			break;
		case '\r':
			r = 0x02;
            fig = 2;
			break;
        default:
			if( (code >= 0x20) && (code <= 0x7f) ){
				code -= 0x20;
				r = _TTY[code].Code;
                fig = _TTY[code].Fig;
            }
            else {
				r = -1;
            }
        	break;
    }
    return r;
}
//---------------------------------------------------------------------------
// 15bits�f�[�^���쐬����
int __fastcall CBAUDOT::DblCode(int code)
{
	int d = 7;		// Stop bits
	int i;
    for( i = 0; i < 5; i++, code = code >> 1 ){
		d = d << 1;
		if( code & 0x0001 ) d |= 1;
        d = d << 1;
        if( code & 0x0001 ) d |= 1;
    }
    d = d << 2;		// Start bits
    return d;
}
//---------------------------------------------------------------------------
int __fastcall CBAUDOT::GetCode(int &count, int code)
{
	int fig;
	int r = GetOneCode(fig, code);
    if( r < 0 ) return r;

	int ext_r = r;
	int ext_f = 0;

    r = DblCode(r);
    if( (m_OutFig < 0) || ((fig != 2)&&(m_OutFig != fig)) || ((m_CodeB4 == ' ')&&(fig==1)) ){
        switch(fig){
            case 1:		// FIG
				m_OutFig = 1;
                r = r << 15;
				r |= DblCode(0x1b);
				ext_f = 0x1b;
            	break;
            default:	// LTR
				m_OutFig = 0;
                r = r << 15;
				r |= DblCode(0x1f);
				ext_f = 0x1f;
            	break;
        }
        count = 30;
    }
    else {
		count = 15;
    }
	m_CodeB4 = code;

	if( ext_f ) m_pMainCtrl->ExtFskIt(ext_f);
	m_pMainCtrl->ExtFskIt(ext_r);

    return r;
}
//---------------------------------------------------------------------------
const char _LTR[32]={
	0x00, 'T', 0x0d, 'O', ' ', 'H', 'N', 'M',
	0x0a, 'L', 'R', 'G', 'I', 'P', 'C', 'V',
	'E', 'Z', 'D', 'B', 'S', 'Y', 'F', 'X',
	'A', 'W', 'J', 0x00, 'U', 'Q', 'K', 0x00,
};
const char _FIG[32]={
	0x00, '5', 0x0d, '9', ' ', 'h', ',', '.',
	0x0a, ')', '4', '&', '8', '0', ':', ';',
	'3', '"', '$', '?', 's', '6', '!', '/',
	'-', '2', 0x27, 0x00, '7', '1', '(', 0x00,
};

CRTTY::CRTTY()
{
	m_outfig = 3;

	m_fig = 0;
	m_uos = TRUE;

	m_txuos = 1;
    m_CodeSet = 0;		// 0:S-BELL, 1:J-BELL
	SetCodeSet();
}

void CRTTY::SetCodeSet(void)
{
	memcpy(m_TBL, _TTY, sizeof(m_TBL));
	if( m_CodeSet ){		// J-BELL
		m_TBL[7].Code = 0x14;
	}
}

// 11011 FIG
// 11111 LTR
char CRTTY::ConvAscii(int d)
{
	char c = 0;
	d &= 0x1f;
	if( d == 0x1b ){		// FIG
		m_fig = 1;
	}
	else if( d == 0x1f ){	// LTR
		m_fig = 0;
	}
	else if( m_fig ){
		c = _FIG[d];
		if( m_CodeSet ){
			switch(c){
				case 's':
					c = 0x27;
					break;
				case 0x27:
					c = 'j';
					break;
			}
		}
		if( m_uos ){
			switch(c){
				case ' ':
//                case 0x0d:
//                case 0x0a:
					m_fig = 0;
					break;
				default:
					break;
			}
		}
	}
	else {
		c = _LTR[d];
	}
	return c;
}

int CRTTY::ConvRTTY(char d)
{
	int fig = 2;
	int r = 0;
	switch(d){
		case '_':
			r = 0x00ff;
			break;
		case '~':
			r = 0x00fe;
			break;
		case '[':
			r = 0x00fd;
			break;
		case ']':
			r = 0x00fc;
			break;
		case 0x0a:
			r = 0x08;
			break;
		case 0x0d:
			r = 0x02;
			break;
		case 0x1b:
			r = 0x1b;
			fig = 1;
			break;
		case 0x1f:
			r = 0x1f;
			fig = 0;
			break;
		default:
			if( d >= 0x20 ){
				d -= char(0x20);
				r = m_TBL[d].Code;
				fig = m_TBL[d].Fig;
			}
			break;
	}
	if( fig != 2 ){
		if( fig != m_outfig ){
			r |= (fig ? 0x1b00 : 0x1f00);
			m_outfig = fig;
		}
	}
	else if( r == 0x04 ){	// �X�y�[�X�̎�
		if( m_outfig == 1 ) m_outfig = 2;
	}
	return r;
}

int CRTTY::ConvRTTY(BYTE *t, LPCSTR p)
{
	int n;
	int d;
	for( n = 0;*p; p++ ){
		d = ConvRTTY(*p);
		if( d & 0x0000ff00 ){
			*t++ = BYTE(d >> 8);
			n++;
		}
		*t++ = BYTE(d);
		n++;
	}
	return n;
}

int CRTTY::GetShift(char d)
{
	int fig = 2;
	if( d >= 0x20 ){
		d -= char(0x20);
		fig = m_TBL[d].Fig;
		if( !m_TBL[d].Code ){
			fig = 2;
		}
	}
	return fig;
}

char CRTTY::InvShift(char c)
{
	int fs = GetShift(c);
	if( (c == 'h') || (c == 's') || (c == 'j') ) fs = 1;
	int d = ConvRTTY(c) & 0x001f;
	switch(fs){
		case 0:
			if( _FIG[d] ){
				c = _FIG[d];
				if( m_CodeSet ){
					switch(c){
						case 's':
							c = 0x27;
							break;
						case 0x27:
							c = 'j';
							break;
					}
				}
			}
			return c;
		case 1:
			return _LTR[d] ? _LTR[d] : c;
		default:
			return c;
	}
}

//---------------------------------------------------------------------------
CClock::CClock()
{
	m_pData = NULL;
	m_SampleFreq = SAMPFREQ;
    m_ToneFreq = 1000.0;
}

//---------------------------------------------------------------------------
CClock::~CClock()
{
	Delete();
}

//---------------------------------------------------------------------------
void __fastcall CClock::Delete(void)
{
	if( m_pData ){
		delete m_pData;
		m_pData = NULL;
	}
}
//---------------------------------------------------------------------------
void __fastcall CClock::Create(int max)
{
	m_Width = max;
    if( m_pData ) delete m_pData;
    m_pData = new int[m_Width];
    memset(m_pData, 0, sizeof(int)*m_Width);
    m_dNow = 0;
	m_BPF.m_Freq = m_ToneFreq;
    m_BPF.m_BW = 100.0;
    SetSampleFreq(m_SampleFreq);
    m_LPF.Create(ffLPF, 100, m_SampleFreq, 3, 0, 0);

}

//---------------------------------------------------------------------------
void __fastcall CClock::SetSampleFreq(double f)
{
	m_SampleFreq = f;
	if( !IsActive() ) return;
    m_dAdd = m_Width / m_SampleFreq;
    m_BPF.SetSampleFreq(m_SampleFreq);
}

//---------------------------------------------------------------------------
void __fastcall CClock::SetToneFreq(double f)
{
	m_ToneFreq = f;
	if( !IsActive() ) return;
	m_BPF.m_Freq = m_ToneFreq;
    m_BPF.SetSampleFreq(m_SampleFreq);
}
//---------------------------------------------------------------------------
BOOL __fastcall CClock::Do(short ds)
{
	BOOL f = FALSE;
	double d = m_BPF.Do(ds);
    d = m_LPF.Do(ABS(d));
	int i = int(m_dNow);
    if( i < 0 ) i = 0;
    if( i > m_Width ) i = m_Width;
    m_pData[i] = int(d);
    m_dNow += m_dAdd;
    if( m_dNow >= m_Width ){
		f = TRUE;
    	m_dNow -= m_Width;
    }
    if( m_dNow < 0 ) m_dNow += m_Width;
    return f;
}
//---------------------------------------------------------------------------
CAMPCONT::CAMPCONT()
{
    m_Max = g_SinTable.m_Size / 4;
    m_iMax = g_SinTable.m_Size / 4;
    m_Cnt = m_Max;
    m_S = 0;
    SetMax(16);
}
//---------------------------------------------------------------------------
void __fastcall CAMPCONT::SetMax(int max)
{
    m_ADD = m_Max / double(max);
}
//---------------------------------------------------------------------------
void __fastcall CAMPCONT::Reset(void)
{
	m_Cnt = m_Max;
    m_S = 0;
}
//---------------------------------------------------------------------------
double __fastcall CAMPCONT::Do(int s)
{
	if( s != m_S ){
		m_Cnt = 0.0;
	    m_S = s;
    }
	int r = int(m_Cnt);
    if( r >= m_Max ) return s;
    m_Cnt += m_ADD;
    if( s ){
		return g_SinTable.m_tSin[r];
    }
    else {
		return g_SinTable.m_tSin[r+m_iMax];
    }
}


//---------------------------------------------------------------------------
CBFifo::CBFifo()
{
	m_WP = m_RP = m_CNT = m_MAX = 0;
	m_pBase = NULL;
	m_D = FALSE;
}
//---------------------------------------------------------------------------
CBFifo::~CBFifo()
{
	if( m_pBase ){
		delete m_pBase;
        m_pBase = NULL;
    }
}
//---------------------------------------------------------------------------
void __fastcall CBFifo::Create(int max)
{
	if( m_pBase ) delete m_pBase;
    m_pBase = new BOOL[max];
	m_WP = m_RP = m_CNT = 0;
    m_MAX = max;
}
//---------------------------------------------------------------------------
void __fastcall CBFifo::PutFlag(BOOL f)
{
	if( m_CNT < m_MAX ){
		m_pBase[m_WP] = f;
        m_WP++;
        if( m_WP >= m_MAX ) m_WP = 0;
        m_CNT++;
        if( m_CNT >= m_MAX ) m_CNT = m_MAX;
    }
}
//---------------------------------------------------------------------------
BOOL __fastcall CBFifo::GetFlag(void)
{
	if( m_CNT ){
		m_D = m_pBase[m_RP];
        m_RP++;
        if( m_RP >= m_MAX ) m_RP = 0;
        m_CNT--;
    }
	return m_D;
}
/*=============================================================================
  CPHASE�N���X
=============================================================================*/
CPHASE::CPHASE()
{
	m_MFSK_TONES = 16;
	m_MFSK_SPEED = 15.625;
	m_MFSK_BASEPOINT = int(MFSK_BASEFREQ/15.625);

	m_SampleFreq = 11025.0*0.5;
    m_CarrierFreq = 1750;
    m_MixerFreq = 0;
    SetSampleFreq(m_SampleFreq);
}
//--------------------------------------------------------------------------
void __fastcall CPHASE::SetMFSKType(int type)
{
	MFSK_SetPara(type, &m_MFSK_TONES, &m_MFSK_SPEED, NULL);
    m_MFSK_BASEPOINT = int(MFSK_BASEFREQ/m_MFSK_SPEED);
}
//--------------------------------------------------------------------------
void __fastcall CPHASE::SetSampleFreq(double f)
{
	m_SampleFreq = f;
    Create();
}
//--------------------------------------------------------------------------
void __fastcall CPHASE::Create(void)
{
    m_SymbolLen = m_SampleFreq/m_MFSK_SPEED;
	m_MixerFreq = double(m_MFSK_BASEPOINT) * m_SampleFreq / m_SymbolLen;
	m_VCO.SetSampleFreq(m_SampleFreq);
	m_Hilbert.Create(20, m_SampleFreq, 25.0, m_SampleFreq*0.5 - 25.0);
    m_Hilbert.Clear();
    SetCarrierFreq(m_CarrierFreq);
    m_SlideFFT.Create(int(m_SymbolLen + 0.5), m_MFSK_BASEPOINT, m_MFSK_TONES);
}
//--------------------------------------------------------------------------
void __fastcall CPHASE::SetCarrierFreq(double f)
{
	m_CarrierFreq = f;
    m_VCO.SetFreeFreq(m_CarrierFreq - m_MixerFreq);
}
//--------------------------------------------------------------------------
CLX* __fastcall CPHASE::Do(double d)
{
	m_Hilbert.Do(m_sig, d);					// ���f����

    CLX z;
	z.r = m_VCO.Do();
    z.j = m_VCO.DoCos();
	z *= m_sig;								// ���g���ϊ�

	return m_SlideFFT.Do(z);
}
/*=============================================================================
  CDecMFSK�N���X
=============================================================================*/
CDecMFSK::CDecMFSK(CMMVARICtrl *pMainCtrl)
{
	m_pMainCtrl = pMainCtrl;
	m_MFSK_TONES = 16;
	m_MFSK_SPEED = 15.625;
	m_MFSK_BITS = 4;
	m_MFSK_BW = m_MFSK_SPEED * (m_MFSK_TONES - 1);

	m_InterLeaver.Init(m_MFSK_BITS);
	m_AmpStgPtr = 0;
	m_SHDATA = 0;

	m_Metric1 = m_Metric2 = 0;
	m_ViterbiPair[0] = m_ViterbiPair[1] = 0;
	m_ViterbiPhase = 0;
	m_Viterbi1.Init(MFSK_VITERBI_K, MFSK_VITERBI_POLY1, MFSK_VITERBI_POLY2);
	m_Viterbi2.Init(MFSK_VITERBI_K, MFSK_VITERBI_POLY1, MFSK_VITERBI_POLY2);

	memset(m_SymStg, 0, sizeof(m_SymStg));

	m_AvgMetric = 0;

	m_dNow = 0;

	m_Tmg = m_SyncState = 0;

    m_fLSB = FALSE;
    m_RxData.Clear();

	m_fSQ = FALSE;
	m_fDelaySQ = FALSE;
	m_SQDelayCount = 0;

	m_AvgClock.Create(8);
    m_AvgAFC.Create(8);
    m_PrevZ = 0;
    m_kAFC = 0;
    m_AFCWidth = 0;
    SetSampleFreq(11025*0.5);
}
//--------------------------------------------------------------------------
void __fastcall CDecMFSK::SetMFSKType(int type)
{
	MFSK_SetPara(type, &m_MFSK_TONES, &m_MFSK_SPEED, &m_MFSK_BITS);
	m_MFSK_BW = m_MFSK_SPEED * (m_MFSK_TONES - 1);
	m_InterLeaver.Init(m_MFSK_BITS);
	SetSampleFreq(m_SampleFreq);
}
//--------------------------------------------------------------------------
void __fastcall CDecMFSK::SetCarrierFreq(double f)
{
	if( m_pMainCtrl->m_MFSK_Center ){
		f -= m_MFSK_BW*0.5;
    }
    else {
		if( m_fLSB ) f -= m_MFSK_BW;
    }
	m_Phase.SetCarrierFreq(f);
    m_AvgAFC.Reset();
}
//--------------------------------------------------------------------------
void __fastcall CDecMFSK::Reset(void)
{
	m_SHDATA = 0;

    m_dNow = 0;
	m_dTW = m_dBTW = m_SampleFreq / m_MFSK_SPEED;
    m_dBTWL = m_dBTW * (1.0-(15000.0 * 1.0e-6));
    m_dBTWH = m_dBTW * (1.0+(15000.0 * 1.0e-6));

	m_AvgMetric = 0;
	memset(m_SymStg, 0, sizeof(m_SymStg));

	m_AmpStgPtr = 0;
	memset(m_AmpStg, 0, sizeof(m_AmpStg));

	m_Metric1 = m_Metric2 = 0;
	m_ViterbiPair[0] = m_ViterbiPair[1] = 0;
	m_ViterbiPhase = 0;

    m_Viterbi1.Reset();
    m_Viterbi2.Reset();

    m_RxData.Clear();
	m_InterLeaver.Reset();

	m_SyncState = FALSE;

	m_fSQ = FALSE;
	m_fDelaySQ = FALSE;
	m_SQDelayCount = 0;
    ResetMeas();
}
//--------------------------------------------------------------------------
void __fastcall CDecMFSK::SetSampleFreq(double f)
{
	m_SampleFreq = f;
	m_dTW = m_dBTW = m_SampleFreq / m_MFSK_SPEED;
    m_kAFC = m_MFSK_SPEED / (2.0*PI);
    m_AFCWidth = m_MFSK_SPEED * 0.48;
    m_AmpStgMax = int(m_dBTW * 2);
#if DEBUG
	if( m_AmpStgMax > AMPSTGMAX ) Application->MainForm->Caption = "Over AmpStgSize of MFSK";
#endif
	memset(m_AmpStg, 0, sizeof(m_AmpStg));
    m_Phase.SetMFSKType(MFSK_Tones2Type(m_MFSK_TONES));
    m_Phase.SetSampleFreq(m_SampleFreq);
	Reset();
}
//--------------------------------------------------------------------------
void __fastcall CDecMFSK::ResetMeas(void)
{
    m_AvgClock.Reset();
    m_MeasClock = m_MeasCounter = m_MeasStage = 0;
    m_dTW = m_dBTW;
};
//--------------------------------------------------------------------------
double __fastcall CDecMFSK::GetFactor(void)
{
//    Application->MainForm->Caption = m_ViterbiPhase;
	if( m_ViterbiPhase < 256 ){
		return 0.001;
    }
    else if( m_MFSK_TONES == 8 ){
		return 0.01625;
    }
    else {
		return 0.03125;
    }
}
//--------------------------------------------------------------------------
// - MFSK8,MFSK31�̏ꍇ�͊�t�F�[�Y�Ƌ����t�F�[�Y�����邽�߁A
//   2��Viterbi�̂����ق������
// - MFSK16��2���1��Viterbi2�����s����
void __fastcall CDecMFSK::StreamSym(BYTE sym)
{
	int c, metric;

	m_ViterbiPair[0] = m_ViterbiPair[1]; m_ViterbiPair[1] = sym;
	m_ViterbiPhase++;
	if( m_ViterbiPhase & 1 ){	// MFSK8, MFSK31
		if( m_MFSK_TONES == 16 ) return;
		if( (c = m_Viterbi1.Decode(&metric, m_ViterbiPair)) == -1 ) return;
		DoAvg(m_Metric1, metric, GetFactor());
		if( m_Metric1 < m_Metric2 ) return;
		m_AvgMetric = m_Metric1;
//		Application->MainForm->Caption = "Phase odd";
	}
    else {						// MFSK8, MFSK16, MFSK31
		if( (c = m_Viterbi2.Decode(&metric, m_ViterbiPair)) == -1 ) return;
		DoAvg(m_Metric2, metric, GetFactor());
		if( (m_MFSK_TONES != 16) && (m_Metric2 < m_Metric1) ) return;
		m_AvgMetric = m_Metric2;
//		Application->MainForm->Caption = "Phase even";
	}
//	char bf[256];
//    sprintf(bf, "%d", m_ViterbiPhase);
//    Application->MainForm->Caption = bf;
//
//	S/N			Metric
//	30dB		240
//	20dB		220
//	10dB		110
//	Application->MainForm->Caption = int(m_AvgMetric);
//
	m_SHDATA = m_SHDATA << 1;
    if( c ) m_SHDATA |= 1;

	if( (m_SHDATA & 7) == 1 ){		// 001��T��
		c = g_VariCode.DecodeMFSK(m_SHDATA >> 1);
		if( (c != -1) && (m_fSQ || m_fDelaySQ) ) m_RxData.PutData(c);
		m_SHDATA = 1;				// ���̕���(1xxx)�̐擪
	}
}
//--------------------------------------------------------------------------
BYTE __fastcall Limit256(double x)
{
	if( x < 0 ) return 0;
    if( x > 255 ) return 255;
    return BYTE(x);
}
//--------------------------------------------------------------------------
void __fastcall CDecMFSK::RecSym(const double *pAmp)
{
	int		i, n;
	BYTE	tSym[MFSK_MAXBITS];
	double	tVal[MFSK_MAXBITS];
	double	*dp;

	memset(tVal, 0, sizeof(tVal));
	double sum = 1e-10;
	for(i = 0; i < m_MFSK_TONES; i++){
		if( m_fLSB ){
			n = m_MFSK_TONES - i - 1;
        }
		else {
			n = i;
        }

		double v = pAmp[n];				// ���U��
		int gray = (i >> 1) ^ i;		// �O���[�R�[�h�ɕϊ�
        int mask = m_MFSK_TONES >> 1;
		dp = tVal;
		for( n = 0; n < m_MFSK_BITS; n++, mask = mask >> 1, dp++ ){
			if( gray & mask ){
				*dp += v;
			}
			else {
				*dp -= v;
			}
		}
		sum += v;
	}
	sum = 128.0 / sum;
	dp = tVal;
    BYTE *bp = tSym;
	for(i = 0; i < m_MFSK_BITS; i++){
		*bp++ = Limit256(128.0 + (sum * *dp++));
	}
	m_InterLeaver.DecodeSyms(tSym);
	bp = tSym;
	for(i = 0; i < m_MFSK_BITS; i++) StreamSym(*bp++);
}

//--------------------------------------------------------------------------
BYTE __fastcall CDecMFSK::DecodeSym(CLX *pFFT, double *pAmp)
{
	int sym = 0;
	double d;
    double max = 0.0;

	for( int i = 0; i < m_MFSK_TONES; i++ ){
		*pAmp++ = d = pFFT[i].vAbs();
		if( d > max ){
			max = d;
			sym = i;
		}
	}
	return BYTE(sym);
}

//--------------------------------------------------------------------------
void __fastcall CDecMFSK::DoSync(void)
{
/*
	�P�O�̃V���{���̍ő�U���ʒu�𓾂�

	m_dBTW = 7, m_AmpStgMax = 14
	   v				= m_AmpStgPtr
	22220001111111		= m_AmpStg[]
	<--i<---------		i = 0 to 13
    3210dcba987654
	          N
*/
    int N = 0;
	double max = 0.0;
	double *pAmp = &m_AmpStg[m_AmpStgPtr][m_SymStg[1]];
	for(int i = 0; i < m_AmpStgMax; i++){
		if( pAmp < m_AmpStg[0] ){
			pAmp = &m_AmpStg[m_AmpStgMax-1][m_SymStg[1]];
        }
		if( *pAmp > max){
			N = i;
			max = *pAmp;
		}
        pAmp -= MFSK_MAXTONES;
	}
	// �J�E���^��␳����
	m_dNow += (N - m_dTW) * 0.125;

    // �^�C�~���O�v���̊J�n
	if( m_MeasStage < 16 ){
    	m_MeasStage++;
	   	m_MeasClock = m_MeasCounter = 0;
        m_SyncState = FALSE;
    }
    else {
		m_SyncState = (fabs(m_dNow) < (m_dTW * 0.05));
    }
//	char bf[256];
//    sprintf(bf, "%.1lf", m_dNow);
//	Application->MainForm->Caption = bf;
}

//--------------------------------------------------------------------------
void __fastcall CDecMFSK::DoMeas(void)
{
	if( m_MeasStage ){
		m_MeasClock++;
		if( m_MeasClock >= 128 ){
			m_AvgClock.Do(double(m_MeasCounter)/double(m_MeasClock));
		   	m_MeasClock = m_MeasCounter = 0;

       		// ATC����
			if( IsMetSQ() ){
				DoAvg(m_dTW, m_AvgClock.GetAvg(), (m_AvgMetric >= 200) ? 0.8 : 0.4);
				if( m_dTW < m_dBTWL ){ m_dTW = m_dBTWL; }
       	        if( m_dTW > m_dBTWH ){ m_dTW = m_dBTWH; }
            }
			else {
				DoAvg(m_dTW, m_dBTW, 0.25);
			}
        }
	}
}
//--------------------------------------------------------------------------
void __fastcall CDecMFSK::DoAFC(CLX *pFFT)
{
	// �R�V���{���A���œ������g��
	if( (m_SymStg[2] != m_SymStg[1]) || (m_SymStg[2] != m_SymStg[0]) ) return;
	// �x�[�X�g�[���݂̂����o����
	if( m_fLSB ){
		if( m_SymStg[2] != (m_MFSK_TONES - 1) ) return;
    }
    else {
		if( m_SymStg[2] != 0 ) return;
    }

	int err = GetClockError();
	if( ABS(err) >= 2000 ) return;	// ATC�␳���傫�����͌덷���傫��

	CLX		z;
	pFFT += m_SymStg[2];
	pFFT->PhDiff(z, m_PrevZ);			// �ʑ����̂ݕK�v�Ȃ̂ŕ���̌v�Z�͕s�v
	double ferr = z.Phase() * m_kAFC;	// �ʑ��������g���΍��ɕϊ�
	if( (ferr > -m_AFCWidth) && (ferr < m_AFCWidth) ){
		m_AvgAFC.DoZ(ferr);
    }

#if 0
	double d = pFFT->Phase() - m_PrevZ.Phase();
    d += fmod(2*PI*(m_dBTW-m_dTW), 2*PI);
	if( d >= PI ){d = d - PI*2;} else if( d <= -PI ){d = d + PI*2;}
    d *= m_kAFC;
	char bf[256];
	sprintf(bf, "%.2lf %.2lf %.2lf", d, ferr, m_dBTW-m_dTW);
    Application->MainForm->Caption = bf;
#endif
}
//--------------------------------------------------------------------------
BOOL __fastcall CDecMFSK::GetAFCShift(double &fq)
{
	if( m_AvgAFC.IsFull() ){
		fq = m_AvgAFC.GetAvg();
        m_AvgAFC.Reset();
        return TRUE;
    }
    else {
		return FALSE;
    }
}
//--------------------------------------------------------------------------
#define DEBUG_MFSK	FALSE
#if DEBUG_MFSK
FILE *pMFSKFP = fopen("test.txt", "wt");
#endif
//--------------------------------------------------------------------------
int __fastcall CDecMFSK::Do(CLX *pFFT, BOOL fSQ)
{
    BOOL f = FALSE;

	if( m_AmpStgPtr >= m_AmpStgMax ) m_AmpStgPtr = 0;
	m_SymStg[2] = DecodeSym(pFFT, m_AmpStg[m_AmpStgPtr]);

	m_MeasCounter++;
	m_dNow += 1.0;
	if( m_dNow >= m_dTW ){
		m_dNow -= m_dTW;
		m_Tmg = !m_Tmg;
		if( fSQ ) DoMeas();

#if DEBUG_MFSK
		if( fSQ ) fprintf(pMFSKFP, "%02X\n", m_SymStg[2]);
#endif

		// �x���X�P���`����
        if( m_fSQ != fSQ ){
			m_fSQ = fSQ;
			if( !m_pMainCtrl->m_MFSK_SQ_Metric ){
	            m_SQDelayCount = 32;
            }
		    ResetMeas();
        }
        else if( m_SQDelayCount ){
			m_SQDelayCount--;
            if( !m_SQDelayCount ){
				m_fDelaySQ = m_fSQ;
            }
        }
		RecSym(m_AmpStg[m_AmpStgPtr]);
		// �V���{����2��A���ŕω������ꍇ�̂ݓ����ʒu���v�Z
		if( (m_SymStg[2] != m_SymStg[1]) && (m_SymStg[0] != m_SymStg[1]) ){
			DoSync();
        }
		if( fSQ ) DoAFC(pFFT);
		m_PrevZ = pFFT[m_SymStg[2]];
		memcpy(m_SymStg, &m_SymStg[1], 2 * sizeof(m_SymStg[0]));
        f = TRUE;
	}
	m_AmpStgPtr++;
	return f;
}

//--------------------------------------------------------------------------
int __fastcall CDecMFSK::GetData(void)
{
	if( m_RxData.GetCount() ){
		return m_RxData.GetData();
    }
    else {
		return -1;
    }
}
//--------------------------------------------------------------------------
int __fastcall CDecMFSK::GetClockError(void)
{
	if( m_AvgClock.GetCount() ){
//		double tw = (m_MeasStage >= 2) ? m_dTW : m_AvgClock.GetAvg();
		return int(((m_dTW / m_dBTW) - 1.0) * 1e6);
    }
    else {
		return 0;
    }
}
//--------------------------------------------------------------------------
void __fastcall CDecMFSK::SetTmg(int ppm)
{
	if( ppm < -15000 ) ppm = -15000;
    if( ppm > 15000 ) ppm = 15000;

	m_dBTW = m_SampleFreq / m_MFSK_SPEED;
	m_dTW = m_dBTW = m_dBTW + (m_dBTW * ppm * 1.0e-6);
}
//--------------------------------------------------------------------------
// 0 to 2048
int __fastcall CDecMFSK::GetMetric(int sw)
{
	double d;
	switch(sw){
		case 1:
        	d = m_Metric2;
            break;
		case 2:
        	d = m_Metric1;
            break;
        default:
        	d = m_AvgMetric;
            break;
    }
	if( d < 80.0 ){
		d = (d - 80.0) * 2.0 + 80.0;
        if( d < 0.0 ) d = 0.0;
    }
    else if( d >= 200.0 ){
		d = 200.0 + (d - 200.0) * ((512.0-200.0) / (256.0-200.0));
    }
    return int(d*4.0);
}
/*=============================================================================
  CPlayBack�N���X
=============================================================================*/
CPlayBack::CPlayBack()
{
	m_StgWidth = 0;
    m_StgMax = 0;
    m_pStg = NULL;
    Clear();
}
//--------------------------------------------------------------------------
CPlayBack::~CPlayBack()
{
	if( m_pStg ) delete m_pStg;
}
//--------------------------------------------------------------------------
void __fastcall CPlayBack::Delete(void)
{
	m_StgMax = 0;
    m_StgWidth = 0;
    if( m_pStg ){
		delete m_pStg;
        m_pStg = NULL;
    }
}
//--------------------------------------------------------------------------
void __fastcall CPlayBack::Init(int wSize, int SampBase)
{
	m_StgWidth = wSize;
    m_StgMax = int((SampBase * 60.0 / wSize) + 0.5);
	int wmax = m_StgMax * wSize;
    if( m_pStg ) delete m_pStg;
	m_pStg = new short[wmax];
    Clear();
}
//--------------------------------------------------------------------------
void __fastcall CPlayBack::Clear(void)
{
	m_StgCnt = m_StgRCnt = 0;
    m_StgW = m_StgR = 0;
    m_WTimer = 0;
}
//--------------------------------------------------------------------------
void __fastcall CPlayBack::Write(const short *p)
{
	if( m_StgRCnt ) return;
	if( m_WTimer ){
		m_WTimer--;
        return;
    }
	memcpy(&m_pStg[m_StgW*m_StgWidth], p, sizeof(short)*m_StgWidth);
    m_StgW++;
    if( m_StgW >= m_StgMax ) m_StgW = 0;
    if( m_StgCnt < m_StgMax ) m_StgCnt++;
}
//--------------------------------------------------------------------------
BOOL __fastcall CPlayBack::Read(short *p)
{
	if( !m_StgRCnt ) return FALSE;

	memcpy(p, &m_pStg[m_StgR*m_StgWidth], sizeof(WORD)*m_StgWidth);
   	m_StgR++;
    if( m_StgR >= m_StgMax ) m_StgR = 0;
    m_StgRCnt--;
    return TRUE;
}
//--------------------------------------------------------------------------
void __fastcall CPlayBack::StopPlaying(void)
{
    m_StgRCnt = 0;
}
//--------------------------------------------------------------------------
BOOL __fastcall CPlayBack::StartPlaying(int s)
{
	if( !m_StgCnt ) return FALSE;

	m_WTimer = m_StgMax * 3 / 60;
	m_StgRCnt = m_StgMax * s / 60;
    if( m_StgRCnt > m_StgCnt ) m_StgRCnt = m_StgCnt;
    m_StgR = m_StgW - m_StgRCnt;
	if(m_StgR < 0) m_StgR += m_StgMax;
    return m_StgRCnt;
}

/*=============================================================================
  CDelay�N���X
=============================================================================*/
//---------------------------------------------------------------------------
CDelay::CDelay(void)
{
	m_pStg = NULL;
    m_Delay = 0;
    m_CurPnt = 0;
    m_Count = 0;
}
//---------------------------------------------------------------------------
CDelay::~CDelay()
{
	if( m_pStg ) delete m_pStg;
}
//---------------------------------------------------------------------------
void __fastcall CDelay::Create(int delay)
{
	if( !m_pStg || (m_Delay != delay) ){
		if( m_pStg ) delete m_pStg;
        m_pStg = new double[delay];
    }
    m_Delay = delay;
    Reset();
}
//---------------------------------------------------------------------------
void __fastcall CDelay::Reset(void)
{
	memset(m_pStg, 0, sizeof(double)*m_Delay);
    m_CurPnt = 0;
    m_Count = 0;
}
//---------------------------------------------------------------------------
double __fastcall CDelay::Do(const double &d)
{
	double *cp = &m_pStg[m_CurPnt];
    double r = *cp;
    *cp = d;
    m_CurPnt = CIRCULATE(m_CurPnt+1, m_Delay);
    if( m_Count < m_Delay ) m_Count++;
    return r;
}
//---------------------------------------------------------------------------
double __fastcall CDelay::GetData(void)
{
	return m_pStg[m_CurPnt];
}
/*=============================================================================
  CCIC�N���X
=============================================================================*/
//---------------------------------------------------------------------------
CCIC::CCIC(void)
{
	m_N = 0;
    m_K = 0;
}

//---------------------------------------------------------------------------
void __fastcall CCIC::Create(int n)
{
	m_N = n;
    m_K = 1.0 / double(n);

    m_Com.Create(n);
    m_Z = 0;
}

//---------------------------------------------------------------------------
double __fastcall CCIC::Do(double d)
{
	d -= m_Com.Do(d);
	d += m_Z;
	m_Z = d;
    return d * m_K;
}
/*=============================================================================
  CCICM�N���X
=============================================================================*/
//---------------------------------------------------------------------------
CCICM::CCICM(void)
{
	m_N = 0;
}
//---------------------------------------------------------------------------
CCICM::~CCICM(void)
{
}
//---------------------------------------------------------------------------
void __fastcall CCICM::Create(int stages, int N)
{
	m_N = stages;
	for( int i = 0; i < stages; i++ ){
		m_tCIC[i].Create(N);
    }

}
//---------------------------------------------------------------------------
double __fastcall CCICM::Do(double d)
{
	CCIC *cp = m_tCIC;
	for( int i = 0; i < m_N; i++, cp++ ){
		d = cp->Do(d);
    }
    return d;
}
/*=============================================================================
  CRxSet�N���X
=============================================================================*/
//---------------------------------------------------------------------------
// ����̏�����
CRxSet::CRxSet()
{
	m_pMain = NULL;
	m_Mode = sys.m_DefaultMode;
	m_Charset = sys.m_BaseCharset;
    m_fTWO = 0;
    m_fMBCS = 0;
    m_fAFC = TRUE;
    m_SQLevel = 300;
	m_SQ = FALSE;
    m_SQTimer = 0;
    m_Speed = SPEED;
    m_AFCTimerW = 0;
    m_AFCTimerN = 0;
    m_AFCTimerW2 = 0;
    m_AFCFQ = 0;
    m_AFCSQ = 0;
    m_fATC = FALSE;
	m_fRTTYFFT = FALSE;
    m_WaterW = 400;
    m_pFFT = NULL;
	m_CarrierFreq = 1750;
	m_MFSK_TYPE = typMFSK16;
	m_AFCTimerPSK = m_AFCTimerMFSK = 0;

	memset(&m_StgFFT, 0, sizeof(m_StgFFT));
	m_StgFFT.VW = 100;

    m_cAutoTS1 = m_cAutoTS2 = 0;
	m_AvgAFC.Create(8);

	m_pDem = NULL;
	m_Timing = 0x80000000;

	m_RxShift = 170.0;
	m_fCollectScope = FALSE;
	m_PFTimer = 0;
	m_fOver = FALSE;
}
//---------------------------------------------------------------------------
CRxSet::~CRxSet()
{
	Delete();
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetMFSKType(int type)
{
	m_MFSK_TYPE = type;
	if( IsActive() ) m_pDem->SetMFSKType(type);
}
//---------------------------------------------------------------------------
double __fastcall CRxSet::GetSpeed(void)
{
	if( IsMFSK() ){
		return IsActive() ? m_pDem->m_MFSK_SPEED : 15.625;
    }
    else {
		return m_Speed;
    }
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetSpeed(double b)
{
	if( b < MIN_SPEED ) return;
   	if( b > MAX_SPEED ) return;
	if( m_Speed != b ){
		m_Speed = b;
   		if( IsActive() ) m_pDem->SetSpeed(b);
    }
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetMode(int mode)
{
	int Offset = 0;
    double Speed = 0.0;
	if( m_Mode != mode ){
		if( ::IsRTTY(mode) ){
			if( !::IsRTTY(m_Mode) ){
				Speed = 45.45;
            }
        }
		else if( ::IsMFSK(mode) ){
			if( !::IsMFSK(m_Mode) ){
				Speed = 15.625;
            }
        }
        else {
			if( ::IsRTTY(m_Mode) || ::IsMFSK(m_Mode) ){
				Speed = 31.25;
            }
        }
        if( ::IsMFSK(mode) && ::IsMFSK(m_Mode) && !m_pMain->m_MFSK_Center ){
			int bw = int(m_pDem ? m_pDem->m_MFSK_BW : 234.375);
			Offset = (mode == MODE_mfsk_U) ? -bw : bw;
        }
		m_Timing = 0x80000000;
    }
	m_Mode = mode;
	if( m_pMain ){
		if( Speed > 1.0 ) m_pMain->SetSpeed(this, Speed);
		m_pMain->InitCollect(this, ::IsRTTY(m_Mode) ? 7 : 14);
	}
	switch(m_Mode){
		case MODE_RTTY:
        case MODE_U_RTTY:
		case MODE_GMSK:
        case MODE_FSK:
        case MODE_FSKW:
        case MODE_BPSK:
			m_fTWO = FALSE;
           	break;
        case MODE_N_BPSK:
        case MODE_mfsk_L:
        case MODE_mfsk_U:
			m_fTWO = TRUE;
           	break;
    }
	m_fMBCS = FALSE;
	m_AFCTimerPSK = m_AFCTimerMFSK = 0;
	m_PFTimer = 0;
    if( IsActive() ){
		m_pDem->SetType(m_Mode);
		m_AvgAFC.Create(IsMFSK() ? 4 : 8);
		m_AvgAFC.Reset(m_pDem->m_CarrierFreq);
		SetCollect(m_fCollectScope);
	}
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetCarrierFreq(double f)
{
	if( IsActive() ){
		m_pDem->SetCarrierFreq(f);
    }
	m_CarrierFreq = int(f);
    m_AvgAFC.Reset(f);
	m_AFCTimerPSK = 0;
    m_AFCFQ = int(f);
	if( !IsBPSK() ) m_PFTimer = 4;
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::Create(BOOL fSub, CMMVARICtrl *pMain)
{
	m_pMain = pMain;
	m_pFFT = &pMain->m_FFT;
	m_SQ = FALSE;
    m_SQTimer = 0;
    m_AFCTimerW = 0;
    m_AFCTimerN = 0;
    m_AFCTimerW2 = 0;
    m_AFCFQ = 0;
    m_AFCSQ = 0;
    m_fATC = FALSE;

	m_StgFFT.dBSum = 0;
    m_StgFFT.dBMax = 0;
	m_StgFFT.dBWMax = 0;
	m_StgFFT.VW = 100;
    m_StgFFT.DispSig = 0;
    m_StgFFT.Timer = 0;
	InitStgFFT();

    m_cAutoTS1 = m_cAutoTS2 = 0;

    m_PeakSig = 0;
    m_AvgSig.Create(32);
	LimitInt(&m_CarrierFreq, MIN_CARRIER, m_pMain->m_MaxCarrier);
	LimitInt(&m_SQLevel, 0, LEVELMAX);
    LimitDbl(&m_Speed, MIN_SPEED, MAX_SPEED);

	if( IsRTTY() ) m_Speed = 45.45;

	EPHASE(P_SUBCREATE);
	if( !m_pDem ){
		m_pDem = new CDEMFSK(pMain);
		if( !m_pDem ){
//			MainVARI->SetErrorMsg("Out of memory");
			return;
        }
		m_pDem->m_RTTYShift = m_RxShift;
		m_pDem->m_fRTTYFFT = m_fRTTYFFT;
		m_pDem->SetMFSKType(m_MFSK_TYPE);
        m_pDem->SetSampleFreq(m_pMain->m_DemSampleFreq);
        m_pDem->SetSpeed(m_Speed);
        m_pDem->SetType(m_Mode);
        m_pDem->m_Decode.SetATCSpeed(pMain->m_ATCSpeed);
        m_pDem->m_Decode.SetATCLimit(pMain->m_ATCLimit);
		if( fSub ){			// �T�u�E�C���h�E�̏ꍇ
            m_pDem->m_Decode.m_fATC = TRUE;
            m_pDem->m_fAFC = TRUE;
            m_pDem->m_fEnableAFC = TRUE;
			CDEMFSK *pDem = pMain->m_RxSet[0].m_pDem;
            m_pDem->MakeBPF(pDem->m_PreBPFTaps);
            m_pDem->m_pBPF = &pDem->m_inBPF;
			SetCarrierFreq(m_CarrierFreq);
//            m_fJA = MainVARI->m_RxSet[0].m_fJA;
        }
        InitStgFFT();
	    m_StgFFT.Timer = 0;
		SetCollect(m_fCollectScope);
    }
	EPHASE(P_SUBVIEW);
#if 0
    if( fView && !m_pView ){
        m_pView = new TRxViewDlg(MainVARI);
		if( !m_pView ){
			delete m_pDem; m_pDem = NULL;
			MainVARI->SetErrorMsg("Out of memory");
			return;
        }
        m_pView->m_pRxSet = this;
 		m_pView->SetBounds(m_rcView.left, m_rcView.top, m_rcView.right, m_rcView.bottom);
        m_pView->UpdateWaterWidth();
		m_pView->m_Dump.SetRTTY(IsRTTY());
		m_pView->m_Dump.ShowCtrl(sys.m_fShowCtrlCode);
		if( m_FontData.m_Height ){
			FontData2Font(m_pView->PC->Font, &m_FontData);
        }
		m_pView->OnUpdateFont();
        memcpy(m_pView->m_Dump.m_Color, MainVARI->m_Dump.m_Color, sizeof(MainVARI->m_Dump.m_Color));
//        ::SetWindowPos(m_pView->Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
    }
#endif
	EPHASE(P_NULL);
    SetMode(m_Mode);
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::Delete(void)
{
	if( m_pDem ){
		m_CarrierFreq = int(m_pDem->m_CarrierFreq + 0.5);
    	delete m_pDem;
        m_pDem = NULL;
    }
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetSampleFreq(double f)
{
	if( !IsActive() ) return;
	m_pDem->SetSampleFreq(f);
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::InitStgFFT(void)
{
	if( m_pFFT->m_FFTGain ){
		m_StgFFT.Sum = 1024;
	    m_StgFFT.Max = 1024;
    	m_StgFFT.WMax = 1024;
    }
    else {
        m_StgFFT.Sum = 5000;
   	    m_StgFFT.Max = 5000;
   	    m_StgFFT.WMax = 5000;
    }
    m_StgFFT.Sig = 0;
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::ClearWindow(void)
{
	if( !IsActive() ) return;
}
//---------------------------------------------------------------------------
double __fastcall CRxSet::GetBandWidth(void)
{
	switch(m_Mode){
		case MODE_GMSK:
        case MODE_BPSK:
        case MODE_N_BPSK:
			return m_Speed;
        case MODE_FSK:
			return m_Speed*2;
        case MODE_FSKW:
		case MODE_RTTY:
        case MODE_U_RTTY:
			return m_pDem->m_RTTYShift;
        case MODE_mfsk_L:
        case MODE_mfsk_U:
			if( m_pDem ){
				return m_pDem->m_MFSK_BW;
            }
            else {
	        	return 234.375;
            }
    }
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetRTTYFFT(BOOL f)
{
	m_fRTTYFFT = f;
	if( IsActive() ) m_pDem->m_fRTTYFFT = f;
}

//---------------------------------------------------------------------------
LPCSTR GetVariType(BYTE charset, int mode)
{
	if( (mode == MODE_RTTY) || (mode == MODE_U_RTTY) ){
		return "BAUDOT";
    }
	else if( IsMFSK(mode) ){
    	LPCSTR p;
		switch(charset){
			case SHIFTJIS_CHARSET:
				p = "VariMFSK/JA";
				break;
			case HANGEUL_CHARSET:
		        case JOHAB_CHARSET:
       		   	p = "VariMFSK/HL";
	           	break;
			case CHINESEBIG5_CHARSET:   //
   	    	   	p = "VariMFSK/BV";
       	    	break;
			case 134:       // �ȗ�
				p = "VariMFSK/BY";
				break;
			default:
            	p = "VariMFSK";
   		        break;
	    }
        return p;
    }
	BOOL f = mode == (MODE_N_BPSK);
	LPCSTR p = "VariSTD";
	switch(charset){
		case SHIFTJIS_CHARSET:
			p = f ? "VariSTD/JA":"VariJA";
			break;
		case HANGEUL_CHARSET:
	        case JOHAB_CHARSET:
       	   	p = f ? "VariSTD/HL":"VariHL";
           	break;
		case CHINESEBIG5_CHARSET:   //
   	       	p = f ? "VariSTD/BV":"VariBV";
       	    break;
		case 134:       // �ȗ�
			p = f ? "VariSTD/BY":"VariBY";
			break;
		default:
   	        break;
    }
	return p;
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetRxShift(double shift)
{
	m_RxShift = shift;
	if( IsActive() ){
		m_pDem->SetRTTYShift(shift);
	}
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetCollect(BOOL f)
{
	m_fCollectScope = f;
	if( IsActive() ) m_pDem->SetCollect(f, int(m_pMain->m_BufferSize*m_pMain->m_DecFactor));
}
//---------------------------------------------------------------------------
BOOL __fastcall CRxSet::IsFreqErr(double d)
{
	if( IsBPSK() ){
		return fabs(d) > 0.30;		// 0.15 X 2.0
    }
	else {
		return fabs(d) > 0.20;
    }
}
/*=============================================================================
  CNotches�N���X
=============================================================================*/
//---------------------------------------------------------------------------
CNotches::CNotches(void)
{
	m_nBaseTaps = 128;
    m_NotchWidth = 1;
	m_Count = m_Max = 0;
    m_pBase = NULL;
    m_SampleFreq = SAMPFREQ;
}
//---------------------------------------------------------------------------
CNotches::~CNotches()
{
	Delete();
}
//---------------------------------------------------------------------------
void __fastcall CNotches::Delete(void)
{
	if( m_pBase ){
    	delete m_pBase;
        m_pBase = NULL;
    }
    m_Count = m_Max = 0;
    m_FIR.Delete();
}
//---------------------------------------------------------------------------
void __fastcall CNotches::Create(void)
{
	if( m_Count ){
		if( m_Count == 1 ){		// �V���O���m�b�`
			m_nTaps = int(m_nBaseTaps * m_SampleFreq / 11025);
    	    m_nTaps = (m_nTaps + 1) & 0x0ffe;
			if( m_nTaps < 4 ) m_nTaps = 4;
			double fl = m_pBase[0].Freq - m_NotchWidth;
			double fh = m_pBase[0].Freq + m_NotchWidth;
        	int att = m_NotchWidth > 50 ? 60 : 10;
			double maxCarrier = m_SampleFreq * 0.49;
			if( fl < 100.0 ){
				m_FIR.Create(m_nTaps, ffHPF, m_SampleFreq, fh, fh, 60, 1.0);
    	    }
	        else if( (fh > maxCarrier) && (m_NotchWidth >= 10) ){
				m_FIR.Create(m_nTaps, ffLPF, m_SampleFreq, fl, fl, 60, 1.0);
			}
	        else {
				m_FIR.Create(m_nTaps, ffBEF, m_SampleFreq, fl, fh, att, 1.0);
        	}
        }
        else {					// �}���`�m�b�`
			m_nTaps = int(m_nBaseTaps * 1.5 * m_SampleFreq / 11025);
    	    m_nTaps = (m_nTaps + 1) & 0x0ffe;
			if( m_nTaps < 4 ) m_nTaps = 4;
            int e = m_nTaps / 2;
			double *pSamp = new double[e];
            double w = m_SampleFreq / m_nTaps;
            if( w > 30.0 ) w = 30.0;
            double bw = m_NotchWidth;
            if( bw < w ) bw = w;
		    for( int i = 0; i < e; i++ ){
				double fq = i * m_SampleFreq / m_nTaps;
				double gm = 1.0;
			    for( int j = 0; j < m_Count; j++ ){
					double g = fq - m_pBase[j].Freq;
			        g = fabs(g);
			        g = g * 0.5 / bw;
			        g *= g;
			        if( g < gm ) gm = g;
			    }
                pSamp[i] = gm;
		    }
            m_FIR.CreateSamp(m_nTaps, m_SampleFreq, pSamp, 0);
            delete pSamp;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall CNotches::Alloc(int nIndex)
{
	if( nIndex >= m_Max ){
		int max = m_Max ? m_Max * 2 : 32;
        NOTCHCTR *pNew = new NOTCHCTR[max];
        if( m_Count ) memcpy(pNew, m_pBase, m_Count * sizeof(NOTCHCTR));
        if( m_pBase ) delete m_pBase;
        m_pBase = pNew;
        m_Max = max;
    }
}
//---------------------------------------------------------------------------
int __fastcall CNotches::Find(int Freq)
{
	for( int i = 0; i < m_Count; i++ ){
		if( m_pBase[i].Freq == Freq ) return i;
    }
    return -1;
}
//---------------------------------------------------------------------------
void __fastcall CNotches::Add(int Freq, BOOL bFouce)
{
	if( !bFouce && (Find(Freq) >= 0) ) return;

	Alloc(m_Count);
    m_pBase[m_Count].Freq = short(Freq);
    m_Count++;
    Create();
}
//---------------------------------------------------------------------------
void __fastcall CNotches::SetFreq(int nIndex, int Freq)
{
	if( nIndex < m_Count ){
		if( m_pBase[nIndex].Freq != Freq ){
			m_pBase[nIndex].Freq = Freq;
    	    Create();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall CNotches::Delete(int nIndex)
{
	if( (nIndex < 0) || (nIndex >= m_Count) ) return;

	NOTCHCTR *np = &m_pBase[nIndex];
    int size = sizeof(NOTCHCTR)*(m_Count-nIndex-1);
	if( size > 0 ) memcpy(np, np+1, size);
    m_Count--;
	if( m_Count ){
	    Create();
    }
    else {
		Delete();
    }
}

/*=============================================================================
  CPHASEX�N���X
=============================================================================*/
CPHASEX::CPHASEX()
{
	m_TONES = 3;
	m_SHIFT = 170.0;
    m_BASEPOINT = int(CPHASEX_BASEFREQ * m_TONES / m_SHIFT);


	m_SampleFreq = 11025.0*0.5;
    m_CarrierFreq = 1750;
    m_MixerFreq = 0;
    SetSampleFreq(m_SampleFreq);
	m_AGC.SetCarrierFreq(m_CarrierFreq);
}
//--------------------------------------------------------------------------
void __fastcall CPHASEX::SetSampleFreq(double f)
{
	m_SampleFreq = f;
    Create();
}
//--------------------------------------------------------------------------
void __fastcall CPHASEX::Create(void)
{
//    m_SymbolLen = m_SampleFreq/m_SPEED;
//	m_LPF.Create(48, ffLPF, m_SampleFreq, m_SHIFT*2, m_SHIFT*2, 40.0, 1.0);
	m_SymbolLen = m_TONES * m_SampleFreq / m_SHIFT;
    m_BASEPOINT = int(CPHASEX_BASEFREQ * m_TONES / m_SHIFT);

	m_MixerFreq = double(m_BASEPOINT) * m_SampleFreq / m_SymbolLen;
	m_VCO.SetSampleFreq(m_SampleFreq);
	m_Hilbert.Create(20, m_SampleFreq, 25.0, m_SampleFreq*0.5 - 25.0);
    m_Hilbert.Clear();
    SetCarrierFreq(m_CarrierFreq);
    m_SlideFFT.Create(int(m_SymbolLen + 0.5), m_BASEPOINT, m_TONES);
	m_AGC.SetSampleFreq(m_SampleFreq);
	m_AGC.SetCarrierFreq(m_CarrierFreq);
#if LOGFFT
    m_fp = fopen("FFT.txt", "wt");
#endif
//	ShowPara();
}
//--------------------------------------------------------------------------
void __fastcall CPHASEX::SetShift(double f)
{
	m_SHIFT = f * (m_TONES + 1) / m_TONES;
    Create();
}
//--------------------------------------------------------------------------
void __fastcall CPHASEX::SetCarrierFreq(double f)
{
	m_CarrierFreq = f;
    m_VCO.SetFreeFreq(m_CarrierFreq - m_MixerFreq);
	m_AGC.SetCarrierFreq(m_CarrierFreq);
//	ShowPara();
}
//--------------------------------------------------------------------------
CLX* __fastcall CPHASEX::Do(double d)
{
	m_Hilbert.Do(m_sig, d);					// ���f����

    CLX z;
	z.r = m_VCO.Do();
    z.j = m_VCO.DoCos();
	z *= m_sig;								// ���g���ϊ�

//	m_LPF.Do(z);
	return m_SlideFFT.Do(z);
}
//--------------------------------------------------------------------------
void __fastcall CPHASEX::DoFSK(double d)
{
	d = m_AGC.Do(d);
	CLX *pFFT = Do(d);
    m_dm = pFFT[0].vAbs();
    m_ds = pFFT[m_TONES-1].vAbs();

#if LOGFFT
    for( int i = 0; i < m_TONES; i++ ){
		if( i ) fprintf(m_fp, ",");
		fprintf(m_fp, "%.0lf", pFFT[i].vAbs());
    }
    fprintf(m_fp, "\n");
#endif
}
