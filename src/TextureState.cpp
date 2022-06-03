/******************************************************************************
 *
 * File: TextureState.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture State
 * 
 *****************************************************************************/

#include "TextureState.h"

#ifdef _PLATFORM_IPHONE

#ifdef _USE_OPENGL

#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#endif /* _USE_OPENGL */

#endif /* _PLATFORM_IPHONE */

TextureState::TextureState()
{
	setDefaultValues();
}

TextureState& TextureState::operator=(const TextureState& rhs)
{
	Uint32	i;

	if (&rhs != this)
	{
		_borderColor = rhs._borderColor;
		_blendColor = rhs._blendColor;

		_wrapS = rhs._wrapS;
		_wrapT = rhs._wrapT;

		_minMipMapFilter = rhs._minMipMapFilter;
		_magMipMapFilter = rhs._magMipMapFilter;

		_mode = rhs._mode;

		for (i=0;i<NUM_TEXTURE_UNITS;i++)
		{
			_combineSrcRGB[i] = rhs._combineSrcRGB[i];
			_combineSrcAlpha[i] = rhs._combineSrcAlpha[i];
			_combineOpRGB[i] = rhs._combineOpRGB[i];
			_combineOpAlpha[i] = rhs._combineOpAlpha[i];
		}

		_combineRGB = rhs._combineRGB;
		_combineAlpha = rhs._combineAlpha;

		_combineScaleRGB = rhs._combineScaleRGB;
		_combineScaleAlpha = rhs._combineScaleAlpha;
	}

	return *this;
}

bool TextureState::operator==(const TextureState& rhs)
{
	bool	equal = false;

	if (_mode == rhs._mode)
	{
		if (_wrapS == rhs._wrapS && _wrapT == rhs._wrapT)
		{
			if (_borderColor.r == rhs._borderColor.r && _borderColor.g == rhs._borderColor.g && _borderColor.b == rhs._borderColor.b && _borderColor.a == rhs._borderColor.a)
			{
				if (_minMipMapFilter == rhs._minMipMapFilter && _magMipMapFilter == rhs._magMipMapFilter)
				{
					if (_mode == TEXTURE_ENV_MODE_COMBINE)
					{
						if (_combineRGB == rhs._combineRGB && _combineAlpha == rhs._combineAlpha && _combineScaleRGB == rhs._combineScaleRGB && _combineScaleAlpha == rhs._combineScaleAlpha)
						{
							Uint32	i;

							equal = true;
	
							//	Combiner relies on much more elaborate testing
							for (i=0;i<NUM_TEXTURE_UNITS;i++)
							{
								if (_combineSrcRGB[i] != rhs._combineSrcRGB[i])
								{
									equal = false;
									break;
								}

								if (_combineSrcAlpha[i] != rhs._combineSrcAlpha[i])
								{
									equal = false;
									break;
								}

								if (_combineOpRGB[i] != rhs._combineOpRGB[i])
								{
									equal = false;
									break;
								}

								if (_combineOpAlpha[i] != rhs._combineOpAlpha[i])
								{
									equal = false;
									break;
								}
							}
						}
					}
					else
						equal = true;
				}
			}
		}
	}

	return equal;
}

void TextureState::setDefaultValues(void)
{
	Uint32	i;

	_textureObj = NULL;

	_wrapS =TEXTURE_WRAP_CLAMP;
	_wrapT = TEXTURE_WRAP_CLAMP;
	_minMipMapFilter = TEXTURE_MIPMAP_NEAREST;
	_magMipMapFilter = TEXTURE_MIPMAP_NEAREST;
    _mode = TEXTURE_ENV_MODE_REPLACE;
	_combineRGB = TEXTURE_COMBINE_REPLACE;
	_combineAlpha = TEXTURE_COMBINE_REPLACE;
	_combineScaleRGB = TEXTURE_COMBINE_SCALE_ONE;
	_combineScaleAlpha = TEXTURE_COMBINE_SCALE_ONE;

	_borderColor.r = _borderColor.g = _borderColor.b = _borderColor.a = 0.0F;
	_blendColor.r = _blendColor.g = _blendColor.b = _blendColor.a = 0.0F;

	for (i=0;i<NUM_TEXTURE_UNITS;i++)
	{
		_combineSrcRGB[i] = TEXTURE_COMBINE_SRC_TEXTURE;
		_combineSrcAlpha[i] = TEXTURE_COMBINE_SRC_TEXTURE;
		_combineOpRGB[i] = TEXTURE_COMBINE_OP_SRC_COLOR;
		_combineOpAlpha[i] = TEXTURE_COMBINE_OP_SRC_COLOR;
	}
}

void TextureState::setFromStateFlags(Uint32 flags)
{
	if (flags == 0)
		return;

	if (flags & TEXTURE_STATE_ENV_REPLACE)
		_mode = TEXTURE_ENV_MODE_REPLACE;
	else if (flags & TEXTURE_STATE_ENV_MODULATE)
		_mode = TEXTURE_ENV_MODE_MODULATE;
	else if (flags & TEXTURE_STATE_ENV_DECAL)
		_mode = TEXTURE_ENV_MODE_DECAL;
	else if (flags & TEXTURE_STATE_ENV_BLEND)
		_mode = TEXTURE_ENV_MODE_BLEND;
	else if (flags & TEXTURE_STATE_ENV_ADD)
		_mode = TEXTURE_ENV_MODE_ADD;
	else if (flags & TEXTURE_STATE_ENV_COMBINE)
		_mode = TEXTURE_ENV_MODE_COMBINE;

	if (flags & TEXTURE_STATE_WRAP_S_CLAMP)
		_wrapS = TEXTURE_WRAP_CLAMP;
	else if (flags & TEXTURE_STATE_WRAP_S_REPEAT)
		_wrapS = TEXTURE_WRAP_REPEAT;
	else if (flags & TEXTURE_STATE_WRAP_S_CLAMP_BORDER)
		_wrapS = TEXTURE_WRAP_CLAMP_BORDER;
	else if (flags & TEXTURE_STATE_WRAP_S_CLAMP_EDGE)
		_wrapS = TEXTURE_WRAP_CLAMP_EDGE;

	if (flags & TEXTURE_STATE_WRAP_T_CLAMP)
		_wrapT = TEXTURE_WRAP_CLAMP;
	else if (flags & TEXTURE_STATE_WRAP_T_REPEAT)
		_wrapT = TEXTURE_WRAP_REPEAT;
	else if (flags & TEXTURE_STATE_WRAP_T_CLAMP_BORDER)
		_wrapT = TEXTURE_WRAP_CLAMP_BORDER;
	else if (flags & TEXTURE_STATE_WRAP_T_CLAMP_EDGE)
		_wrapT = TEXTURE_WRAP_CLAMP_EDGE;

	if (flags & TEXTURE_STATE_MIN_MIPMAP_FILTER_NEAREST)
		_minMipMapFilter = TEXTURE_MIPMAP_NEAREST;
	else if (flags & TEXTURE_STATE_MIN_MIPMAP_FILTER_LINEAR)
		_minMipMapFilter = TEXTURE_MIPMAP_LINEAR;
	else if (flags & TEXTURE_STATE_MIN_MIPMAP_FILTER_NEAREST_NEAREST)
		_minMipMapFilter = TEXTURE_MIPMAP_NEAREST_NEAREST;
	else if (flags & TEXTURE_STATE_MIN_MIPMAP_FILTER_NEAREST_LINEAR)
		_minMipMapFilter = TEXTURE_MIPMAP_NEAREST_LINEAR;
	else if (flags & TEXTURE_STATE_MIN_MIPMAP_FILTER_LINEAR_NEAREST)
		_minMipMapFilter = TEXTURE_MIPMAP_LINEAR_NEAREST;
	else if (flags & TEXTURE_STATE_MIN_MIPMAP_FILTER_LINEAR_LINEAR)
		_minMipMapFilter = TEXTURE_MIPMAP_LINEAR_LINEAR;

	if (flags & TEXTURE_STATE_MAG_MIPMAP_FILTER_NEAREST)
		_magMipMapFilter = TEXTURE_MIPMAP_NEAREST;
	else if (flags & TEXTURE_STATE_MAG_MIPMAP_FILTER_LINEAR)
		_magMipMapFilter = TEXTURE_MIPMAP_LINEAR;
	else if (flags & TEXTURE_STATE_MAG_MIPMAP_FILTER_NEAREST_NEAREST)
		_magMipMapFilter = TEXTURE_MIPMAP_NEAREST_NEAREST;
	else if (flags & TEXTURE_STATE_MAG_MIPMAP_FILTER_NEAREST_LINEAR)
		_magMipMapFilter = TEXTURE_MIPMAP_NEAREST_LINEAR;
	else if (flags & TEXTURE_STATE_MAG_MIPMAP_FILTER_LINEAR_NEAREST)
		_magMipMapFilter = TEXTURE_MIPMAP_LINEAR_NEAREST;
	else if (flags & TEXTURE_STATE_MAG_MIPMAP_FILTER_LINEAR_LINEAR)
		_magMipMapFilter = TEXTURE_MIPMAP_LINEAR_LINEAR;
}

void TextureState::apply(void)
{
#if 0
	if (_textureObj)
	{
		glEnable(GL_TEXTURE_2D);

	}
	else
		glDisable(GL_TEXTURE_2D);
#endif
}

void TextureState::bindToTexture(Texture* tex)
{
	_textureObj = tex;

#ifdef _USE_OPENGL
	GLenum	val = 0;
	
	
	glBindTexture(tex->_glTarget, tex->_hwID);

	switch (_wrapS)
	{
		case TEXTURE_WRAP_CLAMP:
#ifdef _USE_OPENGL_ES_1_1
			val = GL_CLAMP_TO_EDGE;
			
#else
			
			val = GL_CLAMP;
			
#endif /* _USE_OPENGL_ES_1_1 */
			break;

		case TEXTURE_WRAP_REPEAT:
			val = GL_REPEAT;
			break;

		case TEXTURE_WRAP_CLAMP_BORDER:
			AssertWDesc(1 == 0, "TextureState::bindToTexture() unsupported wrap");
			break;

		case TEXTURE_WRAP_CLAMP_EDGE:
#ifdef _PLATFORM_IPHONE
			
			val = GL_CLAMP_TO_EDGE;

#else
			
			AssertWDesc(1 == 0, "TextureState::bindToTexture() unsupported wrap");
			
#endif /* _PLATFORM_IPHONE */
			break;

		default:
			AssertWDesc(1 == 0, "TextureState::bindToTexture() unsupported wrap");
			break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, val);

	val = 0;

	switch (_wrapT)
	{
		case TEXTURE_WRAP_CLAMP:
#ifdef _USE_OPENGL_ES_1_1
			
			val = GL_CLAMP_TO_EDGE;
			
#else
			
			val = GL_CLAMP;
			
#endif /* _USE_OPENGL_ES_1_1 */
			break;

		case TEXTURE_WRAP_REPEAT:
			val = GL_REPEAT;
			break;

		case TEXTURE_WRAP_CLAMP_BORDER:
			AssertWDesc(1 == 0, "TextureState::bindToTexture() unsupported wrap");
			break;

		case TEXTURE_WRAP_CLAMP_EDGE:
#ifdef _PLATFORM_IPHONE
			
			val = GL_CLAMP_TO_EDGE;
			
#else
			
			AssertWDesc(1 == 0, "TextureState::bindToTexture() unsupported wrap");
			
#endif /* _PLATFORM_IPHONE */
			break;

		default:
			AssertWDesc(1 == 0, "TextureState::bindToTexture() unsupported wrap");
			break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, val);

	val = 0;

	switch (_minMipMapFilter)
	{
		case TEXTURE_MIPMAP_NEAREST:
			val = GL_NEAREST;
			break;

		case TEXTURE_MIPMAP_LINEAR:
			val = GL_LINEAR;
			break;

		case TEXTURE_MIPMAP_NEAREST_NEAREST:
			val = GL_NEAREST_MIPMAP_NEAREST;
			break;

		case TEXTURE_MIPMAP_NEAREST_LINEAR:
			val = GL_NEAREST_MIPMAP_LINEAR;
			break;

		case TEXTURE_MIPMAP_LINEAR_NEAREST:
			val = GL_LINEAR_MIPMAP_NEAREST;
			break;

		case TEXTURE_MIPMAP_LINEAR_LINEAR:
			val = GL_LINEAR_MIPMAP_LINEAR;
			break;

		default:
			AssertWDesc(1 == 0, "TextureState::bindToTexture() unsupported mipmap");
			break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, val);

	val = 0;

	switch (_magMipMapFilter)
	{
		case TEXTURE_MIPMAP_NEAREST:
			val = GL_NEAREST;
			break;

		case TEXTURE_MIPMAP_LINEAR:
			val = GL_LINEAR;
			break;

		case TEXTURE_MIPMAP_NEAREST_NEAREST:
			val = GL_NEAREST_MIPMAP_NEAREST;
			break;

		case TEXTURE_MIPMAP_NEAREST_LINEAR:
			val = GL_NEAREST_MIPMAP_LINEAR;
			break;

		case TEXTURE_MIPMAP_LINEAR_NEAREST:
			val = GL_LINEAR_MIPMAP_NEAREST;
			break;

		case TEXTURE_MIPMAP_LINEAR_LINEAR:
			val = GL_LINEAR_MIPMAP_LINEAR;
			break;

		default:
			AssertWDesc(1 == 0, "TextureState::bindToTexture() unsupported mipmap");
			break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, val);
	
#endif /*_USE_OPENGL */

	tex->bindTextureState(this);
}

#ifdef _USE_OPENGL
GLenum TextureState::TEXTURE_ENV_MODE_To_GL_TEXTURE_ENV(TEXTURE_ENV_MODE mode)
{
	GLenum val = GL_REPLACE;

	switch (mode)
	{
		case TEXTURE_ENV_MODE_REPLACE:
			val = GL_REPLACE;
			break;

		case TEXTURE_ENV_MODE_MODULATE:
			val = GL_MODULATE;
			break;

		case TEXTURE_ENV_MODE_DECAL:
			val = GL_DECAL;
			break;

		case TEXTURE_ENV_MODE_BLEND:
			val = GL_BLEND;
			break;

		case TEXTURE_ENV_MODE_ADD:
			val = GL_ADD;
			break;

		case TEXTURE_ENV_MODE_COMBINE:
			val = GL_COMBINE;
			break;

		default:
			AssertWDesc(1 == 0, "TextureState::bindToTexture() unsupported mode");
			break;
	}

	return val;
}
#endif /* _USE_OPENGL */

