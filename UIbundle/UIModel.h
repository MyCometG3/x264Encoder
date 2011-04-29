/*
 *  UIModel.h
 *  UIbundle
 *
 *  Created by Takashi Mochizuki on 07/12/02.
 *  Copyright 2007-2010 MyCometG3. All rights reserved.
 *
 */

#ifndef UIModel_h
#define UIModel_h

#import "params.h"

@interface kUIModelNAME : NSObject <NSCoding>
{
	params store;
	NSString* fourCC;	// "avc1" or "mp4v" or "xvid"
	
	// ====================================
	
	NSNumber* LOG_INFO;
	NSNumber* LOG_DEBUG;
	NSNumber* LOG_STATS;
	NSNumber* REV_STRUCT;
	
	// codec context flag store
	NSNumber* reserved0x0001;
	NSNumber* FLAG_QSCALE;			// 0x0002
	NSNumber* FLAG_4MV;				// 0x0004
	NSNumber* reserved0x0008;
	NSNumber* FLAG_QPEL;			// 0x0010
	NSNumber* FLAG_GMC;				// 0x0020
	NSNumber* FLAG_MV0;				// 0x0040
	NSNumber* FLAG_PART;			// 0x0080
	NSNumber* reserved0x0100;
	NSNumber* reserved0x0200;
	NSNumber* reserved0x0400;
	NSNumber* FLAG_LOOP_FILTER;		// 0x0800
	NSNumber* reserved0x1000;
	NSNumber* reserved0x2000;
	NSNumber* reserved0x4000;
	NSNumber* FLAG_PSNR;			// 0x8000
	NSNumber* reserved0x00010000;
	NSNumber* FLAG_NORMALIZE_AQP;	// 0x00020000
	NSNumber* FLAG_INTERLACED_DCT;	// 0x00040000;
	NSNumber* reserved0x00080000;
	NSNumber* reserved0x00100000;
	NSNumber* reserved0x00200000;
	NSNumber* reserved0x00400000;
	NSNumber* reserved0x00800000;
	NSNumber* FLAG_AC_PRED;			// 0x01000000
	NSNumber* reserved0x02000000;
	NSNumber* FLAG_CBP_RD;			// 0x04000000
	NSNumber* FLAG_QP_RD;			// 0x08000000
	NSNumber* reserved0x10000000;
	NSNumber* reserved0x20000000;
	NSNumber* reserved0x40000000;
	NSNumber* FLAG_CLOSED_GOP;		// 0x80000000
	
	NSNumber* NATIVE_FPS;		// 1:NTSC, 2:PAL, 3:FILM, 11:N/2, 12:P/2, 13:F/2, 21:Nx2, 22:Px2, 23:Fx2
	NSNumber* MB_DECISION;		// 1:FF_MB_DECISION_SIMPLE, 2:FF_MB_DECISION_BITS, 3:FF_MB_DECISION_RD
	NSNumber* RC_QSQUISH;		// 0-> clipping, 1-> use a nice continous function to limit qscale wthin qmin/qmax
	NSNumber* MPEG_QUANT;		// 0-> h263 quant 1-> mpeg quant.
	NSNumber* THREADS;			// Thread count. 1:single, 2:dual, 3:quad, 4:octa, 9:auto.
	NSNumber* GAMMA;			// 0:DontAddGamma, 1:Gamma2.2
	NSNumber* NCLC;				// 1:DontAddNCLC, 3:616, 4:516, 5:677, 6:111, 7:222
	NSNumber* ME_METHOD;		// 1:EPZS, 2:HEX, 3:UMH, 4:FULL, 5:X1, 6:TESA
	
	NSNumber* SC_THRESHOLD;		// limited to (-75K, 75K) 
	NSNumber* QCOMPRESS;		// limited to (0.20, 0.80) // !!!!!!!!!!!!!!!!!!!!!!
	NSNumber* NOISE_REDUCTION;	// limited to (0, 1000)
	NSNumber* RC_MAXRATE;		// limited to (0, 245760); Kbps
		
	NSNumber* REFS;				// limited to (1, 16); default 1
	NSNumber* ME_RANGE;			// limited to (4, 64); default 16
	NSNumber* MAX_BFRAMES;		// limited to (1, 16); default 1
	NSNumber* CODER_TYPE;		// 1:CAVLC, 2:CABAC
	NSNumber* DIRECTPRED;		// 1:Disabled, 2:Spatial, 3:Temporal, 4:Auto
	NSNumber* CRF;				// 0:Normal, 1:ConstantRateFactor
	NSNumber* ADAPTIVE_BFRAME;	// 0:Normal, 1:B_ADAPT_FAST, 2:B_ADAPT_TRELLIS
	NSNumber* ME_SUBQ;			// limited to (0, 10); subpel quality
	
	NSNumber* TRELLIS;			// 1:Disabled, 2:FinalOnly, 3:All; default 1
	NSNumber* TURBO;			// 1:Disabled, 2:Turbo 1, 3:Turbo 2
	NSNumber* CHROMA_ME;		// 0:OFF, 1:ON; default 1
	NSNumber* PART_4X4;			// 0:OFF, 1:partition=all; default 0
	NSNumber* BIDIR_ME;			// 0:OFF, 1:ON; default 0
	NSNumber* USE3RDPASS;		// 0:OFF, 1:ON; default 0
	NSNumber* LEVEL;			// 9->auto, 13->1.3, 21->2.1, 30->3.0, 31->3.1, 41->4.1, 51->5.1
	NSNumber* EMBEDUUID;		// 0:OFF, 1:ON
	
	// codec context flag2 store
	NSNumber* reserved20x0001;
	NSNumber* reserved20x0002;
	NSNumber* reserved20x0004;
	NSNumber* reserved20x0008;
	NSNumber* FLAG2_BPYRAMID;		// 0x0010
	NSNumber* FLAG2_WPRED;			// 0x0020
	NSNumber* FLAG2_MIXED_REFS;		// 0x0040
	NSNumber* FLAG2_8X8DCT;			// 0x0080
	NSNumber* FLAG2_FASTPSKIP;		// 0x0100
	NSNumber* FLAG2_AUD;			// 0x0200
	NSNumber* FLAG2_BRDO;			// 0x0400
	NSNumber* reserved20x0800;
	NSNumber* reserved20x1000;
	NSNumber* reserved20x2000;
	NSNumber* reserved20x4000;
	NSNumber* reserved20x8000;
	NSNumber* reserved20x00010000;
	NSNumber* reserved20x00020000;
	NSNumber* reserved20x00040000;
	NSNumber* reserved20x00080000;
	NSNumber* reserved20x00100000;
	NSNumber* reserved20x00200000;
	NSNumber* FLAG2_MBTREE;			// 0x00040000
	NSNumber* FLAG2_PSY;			// 0x00080000;
	NSNumber* FLAG2_SSIM;			// 0x00100000;
	NSNumber* reserved20x02000000;
	NSNumber* reserved20x04000000;
	NSNumber* reserved20x08000000;
	NSNumber* reserved20x10000000;
	NSNumber* reserved20x20000000;
	NSNumber* reserved20x40000000;
	NSNumber* reserved20x80000000;
	
	NSNumber* RC_BUFSIZE;				// limited to (0, 245760); Kbit
	NSNumber* AQ_STRENGTH;				// limited to (0.50, 1.50) // !!!!!!!!!!!!!!!!!!!!!!
	NSNumber* PSYRD_RDO;				// limited to (0.0, 10.0) // !!!!!!!!!!!!!!!!!!!!!!
	NSNumber* PSYRD_TRELLIS;			// limited to (0.0, 10.0) // !!!!!!!!!!!!!!!!!!!!!!
	
	NSNumber* DEBLOCK_ALPHA;			// limited to (-6, 6); default 0
	NSNumber* DEBLOCK_BETA;				// limited to (-6, 6); default 0
	NSNumber* AQ_MODE;					// 1:NONE, 2:VARIANCE, 3:AUTOVARIANCE; default 2
	NSNumber* LUMI_MASKING;				// xvid; lumi_masking
	
	NSNumber* KEYINT_MIN;				// x264; IDR Interval; 0-25; clipped at (1 + KEYINT/2); default 25
	NSNumber* CQM_PRESET;				// x264; quantizer matrix; 0:flat16, 1:jvt; default 0
	NSNumber* NO_DCT_DECIMATE;			// x264; dct-decimate; 0:On, 1:Off; default 0
	NSNumber* MAX_QDIFF;				// limited to (3, 16); x264 default 4; mp4v default 3
	
	NSNumber* CHROMAOFFSET;				// x264; chroma-qp-offset; (-12, 12) default 0
	NSNumber* FILTERPRESET;				// CoreVF; off:0, 1-8:preset1-preset8, -1:CoreVF Default
	NSNumber* SUBSAMPLING;				// 422->420; 0:progressive, 1:interlaced-3:1, 2:interlaced-5:3, -1:422direct(CoreVF)
	NSNumber* X264PROFILE;				// x264; --profile (git-1177); default 3
	
	NSNumber* X264PRESET;				// x264; --preset (git-1177); default 0
	NSNumber* X264TUNE;					// x264; --tune (git-1177); default 0
//	NSNumber* MBTREE;					// legacy; x264; --mbtree (git-1197); default 1 ; Use FLAG2_MBTREE instead from 1.1.10
//	NSNumber* PSY;						// legacy; x264; --psy (git-1197); default 1 ; Use FLAG2_PSY instead from 1.2.3
	
	NSNumber* RC_LOOKAHEAD;				// x264; --rc-lookahead (git-1197); default 40
	NSNumber* IP_FACTOR;				// limited to (-3.0, 3.0); default 1.4 // !!!!!!!!!!!!!!!!!!!!!!
	NSNumber* PB_FACTOR;				// limited to (-3.0, 3.0); default 1.3 // !!!!!!!!!!!!!!!!!!!!!!
	
	NSNumber* WEIGHTP;					// x264; --weightp; 1:Disabled, 2:Weighted refs, 3:Weighted refs + Duplicates
	NSNumber* TOPFIELDFIRST;			// top field first; default 0 (=Progressive or bottom field first)
	NSNumber* LOSSLESS;					// x264; same as "--qp 0"; 1:On, 1:Off; default is 0
	NSNumber* BD_TUNE;					// x264; --nal-hrd vbr --b-pyramid strict --slices 4 
	
	NSNumber* USEASPECTRATIO;			// Embed container level Pixel Aspect Ratio
	NSNumber* USECLEANAPERTURE;			// Embed container level Clean Aperture
	NSNumber* TAGASPECTRATIO;			// Preset Tag for Pixel Aspect Ratio
	NSNumber* TAGCLEANAPERTURE;			// Preset Tag for Clean Aperture
	
	NSNumber*	HSPACING;               /* Horizontal Spacing */
	NSNumber*	VSPACING;               /* Vertical Spacing */
	NSNumber*	CLEANAPERTUREWIDTHN;    /* width of clean aperture, numerator, denominator */
	NSNumber*	CLEANAPERTUREWIDTHD;
	NSNumber*	CLEANAPERTUREHEIGHTN;   /* height of clean aperture, numerator, denominator*/
	NSNumber*	CLEANAPERTUREHEIGHTD;
	NSNumber*	HORIZOFFN;              /* horizontal offset of clean aperture center minus (width-1)/2, numerator, denominator */
	NSNumber*	HORIZOFFD;
	NSNumber*	VERTOFFN;               /* vertical offset of clean aperture center minus (height-1)/2, numerator, denominator */
	NSNumber*	VERTOFFD;
	
	NSNumber* OVERRIDECRFQSCALE;		/* override quality slider setting in application for CRF/QSCALE */
	NSNumber* USERCRFQSCALE;			/* user specified qscale value; default 23 */
	NSNumber* OVERRIDEQMIN;				/* override quality slider setting in application for ABR */
	NSNumber* USERQMIN;					/* user specified qmin value; default 3 */
	
	NSNumber* FAKEINTERLACED;			// x264; --fake-interlaced; default 0 (=off)
	
	// ====================================
}

- (void)resetToParams:(params*)inParams;
- (params*)currentParams;

- (void)resetToDefaultKeeping:(BOOL)keepIt;
- (void)resetToiPodKeeping:(BOOL)keepIt;
- (void)resetToDefaultTunedKeeping:(BOOL)keepIt;
- (void)resetToiPodTunedKeeping:(BOOL)keepIt;

//- (void)resetToDefault;
//- (void)resetToiPod;
//- (void)resetToDefaultTuned;
//- (void)resetToiPodTuned;
- (void)resetTox264Preset;

- (BOOL)isAvc1;
- (BOOL)isMp4v;
- (BOOL)isXvid;
- (BOOL)isAvc1OrMp4v;
- (BOOL)isMp4vOrXvid;
- (BOOL)isXvidOrAvc1;
- (BOOL)isNA;

- (BOOL)isCoreVFReady;

@end

#endif // UIModel_h
