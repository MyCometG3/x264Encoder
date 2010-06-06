/*
 *  x264EncoderDispatch.h
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

	ComponentSelectorOffset (8)

	ComponentRangeCount (1)
	ComponentRangeShift (8)
	ComponentRangeMask	(FF)

	ComponentRangeBegin (0)
		ComponentError	 (GetMPWorkFunction)
		ComponentError	 (Unregister)
		StdComponentCall (Target)
		ComponentError   (Register)
		StdComponentCall (Version)
		StdComponentCall (CanDo)
		StdComponentCall (Close)
		StdComponentCall (Open)
	ComponentRangeEnd (0)
		
	ComponentRangeBegin (1)
		ComponentCall (GetCodecInfo)							// 0
		ComponentError (GetCompressionTime)
		ComponentCall (GetMaxCompressionSize)
		ComponentError (PreCompress)
		ComponentError (BandCompress)
		ComponentError (PreDecompress)							// 5
		ComponentError (BandDecompress)
		ComponentError (Busy)
		ComponentError (GetCompressedImageSize)
		ComponentError (GetSimilarity)
		ComponentError (TrimImage)								// 10
		ComponentCall (RequestSettings)
		ComponentCall (GetSettings)
		ComponentCall (SetSettings)
		ComponentError (Flush)
		ComponentError (SetTimeCode)							// 15
		ComponentError (IsImageDescriptionEquivalent)
		ComponentError (NewMemory)
		ComponentError (DisposeMemory)
		ComponentError (HitTestData)
		ComponentError (NewImageBufferMemory)					// 20
		ComponentError (ExtractAndCombineFields)
		ComponentError (GetMaxCompressionSizeWithSources)
		ComponentError (SetTimeBase)
		ComponentError (SourceChanged)
		ComponentError (FlushLastFrame)							// 25
		ComponentError (GetSettingsAsText)
		ComponentError (GetParameterListHandle)
		ComponentError (GetParameterList)
		ComponentError (CreateStandardParameterDialog)
		ComponentError (IsStandardParameterDialogEvent)			// 30
		ComponentError (DismissStandardParameterDialog)
		ComponentError (StandardParameterDialogDoAction)
		ComponentError (NewImageGWorld)
		ComponentError (DisposeImageGWorld)
		ComponentError (HitTestDataWithFlags)					// 35
		ComponentError (ValidateParameters)
		ComponentError (GetBaseMPWorkFunction)
		ComponentError (LockBits)
		ComponentError (UnlockBits)
		ComponentError (RequestGammaLevel)						// 40
		ComponentError (GetSourceDataGammaLevel)
		ComponentError (42)
		ComponentError (GetDecompressLatency)
		ComponentError (MergeFloatingImageOntoWindow)
		ComponentError (RemoveFloatingImage)					// 45
		ComponentError (GetDITLForSize)
		ComponentError (DITLInstall)
		ComponentError (DITLEvent)
		ComponentError (DITLItem)
		ComponentError (DITLRemove)								// 50
		ComponentError (DITLValidateInput)
		ComponentError (52)
		ComponentError (53)
		ComponentError (GetPreferredChunkSizeAndAlignment)
		ComponentCall (PrepareToCompressFrames)					// 55
		ComponentCall (EncodeFrame)
		ComponentCall (CompleteFrame)
    	ComponentCall (BeginPass)
    	ComponentCall (EndPass)
		ComponentCall (ProcessBetweenPasses)					// 60
	ComponentRangeEnd (1)
