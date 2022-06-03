/******************************************************************************
 *
 * File: SpriteAnimSet.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Sprite Anim Set Object
 * 
 *****************************************************************************/

#ifndef __SPRITE_ANIM_SET_H__
#define __SPRITE_ANIM_SET_H__

#include "Platform.h"
#include "EOSError.h"
#include "Graphics.h"
#include "SpriteSet.h"

typedef struct
{
	Uint32			endian;
	Uint32			version;
	AddressOffset	nameOffset;
	ObjectID		spriteSetID;
	AddressOffset	spriteSetNameOffset;
	Uint32			numSequences;
	AddressOffset	sequences;
	Uint32			numKeyFrames;
	AddressOffset	keyFrames;
	Uint32			numHotspotsInFrame;
	Uint32			numHotspotAttachFrames;
	AddressOffset	hotspotAttach;
	Uint32			numHotspotsXformInFrame;
	Uint32			numHotspotXformAttachFrames;
	AddressOffset	hotspotXformAttach;
	Uint32			numNameOffsets;
	AddressOffset	nameOffsets;
	AddressOffset	nameData;
} SpriteAnimSetHeader;

class SpriteAnimSetManager;

class SpriteAnimSet : public EOSObject
{
private:
	Boolean				_used;
	ObjectID			_id;
	Char*				_name;

	SpriteAnimSetManager*	_spriteAnimSetMgr;

	Uint32 				_numSequences;
	SpriteAnimSequence*	_sequences;
	Uint32				_numKeyFrames;
	SpriteAnimKeyFrame*	_keyFrames;

	Uint32				_numHotspotsInFrame;
	Uint32				_numHotspotAttachFrames;
	Point2D*			_hotspotAttach;

	Uint32				_numHotspotsXformInFrame;
	Uint32				_numHotspotXformAttachFrames;
	HotSpotData*		_hotspotXformAttach;

	Char*				_nameData;
	Char**				_nameList;

	SpriteSet*			_spriteSet;

	Uint32				_memUsage;

public:
	SpriteAnimSet();
	~SpriteAnimSet();

	EOSError					create(ObjectID objid, const Char* name, SpriteSet* set, Uint8* data, Uint32 datasize);

	EOSError					createFromFile(const Char* filename, ObjectID objid, const Char* name, SpriteSet* set, Uint8* buffer, Uint32 maxBufferSize);

	void 						destroy(void);

	inline void					setSpriteAnimSetManager(SpriteAnimSetManager* mgr) { _spriteAnimSetMgr = mgr; }

	inline ObjectID				getRefID(void) const { return _id; }
	inline Char*				getName(void) const { return _name; }

	void						setSpriteSet(SpriteSet* set);
	inline SpriteSet*			getSpriteSet(void) { return _spriteSet; }

	inline Uint32				getNumAnimSequences(void) { return _numSequences; }
	inline SpriteAnimSequence*	getAnimSequence(ObjectID objid) { return &_sequences[objid]; }

	inline Uint32				getNumKeyFrames(void) { return _numKeyFrames; }
	inline SpriteAnimKeyFrame*	getAnimKeyFrame(ObjectID objid) { return &_keyFrames[objid]; }

	inline Boolean				isUsed(void) const { return _used; }
	void						setUsed(Boolean used);

	TextureAtlasSubTexture*		findTextureAtlasSubTexture(const Char* name);

	inline Uint32				getMemoryUsage(void) { return _memUsage; }

	void						setRefID(ObjectID objid);
	void						setName(const Char* name);
};

#endif /* __SPRITE_ANIM_SET_H__ */
