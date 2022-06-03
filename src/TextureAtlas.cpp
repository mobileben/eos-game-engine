/******************************************************************************
 *
 * File: TextureAtlas.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Atlas object
 * 
 *****************************************************************************/

#include "TextureAtlas.h"
#include "Graphics.h"
#include "Endian.h"
#include "File.h"
#include "App.h"

TextureAtlas::TextureAtlas() : _used(false), _id(0xFFFFFFFF), _name(NULL), _numSubTextures(0), _subTextures(NULL), _verticeList(NULL), _uvList(NULL), _nameData(NULL), _nameList(NULL), _memUsage(0), _textureAtlasMgr(NULL)
{
}

TextureAtlas::~TextureAtlas()
{
	if (_name)
	{
		delete _name;
		_name = NULL;
	}

	_textureAtlasMgr = NULL;

	destroy();
}

EOSError TextureAtlas::create(ObjectID objid, const Char* name, Texture* tex, Uint32 numSubTex, SpriteTextureRefRAW* subTex, Uint32 numVerts, Point2D* verts, Uint32 numUVs, Point2D* uvs)
{
	EOSError	error = EOSErrorNone;
	Uint32		i, j;

	AssertWDesc(tex != NULL, "TextureAtlas::create() NULL tex.");
	AssertWDesc(subTex != NULL, "TextureAtlas::create() NULL subTex.");
	AssertWDesc(verts != NULL, "TextureAtlas::create() NULL verts.");
	AssertWDesc(uvs != NULL, "TextureAtlas::create() NULL uvs.");

	AssertWDesc(numSubTex > 0, "TextureAtlas::create() numSubTex must be > 0.");
	AssertWDesc(numVerts > 0, "TextureAtlas::create() numVerts must be > 0.");
	AssertWDesc(numUVs > 0, "TextureAtlas::create() numUVs must be > 0.");

	_id = objid;
	setName(name);

	destroy();

	_subTextures = new TextureAtlasSubTexture[numSubTex];
	_numSubTextures = numSubTex;

	_verticeList = new Point2D[numVerts];

	_uvList = new Point2D[numUVs];

	AssertWDesc(_subTextures != NULL, "TextureAtlas::create() NULL _subTextures.");
	AssertWDesc(_verticeList != NULL, "TextureAtlas::create() NULL _verticeList.");
	AssertWDesc(_uvList != NULL, "TextureAtlas::create() NULL _uvList.");

	if (_subTextures && _verticeList && _uvList)
	{
		_memUsage = sizeof(TextureAtlasSubTexture) * numSubTex + sizeof(Point2D) * numVerts + sizeof(Point2D) * numUVs;

		memcpy(_verticeList, verts, sizeof(Point2D) * numVerts);
		memcpy(_uvList, uvs, sizeof(Point2D) * numUVs);

		for (i=0;i<numSubTex;i++)
		{
			_subTextures[i].atlas = this;
			_subTextures[i].width = subTex[i].width;
			_subTextures[i].height = subTex[i].height;

			for (j=0;j<4;j++)
			{
#ifdef _USE_OPENGL_ES_1_1

				_subTextures[i].data.PT2D_DIRECT.vertices[j] = _verticeList[subTex[i].vertices[j]];
				_subTextures[i].data.PT2D_DIRECT.uvs[j] = _uvList[subTex[i].uvs[j]];

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

				_subTextures[i].data.PT2D_PTR.vertices[j] = &_verticeList[subTex[i].vertices[j]];
				_subTextures[i].data.PT2D_PTR.uvs[j] = &_uvList[subTex[i].uvs[j]];

#else

				_subTextures[i].data.PT2D_DIRECT.vertices[j] = _verticeList[subTex[i].vertices[j]];
				_subTextures[i].data.PT2D_DIRECT.uvs[j] = _uvList[subTex[i].uvs[j]];

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_! */
			}
		}

		//	For now, get the texture from
		_textureState._textureObj = tex;
	}
	else
	{
		destroy();
		error = EOSErrorNoMemory;
	}

	if (error == EOSErrorNone)
	{
		if (_textureAtlasMgr)
		{
			_textureAtlasMgr->updateUsage();
		}
	}

	AssertWDesc(error == EOSErrorNone, "TextureAtlas::create() error");

	return error;
}

EOSError TextureAtlas::create(ObjectID objid, const Char* name, Texture* tex, Uint8* data, Uint32 datasize)
{
	EOSError				error = EOSErrorNone;
	TextureAtlasHeader*		header = (TextureAtlasHeader*) data;
	SpriteTextureRefRAW*	texrefraw;
	Point2D*				srcpts;
	Endian					endian;
	Uint32					i, j;
	Char*					srcnames;
	Uint32*					offsets;

	AssertWDesc(data != NULL, "TextureAtlas::create() NULL data.");

	if (header->endian == 0x04030201)
		endian.switchEndian();

	_id = objid;

	setName(name);

	destroy();

	//	Check to see if we have an embedded texture

	//	Since they are needed in unpacking, do vertice and uvs first

	//	Now unpack the vertice data
	_verticeList = new Point2D[endian.swapUint32(header->numVertices)];

	AssertWDesc(_verticeList != NULL, "TextureAtlas::create() Memory failed.");

	if (_verticeList)
	{
		_memUsage += sizeof(Point2D) * endian.swapUint32(header->numVertices);

		srcpts = (Point2D*) (data + endian.swapUint32(header->vertices));

		for (i=0;i<endian.swapUint32(header->numVertices);i++)
		{
			_verticeList[i].x = endian.swapFloat32(srcpts[i].x);
			_verticeList[i].y = endian.swapFloat32(srcpts[i].y);
		}
	}
	else
		error = EOSErrorNoMemory;

	//	Now unpack the uv data
	_uvList = new Point2D[endian.swapUint32(header->numUVs)];

	AssertWDesc(_uvList != NULL, "TextureAtlas::create() Memory failed.");

	if (_uvList)
	{
		_memUsage += sizeof(Point2D) * endian.swapUint32(header->numUVs);

		srcpts = (Point2D*) (data + endian.swapUint32(header->uvs));

		for (i=0;i<endian.swapUint32(header->numUVs);i++)
		{
			_uvList[i].x = endian.swapFloat32(srcpts[i].x);
			_uvList[i].y = endian.swapFloat32(srcpts[i].y);
		}
	}
	else
		error = EOSErrorNoMemory;

	//	Now unpack the names
	_nameData = new Char[datasize - endian.swapUint32(header->nameData)];
	_nameList = new Char*[endian.swapUint32(header->numNameOffsets)];

	AssertWDesc(_nameData != NULL, "TextureAtlas::create() Memory failed.");

	if (_nameData && _nameList)
	{
		_memUsage += sizeof(Char) * (datasize - endian.swapUint32(header->nameData)) + sizeof(Char*) * endian.swapUint32(header->numNameOffsets);

		srcnames = (Char*) (data + endian.swapUint32(header->nameData));
		offsets = (Uint32*) (data + endian.swapUint32(header->nameOffsets));

		memcpy(_nameData, srcnames, datasize - endian.swapUint32(header->nameData));

		for (i=0;i<endian.swapUint32(header->numNameOffsets);i++)
			_nameList[i] = &_nameData[endian.swapUint32(offsets[i])];
	}
	else
		error = EOSErrorNoMemory;

	//	Now unpack the subtexture data
	if (error == EOSErrorNone)
	{
		_numSubTextures = endian.swapUint32(header->numSubTextures);
		_subTextures = new TextureAtlasSubTexture[_numSubTextures];
	
		AssertWDesc(_subTextures != NULL, "TextureAtlas::create() Memory failed.");
	
		if (_subTextures)
		{
			_memUsage += sizeof(TextureAtlasSubTexture) * _numSubTextures;

			texrefraw = (SpriteTextureRefRAW*) (data + endian.swapUint32(header->subTextures));
	
			for (i=0;i<_numSubTextures;i++)
			{
				_subTextures[i].atlas = this;
				_subTextures[i].width = endian.swapUint32(texrefraw[i].width);
				_subTextures[i].height = endian.swapUint32(texrefraw[i].height);
	
				for (j=0;j<4;j++)
				{
#ifdef _USE_OPENGL_ES_1_1

					_subTextures[i].data.PT2D_DIRECT.vertices[j] = _verticeList[endian.swapUint32(texrefraw[i].vertices[j])];
					_subTextures[i].data.PT2D_DIRECT.uvs[j] = _uvList[endian.swapUint32(texrefraw[i].uvs[j])];

#else

#ifdef _USE_OPENGL_IMMEDIATE_MODE

					_subTextures[i].data.PT2D_PTR.vertices[j] = &_verticeList[endian.swapUint32(texrefraw[i].vertices[j])];
					_subTextures[i].data.PT2D_PTR.uvs[j] = &_uvList[endian.swapUint32(texrefraw[i].uvs[j])];

#else

					_subTextures[i].data.PT2D_DIRECT.vertices[j] = _verticeList[endian.swapUint32(texrefraw[i].vertices[j])];
					_subTextures[i].data.PT2D_DIRECT.uvs[j] = _uvList[endian.swapUint32(texrefraw[i].uvs[j])];

#endif /* _USE_OPENGL_IMMEDIATE_MODE */

#endif /* _USE_OPENGL_ES_1_1 */
				}
			}
		}
		else
			error = EOSErrorNoMemory;
	}

	if (error != EOSErrorNone)
		destroy();
	else
	{
		//	For now, get the texture from
		_textureState._textureObj = tex;

		_textureAtlasMgr->updateUsage();
	}

	AssertWDesc(error == EOSErrorNone, "TextureAtlas::create() error");

	return error;
}

EOSError TextureAtlas::createFromFile(const Char* filename, ObjectID objid, const Char* name, Texture* tex, Uint8* buffer, Uint32 maxBufferSize)
{
	EOSError	error = EOSErrorNone;
	FileDescriptor	fdesc;
	File			file;
	
	AssertWDesc(name != NULL, "TextureAtlas::createFromFile() NULL name");
	
	fdesc.setFileAccessType(FileAccessTypeReadOnly);
	
	fdesc.setFilename(filename);
	
	file.setFileDescriptor(fdesc);
	
	error = file.open();
	
	if (error == EOSErrorNone)
	{
		error = file.readUint8(buffer, file.length());
		
		if (error == EOSErrorNone)
		{
			error = create(objid, name, tex, buffer, file.length());
		}
		
		file.close();
	}
	else
	{
		AssertWDesc(1 == 0, "Could not load\n");
	}
	
	return error;
}

ObjectID TextureAtlas::findSubTextureIDFromName(const Char* name)
{
	ObjectID 	objID = InvalidObjectID;
	Uint32		i;

	if (_nameList)
	{
		for (i=0;i<_numSubTextures;i++)
		{
			if (!strcmp(_nameList[i], name))
			{
				objID = i;
				break;
			}
		}
	}

	return objID;
}

TextureAtlasSubTexture* TextureAtlas::findSubTextureFromName(const Char* name)
{
	TextureAtlasSubTexture* subtex = NULL;
	Uint32					i;

	if (_nameList)
	{
		for (i=0;i<_numSubTextures;i++)
		{
			if (!strcmp(_nameList[i], name))
			{
				subtex = &_subTextures[i];
				break;
			}
		}
	}

	return subtex;
}

TextureAtlasSubTexture* TextureAtlas::getSubTexture(ObjectID objid)
{
	TextureAtlasSubTexture* subtex = NULL;

	AssertWDesc(objid < _numSubTextures, "TextureAtlas::getSubTextures() id out of bounds.");
	AssertWDesc(objid != 0xFFFFFFFF, "TextureAtlas::getSubTextures() invalid id.");
	AssertWDesc(_subTextures != NULL, "TextureAtlas::getSubTextures() NULL _subTextures.");

	subtex = &_subTextures[objid];

	return subtex;
}

void TextureAtlas::destroy(void)
{
	if (_subTextures)
	{
		delete [] _subTextures;
		_subTextures = NULL;

		_numSubTextures = 0;
	}

	if (_verticeList)
	{
		delete [] _verticeList;
		_verticeList = NULL;
	}

	if (_uvList)
	{
		delete [] _uvList;
		_uvList = NULL;
	}

	if (_nameData)
	{
		delete [] _nameData;
		_nameData = NULL;
	}

	if (_nameList)
	{
		delete [] _nameList;
		_nameList = NULL;
	}
}

void TextureAtlas::setTexture(Texture* tex)
{
	_textureState._textureObj = tex;
}

void TextureAtlas::setUsed(Boolean used)
{
	_used = used;

	if (_textureAtlasMgr)
		_textureAtlasMgr->updateUsage();
}

void TextureAtlas::setRefID(ObjectID objid)
{
	_id = objid;
}

void TextureAtlas::setName(const Char* name)
{
	if (_name)
	{
		delete _name;
		_name = NULL;
	}

	if (name)
	{
		_name = new Char[strlen(name) + 1];

		if (_name)
		{
			strcpy(_name, name);
		}
	}
}

