/*
 *  UIModel.m
 *  UIbundle
 *
 *  Created by Takashi Mochizuki on 07/12/02.
 *  Copyright 2007-2010 MyCometG3. All rights reserved.
 *
 */

#import "UIModel.h"

@implementation kUIModelNAME

#define UIBool		NSNumber numberWithBool
#define UIInt		NSNumber numberWithInt
#define UIFloat		NSNumber numberWithFloat
#define UIChar		NSNumber numberWithChar
#define UIZero		[NSNumber numberWithInt:0]
#define UIOne		[NSNumber numberWithInt:1]
#define UITwo		[NSNumber numberWithInt:2]
#define UIFalse		[NSNumber numberWithBool:FALSE]
#define UITrue		[NSNumber numberWithBool:TRUE]

#pragma mark -

+ (void)initialize
{
    [self setKeys:[NSArray arrayWithObject:@"fourCC"] triggerChangeNotificationsForDependentKey:@"isMp4v"];
    [self setKeys:[NSArray arrayWithObject:@"fourCC"] triggerChangeNotificationsForDependentKey:@"isAvc1"];
    [self setKeys:[NSArray arrayWithObject:@"fourCC"] triggerChangeNotificationsForDependentKey:@"isXvid"];
    [self setKeys:[NSArray arrayWithObject:@"fourCC"] triggerChangeNotificationsForDependentKey:@"isAvc1OrMp4v"];
    [self setKeys:[NSArray arrayWithObject:@"fourCC"] triggerChangeNotificationsForDependentKey:@"isMp4vOrXvid"];
    [self setKeys:[NSArray arrayWithObject:@"fourCC"] triggerChangeNotificationsForDependentKey:@"isXvidOrAvc1"];
}

- (id)init
{
	if(self = [super init]) {
//		[self resetToDefault];
	}
	return self;
}

- (void)resetToParams:(params*)inParams
{
	if(!fourCC) [self setValue:@"avc1" forKey:@"fourCC"];
	
	//
	memcpy(&store, inParams, sizeof(params));
	
	[self setValue:[UIBool:inParams->LOG_INFO]			forKey:@"LOG_INFO"];
	[self setValue:[UIBool:inParams->LOG_DEBUG]			forKey:@"LOG_DEBUG"];
	[self setValue:[UIBool:inParams->LOG_STATS]			forKey:@"LOG_STATS"];
	[self setValue:[NSNumber numberWithUnsignedChar:inParams->REV_STRUCT]	forKey:@"REV_STRUCT"];

	[self setValue:[UIBool:inParams->FLAG_QSCALE]		forKey:@"FLAG_QSCALE"];
	[self setValue:[UIBool:inParams->FLAG_4MV]			forKey:@"FLAG_4MV"];
	[self setValue:[UIBool:inParams->FLAG_QPEL]			forKey:@"FLAG_QPEL"];
	[self setValue:[UIBool:inParams->FLAG_GMC]			forKey:@"FLAG_GMC"];
	[self setValue:[UIBool:inParams->FLAG_MV0]			forKey:@"FLAG_MV0"];
	[self setValue:[UIBool:inParams->FLAG_PART]			forKey:@"FLAG_PART"];
	[self setValue:[UIBool:inParams->FLAG_LOOP_FILTER]	forKey:@"FLAG_LOOP_FILTER"];
	[self setValue:[UIBool:inParams->FLAG_PSNR]			forKey:@"FLAG_PSNR"];
	[self setValue:[UIBool:inParams->FLAG_NORMALIZE_AQP]	forKey:@"FLAG_NORMALIZE_AQP"];
	[self setValue:[UIBool:inParams->FLAG_INTERLACED_DCT]	forKey:@"FLAG_INTERLACED_DCT"];
	[self setValue:[UIBool:inParams->FLAG_AC_PRED]		forKey:@"FLAG_AC_PRED"];
	[self setValue:[UIBool:inParams->FLAG_CBP_RD]		forKey:@"FLAG_CBP_RD"];
	[self setValue:[UIBool:inParams->FLAG_QP_RD]		forKey:@"FLAG_QP_RD"];
	[self setValue:[UIBool:inParams->FLAG_CLOSED_GOP]	forKey:@"FLAG_CLOSED_GOP"];
	
	[self setValue:[UIChar:inParams->NATIVE_FPS]		forKey:@"NATIVE_FPS"];
	[self setValue:[UIChar:inParams->MB_DECISION]		forKey:@"MB_DECISION"];
	[self setValue:[UIChar:inParams->RC_QSQUISH]		forKey:@"RC_QSQUISH"];
	[self setValue:[UIChar:inParams->MPEG_QUANT]		forKey:@"MPEG_QUANT"];
	[self setValue:[UIChar:inParams->THREADS]			forKey:@"THREADS"];	// 2:dual
	[self setValue:[UIChar:inParams->GAMMA]				forKey:@"GAMMA"];
	[self setValue:[UIChar:inParams->NCLC]				forKey:@"NCLC"];
	[self setValue:[UIChar:inParams->ME_METHOD]			forKey:@"ME_METHOD"];	// 2:HEX
	
	[self setValue:[UIInt:inParams->SC_THRESHOLD]		forKey:@"SC_THRESHOLD"];
	[self setValue:[UIFloat:(inParams->QCOMPRESS/100.0)]	forKey:@"QCOMPRESS"];	//!!!!!!!!!!!!!!!!!!!!!
	[self setValue:[UIInt:inParams->NOISE_REDUCTION]	forKey:@"NOISE_REDUCTION"];
	[self setValue:[UIInt:inParams->RC_MAXRATE]			forKey:@"RC_MAXRATE"];
	
	[self setValue:[UIChar:inParams->REFS]				forKey:@"REFS"];
	[self setValue:[UIChar:inParams->ME_RANGE]			forKey:@"ME_RANGE"];
	[self setValue:[UIChar:inParams->MAX_BFRAMES]		forKey:@"MAX_BFRAMES"];
	[self setValue:[UIChar:inParams->CODER_TYPE]		forKey:@"CODER_TYPE"];
	[self setValue:[UIChar:inParams->DIRECTPRED]		forKey:@"DIRECTPRED"];
	[self setValue:[UIChar:inParams->CRF]				forKey:@"CRF"];
	[self setValue:[UIChar:inParams->ADAPTIVE_BFRAME]	forKey:@"ADAPTIVE_BFRAME"];
	[self setValue:[UIChar:inParams->ME_SUBQ]			forKey:@"ME_SUBQ"];
	
	[self setValue:[UIChar:inParams->TRELLIS]			forKey:@"TRELLIS"];
	[self setValue:[UIChar:inParams->TURBO]				forKey:@"TURBO"];
	[self setValue:[UIChar:inParams->CHROMA_ME]			forKey:@"CHROMA_ME"];
	[self setValue:[UIChar:inParams->PART_4X4]			forKey:@"PART_4X4"];
	[self setValue:[UIChar:inParams->BIDIR_ME]			forKey:@"BIDIR_ME"];
	[self setValue:[UIChar:inParams->USE3RDPASS]		forKey:@"USE3RDPASS"];
	[self setValue:[UIChar:inParams->LEVEL]				forKey:@"LEVEL"];
	[self setValue:[UIChar:inParams->EMBEDUUID]			forKey:@"EMBEDUUID"];
	
	[self setValue:[UIBool:inParams->FLAG2_BPYRAMID]	forKey:@"FLAG2_BPYRAMID"];
	[self setValue:[UIBool:inParams->FLAG2_WPRED]		forKey:@"FLAG2_WPRED"];
	[self setValue:[UIBool:inParams->FLAG2_MIXED_REFS]	forKey:@"FLAG2_MIXED_REFS"];
	[self setValue:[UIBool:inParams->FLAG2_8X8DCT]		forKey:@"FLAG2_8X8DCT"];
	[self setValue:[UIBool:inParams->FLAG2_FASTPSKIP]	forKey:@"FLAG2_FASTPSKIP"];
	[self setValue:[UIBool:inParams->FLAG2_AUD]			forKey:@"FLAG2_AUD"];
	[self setValue:[UIBool:inParams->FLAG2_BRDO]		forKey:@"FLAG2_BRDO"];
	[self setValue:[UIBool:inParams->FLAG2_MBTREE]		forKey:@"FLAG2_MBTREE"];
	[self setValue:[UIBool:inParams->FLAG2_PSY]			forKey:@"FLAG2_PSY"];
	[self setValue:[UIBool:inParams->FLAG2_SSIM]		forKey:@"FLAG2_SSIM"];
	
	[self setValue:[UIInt:inParams->RC_BUFSIZE]			forKey:@"RC_BUFSIZE"];
	[self setValue:[UIFloat:(inParams->AQ_STRENGTH/100.0)]	forKey:@"AQ_STRENGTH"];	//!!!!!!!!!!!!!!!!!!!!!
	[self setValue:[UIFloat:(inParams->PSYRD_RDO/10.0)]	forKey:@"PSYRD_RDO"];	//!!!!!!!!!!!!!!!!!!!!!
	[self setValue:[UIFloat:(inParams->PSYRD_TRELLIS/10.0)]	forKey:@"PSYRD_TRELLIS"];	//!!!!!!!!!!!!!!!!!!!!!
	
	[self setValue:[UIChar:inParams->DEBLOCK_ALPHA]		forKey:@"DEBLOCK_ALPHA"];
	[self setValue:[UIChar:inParams->DEBLOCK_BETA]		forKey:@"DEBLOCK_BETA"];
	[self setValue:[UIChar:inParams->AQ_MODE]			forKey:@"AQ_MODE"];
	[self setValue:[UIChar:inParams->LUMI_MASKING]		forKey:@"LUMI_MASKING"];
	
	[self setValue:[UIChar:inParams->KEYINT_MIN]		forKey:@"KEYINT_MIN"];
	[self setValue:[UIChar:inParams->CQM_PRESET]		forKey:@"CQM_PRESET"];
	[self setValue:[UIChar:inParams->NO_DCT_DECIMATE]	forKey:@"NO_DCT_DECIMATE"];
	[self setValue:[UIChar:inParams->MAX_QDIFF]			forKey:@"MAX_QDIFF"];
	
	[self setValue:[UIChar:inParams->CHROMAOFFSET]		forKey:@"CHROMAOFFSET"];
	[self setValue:[UIChar:inParams->FILTERPRESET]		forKey:@"FILTERPRESET"];
	[self setValue:[UIChar:inParams->SUBSAMPLING]		forKey:@"SUBSAMPLING"];
	[self setValue:[UIChar:inParams->X264PROFILE]		forKey:@"X264PROFILE"];
	
	[self setValue:[UIChar:inParams->X264PRESET]		forKey:@"X264PRESET"];
	[self setValue:[UIChar:inParams->X264TUNE]			forKey:@"X264TUNE"];
//	[self setValue:[UIChar:inParams->MBTREE]			forKey:@"MBTREE"];
//	[self setValue:[UIChar:inParams->PSY]				forKey:@"PSY"];
	
	[self setValue:[UIInt:inParams->RC_LOOKAHEAD]		forKey:@"RC_LOOKAHEAD"];
	[self setValue:[UIFloat:(inParams->IP_FACTOR/100.0)]	forKey:@"IP_FACTOR"];	//!!!!!!!!!!!!!!!!!!!!!
	[self setValue:[UIFloat:(inParams->PB_FACTOR/100.0)]	forKey:@"PB_FACTOR"];	//!!!!!!!!!!!!!!!!!!!!!
	
	[self setValue:[UIChar:inParams->WEIGHTP]			forKey:@"WEIGHTP"];
	[self setValue:[UIChar:inParams->TOPFIELDFIRST]		forKey:@"TOPFIELDFIRST"];
	[self setValue:[UIChar:inParams->LOSSLESS]			forKey:@"LOSSLESS"];
	[self setValue:[UIChar:inParams->BD_TUNE]			forKey:@"BD_TUNE"];
	
	[self setValue:[UIChar:inParams->USEASPECTRATIO]		forKey:@"USEASPECTRATIO"];
	[self setValue:[UIChar:inParams->USECLEANAPERTURE]		forKey:@"USECLEANAPERTURE"];
	[self setValue:[UIChar:inParams->TAGASPECTRATIO]		forKey:@"TAGASPECTRATIO"];
	[self setValue:[UIChar:inParams->TAGCLEANAPERTURE]		forKey:@"TAGCLEANAPERTURE"];
	
	[self setValue:[UIInt:inParams->HSPACING]			forKey:@"HSPACING"];
	[self setValue:[UIInt:inParams->VSPACING]			forKey:@"VSPACING"];
	[self setValue:[UIInt:inParams->CLEANAPERTUREWIDTHN]	forKey:@"CLEANAPERTUREWIDTHN"];
	[self setValue:[UIInt:inParams->CLEANAPERTUREWIDTHD]	forKey:@"CLEANAPERTUREWIDTHD"];
	[self setValue:[UIInt:inParams->CLEANAPERTUREHEIGHTN]	forKey:@"CLEANAPERTUREHEIGHTN"];
	[self setValue:[UIInt:inParams->CLEANAPERTUREHEIGHTD]	forKey:@"CLEANAPERTUREHEIGHTD"];
	[self setValue:[UIInt:inParams->HORIZOFFN]			forKey:@"HORIZOFFN"];
	[self setValue:[UIInt:inParams->HORIZOFFD]			forKey:@"HORIZOFFD"];
	[self setValue:[UIInt:inParams->VERTOFFN]			forKey:@"VERTOFFN"];
	[self setValue:[UIInt:inParams->VERTOFFD]			forKey:@"VERTOFFD"];
	
	[self setValue:[UIChar:inParams->OVERRIDECRFQSCALE]	forKey:@"OVERRIDECRFQSCALE"];
	[self setValue:[UIChar:inParams->USERCRFQSCALE]		forKey:@"USERCRFQSCALE"];
	[self setValue:[UIChar:inParams->OVERRIDEQMIN]		forKey:@"OVERRIDEQMIN"];
	[self setValue:[UIChar:inParams->USERQMIN]			forKey:@"USERQMIN"];
	
	[self setValue:[UIChar:inParams->FAKEINTERLACED]	forKey:@"FAKEINTERLACED"];
}

- (params*)currentParams
{
	params* outParams = (params*)&store;
	memset(&store, 0, sizeof(params));
	
	outParams->LOG_INFO			= [[self valueForKey:@"LOG_INFO"] boolValue];
	outParams->LOG_DEBUG		= [[self valueForKey:@"LOG_DEBUG"] boolValue];
	outParams->LOG_STATS		= [[self valueForKey:@"LOG_STATS"] boolValue];
//	outParams->REV_STRUCT		= [[self valueForKey:@"REV_STRUCT"] unsignedCharValue];
	
	outParams->FLAG_QSCALE		= [[self valueForKey:@"FLAG_QSCALE"] boolValue];
	outParams->FLAG_4MV			= [[self valueForKey:@"FLAG_4MV"] boolValue];
	outParams->FLAG_QPEL		= [[self valueForKey:@"FLAG_QPEL"] boolValue];
	outParams->FLAG_GMC			= [[self valueForKey:@"FLAG_GMC"] boolValue];
	outParams->FLAG_MV0			= [[self valueForKey:@"FLAG_MV0"] boolValue];
	outParams->FLAG_PART		= [[self valueForKey:@"FLAG_PART"] boolValue];
	outParams->FLAG_LOOP_FILTER	= [[self valueForKey:@"FLAG_LOOP_FILTER"] boolValue];
	outParams->FLAG_PSNR		= [[self valueForKey:@"FLAG_PSNR"] boolValue];
	outParams->FLAG_NORMALIZE_AQP	= [[self valueForKey:@"FLAG_NORMALIZE_AQP"] boolValue];
	outParams->FLAG_INTERLACED_DCT	= [[self valueForKey:@"FLAG_INTERLACED_DCT"] boolValue];
	outParams->FLAG_AC_PRED		= [[self valueForKey:@"FLAG_AC_PRED"] boolValue];
	outParams->FLAG_CBP_RD		= [[self valueForKey:@"FLAG_CBP_RD"] boolValue];
	outParams->FLAG_QP_RD		= [[self valueForKey:@"FLAG_QP_RD"] boolValue];
	outParams->FLAG_CLOSED_GOP	= [[self valueForKey:@"FLAG_CLOSED_GOP"] boolValue];
	
	outParams->NATIVE_FPS		= [[self valueForKey:@"NATIVE_FPS"] charValue];
	outParams->MB_DECISION		= [[self valueForKey:@"MB_DECISION"] charValue];
	outParams->RC_QSQUISH		= [[self valueForKey:@"RC_QSQUISH"] charValue];
	outParams->MPEG_QUANT		= [[self valueForKey:@"MPEG_QUANT"] charValue];
	outParams->THREADS			= [[self valueForKey:@"THREADS"] charValue];
	outParams->GAMMA			= [[self valueForKey:@"GAMMA"] charValue];
	outParams->NCLC				= [[self valueForKey:@"NCLC"] charValue];
	outParams->ME_METHOD		= [[self valueForKey:@"ME_METHOD"] charValue];
	
	outParams->SC_THRESHOLD		= [[self valueForKey:@"SC_THRESHOLD"] intValue];
	outParams->QCOMPRESS		= (int)([[self valueForKey:@"QCOMPRESS"] floatValue] * 100.0 + 0.1);	// !!!!!!!!!!!!!!!
	outParams->NOISE_REDUCTION	= [[self valueForKey:@"NOISE_REDUCTION"] intValue];
	outParams->RC_MAXRATE		= [[self valueForKey:@"RC_MAXRATE"] intValue];
	
	outParams->REFS				= [[self valueForKey:@"REFS"] charValue];
	outParams->ME_RANGE			= [[self valueForKey:@"ME_RANGE"] charValue];
	outParams->MAX_BFRAMES		= [[self valueForKey:@"MAX_BFRAMES"] charValue];
	outParams->CODER_TYPE		= [[self valueForKey:@"CODER_TYPE"] charValue];
	outParams->DIRECTPRED		= [[self valueForKey:@"DIRECTPRED"] charValue];
	outParams->CRF				= [[self valueForKey:@"CRF"] charValue];
	outParams->ADAPTIVE_BFRAME	= [[self valueForKey:@"ADAPTIVE_BFRAME"] charValue];
	outParams->ME_SUBQ			= [[self valueForKey:@"ME_SUBQ"] charValue];
	
	outParams->TRELLIS			= [[self valueForKey:@"TRELLIS"] charValue];
	outParams->TURBO			= [[self valueForKey:@"TURBO"] charValue];
	outParams->CHROMA_ME		= [[self valueForKey:@"CHROMA_ME"] charValue];
	outParams->PART_4X4			= [[self valueForKey:@"PART_4X4"] charValue];
	outParams->BIDIR_ME			= [[self valueForKey:@"BIDIR_ME"] charValue];
	outParams->USE3RDPASS		= [[self valueForKey:@"USE3RDPASS"] charValue];
	outParams->LEVEL			= [[self valueForKey:@"LEVEL"] charValue];
	outParams->EMBEDUUID		= [[self valueForKey:@"EMBEDUUID"] charValue];
	
	outParams->FLAG2_BPYRAMID	= [[self valueForKey:@"FLAG2_BPYRAMID"] boolValue];
	outParams->FLAG2_WPRED		= [[self valueForKey:@"FLAG2_WPRED"] boolValue];
	outParams->FLAG2_MIXED_REFS	= [[self valueForKey:@"FLAG2_MIXED_REFS"] boolValue];
	outParams->FLAG2_8X8DCT		= [[self valueForKey:@"FLAG2_8X8DCT"] boolValue];
	outParams->FLAG2_FASTPSKIP	= [[self valueForKey:@"FLAG2_FASTPSKIP"] boolValue];
	outParams->FLAG2_AUD		= [[self valueForKey:@"FLAG2_AUD"] boolValue];
	outParams->FLAG2_BRDO		= [[self valueForKey:@"FLAG2_BRDO"] boolValue];
	outParams->FLAG2_MBTREE		= [[self valueForKey:@"FLAG2_MBTREE"] boolValue];
	outParams->FLAG2_PSY		= [[self valueForKey:@"FLAG2_PSY"] boolValue];
	outParams->FLAG2_SSIM		= [[self valueForKey:@"FLAG2_SSIM"] boolValue];
	
	outParams->RC_BUFSIZE		= [[self valueForKey:@"RC_BUFSIZE"] intValue];
	outParams->AQ_STRENGTH		= (int)([[self valueForKey:@"AQ_STRENGTH"] floatValue] * 100.0 + 0.1);	// !!!!!!!!!!!!!!!
	outParams->PSYRD_RDO		= (int)([[self valueForKey:@"PSYRD_RDO"] floatValue] * 10.0 + 0.1);	// !!!!!!!!!!!!!!!
	outParams->PSYRD_TRELLIS	= (int)([[self valueForKey:@"PSYRD_TRELLIS"] floatValue] * 10.0 + 0.1);	// !!!!!!!!!!!!!!!
	
	outParams->DEBLOCK_ALPHA	= [[self valueForKey:@"DEBLOCK_ALPHA"] charValue];
	outParams->DEBLOCK_BETA		= [[self valueForKey:@"DEBLOCK_BETA"] charValue];
	outParams->AQ_MODE			= [[self valueForKey:@"AQ_MODE"] charValue];
	outParams->LUMI_MASKING		= [[self valueForKey:@"LUMI_MASKING"] charValue];
	
	outParams->KEYINT_MIN		= [[self valueForKey:@"KEYINT_MIN"] charValue];
	outParams->CQM_PRESET		= [[self valueForKey:@"CQM_PRESET"] charValue];
	outParams->NO_DCT_DECIMATE	= [[self valueForKey:@"NO_DCT_DECIMATE"] charValue];
	outParams->MAX_QDIFF		= [[self valueForKey:@"MAX_QDIFF"] charValue];
	
	outParams->CHROMAOFFSET		= [[self valueForKey:@"CHROMAOFFSET"] charValue];
	outParams->FILTERPRESET		= [[self valueForKey:@"FILTERPRESET"] charValue];
	outParams->SUBSAMPLING		= [[self valueForKey:@"SUBSAMPLING"] charValue];
	outParams->X264PROFILE		= [[self valueForKey:@"X264PROFILE"] charValue];
	
	outParams->X264PRESET		= [[self valueForKey:@"X264PRESET"] charValue];
	outParams->X264TUNE			= [[self valueForKey:@"X264TUNE"] charValue];
//	outParams->MBTREE			= [[self valueForKey:@"MBTREE"] charValue];
//	outParams->PSY				= [[self valueForKey:@"PSY"] charValue];
	
	outParams->RC_LOOKAHEAD		= [[self valueForKey:@"RC_LOOKAHEAD"] intValue];
	outParams->IP_FACTOR		= (int)([[self valueForKey:@"IP_FACTOR"] floatValue] * 100.0 + 0.1);	// !!!!!!!!!!!!!!!
	outParams->PB_FACTOR		= (int)([[self valueForKey:@"PB_FACTOR"] floatValue] * 100.0 + 0.1);	// !!!!!!!!!!!!!!!
	
	outParams->WEIGHTP			= [[self valueForKey:@"WEIGHTP"] charValue];
	outParams->TOPFIELDFIRST	= [[self valueForKey:@"TOPFIELDFIRST"] charValue];
	outParams->LOSSLESS			= [[self valueForKey:@"LOSSLESS"] charValue];
	outParams->BD_TUNE			= [[self valueForKey:@"BD_TUNE"] charValue];
	
	outParams->USEASPECTRATIO	= [[self valueForKey:@"USEASPECTRATIO"] charValue];
	outParams->USECLEANAPERTURE	= [[self valueForKey:@"USECLEANAPERTURE"] charValue];
	outParams->TAGASPECTRATIO	= [[self valueForKey:@"TAGASPECTRATIO"] charValue];
	outParams->TAGCLEANAPERTURE	= [[self valueForKey:@"TAGCLEANAPERTURE"] charValue];
	
	outParams->HSPACING	= [[self valueForKey:@"HSPACING"] intValue];
	outParams->VSPACING	= [[self valueForKey:@"VSPACING"] intValue];
	outParams->CLEANAPERTUREWIDTHN	= [[self valueForKey:@"CLEANAPERTUREWIDTHN"] intValue];
	outParams->CLEANAPERTUREWIDTHD	= [[self valueForKey:@"CLEANAPERTUREWIDTHD"] intValue];
	outParams->CLEANAPERTUREHEIGHTN	= [[self valueForKey:@"CLEANAPERTUREHEIGHTN"] intValue];
	outParams->CLEANAPERTUREHEIGHTD	= [[self valueForKey:@"CLEANAPERTUREHEIGHTD"] intValue];
	outParams->HORIZOFFN	= [[self valueForKey:@"HORIZOFFN"] intValue];
	outParams->HORIZOFFD	= [[self valueForKey:@"HORIZOFFD"] intValue];
	outParams->VERTOFFN	= [[self valueForKey:@"VERTOFFN"] intValue];
	outParams->VERTOFFD	= [[self valueForKey:@"VERTOFFD"] intValue];
	
	outParams->OVERRIDECRFQSCALE	= [[self valueForKey:@"OVERRIDECRFQSCALE"] charValue];
	outParams->USERCRFQSCALE	= [[self valueForKey:@"USERCRFQSCALE"] charValue];
	outParams->OVERRIDEQMIN		= [[self valueForKey:@"OVERRIDEQMIN"] charValue];
	outParams->USERQMIN			= [[self valueForKey:@"USERQMIN"] charValue];
	
	outParams->FAKEINTERLACED	= [[self valueForKey:@"FAKEINTERLACED"] charValue];
	return outParams;
}

#pragma mark -

- (void)resetToDefaultKeeping:(BOOL)keepIt
{
	if(!fourCC) [self setValue:@"avc1" forKey:@"fourCC"];
	
	// 
	if(!keepIt) {
		[self setValue:UIFalse		forKey:@"LOG_INFO"];		// OFF
		[self setValue:UIFalse		forKey:@"LOG_DEBUG"];		// OFF
		[self setValue:UIFalse		forKey:@"LOG_STATS"];		// OFF
		
		[self setValue:UIFalse		forKey:@"FLAG_PSNR"];		// OFF
		
		[self setValue:UIOne		forKey:@"NATIVE_FPS"];		// 1:NTSC
		[self setValue:UIZero		forKey:@"GAMMA"];			// 0:NO gamma
		[self setValue:UIOne		forKey:@"NCLC"];			// 1:NO nclc
		
		[self setValue:UIZero		forKey:@"USE3RDPASS"];		// 0:OFF
//		[self setValue:UIZero		forKey:@"EMBEDUUID"];		// 0:OFF
		
		[self setValue:UIZero		forKey:@"FILTERPRESET"];	// 0
		[self setValue:UIZero		forKey:@"SUBSAMPLING"];		// 0
		
		[self setValue:UIZero		forKey:@"USEASPECTRATIO"];		// 0
		[self setValue:UIZero		forKey:@"USECLEANAPERTURE"];	// 0
		[self setValue:UIZero		forKey:@"TAGASPECTRATIO"];		// 0
		[self setValue:UIZero		forKey:@"TAGCLEANAPERTURE"];	// 0
		
		[self setValue:UIOne		forKey:@"HSPACING"];		// 1
		[self setValue:UIOne		forKey:@"VSPACING"];		// 1
		[self setValue:UIZero		forKey:@"CLEANAPERTUREWIDTHN"];		// 0
		[self setValue:UIOne		forKey:@"CLEANAPERTUREWIDTHD"];		// 1
		[self setValue:UIZero		forKey:@"CLEANAPERTUREHEIGHTN"];	// 0
		[self setValue:UIOne		forKey:@"CLEANAPERTUREHEIGHTD"];	// 1
		[self setValue:UIZero		forKey:@"HORIZOFFN"];		// 0
		[self setValue:UIOne		forKey:@"HORIZOFFD"];		// 1
		[self setValue:UIZero		forKey:@"VERTOFFN"];		// 0
		[self setValue:UIOne		forKey:@"VERTOFFD"];		// 1
		
		// These params are considered as "behavior" params.
		if([self isAvc1]) {
			[self setValue:UIZero		forKey:@"OVERRIDECRFQSCALE"];	// OFF
			[self setValue:[UIChar:23]	forKey:@"USERCRFQSCALE"];	// 23
			[self setValue:UIOne		forKey:@"OVERRIDEQMIN"];	// ON
			[self setValue:[UIChar:0]	forKey:@"USERQMIN"];		// 0	// x264 r1795 changed default qmin from 10 to 0
		} else {
			[self setValue:UIZero		forKey:@"OVERRIDECRFQSCALE"];	// OFF
			[self setValue:[UIChar:2]	forKey:@"USERCRFQSCALE"];	// 2
			[self setValue:UIZero		forKey:@"OVERRIDEQMIN"];	// OFF
			[self setValue:[UIChar:2]	forKey:@"USERQMIN"];		// 2
		}
	}

	// Initial values for CODEC_ID_H264 (=x264)
	if([self isAvc1]) {
		[self setValue:UIFalse		forKey:@"FLAG_QSCALE"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG_4MV"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG_QPEL"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG_GMC"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG_MV0"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG_PART"];		// OFF
		[self setValue:UITrue		forKey:@"FLAG_LOOP_FILTER"];	// ON
//		[self setValue:UIFalse		forKey:@"FLAG_PSNR"];			// OFF
		[self setValue:UIFalse		forKey:@"FLAG_NORMALIZE_AQP"];	// OFF
		[self setValue:UIFalse		forKey:@"FLAG_INTERLACED_DCT"];	// OFF
		[self setValue:UIFalse		forKey:@"FLAG_AC_PRED"];	// OFF
		[self setValue:UIFalse		forKey:@"FLAG_CBP_RD"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG_QP_RD"];		// OFF
		[self setValue:UITrue		forKey:@"FLAG_CLOSED_GOP"];	// ON
		
//		[self setValue:UIOne		forKey:@"NATIVE_FPS"];		// 1:NTSC
		[self setValue:UIOne		forKey:@"MB_DECISION"];		// 1:SIMPLE
		[self setValue:UIZero		forKey:@"RC_QSQUISH"];		// 0:Clip
		[self setValue:UIZero		forKey:@"MPEG_QUANT"];		// 0:h263
		[self setValue:[UIChar:9]	forKey:@"THREADS"];			// 9:auto
//		[self setValue:UIZero		forKey:@"GAMMA"];			// 0:NO gamma
//		[self setValue:UIOne		forKey:@"NCLC"];			// 1:NO nclc
		[self setValue:UITwo		forKey:@"ME_METHOD"];		// 2:HEX
		
		[self setValue:[UIInt:40]	forKey:@"SC_THRESHOLD"];	// 40
		[self setValue:[UIFloat:0.6]	forKey:@"QCOMPRESS"];	// 0.60 !!!!!!!!!!!!!!!!!!!!!
		[self setValue:UIZero		forKey:@"NOISE_REDUCTION"];	// 0
		[self setValue:UIZero		forKey:@"RC_MAXRATE"];		// 0 Kbps
		
		[self setValue:[UIChar:3]	forKey:@"REFS"];			// 3
		[self setValue:[UIChar:16]	forKey:@"ME_RANGE"];		// 16
		[self setValue:[UIChar:3]	forKey:@"MAX_BFRAMES"];		// 3
		[self setValue:UITwo		forKey:@"CODER_TYPE"];		// 2:CABAC
		[self setValue:UITwo		forKey:@"DIRECTPRED"];		// 2:Spacial
		[self setValue:UIZero		forKey:@"CRF"];				// 0:No crf
		[self setValue:UIOne		forKey:@"ADAPTIVE_BFRAME"];	// 1:Use Adaptive B-frame - fast
		[self setValue:[UIChar:7]	forKey:@"ME_SUBQ"];			// 7
		
		[self setValue:UITwo		forKey:@"TRELLIS"];			// 2:FinalOnly
		[self setValue:[UIChar:3]	forKey:@"TURBO"];			// 3:Turbo 2
		[self setValue:UIOne		forKey:@"CHROMA_ME"];		// 1:ON
		[self setValue:UIZero		forKey:@"PART_4X4"];		// 0:OFF
		[self setValue:UIZero		forKey:@"BIDIR_ME"];		// 0:OFF
//		[self setValue:UIZero		forKey:@"USE3RDPASS"];		// 0:OFF
		[self setValue:[UIChar:9]	forKey:@"LEVEL"];			// 9:Level auto
		[self setValue:UIZero		forKey:@"EMBEDUUID"];		// 0:OFF
		
		[self setValue:UITrue		forKey:@"FLAG2_BPYRAMID"];	// ON
		[self setValue:UITrue		forKey:@"FLAG2_WPRED"];		// ON
		[self setValue:UITrue		forKey:@"FLAG2_MIXED_REFS"];	// ON
		[self setValue:UITrue		forKey:@"FLAG2_8X8DCT"];	// ON
		[self setValue:UITrue		forKey:@"FLAG2_FASTPSKIP"];	// ON
		[self setValue:UIFalse		forKey:@"FLAG2_AUD"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_BRDO"];		// OFF
		[self setValue:UITrue		forKey:@"FLAG2_MBTREE"];	// ON
		[self setValue:UITrue		forKey:@"FLAG2_PSY"];		// ON
		[self setValue:UIFalse		forKey:@"FLAG2_SSIM"];		// OFF
		
		[self setValue:UIZero		forKey:@"RC_BUFSIZE"];		// 0 Kbit
		[self setValue:[UIFloat:1.0]	forKey:@"AQ_STRENGTH"];	// 1.00 !!!!!!!!!!!!!!!!!!!!!
		[self setValue:[UIFloat:1.0]	forKey:@"PSYRD_RDO"];	// 1.0 !!!!!!!!!!!!!!!!!!!!!
		[self setValue:[UIFloat:0.0]	forKey:@"PSYRD_TRELLIS"];	// 0.0 !!!!!!!!!!!!!!!!!!!!!

		[self setValue:UIZero		forKey:@"DEBLOCK_ALPHA"];	// 0
		[self setValue:UIZero		forKey:@"DEBLOCK_BETA"];	// 0
		[self setValue:[UIChar:2]	forKey:@"AQ_MODE"];			// 2:VARIANCE
		[self setValue:[UIChar:0]	forKey:@"LUMI_MASKING"];	// 0:OFF

		[self setValue:[UIChar:0]	forKey:@"KEYINT_MIN"];	// 0:auto
		[self setValue:UIZero		forKey:@"CQM_PRESET"];	// 0
		[self setValue:UIZero		forKey:@"NO_DCT_DECIMATE"];	// 0
		[self setValue:[UIChar:4]	forKey:@"MAX_QDIFF"];		// 4
		
		[self setValue:UIZero		forKey:@"CHROMAOFFSET"];	// 0
//		[self setValue:UIZero		forKey:@"FILTERPRESET"];	// 0
//		[self setValue:UIZero		forKey:@"SUBSAMPLING"];		// 0
		[self setValue:[UIChar:X264PROFILE_HIGH]	forKey:@"X264PROFILE"];		// X264PROFILE_HIGH
		
		[self setValue:UIZero		forKey:@"X264PRESET"];		// 0
		[self setValue:UIZero		forKey:@"X264TUNE"];		// 0
//		[self setValue:UITrue		forKey:@"MBTREE"];			// ON
//		[self setValue:UITrue		forKey:@"PSY"];				// ON
		
		[self setValue:[UIInt:40]	forKey:@"RC_LOOKAHEAD"];	// 40
		[self setValue:[UIFloat:1.4]	forKey:@"IP_FACTOR"];	// 1.4 !!!!!!!!!!!!!!!!!!!!!
		[self setValue:[UIFloat:1.3]	forKey:@"PB_FACTOR"];	// 1.3 !!!!!!!!!!!!!!!!!!!!!
		
		[self setValue:[UIChar:3]	forKey:@"WEIGHTP"];			// 3:Weighted refs + Duplicates
		[self setValue:UIZero		forKey:@"TOPFIELDFIRST"];	// 0:Progressive or bottom field first
		[self setValue:UIZero		forKey:@"LOSSLESS"];		// 0
		[self setValue:UIZero		forKey:@"BD_TUNE"];			// OFF
		
		[self setValue:UIZero		forKey:@"FAKEINTERLACED"];	// OFF
	}
	
	// Initial values for CODEC_ID_MPEG4
	if([self isMp4v]) {
		[self setValue:UIFalse		forKey:@"FLAG_QSCALE"];		// OFF
		[self setValue:UITrue		forKey:@"FLAG_4MV"];		// ON
		[self setValue:UIFalse		forKey:@"FLAG_QPEL"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG_GMC"];		// OFF
		[self setValue:UITrue		forKey:@"FLAG_MV0"];		// ON
		[self setValue:UIFalse		forKey:@"FLAG_PART"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG_LOOP_FILTER"];	// OFF
//		[self setValue:UIFalse		forKey:@"FLAG_PSNR"];			// OFF
		[self setValue:UIFalse		forKey:@"FLAG_NORMALIZE_AQP"];	// OFF
		[self setValue:UIFalse		forKey:@"FLAG_INTERLACED_DCT"];	// OFF
		[self setValue:UIFalse		forKey:@"FLAG_AC_PRED"];	// OFF
		[self setValue:UIFalse		forKey:@"FLAG_CBP_RD"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG_QP_RD"];		// OFF
		[self setValue:UITrue		forKey:@"FLAG_CLOSED_GOP"];	// ON
		
//		[self setValue:UIOne		forKey:@"NATIVE_FPS"];		// 1:NTSC
		[self setValue:UITwo		forKey:@"MB_DECISION"];		// 2:BITS
		[self setValue:UIOne		forKey:@"RC_QSQUISH"];		// 1:No Clip
		[self setValue:UIZero		forKey:@"MPEG_QUANT"];		// 0:h263
		[self setValue:UITwo		forKey:@"THREADS"];			// 2:dual
//		[self setValue:UIZero		forKey:@"GAMMA"];			// 0:NO gamma
//		[self setValue:UIOne		forKey:@"NCLC"];			// 1:NO nclc
		[self setValue:UIOne		forKey:@"ME_METHOD"];		// 1:DIA(EPZS)
		
		[self setValue:UIZero		forKey:@"SC_THRESHOLD"];	// 0
		[self setValue:[UIFloat:0.6]	forKey:@"QCOMPRESS"];	// 0.60 !!!!!!!!!!!!!!!!!!!!!
		[self setValue:UIZero		forKey:@"NOISE_REDUCTION"];	// 0
		[self setValue:UIZero		forKey:@"RC_MAXRATE"];		// 0 Kbps
		
		[self setValue:UIOne		forKey:@"REFS"];			// 1
		[self setValue:[UIChar:16]	forKey:@"ME_RANGE"];		// 16
		[self setValue:UIOne		forKey:@"MAX_BFRAMES"];		// 1
		[self setValue:UIOne		forKey:@"CODER_TYPE"];		// 1:CAVLC
		[self setValue:[UIChar:3]	forKey:@"DIRECTPRED"];		// 3:Temporal
		[self setValue:UIZero		forKey:@"CRF"];				// 0:No crf
		[self setValue:UIZero		forKey:@"ADAPTIVE_BFRAME"];	// 0:No Adaptive B-frame
		[self setValue:[UIChar:8]	forKey:@"ME_SUBQ"];			// 8
		
		[self setValue:UIOne		forKey:@"TRELLIS"];			// 1:Disabled
		[self setValue:[UIChar:2]	forKey:@"TURBO"];			// 2:Turbo 1
		[self setValue:UIZero		forKey:@"CHROMA_ME"];		// 0:OFF
		[self setValue:UIZero		forKey:@"PART_4X4"];		// 0:OFF
		[self setValue:UIZero		forKey:@"BIDIR_ME"];		// 0:OFF
//		[self setValue:UIZero		forKey:@"USE3RDPASS"];		// 0:OFF
		[self setValue:[UIChar:9]	forKey:@"LEVEL"];			// 9:Level auto
		[self setValue:UIZero		forKey:@"EMBEDUUID"];		// 0:OFF
		
		[self setValue:UIFalse		forKey:@"FLAG2_BPYRAMID"];	// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_WPRED"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_MIXED_REFS"];	// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_8X8DCT"];	// OFF
		[self setValue:UITrue		forKey:@"FLAG2_FASTPSKIP"];	// ON
		[self setValue:UIFalse		forKey:@"FLAG2_AUD"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_BRDO"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_MBTREE"];	// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_PSY"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_SSIM"];		// OFF
		
		[self setValue:UIZero		forKey:@"RC_BUFSIZE"];		// 0 Kbit
		[self setValue:[UIFloat:1.0]	forKey:@"AQ_STRENGTH"];	// 1.00 !!!!!!!!!!!!!!!!!!!!!
		[self setValue:[UIFloat:1.0]	forKey:@"PSYRD_RDO"];	// 1.0 !!!!!!!!!!!!!!!!!!!!!
		[self setValue:[UIFloat:0.0]	forKey:@"PSYRD_TRELLIS"];	// 0.0 !!!!!!!!!!!!!!!!!!!!!

		[self setValue:UIZero		forKey:@"DEBLOCK_ALPHA"];	// 0
		[self setValue:UIZero		forKey:@"DEBLOCK_BETA"];	// 0
		[self setValue:[UIChar:2]	forKey:@"AQ_MODE"];			// 2:VARIANCE
		[self setValue:[UIChar:0]	forKey:@"LUMI_MASKING"];	// 0:OFF

		[self setValue:[UIChar:25]	forKey:@"KEYINT_MIN"];		// 25
		[self setValue:UIZero		forKey:@"CQM_PRESET"];		// 0
		[self setValue:UIZero		forKey:@"NO_DCT_DECIMATE"];	// 0
		[self setValue:[UIChar:3]	forKey:@"MAX_QDIFF"];		// 3
		
		[self setValue:UIZero		forKey:@"CHROMAOFFSET"];	// 0
//		[self setValue:UIZero		forKey:@"FILTERPRESET"];	// 0
//		[self setValue:UIZero		forKey:@"SUBSAMPLING"];		// 0
		[self setValue:[UIChar:X264PROFILE_HIGH]	forKey:@"X264PROFILE"];		// X264PROFILE_HIGH
		
		[self setValue:UIZero		forKey:@"X264PRESET"];		// 0
		[self setValue:UIZero		forKey:@"X264TUNE"];		// 0
//		[self setValue:UIFalse		forKey:@"MBTREE"];			// OFF
//		[self setValue:UIFalse		forKey:@"PSY"];				// OFF
		
		[self setValue:[UIInt:0]	forKey:@"RC_LOOKAHEAD"];	// 0
		[self setValue:[UIFloat:1.25]	forKey:@"IP_FACTOR"];	// 1.25 !!!!!!!!!!!!!!!!!!!!!
		[self setValue:[UIFloat:1.25]	forKey:@"PB_FACTOR"];	// 1.25 !!!!!!!!!!!!!!!!!!!!!
		
		[self setValue:UIOne		forKey:@"WEIGHTP"];			// 1:Disabled
		[self setValue:UIZero		forKey:@"TOPFIELDFIRST"];	// 0:Progressive or bottom field first
		[self setValue:UIZero		forKey:@"LOSSLESS"];		// 0
		[self setValue:UIZero		forKey:@"BD_TUNE"];			// OFF
		
		[self setValue:UIZero		forKey:@"FAKEINTERLACED"];	// OFF
	}
	
	// Initial values for CODEC_ID_XVID
	if([self isXvid]) {
		[self setValue:UIFalse		forKey:@"FLAG_QSCALE"];		// OFF
		[self setValue:UITrue		forKey:@"FLAG_4MV"];		// ON
		[self setValue:UIFalse		forKey:@"FLAG_QPEL"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG_GMC"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG_MV0"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG_PART"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG_LOOP_FILTER"];	// OFF
//		[self setValue:UIFalse		forKey:@"FLAG_PSNR"];			// OFF
		[self setValue:UIFalse		forKey:@"FLAG_NORMALIZE_AQP"];	// OFF
		[self setValue:UIFalse		forKey:@"FLAG_INTERLACED_DCT"];	// OFF
		[self setValue:UITrue		forKey:@"FLAG_AC_PRED"];	// ON
		[self setValue:UIFalse		forKey:@"FLAG_CBP_RD"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG_QP_RD"];		// OFF
		[self setValue:UITrue		forKey:@"FLAG_CLOSED_GOP"];	// ON
		
//		[self setValue:UIOne		forKey:@"NATIVE_FPS"];		// 1:NTSC
		[self setValue:UIOne		forKey:@"MB_DECISION"];		// 1:SIMPLE
		[self setValue:UIZero		forKey:@"RC_QSQUISH"];		// 0:Clip
		[self setValue:UIZero		forKey:@"MPEG_QUANT"];		// 0:h263
		[self setValue:UITwo		forKey:@"THREADS"];			// 2:dual
//		[self setValue:UIZero		forKey:@"GAMMA"];			// 0:NO gamma
//		[self setValue:UIOne		forKey:@"NCLC"];			// 1:NO nclc
		[self setValue:[UIInt:4]	forKey:@"ME_METHOD"];		// 4:FULL
		
		[self setValue:UIZero		forKey:@"SC_THRESHOLD"];	// 0
		[self setValue:[UIFloat:0.6]	forKey:@"QCOMPRESS"];	// 0.60 !!!!!!!!!!!!!!!!!!!!!
		[self setValue:UIZero		forKey:@"NOISE_REDUCTION"];	// 0
		[self setValue:UIZero		forKey:@"RC_MAXRATE"];		// 0 Kbps
		
		[self setValue:UIOne		forKey:@"REFS"];			// 1
		[self setValue:[UIChar:16]	forKey:@"ME_RANGE"];		// 16
		[self setValue:UIOne		forKey:@"MAX_BFRAMES"];		// 1
		[self setValue:UIOne		forKey:@"CODER_TYPE"];		// 1:CAVLC
		[self setValue:[UIChar:3]	forKey:@"DIRECTPRED"];		// 3:Temporal
		[self setValue:UIZero		forKey:@"CRF"];				// 0:No crf
		[self setValue:UIZero		forKey:@"ADAPTIVE_BFRAME"];	// 0:No Adaptive B-frame
		[self setValue:[UIChar:8]	forKey:@"ME_SUBQ"];			// 8
		
		[self setValue:[UIChar:3]	forKey:@"TRELLIS"];			// 3:All
		[self setValue:UIOne		forKey:@"TURBO"];			// 1:Disabled
		[self setValue:UIOne		forKey:@"CHROMA_ME"];		// 1:ON
		[self setValue:UIZero		forKey:@"PART_4X4"];		// 0:OFF
		[self setValue:UIZero		forKey:@"BIDIR_ME"];		// 0:OFF
//		[self setValue:UIZero		forKey:@"USE3RDPASS"];		// 0:OFF
		[self setValue:[UIChar:9]	forKey:@"LEVEL"];			// 9:Level auto
		[self setValue:UIZero		forKey:@"EMBEDUUID"];		// 0:OFF
		
		[self setValue:UIFalse		forKey:@"FLAG2_BPYRAMID"];	// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_WPRED"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_MIXED_REFS"];	// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_8X8DCT"];	// OFF
		[self setValue:UITrue		forKey:@"FLAG2_FASTPSKIP"];	// ON
		[self setValue:UIFalse		forKey:@"FLAG2_AUD"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_BRDO"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_MBTREE"];	// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_PSY"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_SSIM"];		// OFF
		
		[self setValue:UIZero		forKey:@"RC_BUFSIZE"];		// 0 Kbit
		[self setValue:[UIFloat:1.0]	forKey:@"AQ_STRENGTH"];	// 1.00 !!!!!!!!!!!!!!!!!!!!!
		[self setValue:[UIFloat:1.0]	forKey:@"PSYRD_RDO"];	// 1.0 !!!!!!!!!!!!!!!!!!!!!
		[self setValue:[UIFloat:0.0]	forKey:@"PSYRD_TRELLIS"];	// 0.0 !!!!!!!!!!!!!!!!!!!!!

		[self setValue:UIZero		forKey:@"DEBLOCK_ALPHA"];	// 0
		[self setValue:UIZero		forKey:@"DEBLOCK_BETA"];	// 0
		[self setValue:[UIChar:2]	forKey:@"AQ_MODE"];			// 2:VARIANCE
		[self setValue:[UIChar:0]	forKey:@"LUMI_MASKING"];	// 0:OFF

		[self setValue:[UIChar:25]	forKey:@"KEYINT_MIN"];	// 25
		[self setValue:UIZero		forKey:@"CQM_PRESET"];	// 0
		[self setValue:UIZero		forKey:@"NO_DCT_DECIMATE"];	// 0
		[self setValue:[UIChar:3]	forKey:@"MAX_QDIFF"];		// 3
		
		[self setValue:UIZero		forKey:@"CHROMAOFFSET"];	// 0
//		[self setValue:UIZero		forKey:@"FILTERPRESET"];	// 0
//		[self setValue:UIZero		forKey:@"SUBSAMPLING"];		// 0
		[self setValue:[UIChar:X264PROFILE_HIGH]	forKey:@"X264PROFILE"];		// X264PROFILE_HIGH
		
		[self setValue:UIZero		forKey:@"X264PRESET"];		// 0
		[self setValue:UIZero		forKey:@"X264TUNE"];		// 0
//		[self setValue:UIFalse		forKey:@"MBTREE"];			// OFF
//		[self setValue:UIFalse		forKey:@"PSY"];				// OFF
		
		[self setValue:[UIInt:0]	forKey:@"RC_LOOKAHEAD"];	// 0
		[self setValue:[UIFloat:1.25]	forKey:@"IP_FACTOR"];	// 1.25 !!!!!!!!!!!!!!!!!!!!!
		[self setValue:[UIFloat:1.5]	forKey:@"PB_FACTOR"];	// 1.5 !!!!!!!!!!!!!!!!!!!!!
		
		[self setValue:UIOne		forKey:@"WEIGHTP"];			// 1:Disabled
		[self setValue:UIZero		forKey:@"TOPFIELDFIRST"];	// 0:Progressive or bottom field first
		[self setValue:UIZero		forKey:@"LOSSLESS"];		// 0
		[self setValue:UIZero		forKey:@"BD_TUNE"];			// OFF
		
		[self setValue:UIZero		forKey:@"FAKEINTERLACED"];	// OFF
	}
}

- (void)resetToiPodKeeping:(BOOL)keepIt
{
	[self resetToDefaultKeeping:keepIt];
	
	if([self isAvc1]) {
		[self setValue:[UIChar:1]	forKey:@"EMBEDUUID"];		// 1:ON
		[self setValue:[UIChar:1]	forKey:@"CODER_TYPE"];		// 1:CAVLC
		[self setValue:[UIChar:30]	forKey:@"LEVEL"];			// 3:3.0
		[self setValue:UIFalse		forKey:@"FLAG2_BPYRAMID"];	// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_WPRED"];		// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_MIXED_REFS"];	// OFF
		[self setValue:UIFalse		forKey:@"FLAG2_8X8DCT"];	// OFF
		
		[self setValue:UIOne		forKey:@"TRELLIS"];			// 1:Disabled
		[self setValue:[UIChar:X264PROFILE_BASE]	forKey:@"X264PROFILE"];		// 1:Baseline
		
		[self setValue:UIOne		forKey:@"REFS"];			// 1
		[self setValue:[UIChar:6]	forKey:@"ME_SUBQ"];			// 6
		[self setValue:[UIInt:1]	forKey:@"MAX_BFRAMES"];		// 1	/* No B-frame for iPod */
		
		[self setValue:UIOne		forKey:@"WEIGHTP"];			// 1:Disabled
	}
	[self setValue:[UIInt:10000]	forKey:@"RC_MAXRATE"];		// 10000Kbps; same as Level 3.0 max rate
	[self setValue:[UIInt:2048]		forKey:@"RC_BUFSIZE"];		// 2048Kbps
}

- (void)resetToDefaultTunedKeeping:(BOOL)keepIt
{
	[self resetToDefaultKeeping:keepIt];
	
	if([self isAvc1]) {
		[self setValue:UIFalse		forKey:@"FLAG2_FASTPSKIP"];	// OFF
		[self setValue:UIOne		forKey:@"NO_DCT_DECIMATE"];	// 1:ON
		
		[self setValue:[UIInt:3]	forKey:@"ME_METHOD"];		// 3:UMH
		[self setValue:[UIChar:4]	forKey:@"DIRECTPRED"];		// 4:Auto
//		[self setValue:[UIInt:3]	forKey:@"TRELLIS"];			// 3:All
		[self setValue:UITwo		forKey:@"ADAPTIVE_BFRAME"];	// 2:Use Adaptive B-frame - Trellis
		
		[self setValue:[UIChar:8]	forKey:@"ME_SUBQ"];			// 8
		[self setValue:[UIInt:80]	forKey:@"SC_THRESHOLD"];	// 80
		[self setValue:[UIChar:1]	forKey:@"KEYINT_MIN"];		// 1
	}
	if([self isMp4v]) {
//		[self setValue:UITrue		forKey:@"FLAG_QPEL"];		// ON
		[self setValue:UITrue		forKey:@"FLAG_CBP_RD"];		// ON
//		[self setValue:UITrue		forKey:@"FLAG_QP_RD"];		// ON
//		[self setValue:UIOne		forKey:@"BIDIR_ME"];		// 1:ON
//		[self setValue:UIOne		forKey:@"MPEG_QUANT"];		// 1:mpeg
		
		[self setValue:[UIInt:3]	forKey:@"MB_DECISION"];		// 3:RD
		[self setValue:[UIChar:3]	forKey:@"TRELLIS"];			// 3:All
		[self setValue:UIOne		forKey:@"ADAPTIVE_BFRAME"];	// 1:Use Adaptive B-frame - fast
		
		[self setValue:[UIInt:3]	forKey:@"MAX_BFRAMES"];		// 3
	}
	if([self isXvid]) {
//		[self setValue:UITrue		forKey:@"FLAG_QPEL"];		// ON
		[self setValue:UITrue		forKey:@"FLAG2_BRDO"];		// ON
//		[self setValue:UIOne		forKey:@"MPEG_QUANT"];		// 1:mpeg
		[self setValue:[UIChar:1]	forKey:@"LUMI_MASKING"];	// 1:ON
		
		[self setValue:[UIInt:3]	forKey:@"MB_DECISION"];		// 3:RD
		
		[self setValue:[UIInt:3]	forKey:@"MAX_BFRAMES"];		// 3
	}
}

- (void)resetToiPodTunedKeeping:(BOOL)keepIt
{
	[self resetToiPodKeeping:keepIt];
	
	if([self isAvc1]) {
		[self setValue:UITrue		forKey:@"FLAG2_MIXED_REFS"];	// ON
		[self setValue:UIFalse		forKey:@"FLAG2_FASTPSKIP"];	// OFF
		[self setValue:UIOne		forKey:@"NO_DCT_DECIMATE"];	// 1
		
		[self setValue:[UIInt:3]	forKey:@"ME_METHOD"];		// 3:UMH
		[self setValue:[UIChar:4]	forKey:@"DIRECTPRED"];		// 4:Auto
		
		[self setValue:[UIInt:3]	forKey:@"REFS"];			// 3
		[self setValue:[UIInt:80]	forKey:@"SC_THRESHOLD"];	// 80
		
		[self setValue:UIOne		forKey:@"WEIGHTP"];			// 1:Disabled
	}
	if([self isMp4v]) {
//		[self setValue:UITrue		forKey:@"FLAG_QPEL"];		// ON
		[self setValue:UITrue		forKey:@"FLAG_CBP_RD"];		// ON
//		[self setValue:UITrue		forKey:@"FLAG_QP_RD"];		// ON
		[self setValue:UIOne		forKey:@"BIDIR_ME"];		// 1:ON
		
		[self setValue:[UIInt:3]	forKey:@"MB_DECISION"];		// 3:RD
//		[self setValue:[UIChar:3]	forKey:@"TRELLIS"];			// 3:All
	}
	if([self isXvid]) {
//		[self setValue:UITrue		forKey:@"FLAG_QPEL"];		// ON
		[self setValue:[UIChar:1]	forKey:@"LUMI_MASKING"];	// 1:ON
		
		[self setValue:UITwo		forKey:@"MB_DECISION"];		// 2:BITS
	}
}

#pragma mark -

//- (void)resetToDefault
//{
//	BOOL keepIt = (GetCurrentKeyModifiers() & optionKey) ? TRUE : FALSE;
//	[self resetToDefaultKeeping:keepIt];
//}
//
//- (void)resetToiPod
//{
//	BOOL keepIt = (GetCurrentKeyModifiers() & optionKey) ? TRUE : FALSE;
//	[self resetToiPodKeeping:keepIt];
//}
//
//- (void)resetToDefaultTuned
//{
//	BOOL keepIt = (GetCurrentKeyModifiers() & optionKey) ? TRUE : FALSE;
//	[self resetToDefaultTunedKeeping:keepIt];
//}
//
//- (void)resetToiPodTuned
//{
//	BOOL keepIt = (GetCurrentKeyModifiers() & optionKey) ? TRUE : FALSE;
//	[self resetToiPodTunedKeeping:keepIt];
//}

- (void)resetTox264Preset
{
//	[self resetToDefault];	// hacked in UIController
	
	if([self isAvc1]) {
		switch ([X264PRESET charValue]) {
		case X264PRESET_ULTRAFAST:
			[self setValue:UIOne		forKey:@"REFS"];			// 1
			[self setValue:UIZero		forKey:@"SC_THRESHOLD"];	// 0
			[self setValue:UIFalse		forKey:@"FLAG_LOOP_FILTER"];	// OFF
			[self setValue:UIOne		forKey:@"CODER_TYPE"];		// 1:CAVLC
			[self setValue:UIZero		forKey:@"MAX_BFRAMES"];		// 0
			// param->analyse.intra = 0;							// patched libx264.c
			// param->analyse.inter = 0;							// patched libx264.c
			[self setValue:UIFalse		forKey:@"FLAG2_8X8DCT"];	// OFF
			[self setValue:UIOne		forKey:@"ME_METHOD"];		// 1:DIA(EPZS)
			[self setValue:UIZero		forKey:@"ME_SUBQ"];			// 0
			[self setValue:UIOne		forKey:@"AQ_MODE"];			// 1:NONE
			[self setValue:UIFalse		forKey:@"FLAG2_MIXED_REFS"];	// OFF
			[self setValue:UIOne		forKey:@"TRELLIS"];			// 1:Disabled
			[self setValue:UIZero		forKey:@"ADAPTIVE_BFRAME"];	// 0:No Adaptive B-frame
//			[self setValue:UIFalse		forKey:@"MBTREE"];			// OFF
			[self setValue:UIFalse		forKey:@"FLAG2_MBTREE"];	// OFF
			[self setValue:UIOne		forKey:@"WEIGHTP"];			// 1:Disabled
			[self setValue:UIFalse		forKey:@"FLAG2_WPRED"];		// OFF
			[self setValue:UIZero		forKey:@"RC_LOOKAHEAD"];	// 0
			break;
		case X264PRESET_SUPERFAST:
			// param->analyse.inter = X264_ANALYSE_I8x8|X264_ANALYSE_I4x4;	// patched libx264.c
			[self setValue:UIOne		forKey:@"ME_METHOD"];		// 1:DIA(EPZS)
			[self setValue:UIOne		forKey:@"ME_SUBQ"];			// 1
			[self setValue:UIOne		forKey:@"REFS"];			// 1
			[self setValue:UIFalse		forKey:@"FLAG2_MIXED_REFS"];	// OFF
			[self setValue:UIOne		forKey:@"TRELLIS"];			// 1:Disabled
//			[self setValue:UIFalse		forKey:@"MBTREE"];			// OFF
			[self setValue:UIFalse		forKey:@"FLAG2_MBTREE"];	// OFF
			[self setValue:UITwo		forKey:@"WEIGHTP"];			// 2:Weighted refs
			[self setValue:UIZero		forKey:@"RC_LOOKAHEAD"];	// 0
			break;
		case X264PRESET_VERYFAST:
			[self setValue:UITwo		forKey:@"ME_METHOD"];		// 2:HEX
			[self setValue:UITwo		forKey:@"ME_SUBQ"];			// 2
			[self setValue:UIOne		forKey:@"REFS"];			// 1
			[self setValue:UIFalse		forKey:@"FLAG2_MIXED_REFS"];	// OFF
			[self setValue:UIOne		forKey:@"TRELLIS"];			// 1:Disabled
//			[self setValue:UIFalse		forKey:@"MBTREE"];			// OFF
//			[self setValue:UIFalse		forKey:@"FLAG2_MBTREE"];	// OFF
			[self setValue:UITwo		forKey:@"WEIGHTP"];			// 2:Weighted refs
			[self setValue:[UIInt:10]	forKey:@"RC_LOOKAHEAD"];	// 10
			break;
		case X264PRESET_FASTER:
			[self setValue:UIFalse		forKey:@"FLAG2_MIXED_REFS"];	// OFF
			[self setValue:UITwo		forKey:@"REFS"];			// 2
			[self setValue:[UIChar:4]	forKey:@"ME_SUBQ"];			// 4
			[self setValue:UITwo		forKey:@"WEIGHTP"];			// 2:Weighted refs
			[self setValue:[UIInt:20]	forKey:@"RC_LOOKAHEAD"];	// 20
			break;
		case X264PRESET_FAST:
			[self setValue:[UIChar:2]	forKey:@"REFS"];			// 2
			[self setValue:[UIChar:6]	forKey:@"ME_SUBQ"];			// 6
			[self setValue:UITwo		forKey:@"WEIGHTP"];			// 2:Weighted refs
			[self setValue:[UIInt:30]	forKey:@"RC_LOOKAHEAD"];	// 30
			break;
		case X264PRESET_MEDIUM:
			/* Default is medium */
			break;
		case X264PRESET_SLOW:
			[self setValue:[UIChar:3]	forKey:@"ME_METHOD"];		// 3:UMH
			[self setValue:[UIChar:8]	forKey:@"ME_SUBQ"];			// 8
			[self setValue:[UIChar:5]	forKey:@"REFS"];			// 5
			[self setValue:UITwo		forKey:@"ADAPTIVE_BFRAME"];	// 2:Use Adaptive B-frame - Trellis
			[self setValue:[UIChar:4]	forKey:@"DIRECTPRED"];		// 4:Auto
			[self setValue:[UIInt:50]	forKey:@"RC_LOOKAHEAD"];	// 50
			break;
		case X264PRESET_SLOWER:
			[self setValue:[UIChar:3]	forKey:@"ME_METHOD"];		// 3:UMH
			[self setValue:[UIChar:9]	forKey:@"ME_SUBQ"];			// 9
			[self setValue:[UIChar:8]	forKey:@"REFS"];			// 8
			[self setValue:UITwo		forKey:@"ADAPTIVE_BFRAME"];	// 2:Use Adaptive B-frame - Trellis
			[self setValue:[UIChar:4]	forKey:@"DIRECTPRED"];		// 4:Auto
			[self setValue:UIOne		forKey:@"PART_4X4"];		// 1:ON
			[self setValue:[UIChar:3]	forKey:@"TRELLIS"];			// 3:All
			[self setValue:[UIInt:60]	forKey:@"RC_LOOKAHEAD"];	// 60
			break;
		case X264PRESET_VERYSLOW:
			[self setValue:[UIChar:3]	forKey:@"ME_METHOD"];		// 3:UMH
			[self setValue:[UIChar:10]	forKey:@"ME_SUBQ"];			// 10
			[self setValue:[UIChar:24]	forKey:@"ME_RANGE"];		// 24
			[self setValue:[UIChar:16]	forKey:@"REFS"];			// 16
			[self setValue:UITwo		forKey:@"ADAPTIVE_BFRAME"];	// 2:Use Adaptive B-frame - Trellis
			[self setValue:[UIChar:4]	forKey:@"DIRECTPRED"];		// 4:Auto
			[self setValue:UIOne		forKey:@"PART_4X4"];		// 1:ON
			[self setValue:[UIChar:3]	forKey:@"TRELLIS"];			// 3:All
			[self setValue:[UIChar:8]	forKey:@"MAX_BFRAMES"];		// 8
			[self setValue:[UIInt:60]	forKey:@"RC_LOOKAHEAD"];	// 60
			break;
		case X264PRESET_PLACEBO:
			[self setValue:[UIChar:6]	forKey:@"ME_METHOD"];		// 6:TESA
			[self setValue:[UIChar:10]	forKey:@"ME_SUBQ"];			// 10
			[self setValue:[UIChar:24]	forKey:@"ME_RANGE"];		// 24
			[self setValue:[UIChar:16]	forKey:@"REFS"];			// 16
			[self setValue:UITwo		forKey:@"ADAPTIVE_BFRAME"];	// 2:Use Adaptive B-frame - Trellis
			[self setValue:[UIChar:4]	forKey:@"DIRECTPRED"];		// 4:Auto
			[self setValue:UIOne		forKey:@"PART_4X4"];		// 1:ON
			[self setValue:UIFalse		forKey:@"FLAG2_FASTPSKIP"];	// OFF
			[self setValue:[UIChar:3]	forKey:@"TRELLIS"];			// 3:All
			[self setValue:[UIChar:16]	forKey:@"MAX_BFRAMES"];		// 16
			[self setValue:[UIInt:60]	forKey:@"RC_LOOKAHEAD"];	// 60
			
			[self setValue:UIOne		forKey:@"TURBO"];			// 1:Disabled
			break;
		default:
			break;
		}
		switch ([X264TUNE charValue]) {
		case X264TUNE_FILM:
			[self setValue:[UIChar:-1]		forKey:@"DEBLOCK_ALPHA"];	// -1
			[self setValue:[UIChar:-1]		forKey:@"DEBLOCK_BETA"];	// -1
			[self setValue:[UIFloat:0.1]	forKey:@"PSYRD_TRELLIS"];	// 0.1 instead of 0.15 !!!!!!!!!!!!!!!!!!!!!
			break;
		case X264TUNE_ANIMATION:
			if([REFS charValue]>1) 
				[self setValue:[UIChar:[REFS charValue]*2]	forKey:@"REFS"];	// Double REF value
			[self setValue:[UIChar:1]		forKey:@"DEBLOCK_ALPHA"];	// 1
			[self setValue:[UIChar:1]		forKey:@"DEBLOCK_BETA"];	// 1
			[self setValue:[UIFloat:0.4]	forKey:@"PSYRD_RDO"];		// 0.4
			[self setValue:[UIFloat:0.6]	forKey:@"AQ_STRENGTH"];		// 0.6
			[self setValue:[UIChar:[MAX_BFRAMES charValue]+2]	forKey:@"MAX_BFRAMES"];	// +2 MAX_BFRAMES
			break;
		case X264TUNE_GRAIN:
			[self setValue:[UIChar:-2]		forKey:@"DEBLOCK_ALPHA"];	// -2
			[self setValue:[UIChar:-2]		forKey:@"DEBLOCK_BETA"];	// -2
			[self setValue:[UIFloat:0.3]	forKey:@"PSYRD_TRELLIS"];	// 0.3 instead of 0.25 !!!!!!!!!!!!!!!!!!!!!
			[self setValue:UIOne			forKey:@"NO_DCT_DECIMATE"];	// 1
			[self setValue:[UIFloat:1.1]	forKey:@"PB_FACTOR"];		// 1.1
			[self setValue:[UIFloat:1.1]	forKey:@"IP_FACTOR"];		// 1.1
			[self setValue:[UIFloat:0.5]	forKey:@"AQ_STRENGTH"];		// 0.5
			// param->analyse.i_luma_deadzone[0] = 6;					// patched libx264.c
			// param->analyse.i_luma_deadzone[1] = 6;					// patched libx264.c
			[self setValue:[UIFloat:0.8]	forKey:@"QCOMPRESS"];		// 0.80
			break;
		case X264TUNE_STILLIMAGE:
			[self setValue:[UIChar:-3]		forKey:@"DEBLOCK_ALPHA"];	// -3
			[self setValue:[UIChar:-3]		forKey:@"DEBLOCK_BETA"];	// -3
			[self setValue:[UIFloat:2.0]	forKey:@"PSYRD_RDO"];		// 2.0
			[self setValue:[UIFloat:0.7]	forKey:@"PSYRD_TRELLIS"];	// 0.7
			[self setValue:[UIFloat:1.2]	forKey:@"AQ_STRENGTH"];		// 1.2
			break;
		case X264TUNE_PSNR:
			[self setValue:UIOne			forKey:@"AQ_MODE"];			// 1:NONE
			[self setValue:UIFalse			forKey:@"FLAG2_PSY"];		// OFF
			break;
		case X264TUNE_SSIM:
			[self setValue:[UIChar:3]		forKey:@"AQ_MODE"];			// 3:AUTOVARIANCE
			[self setValue:UIFalse			forKey:@"FLAG2_PSY"];		// OFF
			break;
		case X264TUNE_FASTDECODE:
			[self setValue:UIFalse			forKey:@"FLAG_LOOP_FILTER"];	// OFF
			[self setValue:UIOne			forKey:@"CODER_TYPE"];		// 1:CAVLC
			[self setValue:UIFalse			forKey:@"FLAG2_WPRED"];		// OFF
			[self setValue:UIOne			forKey:@"WEIGHTP"];			// 1:Disabled
			break;
		case X264TUNE_ZEROLATENCY:
			[self setValue:UIZero			forKey:@"RC_LOOKAHEAD"];	// 0
			// x4->params.i_sync_lookahead = 0;							// patched libx264.c
			[self setValue:UIZero			forKey:@"MAX_BFRAMES"];		// 0
			// x4->params.b_sliced_threads = 1;							// patched libx264.c
			// param->b_vfr_input = 0;									// patched libx264.c
			[self setValue:UIFalse			forKey:@"FLAG2_MBTREE"];	// OFF
		case X264TUNE_TOUHOU:
			if([REFS charValue]>1) 
				[self setValue:[UIChar:[REFS charValue]*2]	forKey:@"REFS"];	// Double REF value
			[self setValue:[UIChar:-1]		forKey:@"DEBLOCK_ALPHA"];	// -1
			[self setValue:[UIChar:-1]		forKey:@"DEBLOCK_BETA"];	// -1
			[self setValue:[UIFloat:0.2]	forKey:@"PSYRD_TRELLIS"];	// 0.2
			[self setValue:[UIFloat:1.3]	forKey:@"AQ_STRENGTH"];		// 1.3
			// if( param->analyse.inter & X264_ANALYSE_PSUB16x16 )
			//	param->analyse.inter |= X264_ANALYSE_PSUB8x8;			// patched libx264.c
			break;
		default:
			break;
		}
	}
}

#pragma mark -

- (BOOL)isAvc1
{
	return [fourCC isEqualToString:@"avc1"];
}

- (BOOL)isMp4v
{
	return [fourCC isEqualToString:@"mp4v"];
}

- (BOOL)isXvid
{
	return [fourCC isEqualToString:@"xvid"];
}

- (BOOL)isAvc1OrMp4v
{
	return [fourCC isEqualToString:@"avc1"]||[fourCC isEqualToString:@"mp4v"];
}

- (BOOL)isMp4vOrXvid
{
	return [fourCC isEqualToString:@"mp4v"]||[fourCC isEqualToString:@"xvid"];
}

- (BOOL)isXvidOrAvc1
{
	return [fourCC isEqualToString:@"xvid"]||[fourCC isEqualToString:@"avc1"];
}

- (BOOL)isNA
{
	return FALSE;
}

- (BOOL)isCoreVFReady
{
	if(CVF_Context_Create != NULL) return TRUE;
	return FALSE;
}

#undef UIBool
#undef UIInt
#undef UIFloat
#undef UIChar
#undef UIZero
#undef UIOne
#undef UITwo
#undef UIFalse
#undef UITrue

#pragma mark -
#pragma mark NSCoding protocol

#define UIEncode		encoder encodeObject
#define UIDecodeForKey	decoder decodeObjectForKey

- (void)encodeWithCoder:(NSCoder *)encoder
{
	[UIEncode:fourCC			forKey: @"fourCC"];
	
	[UIEncode:LOG_INFO			forKey: @"LOG_INFO"];
	[UIEncode:LOG_DEBUG			forKey: @"LOG_DEBUG"];
	[UIEncode:LOG_STATS			forKey: @"LOG_STATS"];
	[UIEncode:REV_STRUCT		forKey: @"REV_STRUCT"];
	
	[UIEncode:FLAG_QSCALE		forKey: @"FLAG_QSCALE"];
	[UIEncode:FLAG_4MV			forKey: @"FLAG_4MV"];
	[UIEncode:FLAG_QPEL			forKey: @"FLAG_QPEL"];
	[UIEncode:FLAG_GMC			forKey: @"FLAG_GMC"];
	[UIEncode:FLAG_MV0			forKey: @"FLAG_MV0"];
	[UIEncode:FLAG_PART			forKey: @"FLAG_PART"];
	[UIEncode:FLAG_LOOP_FILTER	forKey: @"FLAG_LOOP_FILTER"];
	[UIEncode:FLAG_PSNR			forKey: @"FLAG_PSNR"];
	[UIEncode:FLAG_NORMALIZE_AQP	forKey: @"FLAG_NORMALIZE_AQP"];
	[UIEncode:FLAG_INTERLACED_DCT	forKey: @"FLAG_INTERLACED_DCT"];
	[UIEncode:FLAG_AC_PRED		forKey: @"FLAG_AC_PRED"];
	[UIEncode:FLAG_CBP_RD		forKey: @"FLAG_CBP_RD"];
	[UIEncode:FLAG_QP_RD		forKey: @"FLAG_QP_RD"];
	[UIEncode:FLAG_CLOSED_GOP	forKey: @"FLAG_CLOSED_GOP"];
	
	[UIEncode:NATIVE_FPS		forKey: @"NATIVE_FPS"];
	[UIEncode:MB_DECISION		forKey: @"MB_DECISION"];
	[UIEncode:RC_QSQUISH		forKey: @"RC_QSQUISH"];
	[UIEncode:MPEG_QUANT		forKey: @"MPEG_QUANT"];
	[UIEncode:THREADS			forKey: @"THREADS"];
	[UIEncode:GAMMA				forKey: @"GAMMA"];
	[UIEncode:NCLC				forKey: @"NCLC"];
	[UIEncode:ME_METHOD			forKey: @"ME_METHOD"];
	
	[UIEncode:SC_THRESHOLD		forKey: @"SC_THRESHOLD"];
	[UIEncode:QCOMPRESS			forKey: @"QCOMPRESS"];
	[UIEncode:NOISE_REDUCTION	forKey: @"NOISE_REDUCTION"];
	[UIEncode:RC_MAXRATE		forKey: @"RC_MAXRATE"];
	
	[UIEncode:REFS				forKey: @"REFS"];
	[UIEncode:ME_RANGE			forKey: @"ME_RANGE"];
	[UIEncode:MAX_BFRAMES		forKey: @"MAX_BFRAMES"];
	[UIEncode:CODER_TYPE		forKey: @"CODER_TYPE"];
	[UIEncode:DIRECTPRED		forKey: @"DIRECTPRED"];
	[UIEncode:CRF				forKey: @"CRF"];
	[UIEncode:ADAPTIVE_BFRAME	forKey: @"ADAPTIVE_BFRAME"];
	[UIEncode:ME_SUBQ			forKey: @"ME_SUBQ"];
	
	[UIEncode:TRELLIS			forKey: @"TRELLIS"];
	[UIEncode:TURBO				forKey: @"TURBO"];
	[UIEncode:CHROMA_ME			forKey: @"CHROMA_ME"];
	[UIEncode:PART_4X4			forKey: @"PART_4X4"];
	[UIEncode:BIDIR_ME			forKey: @"BIDIR_ME"];
	[UIEncode:USE3RDPASS		forKey: @"USE3RDPASS"];
	[UIEncode:LEVEL				forKey: @"LEVEL"];
	[UIEncode:EMBEDUUID			forKey: @"EMBEDUUID"];
	
	[UIEncode:FLAG2_BPYRAMID	forKey: @"FLAG2_BPYRAMID"];
	[UIEncode:FLAG2_WPRED		forKey: @"FLAG2_WPRED"];
	[UIEncode:FLAG2_MIXED_REFS	forKey: @"FLAG2_MIXED_REFS"];
	[UIEncode:FLAG2_8X8DCT		forKey: @"FLAG2_8X8DCT"];
	[UIEncode:FLAG2_FASTPSKIP	forKey: @"FLAG2_FASTPSKIP"];
	[UIEncode:FLAG2_AUD			forKey: @"FLAG2_AUD"];
	[UIEncode:FLAG2_BRDO		forKey: @"FLAG2_BRDO"];
	[UIEncode:FLAG2_MBTREE		forKey: @"FLAG2_MBTREE"];
	[UIEncode:FLAG2_PSY			forKey: @"FLAG2_PSY"];
	[UIEncode:FLAG2_SSIM		forKey: @"FLAG2_SSIM"];
	
	[UIEncode:RC_BUFSIZE		forKey: @"RC_BUFSIZE"];
	[UIEncode:AQ_STRENGTH		forKey: @"AQ_STRENGTH"];
	[UIEncode:PSYRD_RDO			forKey: @"PSYRD_RDO"];
	[UIEncode:PSYRD_TRELLIS		forKey: @"PSYRD_TRELLIS"];

	[UIEncode:DEBLOCK_ALPHA		forKey: @"DEBLOCK_ALPHA"];
	[UIEncode:DEBLOCK_BETA		forKey: @"DEBLOCK_BETA"];
	[UIEncode:AQ_MODE			forKey: @"AQ_MODE"];
	[UIEncode:LUMI_MASKING		forKey: @"LUMI_MASKING"];

	[UIEncode:KEYINT_MIN		forKey: @"KEYINT_MIN"];
	[UIEncode:CQM_PRESET		forKey: @"CQM_PRESET"];
	[UIEncode:NO_DCT_DECIMATE	forKey: @"NO_DCT_DECIMATE"];
	[UIEncode:MAX_QDIFF			forKey: @"MAX_QDIFF"];

	[UIEncode:CHROMAOFFSET		forKey: @"CHROMAOFFSET"];
	[UIEncode:FILTERPRESET		forKey: @"FILTERPRESET"];
	[UIEncode:SUBSAMPLING		forKey: @"SUBSAMPLING"];
	[UIEncode:X264PROFILE		forKey: @"X264PROFILE"];
	
	[UIEncode:X264PRESET		forKey: @"X264PRESET"];
	[UIEncode:X264TUNE			forKey: @"X264TUNE"];
//	[UIEncode:MBTREE			forKey: @"MBTREE"];
//	[UIEncode:PSY				forKey: @"PSY"];
	
	[UIEncode:RC_LOOKAHEAD		forKey: @"RC_LOOKAHEAD"];
	[UIEncode:IP_FACTOR			forKey: @"IP_FACTOR"];
	[UIEncode:PB_FACTOR			forKey: @"PB_FACTOR"];
	
	[UIEncode:WEIGHTP			forKey: @"WEIGHTP"];
	[UIEncode:TOPFIELDFIRST		forKey: @"TOPFIELDFIRST"];
	[UIEncode:LOSSLESS			forKey: @"LOSSLESS"];
	[UIEncode:BD_TUNE			forKey: @"BD_TUNE"];
	
	[UIEncode:USEASPECTRATIO	forKey: @"USEASPECTRATIO"];
	[UIEncode:USECLEANAPERTURE	forKey: @"USECLEANAPERTURE"];
	[UIEncode:TAGASPECTRATIO	forKey: @"TAGASPECTRATIO"];
	[UIEncode:TAGCLEANAPERTURE	forKey: @"TAGCLEANAPERTURE"];
	
	[UIEncode:HSPACING			forKey: @"HSPACING"];
	[UIEncode:VSPACING			forKey: @"VSPACING"];
	[UIEncode:CLEANAPERTUREWIDTHN	forKey: @"CLEANAPERTUREWIDTHN"];
	[UIEncode:CLEANAPERTUREWIDTHD	forKey: @"CLEANAPERTUREWIDTHD"];
	[UIEncode:CLEANAPERTUREHEIGHTN	forKey: @"CLEANAPERTUREHEIGHTN"];
	[UIEncode:CLEANAPERTUREHEIGHTD	forKey: @"CLEANAPERTUREHEIGHTD"];
	[UIEncode:HORIZOFFN			forKey: @"HORIZOFFN"];
	[UIEncode:HORIZOFFD			forKey: @"HORIZOFFD"];
	[UIEncode:VERTOFFN			forKey: @"VERTOFFN"];
	[UIEncode:VERTOFFD			forKey: @"VERTOFFD"];
	
	[UIEncode:OVERRIDECRFQSCALE	forKey: @"OVERRIDECRFQSCALE"];
	[UIEncode:USERCRFQSCALE		forKey: @"USERCRFQSCALE"];
	[UIEncode:OVERRIDEQMIN		forKey: @"OVERRIDEQMIN"];
	[UIEncode:USERQMIN			forKey: @"USERQMIN"];
	
	[UIEncode:FAKEINTERLACED	forKey: @"FAKEINTERLACED"];
}

- (id)initWithCoder:(NSCoder *)decoder
{
	if(self = [super init]) {
		id val;
		val = [UIDecodeForKey: @"fourCC"];
		if(val) [self setValue:val forKey: @"fourCC"];
		
		[self resetToDefaultKeeping:FALSE];
		
		val = [UIDecodeForKey: @"LOG_INFO"];
		if(val) [self setValue:val forKey: @"LOG_INFO"];
		val = [UIDecodeForKey: @"LOG_DEBUG"];
		if(val) [self setValue:val forKey: @"LOG_DEBUG"];
		val = [UIDecodeForKey: @"LOG_STATS"];
		if(val) [self setValue:val forKey: @"LOG_STATS"];
		val = [UIDecodeForKey: @"REV_STRUCT"];
		if(val) [self setValue:val forKey: @"REV_STRUCT"];
		
		val = [UIDecodeForKey: @"FLAG_QSCALE"];
		if(val) [self setValue:val forKey: @"FLAG_QSCALE"];
		val = [UIDecodeForKey: @"FLAG_4MV"];
		if(val) [self setValue:val forKey: @"FLAG_4MV"];
		val = [UIDecodeForKey: @"FLAG_QPEL"];
		if(val) [self setValue:val forKey: @"FLAG_QPEL"];
		val = [UIDecodeForKey: @"FLAG_GMC"];
		if(val) [self setValue:val forKey: @"FLAG_GMC"];
		val = [UIDecodeForKey: @"FLAG_MV0"];
		if(val) [self setValue:val forKey: @"FLAG_MV0"];
		val = [UIDecodeForKey: @"FLAG_PART"];
		if(val) [self setValue:val forKey: @"FLAG_PART"];
		val = [UIDecodeForKey: @"FLAG_LOOP_FILTER"];
		if(val) [self setValue:val forKey: @"FLAG_LOOP_FILTER"];
		val = [UIDecodeForKey: @"FLAG_PSNR"];
		if(val) [self setValue:val forKey: @"FLAG_PSNR"];
		val = [UIDecodeForKey: @"FLAG_NORMALIZE_AQP"];
		if(val) [self setValue:val forKey: @"FLAG_NORMALIZE_AQP"];
		val = [UIDecodeForKey: @"FLAG_INTERLACED_DCT"];
		if(val) [self setValue:val forKey: @"FLAG_INTERLACED_DCT"];
		val = [UIDecodeForKey: @"FLAG_AC_PRED"];
		if(val) [self setValue:val forKey: @"FLAG_AC_PRED"];
		val = [UIDecodeForKey: @"FLAG_CBP_RD"];
		if(val) [self setValue:val forKey: @"FLAG_CBP_RD"];
		val = [UIDecodeForKey: @"FLAG_QP_RD"];
		if(val) [self setValue:val forKey: @"FLAG_QP_RD"];
		val = [UIDecodeForKey: @"FLAG_CLOSED_GOP"];
		if(val) [self setValue:val forKey: @"FLAG_CLOSED_GOP"];
		
		val = [UIDecodeForKey: @"NATIVE_FPS"];
		if(val) [self setValue:val forKey: @"NATIVE_FPS"];
		val = [UIDecodeForKey: @"MB_DECISION"];
		if(val) [self setValue:val forKey: @"MB_DECISION"];
		val = [UIDecodeForKey: @"RC_QSQUISH"];
		if(val) [self setValue:val forKey: @"RC_QSQUISH"];
		val = [UIDecodeForKey: @"MPEG_QUANT"];
		if(val) [self setValue:val forKey: @"MPEG_QUANT"];
		val = [UIDecodeForKey: @"THREADS"];
		if(val) [self setValue:val forKey: @"THREADS"];
		val = [UIDecodeForKey: @"GAMMA"];
		if(val) [self setValue:val forKey: @"GAMMA"];
		val = [UIDecodeForKey: @"NCLC"];
		if(val) [self setValue:val forKey: @"NCLC"];
		val = [UIDecodeForKey: @"ME_METHOD"];
		if(val) [self setValue:val forKey: @"ME_METHOD"];
		
		val = [UIDecodeForKey: @"SC_THRESHOLD"];
		if(val) [self setValue:val forKey: @"SC_THRESHOLD"];
		val = [UIDecodeForKey: @"QCOMPRESS"];
		if(val) [self setValue:val forKey: @"QCOMPRESS"];
		val = [UIDecodeForKey: @"NOISE_REDUCTION"];
		if(val) [self setValue:val forKey: @"NOISE_REDUCTION"];
		val = [UIDecodeForKey: @"RC_MAXRATE"];
		if(val) [self setValue:val forKey: @"RC_MAXRATE"];
		
		val = [UIDecodeForKey: @"REFS"];
		if(val) [self setValue:val forKey: @"REFS"];
		val = [UIDecodeForKey: @"ME_RANGE"];
		if(val) [self setValue:val forKey: @"ME_RANGE"];
		val = [UIDecodeForKey: @"MAX_BFRAMES"];
		if(val) [self setValue:val forKey: @"MAX_BFRAMES"];
		val = [UIDecodeForKey: @"CODER_TYPE"];
		if(val) [self setValue:val forKey: @"CODER_TYPE"];
		val = [UIDecodeForKey: @"DIRECTPRED"];
		if(val) [self setValue:val forKey: @"DIRECTPRED"];
		val = [UIDecodeForKey: @"CRF"];
		if(val) [self setValue:val forKey: @"CRF"];
		val = [UIDecodeForKey: @"ADAPTIVE_BFRAME"];
		if(val) [self setValue:val forKey: @"ADAPTIVE_BFRAME"];
		val = [UIDecodeForKey: @"ME_SUBQ"];
		if(val) [self setValue:val forKey: @"ME_SUBQ"];
		
		val = [UIDecodeForKey: @"TRELLIS"];
		if(val) [self setValue:val forKey: @"TRELLIS"];
		val = [UIDecodeForKey: @"TURBO"];
		if(val) [self setValue:val forKey: @"TURBO"];
		val = [UIDecodeForKey: @"CHROMA_ME"];
		if(val) [self setValue:val forKey: @"CHROMA_ME"];
		val = [UIDecodeForKey: @"PART_4X4"];
		if(val) [self setValue:val forKey: @"PART_4X4"];
		val = [UIDecodeForKey: @"BIDIR_ME"];
		if(val) [self setValue:val forKey: @"BIDIR_ME"];
		val = [UIDecodeForKey: @"USE3RDPASS"];
		if(val) [self setValue:val forKey: @"USE3RDPASS"];
		val = [UIDecodeForKey: @"LEVEL"];
		if(val) [self setValue:val forKey: @"LEVEL"];
		val = [UIDecodeForKey: @"EMBEDUUID"];
		if(val) [self setValue:val forKey: @"EMBEDUUID"];
		
		val = [UIDecodeForKey: @"FLAG2_BPYRAMID"];
		if(val) [self setValue:val forKey: @"FLAG2_BPYRAMID"];
		val = [UIDecodeForKey: @"FLAG2_WPRED"];
		if(val) [self setValue:val forKey: @"FLAG2_WPRED"];
		val = [UIDecodeForKey: @"FLAG2_MIXED_REFS"];
		if(val) [self setValue:val forKey: @"FLAG2_MIXED_REFS"];
		val = [UIDecodeForKey: @"FLAG2_8X8DCT"];
		if(val) [self setValue:val forKey: @"FLAG2_8X8DCT"];
		val = [UIDecodeForKey: @"FLAG2_FASTPSKIP"];
		if(val) [self setValue:val forKey: @"FLAG2_FASTPSKIP"];
		val = [UIDecodeForKey: @"FLAG2_AUD"];
		if(val) [self setValue:val forKey: @"FLAG2_AUD"];
		val = [UIDecodeForKey: @"FLAG2_BRDO"];
		if(val) [self setValue:val forKey: @"FLAG2_BRDO"];
		val = [UIDecodeForKey: @"FLAG2_MBTREE"];
		if(val) [self setValue:val forKey: @"FLAG2_MBTREE"];
		val = [UIDecodeForKey: @"FLAG2_PSY"];
		if(val) [self setValue:val forKey: @"FLAG2_PSY"];
		val = [UIDecodeForKey: @"FLAG2_SSIM"];
		if(val) [self setValue:val forKey: @"FLAG2_SSIM"];
		
		val = [UIDecodeForKey: @"RC_BUFSIZE"];
		if(val) [self setValue:val forKey: @"RC_BUFSIZE"];
		val = [UIDecodeForKey: @"AQ_STRENGTH"];
		if(val) [self setValue:val forKey: @"AQ_STRENGTH"];
		val = [UIDecodeForKey: @"PSYRD_RDO"];
		if(val) [self setValue:val forKey: @"PSYRD_RDO"];
		val = [UIDecodeForKey: @"PSYRD_TRELLIS"];
		if(val) [self setValue:val forKey: @"PSYRD_TRELLIS"];

		val = [UIDecodeForKey: @"DEBLOCK_ALPHA"];
		if(val) [self setValue:val forKey: @"DEBLOCK_ALPHA"];
		val = [UIDecodeForKey: @"DEBLOCK_BETA"];
		if(val) [self setValue:val forKey: @"DEBLOCK_BETA"];
		val = [UIDecodeForKey: @"AQ_MODE"];
		if(val) [self setValue:val forKey: @"AQ_MODE"];
		val = [UIDecodeForKey: @"LUMI_MASKING"];
		if(val) [self setValue:val forKey: @"LUMI_MASKING"];

		val = [UIDecodeForKey: @"KEYINT_MIN"];
		if(val) [self setValue:val forKey: @"KEYINT_MIN"];
		val = [UIDecodeForKey: @"CQM_PRESET"];
		if(val) [self setValue:val forKey: @"CQM_PRESET"];
		val = [UIDecodeForKey: @"NO_DCT_DECIMATE"];
		if(val) [self setValue:val forKey: @"NO_DCT_DECIMATE"];
		val = [UIDecodeForKey: @"MAX_QDIFF"];
		if(val) [self setValue:val forKey: @"MAX_QDIFF"];

		val = [UIDecodeForKey: @"CHROMAOFFSET"];
		if(val) [self setValue:val forKey: @"CHROMAOFFSET"];
		val = [UIDecodeForKey: @"FILTERPRESET"];
		if(val) [self setValue:val forKey: @"FILTERPRESET"];
		val = [UIDecodeForKey: @"SUBSAMPLING"];
		if(val) [self setValue:val forKey: @"SUBSAMPLING"];
		val = [UIDecodeForKey: @"X264PROFILE"];
		if(val) [self setValue:val forKey: @"X264PROFILE"];
		
		val = [UIDecodeForKey: @"X264PRESET"];
		if(val) [self setValue:val forKey: @"X264PRESET"];
		val = [UIDecodeForKey: @"X264TUNE"];
		if(val) [self setValue:val forKey: @"X264TUNE"];
//		val = [UIDecodeForKey: @"MBTREE"];
//		if(val) [self setValue:val forKey: @"MBTREE"];
//		val = [UIDecodeForKey: @"PSY"];
//		if(val) [self setValue:val forKey: @"PSY"];
		
		val = [UIDecodeForKey: @"RC_LOOKAHEAD"];
		if(val) [self setValue:val forKey: @"RC_LOOKAHEAD"];
		val = [UIDecodeForKey: @"IP_FACTOR"];
		if(val) [self setValue:val forKey: @"IP_FACTOR"];
		val = [UIDecodeForKey: @"PB_FACTOR"];
		if(val) [self setValue:val forKey: @"PB_FACTOR"];
		
		val = [UIDecodeForKey: @"WEIGHTP"];
		if(val) [self setValue:val forKey: @"WEIGHTP"];
		val = [UIDecodeForKey: @"TOPFIELDFIRST"];
		if(val) [self setValue:val forKey: @"TOPFIELDFIRST"];
		val = [UIDecodeForKey: @"LOSSLESS"];
		if(val) [self setValue:val forKey: @"LOSSLESS"];
		val = [UIDecodeForKey: @"BD_TUNE"];
		if(val) [self setValue:val forKey: @"BD_TUNE"];
		
		val = [UIDecodeForKey: @"USEASPECTRATIO"];
		if(val) [self setValue:val forKey: @"USEASPECTRATIO"];
		val = [UIDecodeForKey: @"USECLEANAPERTURE"];
		if(val) [self setValue:val forKey: @"USECLEANAPERTURE"];
		val = [UIDecodeForKey: @"TAGASPECTRATIO"];
		if(val) [self setValue:val forKey: @"TAGASPECTRATIO"];
		val = [UIDecodeForKey: @"TAGCLEANAPERTURE"];
		if(val) [self setValue:val forKey: @"TAGCLEANAPERTURE"];
		
		val = [UIDecodeForKey: @"HSPACING"];
		if(val) [self setValue:val forKey: @"HSPACING"];
		val = [UIDecodeForKey: @"VSPACING"];
		if(val) [self setValue:val forKey: @"VSPACING"];
		val = [UIDecodeForKey: @"CLEANAPERTUREWIDTHN"];
		if(val) [self setValue:val forKey: @"CLEANAPERTUREWIDTHN"];
		val = [UIDecodeForKey: @"CLEANAPERTUREWIDTHD"];
		if(val) [self setValue:val forKey: @"CLEANAPERTUREWIDTHD"];
		val = [UIDecodeForKey: @"CLEANAPERTUREHEIGHTN"];
		if(val) [self setValue:val forKey: @"CLEANAPERTUREHEIGHTN"];
		val = [UIDecodeForKey: @"CLEANAPERTUREHEIGHTD"];
		if(val) [self setValue:val forKey: @"CLEANAPERTUREHEIGHTD"];
		val = [UIDecodeForKey: @"HORIZOFFN"];
		if(val) [self setValue:val forKey: @"HORIZOFFN"];
		val = [UIDecodeForKey: @"HORIZOFFD"];
		if(val) [self setValue:val forKey: @"HORIZOFFD"];
		val = [UIDecodeForKey: @"VERTOFFN"];
		if(val) [self setValue:val forKey: @"VERTOFFN"];
		val = [UIDecodeForKey: @"VERTOFFD"];
		if(val) [self setValue:val forKey: @"VERTOFFD"];
		
		val = [UIDecodeForKey: @"OVERRIDECRFQSCALE"];
		if(val) [self setValue:val forKey: @"OVERRIDECRFQSCALE"];
		val = [UIDecodeForKey: @"USERCRFQSCALE"];
		if(val) [self setValue:val forKey: @"USERCRFQSCALE"];
		val = [UIDecodeForKey: @"OVERRIDEQMIN"];
		if(val) [self setValue:val forKey: @"OVERRIDEQMIN"];
		val = [UIDecodeForKey: @"USERQMIN"];
		if(val) [self setValue:val forKey: @"USERQMIN"];
		
		val = [UIDecodeForKey: @"FAKEINTERLACED"];
		if(val) [self setValue:val forKey: @"FAKEINTERLACED"];
	}
	
	return self;
}

#undef UIEncode
#undef UIDecodeForKey

@end
