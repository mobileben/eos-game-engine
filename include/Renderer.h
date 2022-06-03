/******************************************************************************
 *
 * File: Renderer.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Renderer
 * 
 *****************************************************************************/

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "Platform.h"
#include "RenderState.h"
#include "TextureState.h"
#include "TextureAtlas.h"
#include "SpriteSet.h"

typedef struct
{
	Uint32	vertices;
	Uint32	polys;
	Uint32	texPolys;
	Uint32	texEnvChanges;
	Uint32	texChanges;
	Uint32	alphaBlendChanges;
	Uint32	colorChanges;
	Uint32	renderStateChanges;
	Uint32	rotates;
	Uint32	ignoredRotates;
	Uint32	scales;
	Uint32	ignoredScales;
	Uint32 	reusedTexArray; 
	Uint32 	reusedVertArray; 
} RendererStats;

static const int RendererMaxRenderStateStack = 256;
static const int RendererMaxTextureStateStack = 32;
static const int RendererMaxColorBlendingStack = 32;

class Gfx;

class Renderer : public EOSFrameworkComponent
{
public:
#ifdef _USE_OPENGL_ES_1_1
	
	static const int RendererBasicPrimitiveNumVertices = 3;
	static const int RendererBasicPrimitivesPerQuad = 2;
	
#else

	static const int RendererBasicPrimitiveNumVertices = 4;
	static const int RendererBasicPrimitivesPerQuad = 1;
	
#endif /* _USE_OPENGL_ES_1_1 */
	
	typedef enum
	{
		RenderStateHint_Unknown = 0,
	} RenderStateHint;

	typedef enum
	{
		Renderer2DCoordinateSystemTraditional = 0,	//	Upper left is 0, 0, with +X to the right and +Y down
		Renderer2DCoordinateSystemOpenGL,			//	Standard Open GL, with lower left 0, 0 and +X to the right and +Y up
		Renderer2DCoordinateSystemCenter,			//	Center of the screen is origin
	} Renderer2DCoordinateSystem;

	typedef enum
	{
		RendererTextureUnit0 = 0,
		RendererTextureUnit1,
		RendererTextureUnitMax,
		RendererTextureUnitIllegal = RendererTextureUnitMax,
	} RendererTextureUnit;

private:
	Boolean						_isLandscape;
	Boolean						_is2DMode;

	Boolean						_recordStats;

	Renderer2DCoordinateSystem	_coordSys2D;

	Uint32				_displayW;	//	This will need to change later
	Uint32				_displayH;	//	This will need to change later

	Uint32				_renderStateStackIndex[RenderState::RENDER_STATE_LAST];
	RenderState*		_renderStateStack[RenderState::RENDER_STATE_LAST][RendererMaxRenderStateStack];

	Uint32				_textureStateStackIndex;
	TextureState		_textureStateStack[RendererMaxTextureStateStack];

	Uint32				_colorBlendStackIndex;
	Uint32				_colorBlendStack[RendererMaxColorBlendingStack];
	ColorRGBA			_colorMultiplier[RendererMaxColorBlendingStack];
	ColorRGBA			_colorOffset[RendererMaxColorBlendingStack];

	Gfx*				_gfx;

	TextureState::TEXTURE_ENV_MODE	_currentTextureEnvMode[RendererTextureUnitMax];

	Boolean				_primaryColorChanged;
	ColorRGBA			_primaryColor;
	ColorRGBA			_defaultPrimaryColor;

	RendererTextureUnit	_currentTextureUnit;
	Boolean				_texturingEnabled[RendererTextureUnitMax];

	Boolean				_vertArrayClientState;
	Boolean				_colorArrayClientState;
	Boolean				_texCoordArrayClientState[RendererTextureUnitMax];

	HWTextureID			_currentTexture[RendererTextureUnitMax];

	RenderStateHint		_renderStateHint;

	RenderStateHint		_guessRenderStateHint(void);

	RendererStats		_rendererStats;

#ifdef _USE_OPENGL

	Float32*			_currentTexCoordArray[RendererTextureUnitMax];
	Float32*			_currentVertArray;
	Float32*			_currentColorArray;

	GLfloat				_tmpMatrix[16];
	GLfloat				_tmpRectVerts[8];
	GLfloat				_tmpUVs[8];

#ifdef _PLATFORM_PC
#ifndef _USE_PVR_OPENGLES
	PFNGLACTIVETEXTUREPROC			_funcActiveTexture;
	PFNGLCLIENTACTIVETEXTUREPROC 	_funcClientActiveTexture;
#endif /* _USE_PVR_OPENGLES */
#endif /* _PLATFORM_PC */

#endif /* _USE_OPENGL */

public:
	Renderer();
	~Renderer();

	//	Init is a pure initialize of the states
	void			init(void);

	EOSError		beginFrame(void);
	void			endFrame(void);

	void			bindGfx(Gfx* gfx);
	inline Gfx*		getBoundGfx(void) const { return _gfx; }

	void			pushRenderStateStack(RenderState::RENDER_STATE state);
	void			popRenderStateStack(RenderState::RENDER_STATE state);

	void			setRenderState(AlphaRenderState* state);
	void			setRenderState(MaterialRenderState* state);
	void			setRenderState(WireframeRenderState* state);
	void			setRenderState(CollisionRenderState* state);
	void			setRenderState(CullRenderState* state);
	void			setRenderState(ZBufferRenderState* state);

	RenderState*	getRenderState(RenderState::RENDER_STATE state);

	//	Invalidates current render state so it can be forcibly re-enabled. Does not affect the stack position.
	void			flushRenderState(RenderState::RENDER_STATE state);

	void			pushTextureState(void);
	void			popTextureState(void);
	void			setTextureState(TextureState* state);
	TextureState*	getTextureState(void);
	inline Boolean	isTexturingEnabled(void) { return _textureStateStack[_textureStateStackIndex]._textureObj != NULL; }

	//	Invalidates current texture state so it can be forcibly re-enabled. Does not affect the stack position.
	void			flushTextureState(void);

	void			setTextureEnvMode(TextureState::TEXTURE_ENV_MODE mode, RendererTextureUnit texunit = RendererTextureUnit0);
	void			flushTextureEnvMode(RendererTextureUnit texunit = RendererTextureUnit0);

	void			setTextureUnit(RendererTextureUnit texunit);  
	void			enableTexturing(void);
	void			disableTexturing(void);
	void			disableAllTexturing(void);

	void			enableVertArrayClientState(void);
	void			disableVertArrayClientState(void);
	void			enableTexCoordArrayClientState(void);
	void			disableTexCoordArrayClientState(void);
	void			enableColorArrayClientState(void);
	void			disableColorArrayClientState(void);

	void			setTexCoordArray(Float32* data, Boolean force = false);
	void			setVertArray(Float32* data, Boolean force = false);
	void			setColorArray(Float32* data, Boolean force = false);

#ifdef _USE_OPENGL
	void			setTexture(GLenum target, HWTextureID texID);
#endif /* _USE_OPENGL */

	void			setTexture(HWTextureID texID);
	void			flushTexture(void);
	void			flushAllTextures(void);

	void			setPrimaryColor(const ColorRGB& color);
	void			setPrimaryColor(const ColorRGBA& color);
	void			restoreDefaultPrimaryColor(void);

	void			drawLine(ColorRGB& rgb, Point2D& pt0, Point2D& pt1);
	void			drawLine(ColorRGBA& rgba, Point2D& pt0, Point2D& pt1);

	void			drawLineRect(ColorRGB& rgb, Point2D& pos, Point2D& wh);
	void			drawLineRect(ColorRGBA& rgba, Point2D& pos, Point2D& wh);
	void			drawFillRect(ColorRGB& rgb, Point2D& pos, Point2D& wh);
	void			drawFillRect(ColorRGBA& rgba, Point2D& pos, Point2D& wh);

	void			drawSprite(const Sprite& sprite, DrawControl control = 0);
	void			drawSprite(const Sprite& sprite, DrawControl control, SpriteColorOp& colorOp);
	void			drawSprite(const Sprite& sprite, const Point2D& pos, Float32 rotate = 0.0F, DrawControl control = 0);
	void			drawSprite(const Sprite& sprite, const Point2D& pos, Float32 rotate, DrawControl control, SpriteColorOp& colorOp);
	void			drawSprite(const Sprite& sprite, const Point2D& pos, Float32 scalex, Float32 scaley, Float32 rotate, DrawControl control = 0);
	void			drawSprite(const Sprite& sprite, const Point2D& pos, Float32 scalex, Float32 scaley, Float32 rotate, DrawControl control, SpriteColorOp& colorOp);

	void			drawSubSprite(const SubSprite& sprite);
	void			drawSubTexture(const TextureAtlasSubTexture& tex, Uint32 control = 0);
	void			drawSubTexture(const TextureAtlasSubTexture& tex, const Point2D& pos, Uint32 control = 0);
	void			drawSubTexture(const TextureAtlasSubTexture& tex, const Point2D& pos, Float32 rotate, Uint32 control = 0);
	void			drawSubTexture(const TextureAtlasSubTexture& tex, const Point2D& pos, Float32 rotate, Float32 scale, Uint32 control = 0);
	void			drawSubTexture(const TextureAtlasSubTexture& tex, const Point2D& pos, const ColorRGBA& rgba, Uint32 control = 0);
	void			drawSubTexture(const TextureAtlasSubTexture& tex, const Point2D& pos, const ColorRGBA& rgba, Float32 rotate, Uint32 control = 0);
	void			drawSubTexture(ObjectID tex, const Point2D& pos, const Point2D& wh, const Point2D& texpos, Uint32 control = 0);
	void			drawSubTexture(ObjectID tex, const Point2D& pos, const Point2D& wh, const Point2D& texpos, Float32 rotate, Uint32 control = 0);

#ifdef _USE_OPENGL

	void			drawSubTexture(ObjectID tex, Point2D& pos, GLfloat* verts, GLfloat* uvs, Uint32 control = 0);
	void			drawSubTexture(Texture* tex, Point2D& pos, GLfloat* verts, GLfloat* uvs, Uint32 control = 0);
	void			drawSubTextureArray(Texture* tex, Point2D& pos, GLfloat* verts, GLfloat* uvs, Uint32 num);
	void			drawSubTextureArray(Texture* tex, Point2D& pos, GLfloat* verts, GLfloat* uvs, ColorRGBA* rgba, Uint32 num);

	void			generateGLQuadCoordinates(GLfloat* verts, GLfloat* uvs, Point2D& xy, Point2D& wh, Point2D& tmapWH, Uint32 control = 0);
	void			generateGLQuadCoordinates(GLfloat* verts, GLfloat* uvs, Point2D& offset, Point2D& xy, Point2D& wh, Point2D& tmapWH, Uint32 control = 0);

#endif /* _USE_OPENGL */

	void 								set2DCoordinateSystem(Renderer2DCoordinateSystem type);
	inline Renderer2DCoordinateSystem	get2DCoordinateSystem(void) { return _coordSys2D; }

	void			pushHWMatrix(void);
	void			popHWMatrix(void);

	void			HWRotateX(Float32 ang);
	void			HWRotateY(Float32 ang);
	void			HWRotateZ(Float32 ang);

	void			HWTranslate2D(Float32 x, Float32 y);
	void			HWTranslate3D(Float32 x, Float32 y, Float32 z);
	
	void			renderMode2D(void);

	void			init2DDrawList(void);
	void			reset2DDrawList(void);
	void			render2D(void);

	void			setLandscape(Boolean landscape);
	inline Boolean	isLandscape(void) { return _isLandscape; }

	inline RendererStats*	getRendererStats(void) { return &_rendererStats; }
};

#endif /* __RENDERER_H__ */

