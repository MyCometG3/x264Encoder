/*
 *  x264Encoder.c
 *  x264Encoder
 *
 *  Created by Takashi Mochizuki on 07/02/18. (mochi at da2.so-net.ne.jp)
 *  Copyright 2007-2010 MyCometG3. All rights reserved.
 *

This file is part of x264Encoder.

    x264Encoder is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    x264Encoder is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with x264Encoder; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 */

#include "x264EncoderVersion.h"
#include "x264EncoderUtil.h"

#define kNibName "Settings"
#define kMaxSourceFrame 128			/* So much encoder delay... */
#define kBundleIdentifier "com.MyCometG3.x264Encoder"
#define kDispatchFile "x264EncoderDispatch.h"
#define kCodecID CODEC_ID_H264
#define kCodec_FormatType "avc1"	/* not 'avc1' but "avc1" here */
#define kCodec_cType 'avc1'			/* OSType */
#define kExtraDataTag 'avcC'		/* MPEG4=esds, x264=avcC */
#define REMOVELOG 1					/* Should be 1 */
#define WRITELOG 0					/* MPEG4=1, x264=0, xvid=1 */
#define MPEG4	0					/* MPEG4=1, x264=0, xvid=0 */
#define XVID	0					/* MPEG4=0, x264=0, xvid=1 */
#define TESTVFR 1					/* MPEG4=0, x264=1, xvid=0 */ /* test vfr (variable frame rate) support */
#define X264	1					/* MPEG4=0, x264=1, xvid=0 */
#define NATIVERECT 0				/* default u/v plane does not have correct alignment */
#define USECoreVF 1					/* CoreVF Framework support */
#define STRICTFLAG 0				/* Use strict flags on each samples */

#define REV_ENDIAN_BIG	1			/* params struct version; 0:NativeEndian, positive:BigEndian */
#define REV_PSYRD 2					/* params struct version; with psyrd, and without FLAG2_TRELLIS_QUANT */
#define REV_MAXQDIFF 3				/* params struct version; with MAXQDIFF and CHROMAOFFSET */
#define REV_COREVF 4				/* params struct version; with CoreVF framework support; 1.0.0 */
#define REV_SUBSAMPLE 5				/* params struct version; chroma subsampling mode support; 1.0.1 */
#define REV_X264PROFILE 6			/* params struct version; new x264 profile support; 1.1.0 */
#define REV_MBTREE 7				/* params struct version; new x264 mbtree support; 1.1.2 */
#define REV_WEIGHTP 8				/* params struct version; with weightp; 1.1.8 */
#define REV_FLAG2_MBTREE 9			/* params struct version; libav20774 support FLAG2_MBTREE natively; 1.1.10 */
#define REV_INTERLACED 10			/* params struct version; interlaced,lossless,aspect,cleanaperture; 1.2.0 */
#define REV_FLAG2_PSY 11			/* params struct version; libav22670 supports FLAG2_PSY, FLAG2_SSIM natively; 1.2.3 */
#define REV_LEVELFREE 12			/* params struct version; support all h.264 level value; 1.2.7 */
#define REV_IGNOREQSLIDER 13		/* params struct version; user can ignore quality slider; 1.2.8 */
#define REV_FAKEINTERLACED 14		/* params struct version; new --fake-interlaced support; 1.2.9 */
#define REV_X264OPENGOP 15			/* params struct version; support open gop; 1.2.13 */

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

#pragma mark -
#pragma mark Struct definitions


typedef struct {
	// 
	Boolean LOG_INFO;
	Boolean LOG_DEBUG;
	Boolean LOG_STATS;
	Boolean REV_STRUCT;				// 0:native-endian, 1:big-endian, 2:(0.9.0~).
	
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
	int QCOMPRESS;					// limited to (50, 80) as (0.50, 0.80)
	int NOISE_REDUCTION;			// limited to (0, 1000)
	int RC_MAXRATE;					// limited to (0, 245760); Kbps
		
	char REFS;						// limited to (1, 13); default 1
	char ME_RANGE;					// limited to (4, 64); default 16
	char MAX_BFRAMES;				// limited to (1, 13); default 1
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
	char LUMI_MASKING;				// xvid; lumi_masking; 0:off, 1:on; default 0
	
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
	
	UInt32	hSpacing;				/* Horizontal Spacing */
	UInt32	vSpacing;				/* Vertical Spacing */
	UInt32	cleanApertureWidthN;	/* width of clean aperture, numerator, denominator */
	UInt32	cleanApertureWidthD;
	UInt32	cleanApertureHeightN;   /* height of clean aperture, numerator, denominator*/
	UInt32	cleanApertureHeightD;
	SInt32	horizOffN;				/* horizontal offset of clean aperture center minus (width-1)/2, numerator, denominator */
	UInt32	horizOffD;
	SInt32	vertOffN;				/* vertical offset of clean aperture center minus (height-1)/2, numerator, denominator */
	UInt32	vertOffD;
	
	char OVERRIDECRFQSCALE;			/* override quality slider setting in application for CRF/QSCALE */
	char USERCRFQSCALE;				/* user specified qscale value; default 23 */
	char OVERRIDEQMIN;				/* override quality slider setting in application for ABR */
	char USERQMIN;					/* user specified qmin value; default 4 */
	
	char FAKEINTERLACED;			// x264; --fake-interlaced; default 0 (=off)
	char reservedChar2;				// 
	char reservedChar3;				// 
	char reservedChar4;				// 
} params;

// Additional parameters which are not supported in AVCodecContext struct
typedef struct {
	char* log_file_path;
	char CQM_PRESET;
	char NO_DCT_DECIMATE;
	char X264PRESET;
	char X264TUNE;
	char TOPFIELDFIRST;
	char BD_TUNE;
	int TIMESCALE;
	char FAKEINTERLACED;
} params_opaque;

typedef struct {
	ComponentInstance				self;
	ComponentInstance				target;
	
	ICMCompressorSessionRef 		session; // NOTE: we do not need to retain or release this
	ICMCompressionSessionOptionsRef	sessionOptions;
	
	Boolean meetsErr;
	
	// ICMCompressorFrame set
	CFMutableArrayRef array_source_frame;
	
	// 
	params params;
	ImageDescriptionHandle imageDescription;
	params_opaque params_opaque;
	
	//
	int width, height, widthRoundedUp, heightRoundedUp;
	
	// 
	Boolean use_B_frame;
	Boolean only_I_frame;
	Boolean add_gamma;
	Boolean add_nclc;
	Boolean add_uuid;				// Special UUID for iPod Video VGA H.264
	
	int frame_count;
	int delayCount;
	
	// time calculation
	CFDateRef initialDate;
	CFDateRef passDate;
	
	// Multipass support
	int ICM_passcount;
	Boolean emit_frame;
	Boolean use3rdpass;
	Boolean fakeMultipass;
	
	// libavcodec
	AVCodec* codec;
	AVCodecContext* codecCont;
	
	Boolean codec_is_opened;
	Boolean codec_open_failed;
	Boolean extraDataReady;
	
	FILE* logfile;
	
	int64_t next_pts_value;
	int64_t	video_size;
	
	// Encoded frame buffer
	UInt8* encode_buffer;
	int encode_buffer_size;
	int frame_size;
	
	// YUV Source frame buffer
	AVFrame* frame;
	UInt8* planer_buffer;
	
	// Chroma subsampling mode
	int subsampling;
	
#if USECoreVF
	int					filterPreset;
	CFStringRef			filterString;
	CVF_Context*		cvfCont;
	CVF_Video*			cvfInVideo;
	CVF_Video*			cvfOutVideo;
	CVF_Frame*			cvfInFrame;
	CVF_Frame*			cvfOutFrame;
	CFMutableArrayRef	cvfSourceArray;
#endif
} lavcEncoderGlobalRecord, *lavcEncoderGlobals;


#pragma mark -
#pragma mark Component requirements


// Setup required for ComponentDispatchHelper.c
#define IMAGECODEC_BASENAME() 		lavcEncoder_
#define IMAGECODEC_GLOBALS() 		lavcEncoderGlobals storage

#define CALLCOMPONENT_BASENAME()	IMAGECODEC_BASENAME()
#define	CALLCOMPONENT_GLOBALS()		IMAGECODEC_GLOBALS()

#define QT_BASENAME()				CALLCOMPONENT_BASENAME()
#define QT_GLOBALS()				CALLCOMPONENT_GLOBALS()

#define COMPONENT_UPP_PREFIX()		uppImageCodec
#define COMPONENT_DISPATCH_FILE		kDispatchFile
#define COMPONENT_SELECT_PREFIX()  	kImageCodec


// Include Component Dispatch Macros and others
#include <CoreServices/Components.k.h>
#include <QuickTime/ImageCodec.k.h>
#include <QuickTime/ImageCompression.k.h>
#include <QuickTime/ComponentDispatchHelper.c>


#pragma mark -
#pragma mark UIbundle Prototypes


typedef void (*InitializeNibProc)();
typedef void (*SetFourCCProc)(CFStringRef fourCC);
typedef void (*SetBundleIDProc)(CFStringRef newBundleID);
typedef params* (*ShowDialogProc)(params* inParams);
typedef void (*ReleaseNibProc)();
typedef void (*InitializeCocoaProc)();
InitializeNibProc uiInitializeNib;
SetFourCCProc uiSetFourCC;
SetBundleIDProc uiSetBundleID;
ShowDialogProc uiShowDialog;
ReleaseNibProc uiReleaseNib;
InitializeCocoaProc uiInitializeCocoa;


#pragma mark -
#pragma mark Prototypes


static OSStatus setup_codecCont(lavcEncoderGlobalRecord *glob);
static OSStatus open_libAV(lavcEncoderGlobalRecord *glob);
static OSStatus process_libAV(lavcEncoderGlobalRecord *glob, ICMCompressorSourceFrameRef source_frame);
static OSStatus emitFrameData(lavcEncoderGlobalRecord *glob);

static OSStatus createPixelBufferAttributesDictionary( lavcEncoderGlobalRecord *glob, 
	const OSType *pixelFormatList, int pixelFormatCount, CFMutableDictionaryRef *pixelBufferAttributesOut );

static ICMCompressorSourceFrameRef getSourceFrameForEncodedFrame(lavcEncoderGlobalRecord *glob);
static Boolean removeSourceFrame(lavcEncoderGlobalRecord *glob, ICMCompressorSourceFrameRef source_frame);
static Boolean doesQueueContainEarlierDisplayNumbers( lavcEncoderGlobalRecord *glob, long display_number );
static Boolean removeSourceFrameForEncodedFrame( lavcEncoderGlobalRecord *glob );

static Handle avcCExtension( lavcEncoderGlobalRecord *glob );
static Handle esdsExtension( lavcEncoderGlobalRecord *glob );
static OSStatus prepareImageDescriptionExtensions(lavcEncoderGlobalRecord *glob);

static void logInfo(lavcEncoderGlobalRecord *glob, char* fmt, ...);
static void logDebug(lavcEncoderGlobalRecord *glob, char* fmt, ...);

static OSStatus openCoreVF( lavcEncoderGlobals glob );
static OSStatus closeCoreVF( lavcEncoderGlobals glob );

static void checkValues( lavcEncoderGlobals glob );
static void initValues( lavcEncoderGlobals glob );


#pragma mark -
#pragma mark Standard Component Call handler


// StdComponentCall (Target)							// 2
ComponentResult lavcEncoder_Target(lavcEncoderGlobalRecord *glob, ComponentInstance target)
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	glob->target = target;
	return noErr;
}


// StdComponentCall (Version)							// 4
ComponentResult lavcEncoder_Version(lavcEncoderGlobalRecord *glob)
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	return klavcEncoderVersion;
}


// StdComponentCall (CanDo)								// 5
// ...missing???


// StdComponentCall (Close)								// 6
ComponentResult lavcEncoder_Close(lavcEncoderGlobalRecord *glob, ComponentInstance self)
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	if (glob) {
#if REMOVELOG
		if( glob->params_opaque.log_file_path && strlen(glob->params_opaque.log_file_path) > 0 ) {
			int ret = noErr;
			
			char* path = NULL;
			char* suffix = NULL;
			int size = 0;
			
			ret = remove(glob->params_opaque.log_file_path);
			if( ret && errno != ENOENT ) 
				fprintf(stderr, "(%s) removal failed.(%d)\n", glob->params_opaque.log_file_path, ret);
			
			suffix = ".mbtree";
			size = 1 + strlen(suffix) + strlen(glob->params_opaque.log_file_path);
			if( path = calloc(1, size) ) {
				snprintf(path, size, "%s%s", glob->params_opaque.log_file_path, suffix);
				ret = remove(path);
				if( ret && errno != ENOENT ) 
					fprintf(stderr, "(%s) removal failed.(%d)\n", path, ret);
				free(path);
			}
			
			suffix = ".temp";
			size = 1 + strlen(suffix) + strlen(glob->params_opaque.log_file_path);
			if( path = calloc(1, size) ) {
				snprintf(path, size, "%s%s", glob->params_opaque.log_file_path, suffix);
				ret = remove(path);
				if( ret && errno != ENOENT ) 
					fprintf(stderr, "(%s) removal failed.(%d)\n", path, ret);
				free(path);
			}
			
			suffix = ".mbtree.temp";
			size = 1 + strlen(suffix) + strlen(glob->params_opaque.log_file_path);
			if( path = calloc(1, size) ) {
				snprintf(path, size, "%s%s", glob->params_opaque.log_file_path, suffix);
				ret = remove(path);
				if( ret && errno != ENOENT ) 
					fprintf(stderr, "(%s) removal failed.(%d)\n", path, ret);
				free(path);
			}
			
			// release path string buffer
			free(glob->params_opaque.log_file_path);
			glob->params_opaque.log_file_path = NULL;
		}
#endif
		
		// Clean up
		if( glob->codecCont ) {
			if( glob->codec_is_opened ) {
				avcodec_close( glob->codecCont );
				glob->codec_is_opened = FALSE;
				glob->extraDataReady = FALSE;
			}
			
#if USECoreVF
			if( glob->cvfCont ) closeCoreVF( glob );
#endif
			
			if( glob->encode_buffer ) {
				free( glob->encode_buffer );
				glob->encode_buffer = NULL;
			}
			if( glob->planer_buffer ) {
				av_free( glob->planer_buffer );
				glob->planer_buffer = NULL;
			}
			
			if( glob->frame ) {
				av_free( glob->frame );
				glob->frame = NULL;
			}
			
			av_free( glob->codecCont );
			glob->codecCont = NULL;
		}
		
		// 
		if (glob->array_source_frame) {
			CFArrayRemoveAllValues(glob->array_source_frame);
			CFRelease(glob->array_source_frame);
			glob->array_source_frame = NULL;
		}
		
		if( glob->sessionOptions ) {
			ICMCompressionSessionOptionsRelease( glob->sessionOptions );
			glob->sessionOptions = NULL;
		}
		
		// 
		if( glob->initialDate && glob->frame_count > 1) {
			CFDateRef lastDate = CFDateCreate(kCFAllocatorDefault, CFAbsoluteTimeGetCurrent());
			CFTimeInterval diff = CFDateGetTimeIntervalSinceDate(lastDate, glob->initialDate);
			logInfo(glob, "lavcEncoder: - Total time: %.1f sec.\n", diff);
			CFRelease(lastDate);
			
			CFRelease(glob->initialDate);
			glob->initialDate = NULL;
		}
		
		// Release Component Instance Storage
		free(glob);
	}
	
	// Exit with no error
	return noErr;
}


// StdComponentCall (Open)								// 7
ComponentResult lavcEncoder_Open(lavcEncoderGlobalRecord *glob, ComponentInstance self)
{
//	logDebug(NULL, "lavcEncoder: [%s]\n", __FUNCTION__);
	
	// Create Component Instance Storage
	glob = calloc(sizeof(lavcEncoderGlobalRecord), 1);
	if (!glob)
		return memFullErr;

//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	SetComponentInstanceStorage(self, (Handle)glob);
	
	// Set Initial Instance
	glob->self = self;
	glob->target = self;
	
	//
	glob->ICM_passcount = -1;
	
	//
	initValues(glob);
	
	return noErr;
}


#pragma mark -
#pragma mark Component Call handler


// ComponentCall (GetCodecInfo)							// 0
ComponentResult lavcEncoder_GetCodecInfo(lavcEncoderGlobalRecord *glob, CodecInfo *info)
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	OSErr err = noErr;
	CodecInfo **tempCodecInfo = NULL;

	if (info == NULL) {
		err = paramErr;
	} else {
		err = GetComponentResource((Component)glob->self, codecInfoResourceType, 256, (Handle *)&tempCodecInfo);
		if (err == noErr) {
			*info = **tempCodecInfo;
			DisposeHandle((Handle)tempCodecInfo);
		}
	}

	return err;
}


// ComponentCall (GetMaxCompressionSize)				// 2
ComponentResult lavcEncoder_GetMaxCompressionSize(lavcEncoderGlobalRecord *glob, 
			PixMapHandle src, const Rect *srcRect, short depth, CodecQ quality, long *size)
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	if( !size ) {
		fprintf(stderr, "ERROR: No size location given.\n");
		return paramErr;
	}
	
	*size = (srcRect->right - srcRect->left) * (srcRect->bottom - srcRect->top) * 4;
	
	return noErr;
}


#pragma mark -


// ComponentCall (RequestSettings)						// 11
ComponentResult lavcEncoder_RequestSettings(lavcEncoderGlobalRecord *glob, 
			Handle settings, Rect *rp, ModalFilterUPP filter_proc)
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	// Check for glob-struct update (safty)
	checkValues(glob);
	
	// Prepare UIbundle Reference
	CFBundleRef baseBundleRef = NULL, uiBundleRef = NULL;
	CFURLRef baseBundleURL = NULL, uiBundleURL = NULL;
	
	baseBundleRef = CFBundleGetBundleWithIdentifier(CFSTR(kBundleIdentifier));
	if(baseBundleRef) {
		baseBundleURL = CFBundleCopyPrivateFrameworksURL( baseBundleRef );
		if(baseBundleURL) {
			uiBundleURL = CFURLCreateCopyAppendingPathComponent( kCFAllocatorDefault, baseBundleURL, CFSTR("UIbundle.bundle"), false );
			if(uiBundleURL) {
				uiBundleRef = CFBundleCreate( kCFAllocatorDefault, uiBundleURL );
				CFRelease(uiBundleURL);
				uiBundleURL = NULL;
			}
			CFRelease(baseBundleURL);
			baseBundleURL = NULL;
		}
	}
	
	// Show dialog
	if ( uiBundleRef ) {
		// Get Function pointers
		uiInitializeNib = (InitializeNibProc)CFBundleGetFunctionPointerForName(uiBundleRef, CFSTR("InitializeNib"));
		uiSetFourCC = (SetFourCCProc)CFBundleGetFunctionPointerForName(uiBundleRef, CFSTR("SetFourCC"));
		uiSetBundleID = (SetBundleIDProc)CFBundleGetFunctionPointerForName(uiBundleRef, CFSTR("SetBundleID"));
		uiShowDialog = (ShowDialogProc)CFBundleGetFunctionPointerForName(uiBundleRef, CFSTR("ShowDialog"));
		uiReleaseNib = (ReleaseNibProc)CFBundleGetFunctionPointerForName(uiBundleRef, CFSTR("ReleaseNib"));
		uiInitializeCocoa = (InitializeCocoaProc)CFBundleGetFunctionPointerForName(uiBundleRef, CFSTR("InitializeCocoa"));
		
		// Check if something corrupted
		assert(uiInitializeNib);
		assert(uiSetFourCC);
		assert(uiSetBundleID);
		assert(uiShowDialog);
		assert(uiReleaseNib);
		assert(uiInitializeCocoa);
		
		// Initialize Cocoa
		uiInitializeCocoa();
		
		// Initialize controller
		uiInitializeNib();
		uiSetFourCC(CFSTR(kCodec_FormatType));
		uiSetBundleID(CFSTR(kBundleIdentifier));
		
		// Copy supplied setting into glob
		lavcEncoder_SetSettings(glob, settings);
		
		// Call UIbundle
		params* outParamsPtr = uiShowDialog(&glob->params);
		
		// Get result settings
		if(outParamsPtr) 
			glob->params = *outParamsPtr;
		
		// Release controller
		uiReleaseNib();
		
		// Release UIBundle CFBundleRef
		CFRelease( uiBundleRef );
	} else {
		AudioServicesPlayAlertSound(kUserPreferredAlert);
		fprintf(stderr, "ERROR: Failed to load UIbundle.");
	}
	
	// 
	lavcEncoder_GetSettings(glob, settings);
	
	return noErr;
} // ComponentCall (RequestSettings)


// ComponentCall (GetSettings)							// 12
ComponentResult lavcEncoder_GetSettings(lavcEncoderGlobalRecord *glob, Handle settings)
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	// Check for glob-struct update
	checkValues(glob);
	
	SetHandleSize(settings, sizeof(glob->params));
	memcpy(*settings, &glob->params, sizeof(glob->params));		// write out
	
	// Endian handler
	params *p = (params*)*settings;
	p->REV_STRUCT		= REV_X264OPENGOP;	// 1.2.13 or later.
	
	p->SC_THRESHOLD		= EndianS32_NtoB(p->SC_THRESHOLD);
	p->QCOMPRESS		= EndianS32_NtoB(p->QCOMPRESS);
	p->NOISE_REDUCTION	= EndianS32_NtoB(p->NOISE_REDUCTION);
	p->RC_MAXRATE		= EndianS32_NtoB(p->RC_MAXRATE);
	p->RC_BUFSIZE		= EndianS32_NtoB(p->RC_BUFSIZE);
	p->AQ_STRENGTH		= EndianS32_NtoB(p->AQ_STRENGTH);
	p->PSYRD_RDO		= EndianS32_NtoB(p->PSYRD_RDO);
	p->PSYRD_TRELLIS	= EndianS32_NtoB(p->PSYRD_TRELLIS);
	p->RC_LOOKAHEAD		= EndianS32_NtoB(p->RC_LOOKAHEAD);
	p->IP_FACTOR		= EndianS32_NtoB(p->IP_FACTOR);
	p->PB_FACTOR		= EndianS32_NtoB(p->PB_FACTOR);
	
	p->hSpacing			= EndianU32_NtoB(p->hSpacing);
	p->vSpacing			= EndianU32_NtoB(p->vSpacing);
	p->cleanApertureWidthN	= EndianU32_NtoB(p->cleanApertureWidthN);
	p->cleanApertureWidthD	= EndianU32_NtoB(p->cleanApertureWidthD);
	p->cleanApertureHeightN	= EndianU32_NtoB(p->cleanApertureHeightN);
	p->cleanApertureHeightD	= EndianU32_NtoB(p->cleanApertureHeightD);
	p->horizOffN		= EndianS32_NtoB(p->horizOffN);
	p->horizOffD		= EndianU32_NtoB(p->horizOffD);
	p->vertOffN			= EndianS32_NtoB(p->vertOffN);
	p->vertOffD			= EndianU32_NtoB(p->vertOffD);
	
	return noErr;
}


// ComponentCall (SetSettings)							// 13
ComponentResult lavcEncoder_SetSettings(lavcEncoderGlobalRecord *glob, Handle settings)
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	if( sizeof(glob->params) < GetHandleSize(settings) ) return paramErr;
	memcpy(&glob->params, *settings, GetHandleSize(settings));		// read in
	
	// Endian Handler
	params *p = &glob->params;
	if( !(p->QCOMPRESS <= 100 && p->QCOMPRESS >= 0 )	// Endian check
	) {
		p->SC_THRESHOLD		= EndianS32_BtoN(p->SC_THRESHOLD);
		p->QCOMPRESS		= EndianS32_BtoN(p->QCOMPRESS);
		p->NOISE_REDUCTION	= EndianS32_BtoN(p->NOISE_REDUCTION);
		p->RC_MAXRATE		= EndianS32_BtoN(p->RC_MAXRATE);
		p->RC_BUFSIZE		= EndianS32_BtoN(p->RC_BUFSIZE);
		p->AQ_STRENGTH		= EndianS32_BtoN(p->AQ_STRENGTH);
		p->PSYRD_RDO		= EndianS32_BtoN(p->PSYRD_RDO);
		p->PSYRD_TRELLIS	= EndianS32_BtoN(p->PSYRD_TRELLIS);
		p->RC_LOOKAHEAD		= EndianS32_BtoN(p->RC_LOOKAHEAD);
		p->IP_FACTOR		= EndianS32_BtoN(p->IP_FACTOR);
		p->PB_FACTOR		= EndianS32_BtoN(p->PB_FACTOR);
		
		p->hSpacing			= EndianU32_BtoN(p->hSpacing);
		p->vSpacing			= EndianU32_BtoN(p->vSpacing);
		p->cleanApertureWidthN	= EndianU32_BtoN(p->cleanApertureWidthN);
		p->cleanApertureWidthD	= EndianU32_BtoN(p->cleanApertureWidthD);
		p->cleanApertureHeightN	= EndianU32_BtoN(p->cleanApertureHeightN);
		p->cleanApertureHeightD	= EndianU32_BtoN(p->cleanApertureHeightD);
		p->horizOffN		= EndianS32_BtoN(p->horizOffN);
		p->horizOffD		= EndianU32_BtoN(p->horizOffD);
		p->vertOffN			= EndianS32_BtoN(p->vertOffN);
		p->vertOffD			= EndianU32_BtoN(p->vertOffD);
	}
	
	if(p->REV_STRUCT < REV_PSYRD) { /* 2: 0.9.0 or later */
		p->PSYRD_RDO = 10;
		p->PSYRD_TRELLIS = 0;
		p->AQ_MODE = ((p->AQ_MODE == 3) ? 1 : 0);
	}
	if(p->REV_STRUCT < REV_MAXQDIFF) { /* 3: 0.9.5 or later */
#if X264
		p->MAX_QDIFF = 4;
#else
		p->MAX_QDIFF = 3;
#endif
		p->CHROMAOFFSET = 0;
	}
	if(p->REV_STRUCT < REV_COREVF) { /* 4: 1.0.0 or later */
		p->FILTERPRESET = 0;
	}
	if(p->REV_STRUCT < REV_SUBSAMPLE) { /* 5: 1.0.1 or later */
		p->SUBSAMPLING = 0;
	}
	if(p->REV_STRUCT < REV_X264PROFILE) { /* 6: 1.1.0 or later */
		p->X264PROFILE = 3;
		p->X264PRESET = 0;
		p->X264TUNE = 0;
	}
	if(p->REV_STRUCT < REV_MBTREE) { /* 7: 1.1.2 or later */
#if X264
		p->MBTREE = 1;
		p->PSY = 1;
		p->RC_LOOKAHEAD = 40;
		p->IP_FACTOR = 140;
		p->PB_FACTOR = 130;
#endif
#if MPEG4
		p->MBTREE = 0;
		p->PSY = 0;
		p->RC_LOOKAHEAD = 0;
		p->IP_FACTOR = 125;
		p->PB_FACTOR = 125;
#endif
#if XVID
		p->MBTREE = 0;
		p->PSY = 0;
		p->RC_LOOKAHEAD = 0;
		p->IP_FACTOR = 125;
		p->PB_FACTOR = 150;
#endif
	}
	if(p->REV_STRUCT < REV_WEIGHTP) { /* 8: 1.1.8 or later */
#if X264
		p->WEIGHTP = 3;
#else
		p->WEIGHTP = 1;
#endif
	}
	if(p->REV_STRUCT < REV_FLAG2_MBTREE) { /* 9: 1.1.10 or later */
#if 1
		p->FLAG2_MBTREE = p->MBTREE;	// Copy old param to new param (used under 1.1.2-1.1.9)
#else
  #if X264
		p->FLAG2_MBTREE = 1;
  #else
		p->FLAG2_MBTREE = 0;
  #endif
#endif
	}
	if(p->REV_STRUCT < REV_INTERLACED) { /* 10: 1.2.0 or later */
		p->FLAG_INTERLACED_DCT = 0;
		p->TOPFIELDFIRST = 0;
		p->LOSSLESS = 0;
		p->USEASPECTRATIO = 0;
		p->USECLEANAPERTURE = 0;
		p->TAGASPECTRATIO = 0;
		p->TAGCLEANAPERTURE = 0;
		p->hSpacing			= 1;		// Square pixel
		p->vSpacing			= 1;		// Square pixel
		p->cleanApertureWidthN	= 0;	// undefined
		p->cleanApertureWidthD	= 1;	// equal denomiter
		p->cleanApertureHeightN	= 0;	// undefined
		p->cleanApertureHeightD	= 1;	// equal denomiter
		p->horizOffN		= 0;		// no offset
		p->horizOffD		= 1;		// equal denomiter
		p->vertOffN			= 0;		// no offset
		p->vertOffD			= 1;		// equal denomiter
	}
	if(p->REV_STRUCT < REV_FLAG2_PSY) { /* 11: 1.2.3 or later */
		p->FLAG2_PSY = p->PSY;			// Copy old param to new param (used under 1.1.2-1.2.2)
		p->BD_TUNE = 0;
	}
	if(p->REV_STRUCT < REV_LEVELFREE) { /* 12: 1.2.7 or later */
		switch( p->LEVEL ) {
		case 1:
			p->LEVEL = 13;
			break;
		case 2:
			p->LEVEL = 21;
			break;
		case 3:
			p->LEVEL = 30;
			break;
		case 4:
			p->LEVEL = 31;
			break;
		case 5:
			p->LEVEL = 41;
			break;
		case 6:
			p->LEVEL = 51;
			break;
		case 9:
		default:
			p->LEVEL = 9;
			break;
		}
	}
	if(p->REV_STRUCT < REV_IGNOREQSLIDER) { /* 13: 1.2.8 or later */
#if X264
		p->OVERRIDECRFQSCALE = 0;
		p->USERCRFQSCALE = 23;
		p->OVERRIDEQMIN = 1;
		p->USERQMIN = 0;	// x264 r1795 changed default qmin from 10 to 0
#endif
#if MPEG4
		p->OVERRIDECRFQSCALE = 0;
		p->USERCRFQSCALE = 2;
		p->OVERRIDEQMIN = 0;
		p->USERQMIN = 2;
#endif
#if XVID
		p->OVERRIDECRFQSCALE = 0;
		p->USERCRFQSCALE = 2;
		p->OVERRIDEQMIN = 0;
		p->USERQMIN = 2;
#endif
	}
	if(p->REV_STRUCT < REV_FAKEINTERLACED) { /* 14: 1.2.9 or later */
		p->FAKEINTERLACED = 0;
	}
#if X264
	if(p->REV_STRUCT < REV_X264OPENGOP) { /* 15: 1.2.13 or later*/
		p->FLAG_CLOSED_GOP = TRUE;
	}
#endif
	
	// Check for glob-struct update
	checkValues(glob);
	
#if USECoreVF
	CFStringRef appID = CFSTR("com.MyCometG3.CoreVF");
	CFPreferencesAppSynchronize(appID);
#endif
	return noErr;
}


#pragma mark -


// ComponentCall (PrepareToCompressFrames)				// 55
ComponentResult lavcEncoder_PrepareToCompressFrames(lavcEncoderGlobalRecord *glob,
													ICMCompressorSessionRef session,
													ICMCompressionSessionOptionsRef session_options,
													ImageDescriptionHandle image_description,
													void *reserved,
													CFDictionaryRef *compressor_pixel_buffer_attributes_out)
{
	logDebug(glob, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	ComponentResult err = noErr;
	
	{
		glob->meetsErr = FALSE;
		
		// ICMCompressionSession and SessionOptions handling
		glob->session = session;
		
		// 
		if( glob->sessionOptions )
			ICMCompressionSessionOptionsRelease( glob->sessionOptions );
		glob->sessionOptions = session_options;
		ICMCompressionSessionOptionsRetain(glob->sessionOptions);
	
		// Create source frames array
		if (glob->array_source_frame) {
			CFArrayRemoveAllValues(glob->array_source_frame);
			CFRelease(glob->array_source_frame);
		}
		glob->array_source_frame = CFArrayCreateMutable(NULL, kMaxSourceFrame, &kCFTypeArrayCallBacks);
		// This array_source_frame will be released at Close()
		
		/* ==================================== */
		
		// Reserve ImageDescriptionHandle to be able to modify later 
		glob->imageDescription = image_description;
		
		// Update FourCC code as preffered one
		(*image_description)->cType = kCodec_cType;
		
		// Prepare rectangles prior to build attributes dictionary
		glob->width = (*image_description)->width;
		glob->height = (*image_description)->height;
		
		glob->widthRoundedUp = roundUpToMultipleOf16( glob->width );	// Macroblock width is 16
		glob->heightRoundedUp = roundUpToMultipleOf16( glob->height );	// Macroblock height is 16
	
		// Prepare two flags prior to build attributes dictionary
		glob->add_gamma = glob->params.GAMMA;
		glob->add_nclc = ( glob->params.NCLC > 2 );
		
#if X264
		// Install uuid atom or not
		glob->add_uuid = glob->params.EMBEDUUID;
#endif
		
		/* ==================================== */
		
		// Create a pixel buffer attributes dictionary
		OSType pixelFormatList[] = { k422YpCbCr8PixelFormat };
		CFMutableDictionaryRef compressorPixelBufferAttributes = NULL;
		
		err = createPixelBufferAttributesDictionary( glob, 
				pixelFormatList, sizeof(pixelFormatList) / sizeof(OSType),
				&compressorPixelBufferAttributes );
		if( err || !compressorPixelBufferAttributes ) {
			fprintf(stderr, "ERROR: createPixelBufferAttributesDictionary() failed.\n");
			err = paramErr;
			goto bail;
		}
		*compressor_pixel_buffer_attributes_out = compressorPixelBufferAttributes;
		// Returned CFDictionaryRef will be released by ICM later
	
		/* ==================================== */
		
		// Setup ImageDescription Extensions like esds
		err = prepareImageDescriptionExtensions(glob);
		if(err) goto bail;
	}
	
	/* ================================================================================= */
	
	if( glob->ICM_passcount == -1 ) {
		// Benchmark
		glob->initialDate = CFDateCreate(kCFAllocatorDefault, CFAbsoluteTimeGetCurrent());
		
		/* ==================================== */
		
		// Initialize libavcodec
		avcodec_init();
		avcodec_register_all();
		
		// Check codec
		glob->codec = avcodec_find_encoder(kCodecID);
		if( !glob->codec ) {
			fprintf(stderr, "ERROR: Codec not found\n");
			err = paramErr;
			goto bail;
		}
		
		/* ==================================== */
		
		// Prepare log file path
		glob->logfile = NULL;
		FSRef temp_folder;
		
		err = FSFindFolder( kOnAppropriateDisk, kTemporaryFolderType, TRUE, &temp_folder );
		if( !err ) {
			CFURLRef temp_folder_URL = NULL;
			CFStringRef temp_folder_path = NULL;
			CFStringRef log_file_path_raw = NULL;
			
			srandom( time(NULL) );
			temp_folder_URL = CFURLCreateFromFSRef( NULL, &temp_folder );
			temp_folder_path = CFURLCopyFileSystemPath( temp_folder_URL, kCFURLPOSIXPathStyle );
			log_file_path_raw = CFStringCreateWithFormat( NULL, NULL, 
										CFSTR("%@/logfile-%d-%ld"), temp_folder_path,getpid() ,random() );
			
			// Copy to path string buffer
			Boolean ret = FALSE;
			if(glob->params_opaque.log_file_path == NULL) {
				int newSize = 1 + CFStringGetMaximumSizeOfFileSystemRepresentation(log_file_path_raw);
				char* newPtr = calloc( newSize, 1 );
				if( newPtr ) {
					glob->params_opaque.log_file_path = newPtr;
					ret = CFStringGetFileSystemRepresentation(log_file_path_raw,
						glob->params_opaque.log_file_path, newSize );
				}
			}
			
			CFRelease(temp_folder_URL);
			CFRelease(temp_folder_path);
			CFRelease(log_file_path_raw);
			
			if( !ret ) {
				fprintf(stderr, "ERROR: Getting log file path failed.\n");
				err = paramErr;
				goto bail;
			}
		}
	}
	
	/* ================================================================================= */
		
	// Prepare codec context and buffers
	if( !glob->codecCont ) {
		// Prepare codec context
		av_log_set_level(AV_LOG_QUIET);
		glob->codecCont = avcodec_alloc_context();
		if( !glob->codecCont ) {
			fprintf(stderr, "ERROR: Allocating codec context failed.\n");
			err = memFullErr;
			goto bail;
		}
		
		/* ==================================== */
		
		logDebug(glob, "lavcEncoder: codecCont->{flags, flags2} = {0x%010x, 0x%010x}\n"
			, glob->codecCont->flags, glob->codecCont->flags2);
		
		// codec is opend at EncodeFrame()
		glob->codec_is_opened = FALSE;
		glob->codec_open_failed = FALSE;
		glob->extraDataReady = FALSE;
		
		// Write frame out or not. This field is updated at ProcessBetweenPasses().
		glob->emit_frame = TRUE;
		
		// Additional encoding pass support (x264)
		glob->use3rdpass = glob->params.USE3RDPASS;
		
		// pts counter for AV_NOPTS_VALUE
		glob->next_pts_value = 0;
		
		// Total bytes of all encoded frame
		glob->video_size = 0;
		
		// Total encoded frames.
		glob->frame_count = 0;
		
		// delayed frame count.
		glob->delayCount = 0;
		
		// chroma subsampling
		glob->subsampling = glob->params.SUBSAMPLING;
		if(glob->subsampling < 0 && CVF_Context_Create == NULL) AudioServicesPlayAlertSound(kUserPreferredAlert); //glob->subsampling = 0;
		
#if USECoreVF
		// CoreVF filter support
		glob->filterPreset = glob->params.FILTERPRESET;
		if(glob->subsampling < 0 && glob->filterPreset == 0) AudioServicesPlayAlertSound(kUserPreferredAlert); //glob->subsampling = 0;
#endif
		
		/* ==================================== */
		
		// Fill a few codecContext prior to build AVFrame
		glob->codecCont->width = (glob->width+1) & ~1;		// Use even-value-width in libavcodec 
		glob->codecCont->height = (glob->height+1) & ~1;	// Use even-value-height in libavcodec
		
		glob->codecCont->pix_fmt = PIX_FMT_YUV420P;
		
		// Encoded AVFrame buffer
		int PixelCountRU = glob->widthRoundedUp * glob->heightRoundedUp;
		glob->encode_buffer_size = 4 * PixelCountRU;
		glob->encode_buffer = calloc(1, glob->encode_buffer_size);
		if( !glob->encode_buffer ) {
			fprintf(stderr, "ERROR: Allocating coded frame buffer failed.\n");
			err = memFullErr;
			goto bail;
		}
		
		/* ==================================== */
		
		// Planer YUV Source AVFrame*
		glob->frame = avcodec_alloc_frame();
		if( !glob->frame ) {
			fprintf(stderr, "ERROR: avcodec_alloc_frame failed.\n");
			err = memFullErr;
			goto bail;
		}
		
		// YUV Source AVFrame buffer
#if NATIVERECT
		if( glob->codecCont->get_buffer(glob->codecCont,glob->frame) ) {
			fprintf(stderr, "ERROR: avcodec_default_get_buffer() failed.\n");
			err = memFullErr;
			goto bail;
		}
#else
		glob->planer_buffer = av_malloc(avpicture_get_size(
						PIX_FMT_YUV420P, glob->widthRoundedUp, glob->heightRoundedUp) );
		if( !glob->planer_buffer ) {
			fprintf(stderr, "ERROR: av_malloc failed.\n");
			err = memFullErr;
			goto bail;
		}
		avpicture_fill( (AVPicture*)glob->frame, glob->planer_buffer,
						PIX_FMT_YUV420P, glob->widthRoundedUp, glob->heightRoundedUp );
#endif
		
		//
#if USECoreVF
		if( glob->filterPreset ) err = openCoreVF(glob);
		if( err ) {
			fprintf(stderr, "ERROR: Failed to initialize CoreVF engine.\n");
			goto bail;
		}
#endif
	}
	
	/* ================================================================================= */
	
	// Setup encoder parameters in codecContext
	if (setup_codecCont(glob)) {
		err = paramErr;
		goto bail;
	}
	
	/* ================================================================================= */
	
	{
#if X264
		// Check limitation for iPod/iTunes support
		if(glob->add_uuid && glob->params.CODER_TYPE > 1){		// CAVLC only; no CABAC
			fprintf(stderr, "ERROR: Unable to add UUID; Incompatible CABAC detected. (CAVLC required)\n");
			AudioServicesPlayAlertSound(kUserPreferredAlert); glob->add_uuid = FALSE;
		}
		if(glob->add_uuid && glob->params.LEVEL > 30) {			// iPod requires H.264/AVC Level 3.0 or below
			fprintf(stderr, "ERROR: Unable to add UUID; Incompatible Level detected. (Level 3.0 max)\n");
			AudioServicesPlayAlertSound(kUserPreferredAlert); glob->add_uuid = FALSE;
		}
		if(glob->add_uuid && glob->codecCont->rc_buffer_size > 2048*1024) {	// Kbit; iPod Video VBV buffer size is 2Mbit
			fprintf(stderr, "ERROR: Unable to add UUID; Incompatible RC_BUFSIZE detected. (%d Kbps)\n"
				, glob->codecCont->rc_buffer_size/1024);
			AudioServicesPlayAlertSound(kUserPreferredAlert); glob->add_uuid = FALSE;
		}
		if(glob->add_uuid && glob->use_B_frame) {				// No B frame
			fprintf(stderr, "ERROR: Unable to add UUID; Incompatible frame reordering detected. (i.e. B-frame)\n");
			AudioServicesPlayAlertSound(kUserPreferredAlert); glob->add_uuid = FALSE;
		}
		if(glob->add_uuid && (glob->width + glob->height)/16 > 70) {	// Max macro block count is 70
			fprintf(stderr, "ERROR: Unable to add UUID; Incompatible large frame detected. (%d,%d)\n"
				, glob->width, glob->height);
			AudioServicesPlayAlertSound(kUserPreferredAlert); glob->add_uuid = FALSE;
		}
		if(glob->add_uuid && glob->codecCont->bit_rate > 1536*1024) {	// Max bitrate is 1500kbps
			fprintf(stderr, "ERROR: Unable to add UUID; Incompatible bit_rate detected. (%d Kbps)\n"
				, glob->codecCont->bit_rate/1024);
			AudioServicesPlayAlertSound(kUserPreferredAlert); glob->add_uuid = FALSE;
		}
#endif
#if MPEG4
		// Adjust incompatible flags
		if( glob->codecCont->trellis == 0 ) 
			glob->codecCont->flags &= ~ CODEC_FLAG_CBP_RD;
		if( glob->codecCont->mb_decision != FF_MB_DECISION_RD )
			glob->codecCont->flags &= ~ CODEC_FLAG_QP_RD;
		
		// Special parameters
		if( glob->codecCont->mb_decision == FF_MB_DECISION_RD ) {
			glob->codecCont->me_cmp = 0;								// 0:FF_CMP_SAD 2:FF_CMP_SATD 6:FF_CMP_RD
			glob->codecCont->me_sub_cmp = 6;							// 0:FF_CMP_SAD 2:FF_CMP_SATD 6:FF_CMP_RD
			glob->codecCont->me_pre_cmp = 0;							// 0:FF_CMP_SAD 2:FF_CMP_SATD 6:FF_CMP_RD
			glob->codecCont->mb_cmp = 6;								// 0:FF_CMP_SAD 2:FF_CMP_SATD 6:FF_CMP_RD
			glob->codecCont->dia_size = 512 + 16;						// hex_search 16; motion_est_template.c:diamond_search()
			glob->codecCont->last_predictor_count = 4;					// motion_est_template.c:epzs_motion_search_internal()
//			glob->codecCont->pre_me = 2;								// value 1-4
//			glob->codecCont->pre_dia_size = 2;							// value 1-4
		}
#endif
	}
		
	logDebug(glob, "lavcEncoder: codecCont->{flags, flags2} = {0x%010x, 0x%010x}\n"
		, glob->codecCont->flags, glob->codecCont->flags2);
	
bail:	
	if(err && !glob->meetsErr) {
		AudioServicesPlayAlertSound(kUserPreferredAlert); glob->meetsErr = TRUE;
	}
	return err;
} // ComponentCall (PrepareToCompressFrames)


// ComponentCall (EncodeFrame)							// 56
ComponentResult lavcEncoder_EncodeFrame(lavcEncoderGlobalRecord *glob, 
			ICMCompressorSourceFrameRef source_frame, UInt32 flags)
{
//	logDebug(glob, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	ComponentResult err = paramErr;
	
	// Check
	if( glob->ICM_passcount == 0 ) {
		glob->frame_count++;
		return noErr;
	}
	
	if( glob->fakeMultipass && !glob->emit_frame ) {
		// Skipping this pass... (QSCALE or CRF is enabled)
		err = noErr;
		goto bail;
	}
	
	{
		TimeValue64 displayTimeStamp, displayDuration;
		TimeScale timeScale;
		ICMCompressorSourceFrameGetDisplayTimeStampAndDuration( source_frame, &displayTimeStamp, &displayDuration, &timeScale, NULL);	
		
		long displayNumber;
		displayNumber = ICMCompressorSourceFrameGetDisplayNumber(source_frame);
		
#if 1
		logDebug(glob, "lavcEncoder: [%08x %s] (%d, %lld, %lld, %d)\n", glob, __FUNCTION__
			, displayNumber, displayTimeStamp, displayDuration, timeScale
		);
#endif
		
		// Update media timescale
		glob->params_opaque.TIMESCALE = timeScale;
	}
	
	/* ================================================================================= */
	
	// Open Codec at encoding first frame
	if( !glob->codec_is_opened ) {
		if(open_libAV(glob)) {
			err = paramErr;
			goto bail;
		}
	}
	
	/* ================================================================================= */
	
	{
		// Get source CVPixelBuffer
		CVPixelBufferRef pixel_buffer = NULL;
		OSType pixel_format_type;
		
		if( !source_frame ) {
			fprintf(stderr, "ERROR: No source_frame\n");
			goto bail;
		}
		
		pixel_buffer = ICMCompressorSourceFrameGetPixelBuffer(source_frame);
		if( !pixel_buffer ) {
			fprintf(stderr, "ERROR: No pixel_buffer\n");
			goto bail;
		}
		
		pixel_format_type = CVPixelBufferGetPixelFormatType(pixel_buffer);
		if( pixel_format_type != k422YpCbCr8CodecType ) {	// packed YUV 4:2:2
			fprintf(stderr, "ERROR: Improper pixel_format_type\n");
			goto bail;
		}
		
		// Lock Pixel buffer
		CVPixelBufferLockBaseAddress(pixel_buffer, 0);
		
		// Odd height special; bottom line duplication
		if( 1&(glob->height) ) 
			memcpy(	
				CVPixelBufferGetBaseAddress(pixel_buffer) + CVPixelBufferGetBytesPerRow(pixel_buffer)*(glob->height),
				CVPixelBufferGetBaseAddress(pixel_buffer) + CVPixelBufferGetBytesPerRow(pixel_buffer)*(glob->height-1),
				CVPixelBufferGetBytesPerRow(pixel_buffer) );
		
		// Odd width special; Duplicate right edge's luminance (2vuy only)
		if( 1&(glob->width) ) {
			UInt8* pbroot = CVPixelBufferGetBaseAddress(pixel_buffer);
			UInt32 rowbyte = CVPixelBufferGetBytesPerRow(pixel_buffer);
			UInt32 y;
			for( y = 0; y < (glob->codecCont->height); y++) {
				UInt8* p = (pbroot + y * rowbyte) + (2 * glob->width);
				p[1] = p[-1];
			}
		}
		
		// Convert packed YUV 4:2:2 -> planar YUV 4:2:0
		if(glob->subsampling < 0) {
			assert(CVF_Context_Create);
		} else {
			if(glob->subsampling > 0) {
				copy_2vuy_to_planar_YUV420_i(
//					glob->codecCont->width, glob->codecCont->height,				// width=>ru2, height=>ru2
//					glob->widthRoundedUp, glob->heightRoundedUp,					// width=>ru16, height=>ru16
//					glob->widthRoundedUp, glob->codecCont->height,					// width=>ru16, height=>ru2
					glob->widthRoundedUp, glob->height,								// width>=ru16, height is native
					CVPixelBufferGetBaseAddress(pixel_buffer), CVPixelBufferGetBytesPerRow(pixel_buffer),
					glob->frame->data[0], glob->frame->linesize[0], 
					glob->frame->data[1], glob->frame->linesize[1], 
					glob->frame->data[2], glob->frame->linesize[2],
					glob->subsampling);
			} else 
			if(glob->subsampling == 0) {
				copy_2vuy_to_planar_YUV420(
//					glob->codecCont->width, glob->codecCont->height,				// width=>ru2, height=>ru2
//					glob->widthRoundedUp, glob->heightRoundedUp,					// width=>ru16, height=>ru16
//					glob->widthRoundedUp, glob->codecCont->height,					// width=>ru16, height=>ru2
					glob->widthRoundedUp, glob->height,								// width>=ru16, height is native
					CVPixelBufferGetBaseAddress(pixel_buffer), CVPixelBufferGetBytesPerRow(pixel_buffer),
					glob->frame->data[0], glob->frame->linesize[0], 
					glob->frame->data[1], glob->frame->linesize[1], 
					glob->frame->data[2], glob->frame->linesize[2]);
			}
		
#if 0
			// Fill missing bottom line in chroma planes after conversion (chroma odd line special)
			int chromaH0 = glob->codecCont->height >> 1;							// chromaH0 could be odd.
			if( chromaH0 & 1 ) {
				int offset = (glob->subsampling==0 ? 1 : 2);						// use same field
				memcpy(	glob->frame->data[1] + glob->frame->linesize[1] * chromaH0, 
						glob->frame->data[1] + glob->frame->linesize[1] * (chromaH0 - offset), 
						glob->frame->linesize[1]);									// duplicate bottom line U
				memcpy(	glob->frame->data[2] + glob->frame->linesize[2] * chromaH0, 
						glob->frame->data[2] + glob->frame->linesize[2] * (chromaH0 - offset), 
						glob->frame->linesize[2]);									// duplicate bottom line V
			}
#endif
#if 1
			// Dup luma bottom line under odd height picture
			int lumaH0 = glob->height;
			if( lumaH0 & 1 ) {
				memcpy(
					glob->frame->data[0] + glob->frame->linesize[0] * (lumaH0), 
					glob->frame->data[0] + glob->frame->linesize[0] * (lumaH0 - 1), 
					glob->frame->linesize[0]);
			}
#endif
		}
		
		// Unlock PixelBuffer
		CVPixelBufferUnlockBaseAddress(pixel_buffer, 0);
	}
	
	/* ================================================================================= */
	
	{
		// Encode source frame
		if( CFArrayGetCount(glob->array_source_frame) == kMaxSourceFrame ) {
			fprintf(stderr, "ERROR: Source frame buffer count hit limit(%d).\n", kMaxSourceFrame);
			goto bail;
		}
		
		CFArrayAppendValue(glob->array_source_frame, source_frame);
		glob->frame_count++;
 	}
#if USECoreVF
	if(glob->filterPreset) {
		if(!glob->cvfCont) {
			err = process_libAV(glob, source_frame);
			if(err) goto bail;
		} else {
			// Xfer from AVFrame into CVF_Frame
			if(glob->subsampling >= 0) {
				CVF_CopyPlane(
					glob->frame->data[0], glob->frame->linesize[0], glob->codecCont->height,
					CVF_GetPlaneBase(glob->cvfInFrame, 0),
					CVF_GetPlaneStride(glob->cvfInFrame, 0), CVF_GetPlaneRow(glob->cvfInFrame, 0)
				);
				CVF_CopyPlane(
					glob->frame->data[1], glob->frame->linesize[1], glob->codecCont->height,
					CVF_GetPlaneBase(glob->cvfInFrame, 1),
					CVF_GetPlaneStride(glob->cvfInFrame, 1), CVF_GetPlaneRow(glob->cvfInFrame, 1)
				);
				CVF_CopyPlane(
					glob->frame->data[2], glob->frame->linesize[2], glob->codecCont->height,
					CVF_GetPlaneBase(glob->cvfInFrame, 2),
					CVF_GetPlaneStride(glob->cvfInFrame, 2), CVF_GetPlaneRow(glob->cvfInFrame, 2)
				);
			} else {
				CVPixelBufferRef pixel_buffer = ICMCompressorSourceFrameGetPixelBuffer(source_frame);
				
				// Lock Pixel buffer
				CVPixelBufferLockBaseAddress(pixel_buffer, 0);
				
				// 422 Direct processing; requires CoreVF support
				CVF_CopyPlane(
					CVPixelBufferGetBaseAddress(pixel_buffer), CVPixelBufferGetBytesPerRow(pixel_buffer), 
					glob->heightRoundedUp,
					CVF_GetPlaneBase(glob->cvfInFrame, 0),
					CVF_GetPlaneStride(glob->cvfInFrame, 0), CVF_GetPlaneRow(glob->cvfInFrame, 0)
				);
				
				// Unlock PixelBuffer
				CVPixelBufferUnlockBaseAddress(pixel_buffer, 0);
			}
			
			// push source_frame for later use
			CFArrayAppendValue(glob->cvfSourceArray, source_frame);
			
			// call filter chain
			glob->cvfInFrame->index = glob->cvfCont->inputIndex + 1;	/* not strict but enough */
			glob->cvfOutFrame->index = -1;
			int result = CVF_Context_Process(glob->cvfCont, 
					glob->cvfInFrame->buffer, glob->cvfInFrame->video, 
					glob->cvfOutFrame->buffer, glob->cvfOutFrame->video);
			
			if( result == CVF_ST_FRAMEISREADY || result == CVF_ST_REQMOREFRAME ) {
				err = noErr;	/* we should update this here ! */
			} else {
				fprintf(stderr, "ERROR: CVF_Context_Process() returned error(%d).\n", result);
				err = paramErr;
				goto bail;
			}
			
			// process output frame(s)
			while(CFArrayGetCount(glob->cvfCont->tempCache)) {
				// take out CVF_Frames
				result = CVF_Context_Extract(glob->cvfCont, 
							glob->cvfOutFrame->buffer, glob->cvfOutFrame->video);
				
				if(result == CVF_ST_FRAMEISREADY) {
					// feed from XVF_Frame into libavcodec
					CVF_CopyPlane(
						CVF_GetPlaneBase(glob->cvfOutFrame, 0),
						CVF_GetPlaneStride(glob->cvfOutFrame, 0), CVF_GetPlaneRow(glob->cvfOutFrame, 0),
						glob->frame->data[0], glob->frame->linesize[0], glob->codecCont->height
					);
					CVF_CopyPlane(
						CVF_GetPlaneBase(glob->cvfOutFrame, 1),
						CVF_GetPlaneStride(glob->cvfOutFrame, 1), CVF_GetPlaneRow(glob->cvfOutFrame, 1),
						glob->frame->data[1], glob->frame->linesize[1], glob->codecCont->height
					);
					CVF_CopyPlane(
						CVF_GetPlaneBase(glob->cvfOutFrame, 2),
						CVF_GetPlaneStride(glob->cvfOutFrame, 2), CVF_GetPlaneRow(glob->cvfOutFrame, 2),
						glob->frame->data[2], glob->frame->linesize[2], glob->codecCont->height
					);
					
					if(CFArrayGetCount(glob->cvfSourceArray)) {
						source_frame = (ICMCompressorSourceFrameRef)CFArrayGetValueAtIndex(glob->cvfSourceArray, 0);
						CFArrayRemoveValueAtIndex(glob->cvfSourceArray, 0);
						
						err = process_libAV(glob, source_frame);
						if(err) goto bail;
					} else {
						// Broken filter output; drop output anyway
					}
				} else if(result != CVF_ST_REQMOREFRAME) {
					// error condition
					err = paramErr;
					goto bail;
				}
			} // while(CFArrayGetCount(glob->cvfCont->tempCache))
			
		} // if(glob->cvfCont) 
	} else
#endif
	{
		err = process_libAV(glob, source_frame);
		if(err) goto bail;
	}
	
bail:
	if(err && !glob->meetsErr) {
		AudioServicesPlayAlertSound(kUserPreferredAlert); glob->meetsErr = TRUE;
	}
	return err;
} // ComponentCall (EncodeFrame)


// ComponentCall (CompleteFrame)						// 57
ComponentResult lavcEncoder_CompleteFrame(lavcEncoderGlobalRecord *glob, 
			ICMCompressorSourceFrameRef source_frame, UInt32 flags)
{
	logDebug(glob, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	ComponentResult err = noErr;
	
	if( glob->fakeMultipass && !glob->emit_frame ) {
		// Skipping this pass... (QSCALE or CRF is enabled)
		goto bail;
	}
	
#if USECoreVF
	if(glob->filterPreset) {
		int result = CVF_ST_NOERROR;
		while(glob->cvfCont->inputIndex > glob->cvfCont->outputIndex) {
			if(!CFArrayGetCount(glob->cvfSourceArray)) break;
			
			glob->cvfInFrame->index = -1;
			glob->cvfOutFrame->index = -1;
			result = CVF_Context_Process(glob->cvfCont, NULL, NULL, 
										glob->cvfOutFrame->buffer, glob->cvfOutFrame->video);
			
			if( result == CVF_ST_FRAMEISREADY || result == CVF_ST_REQMOREFRAME ) {
				err = noErr;	/* we should update this here ! */
			} else {
				fprintf(stderr, "ERROR: CVF_Context_Process() returned error(%d).\n", result);
				err = paramErr;
				goto bail;
			}
			
			while( CFArrayGetCount(glob->cvfCont->tempCache) ) {
				result = CVF_Context_Extract(glob->cvfCont, 
							glob->cvfOutFrame->buffer, glob->cvfOutFrame->video);
				
				if(result == CVF_ST_FRAMEISREADY) {
					// feed from XVF_Frame into libavcodec
					CVF_CopyPlane(
						CVF_GetPlaneBase(glob->cvfOutFrame, 0),
						CVF_GetPlaneStride(glob->cvfOutFrame, 0), CVF_GetPlaneRow(glob->cvfOutFrame, 0),
						glob->frame->data[0], glob->frame->linesize[0], glob->codecCont->height
					);
					CVF_CopyPlane(
						CVF_GetPlaneBase(glob->cvfOutFrame, 1),
						CVF_GetPlaneStride(glob->cvfOutFrame, 1), CVF_GetPlaneRow(glob->cvfOutFrame, 1),
						glob->frame->data[1], glob->frame->linesize[1], glob->codecCont->height
					);
					CVF_CopyPlane(
						CVF_GetPlaneBase(glob->cvfOutFrame, 2),
						CVF_GetPlaneStride(glob->cvfOutFrame, 2), CVF_GetPlaneRow(glob->cvfOutFrame, 2),
						glob->frame->data[2], glob->frame->linesize[2], glob->codecCont->height
					);
					
					if(CFArrayGetCount(glob->cvfSourceArray)) {
						source_frame = (ICMCompressorSourceFrameRef)CFArrayGetValueAtIndex(glob->cvfSourceArray, 0);
						CFArrayRemoveValueAtIndex(glob->cvfSourceArray, 0);
						
						err = process_libAV(glob, source_frame);
						if(err) goto bail;
					} else {
						// Broken filter output; drop output anyway
					}
				} else if(result != CVF_ST_REQMOREFRAME) {
					// error condition
					err = paramErr;
					goto bail;
				}
			} // while( CFArrayGetCount(glob->cvfCont->tempCache) )
		}
	}
#endif
	
	// Proceed to libavcodec call
	int limit = FF_MAX_B_FRAMES;
	while(glob->delayCount) {
		// Pass NULL AVFrame into codec's buffer
		glob->frame_size = avcodec_encode_video(glob->codecCont, 
							glob->encode_buffer, glob->encode_buffer_size, NULL);
//		logDebug(glob, "lavcEncoder: glob->frame_size: %010d\n", glob->frame_size);
		
		// Check encoded frame
		if( glob->frame_size > 0 ) {
			glob->delayCount--;
			
			if( glob->emit_frame ) {
				err = emitFrameData(glob);
			} else {
				// last few frames of pass 1; ignore it.
			}
			
			// log file support
			if( glob->logfile ) fprintf( glob->logfile, "%s", glob->codecCont->stats_out );
		} else {
			if( --limit == 0 ) {
				fprintf(stderr, "ERROR: No remaining frames found\n");
				fprintf(stderr, "ERROR: array_source_frame still contains %d frame(s).\n"
					, (int)CFArrayGetCount(glob->array_source_frame));
				err = paramErr;
				break;
			}
		}
	}
	
bail:
	if(err && !glob->meetsErr) {
		AudioServicesPlayAlertSound(kUserPreferredAlert); glob->meetsErr = TRUE;
	}
	return err;
} // ComponentCall (CompleteFrame)


#pragma mark -


// ComponentCall (BeginPass)							// 58
ComponentResult lavcEncoder_BeginPass (lavcEncoderGlobalRecord *glob, 
			ICMCompressionPassModeFlags pass_mode_flags, UInt32 flags, ICMMultiPassStorageRef storage)
{
	logDebug(glob, "lavcEncoder: [%08x %s] ; glob->ICM_passcount = %d\n", glob, __FUNCTION__, glob->ICM_passcount);
	
	ComponentResult err = paramErr;
	glob->passDate = CFDateCreate(kCFAllocatorDefault, CFAbsoluteTimeGetCurrent());
	
	// Check
	if( glob->codec_is_opened ) {
		fprintf(stderr, "ERROR; Codec is already opend.\n");
		goto bail;
	}
	if( !glob->params_opaque.log_file_path || 0 == strlen(glob->params_opaque.log_file_path) ) {
		fprintf(stderr, "ERROR: Checking log file path failed\n");
		goto bail;
	}
	if( glob->logfile ) {
		fprintf(stderr, "ERROR: Log file is already opend.\n");
		goto bail;
	}
	
	logDebug(glob, "lavcEncoder: (IN) pass_mode_flags = %d glob->codecCont->flags= 0x%010x\n"
				, pass_mode_flags, glob->codecCont->flags
				);
	
	// Check ICM flags
	glob->emit_frame = FALSE;	// unset emit_frame before start
	UInt32 flagMask = ( kICMCompressionPassMode_NoSourceFrames |
						kICMCompressionPassMode_OutputEncodedFrames | 
						kICMCompressionPassMode_NotReadyToOutputEncodedFrames );
	
	switch( glob->ICM_passcount ) {
	case -1:
		if( (pass_mode_flags & flagMask) == 
			(kICMCompressionPassMode_NoSourceFrames | kICMCompressionPassMode_NotReadyToOutputEncodedFrames) ) {
			logInfo(glob, "lavcEncoder: Begin pass 0.\n");
			glob->ICM_passcount = 0;	// null pass
			glob->emit_frame = FALSE;
			glob->codecCont->flags &= ~CODEC_FLAG_PASS1;
			glob->codecCont->flags &= ~CODEC_FLAG_PASS2;
			err = noErr;
			goto bail;
		} else {
			logInfo(glob, "lavcEncoder: Begin pass 0.\n");
			fprintf(stderr, "ERROR: Invalid pass_mode_flags for this pass detected.\n");
			goto bail;
		}
		break;
	case 0:
		if( (pass_mode_flags & flagMask) == kICMCompressionPassMode_NotReadyToOutputEncodedFrames ) {
			logInfo(glob, "lavcEncoder: Begin pass 1.\n");
			glob->ICM_passcount = 1;			// analyzing pass
			glob->emit_frame = FALSE;
			glob->codecCont->flags |= CODEC_FLAG_PASS1;
			glob->codecCont->flags &= ~CODEC_FLAG_PASS2;
		} else if( (pass_mode_flags & flagMask) == 
			(kICMCompressionPassMode_NoSourceFrames | kICMCompressionPassMode_NotReadyToOutputEncodedFrames) ) {
			logInfo(glob, "lavcEncoder: Begin pass 1 as null pass.\n");
			glob->ICM_passcount = 1;			// analyzing, but null pass
			glob->emit_frame = FALSE;
			glob->codecCont->flags &= ~CODEC_FLAG_PASS1;
			glob->codecCont->flags &= ~CODEC_FLAG_PASS2;
		} else {
			logInfo(glob, "lavcEncoder: Begin pass 1.\n");
			fprintf(stderr, "ERROR: Invalid pass_mode_flags for this pass detected.\n");
			goto bail;
		}
		break;
	case 1:
		if( (pass_mode_flags & flagMask) == kICMCompressionPassMode_OutputEncodedFrames) {
			logInfo(glob, "lavcEncoder: Begin pass 2.\n");
			glob->ICM_passcount = 2;			// encoding pass
			glob->emit_frame = TRUE;
			glob->codecCont->flags &= ~CODEC_FLAG_PASS1;
			glob->codecCont->flags |= CODEC_FLAG_PASS2;
		} else if( (pass_mode_flags & flagMask) == kICMCompressionPassMode_NotReadyToOutputEncodedFrames ) {
			logInfo(glob, "lavcEncoder: Begin pass 2 (1/2).\n");
			glob->ICM_passcount = 3;			// Additional encoding pass (optional)
			glob->emit_frame = FALSE;
			glob->codecCont->flags |= CODEC_FLAG_PASS1;
			glob->codecCont->flags |= CODEC_FLAG_PASS2;
		} else if( (pass_mode_flags & flagMask) == 
			(kICMCompressionPassMode_NoSourceFrames | kICMCompressionPassMode_NotReadyToOutputEncodedFrames) ) {
			logInfo(glob, "lavcEncoder: Begin pass 2 (1/2) as null pass.\n");
			glob->ICM_passcount = 3;			// Additional encoding, but null pass
			glob->emit_frame = FALSE;
			glob->codecCont->flags &= ~CODEC_FLAG_PASS1;
			glob->codecCont->flags &= ~CODEC_FLAG_PASS2;
		} else {
			logInfo(glob, "lavcEncoder: Begin pass 2.\n");
			fprintf(stderr, "ERROR: Invalid pass_mode_flags for this pass detected.\n");
			goto bail;
		}
		break;
	case 3:										// Begin pass 3 of 3
		if( (pass_mode_flags & flagMask) == kICMCompressionPassMode_OutputEncodedFrames) {
			logInfo(glob, "lavcEncoder: Begin pass 2 (2/2).\n");
			glob->ICM_passcount = 2;			// last encoding pass
			glob->emit_frame = TRUE;
			glob->codecCont->flags &= ~CODEC_FLAG_PASS1;
			glob->codecCont->flags |= CODEC_FLAG_PASS2;
		} else if( (pass_mode_flags & flagMask) == kICMCompressionPassMode_NotReadyToOutputEncodedFrames ) {
			logInfo(glob, "lavcEncoder: Begin pass 2 (1+/2+).\n");
			glob->ICM_passcount = 3;			// More additional encoding pass (optional)
			glob->emit_frame = FALSE;
			glob->codecCont->flags |= CODEC_FLAG_PASS1;
			glob->codecCont->flags |= CODEC_FLAG_PASS2;
		} else if( (pass_mode_flags & flagMask) == 
			(kICMCompressionPassMode_NoSourceFrames | kICMCompressionPassMode_NotReadyToOutputEncodedFrames) ) {
			logInfo(glob, "lavcEncoder: Begin pass 2 (1+/2+) as null pass.\n");
			glob->ICM_passcount = 3;			// More additional encoding, but null pass
			glob->emit_frame = FALSE;
			glob->codecCont->flags &= ~CODEC_FLAG_PASS1;
			glob->codecCont->flags &= ~CODEC_FLAG_PASS2;
		} else {
			logInfo(glob, "lavcEncoder: Begin pass 2 (2/2).\n");
			fprintf(stderr, "ERROR: Invalid pass_mode_flags for this pass detected.\n");
			goto bail;
		}
		break;
	default:
		fprintf(stderr, "ERROR: Invalid pass count detected.\n");
		goto bail;
		break;
	}
	
	if( glob->fakeMultipass ) {
		glob->codecCont->flags &= ~CODEC_FLAG_PASS1;
		glob->codecCont->flags &= ~CODEC_FLAG_PASS2;
		
		if( !glob->emit_frame ) {
			logInfo(glob, "lavcEncoder: - Skipping this pass... (QSCALE or CRF is enabled)\n");
			err = noErr;
			goto bail;
		}
	}
	
	/* ================================================================================= */
	
	err = noErr;
		
	// Handle log file
	if( !glob->fakeMultipass ) {
		logDebug(glob, "lavcEncoder: path = %s\n", glob->params_opaque.log_file_path );
		
#if WRITELOG
		FILE *f;
		int size, ret;
		char *logbuffer;
		
		if( glob->ICM_passcount == 1 ) {
			
			// Pass 1; Open logfile for write
			// logfile will be closed at EndPass()
			f = fopen( glob->params_opaque.log_file_path, "w" );
			if( !f ) {
				fprintf(stderr, "ERROR: Opening log file for write failed.\n");
				err = paramErr;
			}
			glob->logfile = f;
			
		} else if( glob->ICM_passcount == 2 ) {
			
			// Pass 2; Read from logfile into buffer
			// Open, Read, Close, Remove it.
			f = fopen( glob->params_opaque.log_file_path, "r" );
			if( !f ) {
				fprintf(stderr, "ERROR: Opening log file for read failed.\n");
				err = paramErr;
			} else {
				fseek( f, 0, SEEK_END );
				size = ftell( f );
				fseek( f, 0, SEEK_SET );
				
				logbuffer = calloc( size + 1, 1 );
				if( !logbuffer ) {
					fprintf(stderr, "ERROR: Allocating log buffer failed.\n");
					err = paramErr;
				} else {
					size = fread( logbuffer, 1, size, f );
					logbuffer[size] = '\0';
					glob->codecCont->stats_in = logbuffer;
				}
				
				ret = fclose( f );
				if( ret ) {
					fprintf(stderr, "ERROR: Closing log file failed.(%d)\n", errno);
					err = paramErr;
				}
			}
		} else {	// seems redundant...
			fprintf(stderr, "ERROR: Invalid pass count detected.\n");
			err = paramErr;
		}
#endif
	}
	
	// reset global values
	glob->next_pts_value = 0;	// seems redundant...
	glob->video_size = 0;
	glob->frame_count = 0;
	glob->delayCount = 0;
	
bail:
	logDebug(glob, "lavcEncoder: emit_frame = %s\n", (glob->emit_frame ? "TRUE" : "FALSE" ));	
	logDebug(glob, "lavcEncoder: (OUT) pass_mode_flags = %d, glob->codecCont->flags= 0x%010x\n", 
				pass_mode_flags, glob->codecCont->flags);
	
	if(err && !glob->meetsErr) {
		AudioServicesPlayAlertSound(kUserPreferredAlert); glob->meetsErr = TRUE;
	}
	return err;
} // ComponentCall (BeginPass)


// ComponentCall (EndPass)								// 59
ComponentResult lavcEncoder_EndPass(lavcEncoderGlobalRecord *glob)
{
	logDebug(glob, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	ComponentResult err = noErr;
	
	// If pass 0, just print out total number of frames, and exit
	if( glob->ICM_passcount <= 0 ) {
		logInfo(glob, "lavcEncoder: - Total number of frames: %d.\n", glob->frame_count);
		goto bail;
	}
	
	if( glob->fakeMultipass && !glob->emit_frame ) {
		// Skipping this pass... (QSCALE or CRF is enabled)
		goto bail;
	}
	
#if USECoreVF
	ICMCompressorSourceFrameRef source_frame;
	if(glob->filterPreset) {
		int result = CVF_ST_NOERROR;
		while(glob->cvfCont->inputIndex > glob->cvfCont->outputIndex) {
			if(!CFArrayGetCount(glob->cvfSourceArray)) break;
			
			glob->cvfInFrame->index = -1;
			glob->cvfOutFrame->index = -1;
			result = CVF_Context_Process(glob->cvfCont, NULL, NULL, 
										glob->cvfOutFrame->buffer, glob->cvfOutFrame->video);
			
			if( result == CVF_ST_FRAMEISREADY || result == CVF_ST_REQMOREFRAME ) {
				err = noErr;	/* we should update this here ! */
			} else {
				fprintf(stderr, "ERROR: CVF_Context_Process() returned error(%d).\n", result);
				err = paramErr;
				goto bail;
			}
			
			while( CFArrayGetCount(glob->cvfCont->tempCache) ) {
				result = CVF_Context_Extract(glob->cvfCont, 
							glob->cvfOutFrame->buffer, glob->cvfOutFrame->video);

				if(result == CVF_ST_FRAMEISREADY) {
					// feed from XVF_Frame into libavcodec
					CVF_CopyPlane(
						CVF_GetPlaneBase(glob->cvfOutFrame, 0),
						CVF_GetPlaneStride(glob->cvfOutFrame, 0), CVF_GetPlaneRow(glob->cvfOutFrame, 0),
						glob->frame->data[0], glob->frame->linesize[0], glob->codecCont->height
					);
					CVF_CopyPlane(
						CVF_GetPlaneBase(glob->cvfOutFrame, 1),
						CVF_GetPlaneStride(glob->cvfOutFrame, 1), CVF_GetPlaneRow(glob->cvfOutFrame, 1),
						glob->frame->data[1], glob->frame->linesize[1], glob->codecCont->height
					);
					CVF_CopyPlane(
						CVF_GetPlaneBase(glob->cvfOutFrame, 2),
						CVF_GetPlaneStride(glob->cvfOutFrame, 2), CVF_GetPlaneRow(glob->cvfOutFrame, 2),
						glob->frame->data[2], glob->frame->linesize[2], glob->codecCont->height
					);
					
					if(CFArrayGetCount(glob->cvfSourceArray)) {
						source_frame = (ICMCompressorSourceFrameRef)CFArrayGetValueAtIndex(glob->cvfSourceArray, 0);
						CFArrayRemoveValueAtIndex(glob->cvfSourceArray, 0);
						
						err = process_libAV(glob, source_frame);
						if(err) goto bail;
					} else {
						// Broken filter output; drop output anyway
					}
				} else if(result != CVF_ST_REQMOREFRAME) {
					// error condition
					err = paramErr;
					goto bail;
				}
			} // while( CFArrayGetCount(glob->cvfCont->tempCache) )
		}
	}
#endif
	
	// Some app does not call completeFrame at Pass 1... I need last logs, a few rows...
	int limit = FF_MAX_B_FRAMES;
	while( glob->delayCount ) {
		// Pass NULL AVFrame into codec's buffer
		glob->frame_size = avcodec_encode_video(glob->codecCont, 
						glob->encode_buffer, glob->encode_buffer_size, NULL);
//		logDebug(glob, "lavcEncoder: glob->frame_size: %010d\n", glob->frame_size);
		
		// Check encoded frame
		if( glob->frame_size > 0 ) {
			glob->delayCount--;
			
			// log file support
			if( glob->logfile ) fprintf( glob->logfile, "%s", glob->codecCont->stats_out );
		} else {
			if( --limit == 0 ) {
				fprintf(stderr, "ERROR: No remaining frames found\n");
				fprintf(stderr, "ERROR: array_source_frame still contains %d frame(s).\n"
					, (int)CFArrayGetCount(glob->array_source_frame));
				err = paramErr;
				break;
			}
		}
	}
	
#if WRITELOG
	// Handle log file
	if( glob->ICM_passcount == 1 ) {								// After pass 1
		if( (glob->codecCont->flags & CODEC_FLAG_PASS1) && glob->logfile ) {
			logInfo(glob, "lavcEncoder: End pass 1.\n");
			
			fclose( glob->logfile );
			glob->logfile = NULL;
			
		} else {
			fprintf(stderr, "ERROR: Invalid flags or state detected.\n");
			err = paramErr;
		}
	} else if( glob->ICM_passcount == 2 ) {							// After pass 2
		if( (glob->codecCont->flags & CODEC_FLAG_PASS2) && glob->codecCont->stats_in ) {
			logInfo(glob, "lavcEncoder: End pass 2.\n");
			
			free(glob->codecCont->stats_in);
			glob->codecCont->stats_in = NULL;
			
		} else if( glob->fakeMultipass ) {
			;	// do nothing
		} else {
			fprintf(stderr, "ERROR: Invalid flags or state detected.\n");
			err = paramErr;
		}
	} else {
		fprintf(stderr, "ERROR: Invalid pass count detected.\n");
		err = paramErr;
	}
#endif
	
bail:
	// Close codec now
	if( glob->codecCont ) {
		if( glob->codec_is_opened ) {
			avcodec_close( glob->codecCont );
			glob->codec_is_opened = FALSE;
			glob->extraDataReady = FALSE;
		}
		
#if USECoreVF
		if( glob->cvfCont ) closeCoreVF( glob );
#endif
		
		if( glob->encode_buffer ) {
			free( glob->encode_buffer );
			glob->encode_buffer = NULL;
		}
		if( glob->planer_buffer ) {
			free( glob->planer_buffer );
			glob->planer_buffer = NULL;
		}
		
		if( glob->frame ) {
			av_free( glob->frame );
			glob->frame = NULL;
		}
		
		av_free( glob->codecCont );
		glob->codecCont = NULL;
	}
	
	// Show lap time
	if( glob->passDate ) {
		if( glob->ICM_passcount > 0 ) {
			CFDateRef lastDate = CFDateCreate(kCFAllocatorDefault, CFAbsoluteTimeGetCurrent());
			CFTimeInterval diff = CFDateGetTimeIntervalSinceDate(lastDate, glob->passDate);
			CFRelease(lastDate);
			
			char* passstr = "";
			if(glob->ICM_passcount == 1) passstr="1";
			if(glob->ICM_passcount == 2) passstr="2";
			if(glob->use3rdpass && glob->ICM_passcount == 3) passstr="2 (1/2)";
			if(glob->use3rdpass && glob->ICM_passcount == 2) passstr="2 (2/2)";
			
			logInfo(glob, "lavcEncoder: - Pass %s is %.2f fps (%d frames / %.1f sec)\n"
					, passstr
					, (diff > 0.0 ? (double)glob->frame_count/diff : 0.0)
					, glob->frame_count, (double)diff
					);
		}
		CFRelease(glob->passDate);
		glob->passDate = NULL;
	}
	
	if(err && !glob->meetsErr) {
		AudioServicesPlayAlertSound(kUserPreferredAlert); glob->meetsErr = TRUE;
	}
	return err;
} // ComponentCall (EndPass)


// ComponentCall (ProcessBetweenPasses)					// 60
// We do not use ICM's default multi-pass storage file, but use temporary log file.
// This function should be called before every BeginPass...
ComponentResult lavcEncoder_ProcessBetweenPasses(lavcEncoderGlobalRecord *glob, 
			ICMMultiPassStorageRef storage, Boolean *done, ICMCompressionPassModeFlags *flags)
{
	logDebug(glob, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	ComponentResult err = noErr;
	logDebug(glob, "lavcEncoder: (IN) pass_mode_flags = %d \n", *flags);
	
	// Check
	switch (glob->ICM_passcount) {
	case -1:
		logDebug(glob, "lavcEncoder: Prepare pass 0 of 2.\n");
		*flags =  kICMCompressionPassMode_NotReadyToOutputEncodedFrames
				| kICMCompressionPassMode_NoSourceFrames			// null pass...
				;
		break;
	case 0:
		logDebug(glob, "lavcEncoder: Prepare pass 1 of 2.\n");
		*flags =  kICMCompressionPassMode_NotReadyToOutputEncodedFrames
//				| kICMCompressionPassMode_WriteToMultiPassStorage	// pass 1 should be analysis pass...
				;
		break;
	case 1:
		if( !glob->use3rdpass ) {
			logDebug(glob, "lavcEncoder: Prepare pass 2 of 2.\n");
			*flags =  kICMCompressionPassMode_OutputEncodedFrames
//					| kICMCompressionPassMode_ReadFromMultiPassStorage	// pass 2 should be encoding pass...
					;
		} else {
			logDebug(glob, "lavcEncoder: Prepare 2 of 3.\n");	// next ICM_passcount is 3
			*flags =  kICMCompressionPassMode_NotReadyToOutputEncodedFrames
//					| kICMCompressionPassMode_ReadFromMultiPassStorage	// pass 3 is additional pass...
//					| kICMCompressionPassMode_WriteToMultiPassStorage	// pass 3 is additional pass...
					;
		}
		break;
	case 3:
		logDebug(glob, "lavcEncoder: Prepare pass 3 of 3.\n");	// next ICM_passcount is 2
		*flags =  kICMCompressionPassMode_OutputEncodedFrames
//				| kICMCompressionPassMode_ReadFromMultiPassStorage	// pass 2 should be encoding pass...
				;
		break;
	default:
		fprintf(stderr, "ERROR: Invalid pass count detected.\n");
		err = codecErr;
		break;
	}
	
	if( glob->fakeMultipass && !(*flags & kICMCompressionPassMode_OutputEncodedFrames) ) {
		// Skipping this pass... (QSCALE or CRF is enabled)
		*flags =  kICMCompressionPassMode_NotReadyToOutputEncodedFrames
				| kICMCompressionPassMode_NoSourceFrames			// null pass...
				;
	}
	
	logDebug(glob, "lavcEncoder: (OUT) pass_mode_flags = %d \n", *flags);
	*done = TRUE;
	
	if(err && !glob->meetsErr) {
		AudioServicesPlayAlertSound(kUserPreferredAlert); glob->meetsErr = TRUE;
	}
	return err;
} // ComponentCall (ProcessBetweenPasses)


#pragma mark -
#pragma mark Private Functions


static OSStatus setup_codecCont(lavcEncoderGlobalRecord *glob)
{	
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	OSStatus err = paramErr;
	
#if X264
	// Adjust values same as x264 default
	glob->codecCont->partitions = X264_PART_I4X4 | X264_PART_I8X8 | X264_PART_P8X8 | X264_PART_B8X8;
#endif
#if XVID
	glob->codecCont->rc_strategy = FF_RC_STRATEGY_XVID;
	glob->codecCont->b_quant_offset = 1.0;
#endif
	
	/* ==================================== */
	
	// Update Flags
	if( glob->params.FLAG_QSCALE )		glob->codecCont->flags |= CODEC_FLAG_QSCALE;
	else								glob->codecCont->flags &= ~ CODEC_FLAG_QSCALE;
	
	if( glob->params.FLAG_4MV )			glob->codecCont->flags |= CODEC_FLAG_4MV;
	else								glob->codecCont->flags &= ~ CODEC_FLAG_4MV;
	
	if( glob->params.FLAG_QPEL )		glob->codecCont->flags |= CODEC_FLAG_QPEL;
	else								glob->codecCont->flags &= ~ CODEC_FLAG_QPEL;
	
	if( glob->params.FLAG_GMC )			glob->codecCont->flags |= CODEC_FLAG_GMC;
	else								glob->codecCont->flags &= ~ CODEC_FLAG_GMC;
	
	if( glob->params.FLAG_MV0 )			glob->codecCont->flags |= CODEC_FLAG_MV0;
	else								glob->codecCont->flags &= ~ CODEC_FLAG_MV0;
	
	if( glob->params.FLAG_PART )		glob->codecCont->flags |= CODEC_FLAG_PART;
	else								glob->codecCont->flags &= ~ CODEC_FLAG_PART;
	
	if( glob->params.FLAG_LOOP_FILTER )	glob->codecCont->flags |= CODEC_FLAG_LOOP_FILTER;
	else								glob->codecCont->flags &= ~ CODEC_FLAG_LOOP_FILTER;
	
	if( glob->params.FLAG_PSNR )		glob->codecCont->flags |= CODEC_FLAG_PSNR;
	else								glob->codecCont->flags &= ~ CODEC_FLAG_PSNR;
	
	if( glob->params.FLAG_NORMALIZE_AQP )	glob->codecCont->flags |= CODEC_FLAG_NORMALIZE_AQP;
	else									glob->codecCont->flags &= ~ CODEC_FLAG_NORMALIZE_AQP;
	
	if( glob->params.FLAG_INTERLACED_DCT )	glob->codecCont->flags |= CODEC_FLAG_INTERLACED_DCT;
	else									glob->codecCont->flags &= ~ CODEC_FLAG_INTERLACED_DCT;
	
	if( glob->params.FLAG_AC_PRED )		glob->codecCont->flags |= CODEC_FLAG_AC_PRED;
	else								glob->codecCont->flags &= ~ CODEC_FLAG_AC_PRED;
	
	if( glob->params.FLAG_CBP_RD )		glob->codecCont->flags |= CODEC_FLAG_CBP_RD;
	else								glob->codecCont->flags &= ~ CODEC_FLAG_CBP_RD;
	
	if( glob->params.FLAG_QP_RD )		glob->codecCont->flags |= CODEC_FLAG_QP_RD;
	else								glob->codecCont->flags &= ~ CODEC_FLAG_QP_RD;
	
	if( glob->params.FLAG_CLOSED_GOP )	glob->codecCont->flags |= CODEC_FLAG_CLOSED_GOP;
	else								glob->codecCont->flags &= ~ CODEC_FLAG_CLOSED_GOP;
	
	if( glob->params.FLAG2_BPYRAMID )	glob->codecCont->flags2 |= CODEC_FLAG2_BPYRAMID;
	else								glob->codecCont->flags2 &= ~ CODEC_FLAG2_BPYRAMID;
	
	if( glob->params.FLAG2_WPRED )		glob->codecCont->flags2 |= CODEC_FLAG2_WPRED;
	else								glob->codecCont->flags2 &= ~ CODEC_FLAG2_WPRED;
	
	if( glob->params.FLAG2_MIXED_REFS )	glob->codecCont->flags2 |= CODEC_FLAG2_MIXED_REFS;
	else								glob->codecCont->flags2 &= ~ CODEC_FLAG2_MIXED_REFS;
	
	if( glob->params.FLAG2_8X8DCT )		glob->codecCont->flags2 |= CODEC_FLAG2_8X8DCT;
	else								glob->codecCont->flags2 &= ~ CODEC_FLAG2_8X8DCT;
	
	if( glob->params.FLAG2_FASTPSKIP )	glob->codecCont->flags2 |= CODEC_FLAG2_FASTPSKIP;
	else								glob->codecCont->flags2 &= ~ CODEC_FLAG2_FASTPSKIP;
	
	if( glob->params.FLAG2_AUD )		glob->codecCont->flags2 |= CODEC_FLAG2_AUD;
	else								glob->codecCont->flags2 &= ~ CODEC_FLAG2_AUD;
	
	if( glob->params.FLAG2_BRDO )		glob->codecCont->flags2 |= CODEC_FLAG2_BRDO;
	else								glob->codecCont->flags2 &= ~ CODEC_FLAG2_BRDO;
	
	if( glob->params.FLAG2_MBTREE )		glob->codecCont->flags2 |= CODEC_FLAG2_MBTREE;
	else								glob->codecCont->flags2 &= ~ CODEC_FLAG2_MBTREE;
	
	if( glob->params.FLAG2_PSY )		glob->codecCont->flags2 |= CODEC_FLAG2_PSY;
	else								glob->codecCont->flags2 &= ~ CODEC_FLAG2_PSY;
	
	if( glob->params.FLAG2_SSIM )		glob->codecCont->flags2 |= CODEC_FLAG2_SSIM;
	else								glob->codecCont->flags2 &= ~ CODEC_FLAG2_SSIM;
	
	/* ==================================== */
	
	// -mbd ; macroblock decision mode
	switch( glob->params.MB_DECISION ) {
	case 1:
		glob->codecCont->mb_decision = FF_MB_DECISION_SIMPLE;
		break;
	case 2:
		glob->codecCont->mb_decision = FF_MB_DECISION_BITS;
		break;
	case 3:
		glob->codecCont->mb_decision = FF_MB_DECISION_RD;
		break;
	default:
		glob->codecCont->mb_decision = FF_MB_DECISION_SIMPLE;
		break;
	}
	
	// -qsquish ; ratecontrol qmin qmax limiting method. 0 is clipping.
	glob->codecCont->rc_qsquish = (glob->params.RC_QSQUISH ? 1 : 0);
	
	// -mpeg_quant ; quantizer h263/mpeg. 0 is h263 quant.
	glob->codecCont->mpeg_quant = (glob->params.MPEG_QUANT ? 1 : 0);
	
	// -threads ; worker thread count
	switch( glob->params.THREADS ) {
	case 1:
		glob->codecCont->thread_count = 1;
		break;
	case 2:
		glob->codecCont->thread_count = 2;
		break;
	case 3:
		glob->codecCont->thread_count = 4;
		break;
	case 4:
		glob->codecCont->thread_count = 8;
		break;
	case 9:
		glob->codecCont->thread_count = 0;
		break;
	default:
		glob->codecCont->thread_count = 2;
		break;
	}
	
	// -me_method ; motion estimation algorithm used for video coding
	switch( glob->params.ME_METHOD ) {
	case 1:
		glob->codecCont->me_method = ME_EPZS;
		break;
	case 2:
		glob->codecCont->me_method = ME_HEX;
		break;
	case 3:
		glob->codecCont->me_method = ME_UMH;
		break;
	case 4:
		glob->codecCont->me_method = ME_FULL;
		break;
	case 5:
		glob->codecCont->me_method = ME_X1;
		break;
	case 6:
		glob->codecCont->me_method = ME_TESA;
		break;
	default:
		glob->codecCont->me_method = ME_EPZS;
		break;
	}
	
	// -sc_threshold ; larger negative value uses Intra frames more frequently
	glob->codecCont->scenechange_threshold = glob->params.SC_THRESHOLD;
	
	// -qcomp ; larger to use a lot of bit for hard scene 
	glob->codecCont->qcompress = glob->params.QCOMPRESS/100.0;
	
	// -nr ; noise reduction; larger value makes softer image
	glob->codecCont->noise_reduction = glob->params.NOISE_REDUCTION;
	
	// -maxrate ; rate control max rate; Here it is in bps
	glob->codecCont->rc_max_rate = glob->params.RC_MAXRATE * 1024;
	
	// -refs ; reference frames to consider for motion compensation
	glob->codecCont->refs = glob->params.REFS;
	
#if X264
	// -me_range ; limit motion vectors range
	glob->codecCont->me_range = glob->params.ME_RANGE;
#endif
	
	// -coder ; 1.CAVLC 2.CABAC
	switch( glob->params.CODER_TYPE ) {
	case 1:
		glob->codecCont->coder_type = FF_CODER_TYPE_VLC;
		break;
	case 2:
		glob->codecCont->coder_type = FF_CODER_TYPE_AC;
		break;
	default:
		glob->codecCont->coder_type = FF_CODER_TYPE_VLC;
		break;
	}
	
	// -directpred ; direct mv prediction mode
	switch( glob->params.DIRECTPRED ) {
	case 1:
		glob->codecCont->directpred = 0;	// X264_DIRECT_PRED_NONE;
		break;
	case 2:
		glob->codecCont->directpred = 1;	// X264_DIRECT_PRED_SPATIAL;
		break;
	case 3:
		glob->codecCont->directpred = 2;	// X264_DIRECT_PRED_TEMPORAL;
		break;
	default:
		glob->codecCont->directpred = 3;	// X264_DIRECT_PRED_AUTO;
		break;
	}
	
	// -b_strategy ; strategy to choose between I/P/B-frames
	glob->codecCont->b_frame_strategy = glob->params.ADAPTIVE_BFRAME;
	
	// -subq ; sub pel motion estimation quality
	glob->codecCont->me_subpel_quality = glob->params.ME_SUBQ;
	
	// -trellis ; use trellis quantization
	switch( glob->params.TRELLIS ) {
	case 1:
		glob->codecCont->trellis = 0;	// Disabled
		break;
	case 2:
		glob->codecCont->trellis = 1;	// Final Only
		break;
	case 3:
		glob->codecCont->trellis = 2;	// All
		break;
	default:
		glob->codecCont->trellis = 0;	// Disabled
		break;
	}
	
	// (-turbo) ; Faster first pass mode
	// no code here... but in EncodeFrame()
	
	// -cmp 256; chroma ME for subpel and mode decision in P-frames
	if( glob->params.CHROMA_ME )		glob->codecCont->me_cmp |= FF_CMP_CHROMA;
	else								glob->codecCont->me_cmp &= ~ FF_CMP_CHROMA;
	
	// -partp4x4 ; Analyse p8x4, p4x8, p4x4
	if( glob->params.PART_4X4 )			glob->codecCont->partitions |= X264_PART_P4X4;
	else								glob->codecCont->partitions &= ~ X264_PART_P4X4;
	
	// -bidir_refine ; jointly optimize both MVs in B-frames
	glob->codecCont->bidir_refine = (glob->params.BIDIR_ME ? 4 : 1);
	
	// Use third pass mode
	// no code here...
	
	// -level ; H.264/AVC Level
	if( glob->params.LEVEL > 9 )
		glob->codecCont->level = glob->params.LEVEL;
	else {
		assert(glob->params.LEVEL==9);
		glob->codecCont->level = 0;
	}
	
	// -aq_mode
	glob->codecCont->aq_mode = glob->params.AQ_MODE - 1;
	
	// -lumi_mask; luminance masking
	glob->codecCont->lumi_masking = glob->params.LUMI_MASKING * 1.0;
	
	// -bufsize ; rate control buffer size; Here it is in bit
	glob->codecCont->rc_buffer_size = glob->params.RC_BUFSIZE * 1024;
	if (glob->params.RC_BUFSIZE) {
#if X264
		glob->codecCont->rc_initial_buffer_occupancy = glob->params.RC_BUFSIZE * 1024 * 0.90;
#else 
		glob->codecCont->rc_initial_buffer_occupancy = glob->params.RC_BUFSIZE * 1024 * 3 / 4;
#endif
	}
	
	// -deblockalpha, -deblockbeta ;
	glob->codecCont->deblockalpha = glob->params.DEBLOCK_ALPHA;
	glob->codecCont->deblockbeta = glob->params.DEBLOCK_BETA;
	
	// -aq_strength
	glob->codecCont->aq_strength = glob->codecCont->aq_mode ? glob->params.AQ_STRENGTH / 100.0 : 0;
	
	// -psy_rd
	glob->codecCont->psy_rd = glob->params.PSYRD_RDO / 10.0;
	
	// -psy_trellis
	glob->codecCont->psy_trellis = glob->params.PSYRD_TRELLIS / 10.0;
	
	// -keyint_min; KEYINT_MIN
	glob->codecCont->keyint_min = glob->params.KEYINT_MIN;
	
	// x264; --cqm
	glob->params_opaque.CQM_PRESET = glob->params.CQM_PRESET;
	
	// x264; --dct-decimate
	glob->params_opaque.NO_DCT_DECIMATE = glob->params.NO_DCT_DECIMATE;
	
	// -qdiff; 
	glob->codecCont->max_qdiff = glob->params.MAX_QDIFF;
	
	// -chromaoffset;
	glob->codecCont->chromaoffset = glob->params.CHROMAOFFSET;
	
	// x264; --profile
#if X264
	glob->codecCont->profile = glob->params.X264PROFILE;
#endif
	
	// x264; --preset
	glob->params_opaque.X264PRESET = glob->params.X264PRESET;
	
	// x264; --tune
	glob->params_opaque.X264TUNE = glob->params.X264TUNE;
	
	// x264; --rc-lookahead
	glob->codecCont->rc_lookahead = glob->params.RC_LOOKAHEAD;
	
	// inverse of -i_qfactor (1/n); not p-to-i but i-to-p; ex. IP_FACTOR = 1.4 * 100
	glob->codecCont->i_quant_factor = 100.0 / glob->params.IP_FACTOR;
	
	// -b_qfactor; p-to-b; ex. PB_FACTOR = 1.3 * 100
	glob->codecCont->b_quant_factor = glob->params.PB_FACTOR / 100.0;
	
	// x264; --weightp
	glob->codecCont->weighted_p_pred = glob->params.WEIGHTP - 1;
	
	// -aspect
	if( glob->params.USEASPECTRATIO ) {
		glob->codecCont->sample_aspect_ratio.num = glob->params.hSpacing;
		glob->codecCont->sample_aspect_ratio.den = glob->params.vSpacing;
	} else {
		glob->codecCont->sample_aspect_ratio = (AVRational){1,1};
	}
	
	// Pass TOP Field First setting to libx264.c
	glob->params_opaque.TOPFIELDFIRST = glob->params.TOPFIELDFIRST;
	
	// BD specific tuning flag
	glob->params_opaque.BD_TUNE = glob->params.BD_TUNE;
	
	// x264; --fake-interlaced
	glob->params_opaque.FAKEINTERLACED = glob->params.FAKEINTERLACED;
	
	/* ==================================== */
	
	// Only I frame?
	glob->only_I_frame = !(ICMCompressionSessionOptionsGetAllowTemporalCompression( glob->sessionOptions ));
	
	// B frame support
	glob->use_B_frame = ICMCompressionSessionOptionsGetAllowFrameReordering( glob->sessionOptions );
	
	// -g ; default 12 ; Key frame interval
	if( glob->only_I_frame ) {
		glob->codecCont->gop_size = 0;
	} else {
		int key_interval = ICMCompressionSessionOptionsGetMaxKeyFrameInterval( glob->sessionOptions );
		if( key_interval ) {
			key_interval = ((key_interval < 2) ? 2 : key_interval);
//			key_interval = ((key_interval > 300) ? 300 : key_interval);
			
			glob->codecCont->gop_size = key_interval;
		} else {
#if 0
			;		// Leave as default; 12 is libavcodec's default value
#else
			glob->codecCont->gop_size = 60;
#endif
		}
		
		if (!glob->codecCont->keyint_min) glob->codecCont->keyint_min = glob->codecCont->gop_size / 10;
	}
	
	// -bf ; use 'frames' B frames
	if( glob->use_B_frame ) {
		glob->codecCont->max_b_frames = glob->params.MAX_BFRAMES;
#if MPEG4
		if(glob->codecCont->flags & CODEC_FLAG_CLOSED_GOP)
			glob->codecCont->scenechange_threshold = INT32_MAX;
//			glob->codecCont->flags2 |= CODEC_FLAG2_STRICT_GOP;
#endif			
	} else {
		glob->codecCont->max_b_frames = 0;
		
		// should be off when no B-frame
		glob->codecCont->flags &= ~ CODEC_FLAG_CLOSED_GOP;
		glob->codecCont->flags2 &= ~ CODEC_FLAG2_BPYRAMID;
		glob->codecCont->flags2 &= ~ CODEC_FLAG2_BRDO;
	}
	
	// -b ; set video bitrate (in bits/s)
	SInt32 data_rate = 0;
	err = ICMCompressionSessionOptionsGetProperty(glob->sessionOptions,
		kQTPropertyClass_ICMCompressionSessionOptions,
		kICMCompressionSessionOptionsPropertyID_AverageDataRate,
		sizeof(data_rate),
		&data_rate,
		NULL);
	if( err ) {
		fprintf(stderr, "ERROR: Get property average data rate failed.\n");
		err = paramErr;
		goto bail;
	}
	if( data_rate && !(glob->params.FLAG_QSCALE || glob->params.CRF) ) {
		data_rate = ((data_rate < 8192) ? 8192 : data_rate);		// 64Kbps = 8KByte/sec
		data_rate = ((data_rate > 31457280) ? 31457280 : data_rate);	// 240Mbps = 30MByte/sec
		
		glob->codecCont->bit_rate = data_rate * 8;
	} else {
		glob->codecCont->bit_rate = 786432;							// 768Kbps = 128KByte/sec
#if X264
		glob->params.CRF = TRUE;
		if(glob->ICM_passcount != -1) {
			glob->fakeMultipass = TRUE;
			logDebug(glob, "lavcEncoder: - Force CRF; No data_rate is specified.\n");			
		}
#endif
#if MPEG4|XVID
		glob->params.FLAG_QSCALE = TRUE;
		glob->codecCont->flags |= CODEC_FLAG_QSCALE;	// Force QSCALE flag on
		if(glob->ICM_passcount != -1) {
			glob->fakeMultipass = TRUE;
			logDebug(glob, "lavcEncoder: - Force QSCALE; No data_rate is specified.\n");
		}
#endif
	}
	
	// -bt ; set video bitrate tolerance (in bits/s)
#if X264
	glob->codecCont->bit_rate_tolerance = glob->codecCont->bit_rate;
#else
	if( glob->codecCont->bit_rate_tolerance < glob->codecCont->bit_rate * 1.25 ) {
		glob->codecCont->bit_rate_tolerance = glob->codecCont->bit_rate * 1.25;
	}
#endif
	
	// Set quality values
	CodecQ quality;
	err = ICMCompressionSessionOptionsGetProperty(glob->sessionOptions,
		kQTPropertyClass_ICMCompressionSessionOptions,
		kICMCompressionSessionOptionsPropertyID_Quality,
		sizeof(quality),
		&quality,
		NULL);
	if( err ) {
		fprintf(stderr, "ERROR: Get property quality failed.\n");
		err = paramErr;
		goto bail;
	}
	
	// -qmin,-qmax ; mpeg4: 2-31 ; x264: 2-51
	// crf ; x264:1.0-50.0?
	// qscale ; mpeg4:>0.0, <=255.0
	
#if X264
	// -crf ; default 0 ; enables constant quality mode, and selects the quality
	if( glob->params.LOSSLESS ) {
		glob->codecCont->qmin = 0;
		glob->codecCont->qmax = 51;
		glob->codecCont->crf = 0;
		glob->codecCont->cqp = 0;				
	} else if( glob->params.CRF ) {
		// crf  : L(33-28-23-18-13)H
		if(glob->params.OVERRIDECRFQSCALE)
			glob->codecCont->crf = glob->params.USERCRFQSCALE;
		else
			glob->codecCont->crf = 33.0 - (20.0 * quality ) / codecLosslessQuality;
		
		if(glob->params.OVERRIDEQMIN)
			glob->codecCont->qmin = glob->params.USERQMIN;
		else
			glob->codecCont->qmin = 4;	// 0 is x264 default, but Apple H.264 decoder is XXXX...
		glob->codecCont->qmax = 51;	// x264 default
	} else {
		// qmin : L(33-28-23-18-13)H
		// qmax : L(51-51-51-51-51)H
		if(glob->params.OVERRIDEQMIN)
			glob->codecCont->qmin = glob->params.USERQMIN;
		else
			glob->codecCont->qmin = 33.0 - (20.0 * quality ) / codecLosslessQuality;
		
		glob->codecCont->qmax = 51;	// x264 default
		
//		Boolean keyExistsAndHasValidFormat = FALSE;
//		CFStringRef appID = CFSTR("com.MyCometG3.x264Encoder");
//		CFStringRef key = CFSTR("update_qmin_on_abr");
//		CFPreferencesAppSynchronize(appID);
//		Boolean result = CFPreferencesGetAppBooleanValue(key, appID, &keyExistsAndHasValidFormat);
//		if(keyExistsAndHasValidFormat && result) {
//			// qmin : L(33-28-23-18-13)H
//			// qmax : L(51-51-51-51-51)H
//			glob->codecCont->qmin = 33.0 - (20.0 * quality ) / codecLosslessQuality;
//			logInfo(glob, "lavcEncoder: - Modify qmin/qmax to %4.1f/%4.1f.\n", glob->codecCont->qmin, glob->codecCont->qmax);
//		}
	}
	
#endif
#if MPEG4|XVID
	// -qscale ; default 0 ; use fixed qscale
	if( glob->params.FLAG_QSCALE ) {
		// global_quality : L(10.0-  -6.0-  -2.0)H
		if(glob->params.OVERRIDECRFQSCALE)
			glob->codecCont->global_quality = glob->params.USERCRFQSCALE;
		else {
			float video_qscale = 10.0-8.0*quality/codecLosslessQuality;
			glob->codecCont->global_quality = FF_QP2LAMBDA * video_qscale;
		}

		glob->codecCont->qmin =  2;	// ffmpeg default
		glob->codecCont->qmax = 31;	// ffmpeg default
	} else {
		// qmin : L(10- 8- 6- 4- 2)H
		// qmax : L(16-14-12-10- 8)H
		if(glob->params.OVERRIDEQMIN)
			glob->codecCont->qmin = glob->params.USERQMIN;
		else
			glob->codecCont->qmin = 10.5 - (8.0 * quality) / codecLosslessQuality;
		
#if 0
 		glob->codecCont->qmax = glob->codecCont->qmin + 6;
 		glob->codecCont->qmax = (glob->codecCont->qmax > 31.0) ? 31.0 : glob->codecCont->qmax;
#else
		glob->codecCont->qmax = 31;	// ffmpeg default
#endif
	}
#endif
	
	// Set frame rate
	// -r ; default 25/1 ; constant frame rate
	Fixed frame_rate;
	err = ICMCompressionSessionOptionsGetProperty(glob->sessionOptions,
		kQTPropertyClass_ICMCompressionSessionOptions,
		kICMCompressionSessionOptionsPropertyID_ExpectedFrameRate,
		sizeof(frame_rate),
		&frame_rate,
		NULL);
	if( err ) {
		fprintf(stderr, "ERROR: Get property expected frame rate failed.\n");
		err = paramErr;
		goto bail;
	}
	if( !frame_rate ) {
		// this is a temporal value just when codec is not opened
		// If user does not specify expected framerate, rate control may not work well
		
		switch( glob->params.NATIVE_FPS ) {
		case 1:
			frame_rate=FloatToFixed(30000.0/1001);
			break;
		case 2:
			frame_rate=FloatToFixed(25.0);
			break;
		case 3:
			frame_rate=FloatToFixed(24000.0/1001);
			break;
		case 11:
			frame_rate=FloatToFixed(30000.0/1001/2);
			break;
		case 12:
			frame_rate=FloatToFixed(25.0/2);
			break;
		case 13:
			frame_rate=FloatToFixed(24000.0/1001/2);
			break;
		case 21:
			frame_rate=FloatToFixed(30000.0/1001*2);
			break;
		case 22:
			frame_rate=FloatToFixed(25.0*2);
			break;
		case 23:
			frame_rate=FloatToFixed(24000.0/1001*2);
			break;
		default:
			frame_rate=FloatToFixed(30000.0/1001);
			break;
		}
	}
	
	// Fixed frame rate
	float fr = FixedToFloat(frame_rate);
	if( abs(1000*fr - 29970) < 10 ) {
		glob->codecCont->time_base = (AVRational){1001, 30000};			// pts count up by 1
	} else 
	if( abs(1000*fr - 23976) < 10 ) {
		glob->codecCont->time_base = (AVRational){1001, 24000};			// pts count up by 1
	} else 
	if( abs(1000*fr - 59940) < 10 ) {
		glob->codecCont->time_base = (AVRational){1001, 60000};			// pts count up by 1
	} else 
	if( abs(1000*fr - 47952) < 10 ) {
		glob->codecCont->time_base = (AVRational){1001, 48000};			// pts count up by 1
	} else {
		glob->codecCont->time_base = (AVRational){30000.0/fr, 30000};	// pts count up by 1
	}
	
	int i= av_gcd(glob->codecCont->time_base.den, glob->codecCont->time_base.num);
	if(i > 1){
		glob->codecCont->time_base.den /= i;
		glob->codecCont->time_base.num /= i;
	}
	
	// Update color information in codecCont
	if( glob->add_nclc ) {
		switch (glob->params.NCLC) {
		case 3:	// 616
			glob->codecCont->color_primaries =	AVCOL_PRI_SMPTE170M;	// kQTPrimaries_SMPTE_C;
			glob->codecCont->color_trc =		AVCOL_TRC_BT709;		// kQTTransferFunction_ITU_R709_2;
			glob->codecCont->colorspace =		AVCOL_SPC_SMPTE170M;	// kQTMatrix_ITU_R_601_4;
			break;
		case 4:	// 516
			glob->codecCont->color_primaries =	AVCOL_PRI_BT470BG;		// kQTPrimaries_EBU_3213;
			glob->codecCont->color_trc =		AVCOL_TRC_BT709;		// kQTTransferFunction_ITU_R709_2;
			glob->codecCont->colorspace =		AVCOL_SPC_SMPTE170M;	// kQTMatrix_ITU_R_601_4;
			break;
		case 5:	// 677
			glob->codecCont->color_primaries =	AVCOL_PRI_SMPTE170M;	// kQTPrimaries_SMPTE_C;
			glob->codecCont->color_trc =		7;						// kQTTransferFunction_SMPTE_240M_1995;
			glob->codecCont->colorspace =		AVCOL_SPC_SMPTE240M;	// kQTMatrix_SMPTE_240M_1995;
			break;
		case 6:	// 111
			glob->codecCont->color_primaries =	AVCOL_PRI_BT709;		// kQTPrimaries_ITU_R709_2;
			glob->codecCont->color_trc =		AVCOL_TRC_BT709;		// kQTTransferFunction_ITU_R709_2;
			glob->codecCont->colorspace =		AVCOL_SPC_BT709;		// kQTMatrix_ITU_R_709_2;
			break;
		case 7: // 222
			glob->codecCont->color_primaries =	AVCOL_PRI_UNSPECIFIED;	// kQTPrimaries_Unknown;
			glob->codecCont->color_trc =		AVCOL_TRC_UNSPECIFIED;	// kQTTransferFunction_Unknown;
			glob->codecCont->colorspace =		AVCOL_SPC_UNSPECIFIED;	// kQTMatrix_Unknown;
			break;
		default:
			break;
		}
	}
	
	err = noErr;
bail:
	return err;
}

static OSStatus open_libAV(lavcEncoderGlobalRecord *glob)
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	OSStatus err = paramErr;
	
	// In case 'specified bit rate is too low', EncodeFrame failed at EVERY frame at pass 2.
	if( glob->codec_open_failed ) return codecErr;
	
	logDebug(glob, "lavcEncoder: Open codec...\n");
	
	if( glob->ICM_passcount == 1 ) {
#if X264
		int turbo=glob->params.TURBO - 1;
		// Faster first-pass mode (would be same effect as: mplayer/libmpcodecs/ve_x264.c:x264enc_set_param)
		if( turbo == 1 ) {
			glob->codecCont->refs = (glob->codecCont->refs + 1) >> 1;
			glob->codecCont->me_subpel_quality = FFMAX(FFMIN(3, glob->codecCont->me_subpel_quality-1), 1);
			glob->codecCont->partitions	&= ~ X264_PART_P4X4;
			glob->codecCont->partitions &= ~ X264_PART_B8X8;
			glob->codecCont->trellis = 0;
		} else 
		if( turbo == 2 ) {
			glob->codecCont->refs = 1;							// param.i_frame_reference = 1;
			glob->codecCont->flags2 &= ~ CODEC_FLAG2_8X8DCT;	// param.analyse.b_transform_8x8 = 0;
			glob->codecCont->partitions = 0;					// param.analyse.inter = 0;
			glob->codecCont->me_method = ME_EPZS;				// param.analyse.i_me_method = X264_ME_DIA;
			glob->codecCont->me_subpel_quality = FFMIN(2, glob->codecCont->me_subpel_quality);
																// param.analyse.i_subpel_refine = X264_MIN( 2, param->analyse.i_subpel_refine );
			glob->codecCont->trellis = 0;						// param.analyse.i_trellis = 0;
			glob->codecCont->flags2 |= CODEC_FLAG2_FASTPSKIP;	// param->analyse.b_fast_pskip = 1;
		}
		if( turbo ) logInfo(glob, "lavcEncoder: - Faster FirstPass: Mode %d\n", turbo);
#endif
#if MPEG4
		int turbo=glob->params.TURBO - 1;
		// Faster first-pass mode (would be same effect as: mplayer/libmpcodecs/ve_lavc.c:config)
		if( turbo ) {
			glob->codecCont->me_pre_cmp = 0;					// lavc_venc_context->me_pre_cmp = 0; 
			glob->codecCont->me_cmp = 0;						// lavc_venc_context->me_cmp = 0; 
			glob->codecCont->me_sub_cmp = 0;					// lavc_venc_context->me_sub_cmp = 0; 
			glob->codecCont->mb_cmp = 2;						// lavc_venc_context->mb_cmp = 2; 
			
			glob->codecCont->pre_dia_size = 0;					// lavc_venc_context->pre_dia_size = 0;
			glob->codecCont->dia_size = 1;						// lavc_venc_context->dia_size = 1;
			
			glob->codecCont->quantizer_noise_shaping = 0;		// lavc_venc_context->quantizer_noise_shaping = 0; // qns=0
			glob->codecCont->noise_reduction = 0;				// lavc_venc_context->noise_reduction = 0; // nr=0
			glob->codecCont->mb_decision = 0;					// lavc_venc_context->mb_decision = 0; // mbd=0
			
			glob->codecCont->flags &= ~ CODEC_FLAG_QPEL;		// lavc_venc_context->flags &= ~CODEC_FLAG_QPEL;
			glob->codecCont->flags &= ~ CODEC_FLAG_4MV;			// lavc_venc_context->flags &= ~CODEC_FLAG_4MV;
			glob->codecCont->trellis = 0;						// lavc_venc_context->trellis = 0;
			glob->codecCont->flags &= ~ CODEC_FLAG_CBP_RD;		// lavc_venc_context->flags &= ~CODEC_FLAG_CBP_RD;
			glob->codecCont->flags &= ~ CODEC_FLAG_QP_RD;		// lavc_venc_context->flags &= ~CODEC_FLAG_QP_RD;
			glob->codecCont->flags &= ~ CODEC_FLAG_MV0;			// lavc_venc_context->flags &= ~CODEC_FLAG_MV0;
		}
		if( turbo ) logInfo(glob, "lavcEncoder: - Faster FirstPass: Enabled\n");
#endif
	}
#if MPEG4
	if( glob->ICM_passcount == 2 ) {
			glob->codecCont->b_frame_strategy = 0;
	}
#endif
	
	// 
	logInfo(glob, "lavcEncoder: - Source PixelBuffer: Native {%d/%d}, RoundedUp16 {%d/%d}\n", 
				glob->width, glob->height, glob->widthRoundedUp, glob->heightRoundedUp);
	logInfo(glob, "lavcEncoder: - {width/height}: {%d/%d} ; time_base: {%d/%d} ; rate: %.3f\n", 
				glob->codecCont->width, glob->codecCont->height, 
				glob->codecCont->time_base.num, glob->codecCont->time_base.den, 
				(double)glob->codecCont->time_base.den / glob->codecCont->time_base.num
				);
	logInfo(glob, "lavcEncoder: - bit_rate: %d Kbps; {qmin/qmax/qscale/crf}: {%d/%d/%.2f/%.2f}\n", 
				glob->codecCont->bit_rate/1024, glob->codecCont->qmin, glob->codecCont->qmax, 
				(float)glob->codecCont->global_quality/FF_QP2LAMBDA, glob->codecCont->crf);
	logInfo(glob, "lavcEncoder: - gop_size: %d ; max_b_frames: %d ; qcomp: %0.2f ; sc_threshold: %d\n", 
				glob->codecCont->gop_size, glob->codecCont->max_b_frames, 
				glob->codecCont->qcompress, glob->codecCont->scenechange_threshold);
	logInfo(glob, "lavcEncoder: - thread_count: %d ; add_nclc: %s ; add_gamma: %s\n", 
				glob->codecCont->thread_count, 
				(glob->add_nclc ? "ON" : "OFF"), (glob->add_gamma ? "ON" : "OFF"));
	logInfo(glob, "lavcEncoder: - bit_rate_tolerance: %d Kbit ; rc_buffer_size: %d Kbit. ; rc_max_rate = %d Kbps\n"
				, glob->codecCont->bit_rate_tolerance/1024
				, glob->codecCont->rc_buffer_size/1024, glob->codecCont->rc_max_rate/1024);
	
#if USECoreVF
	if( glob->filterString ) {
		CFIndex len = CFStringGetLength(glob->filterString);
		if(len) {
			CFIndex max = CFStringGetMaximumSizeForEncoding(len, kCFStringEncodingASCII);
			char* filterCString = calloc(1, max + 1);
			CFStringGetCString(glob->filterString, filterCString, max + 1, kCFStringEncodingASCII);
			logInfo(glob, "lavcEncoder: - filter: [ %s ]\n", filterCString);
			free(filterCString);
		}
	}
#endif
#if !WRITELOG
	// Pass log-file-path to x264 (with patched x264.c); log file is handled by x264 itself
	// Pass AQ values to x264 (with patched x264.c); AQ is not implemented in libavcodec/libx264.c yet.
	glob->codecCont->opaque = &(glob->params_opaque);
#endif
	
	{
		// Set x264 Log level (set before opening codec)
		av_log_set_level(AV_LOG_QUIET);
		if( glob->params.LOG_INFO ) 
			av_log_set_level(AV_LOG_INFO);
		if( glob->params.LOG_DEBUG ) 
			av_log_set_level(AV_LOG_VERBOSE);
		
#if !X264
		// Thread support
		if (glob->codecCont->thread_count == 0) {
			int numCpu = 0;
			size_t length = sizeof(numCpu);
			int select[2] = { CTL_HW, HW_NCPU };
			int result = sysctl(select, 2, &numCpu, &length, NULL, 0);
			if(!result && numCpu) {
				glob->codecCont->thread_count = numCpu;				// Ex. Two core -> two thread
			} else {
				glob->codecCont->thread_count = 2;
			}
		}
#endif
		
		// Make it ON before opening codec to get extradata; No SPS/PPS in data stream
		glob->codecCont->flags |= CODEC_FLAG_GLOBAL_HEADER;
		
		// libavcodec open codec
		glob->codecCont->codec_id = glob->codec->id;
		glob->codecCont->codec_type = glob->codec->type;
		err = avcodec_open(glob->codecCont, glob->codec);
		if( err < 0 ) {
			fprintf(stderr, "ERROR: Opening codec failed.\n");
			glob->codec_open_failed = TRUE;
			goto bail;
		}
		glob->codec_is_opened = TRUE;

#if 0	//MPEG4
		// Some decoder like 3ivx needs esds info with every I-frame...sigh.
		// xvidcore does not support on-the-fly update of this flag.
		glob->codecCont->flags &= ~CODEC_FLAG_GLOBAL_HEADER;
#endif
		
		// Adjust libavcodec default log level
		av_log_set_level(AV_LOG_QUIET);
		if( glob->params.LOG_INFO ) 
			av_log_set_level(AV_LOG_INFO);
		if( glob->params.LOG_DEBUG ) 
			av_log_set_level(AV_LOG_VERBOSE);
	}
	
	err = noErr;
bail:	
	return err;
}

static OSStatus process_libAV(lavcEncoderGlobalRecord *glob, ICMCompressorSourceFrameRef source_frame)
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	OSStatus err = noErr;
	
#if TESTVFR
	TimeValue64 displayTimeStamp, displayDuration;
	TimeScale timeScale;
	ICMCompressorSourceFrameGetDisplayTimeStampAndDuration( source_frame, &displayTimeStamp, &displayDuration, &timeScale, NULL);	
	
	// Pass a AVFrame into codec's buffer
	glob->frame->pts = displayTimeStamp;
#endif
#if MPEG4|XVID
	glob->frame->quality = ( glob->params.FLAG_QSCALE ? glob->codecCont->global_quality : glob->frame->quality );
#endif
	glob->frame_size = avcodec_encode_video(glob->codecCont, 
						glob->encode_buffer, glob->encode_buffer_size, glob->frame);
//	logDebug(glob, "lavcEncoder: glob->frame_size: %010d\n", glob->frame_size);
	
	/* ==================================== */
	
	// Prepare ExtraData(avcC/esds) ImageDescription extension
	if( !glob->extraDataReady && glob->codecCont->extradata_size ) {
		// remove previous ExtraData atom
		while(TRUE) {
			if( RemoveImageDescriptionExtension(glob->imageDescription, kExtraDataTag, 1) ) break;
		}
		
		// Add extradata atom as ImageDescription extension
		Handle extraDataHdl = NULL;
#if X264
		extraDataHdl = avcCExtension(glob);
#endif
#if MPEG4|XVID
		extraDataHdl = esdsExtension(glob);
#endif
		if( extraDataHdl ) {
			HLock(extraDataHdl);
			err = AddImageDescriptionExtension(glob->imageDescription, extraDataHdl, kExtraDataTag);
			if( err ) {
				fprintf(stderr, "ERROR: Adding extradata failed.\n");
				err = paramErr;
			}
			HUnlock(extraDataHdl);
			DisposeHandle(extraDataHdl);
		} else {
			fprintf(stderr, "ERROR: Getting extradata failed.\n");
			err = paramErr;
		}
		if( err ) goto bail;
		
		glob->extraDataReady = TRUE;
	}	// ( !glob->extraDataReady && glob->codecCont->extradata_size )
	
	/* ==================================== */
	
	// Check encoded frame
	if( glob->frame_size > 0 ) {
		if( glob->emit_frame ) {			// Pass 2
			err = emitFrameData(glob);
		} else {							// Pass 1
			Boolean ret = removeSourceFrameForEncodedFrame(glob);
			if( ret ) err = noErr;
		}
		
		// log file support
		if( glob->logfile ) fprintf( glob->logfile, "%s", glob->codecCont->stats_out );
	} else {
		// Encoded frames may be emitted later
		err = noErr;
		glob->delayCount++;
	}
	
bail:
	return err;
}


// Write out encoded AVFrame into storage
static OSStatus emitFrameData(lavcEncoderGlobalRecord *glob)
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	OSStatus err = noErr;
	ICMCompressorSourceFrameRef source_frame = NULL;
	ICMMutableEncodedFrameRef encoded_frame = NULL;
	
	ICMFrameType frame_type = kICMFrameType_Unknown;
	MediaSampleFlags media_sample_flags = 0;
	
	TimeValue64 displayDurationOut, displayTimeStampOut;
	TimeScale timeScaleOut;
	ICMValidTimeFlags valid_time_flag;
	
	// 
	if( glob->codecCont->coded_frame == NULL ) {
		// Returned non-frame data??? header or trailer?
		fprintf(stderr, "ERROR: Non-frame data found.\n");
		err = paramErr;
		goto bail;
	}
	
	/* ==================================== */
	
	{
		// Get the source frame from array_source_frame using glob->codecCont->coded_frame->pts
		source_frame = getSourceFrameForEncodedFrame(glob);
		if( !source_frame ) {
			fprintf(stderr, "ERROR: getSourceFrameForEncodedFrame() failed.\n");
			err = paramErr;
			goto bail;
		}
		
		// Get source frame stats
		err = ICMCompressorSourceFrameGetDisplayTimeStampAndDuration(source_frame, 
						&displayTimeStampOut, &displayDurationOut, &timeScaleOut, &valid_time_flag);
		if( err ) {
			fprintf(stderr, "ERROR: ICMCompressorSourceFrameGetDisplayTimeStampAndDuration() failed.\n");
			err = paramErr;
			goto bail;
		}
		
#if TESTVFR
		// update fields; x264 wrapper does not update these field
		long displayNumber = ICMCompressorSourceFrameGetDisplayNumber(source_frame);
		glob->codecCont->coded_frame->display_picture_number = displayNumber - 1;
		glob->codecCont->coded_frame->coded_picture_number = glob->frame_count - glob->delayCount - 1;
#if 0
		logDebug(glob, "lavcEncoder: [%s] (%d, %lld, %lld, %d)\n", __FUNCTION__
					, displayNumber, displayTimeStampOut, displayDurationOut, timeScaleOut);
#endif
#endif
#if 0
		logDebug(glob, 
			"lavcEncoder: encoded pts = %lld"
			" coded num = %d"
			" display num = %d"
			" frame = %c"
			" keyframe = %s"
			" quality = %d"
			" \n"
			, glob->codecCont->coded_frame->pts
			, glob->codecCont->coded_frame->coded_picture_number
			, glob->codecCont->coded_frame->display_picture_number
			, av_get_pict_type_char(glob->codecCont->coded_frame->pict_type)
			, ( glob->codecCont->coded_frame->key_frame ? "TRUE" : "FALSE" )
			, glob->codecCont->coded_frame->quality
			);
#endif
	}
	
	/* ==================================== */
	
	if( glob->params.LOG_STATS ) {
		char tmp[256] = {0};
		char buf[4096] = {0};
		snprintf(tmp,256, "STAT :%5d/%3lld+%8lld;", (int)timeScaleOut, displayDurationOut, displayTimeStampOut);
		strncat(buf, tmp, 4096);
		
		// Show stats of every frame
		double ti1, ti2, bitrate, avg_bitrate;
		
		if (glob->codecCont->coded_frame->quality > 100*(float)FF_QP2LAMBDA) {
			snprintf(tmp,256, " frame=%6d q=**.* ", 
				glob->codecCont->coded_frame->display_picture_number + 1);
		} else {
			snprintf(tmp,256, " frame=%6d q=%4.1f ", 
				glob->codecCont->coded_frame->display_picture_number + 1, 
				glob->codecCont->coded_frame->quality/(float)FF_QP2LAMBDA);
		}
		strncat(buf, tmp, 4096);
#if MPEG4
		if (glob->codecCont->flags&CODEC_FLAG_PSNR) {
#if 1
			// ref: ffmpeg.c do_video_stats()
			snprintf(tmp,256, "PSNR= %6.2f ", 
				psnr(glob->codecCont->coded_frame->error[0]
						/ (glob->codecCont->width * glob->codecCont->height * 255.0 * 255.0)));
			strncat(buf, tmp, 4096);
#else
			// ref: ffmpeg.c print_report()
			int j;
			double error, error_sum=0;
			double scale, scale_sum=0;
			char type[3]= {'Y','U','V'};
			for(j=0; j<3; j++){
				error= glob->codecCont->coded_frame->error[j];
				scale= glob->codecCont->width*glob->codecCont->height*255.0*255.0;
				if(j) scale/=4;
				error_sum += error;
				scale_sum += scale;
				snprintf(tmp,256, "%c:%2.2f ", type[j],psnr(error/scale));
				strncat(buf, tmp, 4096);
			}
			snprintf(tmp,256, "*:%2.2f ", psnr(error_sum/scale_sum));
			strncat(buf, tmp, 4096);
#endif
		}
#endif
		snprintf(tmp,256,"f_size=%6d ", glob->frame_size);
		strncat(buf, tmp, 4096);
		
		ti1 = glob->codecCont->coded_frame->display_picture_number 
				* av_q2d(glob->codecCont->time_base);
		ti2 = (glob->codecCont->coded_frame->display_picture_number + 1)
				* av_q2d(glob->codecCont->time_base);
		
		glob->video_size += glob->frame_size;
		bitrate = (glob->frame_size * 8) / av_q2d(glob->codecCont->time_base) / 1024.0;
		avg_bitrate = (double)(glob->video_size * 8) / ti2 / 1024.0;
		
		snprintf(tmp,256, "s_size=%9.0fKB time= %7.2f br= %7.1fKbps avg_br= %7.1fKbps ",
			(double)glob->video_size / 1024, ti1, bitrate, avg_bitrate);
		strncat(buf, tmp, 4096);
		
		snprintf(tmp,256,"type= %c%s\n"
			, av_get_picture_type_char(glob->codecCont->coded_frame->pict_type)
			, ((glob->codecCont->coded_frame->key_frame>0) ? "DR" : "")
			);
		strncat(buf, tmp, 4096);
		fprintf(stderr, "%s", buf);
	}
	
	/* ==================================== */
	
	{
		int droppable_frame = false;
		int partial_sync_frame = false;
//		fprintf(stderr, "codedFrameNum(%d) ",glob->codecCont->coded_frame->coded_picture_number);
		
		// Specify frame properties from encoded AVFrame
		int pict_type = 0;
		int key_frame = 0;
		
		pict_type = glob->codecCont->coded_frame->pict_type;
		key_frame = glob->codecCont->coded_frame->key_frame;
		Boolean reordering = doesQueueContainEarlierDisplayNumbers(glob, ICMCompressorSourceFrameGetDisplayNumber(source_frame));

		if( key_frame && reordering ) 
//			if( (glob->codecCont->max_b_frames > 0) && (glob->codecCont->flags & CODEC_FLAG_CLOSED_GOP) == 0 )
				key_frame = FALSE;
#if X264
		if( !key_frame && pict_type == FF_I_TYPE) {
			/* QuickTime always treats partial sync as MPEG-4 ASP style; i.e. OpenGOP decoding. */
			/* When seek, it start decoding from sync or two partial sync frame to target frame*/
			/* This is not suitable for H.264 stream. */
			/* Non-IDR I frame should NOT be tagged as partial sync. */
			pict_type = FF_P_TYPE;
		}	
#endif
		
		//
		if( key_frame ) {							// 1st I frame (of closed gop)
			frame_type = kICMFrameType_I;			// Diff btw lavc and ICM
			partial_sync_frame = false;
			droppable_frame = false;
		} else if( pict_type == FF_I_TYPE ) {		// 1st I frame (of open gop) or Non IDR I frame
			frame_type = kICMFrameType_I;			// Diff btw lavc and ICM
			partial_sync_frame = true;
			droppable_frame = false;
		} else if( pict_type == FF_P_TYPE ) {		// P frame (Predictive)
			frame_type = kICMFrameType_P;
			partial_sync_frame = false;
			droppable_frame = false;
		} else if( pict_type == FF_B_TYPE ) {		// B frame (Bi-directional predictive)
			frame_type = kICMFrameType_B;
			partial_sync_frame = false;
#if MPEG4|XVID
			droppable_frame = TRUE;
#endif
#if X264
			if((glob->encode_buffer[4]==0x01)		// nal_ref_idc==0 && nal_unit_type==1 (0000 0001 01..)
			|| (glob->encode_buffer[4]==0x09 && glob->encode_buffer[10]==0x01)
										// After Access Unit Delimiter NAL (0000 0001 09xx 0000 0001 01..)
			|| (glob->encode_buffer[4]==0x09 && glob->encode_buffer[9]==0x01)
										// After Access Unit Delimiter NAL (0000 0001 09xx 0000 0101 ..) and shortStartCode
			) {
				droppable_frame = TRUE;
			} else {
				droppable_frame = FALSE;
			}
#endif
		} else if( pict_type == FF_S_TYPE ) {		// Not sure
			frame_type = kICMFrameType_P;
			partial_sync_frame = false;
			droppable_frame = false;
		} else if( pict_type == FF_SI_TYPE ) {		// Not sure
			fprintf(stderr, "ERROR: FF_SI_TYPE data returned at %010lld\n", 
							(long long int)glob->codecCont->coded_frame->display_picture_number+1);
			err = paramErr;
			goto bail;
		} else if( pict_type == FF_SP_TYPE ) {		// Not sure
			fprintf(stderr, "ERROR: FF_SP_TYPE data returned at %010lld\n", 
							(long long int)glob->codecCont->coded_frame->display_picture_number+1);
			err = paramErr;
			goto bail;
		} else {									// Not sure...
			fprintf(stderr, "ERROR: Unknown TYPE data returned at %010lld\n", 
							(long long int)glob->codecCont->coded_frame->display_picture_number+1);
			err = paramErr;
			goto bail;
		}
		
		// Set up ICM media_sample_flags
		if( ! key_frame ) 
			media_sample_flags |= mediaSampleNotSync;
		
		// Partial Sync; Support Open-GOP/Non-IDR I frame
		if( partial_sync_frame ) 
			media_sample_flags |= mediaSamplePartialSync;
		
		// Droppable; seems to be imcompatible with B-Pyramid
#if STRICTFLAG
		if( droppable_frame )
			media_sample_flags |= mediaSampleIsNotDependedOnByOthers;		// = mediaSampleDroppable;
		else
			media_sample_flags |= mediaSampleIsDependedOnByOthers;
#else
		if( droppable_frame 
//		&& (glob->codecCont->flags2 & CODEC_FLAG2_BPYRAMID) == 0			// Not sure
		)
			media_sample_flags |= mediaSampleDroppable;
#endif
		
		// Reordering support
		if( reordering ) 
			media_sample_flags |= mediaSampleEarlierDisplayTimesAllowed;
		
#if STRICTFLAG
		// Sample Dependencies
		if(key_frame || partial_sync_frame)
			media_sample_flags |= mediaSampleDoesNotDependOnOthers;			// I slice or IDR slice
		else
			media_sample_flags |= mediaSampleDependsOnOthers;				// P slice or B slice
#endif		
	}
	
	/* ==================================== */
	
	{
#if X264
		// Get temp NAL buffer
		int tempSize = glob->frame_size;
		
		UInt8* tempPtr = av_malloc(tempSize);
		if( !tempPtr ) {
			fprintf(stderr, "ERROR: av_malloc() failed.\n");
			err = memFullErr;
			goto bail;
		}
		
		// Re-format NAL unit.
		memcpy(tempPtr, glob->encode_buffer, tempSize);
//		{UInt8* p=tempPtr, *pEnd=(tempSize>32?p+32:p+tempSize);for(;p<pEnd;p++){fprintf(stderr,"%02x ",*p);}fprintf(stderr,"\n");}
		
		avc_parse_nal_units(&tempPtr, &tempSize);	// This call does realloc buffer; may also be re-sized
		if( !tempSize ) {
			fprintf(stderr, "ERROR: avc_parse_nal_units() failed.\n");
			err = paramErr;
			av_free(tempPtr);
			goto bail;
		}
//		{UInt8* p=tempPtr, *pEnd=(tempSize>32?p+32:p+tempSize);for(;p<pEnd;p++){fprintf(stderr,"%02x ",*p);}fprintf(stderr,"\n");}
		
		// Create a ICMEncodedFrame for source_frame
		err = ICMEncodedFrameCreateMutable(glob->session, source_frame, tempSize, &encoded_frame);
		if (err || !encoded_frame) {
			fprintf(stderr, "ERROR: ICMEncodedFrameCreateMutable() failed.\n");
			err = paramErr;
			av_free(tempPtr);
			goto bail;
		}
		
		// Put buffer from formated NAL unit into ICMEndoedFrame's buffer
		memcpy(ICMEncodedFrameGetDataPtr(encoded_frame), tempPtr, tempSize);
		av_free(tempPtr);
		
		err = ICMEncodedFrameSetDataSize(encoded_frame, tempSize);
		if( err ) {
			fprintf(stderr, "ERROR: ICMEncodedFrameSetDataSize() failed.\n");
			err = paramErr;
			goto bail;
		}
#endif
#if MPEG4|XVID
		// Create a ICMEncodedFrame for source_frame
		err = ICMEncodedFrameCreateMutable(glob->session, source_frame, glob->frame_size, &encoded_frame);
		if (err || !encoded_frame) {
			fprintf(stderr, "ERROR: ICMEncodedFrameCreateMutable() failed.\n");
			err = paramErr;
			goto bail;
		}
		
		// Put buffer from encoded AVFrame* into ICMEndoedFrame's buffer
		memcpy(ICMEncodedFrameGetDataPtr(encoded_frame), glob->encode_buffer, glob->frame_size);
		
		err = ICMEncodedFrameSetDataSize(encoded_frame, glob->frame_size);
		if( err ) {
			fprintf(stderr, "ERROR: ICMEncodedFrameSetDataSize() failed.\n");
			err = paramErr;
			goto bail;
		}
#endif
		
		// Set up encoded_frame
		err = ICMEncodedFrameSetMediaSampleFlags(encoded_frame, media_sample_flags);
		if( err ) {
			fprintf(stderr, "ERROR: ICMEncodedFrameSetMediaSampleFlags() failed.\n");
			err = paramErr;
			goto bail;
		}
		
		err = ICMEncodedFrameSetFrameType(encoded_frame, frame_type);
		if( err ) {
			fprintf(stderr, "ERROR: ICMEncodedFrameSetFrameType() failed.,\n");
			err = paramErr;
			goto bail;
		}
	}
	
	/* ==================================== */
	
	{
		// Ready to emit data
//		CFShow(encoded_frame);
//		CFShow(source_frame);
		
		// Output the encoded frame.
		err = ICMCompressorSessionEmitEncodedFrame( glob->session, encoded_frame, 1, &source_frame );
		
		// done with this source frame
		Boolean ret;
		ret = removeSourceFrame(glob, source_frame);
		
		if( err ) {
			fprintf(stderr, "ERROR: ICMCompressorSessionEmitEncodedFrame() failed.(%d)\n", (int)err);
			err = paramErr;
			goto bail;
		}
		if( !ret ) {
			fprintf(stderr, "ERROR: removeSourceFrame() failed.\n");
			err = paramErr;
			goto bail;
		}
	}
	
bail:
	if( encoded_frame ) ICMEncodedFrameRelease( encoded_frame );
	return err;
} // emitFrameData()


// Create a dictionary that describes the kinds of pixel buffers that we want to receive.
static OSStatus createPixelBufferAttributesDictionary( lavcEncoderGlobalRecord *glob,
	const OSType *pixelFormatList, int pixelFormatCount, CFMutableDictionaryRef *pixelBufferAttributesOut )
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	OSStatus err = memFullErr;
	int i;
	CFMutableDictionaryRef pixelBufferAttributes = NULL;
	CFNumberRef number = NULL;
	CFMutableArrayRef array = NULL;
	SInt32 width, height, widthRoundedUp, heightRoundedUp, extendRight, extendBottom;
	
	// Create Dictionary and Array
	pixelBufferAttributes = CFDictionaryCreateMutable( 
			NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks );
	if( ! pixelBufferAttributes ) goto bail;
	
	array = CFArrayCreateMutable( NULL, 0, &kCFTypeArrayCallBacks );
	if( ! array ) goto bail;
	
	// Build pixel format list Array
	for( i = 0; i < pixelFormatCount; i++ ) {
		number = CFNumberCreate( NULL, kCFNumberSInt32Type, &pixelFormatList[i] );
		CFArrayAppendValue( array, number );
		CFRelease( number );
		number = NULL;
	}
	
	// Add it into Dictionary 
	CFDictionaryAddValue( pixelBufferAttributes, kCVPixelBufferPixelFormatTypeKey, array );
	CFRelease( array );
	array = NULL;
	
	// Add kCVPixelBufferWidthKey and kCVPixelBufferHeightKey.
	width = glob->width;
	height = glob->height;
	
	addNumberToDictionary( pixelBufferAttributes, kCVPixelBufferWidthKey, width );
	addNumberToDictionary( pixelBufferAttributes, kCVPixelBufferHeightKey, height );
	
	// Add kCVPixelBufferExtendedPixels{Left,Top,Right,Bottom}Keys
	widthRoundedUp = glob->widthRoundedUp;
	heightRoundedUp = glob->heightRoundedUp;
	extendRight = widthRoundedUp - width;
	extendBottom = heightRoundedUp - height;
	
	if( extendRight || extendBottom ) {
		addNumberToDictionary( pixelBufferAttributes, kCVPixelBufferExtendedPixelsRightKey, extendRight );
		addNumberToDictionary( pixelBufferAttributes, kCVPixelBufferExtendedPixelsBottomKey, extendBottom );
	}
	
	// Add kCVPixelBufferBytesPerRowAlignmentKey.
	addNumberToDictionary( pixelBufferAttributes, kCVPixelBufferBytesPerRowAlignmentKey, 16 );
	
	// Add gamma level and YCbCr matrix.
	if( glob->add_gamma ) {
		addDoubleToDictionary( pixelBufferAttributes, kCVImageBufferGammaLevelKey, 2.2 );
	}
	if( glob->add_nclc ) {
		CFStringRef ColorPrimaries = NULL;
		CFStringRef TransferFunction = NULL;
		CFStringRef YCbCrMatrix = NULL;
		switch( glob->params.NCLC ) {
		case 3:	// 616
			ColorPrimaries = kCVImageBufferColorPrimaries_SMPTE_C;			/* AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER */
			TransferFunction = kCVImageBufferTransferFunction_ITU_R_709_2;	/* AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER */
			YCbCrMatrix = kCVImageBufferYCbCrMatrix_ITU_R_601_4;
			break;
		case 4:	// 516
			ColorPrimaries = kCVImageBufferColorPrimaries_EBU_3213;			/* AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER */
			TransferFunction = kCVImageBufferTransferFunction_ITU_R_709_2;	/* AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER */
			YCbCrMatrix = kCVImageBufferYCbCrMatrix_ITU_R_601_4;
			break;
		case 5:	// 677
#if 0	// Legacy 10.5.SDK on Xcode 3.1; seems to be CoreVideo header error.
			ColorPrimaries = kCVImageBufferColorPrimaries_SMPTE_C;			/* AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER */
			TransferFunction = kCVImageBufferTransferFunction_SMPTE_C;		/* AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER_BUT_DEPRECATED */
			YCbCrMatrix = kCVImageBufferYCbCrMatrix_SMPTE_240M_1995;
#else	// 10.6? But this const is defined inside "10.5.SDK" in Xcode 3.2!!
			ColorPrimaries = kCVImageBufferColorPrimaries_SMPTE_C;			/* AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER */
			TransferFunction = kCVImageBufferTransferFunction_SMPTE_240M_1995;	/*  AVAILABLE_MAC_OS_X_VERSION_10_6_AND_LATER */
			YCbCrMatrix = kCVImageBufferYCbCrMatrix_SMPTE_240M_1995;
#endif
			break;
		case 6:	// 111
			ColorPrimaries = kCVImageBufferColorPrimaries_ITU_R_709_2;		/* AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER */
			TransferFunction = kCVImageBufferTransferFunction_ITU_R_709_2;	/* AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER */
			YCbCrMatrix = kCVImageBufferYCbCrMatrix_ITU_R_709_2;
			break;
		case 7: // 222
		default:
			ColorPrimaries = NULL;
			TransferFunction = NULL;
			YCbCrMatrix = NULL;
			break;
		}
		if( ColorPrimaries )												/* AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER */
			CFDictionaryAddValue( pixelBufferAttributes, kCVImageBufferColorPrimariesKey, ColorPrimaries);
		if( TransferFunction )												/* AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER */
			CFDictionaryAddValue( pixelBufferAttributes, kCVImageBufferTransferFunctionKey, TransferFunction);
		if( YCbCrMatrix )
			CFDictionaryAddValue( pixelBufferAttributes, kCVImageBufferYCbCrMatrixKey, YCbCrMatrix);
	}

	err = noErr;
	*pixelBufferAttributesOut = pixelBufferAttributes;
	pixelBufferAttributes = NULL;
	
bail:
	if( pixelBufferAttributes ) CFRelease( pixelBufferAttributes );
	if( number ) CFRelease( number );
	if( array ) CFRelease( array );
	return err;
} // createPixelBufferAttributesDictionary()


#pragma mark -


// Get ICMCompressorSourceFrame for encoded AVFrame from source frame queue
static ICMCompressorSourceFrameRef getSourceFrameForEncodedFrame(lavcEncoderGlobalRecord *glob)
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	int index = 0;
	ICMCompressorSourceFrameRef CF_value = NULL;
	int count = CFArrayGetCount(glob->array_source_frame);
	
	// Get encoded frame's pts value
	int64_t pts = glob->codecCont->coded_frame->pts;
	if( pts == AV_NOPTS_VALUE ) 
		pts = glob->next_pts_value;
	
#if TESTVFR
	TimeValue64 displayTimeStamp, displayDuration;
	TimeScale timeScale;
	for( index = 0; index < count; index++ ) {
		CF_value = (ICMCompressorSourceFrameRef)CFArrayGetValueAtIndex(glob->array_source_frame, index);
		ICMCompressorSourceFrameGetDisplayTimeStampAndDuration(CF_value, &displayTimeStamp, &displayDuration, &timeScale, NULL);
		if( displayTimeStamp == pts ) {
			glob->next_pts_value = displayTimeStamp + displayDuration;
			return CF_value;
		}
	}
#else
	for( index = 0; index < count; index++ ) {
		CF_value = (ICMCompressorSourceFrameRef)CFArrayGetValueAtIndex(glob->array_source_frame, index);
		if( ICMCompressorSourceFrameGetDisplayNumber(CF_value) == (pts+1) ) {
			glob->next_pts_value++;
			return CF_value;
		}
	}
#endif
	
	return NULL;
} // getSourceFrameForEncodedFrame()


// Remove specified ICMCompressorSourceFrame from source frame queue
static Boolean removeSourceFrame(lavcEncoderGlobalRecord *glob, ICMCompressorSourceFrameRef source_frame)
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	int index = 0;
	ICMCompressorSourceFrameRef CF_value = NULL;
	int count = CFArrayGetCount(glob->array_source_frame);
	
	for( index = 0; index < count; index++ ) {
		CF_value = (ICMCompressorSourceFrameRef)CFArrayGetValueAtIndex(glob->array_source_frame, index);
		if( CF_value == source_frame ) {
			CFArrayRemoveValueAtIndex(glob->array_source_frame, index);
			return TRUE;
		}
	}
	
	return FALSE;
}


// Check if source frame queue has the frame for encoded AVFrame
static Boolean doesQueueContainEarlierDisplayNumbers( lavcEncoderGlobalRecord *glob, long display_number )
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	int index = 0;
	ICMCompressorSourceFrameRef CF_value = NULL;
	int count = CFArrayGetCount(glob->array_source_frame);

	for( index = 0; index < count; index++ ) {
		CF_value = (ICMCompressorSourceFrameRef)CFArrayGetValueAtIndex(glob->array_source_frame, index);
		if( ICMCompressorSourceFrameGetDisplayNumber(CF_value) < display_number ) {
			return TRUE;
		}
	}
	
	return FALSE;
}


// Remove ICMCompressorSourceFrame for encoded AVFrame from source frame queue
static Boolean removeSourceFrameForEncodedFrame( lavcEncoderGlobalRecord *glob )
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	ICMCompressorSourceFrameRef source_frame = NULL;
	Boolean ret = FALSE;
	
	// Here we get the source from array_source_frame by glob->codecCont->coded_frame->pts
	source_frame = getSourceFrameForEncodedFrame(glob);
	if( !source_frame ) {
		fprintf(stderr, "ERROR: getSourceFrameForEncodedFrame() failed.\n");
		goto bail;
	}
	
	// done with this source frame
	ret = removeSourceFrame(glob, source_frame);
	if( !ret ) {
		fprintf(stderr, "ERROR: removeSourceFrame() failed.\n");
		goto bail;
	}
	
bail:
	return ret;
}


#pragma mark -


// Prepare avcC atom for ImageDescription Extension.
static Handle avcCExtension( lavcEncoderGlobalRecord *glob )
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	// This code strongly depends onto x264/encoder/encoder.c:x264_encoder_headers()
	// We suppose all nal are byte aligned, and start with 0x000001(BE) ... shortstartcode
	Handle avcCHdl = NULL;
	
	UInt8 *pSEI=NULL, *pSPS=NULL, *pPPS=NULL;
	int lenOfSEI=0, lenOfSPS=0, lenOfPPS=0;
	UInt8* p = (UInt8*)glob->codecCont->extradata;
	UInt8* end = p+glob->codecCont->extradata_size;
	
	// TODO; This loop works only with single SEI/PPS/SPS. Multiple NAL not supported...
	while( p + 4 < end ) {
		if( 0x000001 == EndianU32_BtoN(*(UInt32*)p) >> 8) {
			// NAL Start
			p+=3;
			switch (p[0]) {
			case 0x06:					// SEI NAL
				if(!pSEI) pSEI = p;
				if( pSPS && !lenOfSPS ) lenOfSPS = p - 3 - pSPS;
				if( pPPS && !lenOfPPS ) lenOfPPS = p - 3 - pPPS;
				break;
			case 0x67:					// SPS NAL
				if( pSEI && !lenOfSEI ) lenOfSEI = p - 3 - pSEI;
				if(!pSPS) pSPS = p;
				if( pPPS && !lenOfPPS ) lenOfPPS = p - 3 - pPPS;
				break;
			case 0x68:					// PPS NAL
				if( pSEI && !lenOfSEI ) lenOfSEI = p - 3 - pSEI;
				if( pSPS && !lenOfSPS ) lenOfSPS = p - 3 - pSPS;
				if(!pPPS) pPPS = p;
				break;
			default:
				break;					// ignore
			}
		}
		p++;
	}
	if( pSEI && !lenOfSEI ) lenOfSEI = end-pSEI;
	if( pSPS && !lenOfSPS ) lenOfSPS = end-pSPS;
	if( pPPS && !lenOfPPS ) lenOfPPS = end-pPPS;
	
	// Check SPS and PPS
	if( !(pSPS && lenOfSPS) ) {
		fprintf(stderr, "ERROR: Failed to get SPS NAL unit.\n");
		goto bail;
	}
	if( !(pPPS && lenOfPPS) ) {
		fprintf(stderr, "ERROR: Failed to get PPS NAL unit.\n");
		goto bail;
	}
	if( !(pSEI && lenOfSEI) ) {
		fprintf(stderr, "ERROR: Failed to get SEI NAL unit.\n");
		/* do nothing; SEI is not mandatory */goto bail;
	}
	
	// 
	int lengthOfAVCC = (1 + 3 + 1) + (1 + 2 + lenOfSPS) + (1 + 2 + lenOfPPS);
	avcCHdl = NewHandleClear(lengthOfAVCC);
	if( avcCHdl ) {
		HLock(avcCHdl);
		
		char* avcC = *avcCHdl;
		avcC[0] = 0x01;					// version
		avcC[1] = pSPS[1];				// profile
		avcC[2] = pSPS[2];				// profile compatibility
		avcC[3] = pSPS[3];				// level
		avcC[4] = 0xFF;					// 0x3F<<2 + 0x11
		
		avcC[5] = 0xE1;						// 0x07<<5 + 1 (count of sps)
		avcC[6] = lenOfSPS>>8;				//
		avcC[7] = lenOfSPS&0xFF;			//

		memcpy(&avcC[8], pSPS, lenOfSPS);
		
		avcC[8+lenOfSPS] = 0x01;			// 1 (count of pps)
		avcC[9+lenOfSPS] = lenOfPPS>>8;		//
		avcC[10+lenOfSPS] = lenOfPPS&0xFF;	//
		
		memcpy(&avcC[11+lenOfSPS], pPPS, lenOfPPS);
		
		HUnlock(avcCHdl);
		// This handle should be released by caller
	} else {
		fprintf(stderr, "ERROR: NewHandleClear() failed.\n");
	}
	
//	// Dump SEI // Is offset 21 correct or not?
//	if( lenOfSEI > 21 ) {
//		char *buf = calloc(1,lenOfSEI+1);
//		memcpy(buf,pSEI+21,lenOfSEI-21);
//		logInfo(glob, "lavcEncoder: - SEI: %s \n", buf);
//		free(buf);
//	}
	
bail:	
	return avcCHdl;
} // avcCExtension()


// Prepare elementary stream description atom for ImageDescription Extension.
static Handle esdsExtension( lavcEncoderGlobalRecord *glob )
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	Handle esdsHdl = NULL;
	int lengthOfHeader = 2;
	int lengthOfData05 = glob->codecCont->extradata_size;
	int lengthOfData04 = 1 + 1 + 3 + 4 + 4 + lengthOfHeader + lengthOfData05;
	int lengthOfData06 = 1;
	int lengthOfData03 = 2 + 1 + lengthOfHeader + lengthOfData04 + lengthOfHeader + lengthOfData06;
	int lengthOfDataEsds = 1 + 3 + lengthOfHeader + lengthOfData03;
//	int lengthOfExtension = 4 + 4 + lengthOfDataEsds;	// esds atom length
	
	if( !lengthOfData05 ) {
		fprintf(stderr, "ERROR: No codec extradata found.\n");
		return NULL;
	}
	
	// 
	esdsHdl = NewHandleClear(lengthOfDataEsds);
	if( esdsHdl ) {
		HLock(esdsHdl);
		char* esds = *esdsHdl;		// ES Descriptor box
		
		esds[0] = 0;				// ES Descriptor box version (1)
		esds[1] = 0;				// ES Descriptor box flag 0 (3) BE
		esds[2] = 0;
		esds[3] = 0;
		
		esds[4] = 0x03;				// Desc03 Type ES descriptor (1)
		esds[5] = lengthOfData03;	// Desc03 Length (1)
		
		esds[6] = 0x00;				// ES ID 0 (2) BE
		esds[7] = 0x00;
		esds[8] = 0x10;				// ES Priority 16 (1) ; 00-1f available; default is 10.
		
		esds[9] = 0x04;				// Desc04 Type Decoder config descriptor (1)
		esds[10] = lengthOfData04;	// Desc04 Length (1)
		
		esds[11] = 0x20;			// Object Type ID MPEG-4 Video(1)
		esds[12] = 0x11;			// Stream Type Visual[6] + upstream flag0[1] + reserved1[1]
		
		esds[13] = 0x00;			// Buffer size undefined (3) BE
		esds[14] = 0x00;			// Similar to ffmpeg's mp4 format default value
		esds[15] = 0x00;
		
		esds[16] = 0x00;			// Maximum bit rate 4Mbps(4) BE
		esds[17] = 0x0B;			// Similar to ffmpeg's mp4 format default value
		esds[18] = 0xB8;
		esds[19] = 0x00;
		
		esds[20] = 0x00;			// Average bit rate undefined (4) BE
		esds[21] = 0x00;			// Similar to ffmpeg's mp4 format default value
		esds[22] = 0x00;
		esds[23] = 0x00;
		
		esds[24] = 0x05;			// Desc05 Type Decoder specific info descriptor (1)
		esds[25] = lengthOfData05;	// Desc05 Length (varialble)
		
		esds[26+lengthOfData05] = 0x06;	// Desc06 Type SL Descriptor (1)
		esds[27+lengthOfData05] = lengthOfData06;	// Desc06 Length (1)
		
		esds[28+lengthOfData05] = 0x02;	//SL value 2 (1)
		
		memcpy(esds+26, glob->codecCont->extradata, lengthOfData05);
		
		HUnlock(esdsHdl);
	} else {
		fprintf(stderr, "ERROR: NewHandleClear() failed.\n");
	}
	
	return esdsHdl;
} // esdsExtension()


//
static OSStatus prepareImageDescriptionExtensions(lavcEncoderGlobalRecord *glob)
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	OSStatus err = noErr;
	
#if X264
	if(glob->add_uuid) {
		// Special UUID atom support for iPod H.264
		while(TRUE) {
			if( RemoveImageDescriptionExtension(glob->imageDescription, 'uuid', 1) ) break;
		}
		static UInt8 specialuuid[20] = {0x6B,0x68,0x40,0xF2,0x5F,0x24,0x4F,0xC5,
										0xBA,0x39,0xA5,0x1B,0xCF,0x03,0x23,0xF3,
										0x00,0x00,0x00,0x01};
		Handle specialHndl = NewHandle(20);
		PtrToHand(specialuuid,&specialHndl,20);
		if(specialHndl) {
			HLock(specialHndl);
			err = AddImageDescriptionExtension(glob->imageDescription, specialHndl, 'uuid');
			if( err ) {
				fprintf(stderr, "ERROR: Adding special uuid failed.\n");
				err = paramErr;
			}
			HUnlock(specialHndl);
			DisposeHandle(specialHndl);
		} else {
			fprintf(stderr, "ERROR: Getting special uuid failed.\n");
			err = paramErr;
		}
		if( err ) goto bail;
	}	// glob->add_uuid
#endif
	
	/* ==================================== */
	
	// add imagedescription extension gamma info
	if( glob->add_gamma ) {
		// Set gamma level to image description
		Fixed gamma_level = kQTCCIR601VideoGammaLevel;	/* 2.2, standard television video gamma.*/
		
		err = ICMImageDescriptionSetProperty( glob->imageDescription, 
				kQTPropertyClass_ImageDescription,
				kICMImageDescriptionPropertyID_GammaLevel,
				sizeof(gamma_level),
				&gamma_level);
		if( err ) {
			fprintf(stderr, "ERROR: Setting gamma level failed.\n");
			err = paramErr;
			goto bail;
		}
	}	// glob->add_gamma
	
	/* ==================================== */
	
	// add imagedescription extension nclc info
	if( glob->add_nclc ) {
		Handle nclcHdl = NULL;
		nclcHdl = NewHandleClear(sizeof(NCLCColorInfoImageDescriptionExtension));
		if( nclcHdl ) {
			HLock( nclcHdl );
			
			NCLCColorInfoImageDescriptionExtension* nclc = (NCLCColorInfoImageDescriptionExtension*)(*nclcHdl);
			nclc->colorParamType = kVideoColorInfoImageDescriptionExtensionType;
			glob->add_nclc = TRUE;
			
			switch (glob->params.NCLC) {
			case 3:	// 616
				nclc->primaries =		kQTPrimaries_SMPTE_C;
				nclc->transferFunction = kQTTransferFunction_ITU_R709_2;
				nclc->matrix =			kQTMatrix_ITU_R_601_4;
				break;
			case 4:	// 516
				nclc->primaries =		kQTPrimaries_EBU_3213;
				nclc->transferFunction = kQTTransferFunction_ITU_R709_2;
				nclc->matrix =			kQTMatrix_ITU_R_601_4;
				break;
			case 5:	// 677
				nclc->primaries =		kQTPrimaries_SMPTE_C;
				nclc->transferFunction = kQTTransferFunction_SMPTE_240M_1995;
				nclc->matrix =			kQTMatrix_SMPTE_240M_1995;
				break;
			case 6:	// 111
				nclc->primaries =		kQTPrimaries_ITU_R709_2;
				nclc->transferFunction = kQTTransferFunction_ITU_R709_2;
				nclc->matrix =			kQTMatrix_ITU_R_709_2;
				break;
			case 7: // 222
				nclc->primaries =		kQTPrimaries_Unknown;
				nclc->transferFunction = kQTTransferFunction_Unknown;
				nclc->matrix =			kQTMatrix_Unknown;
				break;
			default:
				glob->add_nclc = FALSE;
				break;
			}
			if( glob->add_nclc ) {
				logDebug(glob, "lavcEncoder: glob->params.NCLC(%d) = %d%d%d\n"
					, glob->params.NCLC, nclc->primaries, nclc->transferFunction, nclc->matrix);
				err = ICMImageDescriptionSetProperty( glob->imageDescription, 
						kQTPropertyClass_ImageDescription,
						kICMImageDescriptionPropertyID_NCLCColorInfo,
						sizeof(NCLCColorInfoImageDescriptionExtension),
						nclc);
				if( err ) {
					fprintf(stderr, "ERROR: Adding nclc extension failed.\n");
					err = paramErr;
				}
			}
			
			HUnlock(nclcHdl);
			DisposeHandle(nclcHdl);
			if( err ) goto bail;
		}
	}	// glob->add_nclc
	
	/* ==================================== */
	
	// add imagedescription FieldInfo
	if( glob->params.FLAG_INTERLACED_DCT ) {
		// Set field info to image description
		Handle fielHdl = NULL;
		fielHdl = NewHandleClear(sizeof(FieldInfoImageDescriptionExtension2));
		if( fielHdl ) {
			HLock( fielHdl );
			FieldInfoImageDescriptionExtension2* fiel = (FieldInfoImageDescriptionExtension2*)(*fielHdl);
			
			// x264 uses mbaff, not paff; two field is contained in one sample. i.e. combined.
			fiel->fields = kQTFieldsInterlaced;
			if(glob->params.TOPFIELDFIRST)
				fiel->detail = kQTFieldDetailSpatialFirstLineEarly;		// Top field first combined
			else
				fiel->detail = kQTFieldDetailSpatialFirstLineLate;		// Bottom field first combined
			
			logDebug(glob, "lavcEncoder: FieldInfoImageDescriptionExtension2 = (%d,%d)\n"
				, fiel->fields, fiel->detail);
			err = ICMImageDescriptionSetProperty( glob->imageDescription, 
					kQTPropertyClass_ImageDescription,
					kICMImageDescriptionPropertyID_FieldInfo,
					sizeof(FieldInfoImageDescriptionExtension2),
					fiel);
			if( err ) {
				fprintf(stderr, "ERROR: Setting field info failed.\n");
				err = paramErr;
			}
			
			HUnlock(fielHdl);
			DisposeHandle(fielHdl);
			if( err ) goto bail;
		}
	}	// glob->params.FLAG_INTERLACED_DCT
	
	/* ==================================== */
	
	// add imagedescription PixelAspectRatio
	if( glob->params.USEASPECTRATIO ) {
		if( glob->params.hSpacing == 0
		||	glob->params.vSpacing == 0
		){
			fprintf(stderr, "ERROR: Invalid parameter for pixel aspect ratio.\n");
			err = paramErr;
			goto bail;
		}
		
		// Set pixel aspect ratio to image description
		Handle paspHdl = NULL;
		paspHdl = NewHandleClear(sizeof(PixelAspectRatioImageDescriptionExtension));
		if( paspHdl ) {
			HLock( paspHdl );
			PixelAspectRatioImageDescriptionExtension* pasp = (PixelAspectRatioImageDescriptionExtension*)(*paspHdl);
			
			pasp->hSpacing = glob->params.hSpacing;
			pasp->vSpacing = glob->params.vSpacing;
			
			logDebug(glob, "lavcEncoder: PixelAspectRatioImageDescriptionExtension = (%d,%d)\n"
				, pasp->hSpacing, pasp->vSpacing);
			err = ICMImageDescriptionSetProperty( glob->imageDescription, 
					kQTPropertyClass_ImageDescription,
					kICMImageDescriptionPropertyID_PixelAspectRatio,
					sizeof(PixelAspectRatioImageDescriptionExtension),
					pasp);
			if( err ) {
				fprintf(stderr, "ERROR: Setting pixel aspect ratio failed.\n");
				err = paramErr;
			}
			
			HUnlock(paspHdl);
			DisposeHandle(paspHdl);
			if( err ) goto bail;
		}
	}	// glob->params.USEASPECTRATIO
	
	/* ==================================== */
	
	// add imagedescription CleanAperture
	if( glob->params.USECLEANAPERTURE ) {
		// Check "divide by 0" error
		if(	glob->params.cleanApertureWidthD == 0 || glob->params.cleanApertureHeightD == 0 
		||	glob->params.horizOffD == 0 || glob->params.vertOffD == 0
		){
			fprintf(stderr, "NOTICE: Skip to apply clean aperture. (denominator is 0)\n");
			AudioServicesPlayAlertSound(kUserPreferredAlert);
			goto bail;
		}
		
		// Check zero size rectangle error
		if(	glob->params.cleanApertureWidthN == 0 || glob->params.cleanApertureHeightN == 0 ) {
			fprintf(stderr, "NOTICE: Skip to apply clean aperture. (zero size apreture)\n");
			AudioServicesPlayAlertSound(kUserPreferredAlert);
			goto bail;
		}
		
#if 0
		// Check out of range error; Just skip to apply instead of return error
		double caW = (double)glob->params.cleanApertureWidthN / glob->params.cleanApertureWidthD;
		double caH = (double)glob->params.cleanApertureHeightN / glob->params.cleanApertureHeightD;
		double hOff = (double)glob->params.horizOffN / glob->params.horizOffD;
		double vOff = (double)glob->params.vertOffN / glob->params.vertOffD;
		if(	caW > glob->width || caH > glob->height ) {
			fprintf(stderr, "NOTICE: Skip to apply clean aperture. (larger than raw image)\n");
			AudioServicesPlayAlertSound(kUserPreferredAlert);
			goto bail;
		} else
		if( caW + fabs(hOff) < (double)glob->width || caH + fabs(vOff) < (double)glob->height ) {
			fprintf(stderr, "NOTICE: Skip to apply clean aperture. (invalid offset)\n");
			AudioServicesPlayAlertSound(kUserPreferredAlert);
			goto bail;
		}
#endif
		
		// Set clean aperture to image description
		Handle clapHdl = NULL;
		clapHdl = NewHandleClear(sizeof(CleanApertureImageDescriptionExtension));
		if( clapHdl ) {
			HLock( clapHdl );
			CleanApertureImageDescriptionExtension* clap = (CleanApertureImageDescriptionExtension*)(*clapHdl);
			
			clap->cleanApertureWidthN = glob->params.cleanApertureWidthN;
			clap->cleanApertureWidthD = glob->params.cleanApertureWidthD;
			clap->cleanApertureHeightN = glob->params.cleanApertureHeightN;
			clap->cleanApertureHeightD = glob->params.cleanApertureHeightD;
			clap->horizOffN = glob->params.horizOffN;
			clap->horizOffD = glob->params.horizOffD;
			clap->vertOffN = glob->params.vertOffN;
			clap->vertOffD = glob->params.vertOffD;
			
			logDebug(glob, "lavcEncoder: CleanApertureImageDescriptionExtension = "
				"rectangle(%d/%d, %d,%d), offset(%d/%d, %d,%d)\n"
				, clap->cleanApertureWidthN, clap->cleanApertureWidthD
				, clap->cleanApertureHeightN, clap->cleanApertureHeightD
				, clap->horizOffN, clap->horizOffD
				, clap->vertOffN, clap->vertOffD
				);
			err = ICMImageDescriptionSetProperty( glob->imageDescription, 
					kQTPropertyClass_ImageDescription,
					kICMImageDescriptionPropertyID_CleanAperture,
					sizeof(CleanApertureImageDescriptionExtension),
					clap);
			if( err ) {
				fprintf(stderr, "ERROR: Setting clean aperture failed.\n");
				err = paramErr;
			}
			
			HUnlock(clapHdl);
			DisposeHandle(clapHdl);
			if( err ) goto bail;
		}
	}	// glob->params.USECLEANAPERTURE
	
bail:
	return err;
}


#pragma mark -


// log Debug support
static void logDebug(lavcEncoderGlobalRecord *glob, char* fmt, ...)
{
	if( glob && !(glob->params.LOG_DEBUG) ) return;
	
	char buf[4096];
	va_list argp;
	va_start(argp, fmt);
	vsnprintf(buf, 4096, fmt, argp);
	va_end(argp);
	
	fprintf(stderr, "DEBUG: %s", buf);
}


// log Info support
static void logInfo(lavcEncoderGlobalRecord *glob, char* fmt, ...)
{
	if( glob && !glob->params.LOG_INFO ) return;
	
	char buf[4096];
	va_list argp;
	va_start(argp, fmt);
	vsnprintf(buf, 4096, fmt, argp);
	va_end(argp);
	
	fprintf(stderr, "INFO : %s", buf);
}


#pragma mark -


#if USECoreVF
// Prepare CoreVF Framework
static OSStatus openCoreVF( lavcEncoderGlobals glob )
{
	logDebug(glob, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	OSStatus err = noErr;
	
	if(	CVF_Context_Create == NULL ) {
		err = paramErr;
		goto bail;
	}
	
	if(glob->filterPreset) {
		CFStringRef appID = CFSTR("com.MyCometG3.CoreVF");
//		CFPreferencesAppSynchronize(appID);	/* cal this at SetSettings */
		CFStringRef keyStr = NULL;
		CFIndex index = glob->filterPreset;
		if(glob->filterPreset == -1) {
			CFNumberRef indexRef = (CFNumberRef)CFPreferencesCopyAppValue(CFSTR("filterPreset"), appID);
			if(indexRef) {
				if(!CFNumberGetValue(indexRef, kCFNumberCFIndexType, &index))
					index = 1;
				CFRelease(indexRef);
			} else {
				index = 1;
			}
		}
		
		keyStr = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("Preset%d"), CVF_CLIP(index, 1, 8));
		if(keyStr) {
			glob->filterString = (CFStringRef)CFPreferencesCopyAppValue(keyStr, appID);
			CFRelease(keyStr);
		}
	}
	if(!glob->filterString || (glob->filterString && !CFStringGetLength(glob->filterString)) ) {
		err = paramErr;
		goto bail;
	}
	
	if(!glob->cvfCont) {
		int cvfLogLevel = CVF_LOGLEVEL_NONE;
		if( glob->params.LOG_INFO )		cvfLogLevel = CVF_LOGLEVEL_LESS;
		if( glob->params.LOG_DEBUG )	cvfLogLevel = CVF_LOGLEVEL_MORE;
		
		glob->cvfCont = CVF_Context_Create(cvfLogLevel);
		if(!glob->cvfCont) {
			err = paramErr;
			goto bail;
		}
		
		int result = FALSE;
		result = CVF_Context_BuildFilterChain(glob->cvfCont, glob->filterString, NULL);
		if(!result) {
			err = paramErr;
			goto bail;
		}
		
#if 1
		CVF_Video *tempV = CVF_Video_CreateDefault(glob->width, glob->height, 
			CVF_PT_I420, CVF_FT_FRAMEBASED);	// Is it CVF_PT_YV12, or CVF_PT_I420 ?
#else
		CVF_Video *tempV = CVF_Video_CreateDefault(glob->codecCont->width, glob->codecCont->height, 
			CVF_PT_I420, CVF_FT_FRAMEBASED);	// Is it CVF_PT_YV12, or CVF_PT_I420 ?
#endif
		assert(tempV);
		glob->cvfOutVideo = CVF_Context_GetVideoRep(glob->cvfCont, tempV);
		assert(glob->cvfOutVideo);
		if(tempV != glob->cvfOutVideo) CVF_Video_Release(tempV);
		
		if(glob->subsampling >= 0) {			// input and output uses planar yuv 420 format
			glob->cvfInVideo = glob->cvfOutVideo;	// Broken support on CoreVideo
		} else {								// -1:422-Direct input;
#if 1
			tempV = CVF_Video_CreateDefault(
				glob->width, glob->height, CVF_PT_UYVY, CVF_FT_FRAMEBASED);
#else
			tempV = CVF_Video_CreateDefault(
				glob->codecCont->width, glob->codecCont->height, CVF_PT_UYVY, CVF_FT_FRAMEBASED);
#endif
			assert(tempV);
			glob->cvfInVideo = CVF_Context_GetVideoRep(glob->cvfCont, tempV);
			assert(glob->cvfInVideo);
			if(tempV != glob->cvfInVideo) CVF_Video_Release(tempV);
		}
		
		glob->cvfInFrame = CVF_Context_GetNewFrame(glob->cvfCont, glob->cvfInVideo);
		glob->cvfOutFrame = CVF_Context_GetNewFrame(glob->cvfCont, glob->cvfOutVideo);
		assert(glob->cvfInFrame) ; assert(glob->cvfOutFrame);
		
		glob->cvfSourceArray = CFArrayCreateMutable(kCFAllocatorDefault, 128, &kCFTypeArrayCallBacks);
	} // if(!glob->cvfCont) 
	
bail:
	if(err) {
		glob->filterPreset = 0;	// Not available 
		if(glob->filterString) {
			CFRelease(glob->filterString); glob->filterString = NULL;
		}
	}
	return err;
}


// Clean up CoreVF Framework
static OSStatus closeCoreVF( lavcEncoderGlobals glob )
{
	logDebug(glob, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	OSStatus err = noErr;
	
	if( glob->cvfCont ) {
		if(glob->cvfSourceArray) CFRelease(glob->cvfSourceArray);
		if(glob->filterString) CFRelease(glob->filterString);
		
		if(glob->cvfInFrame) CVF_Context_UnlockFrame(glob->cvfCont, glob->cvfInFrame);
		if(glob->cvfOutFrame) CVF_Context_UnlockFrame(glob->cvfCont, glob->cvfOutFrame);
		
		CVF_Context_Release(glob->cvfCont);
		glob->cvfCont = NULL;
	}
	
	return err;
}
#endif


#pragma mark -


// Check param range and reset if needed
static void checkValues( lavcEncoderGlobals glob )
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
#if X264
	// In case that app passed older struct ; Some params' zero value are not allowed;
	// glob->params.CQM_PRESET
	// glob->params.NO_DCT_DECIMATE
	
	if( !glob->params.MB_DECISION ) glob->params.MB_DECISION=1;	// SIMPLE
	if( !glob->params.ME_METHOD ) glob->params.ME_METHOD=2;		// HEX
	if( !glob->params.CODER_TYPE ) glob->params.CODER_TYPE=2;	// CABAC
	if( !glob->params.DIRECTPRED ) glob->params.DIRECTPRED=2;	// Spatial
	if( !glob->params.TRELLIS ) glob->params.TRELLIS=2;			// FinalOnly
	if( !glob->params.THREADS ) glob->params.THREADS=9;			// Auto thread
	if( !glob->params.TURBO ) glob->params.TURBO=3;				// Turbo 2
	if( !glob->params.LEVEL ) glob->params.LEVEL=9;				// Level auto
	if( !glob->params.AQ_MODE ) glob->params.AQ_MODE=2;			// VARIANCE
#if 0
	if( glob->params.AQ_MODE > 2 ) glob->params.AQ_MODE=2;		// VARIANCE /* git-969 changed */
#else
																// AutoVARIANCE as 3 /* git-1184 changed */
#endif
	// glob->params.ADAPTIVE_BFRAME
	if( !glob->params.X264PROFILE ) glob->params.X264PROFILE=3;	// 3:High Profile
	if( !glob->params.WEIGHTP ) glob->params.WEIGHTP = 3;				// Smart analysis
	
	if( !glob->params.REFS ) glob->params.REFS = 3;				// 3
//	if( !glob->params.ME_SUBQ ) glob->params.ME_SUBQ = 7;		// 7
	if( !glob->params.ME_RANGE ) glob->params.ME_RANGE = 16;	// 16
//	if( !glob->params.MAX_BFRAMES ) glob->params.MAX_BFRAMES = 3;	// 3
	// glob->params.SC_THRESHOLD
	if( !glob->params.QCOMPRESS ) glob->params.QCOMPRESS = 60;	// 0.60
	// glob->params.NOISE_REDUCTION
	// glob->params.RC_BUFSIZE
	// glob->params.RC_MAXRATE
	// glob->params.DEBLOCK_ALPHA
	// glob->params.DEBLOCK_BETA
	if( !glob->params.AQ_STRENGTH ) glob->params.AQ_STRENGTH = 100;	// 1.0
//	if( !glob->params.KEYINT_MIN ) glob->params.KEYINT_MIN = 25;	// 25
	// glob->params.PSYRD_RDO
	// glob->params.PSYRD_TRELLIS
	if( !glob->params.MAX_QDIFF ) glob->params.MAX_QDIFF = 4;	// 4
	// glob->params.CHROMAOFFSET
	
	if( !glob->params.NCLC ) glob->params.NCLC = 1;				// no nclc
	if( !glob->params.NATIVE_FPS ) glob->params.NATIVE_FPS=1;	// NTSC
	// glob->params.USE3RDPASS
	// glob->params.EMBEDUUID
	// glob->params.FILTERPRESET
	// glob->params.SUBSAMPLING
	
//	// glob->params.MBTREE
//	// glob->params.PSY
	// glob->params.RC_LOOKAHEAD
	if( !glob->params.IP_FACTOR ) glob->params.IP_FACTOR = 140;			// 1.40
	if( !glob->params.PB_FACTOR ) glob->params.PB_FACTOR = 130;			// 1.30
	
	if( !glob->params.hSpacing ) glob->params.hSpacing = 1;		// 1
	if( !glob->params.vSpacing ) glob->params.vSpacing = 1;		// 1
	
	if( !glob->params.cleanApertureWidthD ) glob->params.cleanApertureWidthD = 1;		// 1
	if( !glob->params.cleanApertureHeightD ) glob->params.cleanApertureHeightD = 1;		// 1
	if( !glob->params.horizOffD ) glob->params.horizOffD = 1;		// 1
	if( !glob->params.vertOffD ) glob->params.vertOffD = 1;		// 1
	
	// glob->params.OVERRIDECRFQSCALE
	if( !glob->params.USERCRFQSCALE ) glob->params.USERCRFQSCALE = 23;	// 23
	// glob->params.OVERRIDEQMIN
	// glob->params.USERQMIN									// x264 r1795 changed default qmin from 10 to 0, thus 0 is acceptable now
#endif
#if MPEG4
	// In case that app passed older struct ; Some params' zero value are not allowed;
	// glob->params.CQM_PRESET
	// glob->params.NO_DCT_DECIMATE
	
	if( !glob->params.MB_DECISION ) glob->params.MB_DECISION=2;	// BITS
	if( !glob->params.ME_METHOD ) glob->params.ME_METHOD=1;		// DIA(EPZS)
	if( !glob->params.CODER_TYPE ) glob->params.CODER_TYPE=1;	// CAVLC
	if( !glob->params.DIRECTPRED ) glob->params.DIRECTPRED=3;	// Temporal
	if( !glob->params.TRELLIS ) glob->params.TRELLIS=1;			// Disabled
	if( !glob->params.THREADS ) glob->params.THREADS=9;			// Auto thread
	if( !glob->params.TURBO ) glob->params.TURBO=2;				// Turbo 1
	if( !glob->params.LEVEL ) glob->params.LEVEL=9;				// Level auto
	if( !glob->params.AQ_MODE ) glob->params.AQ_MODE=2;			// VARIANCE
	// glob->params.ADAPTIVE_BFRAME
	if( !glob->params.X264PROFILE ) glob->params.X264PROFILE=3;	// 3:High Profile
	if( !glob->params.WEIGHTP ) glob->params.WEIGHTP = 1;				// Disabled
	
	if( !glob->params.REFS ) glob->params.REFS = 1;				// 1
//	if( !glob->params.ME_SUBQ ) glob->params.ME_SUBQ = 8;		// 8
	if( !glob->params.ME_RANGE ) glob->params.ME_RANGE = 16;	// 16
//	if( !glob->params.MAX_BFRAMES ) glob->params.MAX_BFRAMES = 1;	// 1
	// glob->params.SC_THRESHOLD
	if( !glob->params.QCOMPRESS ) glob->params.QCOMPRESS = 60;	// 0.60
	// glob->params.NOISE_REDUCTION
	// glob->params.RC_BUFSIZE
	// glob->params.RC_MAXRATE
	// glob->params.DEBLOCK_ALPHA
	// glob->params.DEBLOCK_BETA
	if( !glob->params.AQ_STRENGTH ) glob->params.AQ_STRENGTH = 100;	// 1.0
	if( !glob->params.KEYINT_MIN ) glob->params.KEYINT_MIN = 25;	// 25
	// glob->params.PSYRD_RDO
	// glob->params.PSYRD_TRELLIS
	if( !glob->params.MAX_QDIFF ) glob->params.MAX_QDIFF = 3;	// 3
	// glob->params.CHROMAOFFSET
	
	if( !glob->params.NCLC ) glob->params.NCLC = 1;				// no nclc
	if( !glob->params.NATIVE_FPS ) glob->params.NATIVE_FPS=1;	// NTSC
	// glob->params.USE3RDPASS
	// glob->params.EMBEDUUID
	// glob->params.FILTERPRESET
	// glob->params.SUBSAMPLING
	
//	// glob->params.MBTREE
//	// glob->params.PSY
	// glob->params.RC_LOOKAHEAD
	if( !glob->params.IP_FACTOR ) glob->params.IP_FACTOR = 125;			// 1.25
	if( !glob->params.PB_FACTOR ) glob->params.PB_FACTOR = 125;			// 1.25
	
	if( !glob->params.hSpacing ) glob->params.hSpacing = 1;		// 1
	if( !glob->params.vSpacing ) glob->params.vSpacing = 1;		// 1
	
	if( !glob->params.cleanApertureWidthD ) glob->params.cleanApertureWidthD = 1;		// 1
	if( !glob->params.cleanApertureHeightD ) glob->params.cleanApertureHeightD = 1;		// 1
	if( !glob->params.horizOffD ) glob->params.horizOffD = 1;		// 1
	if( !glob->params.vertOffD ) glob->params.vertOffD = 1;		// 1
	
	// glob->params.OVERRIDECRFQSCALE
	if( !glob->params.USERCRFQSCALE ) glob->params.USERCRFQSCALE = 2;	// 2
	// glob->params.OVERRIDEQMIN
	if( !glob->params.USERQMIN ) glob->params.USERQMIN = 2;		// 2
#endif
#if XVID
	// In case that app passed older struct ; Some params' zero value are not allowed;
	// glob->params.CQM_PRESET
	// glob->params.NO_DCT_DECIMATE
	
	if( !glob->params.MB_DECISION ) glob->params.MB_DECISION=2;	// BITS
	if( !glob->params.ME_METHOD ) glob->params.ME_METHOD=4;		// FULL
	if( !glob->params.CODER_TYPE ) glob->params.CODER_TYPE=1;	// CAVLC
	if( !glob->params.DIRECTPRED ) glob->params.DIRECTPRED=3;	// Temporal
	if( !glob->params.TRELLIS ) glob->params.TRELLIS=3;			// All
	if( !glob->params.THREADS ) glob->params.THREADS=9;			// Auto thread
	if( !glob->params.TURBO ) glob->params.TURBO=1;				// Disabled
	if( !glob->params.LEVEL ) glob->params.LEVEL=9;				// Level auto
	if( !glob->params.AQ_MODE ) glob->params.AQ_MODE=2;			// VARIANCE
	// glob->params.ADAPTIVE_BFRAME
	if( !glob->params.X264PROFILE ) glob->params.X264PROFILE=3;	// 3:High Profile
	if( !glob->params.WEIGHTP ) glob->params.WEIGHTP = 1;				// Disabled
	
	if( !glob->params.REFS ) glob->params.REFS = 1;				// 1
//	if( !glob->params.ME_SUBQ ) glob->params.ME_SUBQ = 8;		// 8
	if( !glob->params.ME_RANGE ) glob->params.ME_RANGE = 16;	// 16
//	if( !glob->params.MAX_BFRAMES ) glob->params.MAX_BFRAMES = 1;	// 1
	// glob->params.SC_THRESHOLD
	if( !glob->params.QCOMPRESS ) glob->params.QCOMPRESS = 60;	// 0.60
	// glob->params.NOISE_REDUCTION
	// glob->params.RC_BUFSIZE
	// glob->params.RC_MAXRATE
	// glob->params.DEBLOCK_ALPHA
	// glob->params.DEBLOCK_BETA
	if( !glob->params.AQ_STRENGTH ) glob->params.AQ_STRENGTH = 100;	// 1.0
	if( !glob->params.KEYINT_MIN ) glob->params.KEYINT_MIN = 25;	// 25
	// glob->params.PSYRD_RDO
	// glob->params.PSYRD_TRELLIS
	if( !glob->params.MAX_QDIFF ) glob->params.MAX_QDIFF = 3;	// 3
	// glob->params.CHROMAOFFSET
	
	if( !glob->params.NCLC ) glob->params.NCLC = 1;				// no nclc
	if( !glob->params.NATIVE_FPS ) glob->params.NATIVE_FPS=1;	// NTSC
	// glob->params.USE3RDPASS
	// glob->params.EMBEDUUID
	// glob->params.FILTERPRESET
	// glob->params.SUBSAMPLING
	
//	// glob->params.MBTREE
//	// glob->params.PSY
	// glob->params.RC_LOOKAHEAD
	if( !glob->params.IP_FACTOR ) glob->params.IP_FACTOR = 125;			// 1.25
	if( !glob->params.PB_FACTOR ) glob->params.PB_FACTOR = 150;			// 1.50
	
	if( !glob->params.hSpacing ) glob->params.hSpacing = 1;		// 1
	if( !glob->params.vSpacing ) glob->params.vSpacing = 1;		// 1
	
	if( !glob->params.cleanApertureWidthD ) glob->params.cleanApertureWidthD = 1;		// 1
	if( !glob->params.cleanApertureHeightD ) glob->params.cleanApertureHeightD = 1;		// 1
	if( !glob->params.horizOffD ) glob->params.horizOffD = 1;		// 1
	if( !glob->params.vertOffD ) glob->params.vertOffD = 1;		// 1
	
	// glob->params.OVERRIDECRFQSCALE
	if( !glob->params.USERCRFQSCALE ) glob->params.USERCRFQSCALE = 2;	// 2
	// glob->params.OVERRIDEQMIN
	if( !glob->params.USERQMIN ) glob->params.USERQMIN = 2;		// 2
#endif
} // checkValues()


// Initialize params
static void initValues( lavcEncoderGlobals glob )
{
//	logDebug(NULL, "lavcEncoder: [%08x %s]\n", glob, __FUNCTION__);
	
	glob->params.REV_STRUCT = REV_X264OPENGOP;
#if X264
	// Initial values for CODE_ID_H264 (=x264)
	glob->params.FLAG_QSCALE = FALSE;
	glob->params.FLAG_4MV = FALSE;
	glob->params.FLAG_QPEL = FALSE;
	glob->params.FLAG_GMC = FALSE;
	glob->params.FLAG_MV0 = FALSE;
	glob->params.FLAG_PART = FALSE;
	glob->params.FLAG_LOOP_FILTER = TRUE;
	glob->params.FLAG_PSNR = FALSE;
	glob->params.FLAG_NORMALIZE_AQP = FALSE;
	glob->params.FLAG_INTERLACED_DCT = FALSE;
	glob->params.FLAG_AC_PRED = FALSE;
	glob->params.FLAG_CBP_RD = FALSE;
	glob->params.FLAG_QP_RD = FALSE;
	glob->params.FLAG_CLOSED_GOP = TRUE;
	
	glob->params.NATIVE_FPS = 1;		// NTSC
	glob->params.MB_DECISION = 1;		// SIMPLE
	glob->params.RC_QSQUISH = 0;		// clip
	glob->params.MPEG_QUANT = 0;		// h263
	glob->params.THREADS = 9;			// Auto thread
	glob->params.GAMMA = 0;				// no gamma
	glob->params.NCLC = 1;				// no nclc
	glob->params.ME_METHOD = 2;			// HEX
	
	glob->params.SC_THRESHOLD = 40;		// 
	glob->params.QCOMPRESS = 60;		// 0.60
	glob->params.NOISE_REDUCTION = 0;	// 
	glob->params.RC_MAXRATE = 0;		//
	
	glob->params.REFS = 3;				// 
	glob->params.ME_RANGE = 16;			// 
	glob->params.MAX_BFRAMES = 3;		// 
	glob->params.CODER_TYPE = 2;		// CABAC
	glob->params.DIRECTPRED = 2;		// Spatial
	glob->params.CRF = 0;				// no crf
	glob->params.ADAPTIVE_BFRAME = 1;	// use adaptive b frame - fast
	glob->params.ME_SUBQ = 7;			// 
	
	glob->params.TRELLIS = 2;			// FinalOnly
	glob->params.TURBO = 3;				// Turbo 2
	glob->params.CHROMA_ME = 1;			// ON
	glob->params.PART_4X4 = 0;			// OFF
	glob->params.BIDIR_ME = 0;			// OFF
	glob->params.USE3RDPASS = 0;		// OFF
	glob->params.LEVEL = 9;				// Level auto
	glob->params.EMBEDUUID = 0;			// OFF
	
	glob->params.FLAG2_BPYRAMID = TRUE;
	glob->params.FLAG2_WPRED = TRUE;
	glob->params.FLAG2_MIXED_REFS = TRUE;
	glob->params.FLAG2_8X8DCT = TRUE;
	glob->params.FLAG2_FASTPSKIP = TRUE;
	glob->params.FLAG2_AUD = FALSE;
	glob->params.FLAG2_BRDO = FALSE;
	glob->params.FLAG2_MBTREE = TRUE;
	glob->params.FLAG2_PSY = TRUE;
	glob->params.FLAG2_SSIM = FALSE;
	
	glob->params.RC_BUFSIZE = 0;		// treated as 0 bit
	glob->params.AQ_STRENGTH = 100;		// 1.0
	glob->params.PSYRD_RDO = 10;		// 1.0
	glob->params.PSYRD_TRELLIS = 0;		// 0.0
	
	glob->params.DEBLOCK_ALPHA = 0;		// 0
	glob->params.DEBLOCK_BETA = 0;		// 0
	glob->params.AQ_MODE = 2;			// VARIANCE
	glob->params.LUMI_MASKING = 0;		// 0:OFF
	
	glob->params.KEYINT_MIN = 0;		// 0:auto
	glob->params.CQM_PRESET = 0;		// 0:flat16
	glob->params.NO_DCT_DECIMATE = 0;	// 0:On
	glob->params.MAX_QDIFF = 4;			// 4
	
	glob->params.CHROMAOFFSET = 0;		// 0
	glob->params.FILTERPRESET = 0;		// 0
	glob->params.SUBSAMPLING = 0;		// 0
	glob->params.X264PROFILE = 3;		// 3
	
	glob->params.X264PRESET = 0;		// 0
	glob->params.X264TUNE = 0;			// 0
//	glob->params.MBTREE = 1;			// 1:ON
//	glob->params.PSY = 1;				// 1:ON
	
	glob->params.RC_LOOKAHEAD = 40;		// 40
	glob->params.IP_FACTOR = 140;		// 1.4
	glob->params.PB_FACTOR = 130;		// 1.3
	
	glob->params.WEIGHTP = 3;			// Smart analysis
	glob->params.TOPFIELDFIRST = 0;		// Progressive or Bottom field first
	glob->params.LOSSLESS = 0;			// No lossless
	glob->params.BD_TUNE = 0;			// OFF
	
	glob->params.USEASPECTRATIO = 0;	// Square pixel
	glob->params.USECLEANAPERTURE = 0;	// Fully Clean Aperture
	glob->params.TAGASPECTRATIO = 0;	// Custom
	glob->params.TAGCLEANAPERTURE = 0;	// Custom
	
	glob->params.hSpacing = 1;			// Square pixel
	glob->params.vSpacing = 1;			// Square pixel
	glob->params.cleanApertureWidthN = 0;	// undefined
	glob->params.cleanApertureWidthD = 1;	// equal denomiter
	glob->params.cleanApertureHeightN = 0;	// undefined
	glob->params.cleanApertureHeightD = 1;	// equal denomiter
	glob->params.horizOffN = 0;			// no offset
	glob->params.horizOffD = 1;			// equal denomiter
	glob->params.vertOffN = 0;			// no offset
	glob->params.vertOffD = 1;			// equal denomiter
	
	glob->params.OVERRIDECRFQSCALE = 0;	// OFF
	glob->params.USERCRFQSCALE = 23;	// 23
	glob->params.OVERRIDEQMIN = 1;		// ON
	glob->params.USERQMIN = 4;			// 4
#endif
#if MPEG4
	// Initial values for CODE_ID_MPEG4
	glob->params.FLAG_QSCALE = FALSE;
	glob->params.FLAG_4MV = TRUE;
	glob->params.FLAG_QPEL = FALSE;
	glob->params.FLAG_GMC = FALSE;
	glob->params.FLAG_MV0 = TRUE;
	glob->params.FLAG_PART = FALSE;
	glob->params.FLAG_LOOP_FILTER = FALSE;
	glob->params.FLAG_PSNR = FALSE;
	glob->params.FLAG_NORMALIZE_AQP = FALSE;
	glob->params.FLAG_INTERLACED_DCT = FALSE;
	glob->params.FLAG_AC_PRED = FALSE;
	glob->params.FLAG_CBP_RD = FALSE;
	glob->params.FLAG_QP_RD = FALSE;
	glob->params.FLAG_CLOSED_GOP = TRUE;
	
	glob->params.NATIVE_FPS = 1;		// NTSC
	glob->params.MB_DECISION = 2;		// BITS
	glob->params.RC_QSQUISH = 1;		// no clip
	glob->params.MPEG_QUANT = 0;		// h263
	glob->params.THREADS = 9;			// Auto thread
	glob->params.GAMMA = 0;				// no gamma
	glob->params.NCLC = 1;				// no nclc
	glob->params.ME_METHOD = 1;			// DIA(EPZS)
	
	glob->params.SC_THRESHOLD = 0;		// 
	glob->params.QCOMPRESS = 60;		// 0.60
	glob->params.NOISE_REDUCTION = 0;	// 
	glob->params.RC_MAXRATE = 0;		//
	
	glob->params.REFS = 1;				// 
	glob->params.ME_RANGE = 16;			// 
	glob->params.MAX_BFRAMES = 1;		// 
	glob->params.CODER_TYPE = 1;		// CAVLC
	glob->params.DIRECTPRED = 3;		// Temporal
	glob->params.CRF = 0;				// No crf
	glob->params.ADAPTIVE_BFRAME = 0;	// No adaptive b frame
	glob->params.ME_SUBQ = 8;			// 
	
	glob->params.TRELLIS = 1;			// Disabled
	glob->params.TURBO = 2;				// Turbo 1
	glob->params.CHROMA_ME = 0;			// OFF
	glob->params.PART_4X4 = 0;			// OFF
	glob->params.BIDIR_ME = 0;			// OFF
	glob->params.USE3RDPASS = 0;		// OFF
	glob->params.LEVEL = 9;				// Level auto
	glob->params.EMBEDUUID = 0;			// OFF
	
	glob->params.FLAG2_BPYRAMID = FALSE;
	glob->params.FLAG2_WPRED = FALSE;
	glob->params.FLAG2_MIXED_REFS = FALSE;
	glob->params.FLAG2_8X8DCT = FALSE;
	glob->params.FLAG2_FASTPSKIP = FALSE;
	glob->params.FLAG2_AUD = FALSE;
	glob->params.FLAG2_BRDO = FALSE;
	glob->params.FLAG2_MBTREE = FALSE;
	glob->params.FLAG2_PSY = FALSE;
	glob->params.FLAG2_SSIM = FALSE;
	
	glob->params.RC_BUFSIZE = 0;		// treated as 0 bit
	glob->params.AQ_STRENGTH = 100;		// 1.0
	glob->params.PSYRD_RDO = 10;		// 1.0
	glob->params.PSYRD_TRELLIS = 0;		// 0.0
	
	glob->params.DEBLOCK_ALPHA = 0;		// 0
	glob->params.DEBLOCK_BETA = 0;		// 0
	glob->params.AQ_MODE = 2;			// VARIANCE
	glob->params.LUMI_MASKING = 0;		// 0:OFF
	
	glob->params.KEYINT_MIN = 25;		// 25
	glob->params.CQM_PRESET = 0;		// 0:flat16
	glob->params.NO_DCT_DECIMATE = 1;	// 1:Off
	glob->params.MAX_QDIFF = 3;			// 3
	
	glob->params.CHROMAOFFSET = 0;		// 0
	glob->params.FILTERPRESET = 0;		// 0
	glob->params.SUBSAMPLING = 0;		// 0
	glob->params.X264PROFILE = 3;		// 3
	
	glob->params.X264PRESET = 0;		// 0
	glob->params.X264TUNE = 0;			// 0
//	glob->params.MBTREE = 0;			// 0:OFF
//	glob->params.PSY = 0;				// 0:OFF
	
	glob->params.RC_LOOKAHEAD = 0;		// 0
	glob->params.IP_FACTOR = 125;		// 1.25
	glob->params.PB_FACTOR = 125;		// 1.25
	
	glob->params.WEIGHTP = 1;			// Disabled
	glob->params.TOPFIELDFIRST = 0;		// Progressive or Bottom field first
	glob->params.LOSSLESS = 0;			// No lossless
	glob->params.BD_TUNE = 0;			// OFF
	
	glob->params.USEASPECTRATIO = 0;	// Square pixel
	glob->params.USECLEANAPERTURE = 0;	// Fully Clean Aperture
	glob->params.TAGASPECTRATIO = 0;	// Custom
	glob->params.TAGCLEANAPERTURE = 0;	// Custom
	
	glob->params.hSpacing = 1;			// Square pixel
	glob->params.vSpacing = 1;			// Square pixel
	glob->params.cleanApertureWidthN = 0;	// undefined
	glob->params.cleanApertureWidthD = 1;	// equal denomiter
	glob->params.cleanApertureHeightN = 0;	// undefined
	glob->params.cleanApertureHeightD = 1;	// equal denomiter
	glob->params.horizOffN = 0;			// no offset
	glob->params.horizOffD = 1;			// equal denomiter
	glob->params.vertOffN = 0;			// no offset
	glob->params.vertOffD = 1;			// equal denomiter
	
	glob->params.OVERRIDECRFQSCALE = 0;	// OFF
	glob->params.USERCRFQSCALE = 2;		// 2
	glob->params.OVERRIDEQMIN = 0;		// OFF
	glob->params.USERQMIN = 2;			// 2
#endif
#if XVID
	// Initial values for CODE_ID_MPEG4
	glob->params.FLAG_QSCALE = FALSE;
	glob->params.FLAG_4MV = TRUE;
	glob->params.FLAG_QPEL = FALSE;
	glob->params.FLAG_GMC = FALSE;
	glob->params.FLAG_MV0 = FALSE;
	glob->params.FLAG_PART = FALSE;
	glob->params.FLAG_LOOP_FILTER = FALSE;
	glob->params.FLAG_PSNR = FALSE;
	glob->params.FLAG_NORMALIZE_AQP = FALSE;
	glob->params.FLAG_INTERLACED_DCT = FALSE;
	glob->params.FLAG_AC_PRED = TRUE;
	glob->params.FLAG_CBP_RD = FALSE;
	glob->params.FLAG_QP_RD = FALSE;
	glob->params.FLAG_CLOSED_GOP = TRUE;
	
	glob->params.NATIVE_FPS = 1;		// NTSC
	glob->params.MB_DECISION = 1;		// SIMPLE
	glob->params.RC_QSQUISH = 0;		// no clip
	glob->params.MPEG_QUANT = 0;		// h263
	glob->params.THREADS = 9;			// Auto thread
	glob->params.GAMMA = 0;				// no gamma
	glob->params.NCLC = 1;				// no nclc
	glob->params.ME_METHOD = 4;			// FULL
	
	glob->params.SC_THRESHOLD = 0;		// 
	glob->params.QCOMPRESS = 60;		// 0.60
	glob->params.NOISE_REDUCTION = 0;	// 
	glob->params.RC_MAXRATE = 0;		//
	
	glob->params.REFS = 1;				// 
	glob->params.ME_RANGE = 16;			// 
	glob->params.MAX_BFRAMES = 1;		// 
	glob->params.CODER_TYPE = 1;		// CAVLC
	glob->params.DIRECTPRED = 3;		// Temporal
	glob->params.CRF = 0;				// No crf
	glob->params.ADAPTIVE_BFRAME = 0;	// No adaptive b frame
	glob->params.ME_SUBQ = 8;			// 
	
	glob->params.TRELLIS = 3;			// All
	glob->params.TURBO = 1;				// Disabled
	glob->params.CHROMA_ME = 1;			// ON
	glob->params.PART_4X4 = 0;			// OFF
	glob->params.BIDIR_ME = 0;			// OFF
	glob->params.USE3RDPASS = 0;		// OFF
	glob->params.LEVEL = 9;				// Level auto
	glob->params.EMBEDUUID = 0;			// OFF
	
	glob->params.FLAG2_BPYRAMID = FALSE;
	glob->params.FLAG2_WPRED = FALSE;
	glob->params.FLAG2_MIXED_REFS = FALSE;
	glob->params.FLAG2_8X8DCT = FALSE;
	glob->params.FLAG2_FASTPSKIP = FALSE;
	glob->params.FLAG2_AUD = FALSE;
	glob->params.FLAG2_BRDO = FALSE;
	glob->params.FLAG2_MBTREE = FALSE;
	glob->params.FLAG2_PSY = FALSE;
	glob->params.FLAG2_SSIM = FALSE;
	
	glob->params.RC_BUFSIZE = 0;		// treated as 0 bit
	glob->params.AQ_STRENGTH = 100;		// 1.0
	glob->params.PSYRD_RDO = 10;		// 1.0
	glob->params.PSYRD_TRELLIS = 0;		// 0.0
	
	glob->params.DEBLOCK_ALPHA = 0;		// 0
	glob->params.DEBLOCK_BETA = 0;		// 0
	glob->params.AQ_MODE = 2;			// VARIANCE
	glob->params.LUMI_MASKING = 0;		// 0:OFF
	
	glob->params.KEYINT_MIN = 25;		// 25
	glob->params.CQM_PRESET = 0;		// 0:flat16
	glob->params.NO_DCT_DECIMATE = 1;	// 1:Off
	glob->params.MAX_QDIFF = 3;			// 3
	
	glob->params.CHROMAOFFSET = 0;		// 0
	glob->params.FILTERPRESET = 0;		// 0
	glob->params.SUBSAMPLING = 0;		// 0
	glob->params.X264PROFILE = 3;		// 3
	
	glob->params.X264PRESET = 0;		// 0
	glob->params.X264TUNE = 0;			// 0
//	glob->params.MBTREE = 0;			// 0:OFF
//	glob->params.PSY = 0;				// 0:OFF
	
	glob->params.RC_LOOKAHEAD = 0;		// 0
	glob->params.IP_FACTOR = 125;		// 1.25
	glob->params.PB_FACTOR = 150;		// 1.5
	
	glob->params.WEIGHTP = 1;			// Disabled
	glob->params.TOPFIELDFIRST = 0;		// Progressive or Bottom field first
	glob->params.LOSSLESS = 0;			// No lossless
	glob->params.BD_TUNE = 0;			// OFF
	
	glob->params.USEASPECTRATIO = 0;	// Square pixel
	glob->params.USECLEANAPERTURE = 0;	// Fully Clean Aperture
	glob->params.TAGASPECTRATIO = 0;	// Custom
	glob->params.TAGCLEANAPERTURE = 0;	// Custom
	
	glob->params.hSpacing = 1;			// Square pixel
	glob->params.vSpacing = 1;			// Square pixel
	glob->params.cleanApertureWidthN = 0;	// undefined
	glob->params.cleanApertureWidthD = 1;	// equal denomiter
	glob->params.cleanApertureHeightN = 0;	// undefined
	glob->params.cleanApertureHeightD = 1;	// equal denomiter
	glob->params.horizOffN = 0;			// no offset
	glob->params.horizOffD = 1;			// equal denomiter
	glob->params.vertOffN = 0;			// no offset
	glob->params.vertOffD = 1;			// equal denomiter
	
	glob->params.OVERRIDECRFQSCALE = 0;	// OFF
	glob->params.USERCRFQSCALE = 2;		// 2
	glob->params.OVERRIDEQMIN = 0;		// OFF
	glob->params.USERQMIN = 2;			// 2
#endif
} // initValues()
