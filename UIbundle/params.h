/*
 *  params.h
 *  UIbundle
 *
 *  Created by Takashi Mochizuki on 07/12/02.
 *  Copyright 2007-2010 MyCometG3. All rights reserved.
 *
 */

#ifndef UIparams_h
#define UIparams_h

#define X264PROFILE_UNDEF	0
#define	X264PROFILE_BASE	1
#define	X264PROFILE_MAIN	2
#define	X264PROFILE_HIGH	3

#define	X264PRESET_ULTRAFAST	-3
#define	X264PRESET_SUPERFAST	-5
#define	X264PRESET_VERYFAST	-2
#define	X264PRESET_FASTER	-4
#define	X264PRESET_FAST		-1
#define	X264PRESET_MEDIUM	0
#define	X264PRESET_SLOW		1
#define	X264PRESET_SLOWER	2
#define	X264PRESET_VERYSLOW	4
#define	X264PRESET_PLACEBO	3

#define X264TUNE_UNDEF	0
#define	X264TUNE_FILM	1
#define	X264TUNE_ANIMATION	2
#define	X264TUNE_GRAIN	3
#define	X264TUNE_STILLIMAGE	9
#define	X264TUNE_PSNR	4
#define	X264TUNE_SSIM	5
#define	X264TUNE_TOUHOU	6
#define X264TUNE_FASTDECODE 7
#define X264TUNE_ZEROLATENCY 8

typedef struct {
	// 
	Boolean LOG_INFO;
	Boolean LOG_DEBUG;
	Boolean LOG_STATS;
	Boolean REV_STRUCT;
	
	// codec context flag store
	Boolean reserved0x0001;
	Boolean FLAG_QSCALE;			// 0x0002
	Boolean FLAG_4MV;				// 0x0004
	Boolean reserved0x0008;
	Boolean FLAG_QPEL;				// 0x0010
	Boolean FLAG_GMC;				// 0x0020
	Boolean FLAG_MV0;				// 0x0040
	Boolean FLAG_PART;				// 0x0080
	Boolean reserved0x0100;
	Boolean reserved0x0200;
	Boolean reserved0x0400;
	Boolean FLAG_LOOP_FILTER;		// 0x0800
	Boolean reserved0x1000;
	Boolean reserved0x2000;
	Boolean reserved0x4000;
	Boolean FLAG_PSNR;				// 0x8000
	Boolean reserved0x00010000;
	Boolean FLAG_NORMALIZE_AQP;		// 0x00020000
	Boolean FLAG_INTERLACED_DCT;	// 0x00040000;
	Boolean reserved0x00080000;
	Boolean reserved0x00100000;
	Boolean reserved0x00200000;
	Boolean reserved0x00400000;
	Boolean reserved0x00800000;
	Boolean FLAG_AC_PRED;			// 0x01000000
	Boolean reserved0x02000000;
	Boolean FLAG_CBP_RD;			// 0x04000000
	Boolean FLAG_QP_RD;				// 0x08000000
	Boolean reserved0x10000000;
	Boolean reserved0x20000000;
	Boolean reserved0x40000000;
	Boolean FLAG_CLOSED_GOP;		// 0x80000000
	
	char NATIVE_FPS;				// 1:NTSC, 2:PAL, 3:FILM, 11:N/2, 12:P/2, 13:F/2, 21:Nx2, 22:Px2, 23:Fx2
	char MB_DECISION;				// 1:FF_MB_DECISION_SIMPLE, 2:FF_MB_DECISION_BITS, 3:FF_MB_DECISION_RD
	char RC_QSQUISH;				// 0-> clipping, 1-> use a nice continous function to limit qscale wthin qmin/qmax
	char MPEG_QUANT;				// 0-> h263 quant 1-> mpeg quant.
	char THREADS;					// Thread count. 1:single, 2:dual, 3:quad, 4:octa, 9:auto.
	char GAMMA;						// 0:DontAddGamma, 1:Gamma2.2
	char NCLC;						// 1:DontAddNCLC, 3:616, 4:516, 5:677, 6:111, 7:222
	char ME_METHOD;					// 1:EPZS, 2:HEX, 3:UMH, 4:FULL, 5:X1, 6:TESA
	
	int SC_THRESHOLD;				// limited to (-75K, 75K) 
	int QCOMPRESS;					// limited to (20, 80) as (0.20, 0.80)
	int NOISE_REDUCTION;			// limited to (0, 1000)
	int RC_MAXRATE;					// limited to (0, 245760); Kbps
		
	char REFS;						// limited to (1, 16); default 1
	char ME_RANGE;					// limited to (4, 64); default 16
	char MAX_BFRAMES;				// limited to (1, 16); default 1
	char CODER_TYPE;				// 1:CAVLC, 2:CABAC
	char DIRECTPRED;				// 1:Disabled, 2:Spatial, 3:Temporal, 4:Auto
	char CRF;						// 0:Normal, 1:ConstantRateFactor
	char ADAPTIVE_BFRAME;			// 0:Normal, 1:B_ADAPT_FAST, 2:B_ADAPT_TRELLIS
	char ME_SUBQ;					// limited to (0, 10); subpel quality
	
	char TRELLIS;					// 1:Disabled, 2:FinalOnly, 3:All; default 1
	char TURBO;						// 1:Disabled, 2:Turbo 1, 3:Turbo 2
	char CHROMA_ME;					// 0:OFF, 1:ON; default 1
	char PART_4X4;					// 0:OFF, 1:partition=all; default 0
	char BIDIR_ME;					// 0:OFF, 1:ON; default 0
	char USE3RDPASS;				// 0:OFF, 1:ON; default 0
	char LEVEL;						// 9->auto, 13->1.3, 21->2.1, 30->3.0, 31->3.1, 41->4.1, 51->5.1
	char EMBEDUUID;					// 0:OFF, 1:ON
	
	// codec context flag2 store
	Boolean reserved20x0001;
	Boolean reserved20x0002;
	Boolean reserved20x0004;
	Boolean reserved20x0008;
	Boolean FLAG2_BPYRAMID;			// 0x0010
	Boolean FLAG2_WPRED;			// 0x0020
	Boolean FLAG2_MIXED_REFS;		// 0x0040
	Boolean FLAG2_8X8DCT;			// 0x0080
	Boolean FLAG2_FASTPSKIP;		// 0x0100
	Boolean FLAG2_AUD;				// 0x0200
	Boolean FLAG2_BRDO;				// 0x0400
	Boolean reserved20x0800;
	Boolean reserved20x1000;
	Boolean reserved20x2000;
	Boolean reserved20x4000;
	Boolean reserved20x8000;
	Boolean reserved20x00010000;
	Boolean reserved20x00020000;
	Boolean FLAG2_MBTREE;			// 0x00040000
	Boolean FLAG2_PSY;				// 0x00080000;
	Boolean FLAG2_SSIM;				// 0x00100000;
	Boolean reserved20x00200000;
	Boolean reserved20x00400000;
	Boolean reserved20x00800000;
	Boolean reserved20x01000000;
	Boolean reserved20x02000000;
	Boolean reserved20x04000000;
	Boolean reserved20x08000000;
	Boolean reserved20x10000000;
	Boolean reserved20x20000000;
	Boolean reserved20x40000000;
	Boolean reserved20x80000000;
	
	int RC_BUFSIZE;					// limited to (0, 245760); Kbit
	int AQ_STRENGTH;				// limited to (50, 150) as (0.5, 1.5); default 100
	int PSYRD_RDO;					// limited to (0, 100) as (0.0, 10.0); default 10
	int PSYRD_TRELLIS;				// limited to (0, 100) as (0.0, 10.0); default 0
	
	char DEBLOCK_ALPHA;				// limited to (-6, 6); default 0
	char DEBLOCK_BETA;				// limited to (-6, 6); default 0
	char AQ_MODE;					// 1:NONE, 2:VARIANCE, 3:AUTOVARIANCE; default 2
	char LUMI_MASKING;				// xvid; lumi_masking
	
	char KEYINT_MIN;				// x264; IDR Interval; 0-25; clipped at (1 + KEYINT/2); default 25
	char CQM_PRESET;				// x264; quantizer matrix; 0:flat16, 1:jvt; default 0
	char NO_DCT_DECIMATE;			// x264; dct-decimate; 0:On, 1:Off; default 0
	char MAX_QDIFF;					// limited to (3, 16); x264 default 4; mp4v default 3
	
	char CHROMAOFFSET;				// x264; chroma-qp-offset; (-12, 12) default 0
	char FILTERPRESET;				// CoreVF; off:0, 1-8:preset1-preset8, -1:CoreVF Default
	char SUBSAMPLING;				// 422->420; 0:progressive, 1:interlaced-3:1, 2:interlaced-5:3, -1:422direct(CoreVF)
	char X264PROFILE;				// x264; --profile (git-1177); default 3
	
	char X264PRESET;				// x264; --preset (git-1177); default 0
	char X264TUNE;					// x264; --tune (git-1177); default 0
	char MBTREE;					// legacy; x264; --mbtree (git-1197); default 1 ; Use FLAG2_MBTREE instead from 1.1.10
	char PSY;						// legacy; x264; --psy (git-1197); default 1 ; Use FLAG2_PSY instead from 1.2.3
	
	int RC_LOOKAHEAD;				// x264; --rc-lookahead (git-1197); default 40
	int IP_FACTOR;					// limited to (-300, 300) as (-3.0, 3.0); default 140 (as 1.4)
	int PB_FACTOR;					// limited to (-300, 300) AS (-3.0, 3.0); default 130 (as 1.3)
	int reservedInt4;				// 
	
	char WEIGHTP;					// x264; --weightp; 1:Disabled, 2:Weighted refs, 3:Weighted refs + Duplicates
	char TOPFIELDFIRST;				// top field first; default 0 (=Progressive or bottom field first)
	char LOSSLESS;					// x264; same as "--qp 0"; 1:On, 1:Off; default is 0
	char BD_TUNE;					// x264; --nal-hrd vbr --b-pyramid strict --slices 4 
	
	char USEASPECTRATIO;			// Embed container level Pixel Aspect Ratio
	char USECLEANAPERTURE;			// Embed container level Clean Aperture
	char TAGASPECTRATIO;			// Preset Tag for Pixel Aspect Ratio
	char TAGCLEANAPERTURE;			// Preset Tag for Clean Aperture
	
	UInt32	HSPACING;               /* Horizontal Spacing */
	UInt32	VSPACING;               /* Vertical Spacing */
	UInt32	CLEANAPERTUREWIDTHN;    /* width of clean aperture, numerator, denominator */
	UInt32	CLEANAPERTUREWIDTHD;
	UInt32	CLEANAPERTUREHEIGHTN;   /* height of clean aperture, numerator, denominator*/
	UInt32	CLEANAPERTUREHEIGHTD;
	SInt32	HORIZOFFN;              /* horizontal offset of clean aperture center minus (width-1)/2, numerator, denominator */
	UInt32	HORIZOFFD;
	SInt32	VERTOFFN;               /* vertical offset of clean aperture center minus (height-1)/2, numerator, denominator */
	UInt32	VERTOFFD;
	
	char OVERRIDECRFQSCALE;			/* override quality slider setting in application for CRF/QSCALE */
	char USERCRFQSCALE;				/* user specified qscale value; default 23 */
	char OVERRIDEQMIN;				/* override quality slider setting in application for ABR */
	char USERQMIN;					/* user specified qmin value; default 3 */
	
	char FAKEINTERLACED;			// x264; --fake-interlaced; default 0 (=off)
	char reservedChar2;				// 
	char reservedChar3;				// 
	char reservedChar4;				// 
} params;

#endif	// UIparams_h
