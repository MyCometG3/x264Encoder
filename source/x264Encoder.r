/*
 *  x264Encoder.r
 *  x264Encoder
 *
 *  Created by Takashi Mochizuki on 07/02/18. (mochi at da2.so-net.ne.jp)
 *  Copyright 2007-2009 MyCometG3. All rights reserved.
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

// ---------------------------------------------------
// Mac OS X Mach-O Component: Set TARGET_REZ_CARBON_MACHO to 1
//
// In the target settings of your Xcode project, add one or both of the
// following defines to your OTHER_REZFLAGS depending on the type of component
// you want to build:
//
//      PPC only: -d ppc_$(ppc)
//      x86 only: -d i386_$(i386)
//      Universal Binary: -d ppc_$(ppc) -d i386_$(i386)
//
// Windows Component: Set TARGET_REZ_CARBON_MACHO to 0
// ---------------------------------------------------


// Set to 1 == building Mac OS X
#define TARGET_REZ_CARBON_MACHO 1

#if TARGET_REZ_CARBON_MACHO

    #if defined(ppc_YES)
        // PPC architecture
        #define TARGET_REZ_MAC_PPC 1
    #else
        #define TARGET_REZ_MAC_PPC 0
    #endif

    #if defined(i386_YES)
        // x86 architecture
        #define TARGET_REZ_MAC_X86 1
    #else
        #define TARGET_REZ_MAC_X86 0
    #endif

    #define TARGET_REZ_WIN32 0
#else
    // Must be building on Windows
    #define TARGET_REZ_WIN32 1
#endif

/*
    thng_RezTemplateVersion:
        0 - original 'thng' template    <-- default
        1 - extended 'thng' template	<-- used for multiplatform things
        2 - extended 'thng' template including resource map id
*/
#define thng_RezTemplateVersion 2

#if TARGET_REZ_CARBON_MACHO
    #include <Carbon/Carbon.r>
    #include <QuickTime/QuickTime.r>
#else
    #include "ConditionalMacros.r"
    #include "MacTypes.r"
    #include "Components.r"
    #include "ImageCodec.r"
#endif

#include "x264EncoderVersion.h"

#if 1
#define	kCodec_FormatType	'AVC1'	/* Replaced at PrepareToCompressFrame */
#else
#define	kCodec_FormatType	'avc1'	/* Why!? Could conflict with Apple Export components - iPod/AppleTV/iPhone */
#endif
#define	kCodec_FormatName	"x264Encoder"
#define kCodec_Information	"MPEG-4/AVC (x264) compressor component."

#define kCodec_EncoderFlags ( codecInfoDoes32 | codecInfoDoesMultiPass | codecInfoDoesTemporal | codecInfoDoesRateConstrain | codecInfoDoesReorder )
#define kCodec_FormatFlags	( codecInfoDepth24 | codecInfoSequenceSensitive)

// Component Description
resource 'cdci' (256) {
	kCodec_FormatName,				// Type
	1,								// Version
	1,								// Revision level
	'Mg3b',							// Manufacturer
	0,								// Decompression Flags
	kCodec_EncoderFlags,			// Compression Flags
	kCodec_FormatFlags,				// Format Flags
	128,							// Compression Accuracy
	128,							// Decomression Accuracy
	200,							// Compression Speed
	200,							// Decompression Speed
	128,							// Compression Level
	0,								// Reserved
	16,								// Minimum Height
	16,								// Minimum Width
	0,								// Decompression Pipeline Latency
	0,								// Compression Pipeline Latency
	0								// Private Data
};

resource 'thng' (258) {
	compressorComponentType,				// Type			
	kCodec_FormatType,						// SubType
	'Mg3b',									// Manufacturer
	0,										// - use componentHasMultiplePlatforms
	0,
	0,
	0,
	'STR ',									// Name Type
	256,									// Name ID
	'STR ',									// Info Type
	258,									// Info ID
	0,										// Icon Type
	0,										// Icon ID
	klavcEncoderVersion,
	componentHasMultiplePlatforms +			// Registration Flags 
		componentDoAutoVersion,
	0,										// Resource ID of Icon Family
	{
	#if TARGET_REZ_CARBON_MACHO
		#if TARGET_REZ_MAC_PPC    
			kCodec_EncoderFlags | cmpThreadSafe, 
			'dlle',
			258,
			platformPowerPCNativeEntryPoint,
		#endif
		#if TARGET_REZ_MAC_X86
			kCodec_EncoderFlags | cmpThreadSafe, 
			'dlle',
			258,
			platformIA32NativeEntryPoint,
		#endif
	#endif
	#if TARGET_OS_WIN32
		klavcEncoderFlags, 
		'dlle',
		258,
		platformWin32,
	#endif
	},
	'thnr', 258
};

// Component Public Resource Map
resource 'thnr' (258)
{
	{
		'cpix', 1, 0,		// "public" resource type and ID
		'cpix', 258, 0		// actual resource type and ID
	}
};

// Codec Pixel Format
resource 'cpix' (258)
{
	{
		'2vuy'
	}
};

// Component Name
resource 'STR ' (256) {
	kCodec_FormatName
};

// Component Information
resource 'STR ' (258) {
	kCodec_Information
};

// Code Entry Point for Mach-O and Windows
resource 'dlle' (258) {
	"lavcEncoder_ComponentDispatch"
};
