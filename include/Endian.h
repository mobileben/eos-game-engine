/******************************************************************************
 *
 * File: Endian.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008 2n Productions, All Rights Reserved.
 *
 * Class used to encapsulate endianess
 * 
 *****************************************************************************/

#ifndef __ENDIAN_H__
#define __ENDIAN_H__

#include "DTypes.h"

class Endian
{
public:
	typedef enum
	{
		EndianTypeLittle = 0,
		EndianTypeBig
	} EndianType;

private:
	EndianType	_native_endian;

	EndianType	_target_endian;	//	Endian type we are dealing with

public:
	Endian()
	{
#ifdef _PLATFORM_PC

		_native_endian = _target_endian = EndianTypeLittle;	//	Yah, this is dup'd we do this so we know how to add other native platforms

#elif defined(_PLATFORM_MAC)
		
		_native_endian = _target_endian = EndianTypeLittle;	//	Yah, this is dup'd we do this so we know how to add other native platforms
		
#elif defined(_PLATFORM_IPHONE)
		
		_native_endian = _target_endian = EndianTypeLittle;	//	Yah, this is dup'd we do this so we know how to add other native platforms
		
#else

		#error	_PLATFORM not defined.

#endif /* WIN32 */
	}

	~Endian() {}

	inline EndianType	getNativeEndian(void) const {
		 return _native_endian;
	}

	inline EndianType	getTargetEndian(void) const {
		 return _target_endian;
	}

	inline Boolean willSwap(void) { return _native_endian != _target_endian; }

	inline void setTargetEndian(EndianType type) {
		_target_endian = type;
	}

	inline void switchEndian(void){
		if (_native_endian == EndianTypeLittle)
			_target_endian = EndianTypeBig;
		else
			_target_endian = EndianTypeLittle;
	}

	inline Sint16 swapSint16(Sint16 in) const
	{
		if (_target_endian != _native_endian)
		{
			return ((in & 0xFF00) >> 8) | ((in & 0x00FF) << 8);
		}
		else
			return in;
	}

	inline Sint32 swapSint32(Sint32 in) const
	{
		if (_target_endian != _native_endian)
		{
			return ((in & 0x000000FF) << 24) | ((in & 0x0000FF00) << 8) | ((in & 0x000FF0000) >> 8) | ((in & 0xFF000000) >> 24) ;
		}
		else
			return in;
	}

	inline Uint16 swapUint16(Uint16 in) const
	{
		if (_target_endian != _native_endian)
		{
			return ((in & 0xFF00) >> 8) | ((in & 0x00FF) << 8);
		}
		else
			return in;
	}

	inline Uint32 swapUint32(Uint32 in) const
	{
		if (_target_endian != _native_endian)
		{
			return ((in & 0x000000FF) << 24) | ((in & 0x0000FF00) << 8) | ((in & 0x000FF0000) >> 8) | ((in & 0xFF000000) >> 24) ;
		}
		else
			return in;
	}

	inline Float32 swapFloat32(Float32 in) const
	{
		Uint32*		data = (Uint32*) &in;
		Uint32		out;
		Float32*   	newFloat = (Float32*) &out; 

		if (_target_endian != _native_endian)
		{
			out =(((*data & 0x000000FF) << 24) | ((*data & 0x0000FF00) << 8) | ((*data & 0x000FF0000) >> 8) | ((*data & 0xFF000000) >> 24)) ;

			return *newFloat;
		}
		else
			return in;
	}

	inline Sint16 readSint16(Uint8* in) const
	{
		if (_target_endian != _native_endian)
			return (in[0] << 8) | in[1];
		else
			return in[0] | (in[1]<< 8);
	}

	inline Sint32 readSint32(Uint8* in) const
	{
		if (_target_endian != _native_endian)
			return (in[0] << 24) | (in[1] << 16) | (in[2] << 8) | in[3];
		else
			return in[0] | (in[1] << 8) | (in[2] << 16) | (in[3] << 24);
	}

	inline Uint16 readUint16(Uint8* in) const
	{
		if (_target_endian != _native_endian)
			return (in[0] << 8) | in[1];
		else
			return in[0] | (in[1]<< 8);
	}

	inline Uint32 readUint32(Uint8* in) const
	{
		if (_target_endian != _native_endian)
			return (in[0] << 24) | (in[1] << 16) | (in[2] << 8) | in[3];
		else
			return in[0] | (in[1] << 8) | (in[2] << 16) | (in[3] << 24);
	}

	inline void writeSint16(Uint8* out, Sint16 in) const
	{
		if (_target_endian != _native_endian)
		{
			out[0] = (in & 0xFF00) >> 8;
			out[1] = (in & 0x00FF);
		}
		else
		{
			out[0] = (in & 0x00FF);
			out[1] = (in & 0xFF00) >> 8;
		}
	}

	inline void writeSint32(Uint8* out, Sint32 in) const
	{
		if (_target_endian != _native_endian)
		{
			out[0] = (in & 0xFF000000) >> 24;
			out[1] = (in & 0x00FF0000) >> 16;
			out[2] = (in & 0x0000FF00) >> 8;
			out[3] = (in & 0x000000FF);
		}
		else
		{
			out[0] = (in & 0x000000FF);
			out[1] = (in & 0x0000FF00) >> 8;
			out[2] = (in & 0x00FF0000) >> 16;
			out[3] = (in & 0xFF000000) >> 24;
		}
	}

	inline void writeUint16(Uint8* out, Uint16 in) const
	{
		if (_target_endian != _native_endian)
		{
			out[0] = (in & 0xFF00) >> 8;
			out[1] = (in & 0x00FF);
		}
		else
		{
			out[0] = (in & 0x00FF);
			out[1] = (in & 0xFF00) >> 8;
		}
	}

	inline void writeUint32(Uint8* out, Uint32 in) const
	{
		if (_target_endian != _native_endian)
		{
			out[0] = (in & 0xFF000000) >> 24;
			out[1] = (in & 0x00FF0000) >> 16;
			out[2] = (in & 0x0000FF00) >> 8;
			out[3] = (in & 0x000000FF);
		}
		else
		{
			out[0] = (in & 0x000000FF);
			out[1] = (in & 0x0000FF00) >> 8;
			out[2] = (in & 0x00FF0000) >> 16;
			out[3] = (in & 0xFF000000) >> 24;
		}
	}

	inline Uint32 copyUint16(Uint16* out, Uint8* in, Uint32 num) const
	{
		Uint32	i;

		if (_target_endian != _native_endian)
		{
			for (i=0;i<num * 2;i += 2)
			{
				out[i/2] = (in[i] << 8) | in[i+1];
			}
		}
		else
		{
			for (i=0;i<num * 2;i += 2)
			{
				out[i/2] = in[i] | (in[i+1]<< 8);
			}
		}

		return num * 2;
	}

	inline Uint32 copyUint32(Uint32* out, Uint8* in, Uint32 num) const
	{
		Uint32	i;

		if (_target_endian != _native_endian)
		{
			for (i=0;i<num*4;i += 4)
			{
				out[i/4] =(in[0] << 24) | (in[1] << 16) | (in[2] << 8) | in[3];
			}
		}
		else
		{
			for (i=0;i<num*4;i += 4)
			{
				out[i/4] = in[0] | (in[1] << 8) | (in[2] << 16) | (in[3] << 24);
			}
		}

		return num * 4;
	}
};

#endif /* __ENDIAN_H__ */

