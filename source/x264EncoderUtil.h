/*
 *  x264EncoderUtil.h
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

#ifndef LAVCENCODERUTIL_H
#define LAVCENCODERUTIL_H

int roundUpToMultipleOf16( int n );

void addNumberToDictionary( CFMutableDictionaryRef dictionary, CFStringRef key, SInt32 numberSInt32 );
void addDoubleToDictionary( CFMutableDictionaryRef dictionary, CFStringRef key, double numberDouble );
double psnr(double d);

void avc_parse_nal_units(uint8_t **buf, int *size);

#pragma mark Convert PixelBuffer
void copy_2vuy_to_planar_YUV420(size_t width, size_t height, 
									   const UInt8 *baseAddr_2vuy, size_t rowBytes_2vuy,
									   UInt8 *baseAddr_y, size_t rowBytes_y, 
									   UInt8 *baseAddr_u, size_t rowBytes_u, 
									   UInt8 *baseAddr_v, size_t rowBytes_v);

#pragma mark Convert PixelBuffer (Interlaced Support)
// Support Interlaced Xfer (mode; 0:prog, 1:Weight3-1, 2:Weight5:3)
void copy_2vuy_to_planar_YUV420_i(size_t width, size_t height, 
									   const UInt8 *baseAddr_2vuy, size_t rowBytes_2vuy,
									   UInt8 *baseAddr_y, size_t rowBytes_y, 
									   UInt8 *baseAddr_u, size_t rowBytes_u, 
									   UInt8 *baseAddr_v, size_t rowBytes_v, int mode);

#endif //LAVCENCODERUTIL_H
