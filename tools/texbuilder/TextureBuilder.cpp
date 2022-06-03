/******************************************************************************
 *
 * File: TextureBuilder.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture builder. Builds texture atlases
 * 
 *****************************************************************************/

#include "Platform.h"
#include "TextureBuilder.h"
#include "File.h"

#define ALPHA_INDEX	3
#define RED_INDEX	2
#define GREEN_INDEX	1
#define BLUE_INDEX	0
#define PIXEL_SIZE	4

#define VERSION_NUMBER	1

//	Operators
bool operator<(const TextureArea2& a, const TextureArea2& b)
{
	//	These should have the same bias
	if (a.priority && !b.priority)
	{
		return false;	//	False because priority is for higher items
	}
	else if (!a.priority && b.priority)
	{
		return true;	//	True because priority for higher values
	}
	else if (a.bias == TextureAreaBiasWidth)
	{
		if (a.w == b.w)
			return a.area < b.area;
		else
			return a.w < b.w;
	}
	else
	{
		if (a.h == b.h)
			return a.area < b.area;
		else
			return a.h < b.h;
	}

	return false;
}

bool operator>(const TextureArea2& a, const TextureArea2& b)
{
	//	These should have the same bias
	if (a.priority && !b.priority)
	{
		return true;	//	True because priority for higher values
	}
	else if (!a.priority && b.priority)
	{
		return false;	//	False because priority for higher values
	}
	else if (a.bias == TextureAreaBiasWidth)
	{
		if (a.w == b.w)
			return a.area > b.area;
		else
			return a.w > b.w;
	}
	else
	{
		if (a.h == b.h)
			return a.area > b.area;
		else
			return a.h > b.h;
	}

	return false;
}

bool operator==(const TextureArea2& a, const TextureArea2& b)
{
	if (a.w == b.w && a.h == b.h)
		return true;
	else
		return false;
}

bool operator<(const TextureDetail& a, const TextureDetail& b)
{
	return a.workArea < b.workArea;
}

bool operator>(const TextureDetail& a, const TextureDetail& b)
{
	return a.workArea > b.workArea;
}

bool operator==(const TextureDetail& a, const TextureDetail& b)
{
	return a.workArea.w == b.workArea.w && a.workArea.h == b.workArea.h;
}

TextureDefinition::TextureDefinition() : id(0), transparentColor(NULL),
										padColorTop(NULL), padColorBottom(NULL), padColorLeft(NULL), padColorRight(NULL),
										padTop(NULL), padBottom(NULL), padLeft(NULL), padRight(NULL),
										halfTexel(NULL), priority(NULL), alphaDefringe(NULL), saturation(NULL)
{
}

bool operator<(const TextureDetailPtr& a, const TextureDetailPtr& b)
{
	if (a.texture && b.texture)
		return *a.texture < *b.texture;

	return false;
}

bool operator>(const TextureDetailPtr& a, const TextureDetailPtr& b)
{
	if (a.texture && b.texture)
		return *a.texture > *b.texture;

	return false;
}

TextureDefinition::~TextureDefinition()
{
	if (transparentColor)
		delete transparentColor;

	if (padColorTop)
		delete padColorTop;

	if (padColorBottom)
		delete padColorBottom;

	if (padColorLeft)
		delete padColorLeft;

	if (padColorRight)
		delete padColorRight;

	if (padTop)
		delete padTop;

	if (padBottom)
		delete padBottom;

	if (padLeft)
		delete padLeft;

	if (padRight)
		delete padRight;

	if (halfTexel)
		delete halfTexel;

	if (priority)
		delete priority;

	if (alphaDefringe)
		delete alphaDefringe;

	if (saturation)
		delete saturation;
}

TextureDetail::TextureDetail() : placed(false), master(false), origImage(NULL), workImage(NULL)
{
	Uint32	i;

	origArea.bias = TextureAreaBiasWidth;
	origArea.priority = false;
	origArea.x = 0;
	origArea.y = 0;
	origArea.w = 0;
	origArea.h = 0;
	origArea.area = 0;

	workArea.bias = TextureAreaBiasWidth;
	workArea.priority = false;
	workArea.x = 0;
	workArea.y = 0;
	workArea.w = 0;
	workArea.h = 0;
	workArea.area = 0;

	texDef.texDef = NULL;

	for (i=0;i<4;i++)
	{
		vertices[i] = 0;
		uvs[i] = 0;
	}
}

TextureDetail::~TextureDetail()
{
	if (master)
	{
		if (origImage)
			delete origImage;
	
		if (workImage)
			delete workImage;
	}
}

void TextureDetail::copy(TextureDetail& src)
{
	Uint32	i;

	placed = src.placed;
	master = false;
	origArea = src.origArea;
	workArea = src.workArea;
	texDef.texDef = src.texDef.texDef;

	for (i=0;i<4;i++)
	{
		vertices[i] = src.vertices[i];
		uvs[i] = src.uvs[i];
	}

	origImage = src.origImage;
	workImage = src.workImage;
}

SubTexNode::SubTexNode()
{
	area.bias = TextureAreaBiasWidth;
	area.priority = false;
	area.x = 0;
	area.y = 0;
	area.w = 0;
	area.h = 0;
	area.area = 0;

	texture = NULL;

	child[0] = NULL;
	child[1] = NULL;
}

SubTexNode::~SubTexNode()
{
	if (child[0])
	{
		delete child[0];
		child[0] = NULL;
	}

	if (child[1])
	{
		delete child[1];
		child[1] = NULL;
	}
}

PackedTextureSheet::PackedTextureSheet() : _sourceTexArea(0), _tsheet_width(0), _tsheet_height(0), _image(NULL),
											_exportSubTex(NULL), _exportVertices(NULL), _exportUVs(NULL), _exportNames(NULL),
											_exportNamesOffsets(0), _exportNamesSize(0), _verbose(false), _halfTexelMode(HalfTexelModeOff)
{
	memset(&_exportHeader, 0, sizeof(TextureAtlasHeader));
}

PackedTextureSheet::~PackedTextureSheet()
{
	vector<TextureDetailPtr>::iterator		tex;

	tex = _textures.begin();

	while(tex != _textures.end())
	{
		if (tex->texture)
		{
			delete tex->texture;
		}

		tex++;
	}

	//	Now detroy our tree
	kdtreeDestroy(&_tsheetRoot);

	if (_image)
	{
		delete _image;
		_image = NULL;
	}

	cleanupExportData();
}

void PackedTextureSheet::cleanupExportData(void)
{
	memset(&_exportHeader, 0, sizeof(TextureAtlasHeader));

	if (_exportSubTex)
	{
		delete _exportSubTex;
		_exportSubTex = NULL;
	}

	if (_exportVertices)
	{
		delete _exportVertices;
		_exportVertices = NULL;
	}

	if (_exportUVs)
	{
		delete _exportUVs;
		_exportUVs = NULL;
	}

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

	_exportNamesSize = 0;
}

void PackedTextureSheet::sortTexturesByDefID(void)
{
	Uint32			i, j;
	TextureDetail*	tmp;

	for (i=0;i<_textures.size();i++)
	{
		for (j=i+1;j<_textures.size();j++)
		{
			if (_textures[i].texture->texDef.texDef->id > _textures[j].texture->texDef.texDef->id)
			{
				tmp = _textures[j].texture;

				_textures[j].texture = _textures[i].texture;
				_textures[i].texture = tmp;
			}
		}
	}
}

EOSError PackedTextureSheet::addExportName(Char* name, Char** nameList, Uint32& size, Uint32& offset)
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

void PackedTextureSheet::setTextures(vector<TextureDetailPtr> textures, TextureAreaBias bias)
{
	vector<TextureDetailPtr>::iterator		tex;
	TextureDetail*							detail;
	TextureDetailPtr						texPtr;

	_textures.clear();

	tex = textures.begin();

	while(tex != textures.end())
	{
		detail = new TextureDetail;

		if (detail)
		{
			detail->copy(*tex->texture);

			detail->placed = false;

			//	Now override the bias
			detail->origArea.bias = bias;
			detail->workArea.bias = bias;

			texPtr.texture = detail;
			_textures.push_back(texPtr);
		}

		tex++;
	}
	_sourceTexArea = 0;
}

Boolean PackedTextureSheet::isSourceAreaSufficient(void)
{
	vector<TextureDetailPtr>::iterator		tex;

	_sourceTexArea = 0;

	tex = _textures.begin();

	while(tex != _textures.end())
	{
		if (tex->texture)
		{
			_sourceTexArea += tex->texture->workArea.area;
		}

		tex++;
	}

	if (_sourceTexArea <= (_tsheet_width * _tsheet_height))
		return true;
	else
		return false;
}

void PackedTextureSheet::sortTextures(void)
{
	std::sort(_textures.begin(), _textures.end(), greater<TextureDetailPtr>());
}

void PackedTextureSheet::kdtreeDestroy(SubTexNode* node)
{
	if (node->child[0])
	{
		kdtreeDestroy(node->child[0]);

		delete node->child[0];
		node->child[0] = NULL;
	}

	if (node->child[1])
	{
		kdtreeDestroy(node->child[1]);

		delete node->child[1];
		node->child[1] = NULL;
	}
}

SubTexNode* PackedTextureSheet::kdtreeInsert(TextureDetail* detail, SubTexNode* node)
{
	SubTexNode*	targetNode = NULL;
	Sint32		dw;
	Sint32		dh;

	if (node->child[0] || node->child[1])
	{
		targetNode = kdtreeInsert(detail, node->child[0]);

		if (targetNode == NULL)
			targetNode = kdtreeInsert(detail, node->child[1]);
	}
	else	//	This is a leaf
	{
		//	This is an empty node
		if (node->texture == NULL)
		{
			if (detail->workArea.w <= node->area.w && detail->workArea.h <= node->area.h)
			{
				if (detail->workArea == node->area)
					targetNode = node;
				else
				{
					node->child[0] = new SubTexNode;
					node->child[1] = new SubTexNode;

					if (node->child[0] && node->child[1])
					{
						//	We need to split this node, split it via the largest free area
						dw = (node->area.w - detail->workArea.w);
						dh = (node->area.h - detail->workArea.h);
		
						if (dw > dh)
						{
							node->child[0]->area.bias = detail->workArea.bias;
							node->child[0]->area.x = node->area.x;
							node->child[0]->area.y = node->area.y;
							node->child[0]->area.w = detail->workArea.w;
							node->child[0]->area.h = node->area.h;
							node->child[0]->area.area = node->child[0]->area.w * node->child[0]->area.h;

							node->child[1]->area.bias = detail->workArea.bias;
							node->child[1]->area.x = node->area.x + detail->workArea.w;
							node->child[1]->area.y = node->area.y;
							node->child[1]->area.w = node->area.w - detail->workArea.w;
							node->child[1]->area.h = node->area.h;
							node->child[1]->area.area = node->child[1]->area.w * node->child[1]->area.h;
						}
						else
						{
							node->child[0]->area.bias = detail->workArea.bias;
							node->child[0]->area.x = node->area.x;
							node->child[0]->area.y = node->area.y;
							node->child[0]->area.w = node->area.w;
							node->child[0]->area.h = detail->workArea.h;
							node->child[0]->area.area = node->child[0]->area.w * node->child[0]->area.h;

							node->child[1]->area.bias = detail->workArea.bias;
							node->child[1]->area.x = node->area.x;
							node->child[1]->area.y = node->area.y + detail->workArea.h;
							node->child[1]->area.w = node->area.w;
							node->child[1]->area.h = node->area.h - detail->workArea.h;
							node->child[1]->area.area = node->child[1]->area.w * node->child[1]->area.h;
						}

						targetNode = kdtreeInsert(detail, node->child[0]);
					}
					else
					{
						fprintf(stderr, "ERROR: No memory allocating children nodes.\n");
					}
				}
			}
		}
	}

	return targetNode;
}

EOSError PackedTextureSheet::instantiateTextureSheet(void)
{
	EOSError							error = EOSErrorNone;
	vector<TextureDetailPtr>::iterator	tex;
	Sint32								y;
	Uint8*								line;
	Uint32								lineWidth;
	Uint8*								srcLine; 
	Uint32								srcLineWidth;

	if (_image)
		delete _image;

	_image = new Uint8[_tsheet_width * _tsheet_height * PIXEL_SIZE];

	if (_image)
	{
		memset(_image, 0, _tsheet_width * _tsheet_height * PIXEL_SIZE);

		lineWidth = _tsheet_width * PIXEL_SIZE;

		tex = _textures.begin();

		while (tex != _textures.end())
		{
			srcLineWidth = tex->texture->workArea.w * PIXEL_SIZE;

			if (_verbose)
				printf("Copying %s to %d %d\n", tex->texture->texDef.texDef->label.c_str(), tex->texture->workArea.x, tex->texture->workArea.y);

			for (y=0;y<tex->texture->workArea.h;y++)
			{
				line = &_image[(_tsheet_height - 1 - (y + tex->texture->workArea.y)) * lineWidth];
				srcLine = &tex->texture->workImage[(tex->texture->workArea.h - 1 - y) * srcLineWidth];

				line += tex->texture->workArea.x * PIXEL_SIZE;

				memcpy(line, srcLine, srcLineWidth);
			}

			tex++;
		}
	}
	else
		error = EOSErrorNone;

	return error;
}

Uint32 PackedTextureSheet::findVertice(Point2D& pt)
{
	vector<Point2D>::iterator	ptref;
	int							index = 0;
	Boolean						found = false;

	ptref = _vertices.begin();

	while(ptref != _vertices.end())
	{
		if(ptref->x == pt.x && ptref->y == pt.y)
		{
			found = true;
			break;
		}

		ptref++;
		index++;
	}

	if (found == false)
	{
		//	Add it, index will have been correct due to the index++ above
		_vertices.push_back(pt);
	}

	return index;
}

Uint32 PackedTextureSheet::findUV(Point2D& pt)
{
	vector<Point2D>::iterator	ptref;
	int							index = 0;
	Boolean						found = false;

	ptref = _uvs.begin();

	while(ptref != _uvs.end())
	{
		if(ptref->x == pt.x && ptref->y == pt.y)
		{
			found = true;
			break;
		}

		ptref++;
		index++;
	}

	if (found == false)
	{
		//	Add it, index will have been correct due to the index++ above
		_uvs.push_back(pt);
	}

	return index;
}

EOSError PackedTextureSheet::generateGLCoordinates(TextureDetail* detail)
{
	EOSError	error = EOSErrorNone;
	Point2D		verts[4]; 
	Point2D		uvs[4]; 
	Sint32		i;
	Float32		x, y, w, h;
	Float32		tw, th;
	Float32		halftex = 0.0;

	if (_halfTexelMode == HalfTexelModeOn || (detail->texDef.texDef->halfTexel && *detail->texDef.texDef->halfTexel && _halfTexelMode == HalfTexelModeOff))
	{
		halftex = 0.5F;

	}

	x = (Float32) detail->workArea.x + detail->origArea.x;
	y = (Float32) detail->workArea.y + detail->origArea.y;
	w = (Float32) detail->origArea.w;
	h = (Float32) detail->origArea.h;
	tw = (Float32) _tsheet_width;
	th = (Float32) _tsheet_height;

	//	Here GL coordinates will be built starting from lower left corner, clockwise 0 - 1 - 2 -3, ending up in the lower right corner

	//	0
	verts[0].x = -w / 2.0F;
	verts[0].y = -h / 2.0F;

	uvs[0].x = (x + halftex) / tw;
	uvs[0].y = (th - (y + h + halftex)) / th;

	//	1
	verts[1].x = -w / 2.0F;
	verts[1].y = h / 2.0F;

	uvs[1].x = (x + halftex) / tw;
	uvs[1].y = (th - (y - halftex)) / th;

	//	2
	verts[2].x = w / 2.0F;
	verts[2].y = h / 2.0F;

	uvs[2].x = (x + w - halftex) / tw;
	uvs[2].y = (th - (y - halftex)) / th;

	//	3
	verts[3].x = w / 2.0F;
	verts[3].y = -h / 2.0F;

	uvs[3].x = (x + w - halftex) / tw;
	uvs[3].y = (th - (y + h + halftex)) / th;

	for (i=0;i<4;i++)
	{
		detail->vertices[i] = findVertice(verts[i]);

		detail->uvs[i] = findUV(uvs[i]);
	}

	return error;
}

EOSError PackedTextureSheet::generateExportData(Char* name, Sint32 texID)
{
	EOSError							error = EOSErrorNone;
	vector<Point2D>::iterator			ptref;
	vector<TextureDetailPtr>::iterator	tex;
	Uint32								index;
	Uint32								i;
	Uint32								offset = 0;

	cleanupExportData();

	_exportHeader.endian = 0x01020304;
	_exportHeader.version = VERSION_NUMBER;
	_exportHeader.textureID = texID;

	if (name)
	{
		error = addExportName((Char*) name, &_exportNames, _exportNamesSize, offset);

		_exportHeader.nameOffset = offset;
	}
	else
		_exportHeader.nameOffset = 0xFFFFFFFF;

	_exportHeader.textureFormat = _tsheet_width;

	_exportHeader.width = _tsheet_width;
	_exportHeader.height = _tsheet_height;

	//	No export of texture right now
	_exportHeader.textureSize = 0;
	_exportHeader.texture = 0xFFFFFFFF;

	//	Generate the different data sets needed
	_exportHeader.numSubTextures = (Uint32) _textures.size();
	_exportHeader.numNameOffsets = (Uint32) _textures.size() + 1;

	_exportSubTex = new SpriteTextureRefRAW[_exportHeader.numSubTextures];
	_exportNamesOffsets = new Uint32[_exportHeader.numSubTextures];

	if (_exportSubTex && _exportNamesOffsets)
	{
		memset(_exportSubTex, 0, sizeof(SpriteTextureRefRAW));
		memset(_exportNamesOffsets, 0, sizeof(Uint32));

		sortTexturesByDefID();
	
		tex = _textures.begin();
		index = 0;

		while (tex != _textures.end())
		{
			_exportSubTex[index].id = _exportHeader.textureID;
			_exportSubTex[index].width = tex->texture->origArea.w;
			_exportSubTex[index].height = tex->texture->origArea.h;

			for (i=0;i<4;i++)
			{
				_exportSubTex[index].vertices[i] = tex->texture->vertices[i];
				_exportSubTex[index].uvs[i] = tex->texture->uvs[i];
			}

			error = addExportName((Char*) tex->texture->texDef.texDef->label.c_str(), &_exportNames, _exportNamesSize, offset);
			_exportNamesOffsets[index] = offset;

			if (error != EOSErrorNone)
				break;

			tex++;
			index++;
		}
	}
	else
		error = EOSErrorNoMemory;

	if (error == EOSErrorNone)
	{
		_exportHeader.numVertices = (Uint32) _vertices.size();

		_exportVertices = new Point2D[_vertices.size()];
		index = 0;

		if (_exportVertices)
		{
			ptref = _vertices.begin();

			while (ptref != _vertices.end())
			{
				_exportVertices[index].x = ptref->x;
				_exportVertices[index].y = ptref->y;

				ptref++;
				index++;
			}
		}
		else
			error = EOSErrorNoMemory;
	}

	if (error == EOSErrorNone)
	{
		_exportHeader.numUVs = (Uint32) _uvs.size();

		_exportUVs = new Point2D[_uvs.size()];
		index = 0;

		if (_exportUVs)
		{
			ptref = _uvs.begin();

			while (ptref != _uvs.end())
			{
				_exportUVs[index].x = ptref->x;
				_exportUVs[index].y = ptref->y;

				ptref++;
				index++;
			}
		}
		else
			error = EOSErrorNoMemory;
	}

	//	Note offsets expect data to be exported in the following layers:
	//	Header
	// 	Texture (if exists)
	// 	SubTex
	// 	Verts
	// 	UVs
	// 	NamesOffsets
	// 	Names

	offset = sizeof(TextureAtlasHeader);

	offset += _exportHeader.textureSize;
	_exportHeader.subTextures = offset;

	offset += _exportHeader.numSubTextures * sizeof(SpriteTextureRefRAW);
	_exportHeader.vertices = offset;

	offset += _exportHeader.numVertices * sizeof(Point2D);
	_exportHeader.uvs = offset;

	offset += _exportHeader.numUVs * sizeof(Point2D);

	_exportHeader.nameOffsets = offset;
	offset += _exportHeader.numNameOffsets * sizeof(Uint32);

	_exportHeader.nameData = offset;

	return error;
}

EOSError PackedTextureSheet::exportTGA(Char* filename)
{
	FileDescriptor	fd;
	File			file;
	EOSError		error = EOSErrorNone;

	if (_image)
	{
		fd.setFilename(filename);
		fd.setFileAccessType(FileAccessTypeWriteOnly);
	
		file.setFileDescriptor(fd);
	
		error = file.open();
	
		if (error == EOSErrorNone)
		{
			file.writeUint8((Uint8) 0);	//	Header Length
			file.writeUint8((Uint8) 0);
		
			file.writeUint8((Uint8) 2);	//	imageType RGBA
		
			file.writeUint8((Uint8) 0);
			file.writeUint8((Uint8) 0);
			file.writeUint8((Uint8) 0);
			file.writeUint8((Uint8) 0);
			file.writeUint8((Uint8) 0);
			file.writeUint8((Uint8) 0);
			file.writeUint8((Uint8) 0);
			file.writeUint8((Uint8) 0);
			file.writeUint8((Uint8) 0);
		
			file.writeSint16(_tsheet_width);
			file.writeSint16(_tsheet_height);
		
			file.writeUint8((Uint8) 32);	//	bits
		
			file.writeUint8((Uint8) 0);
		
			file.writeUint8(_image, _tsheet_width * _tsheet_height * PIXEL_SIZE);
	
			file.close();
		}
	}
	else
		error = EOSErrorResourceDoesNotExist;

	return error;
}

#define STR_SIZE	256

EOSError PackedTextureSheet::exportAtlasXML(Char* filename)
{
	FileDescriptor						fd;
	File								file;
	EOSError							error = EOSErrorNone;
	Char								str[STR_SIZE];
	vector<TextureDetailPtr>::iterator	tex;
	Uint32								i;

	if (_image)
	{
		fd.setFilename(filename);
		fd.setFileAccessType(FileAccessTypeWriteOnly);

		file.setFileDescriptor(fd);

		error = file.open();

		if (error == EOSErrorNone)
		{
			sprintf_s(str, STR_SIZE, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
			file.writeSint8((Sint8*) str, (Uint32) strlen(str));

			sprintf_s(str, STR_SIZE, "<!DOCTYPE texatlas SYSTEM \"texatlas.dtd\">\n"); 
			file.writeSint8((Sint8*) str, (Uint32) strlen(str));

			sprintf_s(str, STR_SIZE, "<texatlas>\n");
			file.writeSint8((Sint8*) str, (Uint32) strlen(str));

			sprintf_s(str, STR_SIZE, "\t<name>%s</name>\n", &_exportNames[_exportHeader.nameOffset]);
			file.writeSint8((Sint8*) str, (Uint32) strlen(str));

			sprintf_s(str, STR_SIZE, "\t<dimensions>\n");
			file.writeSint8((Sint8*) str, (Uint32) strlen(str));

			sprintf_s(str, STR_SIZE, "\t\t<width>%d</width>\n", _exportHeader.width);
			file.writeSint8((Sint8*) str, (Uint32) strlen(str));

			sprintf_s(str, STR_SIZE, "\t\t<height>%d</height>\n", _exportHeader.height);
			file.writeSint8((Sint8*) str, (Uint32) strlen(str));

			sprintf_s(str, STR_SIZE, "\t</dimensions>\n");
			file.writeSint8((Sint8*) str, (Uint32) strlen(str));

			tex = _textures.begin();

			while (tex != _textures.end())
			{
				sprintf_s(str, STR_SIZE, "\t<subtex>\n");
				file.writeSint8((Sint8*) str, (Uint32) strlen(str));

				sprintf_s(str, STR_SIZE, "\t\t<refid>%d</refid>\n", tex->texture->texDef.texDef->id);
				file.writeSint8((Sint8*) str, (Uint32) strlen(str));

				sprintf_s(str, STR_SIZE, "\t\t<name>%s</name>\n", tex->texture->texDef.texDef->label.c_str());
				file.writeSint8((Sint8*) str, (Uint32) strlen(str));

				sprintf_s(str, STR_SIZE, "\t\t<vertices>\n");
				file.writeSint8((Sint8*) str, (Uint32) strlen(str));

				for (i=0;i<4;i++)
				{
					sprintf_s(str, STR_SIZE, "\t\t\t<xy%d>\n", i);
					file.writeSint8((Sint8*) str, (Uint32) strlen(str));

					sprintf_s(str, STR_SIZE, "\t\t\t\t<x>%.10f</x>\n", _vertices[tex->texture->vertices[i]].x);
					file.writeSint8((Sint8*) str, (Uint32) strlen(str));

					sprintf_s(str, STR_SIZE, "\t\t\t\t<y>%.10f</y>\n", _vertices[tex->texture->vertices[i]].y);
					file.writeSint8((Sint8*) str, (Uint32) strlen(str));

					sprintf_s(str, STR_SIZE, "\t\t\t</xy%d>\n", i);
					file.writeSint8((Sint8*) str, (Uint32) strlen(str));
				}

				sprintf_s(str, STR_SIZE, "\t\t</vertices>\n");
				file.writeSint8((Sint8*) str, (Uint32) strlen(str));

				sprintf_s(str, STR_SIZE, "\t\t<uvs>\n");
				file.writeSint8((Sint8*) str, (Uint32) strlen(str));

				for (i=0;i<4;i++)
				{
					sprintf_s(str, STR_SIZE, "\t\t\t<uv%d>\n", i);
					file.writeSint8((Sint8*) str, (Uint32) strlen(str));

					sprintf_s(str, STR_SIZE, "\t\t\t\t<u>%.10f</u>\n", _uvs[tex->texture->uvs[i]].x);
					file.writeSint8((Sint8*) str, (Uint32) strlen(str));

					sprintf_s(str, STR_SIZE, "\t\t\t\t<v>%.10f</v>\n", _uvs[tex->texture->uvs[i]].y);
					file.writeSint8((Sint8*) str, (Uint32) strlen(str));

					sprintf_s(str, STR_SIZE, "\t\t\t</uv%d>\n", i);
					file.writeSint8((Sint8*) str, (Uint32) strlen(str));
				}

				sprintf_s(str, STR_SIZE, "\t\t</uvs>\n");
				file.writeSint8((Sint8*) str, (Uint32) strlen(str));

				sprintf_s(str, STR_SIZE, "\t</subtex>\n");
				file.writeSint8((Sint8*) str, (Uint32) strlen(str));

				tex++;
			}

			sprintf_s(str, STR_SIZE, "</texatlas>\n");
			file.writeSint8((Sint8*) str, (Uint32) strlen(str));

			file.close();
		}
	}
	else
		error = EOSErrorResourceDoesNotExist;

	return error;
}

EOSError PackedTextureSheet::exportAtlasBIN(Char* filename)
{
	FileDescriptor	fd;
	File			file;
	EOSError		error = EOSErrorNone;

	if (_image)
	{
		fd.setFilename(filename);
		fd.setFileAccessType(FileAccessTypeWriteOnly);

		file.setFileDescriptor(fd);

		error = file.open();

		if (error == EOSErrorNone)
		{
			file.writeUint8((Uint8*) &_exportHeader, sizeof(TextureAtlasHeader));

			if (_exportHeader.textureSize)
				file.writeUint8(_image, _tsheet_width * _tsheet_height * PIXEL_SIZE);

			file.writeUint8((Uint8*) _exportSubTex, _exportHeader.numSubTextures * sizeof(SpriteTextureRefRAW));
			file.writeUint8((Uint8*) _exportVertices, _exportHeader.numVertices * sizeof(Point2D));
			file.writeUint8((Uint8*) _exportUVs, _exportHeader.numUVs * sizeof(Point2D));
			file.writeUint8((Uint8*) _exportNamesOffsets, _exportHeader.numNameOffsets * sizeof(Uint32));
			file.writeUint8((Uint8*) _exportNames, _exportNamesSize);

			file.close();
		}
	}
	else
		error = EOSErrorResourceDoesNotExist;

	return error;
}

EOSError PackedTextureSheet::generateGLCoordinates(void)
{
	EOSError							error = EOSErrorNone;
	vector<TextureDetailPtr>::iterator	tex;

	tex = _textures.begin();

	while(tex != _textures.end())
	{
		if (tex->texture)
		{
			error = generateGLCoordinates(tex->texture);
		}

		if (error != EOSErrorNone)
			break;

		tex++;
	}

	return error;
}

EOSError PackedTextureSheet::buildTextureSheet(void)
{
	EOSError							error = EOSErrorNone;
	vector<TextureDetailPtr>::iterator	tex;
	SubTexNode*							node;

	sortTextures();

	tex = _textures.begin();

	if (tex != _textures.end())
	{
		_tsheetRoot.area.bias = tex->texture->workArea.bias;
		_tsheetRoot.area.x = 0;
		_tsheetRoot.area.y = 0;
		_tsheetRoot.area.w = _tsheet_width;
		_tsheetRoot.area.h = _tsheet_height;
		_tsheetRoot.area.area = _tsheet_width * _tsheet_height;

		//	Now insert our nodes into the tree
		while (tex != _textures.end())
		{
			node = kdtreeInsert(tex->texture, &_tsheetRoot);

			if (node)
			{
				node->texture = tex->texture;
				tex->texture->placed = true;

				//	Now, assign the X, Y bounds to the texture
				tex->texture->workArea.x = node->area.x;
				tex->texture->workArea.y = node->area.y;

				fprintf(stdout, "Placing texture %s at (%d, %d)\n", tex->texture->texDef.texDef->label.c_str(), node->area.x, node->area.y);

				//	Do a double check here, areas should match!
				if (tex->texture->workArea.w != node->area.w || tex->texture->workArea.h != node->area.h)
				{
					fprintf(stderr, "WARNING: %s dimension is (%d, %d). Node dimension is (%d, %d)\n", tex->texture->texDef.texDef->label.c_str(), tex->texture->workArea.w, tex->texture->workArea.h, node->area.w, node->area.h);
				}
			}
			else
			{
				error = EOSErrorResourceDoesNotExist;
				break;
			}

			tex++;
		}

		if (error == EOSErrorNone)
		{
			error = instantiateTextureSheet();

			if (error == EOSErrorNone)
			{
				error = generateGLCoordinates();
			}
		}
	}

	return error;
}

EOSError PackedTextureSheet::exportTextureSheet(Char* prefix, Char* name, Sint32 texID, Boolean tga, Boolean atlas, Boolean xml)
{
	EOSError	error;
	Char*		expname;
	Char		str[256];

	if (name == NULL)
		expname = prefix;
	else
		expname = name;

	error = generateExportData(expname, texID);

	if (tga)
	{
		sprintf(str, "%s.tga", prefix);
		error = exportTGA(str);
	}

	if (atlas && error == EOSErrorNone)
	{
		sprintf(str, "%s.bin", prefix);
		error = exportAtlasBIN(str);
	}

	if (xml && error == EOSErrorNone)
	{
		sprintf(str, "%s.xml", prefix);
		error = exportAtlasXML(str);
	}

	return error;
}

TextureBuilder::TextureBuilder() : _textureCount(0), _verbose(false), _tsheet_width(0), _tsheet_height(0),
									_transColorMode(TransparentColorModeDefault), _halfTexelMode(HalfTexelModeOff),
									_power2(false), _smallest(false), _optimize(false),
									_exportName(NULL), _exportPrefix(NULL), _exportID(0)
{
	_transColor.r = _transColor.g = _transColor.b = _transColor.a = 0;

	memset(&_padEdgeColor, 0, sizeof(PadEdgeColor));

	_alphaDefringe.mode = AlphaDefringeModeOff;
	_alphaDefringe.amount = 0;

	_saturation.saturate = false;
	_saturation.saturationParam = 1.0;
}

TextureBuilder::~TextureBuilder()
{
	vector<TextureDefinitionPtr>::iterator	def;
	vector<TextureDetailPtr>::iterator		tex;

	def = _textureDefs.begin();

	while(def != _textureDefs.end())
	{
		if (def->texDef)
		{
			delete def->texDef;
		}

		def++;
	}

	tex = _masterTextures.begin();

	while(tex != _masterTextures.end())
	{
		if (tex->texture)
		{
			delete tex->texture;
		}

		tex++;
	}

	if (_exportName)
	{
		delete _exportName;
		_exportName = NULL;
	}

	if (_exportPrefix)
	{
		delete _exportPrefix;
		_exportPrefix = NULL;
	}
}

Boolean	TextureBuilder::isEmptyLine(string line)
{
	Boolean		empty = true;
	Uint32		i;
	const char*	str;

	if (line.length() != 0)
	{
		str = line.c_str();

		for (i=0;i<line.length();i++)
		{
			if ((str[i] < 0x09 || str[i] > 0x0D) && str[i] != 0x20)
			{
				empty = false;
				break;
			}
		}
	}

	return empty;
}

Sint32	TextureBuilder::readLine(Uint8* buffer, Uint8* end, string& str)
{
	Sint32			amount = -1;
	Sint32			new_line_marker = 0;
	Uint8*			tbuffer = buffer;
	Boolean			snip = false;
	Sint32			minus = 0;

	//	Empty the line
	str.clear();

	if(buffer < end)
	{
		while((tbuffer + new_line_marker) < end)
		{
			if(tbuffer[new_line_marker] == 0x0A)
				snip = true;
			else if(tbuffer[new_line_marker] == 0x0D)
			{
				//	See if we have a 0x0A
				if((tbuffer + new_line_marker + 1) < end)
				{
					if(tbuffer[new_line_marker + 1] == 0x0A)
					{
						new_line_marker++;
						minus++;
					}
				}

				snip = true;
			}

			if(snip)
			{
				if(new_line_marker > 0)
					str.append((char*) tbuffer, new_line_marker - minus);

				new_line_marker++;

				amount = new_line_marker;
				break;
			}

			new_line_marker++;
		}
	}

	return amount;
}

Boolean TextureBuilder::parseLineForTextureDef(string line, TextureDefinitionPtr& def, EOSError& error)
{
	Boolean				parsed = false;
	const char*			c_str;
	Sint32				token = 0;
	Boolean				quit = false;
	Uint32				index = 0;
	Sint32				word_start = 0;
	Boolean				in_word = false;
	string				word;
	TextureDefinition*	texDef;
	TextureDefinition	temp_def;

	c_str = line.c_str();

	temp_def.label.clear();
	temp_def.filename.clear();

	def.texDef = NULL;

	while(token < 2 && quit == false && index < strlen(c_str))
	{
		//	Eat any opening whitespaces
		if(c_str[index] == ' ' || c_str[index] == '\t' || c_str[index] == 0x0A || c_str[index] == 0x0D)
		{
			if(in_word)
			{
				word.clear();

				word.append(&c_str[word_start], index - word_start);

				switch(token)
				{
					case 0:
						temp_def.label = word;
						break;

					case 1:
						temp_def.filename = word;
						parsed = true;
						quit = true;
						break;

					default:
						parsed = true;
						quit = true;
						break;
				}

				token++;
				in_word = false;
			}
		}
		else
		{
			if(c_str[index] == '#')
				break;

			if(in_word == false)
				word_start = index;

			in_word = true;
		}

		index++;
	}

	if(in_word && token == 1)
	{
		temp_def.filename.append(&c_str[word_start], index - word_start);
		parsed = true;
		token++;
	}

	if(token < 2)
		error = EOSErrorIllegalGrammar;

	if(error == EOSErrorNone && parsed)
	{
		texDef = new TextureDefinition;

		if(texDef)
		{
			texDef->id = _textureCount;
			_textureCount++;

			texDef->label = temp_def.label;
			texDef->filename = temp_def.filename;

			printf("Adding texdef %s\n", texDef->label.c_str());

			def.texDef = texDef;
		}
		else
		{
			fprintf(stderr, "Could not allocate memory\n");
			error = EOSErrorNone;
		}
	}

	return parsed;
}

EOSError TextureBuilder::parseTextureDefFile(Uint8* buffer, Uint32 size)
{
	EOSError				error = EOSErrorNone;
	Uint8*					tbuffer = buffer;
	Uint8*					end = &buffer[size];
	string					line;
	int						read_bytes;
	int						num_lines = 1;
	TextureDefinitionPtr	texDef;

	if(buffer)
	{
		while(tbuffer < end)
		{
			read_bytes = readLine(tbuffer, end, line);

			if(read_bytes > 0)
			{
				if (isEmptyLine(line) == false)
				{
					if (parseLineForTextureDef(line, texDef, error))
					{
						if(texDef.texDef)
							_textureDefs.push_back(texDef);
					}
					else
					{
						if(error == EOSErrorIllegalGrammar)
							fprintf(stderr, "Line %d: Illegal Grammar\n", num_lines);
						else
							fprintf(stderr, "Line %d: Error %d\n", num_lines, error);
					}
				}

				tbuffer += read_bytes;
			}

			if(error != EOSErrorNone)
				break;

			num_lines++;
		}
	}

	return error;
}

EOSError TextureBuilder::loadTextureDefFile(Char* filename)
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
					error = parseTextureDefFile(buffer, size);

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

void TextureBuilder::setExportPrefix(Char* name)
{
	if (_exportPrefix)
		delete _exportPrefix;

	if (name)
	{
		_exportPrefix = new Char[strlen(name) + 1];
	
		if (_exportPrefix)
			strcpy(_exportPrefix, name);
	}
	else
		_exportPrefix = NULL;
}

void TextureBuilder::setExportName(Char* name)
{
	if (_exportName)
		delete _exportName;

	if (name)
	{
		_exportName = new Char[strlen(name) + 1];
	
		if (_exportName)
			strcpy(_exportName, name);
	}
	else
		_exportName = NULL;
}

EOSError TextureBuilder::assignTexPropToTextureDefinition(XMLElementSubTex* subtex)
{
	EOSError								error = EOSErrorNone;
	TextureDefinition*						texDef = findTextureDefinitionFromLabel(subtex->getName()->getString());

	if (texDef)
	{
		if (subtex->getTransparentColor())
		{
			if (texDef->transparentColor)
				delete texDef->transparentColor;

			texDef->transparentColor = new ColorIntRGBA;

			if (texDef->transparentColor)
			{
				texDef->transparentColor->r = subtex->getTransparentColor()->getRed()->getValue();
				texDef->transparentColor->g = subtex->getTransparentColor()->getGreen()->getValue();
				texDef->transparentColor->b = subtex->getTransparentColor()->getBlue()->getValue();
				texDef->transparentColor->a = subtex->getTransparentColor()->getAlpha()->getValue();
			}
			else
				error = EOSErrorNoMemory;
		}

		if (subtex->getPadAll())
		{
			if (texDef->padTop)
				delete texDef->padTop;

			if (texDef->padBottom)
				delete texDef->padBottom;

			if (texDef->padLeft)
				delete texDef->padLeft;

			if (texDef->padRight)
				delete texDef->padRight;

			texDef->padTop = new Uint32;
			texDef->padBottom = new Uint32;
			texDef->padLeft = new Uint32;
			texDef->padRight = new Uint32;

			if (texDef->padTop)
				*texDef->padTop = subtex->getPadAll()->getValue();
			else
				error = EOSErrorNoMemory;

			if (texDef->padBottom)
				*texDef->padBottom = subtex->getPadAll()->getValue();
			else
				error = EOSErrorNoMemory;

			if (texDef->padLeft)
				*texDef->padLeft = subtex->getPadAll()->getValue();
			else
				error = EOSErrorNoMemory;

			if (texDef->padRight)
				*texDef->padRight = subtex->getPadAll()->getValue();
			else
				error = EOSErrorNoMemory;
		}
		else if (subtex->getPad())
		{
			if (subtex->getPad()->getPadTop())
			{
				if (texDef->padTop)
					delete texDef->padTop;

				texDef->padTop = new Uint32;

				if (texDef->padTop)
					*texDef->padTop = subtex->getPad()->getPadTop()->getValue();
				else
					error = EOSErrorNoMemory;
			}

			if (subtex->getPad()->getPadBottom())
			{
				if (texDef->padBottom)
					delete texDef->padBottom;

				texDef->padBottom = new Uint32;

				if (texDef->padBottom)
					*texDef->padBottom = subtex->getPad()->getPadBottom()->getValue();
				else
					error = EOSErrorNoMemory;
			}

			if (subtex->getPad()->getPadLeft())
			{
				if (texDef->padLeft)
					delete texDef->padLeft;

				texDef->padLeft = new Uint32;

				if (texDef->padLeft)
					*texDef->padLeft = subtex->getPad()->getPadLeft()->getValue();
				else
					error = EOSErrorNoMemory;
			}

			if (subtex->getPad()->getPadRight())
			{
				if (texDef->padRight)
					delete texDef->padRight;
	
				texDef->padRight = new Uint32;	
	
				if (texDef->padRight)
					*texDef->padRight = subtex->getPad()->getPadRight()->getValue();
				else
					error = EOSErrorNoMemory;
			}
		}

		if (subtex->getPadColor())
		{
			if (texDef->padColorTop)
				delete texDef->padColorTop;

			if (texDef->padColorBottom)
				delete texDef->padColorBottom;

			if (texDef->padColorLeft)
				delete texDef->padColorLeft;

			if (texDef->padColorRight)
				delete texDef->padColorRight;

			texDef->padColorTop = new ColorIntRGBA;
			texDef->padColorBottom = new ColorIntRGBA;
			texDef->padColorLeft = new ColorIntRGBA;
			texDef->padColorRight = new ColorIntRGBA;

			if (texDef->padColorTop)
			{
				texDef->padColorTop->r = subtex->getPadColor()->getRed()->getValue();
				texDef->padColorTop->g = subtex->getPadColor()->getGreen()->getValue();
				texDef->padColorTop->b = subtex->getPadColor()->getBlue()->getValue();
				texDef->padColorTop->a = subtex->getPadColor()->getAlpha()->getValue();
			}
			else
				error = EOSErrorNoMemory;

			if (texDef->padColorBottom)
			{
				texDef->padColorBottom->r = subtex->getPadColor()->getRed()->getValue();
				texDef->padColorBottom->g = subtex->getPadColor()->getGreen()->getValue();
				texDef->padColorBottom->b = subtex->getPadColor()->getBlue()->getValue();
				texDef->padColorBottom->a = subtex->getPadColor()->getAlpha()->getValue();
			}
			else
				error = EOSErrorNoMemory;

			if (texDef->padColorLeft)
			{
				texDef->padColorLeft->r = subtex->getPadColor()->getRed()->getValue();
				texDef->padColorLeft->g = subtex->getPadColor()->getGreen()->getValue();
				texDef->padColorLeft->b = subtex->getPadColor()->getBlue()->getValue();
				texDef->padColorLeft->a = subtex->getPadColor()->getAlpha()->getValue();
			}
			else
				error = EOSErrorNoMemory;

			if (texDef->padColorRight)
			{
				texDef->padColorRight->r = subtex->getPadColor()->getRed()->getValue();
				texDef->padColorRight->g = subtex->getPadColor()->getGreen()->getValue();
				texDef->padColorRight->b = subtex->getPadColor()->getBlue()->getValue();
				texDef->padColorRight->a = subtex->getPadColor()->getAlpha()->getValue();
			}
			else
				error = EOSErrorNoMemory;
		}
		else if (subtex->getPadEdgeColor())
		{
			if (subtex->getPadEdgeColor()->getPadColorTop())
			{
				if (texDef->padColorTop)
					delete texDef->padColorTop;

				texDef->padColorTop = new ColorIntRGBA;

				if (texDef->padColorTop)
				{
					texDef->padColorTop->r = subtex->getPadEdgeColor()->getPadColorTop()->getRed()->getValue();
					texDef->padColorTop->g = subtex->getPadEdgeColor()->getPadColorTop()->getGreen()->getValue();
					texDef->padColorTop->b = subtex->getPadEdgeColor()->getPadColorTop()->getBlue()->getValue();
					texDef->padColorTop->a = subtex->getPadEdgeColor()->getPadColorTop()->getAlpha()->getValue();
				}
				else
					error = EOSErrorNoMemory;
			}

			if (subtex->getPadEdgeColor()->getPadColorBottom())
			{
				if (texDef->padColorBottom)
					delete texDef->padColorBottom;

				texDef->padColorBottom = new ColorIntRGBA;

				if (texDef->padColorBottom)
				{
					texDef->padColorBottom->r = subtex->getPadEdgeColor()->getPadColorBottom()->getRed()->getValue();
					texDef->padColorBottom->g = subtex->getPadEdgeColor()->getPadColorBottom()->getGreen()->getValue();
					texDef->padColorBottom->b = subtex->getPadEdgeColor()->getPadColorBottom()->getBlue()->getValue();
					texDef->padColorBottom->a = subtex->getPadEdgeColor()->getPadColorBottom()->getAlpha()->getValue();
				}
				else
					error = EOSErrorNoMemory;
			}

			if (subtex->getPadEdgeColor()->getPadColorLeft())
			{
				if (texDef->padColorLeft)
					delete texDef->padColorLeft;

				texDef->padColorLeft = new ColorIntRGBA;

				if (texDef->padColorLeft)
				{
					texDef->padColorLeft->r = subtex->getPadEdgeColor()->getPadColorLeft()->getRed()->getValue();
					texDef->padColorLeft->g = subtex->getPadEdgeColor()->getPadColorLeft()->getGreen()->getValue();
					texDef->padColorLeft->b = subtex->getPadEdgeColor()->getPadColorLeft()->getBlue()->getValue();
					texDef->padColorLeft->a = subtex->getPadEdgeColor()->getPadColorLeft()->getAlpha()->getValue();
				}
				else
					error = EOSErrorNoMemory;
			}

			if (subtex->getPadEdgeColor()->getPadColorRight())
			{
				if (texDef->padColorRight)
					delete texDef->padColorRight;
	
				texDef->padColorRight = new ColorIntRGBA;

				if (texDef->padColorRight)
				{
					texDef->padColorRight->r = subtex->getPadEdgeColor()->getPadColorRight()->getRed()->getValue();
					texDef->padColorRight->g = subtex->getPadEdgeColor()->getPadColorRight()->getGreen()->getValue();
					texDef->padColorRight->b = subtex->getPadEdgeColor()->getPadColorRight()->getBlue()->getValue();
					texDef->padColorRight->a = subtex->getPadEdgeColor()->getPadColorRight()->getAlpha()->getValue();
				}
				else
					error = EOSErrorNoMemory;
			}
		}

		if (subtex->getHalfTexel())
		{
			if (texDef->halfTexel)
				delete texDef->halfTexel;

			texDef->halfTexel = new Boolean;

			if (texDef->halfTexel)
			{
				if (subtex->getHalfTexel())
					*texDef->halfTexel = subtex->getHalfTexel()->getValue();
				else if (_halfTexelMode == HalfTexelModeOn)
					*texDef->halfTexel = true;
				else
					*texDef->halfTexel = false;
			}
			else
				error = EOSErrorNoMemory;
		}

		if (subtex->getPriority())
		{
			if (texDef->priority)
				delete texDef->priority;

			texDef->priority = new Boolean;

			if (texDef->priority)
				*texDef->priority = subtex->getPriority()->getValue();
			else
				error = EOSErrorNoMemory;
		}

		if (subtex->getAlphaDefringe())
		{
			if (texDef->alphaDefringe)
				delete texDef->alphaDefringe;

			texDef->alphaDefringe = new Uint32;

			if (texDef->alphaDefringe)
				*texDef->alphaDefringe = subtex->getAlphaDefringe()->getValue();
			else
				error = EOSErrorNoMemory;
		}
	}
	else
		error = EOSErrorResourceDoesNotExist;

	return error;
}

TextureDefinition* TextureBuilder::findTextureDefinitionFromLabel(string label)
{
	TextureDefinition*						texDef = NULL;
	vector<TextureDefinitionPtr>::iterator	ref;

	ref = _textureDefs.begin();

	while(ref != _textureDefs.end())
	{
		if (ref->texDef)
		{
			if (ref->texDef->label == label)
			{
				texDef = ref->texDef;
				break;
			}
		}

		ref++;
	}

	return texDef;
}

EOSError TextureBuilder::loadTextureTGAFile(TextureDefinitionPtr& def, TextureDetail& tex)
{
	EOSError		error = EOSErrorNone;
	FileDescriptor	fd;
	File			file;
	unsigned char*	buffer;
	unsigned char*	tbuffer;
	Endian			endian;
	Uint8			headerLength;
	Uint8			imageType;
	Uint8			bits;

	if(def.texDef)
	{
		wstring			filenameUTF16(def.texDef->filename.c_str(), def.texDef->filename.c_str() + def.texDef->filename.length());

		if(_verbose)
			fprintf(stdout, "Loading texture %s..\n", def.texDef->filename.c_str());

		fd.setFilename((UTF16*) filenameUTF16.c_str());
		fd.setFileAccessType(FileAccessTypeReadOnly);

		file.setFileDescriptor(fd);

		error = file.open();

		if(error == EOSErrorNone)
		{
			tbuffer = buffer = new Uint8[file.length()];

			if(tbuffer)
			{
				error = file.readUint8(tbuffer, file.length());

				if (error == EOSErrorNone)
				{
					headerLength = *tbuffer;

					tbuffer++;
					tbuffer++;

					imageType = *tbuffer;
					tbuffer++;

					tbuffer += 9;

					tex.origArea.w = endian.readSint16(tbuffer);
					tbuffer += 2;
					tex.origArea.h = endian.readSint16(tbuffer);
					tbuffer += 2;

					bits = *tbuffer;
					tbuffer++;

					tbuffer += headerLength + 1;

					if (imageType != 10)
					{
						// Support for LUMINANCE and RGBA textures
						if((bits == 32)) 
						{
						}
						else
							error = EOSErrorUnsupported; 
					}
					else
						error = EOSErrorUnsupported; 

#ifdef NOT_YET
					if((float) tex.origArea.h >= (_tsheet_h * VerticalHintPercentage))
						tex.vertical_hint = true;
#endif

					tex.origArea.area = tex.origArea.w * tex.origArea.h;

					if(error == EOSErrorNone)
					{
						tex.texDef = def;

						tex.origImage = new unsigned char[tex.origArea.w * tex.origArea.h * PIXEL_SIZE];

						if(tex.origImage)
							memcpy(tex.origImage, tbuffer, tex.origArea.w * tex.origArea.h * PIXEL_SIZE);
					}
				}

				delete [] buffer;
			}
		}
		else
		{
			error = EOSErrorResourceOpen;
			fprintf(stderr, "Could not open file %s\n", def.texDef->filename.c_str());
		}
	}
	else
		error = EOSErrorNULL;

	return error;
}

EOSError TextureBuilder::loadTextureFiles(void)
{
	EOSError								error = EOSErrorNone;
	vector<TextureDefinitionPtr>::iterator	ref;
	TextureDetailPtr						tex_ptr;
	TextureDetail*							tex;

	ref = _textureDefs.begin();

	while(ref != _textureDefs.end())
	{
		tex = new TextureDetail;

		if(tex)
		{
			tex->master = true;
			error = loadTextureTGAFile(*ref, *tex);
		}
		else
		{
			fprintf(stderr, "Could not allocate memory\n");
			error = EOSErrorNoMemory;
		}

		if(error != EOSErrorNone)
			break;

		tex_ptr.texture = tex;

		_masterTextures.push_back(tex_ptr);
		ref++;
	}

	return error;
}

void TextureBuilder::defringeTextureImage(Uint8* image, Uint32 width, Uint32 height, Uint32 amount)
{
	Sint32			x, y;
	Uint8*	 		line;
	Uint32	 		lineLength;
	Uint32	 		lineOffset;
	Uint8*	 		dline;
	Uint32	 		dlineOffset;
	Float32			pixelRed = 0;
	Float32			pixelGreen = 0;
	Float32			pixelBlue = 0;
	Float32			pixelNum = 0;
	Float32			alpha;
	Sint32			subx, suby;
	Sint32			startx, endx;
	Sint32			starty, endy;

	if (amount == 0)
		return;

	lineLength = PIXEL_SIZE * width;

	for (y=0;y<height;y++)
	{
		line = &image[y * lineLength];

		lineOffset = 0;

		for (x=0;x<width;x++)
		{
			if (line[lineOffset + ALPHA_INDEX] == 0)
			{
				pixelRed = 0;
				pixelGreen = 0;
				pixelBlue = 0;
				pixelNum = 0;

				//	We have a transparent pixel, now do color weighting on it. We only consider non*alpha'd pixels
				starty = y - amount;
				endy = y + amount;

				startx = x - amount;
				endx = x + amount;

				if (starty < 0)
					starty = 0;

				if (startx < 0)
					startx = 0;

				if (endy > (height - 1))
					endy = height - 1;

				if (endx > (width - 1))
					endx = width - 1;

				for (suby=starty;suby<=endy;suby++)
				{
					dline = &image[suby * lineLength];
					dlineOffset = startx * PIXEL_SIZE;

					for (subx=startx;subx<=endx;subx++)
					{
						alpha = (Float32) dline[dlineOffset + ALPHA_INDEX];

						if (alpha != 0.0)
						{
							pixelRed += (Float32) dline[dlineOffset + RED_INDEX] * alpha / 255.0F;
							pixelGreen += (Float32) dline[dlineOffset + GREEN_INDEX] * alpha / 255.0F;
							pixelBlue += (Float32) dline[dlineOffset + BLUE_INDEX] * alpha / 255.0F;
							pixelNum += alpha / 255.0F;
						}

						dlineOffset += PIXEL_SIZE;
					}
				}

				if (pixelNum != 0.0)
				{
					pixelRed /= pixelNum;
					pixelGreen /= pixelNum;
					pixelBlue /= pixelNum;

					if (pixelRed > 255.0)
						pixelRed = 255.0;

					if (pixelGreen > 255.0)
						pixelGreen = 255.0;

					if (pixelBlue > 255.0)
						pixelBlue = 255.0;
				}
				else
					pixelRed = pixelGreen = pixelBlue = 0.0F;

				line[lineOffset + RED_INDEX] = (Uint8) pixelRed;
				line[lineOffset + GREEN_INDEX] = (Uint8) pixelGreen;
				line[lineOffset + BLUE_INDEX] = (Uint8) pixelBlue;
			}

			lineOffset += PIXEL_SIZE;
		}
	}
}

#define SATURATE_RW	0.3086
#define SATURATE_GW	0.6094
#define SATURATE_BW	0.0820

void TextureBuilder::saturateTextureImage(Uint8* image, Uint32 width, Uint32 height, Double64 param)
{
	Double64	a = ((1.0 - param) * SATURATE_RW) + param;
	Double64	b = ((1.0 - param) * SATURATE_RW);
	Double64	c = ((1.0 - param) * SATURATE_RW);

	Double64	d = ((1.0 - param) * SATURATE_GW);
	Double64	e = ((1.0 - param) * SATURATE_GW) + param;
	Double64	f = ((1.0 - param) * SATURATE_GW);

	Double64	g = ((1.0 - param) * SATURATE_BW);
	Double64	h = ((1.0 - param) * SATURATE_BW);
	Double64	i = ((1.0 - param) * SATURATE_BW) + param;

	Uint32		x, y;

	Double64	red;
	Double64	green;
	Double64	blue;

	Double64	tred;
	Double64	tgreen;
	Double64	tblue;

	Uint32		lineLength = width * PIXEL_SIZE;

	for (y=0;y<height;y++)
	{
		for (x=0;x<width;x++)
		{
			red = (Double64) image[y * lineLength + x * PIXEL_SIZE + RED_INDEX];
			green = (Double64) image[y * lineLength + x * PIXEL_SIZE + GREEN_INDEX];
			blue = (Double64) image[y * lineLength + x * PIXEL_SIZE + BLUE_INDEX];

			tred = (a * red + d * green + g * blue);

			tgreen = (b * red + e * green + h * blue);

			tblue = (c * red + f * green + i * blue);

			if (tred < 0.0)
				tred = 0.0;

			if (tred > 255.0)
			{
				tred = 255.0;
			}

			if (tgreen < 0.0)
				tgreen = 0.0;

			if (tgreen > 255.0)
			{
				tgreen = 255.0;
			}

			if (tblue < 0.0)
				tblue = 0.0;

			if (tblue > 255.0)
			{
				tblue = 255.0;
			}

			image[y * lineLength + x * PIXEL_SIZE + RED_INDEX] = (Uint8) tred;
			image[y * lineLength + x * PIXEL_SIZE + GREEN_INDEX] = (Uint8) tgreen;
			image[y * lineLength + x * PIXEL_SIZE + BLUE_INDEX] = (Uint32) tblue;
		}
	}
}

EOSError TextureBuilder::buildWorkTexture(TextureDetail& tex)
{
	EOSError 		error = EOSErrorNone;
	Uint32	 		padtop = 0; 
	Uint32			padbottom = 0; 
	Uint32	 		padleft = 0; 
	Uint32			padright = 0; 
	Uint8*	 		line;
	Uint32	 		lineLength;
	Uint32	 		lineOffset;
	Uint8*	 		origLine;
	Uint32	 		origLineLength;
	Uint32	 		origLineOffset;
	Uint32			x, y;
	ColorIntRGBA	color;
	Boolean			defringed = false;
	Boolean			setcolor = false;

	if (tex.workImage)
		delete tex.workImage;

	if (_padEdgeColor.top.amountMode == PadModeForceOn)
		padtop = _padEdgeColor.top.amount;
	else if (_padEdgeColor.top.amountMode == PadModeIgnore)
		padtop = 0;
	else if (tex.texDef.texDef->padTop)
		padtop = *tex.texDef.texDef->padTop;
	else
		padtop = _padEdgeColor.top.amount;

	if (_padEdgeColor.bottom.amountMode == PadModeForceOn)
		padbottom = _padEdgeColor.bottom.amount;
	else if (_padEdgeColor.bottom.amountMode == PadModeIgnore)
		padbottom = 0;
	else if (tex.texDef.texDef->padTop)
		padbottom = *tex.texDef.texDef->padTop;
	else
		padbottom = _padEdgeColor.bottom.amount;

	if (_padEdgeColor.left.amountMode == PadModeForceOn)
		padleft = _padEdgeColor.left.amount;
	else if (_padEdgeColor.left.amountMode == PadModeIgnore)
		padleft = 0;
	else if (tex.texDef.texDef->padTop)
		padleft = *tex.texDef.texDef->padTop;
	else
		padleft = _padEdgeColor.left.amount;

	if (_padEdgeColor.right.amountMode == PadModeForceOn)
		padright = _padEdgeColor.right.amount;
	else if (_padEdgeColor.right.amountMode == PadModeIgnore)
		padright = 0;
	else if (tex.texDef.texDef->padTop)
		padright = *tex.texDef.texDef->padTop;
	else
		padright = _padEdgeColor.right.amount;

	if (tex.texDef.texDef->priority)
	{
		if (*tex.texDef.texDef->priority == true)
		{
			tex.origArea.priority = true;
			tex.workArea.priority = true;
		}
	}

	if (_saturation.saturate || (tex.texDef.texDef->saturation && *tex.texDef.texDef->saturation))
	{
		Double64	param;

		if (tex.texDef.texDef->saturation)
			param = *tex.texDef.texDef->saturation;
		else
			param = _saturation.saturationParam;

		saturateTextureImage(tex.origImage, tex.origArea.w, tex.origArea.h, param);
	}

	tex.origArea.x = padleft;
	tex.origArea.y = padtop;

	tex.workArea.x = 0;
	tex.workArea.y = 0;
	tex.workArea.w = tex.origArea.w + padleft + padright;
	tex.workArea.h = tex.origArea.h + padtop + padbottom;
	tex.workArea.area = tex.workArea.w * tex.workArea.h;

	tex.workImage = new unsigned char[tex.workArea.w * tex.workArea.h * PIXEL_SIZE];

	if (tex.workImage)
	{
		memset(tex.workImage, 0, tex.workArea.w * tex.workArea.h * PIXEL_SIZE);

		lineLength = PIXEL_SIZE * tex.workArea.w;
		origLineLength = tex.origArea.w * PIXEL_SIZE;

		//	Now fill our image, making truly transparent first black
		for (y=padtop;y<tex.workArea.h-padbottom;y++)
		{
			line = &tex.workImage[y * lineLength];
			origLine = &tex.origImage[(y - padtop) * origLineLength];

			lineOffset = padleft * PIXEL_SIZE;
			origLineOffset = 0;

			for (x=0;x<tex.origArea.w;x++)
			{
				if (origLine[origLineOffset + ALPHA_INDEX] == 0)
				{
					line[lineOffset + ALPHA_INDEX] = 0;
					line[lineOffset + RED_INDEX] = 0;
					line[lineOffset + GREEN_INDEX] = 0;
					line[lineOffset + BLUE_INDEX] = 0;
				}
				else
				{
					line[lineOffset + ALPHA_INDEX] = origLine[origLineOffset + ALPHA_INDEX];
					line[lineOffset+ RED_INDEX] = origLine[origLineOffset + RED_INDEX];
					line[lineOffset+ GREEN_INDEX] = origLine[origLineOffset + GREEN_INDEX];
					line[lineOffset+ BLUE_INDEX] = origLine[origLineOffset + BLUE_INDEX];
				}

				lineOffset += PIXEL_SIZE;
				origLineOffset += PIXEL_SIZE;
			}
		}

		//	Defringe if needed
		if (_alphaDefringe.mode == AlphaDefringeModeOn || (tex.texDef.texDef->alphaDefringe && *tex.texDef.texDef->alphaDefringe > 0 && _alphaDefringe.mode != AlphaDefringeModeIgnore))
		{
			Uint32	amount = _alphaDefringe.amount;

			if (tex.texDef.texDef->alphaDefringe)
				amount = *tex.texDef.texDef->alphaDefringe;

			defringeTextureImage(tex.workImage, tex.workArea.w, tex.workArea.h, amount);
			defringed = true;
		}

		//	Now fill in transparent colors where needed RGBA == (0, 0, 0, 0)
		if (_transColorMode == TransparentColorModeForceOn)
			color = _transColor;
		else if (_transColorMode == TransparentColorModeIgnore)
			color.r = color.g = color.b = color.a = 0;
		else if (tex.texDef.texDef->transparentColor)
			color = *tex.texDef.texDef->transparentColor;
		else
			color = _transColor;

		for (y=padtop;y<tex.workArea.h-padbottom;y++)
		{
			line = &tex.workImage[y * lineLength];

			lineOffset = padleft * PIXEL_SIZE;

			for (x=0;x<tex.origArea.w;x++)
			{
				setcolor = false;

				if (defringed)
				{
					if (line[lineOffset + ALPHA_INDEX] == 0)
					{
						if (line[lineOffset + RED_INDEX] == 0 && line[lineOffset + GREEN_INDEX] == 0 && line[lineOffset + BLUE_INDEX] == 0)
							setcolor = true;
					}
				}
				else if (line[lineOffset + ALPHA_INDEX] == 0)
					setcolor = true;

				if (setcolor)
				{
					line[lineOffset + ALPHA_INDEX] = color.a;
					line[lineOffset + RED_INDEX] = color.r;
					line[lineOffset + GREEN_INDEX] = color.g;
					line[lineOffset + BLUE_INDEX] = color.b;
				}

				lineOffset += PIXEL_SIZE;
				origLineOffset += PIXEL_SIZE;
			}
		}

		//	Fill in our pad colors
		if (_padEdgeColor.top.colorMode == PadColorModeForceOn)
			color = _padEdgeColor.top.color;
		else if (_padEdgeColor.right.amountMode == PadColorModeIgnore)
			color.r = color.g = color.b = color.a = 0;
		else if (tex.texDef.texDef->padColorTop)
			color = *tex.texDef.texDef->padColorTop;
		else
			color = _padEdgeColor.top.color;

		for (y=0;y<padtop;y++)
		{
			line = &tex.workImage[y * lineLength];
			lineOffset = 0;

			for (x=0;x<tex.workArea.w;x++)
			{
				setcolor = false;

				if (defringed)
				{
					if (line[lineOffset + ALPHA_INDEX] == 0)
					{
						if (line[lineOffset + RED_INDEX] == 0 && line[lineOffset + GREEN_INDEX] == 0 && line[lineOffset + BLUE_INDEX] == 0)
							setcolor = true;
					}
				}
				else if (line[lineOffset + ALPHA_INDEX] == 0)
					setcolor = true;

				if (setcolor)
				{
					line[lineOffset + ALPHA_INDEX] = color.a;
					line[lineOffset + RED_INDEX] = color.r;
					line[lineOffset + GREEN_INDEX] = color.g;
					line[lineOffset + BLUE_INDEX] = color.b;
				}

				lineOffset += PIXEL_SIZE;
			}
		}

		if (_padEdgeColor.bottom.colorMode == PadColorModeForceOn)
			color = _padEdgeColor.bottom.color;
		else if (_padEdgeColor.right.amountMode == PadColorModeIgnore)
			color.r = color.g = color.b = color.a = 0;
		else if (tex.texDef.texDef->padColorTop)
			color = *tex.texDef.texDef->padColorTop;
		else
			color = _padEdgeColor.bottom.color;

		for (y=padtop+tex.origArea.h;y<tex.workArea.h;y++)
		{
			line = &tex.workImage[y * lineLength];
			lineOffset = 0;

			for (x=0;x<tex.workArea.w;x++)
			{
				setcolor = false;

				if (defringed)
				{
					if (line[lineOffset + ALPHA_INDEX] == 0)
					{
						if (line[lineOffset + RED_INDEX] == 0 && line[lineOffset + GREEN_INDEX] == 0 && line[lineOffset + BLUE_INDEX] == 0)
							setcolor = true;
					}
				}
				else if (line[lineOffset + ALPHA_INDEX] == 0)
					setcolor = true;

				if (setcolor)
				{
					line[lineOffset + ALPHA_INDEX] = color.a;
					line[lineOffset + RED_INDEX] = color.r;
					line[lineOffset + GREEN_INDEX] = color.g;
					line[lineOffset + BLUE_INDEX] = color.b;
				}

				lineOffset += PIXEL_SIZE;
			}
		}

		if (_padEdgeColor.left.colorMode == PadColorModeForceOn)
			color = _padEdgeColor.left.color;
		else if (_padEdgeColor.right.amountMode == PadColorModeIgnore)
			color.r = color.g = color.b = color.a = 0;
		else if (tex.texDef.texDef->padColorTop)
			color = *tex.texDef.texDef->padColorTop;
		else
			color = _padEdgeColor.left.color;

		for (y=0;y<tex.workArea.h;y++)
		{
			line = &tex.workImage[y * lineLength];
			lineOffset = 0;

			for (x=0;x<padleft;x++)
			{
				setcolor = false;

				if (defringed)
				{
					if (line[lineOffset + ALPHA_INDEX] == 0)
					{
						if (line[lineOffset + RED_INDEX] == 0 && line[lineOffset + GREEN_INDEX] == 0 && line[lineOffset + BLUE_INDEX] == 0)
							setcolor = true;
					}
				}
				else if (line[lineOffset + ALPHA_INDEX] == 0)
					setcolor = true;

				if (setcolor)
				{
					line[lineOffset + ALPHA_INDEX] = color.a;
					line[lineOffset + RED_INDEX] = color.r;
					line[lineOffset + GREEN_INDEX] = color.g;
					line[lineOffset + BLUE_INDEX] = color.b;
				}

				lineOffset += PIXEL_SIZE;
			}
		}

		if (_padEdgeColor.right.colorMode == PadColorModeForceOn)
			color = _padEdgeColor.right.color;
		else if (_padEdgeColor.right.amountMode == PadColorModeIgnore)
			color.r = color.g = color.b = color.a = 0;
		else if (tex.texDef.texDef->padColorTop)
			color = *tex.texDef.texDef->padColorTop;
		else
			color = _padEdgeColor.right.color;

		for (y=0;y<tex.workArea.h;y++)
		{
			line = &tex.workImage[y * lineLength];
			lineOffset = (padleft+tex.origArea.w) * PIXEL_SIZE;

			for (x=padleft+tex.origArea.w;x<tex.workArea.w;x++)
			{
				setcolor = false;

				if (defringed)
				{
					if (line[lineOffset + ALPHA_INDEX] == 0)
					{
						if (line[lineOffset + RED_INDEX] == 0 && line[lineOffset + GREEN_INDEX] == 0 && line[lineOffset + BLUE_INDEX] == 0)
							setcolor = true;
					}
				}
				else if (line[lineOffset + ALPHA_INDEX] == 0)
					setcolor = true;

				if (setcolor)
				{
					line[lineOffset + ALPHA_INDEX] = color.a;
					line[lineOffset + RED_INDEX] = color.r;
					line[lineOffset + GREEN_INDEX] = color.g;
					line[lineOffset + BLUE_INDEX] = color.b;
				}

				lineOffset += PIXEL_SIZE;
			}
		}
	}
	else
		error = EOSErrorNoMemory;

	return error;
}

EOSError TextureBuilder::buildWorkTextures(void)
{
	EOSError							error = EOSErrorNone;
	vector<TextureDetailPtr>::iterator	tex;

	tex = _masterTextures.begin();

	while(tex != _masterTextures.end())
	{
		if (tex->texture)
		{
			error = buildWorkTexture(*tex->texture);
		}

		if (error != EOSErrorNone)
			break;

		tex++;
	}

	return error;
}

EOSError TextureBuilder::validateBaseConfig(void)
{
	EOSError	error = EOSErrorNone;

	_horizBiasedTexSheet.setTexSheetWidth(_tsheet_width);
	_horizBiasedTexSheet.setTexSheetHeight(_tsheet_height);
	_horizBiasedTexSheet.setHalfTexelMode(_halfTexelMode);

	_vertBiasedTexSheet.setTexSheetWidth(_tsheet_width);
	_vertBiasedTexSheet.setTexSheetHeight(_tsheet_height);
	_vertBiasedTexSheet.setHalfTexelMode(_halfTexelMode);

	_horizBiasedTexSheet.setTextures(_masterTextures, TextureAreaBiasWidth);
	_vertBiasedTexSheet.setTextures(_masterTextures, TextureAreaBiasHeight);

	if (_horizBiasedTexSheet.isSourceAreaSufficient() == false)
		error = EOSErrorOutOfBounds;
	else if (_vertBiasedTexSheet.isSourceAreaSufficient() == false)
		error = EOSErrorOutOfBounds;

	return error;
}

EOSError TextureBuilder::generateBestTextureSheet(void)
{
	EOSError	error = EOSErrorNone;
	EOSError	horizError;
	EOSError	vertError;

	horizError = _horizBiasedTexSheet.buildTextureSheet();
	vertError = _vertBiasedTexSheet.buildTextureSheet();

	if (horizError == EOSErrorNone)
	{
		error = _horizBiasedTexSheet.exportTextureSheet(_exportPrefix, _exportName, _exportID, true, true, true);
	}
	else if (vertError == EOSErrorNone)
	{
		error = _vertBiasedTexSheet.exportTextureSheet(_exportPrefix, _exportName, _exportID, true, true, true);
	}

	return error;
}

