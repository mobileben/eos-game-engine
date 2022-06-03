/******************************************************************************
 *
 * File: Renderer.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Renderer
 * 
 *****************************************************************************/

#include "Renderer.h"
#include "App.h"
#include "Gfx.h"
#include "Graphics.h"

Renderer::Renderer()
{
	Uint32  i, j;

	for (i=0;i<RenderState::RENDER_STATE_LAST;i++)
	{
		_renderStateStackIndex[i] = 0;

		for (j=0;j<RendererMaxRenderStateStack;j++)
		{
			switch (i)
			{
				case RenderState::RENDER_STATE_ALPHA:
					_renderStateStack[i][j] = new AlphaRenderState(this);
					break;

				case RenderState::RENDER_STATE_MATERIAL:
					_renderStateStack[i][j] = new MaterialRenderState(this);
					break;

				case RenderState::RENDER_STATE_WIREFRAME:
					_renderStateStack[i][j] = new WireframeRenderState(this);
					break;

				case RenderState::RENDER_STATE_COLLISION:
					_renderStateStack[i][j] = new CollisionRenderState(this);
					break;

				case RenderState::RENDER_STATE_CULL:
					_renderStateStack[i][j] = new CullRenderState(this);
					break;

				case RenderState::RENDER_STATE_ZBUFFER:
					_renderStateStack[i][j] = new ZBufferRenderState(this);
					break;

				default:
					AssertWDesc(1 == 0, "Renderer::Renderer Illegal RenderState");
					break;
			}
		}
	}

	_textureStateStackIndex = 0;
	_gfx = NULL;

	_colorBlendStackIndex = 0;

	for (i=0;i<RendererMaxColorBlendingStack;i++)
	{
		_colorBlendStack[i] = 0;
		_colorMultiplier[i].r = 1.0;
		_colorMultiplier[i].g = 1.0;
		_colorMultiplier[i].b = 1.0;
		_colorMultiplier[i].a = 1.0;
		_colorOffset[i].r = 0.0;
		_colorOffset[i].g = 0.0;
		_colorOffset[i].b = 0.0;
		_colorOffset[i].a = 0.0;
	}

	_is2DMode = true;
	_isLandscape = false;

	_currentTextureUnit = RendererTextureUnitIllegal;

	for (i=0;i<RendererTextureUnitMax;i++)
	{
		_currentTextureEnvMode[i] = TextureState::TEXTURE_ENV_MODE_ILLEGAL;
		_texturingEnabled[i] = false;
		_currentTexture[i] = InvalidHWTextureID;
		_texCoordArrayClientState[i] = false;

#ifdef _USE_OPENGL
		_currentTexCoordArray[i] = NULL;
#endif /* _USE_OPENGL */
	}

	_vertArrayClientState = false;
	_colorArrayClientState = false;

	_primaryColorChanged = false;
	_primaryColor.r = _primaryColor.g = _primaryColor.b = _primaryColor.a = 1.0;
	_defaultPrimaryColor.r = _defaultPrimaryColor.g = _defaultPrimaryColor.b = _defaultPrimaryColor.a = 1.0;

	_recordStats = true;

	_coordSys2D = Renderer2DCoordinateSystemTraditional;

#ifdef _USE_OPENGL

#ifdef _PLATFORM_PC
#ifndef _USE_PVR_OPENGLES
	_funcActiveTexture = NULL;
	_funcClientActiveTexture = NULL;
#endif /* _USE_PVR_OPENGLES */
#endif /* _PLATFORM_PC */

	_currentVertArray = NULL;
	_currentColorArray = NULL;

	for (i=0;i<16;i++)
		_tmpMatrix[i] = 0.0;

	_tmpMatrix[0] = 1.0;
	_tmpMatrix[5] = 1.0;
	_tmpMatrix[10] = 1.0;
	_tmpMatrix[15] = 1.0;

#endif /* _USE_OPENGL*/

	memset(&_rendererStats, 0, sizeof(RendererStats));
}

Renderer::~Renderer()
{
	Uint32  i, j;

	for (i=0;i<RenderState::RENDER_STATE_LAST;i++)
	{
		for (j=0;j<RendererMaxRenderStateStack;j++)
		{
			if (_renderStateStack[i][j] != NULL)
			{
				delete _renderStateStack[i][j];
				_renderStateStack[i][j] = NULL;
			}
		}
	}
}

void Renderer::init(void)
{
	Uint32  i, j;

	_displayW = _app->getGfx()->getWidth();
	_displayH = _app->getGfx()->getHeight();

	//	Reinitialize all stacks to default values, then apply them
	for (i=0;i<RenderState::RENDER_STATE_LAST;i++)
	{
		_renderStateStackIndex[i] = 0;

		for (j=0;j<RendererMaxRenderStateStack;j++)
		{
			_renderStateStack[i][j]->setDefaultValues();
			_renderStateStack[i][j]->renderer = this;
		}

		_renderStateStack[i][0]->apply();
	}

	_textureStateStackIndex = 0;

	for (i=0;i<RendererMaxTextureStateStack;i++)
	{
		_textureStateStack[i].setDefaultValues();
	}

	_textureStateStack[0].apply();

#ifdef _USE_OPENGL

	_appRefPtr->getGfx()->lockGLContext();
	
#ifdef _PLATFORM_PC
#ifndef _USE_PVR_OPENGLES
	_funcActiveTexture = (PFNGLACTIVETEXTUREPROC) wglGetProcAddress("glActiveTexture");
	_funcClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC) wglGetProcAddress("glClientActiveTexture");
#endif /* _USE_PVR_OPENGLES */
#endif /* _PLATFORM_PC */

	setTextureUnit(RendererTextureUnit0);

	glDisable(GL_TEXTURE_2D);	//	Turn off texturing

	_appRefPtr->getGfx()->unlockGLContext();

#endif /* _USE_OPENGL */
}

EOSError Renderer::beginFrame(void)
{
	Uint32      i;
	EOSError    error = EOSErrorNone;

	for (i=0;i<RenderState::RENDER_STATE_LAST;i++)
	{
		if (_renderStateStack[i][_renderStateStackIndex[i]]->stateDirty)
			_renderStateStack[i][_renderStateStackIndex[i]]->apply();
	}

	_colorBlendStackIndex = 0;

	for (i=0;i<RendererMaxColorBlendingStack;i++)
	{
		_colorBlendStack[i] = 0;
		_colorMultiplier[i].r = 1.0;
		_colorMultiplier[i].g = 1.0;
		_colorMultiplier[i].b = 1.0;
		_colorMultiplier[i].a = 1.0;
		_colorOffset[i].r = 0.0;
		_colorOffset[i].g = 0.0;
		_colorOffset[i].b = 0.0;
		_colorOffset[i].a = 0.0;
	}

	memset(&_rendererStats, 0, sizeof(RendererStats));

	_primaryColor = _defaultPrimaryColor;
	_primaryColorChanged = false;

#ifdef _USE_OPENGL

	glColor4f(_defaultPrimaryColor.r, _defaultPrimaryColor.g, _defaultPrimaryColor.b, _defaultPrimaryColor.a);

#endif /* _USE_OPENGL*/

	_recordStats = true;

	return error;
}

void Renderer::endFrame(void)
{
	_recordStats = false;
}

void Renderer::bindGfx(Gfx* gfx)
{
	_gfx = gfx;
}

void Renderer::pushRenderStateStack(RenderState::RENDER_STATE state)
{
	AssertWDesc((_renderStateStackIndex[state] + 1) < RendererMaxRenderStateStack, "Renderer::pushRenderState stack overflow.");

	switch (state)
	{
		case RenderState::RENDER_STATE_ALPHA:
			{
				AlphaRenderState*   dst = (AlphaRenderState*) _renderStateStack[state][_renderStateStackIndex[state] + 1];
				AlphaRenderState*   src = (AlphaRenderState*) _renderStateStack[state][_renderStateStackIndex[state]];

				*dst = *src;
			}
			break;

		case RenderState::RENDER_STATE_MATERIAL:
			{
				MaterialRenderState*    dst = (MaterialRenderState*) _renderStateStack[state][_renderStateStackIndex[state] + 1];
				MaterialRenderState*    src = (MaterialRenderState*) _renderStateStack[state][_renderStateStackIndex[state]];

				*dst = *src;
			}
			break;

		case RenderState::RENDER_STATE_WIREFRAME:
			{
				WireframeRenderState*   dst = (WireframeRenderState*) _renderStateStack[state][_renderStateStackIndex[state] + 1];
				WireframeRenderState*   src = (WireframeRenderState*) _renderStateStack[state][_renderStateStackIndex[state]];

				*dst = *src;
			}
			break;

		case RenderState::RENDER_STATE_COLLISION:
			{
				CollisionRenderState*   dst = (CollisionRenderState*) _renderStateStack[state][_renderStateStackIndex[state] + 1];
				CollisionRenderState*   src = (CollisionRenderState*) _renderStateStack[state][_renderStateStackIndex[state]];

				*dst = *src;
			}
			break;

		case RenderState::RENDER_STATE_CULL:
			{
				CullRenderState*    dst = (CullRenderState*) _renderStateStack[state][_renderStateStackIndex[state] + 1];
				CullRenderState*    src = (CullRenderState*) _renderStateStack[state][_renderStateStackIndex[state]];

				*dst = *src;
			}
			break;

		case RenderState::RENDER_STATE_ZBUFFER:
			{
				ZBufferRenderState* dst = (ZBufferRenderState*) _renderStateStack[state][_renderStateStackIndex[state] + 1];
				ZBufferRenderState* src = (ZBufferRenderState*) _renderStateStack[state][_renderStateStackIndex[state]];

				*dst = *src;
			}
			break;

		default:
			AssertWDesc(1 == 0, "Renderer::pushRenderStateStack Illegal RenderState");
			break;
	}

	_renderStateStackIndex[state]++;

	//	Force the renderer to point to this
	_renderStateStack[state][_renderStateStackIndex[state]]->renderer = this;
}

void Renderer::popRenderStateStack(RenderState::RENDER_STATE state)
{
	AssertWDesc(_renderStateStackIndex[state] > 0, "Renderer::pushRenderState stack underflow.");

	_renderStateStackIndex[state]--;

	_renderStateStack[state][_renderStateStackIndex[state]]->apply();
}

void Renderer::setRenderState(AlphaRenderState* state)
{
	AlphaRenderState* curr = (AlphaRenderState*) _renderStateStack[RenderState::RENDER_STATE_ALPHA][_renderStateStackIndex[RenderState::RENDER_STATE_ALPHA]];

	*curr = *state;
	curr->renderer = this;

	curr->apply();
}

void Renderer::setRenderState(MaterialRenderState* state)
{
	MaterialRenderState* curr = (MaterialRenderState*) _renderStateStack[RenderState::RENDER_STATE_MATERIAL][_renderStateStackIndex[RenderState::RENDER_STATE_MATERIAL]];

	*curr = *state;
	curr->renderer = this;

	curr->apply();
}

void Renderer::setRenderState(WireframeRenderState* state)
{
	WireframeRenderState* curr = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

	*curr = *state;
	curr->renderer = this;

	curr->apply();
}

void Renderer::setRenderState(CollisionRenderState* state)
{
	CollisionRenderState* curr = (CollisionRenderState*) _renderStateStack[RenderState::RENDER_STATE_COLLISION][_renderStateStackIndex[RenderState::RENDER_STATE_COLLISION]];

	*curr = *state;
	curr->renderer = this;

	curr->apply();
}

void Renderer::setRenderState(CullRenderState* state)
{
	CullRenderState* curr = (CullRenderState*) _renderStateStack[RenderState::RENDER_STATE_CULL][_renderStateStackIndex[RenderState::RENDER_STATE_CULL]];

	*curr = *state;
	curr->renderer = this;

	curr->apply();
}

void Renderer::setRenderState(ZBufferRenderState* state)
{
	ZBufferRenderState* curr = (ZBufferRenderState*) _renderStateStack[RenderState::RENDER_STATE_ZBUFFER][_renderStateStackIndex[RenderState::RENDER_STATE_ZBUFFER]];

	*curr = *state;
	curr->renderer = this;

	curr->apply();
}

RenderState* Renderer::getRenderState(RenderState::RENDER_STATE state)
{
	return _renderStateStack[state][_renderStateStackIndex[state]];
}

void Renderer::flushRenderState(RenderState::RENDER_STATE state)
{
	_renderStateStack[state][_renderStateStackIndex[state]]->apply();
}

void Renderer::setTextureEnvMode(TextureState::TEXTURE_ENV_MODE mode, RendererTextureUnit texunit)
{
#ifdef _USE_OPENGL
	GLenum	envmode;
#endif /* _USE_OPENGL */

	if (_currentTextureEnvMode[texunit] != mode)
	{
		_currentTextureEnvMode[texunit] = mode;
		_rendererStats.texEnvChanges++;

#ifdef _USE_OPENGL
		envmode = TextureState::TEXTURE_ENV_MODE_To_GL_TEXTURE_ENV(_currentTextureEnvMode[texunit]);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, envmode);
#endif /* _USE_OPENGL */
	}
}

void Renderer::flushTextureEnvMode(RendererTextureUnit texunit)
{
	_currentTextureEnvMode[texunit] = TextureState::TEXTURE_ENV_MODE_ILLEGAL;
}

void Renderer::setTextureUnit(RendererTextureUnit texunit)
{
	if (_currentTextureUnit != texunit)
	{
		switch (texunit)
		{
			case RendererTextureUnit0:
#if defined(_PLATFORM_PC)&&!(_USE_PVR_OPENGLES)
				_funcActiveTexture(GL_TEXTURE0);
#else
				glActiveTexture(GL_TEXTURE0);
#endif /* _PLATFORM_PC */
				break;

			case RendererTextureUnit1:
#if defined(_PLATFORM_PC)&&!(_USE_PVR_OPENGLES)
				_funcActiveTexture(GL_TEXTURE1);
#else
				glActiveTexture(GL_TEXTURE1);
#endif /* _PLATFORM_PC */
				break;

			default:
				AssertWDesc(1 == 0, "Renderer::setTextureUnit() Illegal TEXUNIT");
				break;
		}

		_currentTextureUnit = texunit;
	}
}

void Renderer::enableTexturing(void)
{
	if (_currentTextureUnit == RendererTextureUnitIllegal)
		setTextureUnit(RendererTextureUnit0);

	if (_texturingEnabled[_currentTextureUnit] != true)
	{
		glEnable(GL_TEXTURE_2D);
		_texturingEnabled[_currentTextureUnit] = true;
	}
}

void Renderer::disableTexturing(void)
{
	if (_currentTextureUnit == RendererTextureUnitIllegal)
		setTextureUnit(RendererTextureUnit0);

	if (_texturingEnabled[_currentTextureUnit] != false)
	{
		glDisable(GL_TEXTURE_2D);
		_texturingEnabled[_currentTextureUnit] = false;
	}
}

void Renderer::disableAllTexturing(void)
{
	RendererTextureUnit	texunit = _currentTextureUnit;
	Uint32				i;

	for (i=RendererTextureUnit0;i<RendererTextureUnitMax;i++)
	{
		if (_texturingEnabled[i] != false)
		{
			setTextureUnit((RendererTextureUnit) i);
			glDisable(GL_TEXTURE_2D);
			_texturingEnabled[i] = false;
		}
	}

	if (_currentTextureUnit == RendererTextureUnitIllegal)
		setTextureUnit(RendererTextureUnit0);
	else
		setTextureUnit(texunit);
}

void Renderer::enableVertArrayClientState(void)
{
	if (_vertArrayClientState != true)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		_vertArrayClientState = true;
	}
}

void Renderer::disableVertArrayClientState(void)
{
	if (_vertArrayClientState != false)
	{
		glDisableClientState(GL_VERTEX_ARRAY);
		_vertArrayClientState = false;
	}
}

void Renderer::enableTexCoordArrayClientState(void)
{
	if (_currentTextureUnit == RendererTextureUnitIllegal)
		setTextureUnit(RendererTextureUnit0);

	if (_texCoordArrayClientState[_currentTextureUnit] != true)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		_texCoordArrayClientState[_currentTextureUnit] = true;
	}
}

void Renderer::disableTexCoordArrayClientState(void)
{
	if (_currentTextureUnit == RendererTextureUnitIllegal)
		setTextureUnit(RendererTextureUnit0);

	if (_texCoordArrayClientState[_currentTextureUnit] != false)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		_texCoordArrayClientState[_currentTextureUnit] = false;
	}
}

void Renderer::enableColorArrayClientState(void)
{
	if (_colorArrayClientState != true)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		_colorArrayClientState = true;
	}
}

void Renderer::disableColorArrayClientState(void)
{
	if (_colorArrayClientState != false)
	{
		glDisableClientState(GL_COLOR_ARRAY);
		_colorArrayClientState = false;
	}
}

#ifdef _USE_OPENGL

void Renderer::setTexture(GLenum target, HWTextureID texID)
{
	if (_currentTextureUnit == RendererTextureUnitIllegal)
		setTextureUnit(RendererTextureUnit0);

	if (_currentTexture[_currentTextureUnit] != texID)
	{
		glBindTexture(target, texID);

		_currentTexture[_currentTextureUnit] = texID;

		_rendererStats.texChanges++;
	}
}

#endif /* _USE_OPENGL */

void Renderer::setTexture(HWTextureID texID)
{
	if (_currentTextureUnit == RendererTextureUnitIllegal)
		setTextureUnit(RendererTextureUnit0);

	if (_currentTexture[_currentTextureUnit] != texID)
	{
		glBindTexture(GL_TEXTURE_2D, texID);

		_currentTexture[_currentTextureUnit] = texID;

		_rendererStats.texChanges++;
	}
}

void Renderer::flushTexture(void)
{
	if (_currentTextureUnit == RendererTextureUnitIllegal)
		setTextureUnit(RendererTextureUnit0);

	_currentTexture[_currentTextureUnit] = InvalidHWTextureID;
}

void Renderer::flushAllTextures(void)
{
	Uint32				i;

	for (i=RendererTextureUnit0;i<RendererTextureUnitMax;i++)
	{
		_currentTexture[i] = InvalidHWTextureID;
	}
}

#ifdef _USE_OPENGL
void Renderer::setTexCoordArray(Float32* data, Boolean force)
{
	if (_currentTextureUnit == RendererTextureUnitIllegal)
		setTextureUnit(RendererTextureUnit0);

	if (force || _currentTexCoordArray[_currentTextureUnit] != data)
	{
		glTexCoordPointer(2, GL_FLOAT, 0, data);
		_currentTexCoordArray[_currentTextureUnit] = data;
	}
	else
		_rendererStats.reusedTexArray++;
}

void Renderer::setVertArray(Float32* data, Boolean force)
{
	if (force || _currentVertArray != data)
	{
		glVertexPointer(2, GL_FLOAT, 0, data);
		_currentVertArray = data;
	}
	else
		_rendererStats.reusedVertArray++;
}

void Renderer::setColorArray(Float32* data, Boolean force)
{
	if (force || _currentColorArray != data)
	{
		glColorPointer(4, GL_FLOAT, 0, data);
		_currentColorArray = data;
	}
}

#endif /* _USE_OPENGL */

void Renderer::setPrimaryColor(const ColorRGB& color)
{
	if (color.r != _primaryColor.r || color.g != _primaryColor.g || color.b != _primaryColor.b || 1.0F != _primaryColor.a)
	{
		_primaryColorChanged = true;

		_primaryColor.r = color.r;
		_primaryColor.g = color.g;
		_primaryColor.b = color.b;
		_primaryColor.a = 1.0F;

#ifdef _USE_OPENGL
		glColor4f(_primaryColor.r, _primaryColor.g, _primaryColor.b, _primaryColor.a);
#endif /* _USE_OPENGL*/

		_rendererStats.colorChanges++;
	}
}

void Renderer::setPrimaryColor(const ColorRGBA& color)
{
	if (color.r != _primaryColor.r || color.g != _primaryColor.g || color.b != _primaryColor.b || color.a != _primaryColor.a)
	{
		_primaryColorChanged = true;

		_primaryColor = color;

#ifdef _USE_OPENGL
		glColor4f(_primaryColor.r, _primaryColor.g, _primaryColor.b, _primaryColor.a);
#endif /* _USE_OPENGL*/

		_rendererStats.colorChanges++;
	}
}

void Renderer::restoreDefaultPrimaryColor(void)
{
	if (_primaryColorChanged)
	{
		_primaryColor = _defaultPrimaryColor;
	
#ifdef _USE_OPENGL
	
		glColor4f(_defaultPrimaryColor.r, _defaultPrimaryColor.g, _defaultPrimaryColor.b, _defaultPrimaryColor.a);
	
#endif /* _USE_OPENGL*/

		_primaryColorChanged = false;
	}
}

void Renderer::pushTextureState(void)
{
	TextureState*   src;
	TextureState*   dst;

	AssertWDesc((_textureStateStackIndex + 1) < RendererMaxTextureStateStack, "Renderer::pushTextureState stack overflow.");

	src = &_textureStateStack[_textureStateStackIndex];
	dst = &_textureStateStack[_textureStateStackIndex + 1];

	*dst = *src;

	_textureStateStackIndex++;
}

void Renderer::popTextureState(void)
{
	AssertWDesc(_textureStateStackIndex > 0, "Renderer::pushTextureState stack underflow.");

	_textureStateStackIndex--;

	_textureStateStack[_textureStateStackIndex].apply();
}

void Renderer::setTextureState(TextureState* state)
{
	_textureStateStack[_textureStateStackIndex] = *state;

	_textureStateStack[_textureStateStackIndex].apply();
}

TextureState* Renderer::getTextureState(void)
{
	return &_textureStateStack[_textureStateStackIndex];
}

void Renderer::flushTextureState(void)
{
	_textureStateStack[_textureStateStackIndex].apply();
}

void Renderer::drawLine(ColorRGB& rgb, Point2D& pt0, Point2D& pt1)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
		setPrimaryColor(rgb);

		disableAllTexturing();

#ifdef _USE_OPENGL	

#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			_tmpRectVerts[0] = pt0.x;
			_tmpRectVerts[1] = _displayH - pt0.y;
			_tmpRectVerts[2] = pt1.x;
			_tmpRectVerts[3] = _displayH - pt1.y;
		}
		else
		{
			_tmpRectVerts[0] = pt0.x;
			_tmpRectVerts[1] = pt0.y;
			_tmpRectVerts[2] = pt1.x;
			_tmpRectVerts[3] = pt1.y;
		}

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINES, 0, 2);

		if (_recordStats)
			_rendererStats.vertices += 2;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			glVertex2f(pt0.x, _displayH - pt0.y);
			glVertex2f(pt1.x, _displayH - (pt1.y));
		}
		else
		{
			glVertex2f(pt0.x, pt0.y);
			glVertex2f(pt1.x, pt1.y);
		}

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 2;

#else

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			_tmpRectVerts[0] = pt0.x;
			_tmpRectVerts[1] = _displayH - pt0.y;
			_tmpRectVerts[2] = pt1.x;
			_tmpRectVerts[3] = _displayH - pt1.y;
		}
		else
		{
			_tmpRectVerts[0] = pt0.x;
			_tmpRectVerts[1] = pt0.y;
			_tmpRectVerts[2] = pt1.x;
			_tmpRectVerts[3] = pt1.y;
		}

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINES, 0, 2);

		if (_recordStats)
			_rendererStats.vertices += 2;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 2);

		if (_recordStats)
			_rendererStats.vertices += 2;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		glVertex2f(_tmpRectVerts[0], _tmpRectVerts[1]);

		glVertex2f(_tmpRectVerts[2], _tmpRectVerts[3]);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 2;

#else
		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 2);

		if (_recordStats)
			_rendererStats.vertices += 2;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}
}

void Renderer::drawLine(ColorRGBA& rgba, Point2D& pt0, Point2D& pt1)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
		setPrimaryColor(rgba);

		disableAllTexturing();

#ifdef _USE_OPENGL	

#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			_tmpRectVerts[0] = pt0.x;
			_tmpRectVerts[1] = _displayH - pt0.y;
			_tmpRectVerts[2] = pt1.x;
			_tmpRectVerts[3] = _displayH - pt1.y;
		}
		else
		{
			_tmpRectVerts[0] = pt0.x;
			_tmpRectVerts[1] = pt0.y;
			_tmpRectVerts[2] = pt1.x;
			_tmpRectVerts[3] = pt1.y;
		}

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINES, 0, 2);

		if (_recordStats)
			_rendererStats.vertices += 2;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			glVertex2f(pt0.x, _displayH - pt0.y);
			glVertex2f(pt1.x, _displayH - (pt1.y));
		}
		else
		{
			glVertex2f(pt0.x, pt0.y);
			glVertex2f(pt1.x, pt1.y);
		}

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 2;

#else

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			_tmpRectVerts[0] = pt0.x;
			_tmpRectVerts[1] = _displayH - pt0.y;
			_tmpRectVerts[2] = pt1.x;
			_tmpRectVerts[3] = _displayH - pt1.y;
		}
		else
		{
			_tmpRectVerts[0] = pt0.x;
			_tmpRectVerts[1] = pt0.y;
			_tmpRectVerts[2] = pt1.x;
			_tmpRectVerts[3] = pt1.y;
		}

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINES, 0, 2);

		if (_recordStats)
			_rendererStats.vertices += 2;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINES, 0, 2);

		if (_recordStats)
			_rendererStats.vertices += 2;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINES);

		glVertex2f(_tmpRectVerts[0], _tmpRectVerts[1]);
		glVertex2f(_tmpRectVerts[2], _tmpRectVerts[3]);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 2;

#else
		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINES, 0, 2);

		if (_recordStats)
			_rendererStats.vertices += 2;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}
}

void Renderer::drawLineRect(ColorRGB& rgb, Point2D& pos, Point2D& wh)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
		setPrimaryColor(rgb);

		disableAllTexturing();

#ifdef _USE_OPENGL	

#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[1] = _displayH - pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[3] = _displayH - (pos.y + wh.y - 1.0F);
			_tmpRectVerts[4] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[5] = _displayH - (pos.y + wh.y - 1.0F);
			_tmpRectVerts[6] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[7] =  _displayH - pos.y;
		}
		else
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[3] = pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[1] = pos.y + wh.y - 1.0F;
			_tmpRectVerts[4] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[5] = pos.y + wh.y - 1.0F;
			_tmpRectVerts[6] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[7] = pos.y;
		}

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			glVertex2f(pos.x, _displayH - pos.y);
			glVertex2f(pos.x, _displayH - (pos.y + wh.y - 1.0F));
			glVertex2f(pos.x + wh.x, _displayH - (pos.y + wh.y - 1.0F));
			glVertex2f(pos.x + wh.x, _displayH - pos.y);
		}
		else
		{
			glVertex2f(pos.x, pos.y);
			glVertex2f(pos.x, pos.y + wh.y);
			glVertex2f(pos.x + wh.x - 1.0F, pos.y + wh.y - 1.0F);
			glVertex2f(pos.x + wh.x - 1.0F, pos.y);
		}

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[1] = _displayH - pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[3] = _displayH - (pos.y + wh.y - 1.0F);
			_tmpRectVerts[4] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[5] = _displayH - (pos.y + wh.y - 1.0F);
			_tmpRectVerts[6] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[7] =  _displayH - pos.y;
		}
		else
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[1] = pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[3] = pos.y + wh.y - 1.0F;
			_tmpRectVerts[4] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[5] = pos.y + wh.y - 1.0F;
			_tmpRectVerts[6] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[7] = pos.y;
		}

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		glVertex2f(_tmpRectVerts[0], _tmpRectVerts[1]);

		glVertex2f(_tmpRectVerts[2], _tmpRectVerts[3]);

		glVertex2f(_tmpRectVerts[4], _tmpRectVerts[5]);

		glVertex2f(_tmpRectVerts[6], _tmpRectVerts[7]);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else
		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}
}

void Renderer::drawLineRect(ColorRGBA& rgba, Point2D& pos, Point2D& wh)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
		setPrimaryColor(rgba);

		disableAllTexturing();

#ifdef _USE_OPENGL

#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[1] = _displayH - pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[3] = _displayH - (pos.y + wh.y - 1.0F);
			_tmpRectVerts[4] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[5] = _displayH - (pos.y + wh.y - 1.0F);
			_tmpRectVerts[6] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[7] =  _displayH - pos.y;
		}
		else
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[1] = pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[3] = pos.y + wh.y - 1.0F;
			_tmpRectVerts[4] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[5] = pos.y + wh.y - 1.0F;
			_tmpRectVerts[6] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[7] = pos.y;
		}

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			glVertex2f(pos.x, _displayH - pos.y);
			glVertex2f(pos.x, _displayH - (pos.y + wh.y - 1.0F));
			glVertex2f(pos.x + wh.x - 1.0F, _displayH - (pos.y + wh.y - 1.0F));
			glVertex2f(pos.x + wh.x - 1.0F, _displayH - pos.y);
		}
		else
		{
			glVertex2f(pos.x, pos.y);
			glVertex2f(pos.x, pos.y + wh.y - 1.0F);
			glVertex2f(pos.x + wh.x - 1.0F, pos.y + wh.y - 1.0F);
			glVertex2f(pos.x + wh.x - 1.0F, pos.y);
		}

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[1] = _displayH - pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[3] = _displayH - (pos.y + wh.y - 1.0F);
			_tmpRectVerts[4] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[5] = _displayH - (pos.y + wh.y - 1.0F);
			_tmpRectVerts[6] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[7] =  _displayH - pos.y;
		}
		else
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[1] = pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[3] = pos.y + wh.y - 1.0F;
			_tmpRectVerts[4] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[5] = pos.y + wh.y - 1.0F;
			_tmpRectVerts[6] = pos.x + wh.x - 1.0F;
			_tmpRectVerts[7] = pos.y;
		}

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		glVertex2f(_tmpRectVerts[0], _tmpRectVerts[1]);

		glVertex2f(_tmpRectVerts[2], _tmpRectVerts[3]);

		glVertex2f(_tmpRectVerts[4], _tmpRectVerts[5]);

		glVertex2f(_tmpRectVerts[6], _tmpRectVerts[7]);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else
		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}
}

void Renderer::drawFillRect(ColorRGB& rgb, Point2D& pos, Point2D& wh)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
		setPrimaryColor(rgb);

		disableAllTexturing();

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[1] = _displayH - pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[3] = _displayH - (pos.y + wh.y);
			_tmpRectVerts[4] = pos.x + wh.x;
			_tmpRectVerts[5] = _displayH - (pos.y + wh.y);
			_tmpRectVerts[6] = pos.x + wh.x;
			_tmpRectVerts[7] =  _displayH - pos.y;
		}
		else
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[1] = pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[3] = pos.y + wh.y;
			_tmpRectVerts[4] = pos.x + wh.x;
			_tmpRectVerts[5] = pos.y + wh.y;
			_tmpRectVerts[6] = pos.x + wh.x;
			_tmpRectVerts[7] = pos.y;
		}

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
		}
#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_QUADS);

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			glVertex2f(pos.x, _displayH - pos.y);
			glVertex2f(pos.x, _displayH - (pos.y + wh.y));
			glVertex2f(pos.x + wh.x, _displayH - (pos.y + wh.y));
			glVertex2f(pos.x + wh.x, _displayH - pos.y);
		}
		else
		{
			glVertex2f(pos.x, pos.y);
			glVertex2f(pos.x, pos.y + wh.y);
			glVertex2f(pos.x + wh.x, pos.y + wh.y);
			glVertex2f(pos.x + wh.x, pos.y);
		}

		glEnd();

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 1;
		}

#else

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[1] = _displayH - pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[3] = _displayH - (pos.y + wh.y);
			_tmpRectVerts[4] = pos.x + wh.x;
			_tmpRectVerts[5] = _displayH - (pos.y + wh.y);
			_tmpRectVerts[6] = pos.x + wh.x;
			_tmpRectVerts[7] =  _displayH - pos.y;
		}
		else
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[1] = pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[3] = pos.y + wh.y;
			_tmpRectVerts[4] = pos.x + wh.x;
			_tmpRectVerts[5] = pos.y + wh.y;
			_tmpRectVerts[6] = pos.x + wh.x;
			_tmpRectVerts[7] = pos.y;
		}

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
		}

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		glVertex2f(_tmpRectVerts[0], _tmpRectVerts[1]);

		glVertex2f(_tmpRectVerts[2], _tmpRectVerts[3]);

		glVertex2f(_tmpRectVerts[4], _tmpRectVerts[5]);

		glVertex2f(_tmpRectVerts[6], _tmpRectVerts[7]);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else
		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}
}

void Renderer::drawFillRect(ColorRGBA& rgba, Point2D& pos, Point2D& wh)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
		setPrimaryColor(rgba);

		disableAllTexturing();

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[1] = _displayH - pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[3] = _displayH - (pos.y + wh.y);
			_tmpRectVerts[4] = pos.x + wh.x;
			_tmpRectVerts[5] = _displayH - (pos.y + wh.y);
			_tmpRectVerts[6] = pos.x + wh.x;
			_tmpRectVerts[7] =  _displayH - pos.y;
		}
		else
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[1] = pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[3] = pos.y + wh.y;
			_tmpRectVerts[4] = pos.x + wh.x;
			_tmpRectVerts[5] = pos.y + wh.y;
			_tmpRectVerts[6] = pos.x + wh.x;
			_tmpRectVerts[7] = pos.y;
		}

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
		}

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_QUADS);

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			glVertex2f(pos.x, _displayH - pos.y);
			glVertex2f(pos.x, _displayH - (pos.y + wh.y));
			glVertex2f(pos.x + wh.x, _displayH - (pos.y + wh.y));
			glVertex2f(pos.x + wh.x, _displayH - pos.y);
		}
		else
		{
			glVertex2f(pos.x, pos.y);
			glVertex2f(pos.x, pos.y + wh.y);
			glVertex2f(pos.x + wh.x, pos.y + wh.y);
			glVertex2f(pos.x + wh.x, pos.y);
		}

		glEnd();

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 1;
		}

#else

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[1] = _displayH - pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[3] = _displayH - (pos.y + wh.y);
			_tmpRectVerts[4] = pos.x + wh.x;
			_tmpRectVerts[5] = _displayH - (pos.y + wh.y);
			_tmpRectVerts[6] = pos.x + wh.x;
			_tmpRectVerts[7] =  _displayH - pos.y;
		}
		else
		{
			_tmpRectVerts[0] = pos.x;
			_tmpRectVerts[1] = pos.y;
			_tmpRectVerts[2] = pos.x;
			_tmpRectVerts[3] = pos.y + wh.y;
			_tmpRectVerts[4] = pos.x + wh.x;
			_tmpRectVerts[5] = pos.y + wh.y;
			_tmpRectVerts[6] = pos.x + wh.x;
			_tmpRectVerts[7] = pos.y;
		}

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
		}

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		glVertex2f(_tmpRectVerts[0], _tmpRectVerts[1]);

		glVertex2f(_tmpRectVerts[2], _tmpRectVerts[3]);

		glVertex2f(_tmpRectVerts[4], _tmpRectVerts[5]);

		glVertex2f(_tmpRectVerts[6], _tmpRectVerts[7]);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else
		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}
}

inline void applySkew(Float32* mat, Float32* glMat)
{
	glMat[0] = mat[SPRITE_MATRIX_A];
	glMat[1] = -mat[SPRITE_MATRIX_B];
	glMat[4] = -mat[SPRITE_MATRIX_C];
	glMat[5] = mat[SPRITE_MATRIX_D];

#ifdef _USE_OPENGL
	glMultMatrixf(glMat);
#endif /* _USE_OPENGL */
}

void Renderer::drawSprite(const Sprite& sprite, const Point2D& pos, Float32 rotate, DrawControl control)
{
#ifdef _USE_OPENGL	
	Uint32                  i;
	SpriteMapping*          mapping;
	SpriteSet*              spriteSet = sprite.spriteSet;
	Float32                 sx, sy;

	glPushMatrix();

//	glLoadIdentity();

	if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		glTranslatef(-sprite.xy->x + pos.x, _displayH - (-sprite.xy->y + pos.y), 0.0);
	else
		glTranslatef(-sprite.xy->x + pos.x, -sprite.xy->y + pos.y, 0.0);

	if (sprite.control & CONTROL_TRANSFORM_MATRIX)
	{
		applySkew(sprite.transform, _tmpMatrix);

		if (rotate != 0.0)
		{
			glRotatef(rotate, 0, 0, 1);
			_rendererStats.rotates++;
		}
		else
			_rendererStats.ignoredRotates++;
	}
	else
	{
		if ((sprite.transform[SPRITE_TRANSFORM_ROTATE_Z] + rotate) != 0.0)
		{
   			glRotatef(-sprite.transform[SPRITE_TRANSFORM_ROTATE_Z] + rotate, 0, 0, 1);
			_rendererStats.rotates++;
		}
		else
			_rendererStats.ignoredRotates++;
	
		sx = sprite.transform[SPRITE_TRANSFORM_SCALE_X];
		sy = sprite.transform[SPRITE_TRANSFORM_SCALE_Y];
	
		if (control & CONTROL_FLIP_H)
			sx = -sx;
	
		if (control & CONTROL_FLIP_V)
			sy = -sy;
	
		if (sprite.control & CONTROL_FLIP_H)
			sx = -sx;
	
		if (sprite.control & CONTROL_FLIP_V)
			sy = -sy;

		if (sx != 1.0 || sy != 1.0)
		{
			glScalef(sx, sy, 1.0F);
			_rendererStats.scales++;
		}
		else
			_rendererStats.ignoredScales++;
	}

	_colorBlendStack[++_colorBlendStackIndex] = _colorBlendStack[_colorBlendStackIndex-1] | (sprite.control & (CONTROL_TRANSFORM_COLOR_MULTIPLIER | CONTROL_TRANSFORM_COLOR_OFFSET));

	if (sprite.control & CONTROL_TRANSFORM_COLOR_MULTIPLIER)
	{	
		_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex-1].r * sprite.colorOp->redMultiplier;

		if (_colorMultiplier[_colorBlendStackIndex].r > 1.0F)
			_colorMultiplier[_colorBlendStackIndex].r = 1.0F;

		_colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex-1].g * sprite.colorOp->greenMultiplier;

		if (_colorMultiplier[_colorBlendStackIndex].g > 1.0F)
			_colorMultiplier[_colorBlendStackIndex].g = 1.0F;

		_colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex-1].b * sprite.colorOp->blueMultiplier;

		if (_colorMultiplier[_colorBlendStackIndex].b > 1.0F)
			_colorMultiplier[_colorBlendStackIndex].b = 1.0F;

		_colorMultiplier[_colorBlendStackIndex].a = _colorMultiplier[_colorBlendStackIndex-1].a * sprite.colorOp->alphaMultiplier;

		if (_colorMultiplier[_colorBlendStackIndex].a > 1.0F)
			_colorMultiplier[_colorBlendStackIndex].a = 1.0F;

	}
	else if (sprite.control & CONTROL_TRANSFORM_COLOR_OFFSET)
	{
		_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex-1].r;
		_colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex-1].g;
		_colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex-1].b;
		_colorMultiplier[_colorBlendStackIndex].a = _colorMultiplier[_colorBlendStackIndex-1].a;
	}

	if (sprite.control & CONTROL_TRANSFORM_COLOR_OFFSET)
	{	
		_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex-1].r + sprite.colorOp->redOffset;

		if (_colorOffset[_colorBlendStackIndex].r > 1.0F)
			_colorOffset[_colorBlendStackIndex].r = 1.0F;

		_colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex-1].g + sprite.colorOp->greenOffset;

		if (_colorOffset[_colorBlendStackIndex].g > 1.0F)
			_colorOffset[_colorBlendStackIndex].g = 1.0F;

		_colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex-1].b + sprite.colorOp->blueOffset;

		if (_colorOffset[_colorBlendStackIndex].b > 1.0F)
			_colorOffset[_colorBlendStackIndex].b = 1.0F;

		_colorOffset[_colorBlendStackIndex].a = _colorOffset[_colorBlendStackIndex-1].a + sprite.colorOp->alphaOffset;

		if (_colorOffset[_colorBlendStackIndex].a > 1.0F)
			_colorOffset[_colorBlendStackIndex].a = 1.0F;
	}
	else
	{
		_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex-1].r;
		_colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex-1].g;
		_colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex-1].b;
		_colorOffset[_colorBlendStackIndex].a = _colorOffset[_colorBlendStackIndex-1].a;
	}

	mapping = sprite.children;

	for (i=0;i<sprite.numChildren;i++)
	{
		if (mapping->mapControl & CONTROL_TYPE_SPRITEMASTER)
		{
			Sprite* next;

			next = spriteSet->getSprite(mapping->offset);

			drawSprite(*next, 0);
		}
		else if (mapping->mapControl & CONTROL_TYPE_SPRITENORMAL)
		{
			SubSprite*  next;

			next = spriteSet->getSubSprite(mapping->offset);

			drawSubSprite(*next);
		}
		else
		{
			TextureAtlasSubTexture* texref;

			texref = spriteSet->getTextureAtlasSubTexture(mapping->offset);

			drawSubTexture(*texref);
		}

		mapping++;
	}

	_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex].a = 1.0F;
	_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex].a = 0.0F;

	_colorBlendStackIndex--;

	setPrimaryColor(_colorMultiplier[_colorBlendStackIndex]);

	glPopMatrix();
#endif /* _USE_OPENGL */	
}

void Renderer::drawSprite(const Sprite& sprite, const Point2D& pos, Float32 rotate, DrawControl control, SpriteColorOp& colorOp)
{
#ifdef _USE_OPENGL	
	Uint32                  i;
	SpriteMapping*          mapping;
	SpriteSet*              spriteSet = sprite.spriteSet;
	Float32                 sx, sy;

	glPushMatrix();

//	glLoadIdentity();

	if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		glTranslatef(-sprite.xy->x + pos.x, _displayH - (-sprite.xy->y + pos.y), 0.0);
	else
		glTranslatef(-sprite.xy->x + pos.x, -sprite.xy->y + pos.y, 0.0);

	if (sprite.control & CONTROL_TRANSFORM_MATRIX)
	{
		applySkew(sprite.transform, _tmpMatrix);

		if (rotate != 0.0)
		{
			glRotatef(rotate, 0, 0, 1);
			_rendererStats.rotates++;
		}
		else
			_rendererStats.ignoredRotates++;
	}
	else
	{
		if ((sprite.transform[SPRITE_TRANSFORM_ROTATE_Z] + rotate) != 0.0)
		{
   			glRotatef(-sprite.transform[SPRITE_TRANSFORM_ROTATE_Z] + rotate, 0, 0, 1);
			_rendererStats.rotates++;
		}
		else
			_rendererStats.ignoredRotates++;
	
		sx = sprite.transform[SPRITE_TRANSFORM_SCALE_X];
		sy = sprite.transform[SPRITE_TRANSFORM_SCALE_Y];
	
		if (control & CONTROL_FLIP_H)
			sx = -sx;
	
		if (control & CONTROL_FLIP_V)
			sy = -sy;
	
		if (sprite.control & CONTROL_FLIP_H)
			sx = -sx;
	
		if (sprite.control & CONTROL_FLIP_V)
			sy = -sy;

		if (sx != 1.0 || sy != 1.0)
		{
			glScalef(sx, sy, 1.0F);
			_rendererStats.scales++;
		}
		else
			_rendererStats.ignoredScales++;
	}

	_colorBlendStack[++_colorBlendStackIndex] = (CONTROL_TRANSFORM_COLOR_MULTIPLIER | CONTROL_TRANSFORM_COLOR_OFFSET);

	_colorMultiplier[_colorBlendStackIndex].r = colorOp.redMultiplier;
	_colorMultiplier[_colorBlendStackIndex].g = colorOp.greenMultiplier;
	_colorMultiplier[_colorBlendStackIndex].b = colorOp.blueMultiplier;
	_colorMultiplier[_colorBlendStackIndex].a = colorOp.alphaMultiplier;
	_colorOffset[_colorBlendStackIndex].r = colorOp.redOffset;
	_colorOffset[_colorBlendStackIndex].g = colorOp.greenOffset;
	_colorOffset[_colorBlendStackIndex].b = colorOp.blueOffset;
	_colorOffset[_colorBlendStackIndex].a = colorOp.alphaOffset;

	mapping = sprite.children;

	for (i=0;i<sprite.numChildren;i++)
	{
		if (mapping->mapControl & CONTROL_TYPE_SPRITEMASTER)
		{
			Sprite* next;

			next = spriteSet->getSprite(mapping->offset);

			drawSprite(*next, 0, colorOp);
		}
		else if (mapping->mapControl & CONTROL_TYPE_SPRITENORMAL)
		{
			SubSprite*  next;

			next = spriteSet->getSubSprite(mapping->offset);

			drawSubSprite(*next);
		}
		else
		{
			TextureAtlasSubTexture* texref;

			texref = spriteSet->getTextureAtlasSubTexture(mapping->offset);

			drawSubTexture(*texref);
		}

		mapping++;
	}

	_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex].a = 1.0F;
	_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex].a = 0.0F;

	_colorBlendStackIndex--;

	setPrimaryColor(_colorMultiplier[_colorBlendStackIndex]);

	glPopMatrix();
#endif /* _USE_OPENGL */	
}

void Renderer::drawSprite(const Sprite& sprite, const Point2D& pos, Float32 scalex, Float32 scaley, Float32 rotate, DrawControl control)
{
#ifdef _USE_OPENGL	
	Uint32                  i;
	SpriteMapping*          mapping;
	SpriteSet*              spriteSet = sprite.spriteSet;
	Float32                 sx, sy;

	glPushMatrix();

//	glLoadIdentity();

	if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		glTranslatef(-sprite.xy->x + pos.x, _displayH - (-sprite.xy->y + pos.y), 0.0);
	else
		glTranslatef(-sprite.xy->x + pos.x, -sprite.xy->y + pos.y, 0.0);

	if (sprite.control & CONTROL_TRANSFORM_MATRIX)
	{
		applySkew(sprite.transform, _tmpMatrix);

		if (rotate != 0.0)
		{
			glRotatef(rotate, 0, 0, 1);
			_rendererStats.rotates++;
		}
		else
			_rendererStats.ignoredRotates++;

		if (scalex != 1.0F || scaley != 1.0F)
			glScalef(scalex, scaley, 1.0F);
		else
			_rendererStats.ignoredScales++;
	}
	else
	{
		if ((sprite.transform[SPRITE_TRANSFORM_ROTATE_Z] + rotate) != 0.0)
		{
			glRotatef(-sprite.transform[SPRITE_TRANSFORM_ROTATE_Z] + rotate, 0, 0, 1);
			_rendererStats.rotates++;
		}
		else
			_rendererStats.ignoredRotates++;
	
		sx = sprite.transform[SPRITE_TRANSFORM_SCALE_X] * scalex;
		sy = sprite.transform[SPRITE_TRANSFORM_SCALE_Y] * scaley;
	
		if (control & CONTROL_FLIP_H)
			sx = -sx;
	
		if (control & CONTROL_FLIP_V)
			sy = -sy;
	
		if (sprite.control & CONTROL_FLIP_H)
			sx = -sx;
	
		if (sprite.control & CONTROL_FLIP_V)
			sy = -sy;

		if (sx != 1.0 || sy != 1.0)
		{
			glScalef(sx, sy, 1.0F);
			_rendererStats.scales++;
		}
		else
			_rendererStats.ignoredScales++;
	}

	_colorBlendStack[++_colorBlendStackIndex] = _colorBlendStack[_colorBlendStackIndex-1] | (sprite.control & (CONTROL_TRANSFORM_COLOR_MULTIPLIER | CONTROL_TRANSFORM_COLOR_OFFSET));

	if (sprite.control & CONTROL_TRANSFORM_COLOR_MULTIPLIER)
	{	
		_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex-1].r * sprite.colorOp->redMultiplier;

		if (_colorMultiplier[_colorBlendStackIndex].r > 1.0F)
			_colorMultiplier[_colorBlendStackIndex].r = 1.0F;

		_colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex-1].g * sprite.colorOp->greenMultiplier;

		if (_colorMultiplier[_colorBlendStackIndex].g > 1.0F)
			_colorMultiplier[_colorBlendStackIndex].g = 1.0F;

		_colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex-1].b * sprite.colorOp->blueMultiplier;

		if (_colorMultiplier[_colorBlendStackIndex].b > 1.0F)
			_colorMultiplier[_colorBlendStackIndex].b = 1.0F;

		_colorMultiplier[_colorBlendStackIndex].a = _colorMultiplier[_colorBlendStackIndex-1].a * sprite.colorOp->alphaMultiplier;

		if (_colorMultiplier[_colorBlendStackIndex].a > 1.0F)
			_colorMultiplier[_colorBlendStackIndex].a = 1.0F;

	}
	else if (sprite.control & CONTROL_TRANSFORM_COLOR_OFFSET)
	{
		_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex-1].r;
		_colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex-1].g;
		_colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex-1].b;
		_colorMultiplier[_colorBlendStackIndex].a = _colorMultiplier[_colorBlendStackIndex-1].a;
	}

	if (sprite.control & CONTROL_TRANSFORM_COLOR_OFFSET)
	{	
		_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex-1].r + sprite.colorOp->redOffset;

		if (_colorOffset[_colorBlendStackIndex].r > 1.0F)
			_colorOffset[_colorBlendStackIndex].r = 1.0F;

		_colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex-1].g + sprite.colorOp->greenOffset;

		if (_colorOffset[_colorBlendStackIndex].g > 1.0F)
			_colorOffset[_colorBlendStackIndex].g = 1.0F;

		_colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex-1].b + sprite.colorOp->blueOffset;

		if (_colorOffset[_colorBlendStackIndex].b > 1.0F)
			_colorOffset[_colorBlendStackIndex].b = 1.0F;

		_colorOffset[_colorBlendStackIndex].a = _colorOffset[_colorBlendStackIndex-1].a + sprite.colorOp->alphaOffset;

		if (_colorOffset[_colorBlendStackIndex].a > 1.0F)
			_colorOffset[_colorBlendStackIndex].a = 1.0F;
	}
	else
	{
		_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex-1].r;
		_colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex-1].g;
		_colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex-1].b;
		_colorOffset[_colorBlendStackIndex].a = _colorOffset[_colorBlendStackIndex-1].a;
	}

	mapping = sprite.children;

	for (i=0;i<sprite.numChildren;i++)
	{
		if (mapping->mapControl & CONTROL_TYPE_SPRITEMASTER)
		{
			Sprite* next;

			next = spriteSet->getSprite(mapping->offset);

			drawSprite(*next);
		}
		else if (mapping->mapControl & CONTROL_TYPE_SPRITENORMAL)
		{
			SubSprite*  next;

			next = spriteSet->getSubSprite(mapping->offset);

			drawSubSprite(*next);
		}
		else
		{
			TextureAtlasSubTexture* texref;

			texref = spriteSet->getTextureAtlasSubTexture(mapping->offset);

			drawSubTexture(*texref);
		}

		mapping++;
	}

	_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex].a = 1.0F;
	_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex].a = 0.0F;

	_colorBlendStackIndex--;

	setPrimaryColor(_colorMultiplier[_colorBlendStackIndex]);

	glPopMatrix();
#endif /* _USE_OPENGL */	
}

void Renderer::drawSprite(const Sprite& sprite, const Point2D& pos, Float32 scalex, Float32 scaley, Float32 rotate, DrawControl control, SpriteColorOp& colorOp)
{
#ifdef _USE_OPENGL	
	Uint32                  i;
	SpriteMapping*          mapping;
	SpriteSet*              spriteSet = sprite.spriteSet;
	Float32                 sx, sy;

	glPushMatrix();

//	glLoadIdentity();

	if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		glTranslatef(-sprite.xy->x + pos.x, _displayH - (-sprite.xy->y + pos.y), 0.0);
	else
		glTranslatef(-sprite.xy->x + pos.x, -sprite.xy->y + pos.y, 0.0);

	if (sprite.control & CONTROL_TRANSFORM_MATRIX)
	{
		applySkew(sprite.transform, _tmpMatrix);

		if (rotate != 0.0)
		{
			glRotatef(rotate, 0, 0, 1);
			_rendererStats.rotates++;
		}
		else
			_rendererStats.ignoredRotates++;

		if (scalex != 1.0F || scaley != 1.0F)
			glScalef(scalex, scaley, 1.0F);
		else
			_rendererStats.ignoredScales++;
	}
	else
	{
		if ((sprite.transform[SPRITE_TRANSFORM_ROTATE_Z] + rotate) != 0.0)
		{
			glRotatef(-sprite.transform[SPRITE_TRANSFORM_ROTATE_Z] + rotate, 0, 0, 1);
			_rendererStats.rotates++;
		}
		else
			_rendererStats.ignoredRotates++;
	
		sx = sprite.transform[SPRITE_TRANSFORM_SCALE_X] * scalex;
		sy = sprite.transform[SPRITE_TRANSFORM_SCALE_Y] * scaley;
	
		if (control & CONTROL_FLIP_H)
			sx = -sx;
	
		if (control & CONTROL_FLIP_V)
			sy = -sy;
	
		if (sprite.control & CONTROL_FLIP_H)
			sx = -sx;
	
		if (sprite.control & CONTROL_FLIP_V)
			sy = -sy;

		if (sx != 1.0 || sy != 1.0)
		{
			glScalef(sx, sy, 1.0F);
			_rendererStats.scales++;
		}
		else
			_rendererStats.ignoredScales++;
	}

	_colorBlendStack[++_colorBlendStackIndex] = (CONTROL_TRANSFORM_COLOR_MULTIPLIER | CONTROL_TRANSFORM_COLOR_OFFSET);

	_colorMultiplier[_colorBlendStackIndex].r = colorOp.redMultiplier;
	_colorMultiplier[_colorBlendStackIndex].g = colorOp.greenMultiplier;
	_colorMultiplier[_colorBlendStackIndex].b = colorOp.blueMultiplier;
	_colorMultiplier[_colorBlendStackIndex].a = colorOp.alphaMultiplier;
	_colorOffset[_colorBlendStackIndex].r = colorOp.redOffset;
	_colorOffset[_colorBlendStackIndex].g = colorOp.greenOffset;
	_colorOffset[_colorBlendStackIndex].b = colorOp.blueOffset;
	_colorOffset[_colorBlendStackIndex].a = colorOp.alphaOffset;

	mapping = sprite.children;

	for (i=0;i<sprite.numChildren;i++)
	{
		if (mapping->mapControl & CONTROL_TYPE_SPRITEMASTER)
		{
			Sprite* next;

			next = spriteSet->getSprite(mapping->offset);

			drawSprite(*next, 0, colorOp);
		}
		else if (mapping->mapControl & CONTROL_TYPE_SPRITENORMAL)
		{
			SubSprite*  next;

			next = spriteSet->getSubSprite(mapping->offset);

			drawSubSprite(*next);
		}
		else
		{
			TextureAtlasSubTexture* texref;

			texref = spriteSet->getTextureAtlasSubTexture(mapping->offset);

			drawSubTexture(*texref);
		}

		mapping++;
	}

	_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex].a = 1.0F;
	_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex].a = 0.0F;

	_colorBlendStackIndex--;

	setPrimaryColor(_colorMultiplier[_colorBlendStackIndex]);

	glPopMatrix();
#endif /* _USE_OPENGL */	
}

void Renderer::drawSprite(const Sprite& sprite, DrawControl control)
{
#ifdef _USE_OPENGL
	Uint32                  i;
	SpriteMapping*          mapping;
	SpriteSet*              spriteSet = sprite.spriteSet;
	Float32                 sx, sy;

	glPushMatrix();

//	glLoadIdentity();

	glTranslatef(sprite.xy->x, -sprite.xy->y, 0.0);

	if (sprite.control & CONTROL_TRANSFORM_MATRIX)
	{
		applySkew(sprite.transform, _tmpMatrix);
	}
	else
	{
		if (sprite.transform[SPRITE_TRANSFORM_ROTATE_Z] != 0.0)
		{
			glRotatef(-sprite.transform[SPRITE_TRANSFORM_ROTATE_Z], 0, 0, 1);
			_rendererStats.rotates++;
		}
		else
			_rendererStats.ignoredRotates++;
	
		sx = sprite.transform[SPRITE_TRANSFORM_SCALE_X];
		sy = sprite.transform[SPRITE_TRANSFORM_SCALE_Y];
	
		if (control & CONTROL_FLIP_H)
			sx = -sx;
	
		if (control & CONTROL_FLIP_V)
			sy = -sy;
	
		if (sprite.control & CONTROL_FLIP_H)
			sx = -sx;
	
		if (sprite.control & CONTROL_FLIP_V)
			sy = -sy;

		if (sx != 1.0 || sy != 1.0)
		{
   			glScalef(sx, sy, 1.0F);
			_rendererStats.scales++;
		}
		else
			_rendererStats.ignoredScales++;
	}

	_colorBlendStack[++_colorBlendStackIndex] = _colorBlendStack[_colorBlendStackIndex-1] | (sprite.control & (CONTROL_TRANSFORM_COLOR_MULTIPLIER | CONTROL_TRANSFORM_COLOR_OFFSET));

	if (sprite.control & CONTROL_TRANSFORM_COLOR_MULTIPLIER)
	{	
		_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex-1].r * sprite.colorOp->redMultiplier;

		if (_colorMultiplier[_colorBlendStackIndex].r > 1.0F)
			_colorMultiplier[_colorBlendStackIndex].r = 1.0F;

		_colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex-1].g * sprite.colorOp->greenMultiplier;

		if (_colorMultiplier[_colorBlendStackIndex].g > 1.0F)
			_colorMultiplier[_colorBlendStackIndex].g = 1.0F;

		_colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex-1].b * sprite.colorOp->blueMultiplier;

		if (_colorMultiplier[_colorBlendStackIndex].b > 1.0F)
			_colorMultiplier[_colorBlendStackIndex].b = 1.0F;

		_colorMultiplier[_colorBlendStackIndex].a = _colorMultiplier[_colorBlendStackIndex-1].a * sprite.colorOp->alphaMultiplier;

		if (_colorMultiplier[_colorBlendStackIndex].a > 1.0F)
			_colorMultiplier[_colorBlendStackIndex].a = 1.0F;

	}
	else if (sprite.control & CONTROL_TRANSFORM_COLOR_OFFSET)
	{
		_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex-1].r;
		_colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex-1].g;
		_colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex-1].b;
		_colorMultiplier[_colorBlendStackIndex].a = _colorMultiplier[_colorBlendStackIndex-1].a;
	}

	if (sprite.control & CONTROL_TRANSFORM_COLOR_OFFSET)
	{	
		_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex-1].r + sprite.colorOp->redOffset;

		if (_colorOffset[_colorBlendStackIndex].r > 1.0F)
			_colorOffset[_colorBlendStackIndex].r = 1.0F;

		_colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex-1].g + sprite.colorOp->greenOffset;

		if (_colorOffset[_colorBlendStackIndex].g > 1.0F)
			_colorOffset[_colorBlendStackIndex].g = 1.0F;

		_colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex-1].b + sprite.colorOp->blueOffset;

		if (_colorOffset[_colorBlendStackIndex].b > 1.0F)
			_colorOffset[_colorBlendStackIndex].b = 1.0F;

		_colorOffset[_colorBlendStackIndex].a = _colorOffset[_colorBlendStackIndex-1].a + sprite.colorOp->alphaOffset;

		if (_colorOffset[_colorBlendStackIndex].a > 1.0F)
			_colorOffset[_colorBlendStackIndex].a = 1.0F;
	}
	else
	{
		_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex-1].r;
		_colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex-1].g;
		_colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex-1].b;
		_colorOffset[_colorBlendStackIndex].a = _colorOffset[_colorBlendStackIndex-1].a;
	}

	mapping = sprite.children;

	for (i=0;i<sprite.numChildren;i++)
	{
		if (mapping->mapControl & CONTROL_TYPE_SPRITEMASTER)
		{
			Sprite* next;

			next = spriteSet->getSprite(mapping->offset);

			drawSprite(*next);
		}
		else if (mapping->mapControl & CONTROL_TYPE_SPRITENORMAL)
		{
			SubSprite*  next;

			next = spriteSet->getSubSprite(mapping->offset);

			drawSubSprite(*next);
		}
		else
		{
			TextureAtlasSubTexture* texref;

			texref = spriteSet->getTextureAtlasSubTexture(mapping->offset);

			drawSubTexture(*texref);
		}

		mapping++;
	}

	_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex].a = 1.0F;
	_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex].a = 0.0F;

	_colorBlendStackIndex--;

	setPrimaryColor(_colorMultiplier[_colorBlendStackIndex]);

	glPopMatrix();
#endif /* _USE_OPENGL */
}

void Renderer::drawSprite(const Sprite& sprite, DrawControl control, SpriteColorOp& colorOp)
{
#ifdef _USE_OPENGL
	Uint32                  i;
	SpriteMapping*          mapping;
	SpriteSet*              spriteSet = sprite.spriteSet;
	Float32                 sx, sy;

	glPushMatrix();

//	glLoadIdentity();

	glTranslatef(sprite.xy->x, -sprite.xy->y, 0.0);

	if (sprite.control & CONTROL_TRANSFORM_MATRIX)
	{
		applySkew(sprite.transform, _tmpMatrix);
	}
	else
	{
		if (sprite.transform[SPRITE_TRANSFORM_ROTATE_Z] != 0.0)
		{
			glRotatef(-sprite.transform[SPRITE_TRANSFORM_ROTATE_Z], 0, 0, 1);
			_rendererStats.rotates++;
		}
		else
			_rendererStats.ignoredRotates++;
	
		sx = sprite.transform[SPRITE_TRANSFORM_SCALE_X];
		sy = sprite.transform[SPRITE_TRANSFORM_SCALE_Y];
	
		if (control & CONTROL_FLIP_H)
			sx = -sx;
	
		if (control & CONTROL_FLIP_V)
			sy = -sy;
	
		if (sprite.control & CONTROL_FLIP_H)
			sx = -sx;
	
		if (sprite.control & CONTROL_FLIP_V)
			sy = -sy;

		if (sx != 1.0 || sy != 1.0)
		{
   			glScalef(sx, sy, 1.0F);
			_rendererStats.scales++;
		}
		else
			_rendererStats.ignoredScales++;
	}

	_colorBlendStack[++_colorBlendStackIndex] = (CONTROL_TRANSFORM_COLOR_MULTIPLIER | CONTROL_TRANSFORM_COLOR_OFFSET);

	_colorMultiplier[_colorBlendStackIndex].r = colorOp.redMultiplier;
	_colorMultiplier[_colorBlendStackIndex].g = colorOp.greenMultiplier;
	_colorMultiplier[_colorBlendStackIndex].b = colorOp.blueMultiplier;
	_colorMultiplier[_colorBlendStackIndex].a = colorOp.alphaMultiplier;
	_colorOffset[_colorBlendStackIndex].r = colorOp.redOffset;
	_colorOffset[_colorBlendStackIndex].g = colorOp.greenOffset;
	_colorOffset[_colorBlendStackIndex].b = colorOp.blueOffset;
	_colorOffset[_colorBlendStackIndex].a = colorOp.alphaOffset;

	mapping = sprite.children;

	for (i=0;i<sprite.numChildren;i++)
	{
		if (mapping->mapControl & CONTROL_TYPE_SPRITEMASTER)
		{
			Sprite* next;

			next = spriteSet->getSprite(mapping->offset);

			drawSprite(*next, 0, colorOp);
		}
		else if (mapping->mapControl & CONTROL_TYPE_SPRITENORMAL)
		{
			SubSprite*  next;

			next = spriteSet->getSubSprite(mapping->offset);

			drawSubSprite(*next);
		}
		else
		{
			TextureAtlasSubTexture* texref;

			texref = spriteSet->getTextureAtlasSubTexture(mapping->offset);

			drawSubTexture(*texref);
		}

		mapping++;
	}

	_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex].a = 1.0F;
	_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex].a = 0.0F;

	_colorBlendStackIndex--;

	setPrimaryColor(_colorMultiplier[_colorBlendStackIndex]);

	glPopMatrix();
#endif /* _USE_OPENGL */
}

void Renderer::drawSubSprite(const SubSprite& sprite)
{
#ifdef _USE_OPENGL
	Uint32          i;
	SpriteMapping*  mapping;
	SpriteSet*      spriteSet = sprite.spriteSet;
	Float32         sx, sy;

	glPushMatrix();

	glTranslatef(sprite.xy->x, -sprite.xy->y, 0.0);

	if (sprite.control & CONTROL_TRANSFORM_MATRIX)
	{
		applySkew(sprite.transform, _tmpMatrix);
	}
	else
	{
		if (sprite.transform[SPRITE_TRANSFORM_ROTATE_Z] != 0.0)
		{
   			glRotatef(-sprite.transform[SPRITE_TRANSFORM_ROTATE_Z], 0, 0, 1);
			_rendererStats.rotates++;
		}
		else
			_rendererStats.ignoredRotates++;
	
		sx = sprite.transform[SPRITE_TRANSFORM_SCALE_X];
		sy = sprite.transform[SPRITE_TRANSFORM_SCALE_Y];
	
		if (sprite.control & CONTROL_FLIP_H)
			sx = -sx;
	
		if (sprite.control & CONTROL_FLIP_V)
			sy = -sy;

		if (sx != 1.0 || sy != 1.0)
		{
   			glScalef(sx, sy, 1.0F);
			_rendererStats.scales++;
		}
		else
			_rendererStats.ignoredScales++;
	}

	if (sprite.control & (CONTROL_TRANSFORM_COLOR_MULTIPLIER | CONTROL_TRANSFORM_COLOR_OFFSET))
	{
		_colorBlendStack[++_colorBlendStackIndex] = _colorBlendStack[_colorBlendStackIndex-1] | (sprite.control & (CONTROL_TRANSFORM_COLOR_MULTIPLIER | CONTROL_TRANSFORM_COLOR_OFFSET));

		if (sprite.control & CONTROL_TRANSFORM_COLOR_MULTIPLIER)
		{	
			_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex-1].r * sprite.colorOp->redMultiplier;

			if (_colorMultiplier[_colorBlendStackIndex].r > 1.0F)
				_colorMultiplier[_colorBlendStackIndex].r = 1.0F;

			_colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex-1].g * sprite.colorOp->greenMultiplier;

			if (_colorMultiplier[_colorBlendStackIndex].g > 1.0F)
				_colorMultiplier[_colorBlendStackIndex].g = 1.0F;

			_colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex-1].b * sprite.colorOp->blueMultiplier;

			if (_colorMultiplier[_colorBlendStackIndex].b > 1.0F)
				_colorMultiplier[_colorBlendStackIndex].b = 1.0F;

			_colorMultiplier[_colorBlendStackIndex].a = _colorMultiplier[_colorBlendStackIndex-1].a * sprite.colorOp->alphaMultiplier;

			if (_colorMultiplier[_colorBlendStackIndex].a > 1.0F)
				_colorMultiplier[_colorBlendStackIndex].a = 1.0F;

		}
		else if (sprite.control & CONTROL_TRANSFORM_COLOR_OFFSET)
		{
			_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex-1].r;
			_colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex-1].g;
			_colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex-1].b;
			_colorMultiplier[_colorBlendStackIndex].a = _colorMultiplier[_colorBlendStackIndex-1].a;
		}

		if (sprite.control & CONTROL_TRANSFORM_COLOR_OFFSET)
		{	
			_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex-1].r + sprite.colorOp->redOffset;

			if (_colorOffset[_colorBlendStackIndex].r > 1.0F)
				_colorOffset[_colorBlendStackIndex].r = 1.0F;

			_colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex-1].g + sprite.colorOp->greenOffset;

			if (_colorOffset[_colorBlendStackIndex].g > 1.0F)
				_colorOffset[_colorBlendStackIndex].g = 1.0F;

			_colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex-1].b + sprite.colorOp->blueOffset;

			if (_colorOffset[_colorBlendStackIndex].b > 1.0F)
				_colorOffset[_colorBlendStackIndex].b = 1.0F;

			_colorOffset[_colorBlendStackIndex].a = _colorOffset[_colorBlendStackIndex-1].a + sprite.colorOp->alphaOffset;

			if (_colorOffset[_colorBlendStackIndex].a > 1.0F)
				_colorOffset[_colorBlendStackIndex].a = 1.0F;
		}
		else
		{
			_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex-1].r;
			_colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex-1].g;
			_colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex-1].b;
			_colorOffset[_colorBlendStackIndex].a = _colorOffset[_colorBlendStackIndex-1].a;
		}
	}
	else if (_colorBlendStack[_colorBlendStackIndex] & (CONTROL_TRANSFORM_COLOR_MULTIPLIER | CONTROL_TRANSFORM_COLOR_OFFSET))
	{
		_colorBlendStack[++_colorBlendStackIndex] = _colorBlendStack[_colorBlendStackIndex - 1];

		_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex - 1].r;
		_colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex - 1].g;
		_colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex - 1].b;
		_colorMultiplier[_colorBlendStackIndex].a = _colorMultiplier[_colorBlendStackIndex - 1].a;

		_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex - 1].r;
		_colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex - 1].g;
		_colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex - 1].b;
		_colorOffset[_colorBlendStackIndex].a = _colorOffset[_colorBlendStackIndex - 1].a;
	}
	else
	{
		_colorBlendStack[++_colorBlendStackIndex] = 0;

		_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex - 1].r;
		_colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex - 1].g;
		_colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex - 1].b;
		_colorMultiplier[_colorBlendStackIndex].a = _colorMultiplier[_colorBlendStackIndex - 1].a;

		_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex - 1].r;
		_colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex - 1].g;
		_colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex - 1].b;
		_colorOffset[_colorBlendStackIndex].a = _colorOffset[_colorBlendStackIndex - 1].a;
	}

	mapping = sprite.children;

	for (i=0;i<sprite.numChildren;i++)
	{
		if (mapping->mapControl & CONTROL_TYPE_SPRITEMASTER)
		{
			Sprite* next;

			next = spriteSet->getSprite(mapping->offset);
			drawSprite(*next);

		}
		else if (mapping->mapControl & CONTROL_TYPE_SPRITENORMAL)
		{
			SubSprite*  next;

			next = spriteSet->getSubSprite(mapping->offset);

			drawSubSprite(*next);
		}
		else
		{
			TextureAtlasSubTexture* texref;

			texref = spriteSet->getTextureAtlasSubTexture(mapping->offset);

			drawSubTexture(*texref);
		}

		mapping++;
	}

	_colorMultiplier[_colorBlendStackIndex].r = _colorMultiplier[_colorBlendStackIndex].g = _colorMultiplier[_colorBlendStackIndex].b = _colorMultiplier[_colorBlendStackIndex].a = 1.0F;
	_colorOffset[_colorBlendStackIndex].r = _colorOffset[_colorBlendStackIndex].g = _colorOffset[_colorBlendStackIndex].b = _colorOffset[_colorBlendStackIndex].a = 0.0F;

	_colorBlendStackIndex--;

	setPrimaryColor(_colorMultiplier[_colorBlendStackIndex]);

	glPopMatrix();
#endif /* _USE_OPENGL */
}

void Renderer::drawSubTexture(const TextureAtlasSubTexture& tex, Uint32 control)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];
	Boolean		multitex = false;

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
#ifdef _USE_OPENGL
		Texture*    texref = tex.atlas->getTextureState()->_textureObj;

		setTextureUnit(RendererTextureUnit0);
		enableTexturing();
		setTexture(texref->getHWTextureID());

		if (_colorBlendStack[_colorBlendStackIndex] & (CONTROL_TRANSFORM_COLOR_MULTIPLIER | CONTROL_TRANSFORM_COLOR_OFFSET))
		{
			if (_colorBlendStack[_colorBlendStackIndex] & CONTROL_TRANSFORM_COLOR_OFFSET)
			{
				setPrimaryColor(_colorMultiplier[_colorBlendStackIndex]);
//				setPrimaryColor(_colorOffset[_colorBlendStackIndex]);


				setTextureEnvMode(TextureState::TEXTURE_ENV_MODE_MODULATE);

//				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
//				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);

//				glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, &_colorMultiplier[_colorBlendStackIndex].r);

				setTextureUnit(RendererTextureUnit1);
				enableTexturing();
//				setTexture(texref->getHWTextureID());
				setTexture(_appRefPtr->getTextureManager()->getColorBlendTexture()->getHWTextureID());

				setTextureEnvMode(TextureState::TEXTURE_ENV_MODE_COMBINE, RendererTextureUnit1);

#if 1
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_ADD);
				glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS);
				glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PREVIOUS);
				glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_CONSTANT);
				glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_CONSTANT);
#else
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_ADD);
				glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS);
				glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PREVIOUS);

//				glTexEnvf(GL_TEXTURE_ENV, GL_SRC2_RGB, GL_TEXTURE);
//				glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_TEXTURE);
//				glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_CONSTANT);
//				glTexEnvf(GL_TEXTURE_ENV, GL_SRC2_ALPHA, GL_CONSTANT);
//				glTexEnvf(GL_TEXTURE_ENV, GL_SRC2_RGB, GL_PRIMARY_COLOR);
//				glTexEnvf(GL_TEXTURE_ENV, GL_SRC2_ALPHA, GL_PRIMARY_COLOR);
//				glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
//				glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA, GL_SRC_ALPHA);

#endif

				glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, &_colorOffset[_colorBlendStackIndex].r);

				multitex = true;
			}
			else
			{
				setTextureEnvMode(TextureState::TEXTURE_ENV_MODE_MODULATE);

				setPrimaryColor(_colorMultiplier[_colorBlendStackIndex]);
			}
		}
		else if (texref->getBoundTextureState())
			setTextureEnvMode(texref->getBoundTextureState()->_mode);
		else
			setTextureEnvMode(TextureState::TEXTURE_ENV_MODE_REPLACE);

#ifdef _USE_OPENGL_ES_1_1

		enableVertArrayClientState();
		disableColorArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);

		if (multitex)
		{
			enableTexCoordArrayClientState();

			setTexCoordArray((Float32*) tex.data.PT2D_DIRECT.uvs);

			setTextureUnit(RendererTextureUnit0);
		}

		enableTexCoordArrayClientState();

		setTexCoordArray((Float32*) tex.data.PT2D_DIRECT.uvs);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
			_rendererStats.texPolys += 2;
		}

#else


#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_QUADS);

		glVertex2f(tex.data.PT2D_PTR.vertices[0]->x, tex.data.PT2D_PTR.vertices[0]->y);

		if (multitex)
		{
			glMultiTexCoord2f(GL_TEXTURE0, tex.data.PT2D_PTR.uvs[0]->x, tex.data.PT2D_PTR.uvs[0]->y);
			glMultiTexCoord2f(GL_TEXTURE1, tex.data.PT2D_PTR.uvs[0]->x, tex.data.PT2D_PTR.uvs[0]->y);
		}
		else
			glTexCoord2f(tex.data.PT2D_PTR.uvs[0]->x, tex.data.PT2D_PTR.uvs[0]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[1]->x, tex.data.PT2D_PTR.vertices[1]->y);

		if (multitex)
		{
			glMultiTexCoord2f(GL_TEXTURE0, tex.data.PT2D_PTR.uvs[1]->x, tex.data.PT2D_PTR.uvs[1]->y);
			glMultiTexCoord2f(GL_TEXTURE1, tex.data.PT2D_PTR.uvs[1]->x, tex.data.PT2D_PTR.uvs[1]->y);
		}
		else
			glTexCoord2f(tex.data.PT2D_PTR.uvs[1]->x, tex.data.PT2D_PTR.uvs[1]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[2]->x, tex.data.PT2D_PTR.vertices[2]->y);

		if (multitex)
		{
			glMultiTexCoord2f(GL_TEXTURE0, tex.data.PT2D_PTR.uvs[2]->x, tex.data.PT2D_PTR.uvs[2]->y);
			glMultiTexCoord2f(GL_TEXTURE1, tex.data.PT2D_PTR.uvs[2]->x, tex.data.PT2D_PTR.uvs[2]->y);
		}
		else
			glTexCoord2f(tex.data.PT2D_PTR.uvs[2]->x, tex.data.PT2D_PTR.uvs[2]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[3]->x, tex.data.PT2D_PTR.vertices[3]->y);

		if (multitex)
		{
			glMultiTexCoord2f(GL_TEXTURE0, tex.data.PT2D_PTR.uvs[3]->x, tex.data.PT2D_PTR.uvs[3]->y);
			glMultiTexCoord2f(GL_TEXTURE1, tex.data.PT2D_PTR.uvs[3]->x, tex.data.PT2D_PTR.uvs[3]->y);
		}
		else
			glTexCoord2f(tex.data.PT2D_PTR.uvs[3]->x, tex.data.PT2D_PTR.uvs[3]->y);

		glEnd();

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 1;
			_rendererStats.texPolys += 1;
		}

#else

		enableVertArrayClientState();
		disableColorArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);

		if (multitex)
		{
			enableTexCoordArrayClientState();

			setTexCoordArray((Float32*) tex.data.PT2D_DIRECT.uvs);

			setTextureUnit(RendererTextureUnit0);
		}

		enableTexCoordArrayClientState();

		setTexCoordArray((Float32*) tex.data.PT2D_DIRECT.uvs);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
			_rendererStats.texPolys += 2;
		}

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

		if (multitex)
		{
			setTextureUnit(RendererTextureUnit1);
			disableTexturing();

			setTextureUnit(RendererTextureUnit0);
		}

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

		disableTexturing();

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		glVertex2f(tex.data.PT2D_PTR.vertices[0]->x, tex.data.PT2D_PTR.vertices[0]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[1]->x, tex.data.PT2D_PTR.vertices[1]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[2]->x, tex.data.PT2D_PTR.vertices[2]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[3]->x, tex.data.PT2D_PTR.vertices[3]->y);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else
		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}
}

void Renderer::drawSubTexture(const TextureAtlasSubTexture& tex, const Point2D& pos, Uint32 control)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

#ifdef _USE_OPENGL
	glPushMatrix();

	if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		glTranslatef(pos.x, _displayH - pos.y, 0.0F);
	else
		glTranslatef(pos.x, pos.y, 0.0F);

	if (control & (CONTROL_FLIP_H | CONTROL_FLIP_V))
	{
		Float32 sx = 1.0F;
		Float32 sy = 1.0F;
		
		if (control & CONTROL_FLIP_H)
			sx = -sx;
		if (control & CONTROL_FLIP_V)
			sy = -sy;
		
		glScalef(sx, sy, 1.0F);
		_rendererStats.scales++;
	}
#endif /* _USE_OPENGL */

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
#ifdef _USE_OPENGL
		Texture*    texref = tex.atlas->getTextureState()->_textureObj;


		setTextureUnit(RendererTextureUnit0);
		enableTexturing();
		setTexture(texref->getHWTextureID());

		if (texref->getBoundTextureState())
			setTextureEnvMode(texref->getBoundTextureState()->_mode);
		else
			setTextureEnvMode(TextureState::TEXTURE_ENV_MODE_REPLACE);

#ifdef _USE_OPENGL_ES_1_1

		enableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);
		setTexCoordArray((Float32*) tex.data.PT2D_DIRECT.uvs);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
			_rendererStats.texPolys += 2;
		}

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_QUADS);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[0]->x, tex.data.PT2D_PTR.uvs[0]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[0]->x, tex.data.PT2D_PTR.vertices[0]->y);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[1]->x, tex.data.PT2D_PTR.uvs[1]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[1]->x, tex.data.PT2D_PTR.vertices[1]->y);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[2]->x, tex.data.PT2D_PTR.uvs[2]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[2]->x, tex.data.PT2D_PTR.vertices[2]->y);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[3]->x, tex.data.PT2D_PTR.uvs[3]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[3]->x, tex.data.PT2D_PTR.vertices[3]->y);

		glEnd();

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 1;
			_rendererStats.texPolys += 1;
		}

#else

		enableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);
		setTexCoordArray((Float32*) tex.data.PT2D_DIRECT.uvs);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
			_rendererStats.texPolys += 2;
		}

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

		disableTexturing();

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		glVertex2f(tex.data.PT2D_PTR.vertices[0]->x, tex.data.PT2D_PTR.vertices[0]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[1]->x, tex.data.PT2D_PTR.vertices[1]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[2]->x, tex.data.PT2D_PTR.vertices[2]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[3]->x, tex.data.PT2D_PTR.vertices[3]->y);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else
		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

#ifdef _USE_OPENGL
	glPopMatrix();
#endif /* _USE_OPENGL */
}

void Renderer::drawSubTexture(const TextureAtlasSubTexture& tex, const Point2D& pos, Float32 rotate, Uint32 control)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

#ifdef _USE_OPENGL
	glPushMatrix();

	if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		glTranslatef(pos.x, _displayH - pos.y, 0.0F);
	else
		glTranslatef(pos.x, pos.y, 0.0F);

	glRotatef(rotate, 0, 0, 1);
	_rendererStats.rotates++;
	
	if (control & (CONTROL_FLIP_H | CONTROL_FLIP_V))
	{
		Float32 sx = 1.0F;
		Float32 sy = 1.0F;
		
		if (control & CONTROL_FLIP_H)
			sx = -sx;
		if (control & CONTROL_FLIP_V)
			sy = -sy;
		
		glScalef(sx, sy, 1.0F);
		_rendererStats.scales++;
	}
	
#endif /* _USE_OPENGL */

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
#ifdef _USE_OPENGL
		Texture*    texref = tex.atlas->getTextureState()->_textureObj;


		setTextureUnit(RendererTextureUnit0);
		enableTexturing();
		setTexture(texref->getHWTextureID());

		if (texref->getBoundTextureState())
			setTextureEnvMode(texref->getBoundTextureState()->_mode);
		else
			setTextureEnvMode(TextureState::TEXTURE_ENV_MODE_REPLACE);

#ifdef _USE_OPENGL_ES_1_1

		enableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);
		setTexCoordArray((Float32*) tex.data.PT2D_DIRECT.uvs);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
			_rendererStats.texPolys += 2;
		}

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_QUADS);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[0]->x, tex.data.PT2D_PTR.uvs[0]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[0]->x, tex.data.PT2D_PTR.vertices[0]->y);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[1]->x, tex.data.PT2D_PTR.uvs[1]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[1]->x, tex.data.PT2D_PTR.vertices[1]->y);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[2]->x, tex.data.PT2D_PTR.uvs[2]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[2]->x, tex.data.PT2D_PTR.vertices[2]->y);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[3]->x, tex.data.PT2D_PTR.uvs[3]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[3]->x, tex.data.PT2D_PTR.vertices[3]->y);

		glEnd();

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 1;
			_rendererStats.texPolys += 1;
		}

#else

		enableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);
		setTexCoordArray((Float32*) tex.data.PT2D_DIRECT.uvs);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
			_rendererStats.texPolys += 2;
		}

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

		disableTexturing();

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		glVertex2f(tex.data.PT2D_PTR.vertices[0]->x, tex.data.PT2D_PTR.vertices[0]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[1]->x, tex.data.PT2D_PTR.vertices[1]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[2]->x, tex.data.PT2D_PTR.vertices[2]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[3]->x, tex.data.PT2D_PTR.vertices[3]->y);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else
		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

#ifdef _USE_OPENGL
	glPopMatrix();
#endif /* _USE_OPENGL */
}

void Renderer::drawSubTexture(const TextureAtlasSubTexture& tex, const Point2D& pos, Float32 rotate, Float32 scale, Uint32 control)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

#ifdef _USE_OPENGL
	glPushMatrix();

	if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		glTranslatef(pos.x, _displayH - pos.y, 0.0F);
	else
		glTranslatef(pos.x, pos.y, 0.0F);

	glRotatef(rotate, 0, 0, 1);
	_rendererStats.rotates++;

	if (scale != 1.0F || control & (CONTROL_FLIP_H | CONTROL_FLIP_V))
	{
		Float32 sx = scale;
		Float32 sy = scale;
		
		if (control & (CONTROL_FLIP_H | CONTROL_FLIP_V))
		{
			if (control & CONTROL_FLIP_H)
				sx = -sx;
			if (control & CONTROL_FLIP_V)
				sy = -sy;
		}
		
		glScalef(sx, sy, 1.0F);
		_rendererStats.scales++;
	}
	else
		_rendererStats.ignoredScales++;
#endif /* _USE_OPENGL */

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
#ifdef _USE_OPENGL
		Texture*    texref = tex.atlas->getTextureState()->_textureObj;


		setTextureUnit(RendererTextureUnit0);
		enableTexturing();
		setTexture(texref->getHWTextureID());

		if (texref->getBoundTextureState())
			setTextureEnvMode(texref->getBoundTextureState()->_mode);
		else
			setTextureEnvMode(TextureState::TEXTURE_ENV_MODE_REPLACE);

#ifdef _USE_OPENGL_ES_1_1

		enableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);
		setTexCoordArray((Float32*) tex.data.PT2D_DIRECT.uvs);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
			_rendererStats.texPolys += 2;
		}

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_QUADS);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[0]->x, tex.data.PT2D_PTR.uvs[0]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[0]->x, tex.data.PT2D_PTR.vertices[0]->y);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[1]->x, tex.data.PT2D_PTR.uvs[1]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[1]->x, tex.data.PT2D_PTR.vertices[1]->y);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[2]->x, tex.data.PT2D_PTR.uvs[2]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[2]->x, tex.data.PT2D_PTR.vertices[2]->y);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[3]->x, tex.data.PT2D_PTR.uvs[3]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[3]->x, tex.data.PT2D_PTR.vertices[3]->y);

		glEnd();

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 1;
			_rendererStats.texPolys += 1;
		}

#else

		enableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);
		setTexCoordArray((Float32*) tex.data.PT2D_DIRECT.uvs);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
			_rendererStats.texPolys += 2;
		}

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

		disableTexturing();

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		glVertex2f(tex.data.PT2D_PTR.vertices[0]->x, tex.data.PT2D_PTR.vertices[0]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[1]->x, tex.data.PT2D_PTR.vertices[1]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[2]->x, tex.data.PT2D_PTR.vertices[2]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[3]->x, tex.data.PT2D_PTR.vertices[3]->y);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else
		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

#ifdef _USE_OPENGL
	glPopMatrix();
#endif /* _USE_OPENGL */
}

void Renderer::drawSubTexture(const TextureAtlasSubTexture& tex, const Point2D& pos, const ColorRGBA& rgba, Uint32 control)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

#ifdef _USE_OPENGL
	glPushMatrix();

	if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		glTranslatef(pos.x, _displayH - pos.y, 0.0F);
	else
		glTranslatef(pos.x, pos.y, 0.0F);

	if (control & (CONTROL_FLIP_H | CONTROL_FLIP_V))
	{
		Float32 sx = 1.0F;
		Float32 sy = 1.0F;
		
		if (control & CONTROL_FLIP_H)
			sx = -sx;
		if (control & CONTROL_FLIP_V)
			sy = -sy;
		
		glScalef(sx, sy, 1.0F);
		_rendererStats.scales++;
	}
#endif /* _USE_OPENGL */

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
		setPrimaryColor(rgba);

#ifdef _USE_OPENGL
		Texture*    texref = tex.atlas->getTextureState()->_textureObj;

		setTextureUnit(RendererTextureUnit0);
		enableTexturing();
		setTexture(texref->getHWTextureID());

		if (texref->getBoundTextureState())
			setTextureEnvMode(texref->getBoundTextureState()->_mode);
		else
			setTextureEnvMode(TextureState::TEXTURE_ENV_MODE_REPLACE);

#ifdef _USE_OPENGL_ES_1_1

		enableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);
		setTexCoordArray((Float32*) tex.data.PT2D_DIRECT.uvs);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
			_rendererStats.texPolys += 2;
		}

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_QUADS);

		setPrimaryColor(rgba);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[0]->x, tex.data.PT2D_PTR.uvs[0]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[0]->x, tex.data.PT2D_PTR.vertices[0]->y);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[1]->x, tex.data.PT2D_PTR.uvs[1]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[1]->x, tex.data.PT2D_PTR.vertices[1]->y);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[2]->x, tex.data.PT2D_PTR.uvs[2]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[2]->x, tex.data.PT2D_PTR.vertices[2]->y);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[3]->x, tex.data.PT2D_PTR.uvs[3]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[3]->x, tex.data.PT2D_PTR.vertices[3]->y);

		glEnd();

		restoreDefaultPrimaryColor();

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 1;
			_rendererStats.texPolys += 1;
		}

#else

		enableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);
		setTexCoordArray((Float32*) tex.data.PT2D_DIRECT.uvs);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
			_rendererStats.texPolys += 2;
		}

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

		disableTexturing();

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		glVertex2f(tex.data.PT2D_PTR.vertices[0]->x, tex.data.PT2D_PTR.vertices[0]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[1]->x, tex.data.PT2D_PTR.vertices[1]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[2]->x, tex.data.PT2D_PTR.vertices[2]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[3]->x, tex.data.PT2D_PTR.vertices[3]->y);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else
		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

#ifdef _USE_OPENGL
	glPopMatrix();
#endif /* _USE_OPENGL */
}

void Renderer::drawSubTexture(const TextureAtlasSubTexture& tex, const Point2D& pos, const ColorRGBA& rgba, Float32 rotate, Uint32 control)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

#ifdef _USE_OPENGL
	glPushMatrix();

	if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		glTranslatef(pos.x, _displayH - pos.y, 0.0F);
	else
		glTranslatef(pos.x, pos.y, 0.0F);

	glRotatef(rotate, 0, 0, 1);
	_rendererStats.rotates++;
	
	if (control & (CONTROL_FLIP_H | CONTROL_FLIP_V))
	{
		Float32 sx = 1.0F;
		Float32 sy = 1.0F;

		if (control & CONTROL_FLIP_H)
			sx = -sx;
		if (control & CONTROL_FLIP_V)
			sy = -sy;
		
		glScalef(sx, sy, 1.0F);
		_rendererStats.scales++;
	}
	
#endif /* _USE_OPENGL */

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
		setPrimaryColor(rgba);

#ifdef _USE_OPENGL
		Texture*    texref = tex.atlas->getTextureState()->_textureObj;

		setTextureUnit(RendererTextureUnit0);
		enableTexturing();
		setTexture(texref->getHWTextureID());

		if (texref->getBoundTextureState())
			setTextureEnvMode(texref->getBoundTextureState()->_mode);
		else
			setTextureEnvMode(TextureState::TEXTURE_ENV_MODE_REPLACE);

#ifdef _USE_OPENGL_ES_1_1

		enableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);
		setTexCoordArray((Float32*) tex.data.PT2D_DIRECT.uvs);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
			_rendererStats.texPolys += 2;
		}

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_QUADS);

		setPrimaryColor(rgba);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[0]->x, tex.data.PT2D_PTR.uvs[0]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[0]->x, tex.data.PT2D_PTR.vertices[0]->y);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[1]->x, tex.data.PT2D_PTR.uvs[1]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[1]->x, tex.data.PT2D_PTR.vertices[1]->y);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[2]->x, tex.data.PT2D_PTR.uvs[2]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[2]->x, tex.data.PT2D_PTR.vertices[2]->y);

		glTexCoord2f(tex.data.PT2D_PTR.uvs[3]->x, tex.data.PT2D_PTR.uvs[3]->y);
		glVertex2f(tex.data.PT2D_PTR.vertices[3]->x, tex.data.PT2D_PTR.vertices[3]->y);

		glEnd();

		restoreDefaultPrimaryColor();

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 1;
			_rendererStats.texPolys += 1;
		}

#else

		enableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);
		setTexCoordArray((Float32*) tex.data.PT2D_DIRECT.uvs);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (_recordStats)
		{
			_rendererStats.vertices += 4;
			_rendererStats.polys += 2;
			_rendererStats.texPolys += 2;
		}

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

		disableTexturing();

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		glVertex2f(tex.data.PT2D_PTR.vertices[0]->x, tex.data.PT2D_PTR.vertices[0]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[1]->x, tex.data.PT2D_PTR.vertices[1]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[2]->x, tex.data.PT2D_PTR.vertices[2]->y);

		glVertex2f(tex.data.PT2D_PTR.vertices[3]->x, tex.data.PT2D_PTR.vertices[3]->y);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else
		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray((Float32*) tex.data.PT2D_DIRECT.vertices);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

#ifdef _USE_OPENGL
	glPopMatrix();
#endif /* _USE_OPENGL */
}

void Renderer::drawSubTexture(ObjectID tex, const Point2D& pos, const Point2D& wh, const Point2D& texpos, Uint32 control)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
#ifdef _USE_OPENGL
		Texture*    texref = _app->getTextureManager()->findTextureFromRefID(tex);

		if (texref != NULL)
		{
			setTextureUnit(RendererTextureUnit0);
			enableTexturing();
			setTexture(texref->getHWTextureID());

			if (texref->getBoundTextureState())
				setTextureEnvMode(texref->getBoundTextureState()->_mode);
			else
				setTextureEnvMode(TextureState::TEXTURE_ENV_MODE_REPLACE);

			if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
			{
				_tmpRectVerts[0] = pos.x;
				_tmpRectVerts[1] = _displayH - pos.y;
				_tmpRectVerts[2] = pos.x;
				_tmpRectVerts[3] = _displayH - (pos.y + wh.y);
				_tmpRectVerts[4] = pos.x + wh.x;
				_tmpRectVerts[5] = _displayH - (pos.y + wh.y);
				_tmpRectVerts[6] = pos.x + wh.x;
				_tmpRectVerts[7] =  _displayH - pos.y;
			}
			else
			{
				_tmpRectVerts[0] = pos.x;
				_tmpRectVerts[1] = pos.y;
				_tmpRectVerts[2] = pos.x;
				_tmpRectVerts[3] = pos.y + wh.y;
				_tmpRectVerts[4] = pos.x + wh.x;
				_tmpRectVerts[5] = pos.y + wh.y;
				_tmpRectVerts[6] = pos.x + wh.x;
				_tmpRectVerts[7] = pos.y;
			}

			_tmpUVs[0] = texpos.x / (Float32) texref->getWidth();
			_tmpUVs[1] = ((Float32) texref->getHeight() - texpos.y) / (Float32) texref->getHeight();
			_tmpUVs[2] = _tmpUVs[0];
			_tmpUVs[3] = ((Float32) texref->getHeight() - (texpos.y + wh.y)) / (Float32) texref->getHeight();
			_tmpUVs[4] = (texpos.x + wh.x) / (Float32) texref->getWidth();
			_tmpUVs[5] = _tmpUVs[3];
			_tmpUVs[6] = _tmpUVs[4];
			_tmpUVs[7] = _tmpUVs[1];

#ifdef _USE_OPENGL_ES_1_1

			enableTexCoordArrayClientState();
			disableColorArrayClientState();
			enableVertArrayClientState();

			setVertArray(_tmpRectVerts, true);
			setTexCoordArray(_tmpUVs, true);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			if (_recordStats)
			{
				_rendererStats.vertices += 4;
				_rendererStats.polys += 2;
				_rendererStats.texPolys += 2;
			}

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

			glBegin(GL_QUADS);

			glTexCoord2f(_tmpUVs[0], _tmpUVs[1]);
			glVertex2f(_tmpRectVerts[0], _tmpRectVerts[1]);

			glTexCoord2f(_tmpUVs[2], _tmpUVs[3]);
			glVertex2f(_tmpRectVerts[2], _tmpRectVerts[3]);

			glTexCoord2f(_tmpUVs[4], _tmpUVs[5]);
			glVertex2f(_tmpRectVerts[4], _tmpRectVerts[5]);

			glTexCoord2f(_tmpUVs[6], _tmpUVs[7]);
			glVertex2f(_tmpRectVerts[6], _tmpRectVerts[7]);

			glEnd();

			if (_recordStats)
			{
				_rendererStats.vertices += 4;
				_rendererStats.polys += 1;
				_rendererStats.texPolys += 1;
			}

#else

			enableTexCoordArrayClientState();
			disableColorArrayClientState();
			enableVertArrayClientState();

			setVertArray(_tmpRectVerts, true);
			setTexCoordArray(_tmpUVs, true);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			if (_recordStats)
			{
				_rendererStats.vertices += 4;
				_rendererStats.polys += 2;
				_rendererStats.texPolys += 2;
			}

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

		}

#endif /* _USE_OPENGL */
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

		disableTexturing();

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		glVertex2f(_tmpRectVerts[0], _tmpRectVerts[1]);

		glVertex2f(_tmpRectVerts[2], _tmpRectVerts[3]);

		glVertex2f(_tmpRectVerts[4], _tmpRectVerts[5]);

		glVertex2f(_tmpRectVerts[6], _tmpRectVerts[7]);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else
		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}
}

void Renderer::drawSubTexture(ObjectID tex, const Point2D& pos, const Point2D& wh, const Point2D& texpos, Float32 rotate, Uint32 control)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

#ifdef _USE_OPENGL
	glPushMatrix();

	if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
	{
		glTranslatef(pos.x + wh.x / 2.0F, (_displayH - (pos.y + wh.y / 2.0F)), 0.0F);
		glRotatef(rotate, 0.0F, 0.0F, 1.0F);
		glTranslatef(-(pos.x + wh.x / 2.0F), -(_displayH - (pos.y + wh.y / 2.0F)), 0.0F);
	}
	else
	{
		glTranslatef(pos.x + wh.x / 2.0F, pos.y + wh.y / 2.0F, 0.0F);
		glRotatef(rotate, 0.0F, 0.0F, 1.0F);
		glTranslatef(-(pos.x + wh.x / 2.0F), -(pos.y + wh.y / 2.0F), 0.0F);
	}

	_rendererStats.rotates++;
#endif /* _USE_OPENGL */

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
#ifdef _USE_OPENGL
		Texture*    texref = _app->getTextureManager()->findTextureFromRefID(tex);

		if (texref != NULL)
		{
			GLfloat tmp;

			setTextureUnit(RendererTextureUnit0);
			enableTexturing();
			setTexture(texref->getHWTextureID());

			if (texref->getBoundTextureState())
				setTextureEnvMode(texref->getBoundTextureState()->_mode);
			else
				setTextureEnvMode(TextureState::TEXTURE_ENV_MODE_REPLACE);

			if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
			{
				_tmpRectVerts[0] = pos.x;
				_tmpRectVerts[1] = _displayH - pos.y;
				_tmpRectVerts[2] = pos.x;
				_tmpRectVerts[3] = _displayH - (pos.y + wh.y);
				_tmpRectVerts[4] = pos.x + wh.x;
				_tmpRectVerts[5] = _displayH - (pos.y + wh.y);
				_tmpRectVerts[6] = pos.x + wh.x;
				_tmpRectVerts[7] =  _displayH - pos.y;
			}
			else
			{
				_tmpRectVerts[0] = pos.x;
				_tmpRectVerts[1] = pos.y;
				_tmpRectVerts[2] = pos.x;
				_tmpRectVerts[3] = pos.y + wh.y;
				_tmpRectVerts[4] = pos.x + wh.x;
				_tmpRectVerts[5] = pos.y + wh.y;
				_tmpRectVerts[6] = pos.x + wh.x;
				_tmpRectVerts[7] = pos.y;
			}

			_tmpUVs[0] = texpos.x / (Float32) texref->getWidth();
			_tmpUVs[1] = ((Float32) texref->getHeight() - texpos.y) / (Float32) texref->getHeight();
			_tmpUVs[2] = _tmpUVs[0];
			_tmpUVs[3] = ((Float32) texref->getHeight() - (texpos.y + wh.y)) / (Float32) texref->getHeight();
			_tmpUVs[4] = (texpos.x + wh.x) / (Float32) texref->getWidth();
			_tmpUVs[5] = _tmpUVs[3];
			_tmpUVs[6] = _tmpUVs[4];
			_tmpUVs[7] = _tmpUVs[1];

			if (control & CONTROL_FLIP_H)
			{
				//	Rearrange the UVs to they are flipped
				tmp = _tmpUVs[0];

				_tmpUVs[0] = _tmpUVs[6];
				_tmpUVs[6] = tmp;

				tmp = _tmpUVs[2];

				_tmpUVs[2] = _tmpUVs[4];
				_tmpUVs[4] = tmp;
			}

			if (control & CONTROL_FLIP_V)
			{
				//	Rearrange the UVs to they are flipped
				tmp = _tmpUVs[1];

				_tmpUVs[1] = _tmpUVs[3];
				_tmpUVs[3] = tmp;

				tmp = _tmpUVs[5];

				_tmpUVs[5] = _tmpUVs[7];
				_tmpUVs[7] = tmp;
			}

#ifdef _USE_OPENGL_ES_1_1

			enableTexCoordArrayClientState();
			disableColorArrayClientState();
			enableVertArrayClientState();

			setVertArray(_tmpRectVerts, true);
			setTexCoordArray(_tmpUVs, true);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			if (_recordStats)
			{
				_rendererStats.vertices += 4;
				_rendererStats.polys += 2;
				_rendererStats.texPolys += 2;
			}

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

			glBegin(GL_QUADS);

			glTexCoord2f(_tmpUVs[0], _tmpUVs[1]);
			glVertex2f(_tmpRectVerts[0], _tmpRectVerts[1]);

			glTexCoord2f(_tmpUVs[2], _tmpUVs[3]);
			glVertex2f(_tmpRectVerts[2], _tmpRectVerts[3]);

			glTexCoord2f(_tmpUVs[4], _tmpUVs[5]);
			glVertex2f(_tmpRectVerts[4], _tmpRectVerts[5]);

			glTexCoord2f(_tmpUVs[6], _tmpUVs[7]);
			glVertex2f(_tmpRectVerts[6], _tmpRectVerts[7]);

			glEnd();

			if (_recordStats)
			{
				_rendererStats.vertices += 4;
				_rendererStats.polys += 1;
				_rendererStats.texPolys += 1;
			}

#else

			enableTexCoordArrayClientState();
			disableColorArrayClientState();
			enableVertArrayClientState();

			setVertArray(_tmpRectVerts, true);
			setTexCoordArray(_tmpUVs, true);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			if (_recordStats)
			{
				_rendererStats.vertices += 4;
				_rendererStats.polys += 2;
				_rendererStats.texPolys += 2;
			}

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

		}

#endif /* _USE_OPENGL */
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

		disableTexturing();

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		glVertex2f(_tmpRectVerts[0], _tmpRectVerts[1]);

		glVertex2f(_tmpRectVerts[2], _tmpRectVerts[3]);

		glVertex2f(_tmpRectVerts[4], _tmpRectVerts[5]);

		glVertex2f(_tmpRectVerts[6], _tmpRectVerts[7]);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(_tmpRectVerts, true);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

#ifdef _USE_OPENGL
	glPopMatrix();
#endif /* _USE_OPENGL */
}

#ifdef _USE_OPENGL

void Renderer::drawSubTexture(ObjectID tex, Point2D& pos, GLfloat* verts, GLfloat* uvs, Uint32 control)
{
	Texture*    texref = _app->getTextureManager()->findTextureFromRefID(tex);

	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

	glPushMatrix();
	if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		glTranslatef(pos.x, _displayH - pos.y, 0.0F);
	else
		glTranslatef(pos.x, pos.y, 0.0F);

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
		if (texref != NULL)
		{
			setTextureUnit(RendererTextureUnit0);
			enableTexturing();
			setTexture(texref->getHWTextureID());

			if (texref->getBoundTextureState())
				setTextureEnvMode(texref->getBoundTextureState()->_mode);
			else
				setTextureEnvMode(TextureState::TEXTURE_ENV_MODE_REPLACE);

#ifdef _USE_OPENGL_ES_1_1

			enableTexCoordArrayClientState();
			disableColorArrayClientState();
			enableVertArrayClientState();

			setVertArray(verts);
			setTexCoordArray(uvs);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			if (_recordStats)
			{
				_rendererStats.vertices += 4;
				_rendererStats.polys += 2;
				_rendererStats.texPolys += 2;
			}

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

			glBegin(GL_QUADS);

			glTexCoord2f(uvs[0], uvs[1]);
			glVertex2f(verts[0], verts[1]);

			glTexCoord2f(uvs[2], uvs[3]);
			glVertex2f(verts[2], verts[3]);

			glTexCoord2f(uvs[4], uvs[5]);
			glVertex2f(verts[4], verts[5]);

			glTexCoord2f(uvs[6], uvs[7]);
			glVertex2f(verts[6], verts[7]);

			glEnd();

			if (_recordStats)
			{
				_rendererStats.vertices += 4;
				_rendererStats.polys += 1;
				_rendererStats.texPolys += 1;
			}

#else

			enableTexCoordArrayClientState();
			disableColorArrayClientState();
			enableVertArrayClientState();

			setVertArray(verts);
			setTexCoordArray(uvs);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			if (_recordStats)
			{
				_rendererStats.vertices += 4;
				_rendererStats.polys += 2;
				_rendererStats.texPolys += 2;
			}

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */
		}
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

		disableTexturing();

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(verts);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		glVertex2f(verts[0], verts[1]);

		glVertex2f(verts[2], verts[3]);

		glVertex2f(verts[4], verts[5]);

		glVertex2f(verts[6], verts[7]);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(verts);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

	glPopMatrix();
}

void Renderer::drawSubTexture(Texture* tex, Point2D& pos, GLfloat* verts, GLfloat* uvs, Uint32 control)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

	glPushMatrix();
	if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		glTranslatef(pos.x, _displayH - pos.y, 0.0F);
	else
		glTranslatef(pos.x, pos.y, 0.0F);

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
		if (tex != NULL)
		{
			setTextureUnit(RendererTextureUnit0);
			enableTexturing();
			setTexture(tex->getHWTextureID());

			if (tex->getBoundTextureState())
				setTextureEnvMode(tex->getBoundTextureState()->_mode);
			else
				setTextureEnvMode(TextureState::TEXTURE_ENV_MODE_REPLACE);

#ifdef _USE_OPENGL_ES_1_1

			enableTexCoordArrayClientState();
			disableColorArrayClientState();
			enableVertArrayClientState();

			setVertArray(verts);
			setTexCoordArray(uvs);

			glDrawArrays(GL_TRIANGLES, 0, RendererBasicPrimitiveNumVertices * RendererBasicPrimitivesPerQuad);

			if (_recordStats)
			{
				_rendererStats.vertices += 4;
				_rendererStats.polys += 2;
				_rendererStats.texPolys += 2;
			}

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

			glBegin(GL_QUADS);

			glTexCoord2f(uvs[0], uvs[1]);
			glVertex2f(verts[0], verts[1]);

			glTexCoord2f(uvs[2], uvs[3]);
			glVertex2f(verts[2], verts[3]);

			glTexCoord2f(uvs[4], uvs[5]);
			glVertex2f(verts[4], verts[5]);

			glTexCoord2f(uvs[6], uvs[7]);
			glVertex2f(verts[6], verts[7]);

			glEnd();

			if (_recordStats)
			{
				_rendererStats.vertices += 4;
				_rendererStats.polys += 1;
				_rendererStats.texPolys += 1;
			}

#else

			enableTexCoordArrayClientState();
			disableColorArrayClientState();
			enableVertArrayClientState();

			setVertArray(verts);
			setTexCoordArray(uvs);

			glDrawArrays(GL_QUADS, 0, RendererBasicPrimitiveNumVertices);

			if (_recordStats)
			{
				_rendererStats.vertices += 4;
				_rendererStats.polys += 1;
				_rendererStats.texPolys += 1;
			}

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */
		}
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

		disableTexturing();

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(verts);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_LINE_LOOP);

		glVertex2f(verts[0], verts[1]);

		glVertex2f(verts[2], verts[3]);

		glVertex2f(verts[4], verts[5]);

		glVertex2f(verts[6], verts[7]);

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += 4;

#else
		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(verts);

		glDrawArrays(GL_LINE_LOOP, 0, 4);

		if (_recordStats)
			_rendererStats.vertices += 4;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

	glPopMatrix();
}

void Renderer::drawSubTextureArray(Texture* tex, Point2D& pos, GLfloat* verts, GLfloat* uvs, Uint32 num)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

	glPushMatrix();

	if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		glTranslatef(pos.x, _displayH - pos.y, 0.0F);
	else
		glTranslatef(pos.x, pos.y, 0.0F);

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
		if (tex != NULL)
		{
			setTextureUnit(RendererTextureUnit0);
			enableTexturing();
			setTexture(tex->getHWTextureID());

			if (tex->getBoundTextureState())
				setTextureEnvMode(tex->getBoundTextureState()->_mode);
			else
				setTextureEnvMode(TextureState::TEXTURE_ENV_MODE_REPLACE);

#ifdef _USE_OPENGL_ES_1_1

			enableTexCoordArrayClientState();
			disableColorArrayClientState();
			enableVertArrayClientState();

			setVertArray(verts);
			setTexCoordArray(uvs);

			glDrawArrays(GL_TRIANGLES, 0, RendererBasicPrimitiveNumVertices * num);

			if (_recordStats)
			{
				_rendererStats.vertices += RendererBasicPrimitiveNumVertices * num;
				_rendererStats.polys += num;
				_rendererStats.texPolys += num;
			}

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

			glBegin(GL_QUADS);

			for (int i=0;i<num;i++)
			{
				glTexCoord2f(uvs[0 + (i * 4)], uvs[1 + (i * 4)]);
				glVertex2f(verts[0 + (i * 4)], verts[1 + (i * 4)]);

				glTexCoord2f(uvs[2 + (i * 4)], uvs[3 + (i * 4)]);
				glVertex2f(verts[2 + (i * 4)], verts[3 + (i * 4)]);

				glTexCoord2f(uvs[4 + (i * 4)], uvs[5 + (i * 4)]);
				glVertex2f(verts[4 + (i * 4)], verts[5 + (i * 4)]);

				glTexCoord2f(uvs[6 + (i * 4)], uvs[7 + (i * 4)]);
				glVertex2f(verts[6 + (i * 4)], verts[7 + (i * 4)]);
			}

			glEnd();

			if (_recordStats)
			{
				_rendererStats.vertices += 4 * num;
				_rendererStats.polys += num;
				_rendererStats.texPolys += num;
			}

#else

			enableTexCoordArrayClientState();
			disableColorArrayClientState();
			enableVertArrayClientState();

			setVertArray(verts);
			setTexCoordArray(uvs);

			glDrawArrays(GL_QUADS, 0, RendererBasicPrimitiveNumVertices * num);

			if (_recordStats)
			{
				_rendererStats.vertices += RendererBasicPrimitiveNumVertices * num;
				_rendererStats.polys += num;
				_rendererStats.texPolys += num;
			}

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */
		}
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

		disableTexturing();

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(verts);

		glDrawArrays(GL_LINE_LOOP, 0, RendererBasicPrimitiveNumVertices * num);

		if (_recordStats)
			_rendererStats.vertices += RendererBasicPrimitiveNumVertices * num;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_QUADS);

		for (int i=0;i<num;i++)
		{
			glVertex2f(verts[0 + (i * 4)], verts[1 + (i * 4)]);

			glVertex2f(verts[2 + (i * 4)], verts[3 + (i * 4)]);

			glVertex2f(verts[4 + (i * 4)], verts[5 + (i * 4)]);

			glVertex2f(verts[6 + (i * 4)], verts[7 + (i * 4)]);
		}

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += RendererBasicPrimitiveNumVertices * num;

#else

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(verts);

		glDrawArrays(GL_LINE_LOOP, 0, RendererBasicPrimitiveNumVertices * num);

		if (_recordStats)
			_rendererStats.vertices += RendererBasicPrimitiveNumVertices * num;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

	glPopMatrix();
}

void Renderer::drawSubTextureArray(Texture* tex, Point2D& pos, GLfloat* verts, GLfloat* uvs, ColorRGBA* rgba, Uint32 num)
{
	WireframeRenderState* wireframe = (WireframeRenderState*) _renderStateStack[RenderState::RENDER_STATE_WIREFRAME][_renderStateStackIndex[RenderState::RENDER_STATE_WIREFRAME]];

	glPushMatrix();

	if (_coordSys2D == Renderer2DCoordinateSystemTraditional)
		glTranslatef(pos.x, _displayH - pos.y, 0.0F);
	else
		glTranslatef(pos.x, pos.y, 0.0F);

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_ONLY)
	{
		setPrimaryColor(*rgba);

		if (tex != NULL)
		{
			setTextureUnit(RendererTextureUnit0);
			enableTexturing();
			setTexture(tex->getHWTextureID());

			if (tex->getBoundTextureState())
				setTextureEnvMode(tex->getBoundTextureState()->_mode);
			else
				setTextureEnvMode(TextureState::TEXTURE_ENV_MODE_REPLACE);

#ifdef _USE_OPENGL_ES_1_1

			enableTexCoordArrayClientState();
			disableColorArrayClientState();
			enableVertArrayClientState();

			setVertArray(verts);
			setTexCoordArray(uvs);

			glDrawArrays(GL_TRIANGLES, 0, RendererBasicPrimitiveNumVertices * num);

			if (_recordStats)
			{
				_rendererStats.vertices += RendererBasicPrimitiveNumVertices * num;
				_rendererStats.polys += num;
				_rendererStats.texPolys += num;
			}

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

			glBegin(GL_QUADS);

			for (int i=0;i<num;i++)
			{
				glTexCoord2f(uvs[0 + (i * 4)], uvs[1 + (i * 4)]);
				glVertex2f(verts[0 + (i * 4)], verts[1 + (i * 4)]);

				glTexCoord2f(uvs[2 + (i * 4)], uvs[3 + (i * 4)]);
				glVertex2f(verts[2 + (i * 4)], verts[3 + (i * 4)]);

				glTexCoord2f(uvs[4 + (i * 4)], uvs[5 + (i * 4)]);
				glVertex2f(verts[4 + (i * 4)], verts[5 + (i * 4)]);

				glTexCoord2f(uvs[6 + (i * 4)], uvs[7 + (i * 4)]);
				glVertex2f(verts[6 + (i * 4)], verts[7 + (i * 4)]);
			}

			glEnd();

			if (_recordStats)
			{
				_rendererStats.vertices += 4 * num;
				_rendererStats.polys += num;
				_rendererStats.texPolys += num;
			}

#else

			enableTexCoordArrayClientState();
			disableColorArrayClientState();
			enableVertArrayClientState();

			setVertArray(verts);
			setTexCoordArray(uvs);

			glDrawArrays(GL_QUADS, 0, RendererBasicPrimitiveNumVertices * num);

			if (_recordStats)
			{
				_rendererStats.vertices += RendererBasicPrimitiveNumVertices * num;
				_rendererStats.polys += num;
				_rendererStats.texPolys += num;
			}

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */
		}

		restoreDefaultPrimaryColor();
	}

	if (wireframe->mode != WireframeRenderState::WIREFRAME_MODE_NONE)
	{
		setPrimaryColor(wireframe->color);

		disableTexturing();

#ifdef _USE_OPENGL
#ifdef _USE_OPENGL_ES_1_1

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(verts);

		glDrawArrays(GL_LINE_LOOP, 0, RendererBasicPrimitiveNumVertices * num);

		if (_recordStats)
			_rendererStats.vertices += RendererBasicPrimitiveNumVertices * num;

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

		glBegin(GL_QUADS);

		for (int i=0;i<num;i++)
		{
			glVertex2f(verts[0 + (i * 4)], verts[1 + (i * 4)]);

			glVertex2f(verts[2 + (i * 4)], verts[3 + (i * 4)]);

			glVertex2f(verts[4 + (i * 4)], verts[5 + (i * 4)]);

			glVertex2f(verts[6 + (i * 4)], verts[7 + (i * 4)]);
		}

		glEnd();

		if (_recordStats)
			_rendererStats.vertices += RendererBasicPrimitiveNumVertices * num;

#else

		disableTexCoordArrayClientState();
		disableColorArrayClientState();
		enableVertArrayClientState();

		setVertArray(verts);

		glDrawArrays(GL_LINE_LOOP, 0, RendererBasicPrimitiveNumVertices * num);

		if (_recordStats)
			_rendererStats.vertices += RendererBasicPrimitiveNumVertices * num;

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */

#endif /* _USE_OPENGL */

		restoreDefaultPrimaryColor();
	}

	glPopMatrix();
}

void Renderer::generateGLQuadCoordinates(GLfloat* verts, GLfloat* uvs, Point2D& xy, Point2D& wh, Point2D& tmapWH, Uint32 control)
{
	GLfloat tmp;

	AssertWDesc(verts != NULL && uvs != NULL, "Renderer::generateGLQuadCoordinates() NULL verts or uvs");

#ifdef _USE_OPENGL_ES_1_1

	verts[0] = -wh.x / 2.0F;
	verts[1] = -wh.y / 2.0F;

	verts[2] = -wh.x / 2.0F;
	verts[3] = wh.y / 2.0F;

	verts[4] = wh.x / 2.0F;
	verts[5] = wh.y / 2.0F;

	verts[6] = verts[0];
	verts[7] = verts[1];

	verts[8] = verts[4];
	verts[9] = verts[5];

	verts[10] = wh.x / 2.0F;
	verts[11] = -wh.y / 2.0F;

	uvs[0] = xy.x / tmapWH.x;
	uvs[1] = (tmapWH.y - (xy.y + wh.y)) / tmapWH.y;

	uvs[2] = uvs[0];
	uvs[3] = (tmapWH.y - xy.y) / tmapWH.y;

	uvs[4] = (xy.x + wh.x) / tmapWH.x;
	uvs[5] = uvs[3];

	uvs[6] = uvs[0];
	uvs[7] = uvs[1];

	uvs[8] = uvs[4];
	uvs[9] = uvs[5];

	uvs[10] = uvs[4];
	uvs[11] = uvs[1];

	if (control & CONTROL_FLIP_H)
	{
		//	Rearrange the UVs to they are flipped
		tmp = uvs[0];

		uvs[0] = uvs[6] = uvs[10];
		uvs[10] = tmp;

		tmp = uvs[2];

		uvs[2] = uvs[8];
		uvs[4] = uvs[8] = tmp;
	}

	if (control & CONTROL_FLIP_V)
	{
		//	Rearrange the UVs to they are flipped
		tmp = uvs[1];

		uvs[1] = uvs[7] = uvs[11];
		uvs[11] = tmp;

		tmp = uvs[3];

		uvs[3] = uvs[9];
		uvs[5] = uvs[9] = tmp;
	}

#else

	verts[0] = -wh.x / 2.0F;
	verts[1] = -wh.y / 2.0F;
	verts[2] = -wh.x / 2.0F;
	verts[3] = wh.y / 2.0F;
	verts[4] = wh.x / 2.0F;
	verts[5] = wh.y / 2.0F;
	verts[6] = wh.x / 2.0F;
	verts[7] = -wh.y / 2.0F;

	uvs[0] = xy.x / tmapWH.x;
	uvs[1] = (tmapWH.y - (xy.y + wh.y)) / tmapWH.y;
	uvs[2] = uvs[0];
	uvs[3] = (tmapWH.y - xy.y) / tmapWH.y;
	uvs[4] = (xy.x + wh.x) / tmapWH.x;
	uvs[5] = uvs[3];
	uvs[6] = uvs[4];
	uvs[7] = uvs[1];

	if (control & CONTROL_FLIP_H)
	{
		//	Rearrange the UVs to they are flipped
		tmp = uvs[0];

		uvs[0] = uvs[6];
		uvs[6] = tmp;

		tmp = uvs[2];

		uvs[2] = uvs[4];
		uvs[4] = tmp;
	}

	if (control & CONTROL_FLIP_V)
	{
		//	Rearrange the UVs to they are flipped
		tmp = uvs[1];

		uvs[1] = uvs[7];
		uvs[7] = tmp;

		tmp = uvs[3];

		uvs[3] = uvs[5];
		uvs[5] = tmp;
	}

#endif /* _USE_OPENGL_ES_1_1 */
}

void Renderer::generateGLQuadCoordinates(GLfloat* verts, GLfloat* uvs, Point2D& offset, Point2D& xy, Point2D& wh, Point2D& tmapWH, Uint32 control)
{
	GLfloat tmp;

	AssertWDesc(verts != NULL && uvs != NULL, "Renderer::generateGLQuadCoordinates() NULL verts or uvs");

#ifdef _USE_OPENGL_ES_1_1

	verts[0] = -(wh.x / 2.0F) + offset.x;
	verts[1] = -(wh.y / 2.0F) + offset.y;

	verts[2] = -(wh.x / 2.0F) + offset.x;
	verts[3] = (wh.y / 2.0F) + offset.y;

	verts[4] = (wh.x / 2.0F) + offset.x;
	verts[5] = (wh.y / 2.0F) + offset.y;

	verts[6] = verts[0];
	verts[7] = verts[1];

	verts[8] = verts[4];
	verts[9] = verts[5];

	verts[10] = (wh.x / 2.0F) + offset.x;
	verts[11] = (-wh.y / 2.0F) + offset.y;

	uvs[0] = xy.x / tmapWH.x;
	uvs[1] = (tmapWH.y - (xy.y + wh.y)) / tmapWH.y;

	uvs[2] = uvs[0];
	uvs[3] = (tmapWH.y - xy.y) / tmapWH.y;

	uvs[4] = (xy.x + wh.x) / tmapWH.x;
	uvs[5] = uvs[3];

	uvs[6] = uvs[0];
	uvs[7] = uvs[1];

	uvs[8] = uvs[4];
	uvs[9] = uvs[5];

	uvs[10] = uvs[4];
	uvs[11] = uvs[1];

	if (control & CONTROL_FLIP_H)
	{
		//	Rearrange the UVs to they are flipped
		tmp = uvs[0];

		uvs[0] = uvs[6] = uvs[10];
		uvs[10] = tmp;

		tmp = uvs[2];

		uvs[2] = uvs[8];
		uvs[4] = uvs[8] = tmp;
	}

	if (control & CONTROL_FLIP_V)
	{
		//	Rearrange the UVs to they are flipped
		tmp = uvs[1];

		uvs[1] = uvs[7] = uvs[11];
		uvs[11] = tmp;

		tmp = uvs[3];

		uvs[3] = uvs[9];
		uvs[5] = uvs[9] = tmp;
	}

#else

	verts[0] = -(wh.x / 2.0F) + offset.x;
	verts[1] = -(wh.y / 2.0F) + offset.y;
	verts[2] = -(wh.x / 2.0F) + offset.x;
	verts[3] = (wh.y / 2.0F) + offset.y;
	verts[4] = (wh.x / 2.0F) + offset.x;
	verts[5] = (wh.y / 2.0F) + offset.y;
	verts[6] = (wh.x / 2.0F) + offset.x;
	verts[7] = (-wh.y / 2.0F) + offset.y;

	uvs[0] = xy.x / tmapWH.x;
	uvs[1] = (tmapWH.y - (xy.y + wh.y)) / tmapWH.y;
	uvs[2] = uvs[0];
	uvs[3] = (tmapWH.y - xy.y) / tmapWH.y;
	uvs[4] = (xy.x + wh.x) / tmapWH.x;
	uvs[5] = uvs[3];
	uvs[6] = uvs[4];
	uvs[7] = uvs[1];

	if (control & CONTROL_FLIP_H)
	{
		//	Rearrange the UVs to they are flipped
		tmp = uvs[0];

		uvs[0] = uvs[6];
		uvs[6] = tmp;

		tmp = uvs[2];

		uvs[2] = uvs[4];
		uvs[4] = tmp;
	}

	if (control & CONTROL_FLIP_V)
	{
		//	Rearrange the UVs to they are flipped
		tmp = uvs[1];

		uvs[1] = uvs[7];
		uvs[7] = tmp;

		tmp = uvs[3];

		uvs[3] = uvs[5];
		uvs[5] = tmp;
	}

#endif /* _USE_OPENGL_ES_1_1 */
}

#endif /* _USE_OPENGL */

void Renderer::set2DCoordinateSystem(Renderer2DCoordinateSystem type)
{
	_coordSys2D = type;
}

void Renderer::pushHWMatrix(void)
{
#ifdef _USE_OPENGL
	glPushMatrix();
#endif /* _USE_OPENGL */
}

void Renderer::popHWMatrix(void)
{
#ifdef _USE_OPENGL
	glPopMatrix();
#endif /* _USE_OPENGL */
}

void Renderer::HWRotateX(Float32 ang)
{
#ifdef _USE_OPENGL
	glRotatef(ang, 1, 0, 0);
#endif
}

void Renderer::HWRotateY(Float32 ang)
{
#ifdef _USE_OPENGL
	glRotatef(ang, 0, 1, 0);
#endif
}

void Renderer::HWRotateZ(Float32 ang)
{
#ifdef _USE_OPENGL
#ifdef _PLATFORM_IPHONE
#ifdef _NOT_YET
	if (_isLandscape)
		glRotatef(-90.0F + ang, 0.0, 0.0, 1.0);
	else
		glRotatef(ang, 0.0, 0.0, 1.0);
#else
	glRotatef(ang, 0.0, 0.0, 1.0);
#endif
#else
	glRotatef(ang, 0.0, 0.0, 1.0);
#endif /* _PLATFORM_IPHONE */
#endif
}

void Renderer::HWTranslate2D(Float32 x, Float32 y)
{
#ifdef _USE_OPENGL
	if (_is2DMode && _coordSys2D == Renderer2DCoordinateSystemTraditional)
		glTranslatef(x, -y, 0.0F);
	else
		glTranslatef(x, y, 0.0F);
#endif /* _USE_OPENGL */
}

void Renderer::HWTranslate3D(Float32 x, Float32 y, Float32 z)
{
#ifdef _USE_OPENGL
	if (_is2DMode && _coordSys2D == Renderer2DCoordinateSystemTraditional)
		glTranslatef(x, -y, z);
	else
		glTranslatef(x, y, z);
#endif /* _USE_OPENGL */
}

void Renderer::renderMode2D(void)
{
	Float32	baseAngle = _gfx->getScreenRotation();

#ifdef _USE_OPENGL

	if (_coordSys2D == Renderer2DCoordinateSystemCenter)
	{
		if (_isLandscape)
			glViewport((GLsizei) ((Float32) _gfx->getHeight() / 2.0F), (GLsizei) ((Float32) _gfx->getWidth() / 2.0F), (GLsizei) _gfx->getHeight(), (GLsizei) (_gfx->getWidth()));
		else
			glViewport((GLsizei) ((Float32) _gfx->getWidth() / 2.0F), (GLsizei) ((Float32) _gfx->getHeight() / 2.0F), (GLsizei) _gfx->getWidth(), (GLsizei) (_gfx->getHeight()));
	}
	else
	{
		if (_isLandscape)
			glViewport(0, 0, _gfx->getHeight(), ((Sint32) _gfx->getWidth()));
		else
			glViewport(0, 0, _gfx->getWidth(), ((Sint32) _gfx->getHeight()));
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

#ifdef _PLATFORM_IPHONE
	if (_isLandscape)
		glRotatef(-90.0 + baseAngle, 0.0, 0.0, 1.0);
	else
		glRotatef(baseAngle, 0.0, 0.0, 1.0);
#else
	glRotatef(baseAngle, 0.0, 0.0, 1.0);
#endif /* _PLATFORM_IPHONE */

#ifdef _USE_OPENGL_ES_1_1

	glOrthof(0.0F, (Float32) _gfx->getWidth(), 0.0F, (Float32) _gfx->getHeight(), -100.0F, 100.0F);

#else

	glOrtho(0.0F, (Float32) _gfx->getWidth(), 0.0F, (Float32) _gfx->getHeight(), -100.0F, 100.0F);

#endif /* _USE_OPENGL_ES_1_1 */

//	glScalef(1, -1, 1);
//	glTranslatef(0, -(Sint32) _gfx->getHeight(), 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

#endif /* _USE_OPENGL */

	_is2DMode = true;
}

void Renderer::setLandscape(Boolean landscape)
{
	_isLandscape = landscape;

	_displayW = _app->getGfx()->getWidth();
	_displayH = _app->getGfx()->getHeight();
}
