/******************************************************************************
 *
 * File: SpriteAnimController.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Animation Controller for sprite based animations
 * 
 *****************************************************************************/

#include "App.h"
#include "Renderer.h"
#include "Graphics.h"
#include "SpriteSet.h"
#include "SpriteAnimController.h"

Char SpriteAnimController::_displayDebugStr[128];

SpriteAnimController::SpriteAnimController()
{
	_animSet = NULL;

	_state = AnimStateDone;

	_frameRemainderTime = 0;

	_currAnimSequence = NULL;
	_currKeyFrame = NULL;

	_currAnimSequenceID = 0;
	_currKeyFrameIndex = 0;

	_iterations = 0;

	_displayDebugInfo = false;

	_completeFunc = NULL;

	_completeFuncObject = NULL;
}

SpriteAnimController::~SpriteAnimController()
{
}

MicroSeconds SpriteAnimController::getAnimDuration(SpriteAnimSequenceID anim)
{
	MicroSeconds 		time = 0;
	SpriteAnimSequence*	seq;
	Uint32				i;

	if (_animSet && anim < _animSet->getNumAnimSequences())
	{
		seq = _animSet->getAnimSequence(anim);

		for (i=0;i<seq->numKeyFrames;i++)
		{
			time += seq->keyFrames[i].duration;
		}
	}

	return time;
}

MicroSeconds SpriteAnimController::getAnimCurrentTime(void)
{
	MicroSeconds 	time = 0;
	Uint32			i;

	if (_animSet && _currAnimSequence)
	{
		for (i=0;i<_currKeyFrameIndex;i++)
		{
			time += _currAnimSequence->keyFrames[i].duration;
		}

		//	Now add the last one
		if (_currKeyFrameIndex < _currAnimSequence->numFrames)
			time += _currAnimSequence->keyFrames[_currKeyFrameIndex].duration - _frameRemainderTime;
	}

	return time;
}

void SpriteAnimController::setAnimCurrentTime(SpriteAnimSequenceID anim, MicroSeconds time, Sint32 iterations)
{
	MicroSeconds 	totalTime = 0;
	Uint32			i;

	if (_animSet)
	{
		_currAnimSequenceID = anim;
		_currAnimSequence = _animSet->getAnimSequence(_currAnimSequenceID);
		_iterations = iterations;

		setCurrentKeyFrame(0);

		for (i=0;i<_currAnimSequence->numKeyFrames;i++)
		{
			if (time > totalTime)
			{
				if ((_currAnimSequence->keyFrames[i].duration + totalTime) < time)
				{
					totalTime += _currAnimSequence->keyFrames[i].duration;
				}
				else
				{
					setCurrentKeyFrame(i);
					_frameRemainderTime = time - totalTime;
					break;
				}
			}
			else
			{
				if (time == 0)
					setCurrentKeyFrame(0);
				else
				{
					setCurrentKeyFrame(_currAnimSequence->numKeyFrames - 1);
					_frameRemainderTime = 0;
				}
				break;
			}
		}

		if (_iterations != 0)
		{
			_state = AnimStatePlaying;
		}
		else
		{
			_state = AnimStateDone;
		}
	}
}

Uint32 SpriteAnimController::getAnimKeyFrameAtTime(SpriteAnimSequenceID anim, MicroSeconds time, MicroSeconds& remainder)
{
	Uint32				frame = 0;
	MicroSeconds		rem = 0;
	SpriteAnimSequence*	seq;
	Uint32				i;

	remainder = 0;

	if (_animSet && anim < _animSet->getNumAnimSequences())
	{
		rem = time;

		seq = _animSet->getAnimSequence(anim);

		for (i=0;i<seq->numKeyFrames;i++)
		{
			if (seq->keyFrames[frame].duration >= rem)
			{
				rem = seq->keyFrames[frame].duration - rem;
				remainder = rem;
				break;
			}
			else
			{
				rem -= seq->keyFrames[frame].duration;
				frame++;
			}
		}
	}

	return frame;
}

void SpriteAnimController::reset(void)
{
	_currAnimSequenceID = 0;
	_currKeyFrameIndex = 0;
	_iterations = 0;

	setAnim(0);
}

void SpriteAnimController::setSpriteAnimSet(SpriteAnimSet* animSet)
{
	_animSet = animSet;

	reset();
}

void SpriteAnimController::clearSpriteAnimSet(void)
{
	_animSet = NULL;
}

void SpriteAnimController::destroySpriteAnimSet(void)
{
	SpriteSet*			spriteSet;
	TextureAtlas*		atlas;
	const TextureState*	tstate;
	Texture*			tex;
	Uint32				i;

	if (_animSet)
	{
		spriteSet = _animSet->getSpriteSet();

		if (spriteSet)
		{
			for (i=0;i<spriteSet->getNumTextureAtlas();i++)
			{
				atlas = spriteSet->getTextureAtlas(i);
	
				if (atlas)
				{
					tstate = atlas->getTextureState();
	
					tex = tstate->_textureObj;
	
					if (tex)
						_appRefPtr->getTextureManager()->destroyTexture(*tex);
	
					_appRefPtr->getTextureAtlasManager()->destroyTextureAtlas(*atlas);
				}
			}

			_appRefPtr->getSpriteSetManager()->destroySpriteSet(*spriteSet);
		}

		_appRefPtr->getSpriteAnimSetManager()->destroySpriteAnimSet(*_animSet);
	}

	_animSet = NULL;
}

SpriteAnimSet* SpriteAnimController::getSpriteAnimSet(void)
{
	return _animSet;
}

void SpriteAnimController::setAnim(SpriteAnimSequenceID anim, Sint32 iterations)
{
	if (_animSet)
	{
		_currAnimSequenceID = anim;
		_currAnimSequence = _animSet->getAnimSequence(_currAnimSequenceID);

		setCurrentKeyFrame(0);

		_iterations = iterations;

		if (_iterations != 0)
		{
			_state = AnimStatePlaying;
		}
		else
		{
			_state = AnimStateDone;
		}
	}
}

void SpriteAnimController::setAnimMaintainTimeline(SpriteAnimSequenceID anim, Sint32 iterations)
{
	if (_animSet)
	{
		Boolean			equalTimeline = false;
		MicroSeconds	currTime = 0;

		if (getAnimDuration(anim) == getAnimDuration(_currAnimSequenceID))
		{
			equalTimeline = true;
			currTime = getAnimCurrentTime();
		}

		_currAnimSequenceID = anim;
		_currAnimSequence = _animSet->getAnimSequence(_currAnimSequenceID);

		//	Save current anim reference time, if bounds don't match, then simply reset back to 0
		if (equalTimeline == false)
		{
			setCurrentKeyFrame(0);
		}
		else
		{
			_currKeyFrameIndex = getAnimKeyFrameAtTime(anim, currTime, _frameRemainderTime);
			_currKeyFrame = _currAnimSequence->keyFrames + _currKeyFrameIndex;
		}

		_iterations = iterations;

		if (_iterations != 0)
		{
			_state = AnimStatePlaying;
		}
		else
		{
			_state = AnimStateDone;
		}
	}
}

Boolean SpriteAnimController::pointInCurrentFrame(Point2D& pt)
{
	Boolean	in = false;

	if (_animSet && _currKeyFrame)
	{
		Sprite*			sprite;

		//	First pass assumes no issues with rotation
		sprite = _animSet->getSpriteSet()->getSprite(_currKeyFrame->spriteNumber);

		if (sprite)
		{
			Float32	dx, dy;

			dx = (_currKeyFrame->hotspotRef.x) - sprite->xy->x - pt.x;

			if (dx < 0.0)
				dx = -dx;

			if (dx <= sprite->width / 2.0)
			{
				dy = (_currKeyFrame->hotspotRef.y) - sprite->xy->y - pt.y;

				if (dy < 0.0)
					dy = -dy;

				if (dy <= sprite->height / 2.0)
					in = true;
			}
		}
	}

	return in;
}

Boolean SpriteAnimController::pointInCurrentFrame(Point2D& pt, Point2D& offset)
{
	Boolean	in = false;

	if (_animSet && _currKeyFrame)
	{
		Sprite*			sprite;

		//	First pass assumes no issues with rotation
		sprite = _animSet->getSpriteSet()->getSprite(_currKeyFrame->spriteNumber);

		if (sprite)
		{
			Float32	dx, dy;

			dx = (offset.x + _currKeyFrame->hotspotRef.x) - pt.x;

			if (dx < 0.0)
				dx = -dx;

			if (dx <= sprite->width / 2.0)
			{
				dy = (offset.y + _currKeyFrame->hotspotRef.y) - pt.y;

				if (dy < 0.0)
					dy = -dy;

				if (dy <= sprite->height / 2.0)
					in = true;
			}
		}
	}

	return in;
}

void SpriteAnimController::update(MicroSeconds microsec)
{
	if (_state == AnimStatePlaying)
	{
		Uint32	frame = _currKeyFrameIndex;
	
		if (microsec > _frameRemainderTime)
		{
			frame++;
			microsec -= _frameRemainderTime;

			while (microsec >= 0)
			{
				if (setCurrentKeyFrame(frame))
				{
					frame = _currKeyFrameIndex;

					if (microsec > _frameRemainderTime)
					{
						microsec -= _frameRemainderTime;
						frame++;
					}
					else
					{
						_frameRemainderTime -= microsec;
						break;
					}
				}
				else
					break;
			}
		}
		else
			_frameRemainderTime -= microsec;
	}
}

void SpriteAnimController::draw(Sint32 x, Sint32 y, DrawControl control)
{
	Sprite*			sprite;
	Point2D				pos;

	if (_animSet && _currKeyFrame)
	{
		sprite = _animSet->getSpriteSet()->getSprite(_currKeyFrame->spriteNumber);

		if (control & CONTROL_FLIP_H)
			pos.x = x - _currKeyFrame->hotspotRef.x;
		else
			pos.x = x + _currKeyFrame->hotspotRef.x;

		if (control & CONTROL_FLIP_V)
			pos.y = y - _currKeyFrame->hotspotRef.y;
		else
			pos.y = y + _currKeyFrame->hotspotRef.y;

		_app->getRenderer()->drawSprite(*sprite, pos, 0.0, control);

		if (_displayDebugInfo)
		{
			Point2D		pt;
			Sprite*		sprite;

			//	First pass assumes no issues with rotation
			sprite = _animSet->getSpriteSet()->getSprite(_currKeyFrame->spriteNumber);

			pt.x = pos.x;
			pt.y = pos.y;

			sprintf(_displayDebugStr, "%d : %d", _currAnimSequenceID, _currKeyFrameIndex);
			_app->drawDebugString((Sint32) pt.x, (Sint32) (pt.y + sprite->height / 2.0F), _displayDebugStr);
		}
	}
}

void SpriteAnimController::draw(Sint32 x, Sint32 y, DrawControl control, SpriteColorOp& colorOp)
{
	Sprite*			sprite;
	Point2D				pos;

	if (_animSet && _currKeyFrame)
	{
		sprite = _animSet->getSpriteSet()->getSprite(_currKeyFrame->spriteNumber);

		if (control & CONTROL_FLIP_H)
			pos.x = x - _currKeyFrame->hotspotRef.x;
		else
			pos.x = x + _currKeyFrame->hotspotRef.x;

		if (control & CONTROL_FLIP_V)
			pos.y = y - _currKeyFrame->hotspotRef.y;
		else
			pos.y = y + _currKeyFrame->hotspotRef.y;

		_app->getRenderer()->drawSprite(*sprite, pos, 0.0, control, colorOp);

		if (_displayDebugInfo)
		{
			Point2D		pt;
			Sprite*		sprite;

			//	First pass assumes no issues with rotation
			sprite = _animSet->getSpriteSet()->getSprite(_currKeyFrame->spriteNumber);

			pt.x = pos.x;
			pt.y = pos.y;

			sprintf(_displayDebugStr, "%d : %d", _currAnimSequenceID, _currKeyFrameIndex);
			_app->drawDebugString((Sint32) pt.x, (Sint32) (pt.y + sprite->height / 2.0F), _displayDebugStr);
		}
	}
}

void SpriteAnimController::draw(Sint32 x, Sint32 y, Float32 rotate, DrawControl control)
{
	Sprite*			sprite;
	Point2D				pos;

	if (_animSet && _currKeyFrame)
	{
		sprite = _animSet->getSpriteSet()->getSprite(_currKeyFrame->spriteNumber);

		if (control & CONTROL_FLIP_H)
			pos.x = x - _currKeyFrame->hotspotRef.x;
		else
			pos.x = x + _currKeyFrame->hotspotRef.x;

		if (control & CONTROL_FLIP_V)
			pos.y = y - _currKeyFrame->hotspotRef.y;
		else
			pos.y = y + _currKeyFrame->hotspotRef.y;

		_app->getRenderer()->drawSprite(*sprite, pos, rotate, control);
	}
}

void SpriteAnimController::draw(Sint32 x, Sint32 y, Float32 rotate, DrawControl control, SpriteColorOp& colorOp)
{
	Sprite*			sprite;
	Point2D				pos;

	if (_animSet && _currKeyFrame)
	{
		sprite = _animSet->getSpriteSet()->getSprite(_currKeyFrame->spriteNumber);

		if (control & CONTROL_FLIP_H)
			pos.x = x - _currKeyFrame->hotspotRef.x;
		else
			pos.x = x + _currKeyFrame->hotspotRef.x;

		if (control & CONTROL_FLIP_V)
			pos.y = y - _currKeyFrame->hotspotRef.y;
		else
			pos.y = y + _currKeyFrame->hotspotRef.y;

		_app->getRenderer()->drawSprite(*sprite, pos, rotate, control, colorOp);
	}
}

void SpriteAnimController::draw(Sint32 x, Sint32 y, Float32 scalex, Float32 scaley, Float32 rotate, DrawControl control)
{
	Sprite*			sprite;
	Point2D				pos;

	if (_animSet && _currKeyFrame)
	{
		sprite = _animSet->getSpriteSet()->getSprite(_currKeyFrame->spriteNumber);

		if (control & CONTROL_FLIP_H)
			pos.x = x - _currKeyFrame->hotspotRef.x;
		else
			pos.x = x + _currKeyFrame->hotspotRef.x;

		if (control & CONTROL_FLIP_H)
			pos.y = y - _currKeyFrame->hotspotRef.y;
		else
			pos.y = y + _currKeyFrame->hotspotRef.y;

		_app->getRenderer()->drawSprite(*sprite, pos, scalex, scaley, rotate, control);
	}
}

void SpriteAnimController::draw(Sint32 x, Sint32 y, Float32 scalex, Float32 scaley, Float32 rotate, DrawControl control, SpriteColorOp& colorOp)
{
	Sprite*			sprite;
	Point2D				pos;

	if (_animSet && _currKeyFrame)
	{
		sprite = _animSet->getSpriteSet()->getSprite(_currKeyFrame->spriteNumber);

		if (control & CONTROL_FLIP_H)
			pos.x = x - _currKeyFrame->hotspotRef.x;
		else
			pos.x = x + _currKeyFrame->hotspotRef.x;

		if (control & CONTROL_FLIP_H)
			pos.y = y - _currKeyFrame->hotspotRef.y;
		else
			pos.y = y + _currKeyFrame->hotspotRef.y;

		_app->getRenderer()->drawSprite(*sprite, pos, scalex, scaley, rotate, control, colorOp);
	}
}

Boolean SpriteAnimController::setCurrentKeyFrame(Uint32 frame)
{
	if (_animSet)
	{
		if (frame < _currAnimSequence->numKeyFrames)
		{
			_currKeyFrame = _currAnimSequence->keyFrames + frame;
			_currKeyFrameIndex = frame;
			_frameRemainderTime = _currKeyFrame->duration;
		}
		else if (frame == _currAnimSequence->numKeyFrames)
		{
			if (_iterations == -1)
			{
				frame = 0;

				_currKeyFrame = _currAnimSequence->keyFrames + frame;
				_currKeyFrameIndex = frame;
				_frameRemainderTime = _currKeyFrame->duration;
			}
			else if (_iterations > 0)
			{
				_iterations--;

				if (_iterations == 0)
				{
					//	We are done
					if (_completeFunc)
						_completeFunc(this, _completeFuncObject, AnimStateDone, 0);
					return false;
				}
				else
				{
					frame = 0;
	
					_currKeyFrame = _currAnimSequence->keyFrames + frame;
					_currKeyFrameIndex = frame;
					_frameRemainderTime = _currKeyFrame->duration;
				}
			}
			else
			{
				//	We are done
				if (_completeFunc)
					_completeFunc(this, _completeFuncObject, AnimStateDone, 0);
				return false;
			}
		}
		else
		{
			//	This means we've exceeded it, this is an error
			AssertWDesc(1 == 0, "SpriteAnimController::setCurrentKeyFrame() frame out of bounds");
			return false;
		}

		return true;
	}

	return false;
}

Uint32 SpriteAnimController::getNumberOfAnim(void)
{
	if (_animSet)
		return _animSet->getNumAnimSequences();
	else
		return 0;
}

SpriteAnimSequenceID SpriteAnimController::getCurrentAnim(void)
{
	return _currAnimSequenceID;
}

Uint32 SpriteAnimController::getCurrentKeyFrame(void)
{
	return _currKeyFrameIndex;
}

Uint32 SpriteAnimController::getIterations(void)
{
	return _iterations;
}

TextureAtlasSubTexture* SpriteAnimController::findTextureAtlasSubTexture(const Char* name)
{
	if (_animSet)
		return _animSet->findTextureAtlasSubTexture(name);
	else
		return NULL;
}

SpriteAnimController::AnimState SpriteAnimController::getAnimState(void)
{
	return _state;
}

MicroSeconds SpriteAnimController::getFrameRemainderTime(void)
{
	return _frameRemainderTime;
}

Sprite* SpriteAnimController::getSpriteFromAnimKeyFrame(SpriteAnimSequenceID anim, Uint32 frame)
{
	Sprite* 			sprite = NULL;
	SpriteAnimSequence*	seq;
	SpriteAnimKeyFrame*	keyframe;

	if (_animSet)
	{
		seq = _animSet->getAnimSequence(anim);

		if (seq)
		{
			keyframe = seq->keyFrames + frame;

			sprite = _animSet->getSpriteSet()->getSprite(keyframe->spriteNumber);
		}
	}

	return sprite;
}

Point2D* SpriteAnimController::getCurrentHotspotRef(void)
{
	if (_currKeyFrame)
		return &_currKeyFrame->hotspotRef;
	else
		return NULL;
}

Point2D* SpriteAnimController::getHotspotRefFromAnimKeyFrame(SpriteAnimSequenceID anim, Uint32 frame)
{
	SpriteAnimSequence*	seq;
	SpriteAnimKeyFrame*	keyframe;

	if (_animSet)
	{
		seq = _animSet->getAnimSequence(anim);

		if (seq)
		{
			keyframe = seq->keyFrames + frame;

			return &keyframe->hotspotRef;
		}
	}

	return NULL;
}

Point2D* SpriteAnimController::getCurrentHotspotAttach(Uint32 type)
{
	if (_currKeyFrame)
		return &_currKeyFrame->hotspotAttach[type];
	else
		return NULL;
}

Point2D* SpriteAnimController::getHotspotAttachFromAnimKeyFrame(Uint32 type, SpriteAnimSequenceID anim, Uint32 frame)
{
	SpriteAnimSequence*	seq;
	SpriteAnimKeyFrame*	keyframe;

	if (_animSet)
	{
		seq = _animSet->getAnimSequence(anim);

		if (seq)
		{
			keyframe = seq->keyFrames + frame;

			return &keyframe->hotspotAttach[type];
		}
	}

	return NULL;
}

SpriteColorOp* SpriteAnimController::getCurrentSpriteColorOp(void)
{
	if (_currKeyFrame)
	{
		Sprite*			sprite;

		sprite = _animSet->getSpriteSet()->getSprite(_currKeyFrame->spriteNumber);

		if (sprite)
			return sprite->colorOp;
		else
			return NULL;
	}
	else
		return NULL;
}

HotSpotData* SpriteAnimController::getCurrentHotspotXformAttach(Uint32 type)
{
	if (_currKeyFrame)
		return &_currKeyFrame->hotspotXformAttach[type];
	else
		return NULL;
}

HotSpotData* SpriteAnimController::getHotspotXformAttachFromAnimKeyFrame(Uint32 type, SpriteAnimSequenceID anim, Uint32 frame)
{
	SpriteAnimSequence*	seq;
	SpriteAnimKeyFrame*	keyframe;

	if (_animSet)
	{
		seq = _animSet->getAnimSequence(anim);

		if (seq)
		{
			keyframe = seq->keyFrames + frame;

			return &keyframe->hotspotXformAttach[type];
		}
	}

	return NULL;
}

Sprite* SpriteAnimController::getCurrentKeyFrameSprite(void)
{
	Sprite*			sprite = NULL;

	if (_animSet && _currKeyFrame)
		sprite = _animSet->getSpriteSet()->getSprite(_currKeyFrame->spriteNumber);

	return sprite;
}

Float32 SpriteAnimController::getCurrentKeyFrameWidth(void)
{
	if (_animSet && _currKeyFrame)
		return _animSet->getSpriteSet()->getSprite(_currKeyFrame->spriteNumber)->width;

	return 0.0F;
}

Float32 SpriteAnimController::getCurrentKeyFrameWidthFromAnimKeyFrame(SpriteAnimSequenceID anim, Uint32 frame)
{
	SpriteAnimSequence*	seq;
	SpriteAnimKeyFrame*	keyframe;

	if (_animSet)
	{
		seq = _animSet->getAnimSequence(anim);

		if (seq)
		{
			keyframe = seq->keyFrames + frame;

			return _animSet->getSpriteSet()->getSprite(keyframe->spriteNumber)->width;
		}
	}

	return 0.0F;
}

Float32 SpriteAnimController::getCurrentKeyFrameHeight(void)
{
	if (_animSet && _currKeyFrame)
		return _animSet->getSpriteSet()->getSprite(_currKeyFrame->spriteNumber)->height;

	return 0.0F;
}

Float32 SpriteAnimController::getCurrentKeyFrameHeightFromAnimKeyFrame(SpriteAnimSequenceID anim, Uint32 frame)
{
	SpriteAnimSequence*	seq;
	SpriteAnimKeyFrame*	keyframe;

	if (_animSet)
	{
		seq = _animSet->getAnimSequence(anim);

		if (seq)
		{
			keyframe = seq->keyFrames + frame;

			return _animSet->getSpriteSet()->getSprite(keyframe->spriteNumber)->height;
		}
	}

	return 0.0F;
}

void SpriteAnimController::forceSetAnim(AnimState state, SpriteAnimSequenceID seq, Uint32 keyFrame, MicroSeconds frameRemainderTime, Uint32 iterations)
{
	setAnim(seq, iterations);
	setCurrentKeyFrame(keyFrame);

	_state = state;
	_frameRemainderTime = frameRemainderTime;

}

void SpriteAnimController::saveSpriteAnimControllerState(SpriteAnimControllerState& state)
{
	state.state = _state;
	state.sequence = _currAnimSequenceID;
	state.keyFrame = _currKeyFrameIndex;
	state.iterations = _iterations;
	state.frameRemainderTime = _frameRemainderTime;
}

void SpriteAnimController::restoreSpriteAnimControllerState(SpriteAnimControllerState& state)
{
	_state = (AnimState) state.state;
	_currAnimSequenceID = state.sequence;
	_currKeyFrameIndex = state.keyFrame;
	_iterations = state.iterations;
	_frameRemainderTime = state.frameRemainderTime;

	if (_animSet)
	{
		_currAnimSequence = _animSet->getAnimSequence(_currAnimSequenceID);
		_currKeyFrame = _currAnimSequence->keyFrames + _currKeyFrameIndex;
	}
}

