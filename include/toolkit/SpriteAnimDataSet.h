/******************************************************************************
 *
 * File: SpriteAnimDataSet.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Parts for encapsulating and loading Sprite Anim Data sets
 * 
 *****************************************************************************/

#ifndef __SPRITEANIMDATASET_H__
#define __SPRITEANIMDATASET_H__

#include "Platform.h"
#include "toolkit/GraphicsToolKit.h"

#include <iterator>
#include <vector>

typedef struct
{
	HotSpotData*	data;
} HotSpotDataContainer;

typedef struct
{
	Point2D*	point2D;
} Point2DContainer;

class SpriteAnimDataSet
{
private:
	vector<SpriteAnimSequenceRAW>		_animSequences;
	vector<SpriteAnimKeyFrameFlashRAW>	_keyFrames;
	vector<Point2DContainer>			_hotspotFrames;
	vector<HotSpotDataContainer>		_hotspotXformFrames;

	vector<string>						_sequenceNames;

	Endian								_endian;

	Float32								_frame_to_time;

	Uint32								_numExportHotSpotAttach;
	Uint32								_numExportHotSpotXformAttach;

	Char*								_exportNames;
	Uint32*								_exportNamesOffsets;
	Uint32								_exportNamesSize;

	Uint32								_spriteSetID;

	Uint32								_version;
	Boolean								_comments;

	void		swapHeaderIfNecessary(FlashSpriteAnimDataHeader* header);
	void		swapAnimSequenceIfNecessary(SpriteAnimSequenceRAW* seq);
	void		swapAnimKeyFrameIfNecessary(SpriteAnimKeyFrameFlashRAW* frame);
	void 		swapNameOffsetIfNecessary(Uint32* offset);
	EOSError 	loadAnimSequences(Uint32 num, Uint8* buffer,Uint32 size);
	EOSError 	loadAnimKeyFrames(Uint32 num, Uint8* buffer,Uint32 size);
	EOSError 	loadNames(Uint32 num, Uint8* buffer,Uint32 size, Uint8* databuffer,Uint32 datasize);
	EOSError 	parseAnimData(Uint8* data, Uint32 size);

	EOSError 	addExportName(Char* name, Char** nameList, Uint32& size, Uint32& offset);

	Uint32		addHotspotFrame(Point2D* hotspots);
	Uint32		addHotspotXformFrame(HotSpotDataRAW* hotspots);

public:
	SpriteAnimDataSet();
	~SpriteAnimDataSet();

	Uint32		getNumAnimSequences(void);
	vector<SpriteAnimSequenceRAW>::iterator			getAnimSequenceAt(Uint32 index);

	Uint32		getNumKeyFrames(void);
	vector<SpriteAnimKeyFrameFlashRAW>::iterator 	getKeyFrameAt(Uint32 index);

	Uint32		getNumSequenceNames(void);
	vector<string>::iterator 						getSequenceNameAt(Uint32 index);

	inline Boolean	getComments(void) { return _comments; }
	inline void		setComments(Boolean comments) { _comments = comments; }

	EOSError	loadDataSet(Char* filename);
	EOSError	exportDataSetAsText(Char* filename, Char* outprefix);
	EOSError	exportDataSetAsBIN(Char* filename);
};

#endif /* __SPRITEANIMDATASET_H__ */
