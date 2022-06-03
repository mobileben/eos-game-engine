/******************************************************************************
 *
 * File: RenderState.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * RenderState
 * 
 *****************************************************************************/

#include "RenderState.h"
#include "Renderer.h"
#include "Gfx.h"

AlphaRenderState::AlphaRenderState()
{
	setDefaultValues();
}

AlphaRenderState::AlphaRenderState(Renderer* r) : RenderState(r)
{
	setDefaultValues();
}

AlphaRenderState& AlphaRenderState::operator=(const AlphaRenderState& rhs)
{
	if (&rhs != this)
	{
		stateDirty = rhs.stateDirty;

		if (rhs.renderer)
			renderer = rhs.renderer;

		enabled = rhs.enabled;
		srcFactor = rhs.srcFactor;
		dstFactor = rhs.dstFactor;

		testEnabled = rhs.testEnabled;
		test = rhs.test;
		testValue = rhs.testValue;
	}

	return *this;
}

bool AlphaRenderState::operator==(const AlphaRenderState& rhs)
{
	bool    equal = false;

	//	== returns fastest equivalence, not precise equals
	if (enabled == rhs.enabled && testEnabled == rhs.testEnabled)
	{
		equal = true;

		if (enabled == true)	//	blend is only important if alpha test is enabled
		{
			if (srcFactor != rhs.srcFactor || dstFactor != rhs.dstFactor)
				equal = false;
		}

		if (equal && testEnabled == true)	//	test features only important if turned on
		{
			if (test != rhs.test || testValue != rhs.testValue)
				equal = false;
		}
	}
	return equal;
}

void AlphaRenderState::setDefaultValues(void)
{
	stateDirty = true;

	enabled = false;
	srcFactor = ALPHA_SRC_FACTOR_ONE;
	dstFactor = ALPHA_DST_FACTOR_ZERO;
	
	testEnabled = false;
	test = ALPHA_TEST_ALWAYS;
	testValue = 0.0F;
}


void AlphaRenderState::apply(void)
{
	if (renderer->getBoundGfx()->isInFrame())
	{
#ifdef _USE_OPENGL		
		if (enabled)
		{
			GLenum  srcVal = 0;
			GLenum  dstVal = 0;

			glEnable(GL_BLEND);

			switch (srcFactor)
			{
				case ALPHA_SRC_FACTOR_ZERO:
					srcVal = GL_ZERO;
					break;

				case ALPHA_SRC_FACTOR_ONE:
					srcVal = GL_ONE;
					break;

				case ALPHA_SRC_FACTOR_DST_COLOR:
					srcVal = GL_DST_COLOR;
					break;

				case ALPHA_SRC_FACTOR_ONE_MINUS_DST_COLOR:
					srcVal = GL_ONE_MINUS_DST_COLOR;
					break;

				case ALPHA_SRC_FACTOR_SRC_ALPHA:
					srcVal = GL_SRC_ALPHA;
					break;

				case ALPHA_SRC_FACTOR_ONE_MINUS_SRC_ALPHA:
					srcVal = GL_ONE_MINUS_SRC_ALPHA;
					break;

				case ALPHA_SRC_FACTOR_DST_ALPHA:
					srcVal = GL_DST_ALPHA;
					break;

				case ALPHA_SRC_FACTOR_ONE_MINUS_DST_ALPHA:
					srcVal = GL_ONE_MINUS_DST_ALPHA;
					break;

				case ALPHA_SRC_FACTOR_SRC_ALPHA_SATURATE:
					srcVal = GL_SRC_ALPHA_SATURATE;
					break;

				case ALPHA_SRC_FACTOR_CONSTANT_COLOR:
					AssertWDesc(1 == 0, "AlphaRenderState::apply() unsupported src blend");
					break;

				case ALPHA_SRC_FACTOR_ONE_MINUS_CONSTANT_COLOR:
					AssertWDesc(1 == 0, "AlphaRenderState::apply() unsupported src blend");
					break;

				case ALPHA_SRC_FACTOR_CONSTANT_ALPHA:
					AssertWDesc(1 == 0, "AlphaRenderState::apply() unsupported src blend");
					break;

				case ALPHA_SRC_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
					AssertWDesc(1 == 0, "AlphaRenderState::apply() unsupported src blend");
					break;

				default:
					AssertWDesc(1 == 0, "AlphaRenderState::apply() unsupported src blend");
					break;
			}

			switch (dstFactor)
			{
				case ALPHA_DST_FACTOR_ZERO:
					dstVal = GL_ZERO;
					break;

				case ALPHA_DST_FACTOR_ONE:
					dstVal = GL_ONE;
					break;

				case ALPHA_DST_FACTOR_SRC_COLOR:
					dstVal = GL_SRC_COLOR;
					break;

				case ALPHA_DST_FACTOR_ONE_MINUS_SRC_COLOR:
					dstVal = GL_ONE_MINUS_SRC_COLOR;
					break;

				case ALPHA_DST_FACTOR_SRC_ALPHA:
					dstVal = GL_SRC_ALPHA;
					break;

				case ALPHA_DST_FACTOR_ONE_MINUS_SRC_ALPHA:
					dstVal = GL_ONE_MINUS_SRC_ALPHA;
					break;

				case ALPHA_DST_FACTOR_DST_ALPHA:
					dstVal = GL_DST_ALPHA;
					break;

				case ALPHA_DST_FACTOR_ONE_MINUS_DST_ALPHA:
					dstVal = GL_ONE_MINUS_DST_ALPHA;
					break;

				case ALPHA_DST_FACTOR_CONSTANT_COLOR:
					AssertWDesc(1 == 0, "AlphaRenderState::apply() unsupported dst blend");
					break;

				case ALPHA_DST_FACTOR_ONE_MINUS_CONSTANT_COLOR:
					AssertWDesc(1 == 0, "AlphaRenderState::apply() unsupported dst blend");
					break;

				case ALPHA_DST_FACTOR_CONSTANT_ALPHA:
					AssertWDesc(1 == 0, "AlphaRenderState::apply() unsupported dst blend");
					break;

				case ALPHA_DST_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
					AssertWDesc(1 == 0, "AlphaRenderState::apply() unsupported dst blend");
					break;

				default:
					AssertWDesc(1 == 0, "AlphaRenderState::apply() unsupported dst blend");
					break;
			}

			glBlendFunc(srcVal, dstVal);
		}
		else
		{
			glDisable(GL_BLEND);
		}

		if (testEnabled)
		{
			GLenum  val = 0;

			glEnable(GL_ALPHA_TEST);

			switch (test)
			{
				case ALPHA_TEST_NEVER:
					val = GL_NEVER;
					break;

				case ALPHA_TEST_LESS:
					val = GL_LESS;
					break;

				case ALPHA_TEST_EQUAL:
					val = GL_EQUAL;
					break;

				case ALPHA_TEST_LEQUAL:
					val = GL_LEQUAL;
					break;

				case ALPHA_TEST_GREATER:
					val = GL_GREATER;
					break;

				case ALPHA_TEST_NOTEQUAL:
					val = GL_NOTEQUAL;
					break;

				case ALPHA_TEST_GEQUAL:
					val = GL_GEQUAL;
					break;

				case ALPHA_TEST_ALWAYS:
					val = GL_ALWAYS;
					break;

				default:
					AssertWDesc(1 == 0, "AlphaRenderState::apply() unsupported alpha test");
					break;
			}

			glAlphaFunc(val, testValue);
		}
		else
		{
			glDisable(GL_ALPHA_TEST);
		}
#endif /* _USE_OPENGL */

		stateDirty = false;
	}
	else
		stateDirty = true;
}

MaterialRenderState::MaterialRenderState()
{
	setDefaultValues();
}

MaterialRenderState::MaterialRenderState(Renderer* r) : RenderState(r)
{
	setDefaultValues();
}

MaterialRenderState& MaterialRenderState::operator=(const MaterialRenderState& rhs)
{
	if (&rhs != this)
	{
		face = rhs.face;

		emissive = rhs.emissive;
		ambient = rhs.ambient;
		diffuse = rhs.diffuse;

		shininess = rhs.shininess;
	}

	return *this;
}

bool MaterialRenderState::operator==(const MaterialRenderState& rhs)
{
	bool    equal = false;

	if (face == rhs.face &&
		emissive.r == rhs.emissive.r && emissive.g == rhs.emissive.g && emissive.b == rhs.emissive.b && emissive.a == rhs.emissive.a &&
		ambient.r == rhs.ambient.r && ambient.g == rhs.ambient.g && ambient.b == rhs.ambient.b && ambient.a == rhs.ambient.a &&
		diffuse.r == rhs.diffuse.r && diffuse.g == rhs.diffuse.g && diffuse.b == rhs.diffuse.b && diffuse.a == rhs.diffuse.a &&
		shininess == rhs.shininess)
	{
		equal = true;
	}

	return equal;
}

void MaterialRenderState::setDefaultValues(void)
{
	stateDirty = true;

	face = MATERIAL_FACE_FRONT;

	emissive.r = emissive.g = emissive.b = 0.0F;
	emissive.a = 1.0F;

	ambient.r = ambient.g = ambient.b = 0.0F;
	ambient.a = 1.0F;

	diffuse.r = diffuse.g = diffuse.b = 0.0F;
	diffuse.a = 1.0F;

	specular.r = specular.g = specular.b = 0.0F;
	specular.a = 1.0F;

	shininess = 0.0F;
}

void MaterialRenderState::apply(void)
{
	if (renderer->getBoundGfx()->isInFrame())
	{
#ifdef _USE_OPENGL
		GLenum  glface = GL_FRONT;

		switch (face)
		{
			case MATERIAL_FACE_FRONT:
				glface = GL_FRONT;
				break;

			case MATERIAL_FACE_BACK:
				glface = GL_BACK;
				break;

			case MATERIAL_FACE_FRONT_AND_BACK:
				glface = GL_FRONT_AND_BACK;
				break;

			default:
				AssertWDesc(1 == 0, "MaterialRenderState::apply() unsupported face");
				break;
		}

		glMaterialfv(glface, GL_EMISSION, (Float32*) &emissive);
		glMaterialfv(glface, GL_DIFFUSE, (Float32*) &diffuse);
		glMaterialfv(glface, GL_AMBIENT, (Float32*) &ambient);
		glMaterialfv(glface, GL_SPECULAR, (Float32*) &specular);
		glMaterialf(glface, GL_SHININESS, shininess);
#endif /* _USE_OPENGL */

		stateDirty = false;
	}
	else
		stateDirty = true;
}

WireframeRenderState::WireframeRenderState()
{
	setDefaultValues();
}

WireframeRenderState::WireframeRenderState(Renderer* r) : RenderState(r)
{
	setDefaultValues();
}

WireframeRenderState& WireframeRenderState::operator=(const WireframeRenderState& rhs)
{
	if (&rhs != this)
	{
		mode = rhs.mode;
		color = rhs.color;
	}

	return *this;
}

bool WireframeRenderState::operator==(const WireframeRenderState& rhs)
{
	bool    equal = false;

	if (mode == rhs.mode &&
		color.r == rhs.color.r && color.g == rhs.color.g && color.b == rhs.color.b && color.a == rhs.color.a)
	{
		equal = true;
	}

	return equal;
}

void WireframeRenderState::setDefaultValues(void)
{
	stateDirty = true;

	mode = WIREFRAME_MODE_NONE;

	color.r = 1.0F;
	color.g = 0.0F;
	color.b = 0.0F;
	color.a = 1.0F;
}

CollisionRenderState::CollisionRenderState()
{
	setDefaultValues();
}

CollisionRenderState::CollisionRenderState(Renderer* r) : RenderState(r)
{
	setDefaultValues();
}

CollisionRenderState& CollisionRenderState::operator=(const CollisionRenderState& rhs)
{
	if (&rhs != this)
	{
		mode = rhs.mode;
		color = rhs.color;
	}

	return *this;
}

bool CollisionRenderState::operator==(const CollisionRenderState& rhs)
{
	bool    equal = false;

	if (mode == rhs.mode &&
		color.r == rhs.color.r && color.g == rhs.color.g && color.b == rhs.color.b && color.a == rhs.color.a)
	{
		equal = true;
	}

	return equal;
}

void CollisionRenderState::setDefaultValues(void)
{
	stateDirty = true;

	mode = COLLISION_MODE_NONE;

	color.r = 0.0F;
	color.g = 1.0F;
	color.b = 0.0F;
	color.a = 1.0F;
}

CullRenderState::CullRenderState()
{
	setDefaultValues();
}

CullRenderState::CullRenderState(Renderer* r) : RenderState(r)
{
	setDefaultValues();
}

CullRenderState& CullRenderState::operator=(const CullRenderState& rhs)
{
	if (&rhs != this)
	{
		enabled = rhs.enabled;
		winding = rhs.winding;
		type = rhs.type;
	}

	return *this;
}

bool CullRenderState::operator==(const CullRenderState& rhs)
{
	bool    equal = false;

	if (enabled == rhs.enabled)
	{
		equal = true;

		if (enabled == true)
		{
			if (winding != rhs.winding || type != rhs.type)
			{
				equal = false;
			}
		}
	}

	return equal;
}

void CullRenderState::setDefaultValues(void)
{
	stateDirty = true;

	enabled =false;
	winding = CULL_WINDING_CCW;
	type = CULL_TYPE_BACK;
}

void CullRenderState::apply(void)
{
	if (renderer->getBoundGfx()->isInFrame())
	{
#ifdef _USE_OPENGL
		if (enabled)
		{
			GLenum  val = 0;

			glEnable(GL_CULL_FACE);

			switch (type)
			{
				case CULL_TYPE_FRONT:
					val = GL_FRONT;
					break;

				case CULL_TYPE_BACK:
					val = GL_BACK;
					break;

				case CULL_TYPE_FRONT_AND_BACK:
					val = GL_FRONT_AND_BACK;
					break;

				default:
					AssertWDesc(1 == 0, "CullRenderState::apply() unsupported type");
					break;
			}

			glCullFace(val);

			val = 0;

			switch (winding)
			{
				case CULL_WINDING_CCW:
					val = GL_CCW;
					break;

				case CULL_WINDING_CW:
					val = GL_CW;
					break;

				default:
					AssertWDesc(1 == 0, "CullRenderState::apply() unsupported winding");
					break;
			}

			glFrontFace(val);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
#endif /* _USE_OPENGL */

		stateDirty = false;
	}
	else
		stateDirty = true;
}

ZBufferRenderState::ZBufferRenderState()
{
	setDefaultValues();
}

ZBufferRenderState::ZBufferRenderState(Renderer* r) : RenderState(r)
{
	setDefaultValues();
}

ZBufferRenderState& ZBufferRenderState::operator=(const ZBufferRenderState& rhs)
{
	if (&rhs != this)
	{
		enabled = rhs.enabled;
		writable = rhs.writable;
		test = rhs.test;
	}

	return *this;
}

bool ZBufferRenderState::operator==(const ZBufferRenderState& rhs)
{
	bool    equal = false;

	if (enabled == rhs.enabled)
	{
		equal = true;

		if (enabled == true)
		{
			if (writable != rhs.writable || test != rhs.test)
			{
				equal = false;
			}
		}
	}

	return equal;
}

void ZBufferRenderState::setDefaultValues(void)
{
	stateDirty = true;

	enabled = false;
	writable = true;
	test = ZBUFFER_TEST_LESS;
}

void ZBufferRenderState::apply(void)
{
	if (renderer->getBoundGfx()->isInFrame())
	{
#ifdef _USE_OPENGL
		GLenum val = 0;

		if (enabled)
		{
#ifdef _USE_OPENGL_ES_1_1

			glEnable(GL_DEPTH_TEST);

#else
			
			glEnable(GL_DEPTH);
			
#endif /* _USE_OPENGL_ES_1_1 */

			if (writable)
				glDepthMask(GL_TRUE);
			else
				glDepthMask(GL_FALSE);

			switch (test)
			{
				case ZBUFFER_TEST_NEVER:
					val = GL_NEVER;
					break;

				case ZBUFFER_TEST_LESS:
					val = GL_LESS;
					break;

				case ZBUFFER_TEST_EQUAL:
					val = GL_EQUAL;
					break;

				case ZBUFFER_TEST_LEQUAL:
					val = GL_LEQUAL;
					break;

				case ZBUFFER_TEST_GREATER:
					val = GL_GREATER;
					break;

				case ZBUFFER_TEST_NOTEQUAL:
					val = GL_NOTEQUAL;
					break;

				case ZBUFFER_TEST_GEQUAL:
					val = GL_GEQUAL;
					break;

				case ZBUFFER_TEST_ALWAYS:
					val = GL_ALWAYS;
					break;

				default:
					AssertWDesc(1 == 0, "ZBufferRenderState::apply() unsupported test");
					break;
			}

			glDepthFunc(val);
		}
		else
		{
#ifdef _USE_OPENGL_ES_1_1
			
			glDisable(GL_DEPTH_TEST);
			
#else
			
			glDisable(GL_DEPTH);
			
#endif /* _USE_OPENGL_ES_1_1 */
		}
#endif /* _USE_OPENGL */

		stateDirty = false;
	}
	else
		stateDirty = true;
}

