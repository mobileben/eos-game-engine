/******************************************************************************
 *
 * File: SpriteAnimController.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Animation Controller for sprite based animations
 * 
 *****************************************************************************/

#ifndef __SPRITE_ANIM_CONTROLLER_H__
#define __SPRITE_ANIM_CONTROLLER_H__

#include "Platform.h"
#include "Graphics.h"
#include "SpriteAnimSet.h"

#define ANIM_FLAG_REPEAT			0x00000001
#define ANIM_FLAG_PINGPONG			0x00000002
#define ANIM_FLAG_REVERSE			0x00000004
#define ANIM_FLAG_SYSTEM_REVERSE	0x00010000

//	Add in ANIM STATE DEFINITIONS FOR BETTER ANIM CONTROL

class SpriteAnimController;

typedef void (*SpriteAnimComplete)(SpriteAnimController* controller, void* object, Uint32 event, Uint32 data);

typedef struct
{
	Uint32	state;
	Uint32	sequence;
	Uint32	keyFrame;
	Uint32	iterations;
	Uint32	frameRemainderTime;
} SpriteAnimControllerState;

class SpriteAnimController : public EOSFrameworkComponent
{
public:
	typedef enum
	{
		AnimStatePlaying = 0,
		AnimStateStopping,
		AnimStateDone,
	} AnimState;

private:
	SpriteAnimSet*			_animSet;

	AnimState				_state;

	MicroSeconds			_frameRemainderTime;

	SpriteAnimSequence*		_currAnimSequence;
	SpriteAnimSequenceID	_currAnimSequenceID;

	SpriteAnimKeyFrame*		_currKeyFrame;

	Uint32					_currKeyFrameIndex;

	Sint32					_iterations;

	SpriteAnimComplete		_completeFunc;
	void*					_completeFuncObject;

	static Char				_displayDebugStr[128];
	Boolean					_displayDebugInfo;

	Uint32 					getAnimKeyFrameAtTime(SpriteAnimSequenceID anim, MicroSeconds time, MicroSeconds& remainder);

public:
	SpriteAnimController();
	~SpriteAnimController();

	void 					reset(void);

	void					setSpriteAnimSet(SpriteAnimSet* animSet);
	void					clearSpriteAnimSet(void);
	void					destroySpriteAnimSet(void);  

	SpriteAnimSet*			getSpriteAnimSet(void);

	void					setAnim(SpriteAnimSequenceID anim, Sint32 iterations = 1);
	void					setAnimMaintainTimeline(SpriteAnimSequenceID anim, Sint32 iterations = 1);

	inline void				setCompleteFunc(SpriteAnimComplete func, void* object) { _completeFunc =  func; _completeFuncObject = object; }

	inline void				setDisplayDebugInfo(Boolean debug) { _displayDebugInfo = debug; }
	inline Boolean			getDisplayDebugInfo(void) { return _displayDebugInfo; }

	Boolean					pointInCurrentFrame(Point2D& pt);
	Boolean					pointInCurrentFrame(Point2D& pt, Point2D& offset);

	void					update(MicroSeconds microsec);

	void					draw(Sint32 x, Sint32 y, DrawControl control = 0);
	void					draw(Sint32 x, Sint32 y, DrawControl control, SpriteColorOp& colorOp);
	void					draw(Sint32 x, Sint32 y, Float32 rotate, DrawControl control);
	void					draw(Sint32 x, Sint32 y, Float32 rotate, DrawControl control, SpriteColorOp& colorOp);
	void					draw(Sint32 x, Sint32 y, Float32 scalex, Float32 scaley, Float32 rotate, DrawControl control);
	void					draw(Sint32 x, Sint32 y, Float32 scalex, Float32 scaley, Float32 rotate, DrawControl control, SpriteColorOp& colorOp);

	Boolean					setCurrentKeyFrame(Uint32 frame);

	Sprite*					getSpriteFromAnimKeyFrame(SpriteAnimSequenceID anim, Uint32 frame = 0);

	Point2D*				getCurrentHotspotRef(void);
	Point2D*				getHotspotRefFromAnimKeyFrame(SpriteAnimSequenceID anim, Uint32 frame = 0);

	SpriteColorOp*			getCurrentSpriteColorOp(void);

	Point2D*				getCurrentHotspotAttach(Uint32 type);
	Point2D*				getHotspotAttachFromAnimKeyFrame(Uint32 type, SpriteAnimSequenceID anim, Uint32 frame = 0);

	HotSpotData*			getCurrentHotspotXformAttach(Uint32 type);
	HotSpotData*			getHotspotXformAttachFromAnimKeyFrame(Uint32 type, SpriteAnimSequenceID anim, Uint32 frame = 0);

	Sprite*					getCurrentKeyFrameSprite(void);
	Float32					getCurrentKeyFrameWidth(void);
	Float32					getCurrentKeyFrameWidthFromAnimKeyFrame(SpriteAnimSequenceID anim, Uint32 frame = 0);
	Float32					getCurrentKeyFrameHeight(void);
	Float32					getCurrentKeyFrameHeightFromAnimKeyFrame(SpriteAnimSequenceID anim, Uint32 frame = 0);
	Uint32					getNumberOfAnim(void);
	SpriteAnimSequenceID	getCurrentAnim(void);
	Uint32					getCurrentKeyFrame(void);
	Uint32					getIterations(void);

	TextureAtlasSubTexture*	findTextureAtlasSubTexture(const Char* name);

	AnimState				getAnimState(void);
	MicroSeconds			getFrameRemainderTime(void);

	MicroSeconds			getAnimDuration(SpriteAnimSequenceID anim);
	MicroSeconds			getAnimCurrentTime(void);
	void					setAnimCurrentTime(SpriteAnimSequenceID anim, MicroSeconds time, Sint32 iterations = 1);

	void					saveSpriteAnimControllerState(SpriteAnimControllerState& state);
	void					restoreSpriteAnimControllerState(SpriteAnimControllerState& state);

	void					forceSetAnim(AnimState state, SpriteAnimSequenceID seq, Uint32 keyFrame, MicroSeconds frameRemainderTime, Uint32 iterations);
};

#endif /* __SPRITE_ANIM_CONTROLLER_H__ */

