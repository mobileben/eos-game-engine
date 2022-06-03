/******************************************************************************
 *
 * File: SpriteDataSet.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Parts for encapsulating and loading Sprite Data sets
 * 
 *****************************************************************************/

#include "toolkit/SpriteDataSet.h"
#include "SpriteSet.h"
#include "Endian.h"
#include "File.h"

SpriteDataSet::SpriteDataSet() : _exportNames(NULL), _exportNamesOffsets(0), _exportNamesSize(0)
{
	_textureAtlasID = 0;

	_version = 0;
	_comments = false;
}

SpriteDataSet::~SpriteDataSet()
{
	if (_exportNames)
	{
		delete _exportNames;
		_exportNames = NULL;
	}

	if (_exportNamesOffsets)
	{
		delete _exportNamesOffsets;
		_exportNamesOffsets = NULL;
	}
}

EOSError SpriteDataSet::addExportName(Char* name, Char** nameList, Uint32& size, Uint32& offset)
{
	Char*		newlist;
	EOSError 	error = EOSErrorNone;
	size_t		strsize;
	Boolean		found = false;
	size_t		curr = 0;

	//	Try and see if it exists already
	if (nameList)
	{
		while (curr < size)
		{
			strsize = strlen(&(*nameList)[curr]);

			if (!strcmp(name, &(*nameList)[curr]))
			{
				offset = (Uint32) curr;
				found = true;
				break;
			}

			curr += strsize + 1;
		}
	}

	if (found == false)
	{
		strsize = strlen(name);

		newlist = new Char[size + strsize + 1];

		if (newlist)
		{
			if ((*nameList))
			{
				memcpy(newlist, (*nameList), size);
				delete (*nameList);
			}

			strcpy(&newlist[size], name);
			(*nameList) = newlist;
			offset = size;
			size += (Uint32) strsize + 1;
		}
		else
			error = EOSErrorNoMemory;
	}

	return error;
}

#define EPSILON	1.0e-7

inline Boolean fpValuesEqual(Float32 v0, Float32 v1)
{
	if (fabs(v0 - v1) <= EPSILON)
		return true;
	else
		return false;
}

Boolean SpriteDataSet::isSkewedMatrix(SpriteTransform& xform, SpriteMatrix& matrix)
{
	Float32	sinval, cosval;
	Float32	a, b, c, d;

	if (matrix.a == matrix.d && matrix.b == - matrix.c)
		return false;
	else
	{
		//	Another more careful check
		sinval = sin((xform.rotateZ * M_PI) / 180.0);
		cosval = cos((xform.rotateZ * M_PI) / 180.0);

		a = xform.scaleX * cosval;
		b = xform.scaleX * sinval;
		c = xform.scaleY * -sinval;
		d = xform.scaleY * cosval;

		if (fpValuesEqual(matrix.a, a))
		{
			if (fpValuesEqual(matrix.b, b))
			{
				if (fpValuesEqual(matrix.c, c))
				{
					if (fpValuesEqual(matrix.d, d))
					{
						return false;
					}
				}
			}
		}

		return true;
	}
}

Uint32 SpriteDataSet::addSpriteTranslate(Point2D& xlate, Uint32& control)
{
	vector<Point2D>::iterator	pt;
	Uint32						index = 0;
	Boolean						found = false;

	pt = _spriteTranslates.begin();

	while (pt != _spriteTranslates.end())
	{
		found = true;

		if (pt->x != xlate.x)
		{
			found = false;
		}
		else if (pt->y != xlate.y)
		{
			found = false;
		}

		if (found)
			break;

		index++;
		pt++;
	}

	if (!found)
	{
		index = (Uint32) _spriteTranslates.size();

		_spriteTranslates.push_back(xlate);
	}

	return index;
}

Uint32 SpriteDataSet::addSpriteTransform(SpriteTransform& xform, Uint32& control)
{
	vector<SpriteTransform>::iterator	transform;
	Uint32								index = 0;
	Boolean								found = false;

	transform = _spriteTransforms.begin();

	while (transform != _spriteTransforms.end())
	{
		found = true;

		if (transform->scaleX != xform.scaleX)
		{
			found = false;
		}
		else if (transform->scaleY != xform.scaleY)
		{
			found = false;
		}
		else if (transform->rotateZ != xform.rotateZ)
		{
			found = false;
		}

		if (found)
			break;

		index++;
		transform++;
	}

	if (!found)
	{
		index = (Uint32) _spriteTransforms.size();

		_spriteTransforms.push_back(xform);
	}

	return index;
}

Uint32 SpriteDataSet::addSpriteMatrix(SpriteTransform& xform, SpriteMatrix& mat, Uint32& control)
{
	vector<SpriteMatrix>::iterator	matrix;
	Uint32								index = 0;
	Boolean								found = false;

	if (isSkewedMatrix(xform, mat))
	{
		matrix = _spriteMatrices.begin();

		while (matrix != _spriteMatrices.end())
		{
			found = true;

			if (matrix->a != mat.a)
			{
				found = false;
			}
			else if (matrix->b != mat.b)
			{
				found = false;
			}
			else if (matrix->c != mat.c)
			{
				found = false;
			}
			else if (matrix->d != mat.d)
			{
				found = false;
			}
			if (found)
				break;

			index++;
			matrix++;
		}

		if (!found)
		{
			index = (Uint32) _spriteMatrices.size();

			_spriteMatrices.push_back(mat);
		}

		control &= ~(CONTROL_TRANSFORM_SCALE | CONTROL_TRANSFORM_ROTATE | CONTROL_TRANSFORM_IDENTITY);
		control |= CONTROL_TRANSFORM_MATRIX;
	}
	else
		index = 0xFFFFFFFF;

	return index;
}

Uint32 SpriteDataSet::addSpriteColorOp(SpriteColorOp& color, Uint32& control)
{
	vector<SpriteColorOp>::iterator		op;
	Uint32								index = 0;
	Boolean								found = false;

	if (!(control & CONTROL_TRANSFORM_COLOR_MULTIPLIER) && !(control & CONTROL_TRANSFORM_COLOR_OFFSET))
		return 0xFFFFFFFF;

	op = _spriteColorOps.begin();

	while (op != _spriteColorOps.end())
	{
		found = true;

		if (op->redMultiplier != color.redMultiplier)
		{
			found = false;
		}
		else if (op->greenMultiplier != color.greenMultiplier)
		{
			found = false;
		}
		else if (op->blueMultiplier != color.blueMultiplier)
		{
			found = false;
		}
		else if (op->alphaMultiplier != color.alphaMultiplier)
		{
			found = false;
		}
		else if (op->redOffset != color.redOffset)
		{
			found = false;
		}
		else if (op->greenOffset != color.greenOffset)
		{
			found = false;
		}
		else if (op->blueOffset != color.blueOffset)
		{
			found = false;
		}
		else if (op->alphaOffset != color.alphaOffset)
		{
			found = false;
		}

		if (found)
			break;

		index++;
		op++;
	}

	if (!found)
	{
		index = (Uint32) _spriteColorOps.size();

		_spriteColorOps.push_back(color);
	}

	return index;
}

Uint32 SpriteDataSet::getNumMasterSprites(void)
{
	return (Uint32) _masterSprites.size();
}

vector<MasterSpriteFlashRAW>::iterator SpriteDataSet::getMasterSpriteAt(Uint32 index)
{
	return _masterSprites.begin() + index;
}

Uint32 SpriteDataSet::getNumNormalSprites(void)
{
	return (Uint32) _normalSprites.size();
}

vector<SpriteFlashRAW>::iterator SpriteDataSet::getNormalSpriteAt(Uint32 index)
{
	return _normalSprites.begin() + index;
}

Uint32 SpriteDataSet::getNumSpriteMappings(void)
{
	return (Uint32) _spriteMappings.size();
}

vector<SpriteMapping>::iterator SpriteDataSet::getSpriteMappingAt(Uint32 index)
{
	return _spriteMappings.begin() + index;
}

Uint32 SpriteDataSet::getNumTextureRefNames(void)
{
	return (Uint32) _textureRefNames.size();
}

vector<string>::iterator SpriteDataSet::getTextureRefNameAt(Uint32 index)
{
	return _textureRefNames.begin() + index;
}

void SpriteDataSet::swapHeaderIfNecessary(FlashSpriteDataHeader* header)
{
	if (header->endian == 0x04030201)
	{
		_endian.switchEndian();
	}

	header->endian = _endian.swapUint32(header->endian);
	header->version = _endian.swapUint32(header->version);
	header->numMasterSprites = _endian.swapUint32(header->numMasterSprites);
	header->masterSprites = _endian.swapUint32(header->masterSprites);
	header->numNormalSprites = _endian.swapUint32(header->numNormalSprites);
	header->normalSprites = _endian.swapUint32(header->normalSprites);
	header->numSpriteMappings = _endian.swapUint32(header->numSpriteMappings);
	header->spriteMappings = _endian.swapUint32(header->spriteMappings);
	header->numNameOffsets = _endian.swapUint32(header->numNameOffsets);
	header->nameOffsets = _endian.swapUint32(header->nameOffsets);
	header->nameData = _endian.swapUint32(header->nameData);
}

void SpriteDataSet::swapMasterSpriteIfNecessary(MasterSpriteFlashRAW* sprite)
{
	//	swapHeaderIfNecessary sets the endian control
	sprite->control = _endian.swapUint32(sprite->control);
	sprite->x = _endian.swapFloat32(sprite->x);
	sprite->y = _endian.swapFloat32(sprite->y);
	sprite->scaleX = _endian.swapFloat32(sprite->scaleX);
	sprite->scaleY = _endian.swapFloat32(sprite->scaleY);
	sprite->rotateZ = _endian.swapFloat32(sprite->rotateZ);

	sprite->mat_a = _endian.swapFloat32(sprite->mat_a);
	sprite->mat_b = _endian.swapFloat32(sprite->mat_b);
	sprite->mat_c = _endian.swapFloat32(sprite->mat_c);
	sprite->mat_d = _endian.swapFloat32(sprite->mat_d);
	sprite->mat_tx = _endian.swapFloat32(sprite->mat_tx);
	sprite->mat_ty = _endian.swapFloat32(sprite->mat_ty);

	sprite->redMultiplier = _endian.swapFloat32(sprite->redMultiplier);
	sprite->greenMultiplier = _endian.swapFloat32(sprite->greenMultiplier);
	sprite->blueMultiplier = _endian.swapFloat32(sprite->blueMultiplier);
	sprite->alphaMultiplier = _endian.swapFloat32(sprite->alphaMultiplier);
	sprite->redOffset = _endian.swapUint32(sprite->redOffset);
	sprite->greenOffset = _endian.swapUint32(sprite->greenOffset);
	sprite->blueOffset = _endian.swapUint32(sprite->blueOffset);
	sprite->alphaOffset = _endian.swapUint32(sprite->alphaOffset);
	sprite->width = _endian.swapFloat32(sprite->width);
	sprite->height = _endian.swapFloat32(sprite->height);
	sprite->numChildren = _endian.swapUint32(sprite->numChildren);
	sprite->firstChildIndex = _endian.swapUint32(sprite->firstChildIndex);
}

void SpriteDataSet::swapNormalSpriteIfNecessary(SpriteFlashRAW* sprite)
{
	//	swapHeaderIfNecessary sets the endian control
	sprite->control = _endian.swapUint32(sprite->control);
	sprite->x = _endian.swapFloat32(sprite->x);
	sprite->y = _endian.swapFloat32(sprite->y);
	sprite->scaleX = _endian.swapFloat32(sprite->scaleX);
	sprite->scaleY = _endian.swapFloat32(sprite->scaleY);
	sprite->rotateZ = _endian.swapFloat32(sprite->rotateZ);

	sprite->mat_a = _endian.swapFloat32(sprite->mat_a);
	sprite->mat_b = _endian.swapFloat32(sprite->mat_b);
	sprite->mat_c = _endian.swapFloat32(sprite->mat_c);
	sprite->mat_d = _endian.swapFloat32(sprite->mat_d);
	sprite->mat_tx = _endian.swapFloat32(sprite->mat_tx);
	sprite->mat_ty = _endian.swapFloat32(sprite->mat_ty);

	sprite->redMultiplier = _endian.swapFloat32(sprite->redMultiplier);
	sprite->greenMultiplier = _endian.swapFloat32(sprite->greenMultiplier);
	sprite->blueMultiplier = _endian.swapFloat32(sprite->blueMultiplier);
	sprite->alphaMultiplier = _endian.swapFloat32(sprite->alphaMultiplier);
	sprite->redOffset = _endian.swapUint32(sprite->redOffset);
	sprite->greenOffset = _endian.swapUint32(sprite->greenOffset);
	sprite->blueOffset = _endian.swapUint32(sprite->blueOffset);
	sprite->alphaOffset = _endian.swapUint32(sprite->alphaOffset);
	sprite->numChildren = _endian.swapUint32(sprite->numChildren);
	sprite->firstChildIndex = _endian.swapUint32(sprite->firstChildIndex);
}

void SpriteDataSet::swapSpriteMappingIfNecessary(SpriteMapping* mapping)
{
	//	swapHeaderIfNecessary sets the endian control
	mapping->mapControl = _endian.swapUint32(mapping->mapControl);
	mapping->offset = _endian.swapUint32(mapping->offset);
}

void SpriteDataSet::swapNameOffsetIfNecessary(Uint32* offset)
{
	//	swapHeaderIfNecessary sets the endian control
	*offset = _endian.swapUint32(*offset);
}

EOSError SpriteDataSet::loadMasterSprites(Uint32 num, Uint8* buffer,Uint32 size)
{
	EOSError			error = EOSErrorNone;
	Uint32				i;
	MasterSpriteFlashRAW*	masters = (MasterSpriteFlashRAW*) buffer;

	for (i=0;i<num;i++)
	{
		swapMasterSpriteIfNecessary(&masters[i]);

		_masterSprites.push_back(masters[i]);
	}

	return error;
}

EOSError SpriteDataSet::loadNormalSprites(Uint32 num, Uint8* buffer,Uint32 size)
{
	EOSError			error = EOSErrorNone;
	Uint32				i;
	SpriteFlashRAW*			sprites = (SpriteFlashRAW*) buffer;

	for (i=0;i<num;i++)
	{
		swapNormalSpriteIfNecessary(&sprites[i]);

		_normalSprites.push_back(sprites[i]);
	}

	return error;
}

EOSError SpriteDataSet::loadSpriteMappings(Uint32 num, Uint8* buffer,Uint32 size)
{
	EOSError			error = EOSErrorNone;
	Uint32				i;
	SpriteMapping*		mappings = (SpriteMapping*) buffer;

	for (i=0;i<num;i++)
	{
		swapSpriteMappingIfNecessary(&mappings[i]);

		_spriteMappings.push_back(mappings[i]);
	}

	return error;
}

EOSError SpriteDataSet::loadNames(Uint32 num, Uint8* buffer,Uint32 size, Uint8* databuffer,Uint32 datasize)
{
	EOSError			error = EOSErrorNone;
	Uint32				i;
	Uint32*				offsets = (Uint32*) buffer;
	string				name;
	Uint32				end;

	for (i=0;i<num;i++)
	{
		swapNameOffsetIfNecessary(&offsets[i]);
	}

	for (i=0;i<num;i++)
	{
		if (i < (num - 1))
			end = offsets[i+1];
		else
			end = datasize;

		name = string((const char*) (databuffer+offsets[i]), end - offsets[i]);

		_textureRefNames.push_back(name);
	}

	return error;
}

EOSError SpriteDataSet::parseSpriteData(Uint8* data, Uint32 size)
{
	EOSError				error = EOSErrorNone;
	FlashSpriteDataHeader*	header = (FlashSpriteDataHeader*) data;

	swapHeaderIfNecessary(header);

	error = loadMasterSprites(header->numMasterSprites, data + header->masterSprites, header->normalSprites - header->masterSprites);

	if (error == EOSErrorNone)
		error = loadNormalSprites(header->numNormalSprites, data + header->normalSprites, header->spriteMappings - header->normalSprites);

	if (error == EOSErrorNone)
		error = loadSpriteMappings(header->numSpriteMappings, data + header->spriteMappings, header->nameOffsets - header->spriteMappings);

	if (error == EOSErrorNone)
		error = loadNames(header->numNameOffsets, data + header->nameOffsets, header->nameData - header->nameOffsets, data + header->nameData, size - header->nameData);

	return error;
}

EOSError SpriteDataSet::loadDataSet(Char* filename)
{
	EOSError		error = EOSErrorNone;
	FileDescriptor	fd;
	File			file;
	Uint8*			buffer;
	Uint32			size;
	wstring			filenameUTF16(filename, filename + strlen(filename));

	if (filenameUTF16.c_str())
	{
		fd.setFilename((UTF16*) filenameUTF16.c_str());
		fd.setFileAccessType(FileAccessTypeReadOnly);

		file.setFileDescriptor(fd);

		error = file.open();

		if (error == EOSErrorNone)
		{
			size = file.length();

			buffer = new Uint8[size];

			if (buffer)
			{
				error = file.readUint8(buffer, size);

				if (error == EOSErrorNone)
					error = parseSpriteData(buffer, size);

				delete [] buffer;
			}
			else
			{
				error = EOSErrorNoMemory;
				fprintf(stderr, "Memory allocation error.\n");
			}

			file.close();
		}
		else
		{
			error = EOSErrorResourceNotOpened;
			fprintf(stderr, "Could not open file %s\n", filename);
		}
	}
	else
	{
		error = EOSErrorNoMemory;
		fprintf(stderr, "Memory allocation error.\n");
	}

	return error;
}

EOSError SpriteDataSet::exportDataSetAsText(Char* filename, Char* outprefix)
{
	EOSError							error = EOSErrorNone;
	vector<MasterSpriteFlashRAW>::iterator	master;
	vector<SpriteFlashRAW>::iterator			sprite;
	vector<SpriteMapping>::iterator		mapping;
	FileDescriptor						fd;
	File								file;
	char								str[256];
	wstring								filenameUTF16(filename, filename + strlen(filename));
	Uint32								cnt;

	fd.setFilename((UTF16*) filenameUTF16.c_str());
	fd.setFileAccessType(FileAccessTypeWriteOnly);

	file.setFileDescriptor(fd);

	error = file.open();

	if (error == EOSErrorNone)
	{
		sprintf_s(str, sizeof(str), "const MasterSpriteFlashRAW\t%s_master_sprites[%d] =\n{\n", outprefix, _masterSprites.size()); 
		file.writeChar(str, (Uint32) strlen(str));

		//	For now, do a quickie 'C' text output of our data
		master = _masterSprites.begin();
		cnt = 0;

		while (master != _masterSprites.end())
		{
			if (_comments)
				sprintf_s(str, sizeof(str), "\t{\t// %d\n", cnt);
			else
				sprintf_s(str, sizeof(str), "\t{\n");

			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t0x%.8X,", master->control);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// control");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", master->x);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// x");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", master->y);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// y");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", master->scaleX);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// scaleX");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", master->scaleY);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// scaleY");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", master->rotateZ);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// rotateZ");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", master->redMultiplier);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// redMultiplier");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", master->greenMultiplier);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// greenMultiplier");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", master->blueMultiplier);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// blueMultiplier");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", master->alphaMultiplier);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// alphaMultiplier");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", (Float32) master->redOffset / 255.0);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// redOffset");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", (Float32) master->greenOffset / 255.0);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// greenOffset");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", (Float32) master->blueOffset / 255.0);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// blueOffset");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", (Float32) master->alphaOffset / 255.0);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// alphaOffset");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", master->width);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// width");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", master->height);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// height");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%d,", master->numChildren);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// numChildren");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%d,", master->firstChildIndex);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// firstChildIndex");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t},\n");
			file.writeChar(str, (Uint32) strlen(str));

			master++;
			cnt++;
		}

		sprintf_s(str, sizeof(str), "};\n");
		file.writeChar(str, (Uint32) strlen(str));

		sprintf_s(str, sizeof(str), "\nconst SpriteFlashRAW\t%s_sprites[%d] =\n{\n", outprefix, _normalSprites.size()); 
		file.writeChar(str, (Uint32) strlen(str));

		//	For now, do a quickie 'C' text output of our data
		sprite = _normalSprites.begin();
		cnt = 0;

		while (sprite != _normalSprites.end())
		{
			if (_comments)
				sprintf_s(str, sizeof(str), "\t{\t// %d\n", cnt);
			else
				sprintf_s(str, sizeof(str), "\t{\n");

			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t0x%.8X,", sprite->control);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// control");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", sprite->x);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// x");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", sprite->y);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// y");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", sprite->scaleX);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// scaleX");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", sprite->scaleY);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// scaleY");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", sprite->rotateZ);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// rotateZ");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", sprite->redMultiplier);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// redMultiplier");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", sprite->greenMultiplier);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// greenMultiplier");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", sprite->blueMultiplier);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// blueMultiplier");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", sprite->alphaMultiplier);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// alphaMultiplier");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", (Float32) sprite->redOffset / 255.0);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// redOffset");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", (Float32) sprite->greenOffset / 255.0);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// greenOffset");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", (Float32) sprite->blueOffset / 255.0);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// blueOffset");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%.10f,", (Float32) sprite->alphaOffset / 255.0);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// alphaOffset");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%d,", sprite->numChildren);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// numChildren");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%d,", sprite->firstChildIndex);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// firstChildIndex");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t},\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprite++;
			cnt++;
		}

		sprintf_s(str, sizeof(str), "};\n");
		file.writeChar(str, (Uint32) strlen(str));

		sprintf_s(str, sizeof(str), "\nconst SpriteMapping\t%s_sprite_mappings[%d] =\n{\n", outprefix, _spriteMappings.size()); 
		file.writeChar(str, (Uint32) strlen(str));

		//	For now, do a quickie 'C' text output of our data
		mapping = _spriteMappings.begin();
		cnt = 0;

		while (mapping != _spriteMappings.end())
		{
			if (_comments)
				sprintf_s(str, sizeof(str), "\t{\t// %d\n", cnt);
			else
				sprintf_s(str, sizeof(str), "\t{\n");

			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t0x%.8X,", mapping->mapControl);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// mapControl");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%d,", mapping->offset);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// offset");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));


			sprintf_s(str, sizeof(str), "\t},\n");
			file.writeChar(str, (Uint32) strlen(str));

			mapping++;
			cnt++;
		}

		sprintf_s(str, sizeof(str), "};\n");
		file.writeChar(str, (Uint32) strlen(str));

		file.close();
	}
	else
	{
		error = EOSErrorResourceOpen;
		fprintf(stderr, "Could not open file %s\n", filename);
	}

	return error;
}

EOSError SpriteDataSet::exportDataSetAsBIN(Char* filename)
{
	EOSError							error = EOSErrorNone;
	vector<MasterSpriteFlashRAW>::iterator	master;
	vector<SpriteFlashRAW>::iterator			sprite;
	vector<SpriteMapping>::iterator		mapping;
	vector<Point2D>::iterator			pt;
	vector<SpriteTransform>::iterator	transform;
	vector<SpriteMatrix>::iterator		matrix;
	vector<SpriteColorOp>::iterator		colorop;
	FileDescriptor						fd;
	File								file;
	wstring								filenameUTF16(filename, filename + strlen(filename));
	Uint32								cnt;
	SpriteSetHeader						header;

	memset(&header, 0, sizeof(SpriteSetHeader));

	fd.setFilename((UTF16*) filenameUTF16.c_str());
	fd.setFileAccessType(FileAccessTypeWriteOnly);

	file.setFileDescriptor(fd);

	error = file.open();

	if (error == EOSErrorNone)
	{
		Uint32*					offsets = NULL;
		Uint32 					nameOffset = 0;
		Uint32					dataOffset = 0;
		MasterSpriteRAW*		masters;
		SpriteRAW*				sprites;
		char					tmpName[EOS_ASSET_NAME_MAX_LENGTH] = "TEST";
		Sint32					value;
		Point2D					xlate;
		SpriteTransform			xform;
		SpriteMatrix			mat;
		SpriteColorOp			color;
		Point2D*				xlateList = NULL;
		SpriteTransform*		xformList = NULL;
		SpriteMatrix*			matList = NULL;
		SpriteColorOp*			colorList = NULL;

		header.endian = 0x01020304;
		header.version = _version;

		header.textureAtlasID = _textureAtlasID;

		dataOffset = sizeof(SpriteSetHeader);
		masters = new MasterSpriteRAW[_masterSprites.size()];
		sprites = new SpriteRAW[_normalSprites.size()];
		offsets = new Uint32[_masterSprites.size()];

		header.numSprites = (Uint32) _masterSprites.size();
		header.sprites = dataOffset;

		master = _masterSprites.begin();
		cnt = 0;

		while (master != _masterSprites.end())
		{
			masters[cnt].control = master->control;

			xlate.x = master->x;
			xlate.y = master->y;

			xform.scaleX = master->scaleX;
			xform.scaleY = master->scaleY;
			xform.rotateZ = master->rotateZ;

			mat.a = master->mat_a;
			mat.b = master->mat_b;
			mat.c = master->mat_c;
			mat.d = master->mat_d;

			masters[cnt].spriteTranslate = addSpriteTranslate(xlate, masters[cnt].control);

			if (isSkewedMatrix(xform, mat) == false)
				masters[cnt].spriteTransform = addSpriteTransform(xform, masters[cnt].control);
			else
				masters[cnt].spriteTransform = addSpriteMatrix(xform, mat, masters[cnt].control);

			color.redMultiplier = master->redMultiplier;
			color.greenMultiplier = master->greenMultiplier;
			color.blueMultiplier = master->blueMultiplier;
			color.alphaMultiplier = master->alphaMultiplier;

			value = (Sint32) master->redOffset;

			color.redOffset = (Float32) value / 255.0;

			value = (Sint32) master->greenOffset;
			color.greenOffset = (Float32) value / 255.0;

			value = (Sint32) master->blueOffset;
			color.blueOffset = (Float32) value / 255.0;

			value = (Sint32) master->alphaOffset;
			color.alphaOffset = (Float32) value / 255.0;

			masters[cnt].spriteColorOp = addSpriteColorOp(color, master->control);

			masters[cnt].width = master->width;
			masters[cnt].height = master->height;
			masters[cnt].numChildren = master->numChildren;
			masters[cnt].firstChildIndex = master->firstChildIndex;

			dataOffset += sizeof(MasterSpriteRAW);

			master++;
			cnt++;
		}

		header.numSubSprites = (Uint32) _normalSprites.size();
		header.subSprites = dataOffset;

		sprite = _normalSprites.begin();
		cnt = 0;

		while (sprite != _normalSprites.end())
		{
			sprites[cnt].control = sprite->control;

			xlate.x = sprite->x;
			xlate.y = sprite->y;

			xform.scaleX = sprite->scaleX;
			xform.scaleY = sprite->scaleY;
			xform.rotateZ = sprite->rotateZ;

			mat.a = sprite->mat_a;
			mat.b = sprite->mat_b;
			mat.c = sprite->mat_c;
			mat.d = sprite->mat_d;

			sprites[cnt].spriteTranslate = addSpriteTranslate(xlate, sprites[cnt].control);

			if (isSkewedMatrix(xform, mat) == false)
				sprites[cnt].spriteTransform = addSpriteTransform(xform, sprites[cnt].control);
			else
				sprites[cnt].spriteTransform = addSpriteMatrix(xform, mat, sprites[cnt].control);

			color.redMultiplier = sprite->redMultiplier;
			color.greenMultiplier = sprite->greenMultiplier;
			color.blueMultiplier = sprite->blueMultiplier;
			color.alphaMultiplier = sprite->alphaMultiplier;

			value = (Sint32) sprite->redOffset;
			color.redOffset = (Float32) value / 255.0;

			value = (Sint32) sprite->greenOffset;
			color.greenOffset = (Float32) value / 255.0;

			value = (Sint32) sprite->blueOffset;
			color.blueOffset = (Float32) value / 255.0;

			value = (Sint32) sprite->alphaOffset;
			color.alphaOffset = (Float32) value / 255.0;

			sprites[cnt].spriteColorOp = addSpriteColorOp(color, sprite->control);

			sprites[cnt].numChildren = sprite->numChildren;
			sprites[cnt].firstChildIndex = sprite->firstChildIndex;

			dataOffset += sizeof(SpriteRAW);

			sprite++;
			cnt++;
		}

		header.numSpriteMappings = (Uint32) _spriteMappings.size();
		header.spriteMappings = dataOffset;

		mapping = _spriteMappings.begin();
		cnt = 0;

		while (mapping != _spriteMappings.end())
		{
			dataOffset += sizeof(SpriteMappingRAW);
			mapping++;
			cnt++;
		}

		if (_spriteTranslates.size() > 0)
		{
			xlateList = new Point2D[_spriteTranslates.size()];
	
			if (xlateList)
			{
				header.numSpriteTranslates = (Uint32) _spriteTranslates.size();
				header.spriteTranslates = dataOffset;
		
				pt = _spriteTranslates.begin();
				cnt = 0;
		
				while (pt != _spriteTranslates.end())
				{
					xlateList[cnt] = *pt;

					dataOffset += sizeof(Point2D);
					pt++;
					cnt++;
				}
			}
			else
				error = EOSErrorNoMemory;
		}

		if (_spriteTransforms.size() > 0)
		{
			xformList = new SpriteTransform[_spriteTransforms.size()];

			if (xformList)
			{
				header.numSpriteTransforms = (Uint32) _spriteTransforms.size();
				header.spriteTransforms = dataOffset;

				transform = _spriteTransforms.begin();
				cnt = 0;

				while (transform != _spriteTransforms.end())
				{
					xformList[cnt] = *transform;

					dataOffset += sizeof(SpriteTransform);
					transform++;
					cnt++;
				}
			}
			else
				error = EOSErrorNoMemory;
		}

		if (_spriteMatrices.size() > 0)
		{
			matList = new SpriteMatrix[_spriteMatrices.size()];
	
			if (matList)
			{
				header.numSpriteMatrices = (Uint32) _spriteMatrices.size();
				header.spriteMatrices = dataOffset;
		
				matrix = _spriteMatrices.begin();
				cnt = 0;
		
				while (matrix != _spriteMatrices.end())
				{
					matList[cnt] = *matrix;

					dataOffset += sizeof(SpriteMatrix);
					matrix++;
					cnt++;
				}
			}
			else
				error = EOSErrorNoMemory;
		}

		if (_spriteColorOps.size() > 0)
		{
			colorList = new SpriteColorOp[_spriteColorOps.size()];
	
			if (colorList)
			{
				header.numSpriteColorOps = (Uint32) _spriteColorOps.size();
				header.spriteColorOps = dataOffset;
		
				colorop = _spriteColorOps.begin();
				cnt = 0;
		
				while (colorop != _spriteColorOps.end())
				{
					colorList[cnt] = *colorop;

					dataOffset += sizeof(SpriteColorOp);
					colorop++;
					cnt++;
				}
			}
			else
				error = EOSErrorNone;
		}

		//	For now, no names

		//	Now write out all the data
		file.writeUint8((Uint8*) &header, (Uint32) sizeof(SpriteSetHeader));
		file.writeUint8((Uint8*) masters, (Uint32) sizeof(MasterSpriteRAW) * _masterSprites.size());
		file.writeUint8((Uint8*) sprites, (Uint32) sizeof(SpriteRAW) * _normalSprites.size());

		mapping = _spriteMappings.begin();

		while (mapping != _spriteMappings.end())
		{
			file.writeUint32(mapping->mapControl);
			file.writeUint32(mapping->offset);

			mapping++;
		}

		file.writeUint8((Uint8*) xlateList, (Uint32) sizeof(Point2D) * _spriteTranslates.size());
		file.writeUint8((Uint8*) xformList, (Uint32) sizeof(SpriteTransform) * _spriteTransforms.size());
		file.writeUint8((Uint8*) matList, (Uint32) sizeof(SpriteMatrix) * _spriteMatrices.size());
		file.writeUint8((Uint8*) colorList, (Uint32) sizeof(SpriteColorOp) * _spriteColorOps.size());

		file.close();

		if (masters)
			delete masters;

		if (sprites)
			delete sprites;

		if (offsets)
			delete offsets;

		if (xformList)
			delete xformList;

		if (matList)
			delete matList;

		if (colorList)
			delete colorList;
	}
	else
	{
		error = EOSErrorResourceOpen;
		fprintf(stderr, "Could not open file %s\n", filename);
	}

	return error;
}

