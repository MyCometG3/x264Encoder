/*
 *  x264EncoderUtil.c
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

#define BENCHMARK 0
#define ENABLEFASTER 1
#define USESIMD 1

#include "x264EncoderUtil.h"


#pragma mark -
#pragma mark utility functions


// Utility to round up to a multiple of 16.
int roundUpToMultipleOf16( int n )
{
	if( 0 != ( n & 15 ) )
		n = ( n + 15 ) & ~15;
	return n;
}


// Utility to add an SInt32 to a CFMutableDictionary.
void addNumberToDictionary( CFMutableDictionaryRef dictionary, CFStringRef key, SInt32 numberSInt32 )
{
	CFNumberRef number = CFNumberCreate( NULL, kCFNumberSInt32Type, &numberSInt32 );
	if( ! number ) 
		return;
	CFDictionaryAddValue( dictionary, key, number );
	CFRelease( number );
}


// Utility to add a double to a CFMutableDictionary.
void addDoubleToDictionary( CFMutableDictionaryRef dictionary, CFStringRef key, double numberDouble )
{
	CFNumberRef number = CFNumberCreate( NULL, kCFNumberDoubleType, &numberDouble );
	if( ! number ) 
		return;
	CFDictionaryAddValue( dictionary, key, number );
	CFRelease( number );
}


// Utility for psnr calc, is come from ffmpeg.c by Fabrice Bellard.
double psnr(double d){
    if(d==0) return INFINITY;
    return -10.0*log(d)/log(10.0);
}


#pragma mark -
#pragma mark nal support utility from ffmpeg project


// nal support utility from ffmpeg project trunk/libavformat/avc.c
const static uint8_t *ff_avc_find_startcode_internal(const uint8_t *p, const uint8_t *end)
{
    const uint8_t *a = p + 4 - ((intptr_t)p & 3);

    for (end -= 3; p < a && p < end; p++) {
        if (p[0] == 0 && p[1] == 0 && p[2] == 1)
            return p;
    }

    for (end -= 3; p < end; p += 4) {
        uint32_t x = *(const uint32_t*)p;
//      if ((x - 0x01000100) & (~x) & 0x80008000) // little endian
//      if ((x - 0x00010001) & (~x) & 0x00800080) // big endian
        if ((x - 0x01010101) & (~x) & 0x80808080) { // generic
            if (p[1] == 0) {
                if (p[0] == 0 && p[2] == 1)
                    return p;
                if (p[2] == 0 && p[3] == 1)
                    return p+1;
            }
            if (p[3] == 0) {
                if (p[2] == 0 && p[4] == 1)
                    return p+2;
                if (p[4] == 0 && p[5] == 1)
                    return p+3;
            }
        }
    }

    for (end += 3; p < end; p++) {
        if (p[0] == 0 && p[1] == 0 && p[2] == 1)
            return p;
    }

    return end + 3;
}

const uint8_t *avc_find_startcode(const uint8_t *p, const uint8_t *end ) {
    const uint8_t *out= ff_avc_find_startcode_internal(p, end);
    if(p<out && out<end && !out[-1]) out--;
    return out;
}


// nal support utility from ffmpeg project trunk/libavformat/movenc.c
void avc_parse_nal_units(uint8_t **buf, int *size)
{
    const uint8_t *p = *buf;
    const uint8_t *end = p + *size;
    const uint8_t *nal_start, *nal_end;
	
    AVIOContext *pb;
    int ret = avio_open_dyn_buf(&pb);
    if(ret < 0)
        return;

    nal_start = avc_find_startcode(p, end);
    while (nal_start < end) {
        while(!*(nal_start++));
        nal_end = avc_find_startcode(nal_start, end);
        avio_wb32(pb, nal_end - nal_start);
        avio_write(pb, nal_start, nal_end - nal_start);
        nal_start = nal_end;
    }
	
    av_freep(buf);
    *size = avio_close_dyn_buf(pb, buf);
}


#pragma mark -
#pragma mark Convert PixelBuffer


// Util to convert chunky yuv422 into planer YUV420
// For bitmap transfer : CVPixelBuffer -> AVFrame.
void copy_2vuy_to_planar_YUV420(size_t width, size_t height, 
									   const UInt8 *baseAddr_2vuy, size_t rowBytes_2vuy,
									   UInt8 *baseAddr_y, size_t rowBytes_y, 
									   UInt8 *baseAddr_u, size_t rowBytes_u, 
									   UInt8 *baseAddr_v, size_t rowBytes_v)
{
	assert( !(width & 0x1) );	/* At least x should be even value */
//	assert( !(width & 0x1 || height & 0x1) );	/* At least both x and y should be even value */
	
#if USESIMD
	#if defined(__BIG_ENDIAN__)
		// Permute index
		register vUInt8 indexUV = { 0, 4, 8,12,16,20,24,28, 2, 6,10,14,18,22,26,30};
		register vUInt8 indexY =  { 1, 3, 5, 7, 9,11,13,15,17,19,21,23,25,27,29,31};
		register vUInt8 indexHI = { 0, 1, 2, 3, 4, 5, 6, 7,16,17,18,19,20,21,22,23};
		register vUInt8 indexLO = { 8, 9,10,11,12,13,14,15,24,25,26,27,28,29,30,31};
	#endif
	#if defined(__LITTLE_ENDIAN__)
		static vUInt16 maskL = {0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF};
		static vUInt32 mask8 = {0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF};
	#endif
#endif
	
#if BENCHMARK
	{clock_t start = clock(); int i ; for(i=0;i<10000;i++) {
#endif
	
#pragma omp parallel
	{	/*start of parallel region*/
	
#if USESIMD	/* memalign hack */
	UInt8* temp_yt_aligned = malloc(roundUpToMultipleOf16(rowBytes_y));
	UInt8* temp_yb_aligned = malloc(roundUpToMultipleOf16(rowBytes_y));
	UInt8* temp_u_aligned  = malloc(roundUpToMultipleOf16(rowBytes_u));
	UInt8* temp_v_aligned  = malloc(roundUpToMultipleOf16(rowBytes_v));
#endif
	
	int y;

#pragma omp for nowait
	for (y = 0; y < height; y += 2) {		// allow overshoot on odd value
//	for (y = 0; y <= height-2; y += 2) {	// left bottom line on odd value
		
		int x=0;
		UInt8 *p2top, *p2bot, *pytop, *pybot, *pu, *pv;
		p2top = (  y) * rowBytes_2vuy + (UInt8*)baseAddr_2vuy;
		p2bot = (1+y) * rowBytes_2vuy + (UInt8*)baseAddr_2vuy;
		pytop = (  y) * rowBytes_y + (UInt8*)baseAddr_y;
		pybot = (1+y) * rowBytes_y + (UInt8*)baseAddr_y;
		pu =	(y/2) * rowBytes_u + (UInt8*)baseAddr_u;
		pv =	(y/2) * rowBytes_v + (UInt8*)baseAddr_v;
		
		if((1+y) >= height) p2bot = p2bot - 1 * rowBytes_2vuy;
		
	#if USESIMD	/* memalign hack */
		bool hackytop = FALSE, hackybot = FALSE, hacku = FALSE, hackv = FALSE;
		if( (UInt32)pytop & 0xF ) { hackytop = TRUE; pytop = temp_yt_aligned; }
		if( (UInt32)pybot & 0xF ) { hackybot = TRUE; pybot = temp_yb_aligned; }
		if( (UInt32)pu    & 0xF ) { hacku    = TRUE; pu    = temp_u_aligned; }
		if( (UInt32)pv    & 0xF ) { hackv    = TRUE; pv    = temp_v_aligned; }
	#endif
		
#if ENABLEFASTER
	#if USESIMD
		#if defined(__BIG_ENDIAN__)
		vec_dst(p2top, (4<<24)|(rowBytes_2vuy/64)<<16|64, 0);
		vec_dst(p2bot, (4<<24)|(rowBytes_2vuy/64)<<16|64, 1);
		#endif	//defined(__BIG_ENDIAN__)
		#if defined(__LITTLE_ENDIAN__)
		// no prefetch applied for intel
		#endif	//defined(__LITTLE_ENDIAN__)
	#endif	//USESIMD
#endif	//ENABLEFASTER

#if ENABLEFASTER
	#if USESIMD
		for (x = 0; x <= width-32; x += 32) {
		#if defined(__BIG_ENDIAN__)
			vUInt8 yuvt0 = vec_ld( 0+x*2, p2top), yuvt1 = vec_ld(16+x*2, p2top);
			vUInt8 yuvt2 = vec_ld(32+x*2, p2top), yuvt3 = vec_ld(48+x*2, p2top);
			vUInt8 yuvb0 = vec_ld( 0+x*2, p2bot), yuvb1 = vec_ld(16+x*2, p2bot);
			vUInt8 yuvb2 = vec_ld(32+x*2, p2bot), yuvb3 = vec_ld(48+x*2, p2bot);
			
			vec_st(vec_perm(yuvt0, yuvt1, indexY),  0+x, pytop);
			vec_st(vec_perm(yuvt2, yuvt3, indexY), 16+x, pytop);
			vec_st(vec_perm(yuvb0, yuvb1, indexY),  0+x, pybot);
			vec_st(vec_perm(yuvb2, yuvb3, indexY), 16+x, pybot);
			
			vUInt8 uv0 = vec_perm(vec_avg(yuvt0, yuvb0), vec_avg(yuvt1, yuvb1), indexUV);
			vUInt8 uv1 = vec_perm(vec_avg(yuvt2, yuvb2), vec_avg(yuvt3, yuvb3), indexUV);
			
			vec_st(vec_perm(uv0, uv1, indexHI),  0+x/2, pu);
			vec_st(vec_perm(uv0, uv1, indexLO),  0+x/2, pv);
		#endif	//defined(__BIG_ENDIAN__)
		#if defined(__LITTLE_ENDIAN__)
			vUInt8 yuvt0 = _mm_load_si128( (__m128i*)( 0+x*2+p2top) ), yuvt1 = _mm_load_si128( (__m128i*)(16+x*2+p2top) );
			vUInt8 yuvt2 = _mm_load_si128( (__m128i*)(32+x*2+p2top) ), yuvt3 = _mm_load_si128( (__m128i*)(48+x*2+p2top) );
			vUInt8 yuvb0 = _mm_load_si128( (__m128i*)( 0+x*2+p2bot) ), yuvb1 = _mm_load_si128( (__m128i*)(16+x*2+p2bot) );
			vUInt8 yuvb2 = _mm_load_si128( (__m128i*)(32+x*2+p2bot) ), yuvb3 = _mm_load_si128( (__m128i*)(48+x*2+p2bot) );
			{
				vUInt8 yt0 = _mm_packus_epi16(	_mm_and_si128(maskL, _mm_srli_si128(yuvt0, 1)), 
												_mm_and_si128(maskL, _mm_srli_si128(yuvt1, 1)) );
				vUInt8 yt1 = _mm_packus_epi16(	_mm_and_si128(maskL, _mm_srli_si128(yuvt2, 1)), 
												_mm_and_si128(maskL, _mm_srli_si128(yuvt3, 1)) );
				vUInt8 yb0 = _mm_packus_epi16(	_mm_and_si128(maskL, _mm_srli_si128(yuvb0, 1)), 
												_mm_and_si128(maskL, _mm_srli_si128(yuvb1, 1)) );
				vUInt8 yb1 = _mm_packus_epi16(	_mm_and_si128(maskL, _mm_srli_si128(yuvb2, 1)), 
												_mm_and_si128(maskL, _mm_srli_si128(yuvb3, 1)) );
				
				_mm_store_si128( (__m128i*)( 0+x+pytop), (__m128i)yt0 );
				_mm_store_si128( (__m128i*)(16+x+pytop), (__m128i)yt1 );
				_mm_store_si128( (__m128i*)( 0+x+pybot), (__m128i)yb0 );
				_mm_store_si128( (__m128i*)(16+x+pybot), (__m128i)yb1 );
			}{
				vUInt8 ut0 = _mm_and_si128( mask8, yuvt0 );
				vUInt8 ut1 = _mm_and_si128( mask8, yuvt1 );
				vUInt8 ut2 = _mm_and_si128( mask8, yuvt2 );
				vUInt8 ut3 = _mm_and_si128( mask8, yuvt3 );
				vUInt8 ub0 = _mm_and_si128( mask8, yuvb0 );
				vUInt8 ub1 = _mm_and_si128( mask8, yuvb1 );
				vUInt8 ub2 = _mm_and_si128( mask8, yuvb2 );
				vUInt8 ub3 = _mm_and_si128( mask8, yuvb3 );
				vUInt8 u = _mm_packus_epi16(	_mm_avg_epu8( _mm_packus_epi16(ut0, ut1), _mm_packus_epi16(ub0, ub1) ), 
												_mm_avg_epu8( _mm_packus_epi16(ut2, ut3), _mm_packus_epi16(ub2, ub3) ) );
				
				_mm_store_si128( (__m128i*)(x/2+pu), (__m128i)u);
			}{
				vUInt8 vt0 = _mm_and_si128( mask8, _mm_srli_si128(yuvt0, 2) );
				vUInt8 vt1 = _mm_and_si128( mask8, _mm_srli_si128(yuvt1, 2) );
				vUInt8 vt2 = _mm_and_si128( mask8, _mm_srli_si128(yuvt2, 2) );
				vUInt8 vt3 = _mm_and_si128( mask8, _mm_srli_si128(yuvt3, 2) );
				vUInt8 vb0 = _mm_and_si128( mask8, _mm_srli_si128(yuvb0, 2) );
				vUInt8 vb1 = _mm_and_si128( mask8, _mm_srli_si128(yuvb1, 2) );
				vUInt8 vb2 = _mm_and_si128( mask8, _mm_srli_si128(yuvb2, 2) );
				vUInt8 vb3 = _mm_and_si128( mask8, _mm_srli_si128(yuvb3, 2) );
				__m128i v = _mm_packus_epi16(	_mm_avg_epu8( _mm_packus_epi16(vt0, vt1), _mm_packus_epi16(vb0, vb1) ), 
												_mm_avg_epu8( _mm_packus_epi16(vt2, vt3), _mm_packus_epi16(vb2, vb3) ) );
				
				_mm_store_si128( (__m128i*)(x/2+pv), (__m128i)v);
			}
		#endif	//defined(__LITTLE_ENDIAN__)
		}	// for(x <= width-32)
		
		if( x == width ) goto ENDOFROW;
		
		pytop += x;
		pybot += x;
		pu += x/2;
		pv += x/2;
		p2top += x*2;
		p2bot += x*2;
	#endif	//USESIMD
		
		for (; x <= width-8; x += 8) {
			UInt32 *ptrYt0 = (UInt32*)&pytop[0], *ptrYt1 = (UInt32*)&pytop[4];
			UInt32 *ptrYb0 = (UInt32*)&pybot[0], *ptrYb1 = (UInt32*)&pybot[4];
			UInt32 *ptrU = (UInt32*)&pu[0], *ptrV = (UInt32*)&pv[0];
		#if defined(__BIG_ENDIAN__)
			__builtin_prefetch(p2top+16+x*2+ 0,0,2);
			__builtin_prefetch(p2bot+16+x*2+ 0,0,2);
			
			*ptrYt0 = (p2top[ 1]<<24) + (p2top[ 3]<<16) + (p2top[ 5]<<8) + (p2top[ 7]);
			*ptrYt1 = (p2top[ 9]<<24) + (p2top[11]<<16) + (p2top[13]<<8) + (p2top[15]);
			*ptrYb0 = (p2bot[ 1]<<24) + (p2bot[ 3]<<16) + (p2bot[ 5]<<8) + (p2bot[ 7]);
			*ptrYb1 = (p2bot[ 9]<<24) + (p2bot[11]<<16) + (p2bot[13]<<8) + (p2bot[15]);
			
			*ptrU =	  (((p2top[ 0]+p2bot[ 0])/2)<<24) + (((p2top[ 4]+p2bot[ 4])/2)<<16)
					+ (((p2top[ 8]+p2bot[ 8])/2)<< 8) + (((p2top[12]+p2bot[12])/2));
			*ptrV =	  (((p2top[ 2]+p2bot[ 2])/2)<<24) + (((p2top[ 6]+p2bot[ 6])/2)<<16)
					+ (((p2top[10]+p2bot[10])/2)<< 8) + (((p2top[14]+p2bot[14])/2));
		#endif
		#if defined(__LITTLE_ENDIAN__)
			*ptrYt0 = (p2top[ 7]<<24) + (p2top[ 5]<<16) + (p2top[ 3]<<8) + (p2top[ 1]);
			*ptrYt1 = (p2top[15]<<24) + (p2top[13]<<16) + (p2top[11]<<8) + (p2top[ 9]);
			*ptrYb0 = (p2bot[ 7]<<24) + (p2bot[ 5]<<16) + (p2bot[ 3]<<8) + (p2bot[ 1]);
			*ptrYb1 = (p2bot[15]<<24) + (p2bot[13]<<16) + (p2bot[11]<<8) + (p2bot[ 9]);
			
			*ptrU =	  (((p2top[12]+p2bot[12])/2)<<24) + (((p2top[ 8]+p2bot[ 8])/2)<<16)
					+ (((p2top[ 4]+p2bot[ 4])/2)<< 8) + (((p2top[ 0]+p2bot[ 0])/2));
			*ptrV =	  (((p2top[14]+p2bot[14])/2)<<24) + (((p2top[10]+p2bot[10])/2)<<16)
					+ (((p2top[ 6]+p2bot[ 6])/2)<< 8) + (((p2top[ 2]+p2bot[ 2])/2));
		#endif
			p2top += 16;
			p2bot += 16;
			pytop += 8;
			pybot += 8;
			pu += 4;
			pv += 4;
		}	// for(x <= width-8)
#endif	//ENABLEFASTER
		
		for (; x <= width-2; x += 2) {
			pytop[0] = p2top[1];
			pytop[1] = p2top[3];
			pybot[0] = p2bot[1];
			pybot[1] = p2bot[3];
			
			pu[0] = ( p2top[0] + p2bot[0] ) / 2;
			pv[0] = ( p2top[2] + p2bot[2] ) / 2;
			
			/* Advance to the next 2x2 block of pixels. */
			p2top += 4;
			p2bot += 4;
			pytop += 2;
			pybot += 2;
			pu += 1;
			pv += 1;
		}	// for(x <= width-2)
		
	#if USESIMD	/* memalign hack */
ENDOFROW:
		if( hackytop )	memcpy((  y) * rowBytes_y + (UInt8*)baseAddr_y, temp_yt_aligned, rowBytes_y);
		if( hackybot )	memcpy((1+y) * rowBytes_y + (UInt8*)baseAddr_y, temp_yb_aligned, rowBytes_y);
		if( hacku )		memcpy((y/2) * rowBytes_u + (UInt8*)baseAddr_u, temp_u_aligned, rowBytes_u);
		if( hackv )		memcpy((y/2) * rowBytes_v + (UInt8*)baseAddr_v, temp_v_aligned, rowBytes_v);
	#endif
		
#if ENABLEFASTER
	#if USESIMD
		#if defined(__BIG_ENDIAN__)
		vec_dss(0);
		vec_dss(1);
		#endif	//defined(__BIG_ENDIAN__)
		#if defined(__LITTLE_ENDIAN__)
		#endif	//defined(__LITTLE_ENDIAN__)
	#endif	//USESIMD
#endif	//ENABLEFASTER
	}	// for(y < height)

#if USESIMD	/* memalign hack */
	free(temp_yt_aligned);
	free(temp_yb_aligned);
	free(temp_u_aligned);
	free(temp_v_aligned);
#endif
	
	}	/*end of parallel region*/
	
#if BENCHMARK
	} clock_t end = clock(); fprintf(stderr, "diff:%d\n", (end-start)); }
#endif
	
}


#pragma mark -
#pragma mark Convert PixelBuffer (Interlaced Support)


// Util to convert chunky yuv422 into planer YUV420
// For bitmap transfer : CVPixelBuffer -> AVFrame.
// Support Interlaced Xfer (mode; 0:prog, 1:Weight3-1, 2:Weight5:3)
void copy_2vuy_to_planar_YUV420_i(size_t width, size_t height, 
									   const UInt8 *baseAddr_2vuy, size_t rowBytes_2vuy,
									   UInt8 *baseAddr_y, size_t rowBytes_y, 
									   UInt8 *baseAddr_u, size_t rowBytes_u, 
									   UInt8 *baseAddr_v, size_t rowBytes_v, int mode)
{
	assert( !(width & 0x1) );	/* At least x should be even value */
//	assert( !(width & 0x1 || height & 0x1) );	/* At least both x and y should be even value */
	
#if USESIMD
	#if defined(__BIG_ENDIAN__)
		// Permute index
		register vUInt8 indexUV = { 1, 5, 9,13,17,21,25,29, 3, 7,11,15,19,23,27,31};	/* !! */
		register vUInt8 indexY =  { 1, 3, 5, 7, 9,11,13,15,17,19,21,23,25,27,29,31};
		register vUInt8 indexHI = { 0, 1, 2, 3, 4, 5, 6, 7,16,17,18,19,20,21,22,23};
		register vUInt8 indexLO = { 8, 9,10,11,12,13,14,15,24,25,26,27,28,29,30,31};
		register vUInt8 multiL, multiS;
		register vUInt16 shift;
		switch (mode) {
		case 2:
			multiL = vec_splat_u8(5); multiS = vec_splat_u8(3); shift = vec_splat_u16(3); 
			break;	// mpeg2 spec
		case 1:
			multiL = vec_splat_u8(3); multiS = vec_splat_u8(1); shift = vec_splat_u16(2); 
			break;	// Avisynth method
		default:
			multiL = vec_splat_u8(1); multiS = vec_splat_u8(1); shift = vec_splat_u16(1); 
			break;	// progressive
		}
	#endif
	#if defined(__LITTLE_ENDIAN__)
		vUInt16 maskL = {0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF};
		vUInt32 mask8 = {0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF};
		vUInt8 multiL, multiS;
		int shift;
		switch (mode) {
		case 2:
			multiL = _mm_set1_epi16(0x0005); multiS = _mm_set1_epi16(0x0003); shift = 3; 
			break;	// mpeg2 spec
		case 1:
			multiL = _mm_set1_epi16(0x0003); multiS = _mm_set1_epi16(0x0001); shift = 2; 
			break;	// Avisynth method
		default:
			multiL = _mm_set1_epi16(0x0001); multiS = _mm_set1_epi16(0x0001); shift = 1; 
			break;	// progressive
		}
	#endif
#endif
	
	int n1, n2, d;						// Define weight of each lines
	switch (mode) {
		case 2:		n1 = 5; n2 = 3; d = 3; break;	// mpeg2 spec
		case 1:		n1 = 3; n2 = 1; d = 2; break;	// Avisynth method
		case 0:		n1 = 1; n2 = 1; d = 1; break;	// progressive
		default:	n1 = 1; n2 = 1; d = 1; break;	// progressive
	}

#if BENCHMARK
	{clock_t start = clock(); int i ; for(i=0;i<10000;i++) {
#endif
	
#pragma omp parallel
	{	/*start of parallel region*/
	
#if USESIMD	/* memalign hack */
	UInt8* temp_yt_aligned = malloc(roundUpToMultipleOf16(rowBytes_y));
	UInt8* temp_yb_aligned = malloc(roundUpToMultipleOf16(rowBytes_y));
	UInt8* temp_u_aligned  = malloc(roundUpToMultipleOf16(rowBytes_u));
	UInt8* temp_v_aligned  = malloc(roundUpToMultipleOf16(rowBytes_v));
#endif
	
	int y;
	
#pragma omp for nowait
	for (y = 0; y < height; y += 2) {		// allow overshoot on odd value
//	for (y = 0; y <= height-2; y += 2) {	// left bottom line on odd value
		int parity = (y/2) & 1;				// Even or Odd line in uv plane
		
		int x=0;
		UInt8 *p2top, *p2bot, *pytop, *pybot, *pu, *pv, *p2topN, *p2botP;
		p2top = (  y) * rowBytes_2vuy + (UInt8*)baseAddr_2vuy;
		p2bot = (1+y) * rowBytes_2vuy + (UInt8*)baseAddr_2vuy;
		p2topN= (2+y) * rowBytes_2vuy + (UInt8*)baseAddr_2vuy;
		p2botP= (y-1) * rowBytes_2vuy + (UInt8*)baseAddr_2vuy;
		pytop = (  y) * rowBytes_y + (UInt8*)baseAddr_y;
		pybot = (1+y) * rowBytes_y + (UInt8*)baseAddr_y;
		pu =	(y/2) * rowBytes_u + (UInt8*)baseAddr_u;
		pv =	(y/2) * rowBytes_v + (UInt8*)baseAddr_v;
		
		if((1+y) >= height) p2bot = p2bot - 2 * rowBytes_2vuy;
		if((2+y) >= height)	p2topN = p2top;		/* same field; not picture's but field's bottom line */
		if((y-1) < 0)		p2botP = p2bot;
		
	#if USESIMD	/* memalign hack */
		bool hackytop = FALSE, hackybot = FALSE, hacku = FALSE, hackv = FALSE;
		if( (UInt32)pytop & 0xF ) { hackytop = TRUE; pytop = temp_yt_aligned; }
		if( (UInt32)pybot & 0xF ) { hackybot = TRUE; pybot = temp_yb_aligned; }
		if( (UInt32)pu    & 0xF ) { hacku    = TRUE; pu    = temp_u_aligned; }
		if( (UInt32)pv    & 0xF ) { hackv    = TRUE; pv    = temp_v_aligned; }
	#endif
		
#if ENABLEFASTER
	#if USESIMD
		#if defined(__BIG_ENDIAN__)
		vec_dst(p2top, (4<<24)|(rowBytes_2vuy/64)<<16|64, 0);
		vec_dst(p2bot, (4<<24)|(rowBytes_2vuy/64)<<16|64, 1);
		vec_dst(p2topN, (4<<24)|(rowBytes_2vuy/64)<<16|64, 2);
		vec_dst(p2botP, (4<<24)|(rowBytes_2vuy/64)<<16|64, 3);
		#endif	//defined(__BIG_ENDIAN__)
		#if defined(__LITTLE_ENDIAN__)
		// no prefetch applied for intel
		#endif	//defined(__LITTLE_ENDIAN__)
	#endif	//USESIMD
#endif	//ENABLEFASTER

#if ENABLEFASTER
	#if USESIMD
		for (x = 0; x <= width-32; x += 32) {
		#if defined(__BIG_ENDIAN__)
			vUInt8 yuvt0 = vec_ld( 0+x*2, p2top), yuvt1 = vec_ld(16+x*2, p2top);
			vUInt8 yuvt2 = vec_ld(32+x*2, p2top), yuvt3 = vec_ld(48+x*2, p2top);
			vUInt8 yuvb0 = vec_ld( 0+x*2, p2bot), yuvb1 = vec_ld(16+x*2, p2bot);
			vUInt8 yuvb2 = vec_ld(32+x*2, p2bot), yuvb3 = vec_ld(48+x*2, p2bot);
			
			vec_st(vec_perm(yuvt0, yuvt1, indexY),  0+x, pytop);
			vec_st(vec_perm(yuvt2, yuvt3, indexY), 16+x, pytop);
			vec_st(vec_perm(yuvb0, yuvb1, indexY),  0+x, pybot);
			vec_st(vec_perm(yuvb2, yuvb3, indexY), 16+x, pybot);
			
			if(!parity) {			// uv even line; use top & next top
				vUInt8 yuvN0 = vec_ld( 0+x*2, p2topN), yuvN1 = vec_ld(16+x*2, p2topN);
				vUInt8 yuvN2 = vec_ld(32+x*2, p2topN), yuvN3 = vec_ld(48+x*2, p2topN);
				
				vUInt16 uv0 = vec_mule(yuvt0, multiL), uv1 = vec_mule(yuvt1, multiL);
				vUInt16 uv2 = vec_mule(yuvt2, multiL), uv3 = vec_mule(yuvt3, multiL);
				vUInt16 uvN0 = vec_mule(yuvN0, multiS), uvN1 = vec_mule(yuvN1, multiS);
				vUInt16 uvN2 = vec_mule(yuvN2, multiS), uvN3 = vec_mule(yuvN3, multiS);
				
				vUInt16 uvW0 = vec_sr(vec_add(uv0, uvN0), shift);	/* [_u_v _u_v _u_v _u_v] */
				vUInt16 uvW1 = vec_sr(vec_add(uv1, uvN1), shift);
				vUInt16 uvW2 = vec_sr(vec_add(uv2, uvN2), shift);
				vUInt16 uvW3 = vec_sr(vec_add(uv3, uvN3), shift);
				
				vUInt8 uvW01 = vec_perm(uvW0, uvW1, indexUV);	/* [uuuu uuuu vvvv vvvv] */
				vUInt8 uvW23 = vec_perm(uvW2, uvW3, indexUV);
				
				vec_st(vec_perm(uvW01, uvW23, indexHI),  0+x/2, pu);
				vec_st(vec_perm(uvW01, uvW23, indexLO),  0+x/2, pv);
			} else {				// uv odd line; use bot & prev bot
				vUInt8 yuvP0 = vec_ld( 0+x*2, p2botP), yuvP1 = vec_ld(16+x*2, p2botP);
				vUInt8 yuvP2 = vec_ld(32+x*2, p2botP), yuvP3 = vec_ld(48+x*2, p2botP);
				
				vUInt16 uv0 = vec_mule(yuvb0, multiL), uv1 = vec_mule(yuvb1, multiL);
				vUInt16 uv2 = vec_mule(yuvb2, multiL), uv3 = vec_mule(yuvb3, multiL);
				vUInt16 uvP0 = vec_mule(yuvP0, multiS), uvP1 = vec_mule(yuvP1, multiS);
				vUInt16 uvP2 = vec_mule(yuvP2, multiS), uvP3 = vec_mule(yuvP3, multiS);
				
				vUInt16 uvW0 = vec_sr(vec_add(uv0, uvP0), shift);	/* [_u_v _u_v _u_v _u_v] */
				vUInt16 uvW1 = vec_sr(vec_add(uv1, uvP1), shift);
				vUInt16 uvW2 = vec_sr(vec_add(uv2, uvP2), shift);
				vUInt16 uvW3 = vec_sr(vec_add(uv3, uvP3), shift);
				
				vUInt8 uvW01 = vec_perm(uvW0, uvW1, indexUV);	/* [uuuu uuuu vvvv vvvv] */
				vUInt8 uvW23 = vec_perm(uvW2, uvW3, indexUV);
				
				vec_st(vec_perm(uvW01, uvW23, indexHI),  0+x/2, pu);
				vec_st(vec_perm(uvW01, uvW23, indexLO),  0+x/2, pv);
			}
		#endif	//defined(__BIG_ENDIAN__)
		#if defined(__LITTLE_ENDIAN__)
			vUInt8 yuvt0 = _mm_load_si128( (__m128i*)( 0+x*2+p2top) ), yuvt1 = _mm_load_si128( (__m128i*)(16+x*2+p2top) );
			vUInt8 yuvt2 = _mm_load_si128( (__m128i*)(32+x*2+p2top) ), yuvt3 = _mm_load_si128( (__m128i*)(48+x*2+p2top) );
			vUInt8 yuvb0 = _mm_load_si128( (__m128i*)( 0+x*2+p2bot) ), yuvb1 = _mm_load_si128( (__m128i*)(16+x*2+p2bot) );
			vUInt8 yuvb2 = _mm_load_si128( (__m128i*)(32+x*2+p2bot) ), yuvb3 = _mm_load_si128( (__m128i*)(48+x*2+p2bot) );
			{
				vUInt8 yt0 = _mm_packus_epi16(	_mm_and_si128(maskL, _mm_srli_si128(yuvt0, 1)), 
												_mm_and_si128(maskL, _mm_srli_si128(yuvt1, 1)) );
				vUInt8 yt1 = _mm_packus_epi16(	_mm_and_si128(maskL, _mm_srli_si128(yuvt2, 1)), 
												_mm_and_si128(maskL, _mm_srli_si128(yuvt3, 1)) );
				vUInt8 yb0 = _mm_packus_epi16(	_mm_and_si128(maskL, _mm_srli_si128(yuvb0, 1)), 
												_mm_and_si128(maskL, _mm_srli_si128(yuvb1, 1)) );
				vUInt8 yb1 = _mm_packus_epi16(	_mm_and_si128(maskL, _mm_srli_si128(yuvb2, 1)), 
												_mm_and_si128(maskL, _mm_srli_si128(yuvb3, 1)) );
				
				_mm_store_si128( (__m128i*)( 0+x+pytop), (__m128i)yt0 );
				_mm_store_si128( (__m128i*)(16+x+pytop), (__m128i)yt1 );
				_mm_store_si128( (__m128i*)( 0+x+pybot), (__m128i)yb0 );
				_mm_store_si128( (__m128i*)(16+x+pybot), (__m128i)yb1 );
			}
			if(!parity) {			// uv even line; use top & next top
				vUInt8 yuvN0 = _mm_load_si128( (__m128i*)( 0+x*2+p2topN) ), yuvN1 = _mm_load_si128( (__m128i*)(16+x*2+p2topN) );
				vUInt8 yuvN2 = _mm_load_si128( (__m128i*)(32+x*2+p2topN) ), yuvN3 = _mm_load_si128( (__m128i*)(48+x*2+p2topN) );
				
				{
					vUInt16 ut01 = _mm_packus_epi16(	_mm_and_si128( mask8, yuvt0 ), 
														_mm_and_si128( mask8, yuvt1 )	);
					vUInt16 ut23 = _mm_packus_epi16(	_mm_and_si128( mask8, yuvt2 ), 
														_mm_and_si128( mask8, yuvt3 )	);
					vUInt16 utN01 = _mm_packus_epi16(	_mm_and_si128( mask8, yuvN0 ), 
														_mm_and_si128( mask8, yuvN1 )	);
					vUInt16 utN23 = _mm_packus_epi16(	_mm_and_si128( mask8, yuvN2 ), 
														_mm_and_si128( mask8, yuvN3 )	);
					ut01 = _mm_mullo_epi16(multiL, ut01); 
					ut23 = _mm_mullo_epi16(multiL, ut23); 
					utN01 = _mm_mullo_epi16(multiS, utN01); 
					utN23 = _mm_mullo_epi16(multiS, utN23); 
					vUInt16 uW01 = _mm_srli_epi16(_mm_adds_epu16(ut01, utN01), shift);
					vUInt16 uW23 = _mm_srli_epi16(_mm_adds_epu16(ut23, utN23), shift);
					
					_mm_store_si128( (__m128i*)(x/2+pu), (__m128i)_mm_packus_epi16(uW01, uW23));
				}{
					vUInt16 vt01 = _mm_packus_epi16(	_mm_and_si128( mask8, _mm_srli_si128(yuvt0, 2) ), 
														_mm_and_si128( mask8, _mm_srli_si128(yuvt1, 2) )	);
					vUInt16 vt23 = _mm_packus_epi16(	_mm_and_si128( mask8, _mm_srli_si128(yuvt2, 2) ), 
														_mm_and_si128( mask8, _mm_srli_si128(yuvt3, 2) )	);
					vUInt16 vtN01 = _mm_packus_epi16(	_mm_and_si128( mask8, _mm_srli_si128(yuvN0, 2) ), 
														_mm_and_si128( mask8, _mm_srli_si128(yuvN1, 2) )	);
					vUInt16 vtN23 = _mm_packus_epi16(	_mm_and_si128( mask8, _mm_srli_si128(yuvN2, 2) ), 
														_mm_and_si128( mask8, _mm_srli_si128(yuvN3, 2) )	);
					vt01 = _mm_mullo_epi16(multiL, vt01); 
					vt23 = _mm_mullo_epi16(multiL, vt23); 
					vtN01 = _mm_mullo_epi16(multiS, vtN01); 
					vtN23 = _mm_mullo_epi16(multiS, vtN23); 
					vUInt16 vW01 = _mm_srli_epi16(_mm_adds_epu16(vt01, vtN01), shift);
					vUInt16 vW23 = _mm_srli_epi16(_mm_adds_epu16(vt23, vtN23), shift);
					
					_mm_store_si128( (__m128i*)(x/2+pv), (__m128i)_mm_packus_epi16(vW01, vW23));
				}
			} else {				// uv odd line; use bot & prev bot
				vUInt8 yuvP0 = _mm_load_si128( (__m128i*)( 0+x*2+p2botP) ), yuvP1 = _mm_load_si128( (__m128i*)(16+x*2+p2botP) );
				vUInt8 yuvP2 = _mm_load_si128( (__m128i*)(32+x*2+p2botP) ), yuvP3 = _mm_load_si128( (__m128i*)(48+x*2+p2botP) );
				
				{
					vUInt16 ub01 = _mm_packus_epi16(	_mm_and_si128( mask8, yuvb0 ), 
														_mm_and_si128( mask8, yuvb1 )	);
					vUInt16 ub23 = _mm_packus_epi16(	_mm_and_si128( mask8, yuvb2 ), 
														_mm_and_si128( mask8, yuvb3 )	);
					vUInt16 ubP01 = _mm_packus_epi16(	_mm_and_si128( mask8, yuvP0 ), 
														_mm_and_si128( mask8, yuvP1 )	);
					vUInt16 ubP23 = _mm_packus_epi16(	_mm_and_si128( mask8, yuvP2 ), 
														_mm_and_si128( mask8, yuvP3 )	);
					ub01 = _mm_mullo_epi16(multiL, ub01); 
					ub23 = _mm_mullo_epi16(multiL, ub23); 
					ubP01 = _mm_mullo_epi16(multiS, ubP01); 
					ubP23 = _mm_mullo_epi16(multiS, ubP23); 
					vUInt16 uW01 = _mm_srli_epi16(_mm_adds_epu16(ub01, ubP01), shift);
					vUInt16 uW23 = _mm_srli_epi16(_mm_adds_epu16(ub23, ubP23), shift);
					
					_mm_store_si128( (__m128i*)(x/2+pu), (__m128i)_mm_packus_epi16(uW01, uW23));
				}{
					vUInt16 vb01 = _mm_packus_epi16(	_mm_and_si128( mask8, _mm_srli_si128(yuvb0, 2) ), 
														_mm_and_si128( mask8, _mm_srli_si128(yuvb1, 2) )	);
					vUInt16 vb23 = _mm_packus_epi16(	_mm_and_si128( mask8, _mm_srli_si128(yuvb2, 2) ), 
														_mm_and_si128( mask8, _mm_srli_si128(yuvb3, 2) )	);
					vUInt16 vbP01 = _mm_packus_epi16(	_mm_and_si128( mask8, _mm_srli_si128(yuvP0, 2) ), 
														_mm_and_si128( mask8, _mm_srli_si128(yuvP1, 2) )	);
					vUInt16 vbP23 = _mm_packus_epi16(	_mm_and_si128( mask8, _mm_srli_si128(yuvP2, 2) ), 
														_mm_and_si128( mask8, _mm_srli_si128(yuvP3, 2) )	);
					vb01 = _mm_mullo_epi16(vb01, multiL); 
					vb23 = _mm_mullo_epi16(vb23, multiL); 
					vbP01 = _mm_mullo_epi16(vbP01, multiS); 
					vbP23 = _mm_mullo_epi16(vbP23, multiS); 
					vUInt16 vW01 = _mm_srli_epi16(_mm_adds_epu16(vb01, vbP01), shift);
					vUInt16 vW23 = _mm_srli_epi16(_mm_adds_epu16(vb23, vbP23), shift);
					
					_mm_store_si128((__m128i*)(x/2+pv), (__m128i)_mm_packus_epi16(vW01, vW23));
				}
			}
		#endif	//defined(__LITTLE_ENDIAN__)
		}	// for(x <= width-32)
		
		if( x == width ) goto ENDOFROW;
		
		pytop += x;
		pybot += x;
		pu += x/2;
		pv += x/2;
		p2top += x*2;
		p2bot += x*2;
		p2topN += x*2;
		p2botP += x*2;
	#endif	//USESIMD
		for (; x <= width-8; x += 8) {
			UInt32 *ptrYt0 = (UInt32*)&pytop[0], *ptrYt1 = (UInt32*)&pytop[4];
			UInt32 *ptrYb0 = (UInt32*)&pybot[0], *ptrYb1 = (UInt32*)&pybot[4];
			UInt32 *ptrU = (UInt32*)&pu[0],*ptrV = (UInt32*)&pv[0];
		#if defined(__BIG_ENDIAN__)
			__builtin_prefetch(p2top+16+x*2+ 0,0,2);
			__builtin_prefetch(p2bot+16+x*2+ 0,0,2);
			
			*ptrYt0 = (p2top[ 1]<<24) + (p2top[ 3]<<16) + (p2top[ 5]<<8) + (p2top[ 7]);
			*ptrYt1 = (p2top[ 9]<<24) + (p2top[11]<<16) + (p2top[13]<<8) + (p2top[15]);
			*ptrYb0 = (p2bot[ 1]<<24) + (p2bot[ 3]<<16) + (p2bot[ 5]<<8) + (p2bot[ 7]);
			*ptrYb1 = (p2bot[ 9]<<24) + (p2bot[11]<<16) + (p2bot[13]<<8) + (p2bot[15]);
			
			if( !parity ) {						// uv even line; use top & next top
				__builtin_prefetch(p2topN+16+x*2+ 0,0,2);
				
				*ptrU =   (((p2top[ 0]*n1+p2topN[ 0]*n2)>>d)<<24) + (((p2top[ 4]*n1+p2topN[ 4]*n2)>>d)<<16)
						+ (((p2top[ 8]*n1+p2topN[ 8]*n2)>>d)<< 8) + (((p2top[12]*n1+p2topN[12]*n2)>>d));
				*ptrV =   (((p2top[ 2]*n1+p2topN[ 2]*n2)>>d)<<24) + (((p2top[ 6]*n1+p2topN[ 6]*n2)>>d)<<16)
						+ (((p2top[10]*n1+p2topN[10]*n2)>>d)<< 8) + (((p2top[14]*n1+p2topN[14]*n2)>>d));
			} else {							// uv odd line; use bot & prev bot
				__builtin_prefetch(p2botP+16+x*2+ 0,0,2);
				
				*ptrU =   (((p2bot[ 0]*n1+p2botP[ 0]*n2)>>d)<<24) + (((p2bot[ 4]*n1+p2botP[ 4]*n2)>>d)<<16)
						+ (((p2bot[ 8]*n1+p2botP[ 8]*n2)>>d)<< 8) + (((p2bot[12]*n1+p2botP[12]*n2)>>d));
				*ptrV =   (((p2bot[ 2]*n1+p2botP[ 2]*n2)>>d)<<24) + (((p2bot[ 6]*n1+p2botP[ 6]*n2)>>d)<<16)
						+ (((p2bot[10]*n1+p2botP[10]*n2)>>d)<< 8) + (((p2bot[14]*n1+p2botP[14]*n2)>>d));
			}
		#endif
		#if defined(__LITTLE_ENDIAN__)
			*ptrYt0 = (p2top[ 7]<<24) + (p2top[ 5]<<16) + (p2top[ 3]<<8) + (p2top[ 1]);
			*ptrYt1 = (p2top[15]<<24) + (p2top[13]<<16) + (p2top[11]<<8) + (p2top[ 9]);
			*ptrYb0 = (p2bot[ 7]<<24) + (p2bot[ 5]<<16) + (p2bot[ 3]<<8) + (p2bot[ 1]);
			*ptrYb1 = (p2bot[15]<<24) + (p2bot[13]<<16) + (p2bot[11]<<8) + (p2bot[ 9]);
			
			if( !parity ) {						// uv even line; use top & next top
				*ptrU =   (((p2top[12]*n1+p2topN[12]*n2)>>d)<<24) + (((p2top[ 8]*n1+p2topN[ 8]*n2)>>d)<<16)
						+ (((p2top[ 4]*n1+p2topN[ 4]*n2)>>d)<< 8) + (((p2top[ 0]*n1+p2topN[ 0]*n2)>>d));
				*ptrV =   (((p2top[14]*n1+p2topN[14]*n2)>>d)<<24) + (((p2top[10]*n1+p2topN[10]*n2)>>d)<<16)
						+ (((p2top[ 6]*n1+p2topN[ 6]*n2)>>d)<< 8) + (((p2top[ 2]*n1+p2topN[ 2]*n2)>>d));
			} else {							// uv odd line; use bot & prev bot
				*ptrU =   (((p2bot[12]*n1+p2botP[12]*n2)>>d)<<24) + (((p2bot[ 8]*n1+p2botP[ 8]*n2)>>d)<<16)
						+ (((p2bot[ 4]*n1+p2botP[ 4]*n2)>>d)<< 8) + (((p2bot[ 0]*n1+p2botP[ 0]*n2)>>d));
				*ptrV =   (((p2bot[14]*n1+p2botP[14]*n2)>>d)<<24) + (((p2bot[10]*n1+p2botP[10]*n2)>>d)<<16)
						+ (((p2bot[ 6]*n1+p2botP[ 6]*n2)>>d)<< 8) + (((p2bot[ 2]*n1+p2botP[ 2]*n2)>>d));
			}
		#endif
			p2top += 16;
			p2bot += 16;
			pytop += 8;
			pybot += 8;
			pu += 4;
			pv += 4;
			p2topN += 16;
			p2botP += 16;
		}	// for(x <= width-8)
#endif	//ENABLEFASTER
		
		for (; x <= width-2; x += 2) {
			pytop[0] = p2top[1];
			pytop[1] = p2top[3];
			pybot[0] = p2bot[1];
			pybot[1] = p2bot[3];
			
			if( !parity ) {
				pu[0] = ( p2top[0]*n1 + p2topN[0]*n2 ) >> d;
				pv[0] = ( p2top[2]*n1 + p2topN[2]*n2 ) >> d;
			} else {
				pu[0] = ( p2bot[0]*n1 + p2botP[0]*n2 ) >> d;
				pv[0] = ( p2bot[2]*n1 + p2botP[2]*n2 ) >> d;
			}
			
			/* Advance to the next 2x2 block of pixels. */
			p2top += 4;
			p2bot += 4;
			pytop += 2;
			pybot += 2;
			pu += 1;
			pv += 1;
			p2topN += 4;
			p2botP += 4;
		}	// for(x <= width-2)
		
	#if USESIMD	/* memalign hack */
ENDOFROW:
		if( hackytop )	memcpy((  y) * rowBytes_y + (UInt8*)baseAddr_y, temp_yt_aligned, rowBytes_y);
		if( hackybot )	memcpy((1+y) * rowBytes_y + (UInt8*)baseAddr_y, temp_yb_aligned, rowBytes_y);
		if( hacku )		memcpy((y/2) * rowBytes_u + (UInt8*)baseAddr_u, temp_u_aligned, rowBytes_u);
		if( hackv )		memcpy((y/2) * rowBytes_v + (UInt8*)baseAddr_v, temp_v_aligned, rowBytes_v);
	#endif
		
#if ENABLEFASTER
	#if USESIMD
		#if defined(__BIG_ENDIAN__)
		vec_dss(0);
		vec_dss(1);
		vec_dss(2);
		vec_dss(3);
		#endif	//defined(__BIG_ENDIAN__)
		#if defined(__LITTLE_ENDIAN__)
		#endif	//defined(__LITTLE_ENDIAN__)
	#endif	//USESIMD
#endif	//ENABLEFASTER
	}	// for(y < height)

#if USESIMD	/* memalign hack */
	free(temp_yt_aligned);
	free(temp_yb_aligned);
	free(temp_u_aligned);
	free(temp_v_aligned);
#endif
	
	}	/*end of parallel region*/
	
#if BENCHMARK
	} clock_t end = clock(); fprintf(stderr, "diff:%d\n", (end-start)); }
#endif
	
}

