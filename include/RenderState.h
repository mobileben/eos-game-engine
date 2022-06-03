/******************************************************************************
 *
 * File: RenderState.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * RenderState
 * 
 *****************************************************************************/

#ifndef __RENDERSTATE_H__
#define __RENDERSTATE_H__

#include "Platform.h"
#include "Graphics.h"

class Renderer;

class RenderState : public EOSObject
{
public:
	typedef enum
	{
		RENDER_STATE_ALPHA = 0,
		RENDER_STATE_MATERIAL,
		RENDER_STATE_WIREFRAME,
		RENDER_STATE_COLLISION,
		RENDER_STATE_CULL,
		RENDER_STATE_ZBUFFER,
		RENDER_STATE_LAST
	} RENDER_STATE;

	typedef enum
	{
		RENDER_STATE_MASK_ALPHA 	= 0x00000001,
		RENDER_STATE_MASK_MATERIAL	= 0x00000002,
		RENDER_STATE_MASK_WIREFRAME = 0x00000004,
		RENDER_STATE_MASK_COLLISION = 0x00000008,
		RENDER_STATE_MASK_CULL		= 0x00000010,
		RENDER_STATE_MASK_ZBUFFER	= 0x00000020,
	} RENDER_STATE_MASK;

	Boolean					stateDirty;
	Renderer*				renderer;

	RenderState() : stateDirty(true), renderer(NULL) {}
	RenderState(Renderer* r) : stateDirty(true), renderer(r) {}
	virtual ~RenderState() {}

	virtual RENDER_STATE	getRenderStateType(void) const =0;
	virtual void			setDefaultValues(void) =0;
	virtual void			apply(void) =0;
};

class AlphaRenderState : public RenderState
{
public:
	typedef enum
	{
		ALPHA_SRC_FACTOR_ZERO = 0,
		ALPHA_SRC_FACTOR_ONE,
		ALPHA_SRC_FACTOR_DST_COLOR,
		ALPHA_SRC_FACTOR_ONE_MINUS_DST_COLOR,
		ALPHA_SRC_FACTOR_SRC_ALPHA,
		ALPHA_SRC_FACTOR_ONE_MINUS_SRC_ALPHA,
		ALPHA_SRC_FACTOR_DST_ALPHA,
		ALPHA_SRC_FACTOR_ONE_MINUS_DST_ALPHA,
		ALPHA_SRC_FACTOR_SRC_ALPHA_SATURATE,
		ALPHA_SRC_FACTOR_CONSTANT_COLOR,
		ALPHA_SRC_FACTOR_ONE_MINUS_CONSTANT_COLOR,
		ALPHA_SRC_FACTOR_CONSTANT_ALPHA,
		ALPHA_SRC_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
	} ALPHA_SRC_FACTOR;

	typedef enum
	{
		ALPHA_DST_FACTOR_ZERO = 0,
		ALPHA_DST_FACTOR_ONE,
		ALPHA_DST_FACTOR_SRC_COLOR,
		ALPHA_DST_FACTOR_ONE_MINUS_SRC_COLOR,
		ALPHA_DST_FACTOR_SRC_ALPHA,
		ALPHA_DST_FACTOR_ONE_MINUS_SRC_ALPHA,
		ALPHA_DST_FACTOR_DST_ALPHA,
		ALPHA_DST_FACTOR_ONE_MINUS_DST_ALPHA,
		ALPHA_DST_FACTOR_CONSTANT_COLOR,
		ALPHA_DST_FACTOR_ONE_MINUS_CONSTANT_COLOR,
		ALPHA_DST_FACTOR_CONSTANT_ALPHA,
		ALPHA_DST_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
	} ALPHA_DST_FACTOR;

	typedef enum
	{
		ALPHA_TEST_NEVER = 0,
		ALPHA_TEST_LESS,
		ALPHA_TEST_EQUAL,
		ALPHA_TEST_LEQUAL,
		ALPHA_TEST_GREATER,
		ALPHA_TEST_NOTEQUAL,
		ALPHA_TEST_GEQUAL,
		ALPHA_TEST_ALWAYS,
	} ALPHA_TEST;

	AlphaRenderState();
	AlphaRenderState(Renderer* r);
	virtual ~AlphaRenderState() {}

	AlphaRenderState& operator=(const AlphaRenderState& rhs);
	bool operator==(const AlphaRenderState& rhs);

	inline RENDER_STATE	getRenderStateType(void) const { return RENDER_STATE_ALPHA; }
	void				setDefaultValues(void);
	void 				apply(void);

	Boolean				enabled;
	ALPHA_SRC_FACTOR	srcFactor;
	ALPHA_DST_FACTOR	dstFactor;

	Boolean				testEnabled;
	ALPHA_TEST			test;
	Float32				testValue;
};

class MaterialRenderState : public RenderState
{
public:
	typedef enum
	{
		MATERIAL_FACE_FRONT = 0,
		MATERIAL_FACE_BACK,
		MATERIAL_FACE_FRONT_AND_BACK
	} MATERIAL_FACE;

	MaterialRenderState();
	MaterialRenderState(Renderer* r);
	virtual ~MaterialRenderState() {}

	MaterialRenderState& operator=(const MaterialRenderState& rhs);
	bool operator==(const MaterialRenderState& rhs);

	inline RENDER_STATE	getRenderStateType(void) const { return RENDER_STATE_MATERIAL; }
	void				setDefaultValues(void);
	void 				apply(void);

	MATERIAL_FACE	face;

	ColorRGBA		emissive;
	ColorRGBA		ambient;
	ColorRGBA		diffuse;
	ColorRGBA		specular;

	Float32			shininess;
};

class WireframeRenderState : public RenderState
{
public:
	typedef enum
	{
		WIREFRAME_MODE_NONE = 0,	// No wireframe
		WIREFRAME_MODE_ONLY,		// Wireframe only	
		WIREFRAME_MODE_ENABLED,		// Wireframe and GFX the wireframe represents		
	} WIREFRAME_MODE;

	WireframeRenderState();
	WireframeRenderState(Renderer* r);
	virtual ~WireframeRenderState() {}

	WireframeRenderState& operator=(const WireframeRenderState& rhs);
	bool operator==(const WireframeRenderState& rhs);

	inline RENDER_STATE	getRenderStateType(void) const { return RENDER_STATE_WIREFRAME; }
	void				setDefaultValues(void);
	void 				apply(void) { stateDirty = false; }

	WIREFRAME_MODE	mode;

	ColorRGBA		color;
};

class CollisionRenderState : public RenderState
{
public:
	typedef enum
	{
		COLLISION_MODE_NONE = 0,	// No collisiong
		COLLISION_MODE_ONLY,		// Collision only	
		COLLISION_MODE_ENABLED,		// Collision and GFX the wireframe represents		
	} COLLISION_MODE;

	CollisionRenderState();
	CollisionRenderState(Renderer* r);
	virtual ~CollisionRenderState() {}

	CollisionRenderState& operator=(const CollisionRenderState& rhs);
	bool operator==(const CollisionRenderState& rhs);

	inline RENDER_STATE	getRenderStateType(void) const { return RENDER_STATE_COLLISION; }
	void				setDefaultValues(void);
	void 				apply(void) { stateDirty = false; }

	COLLISION_MODE	mode;

	ColorRGBA		color;
};

class CullRenderState : public RenderState
{
public:
	typedef enum
	{
		CULL_WINDING_CCW = 0,
		CULL_WINDING_CW,
	} CULL_WINDING;

	typedef enum
	{
		CULL_TYPE_FRONT = 0,
		CULL_TYPE_BACK,
		CULL_TYPE_FRONT_AND_BACK,
	} CULL_TYPE;

	CullRenderState();
	CullRenderState(Renderer* r);
	virtual ~CullRenderState() {}

	CullRenderState& operator=(const CullRenderState& rhs);
	bool operator==(const CullRenderState& rhs);

	inline RENDER_STATE	getRenderStateType(void) const { return RENDER_STATE_CULL; }
	void				setDefaultValues(void);
	void 				apply(void);

	Boolean			enabled;
	CULL_WINDING	winding;
	CULL_TYPE		type;
};

class ZBufferRenderState : public RenderState
{
public:
	typedef enum
	{
		ZBUFFER_TEST_NEVER = 0,
		ZBUFFER_TEST_LESS,
		ZBUFFER_TEST_EQUAL,
		ZBUFFER_TEST_LEQUAL,
		ZBUFFER_TEST_GREATER,
		ZBUFFER_TEST_NOTEQUAL,
		ZBUFFER_TEST_GEQUAL,
		ZBUFFER_TEST_ALWAYS,
	} ZBUFFER_TEST;

	ZBufferRenderState();
	ZBufferRenderState(Renderer* r);
	virtual ~ZBufferRenderState() {}

	ZBufferRenderState& operator=(const ZBufferRenderState& rhs);
	bool operator==(const ZBufferRenderState& rhs);

	inline RENDER_STATE	getRenderStateType(void) const { return RENDER_STATE_ZBUFFER; }
	void				setDefaultValues(void);
	void 				apply(void);

	Boolean			enabled;
	Boolean			writable;
	ZBUFFER_TEST	test;
};

#endif /* __RENDERSTATE_H__ */
