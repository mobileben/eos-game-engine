/******************************************************************************
 *
 * File: TextureBuilder.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture builder. Builds texture atlases
 * 
 *****************************************************************************/

#ifndef __TEXTUREBUILDER_H__
#define __TEXTUREBUILDER_H__

#include "Platform.h"
#include "Graphics.h"
#include "TextureAtlas.h"
#include "toolkit\GraphicsToolKit.h"
#include "TexturePropsDBXML.h"

#include <wchar.h>
#include <iterator>
#include <vector>
#include <map>
#include <algorithm>

typedef enum
{
	TransparentColorModeDefault = 0,
	TransparentColorModeForceOn,
	TransparentColorModeIgnore,
} TransparentColorMode;

typedef enum
{
	PadModeDefault = 0,
	PadModeForceOn,
	PadModeIgnore,
} PadMode;

typedef enum
{
	PadColorModeDefault = 0,
	PadColorModeForceOn,
	PadColorModeIgnore,
} PadColorMode;

typedef enum
{
	HalfTexelModeOff = 0,
	HalfTexelModeOn,
	HalfTexelModeIgnore,
} HalfTexelMode;

typedef enum
{
	AlphaDefringeModeOff = 0,
	AlphaDefringeModeOn,
	AlphaDefringeModeIgnore,
} AlphaDefringeMode;

typedef enum
{
	TextureAreaBiasWidth = 0,
	TextureAreaBiasHeight
} TextureAreaBias;

typedef struct
{
	Uint32	r;
	Uint32 	g;
	Uint32	b;
	Uint32	a;
} ColorIntRGBA;

typedef struct
{
	PadColorMode	colorMode;
	ColorIntRGBA	color;
	PadMode			amountMode;
	Uint32			amount;
} PadColor;

typedef struct
{
	AlphaDefringeMode	mode;
	Uint32				amount;
} AlphaDefringe;

typedef struct
{
	Boolean				saturate;
	Float32				saturationParam;
} Saturation;

typedef struct
{
	TextureAreaBias	bias;
	Boolean			priority;
	Sint32			x;
	Sint32			y;
	Sint32			w;
	Sint32			h;
	Sint32			area;
} TextureArea2;

typedef struct
{
	PadColor	top;
	PadColor	bottom;
	PadColor	left;
	PadColor	right;
} PadEdgeColor;

class TextureDefinition
{
public:	//	All made public for convenience
	int				id;
	string			label;
	string			filename;
	ColorIntRGBA*	transparentColor;
	ColorIntRGBA*	padColorTop;
	ColorIntRGBA*	padColorBottom;
	ColorIntRGBA*	padColorLeft;
	ColorIntRGBA*	padColorRight;
	Uint32*			padTop;
	Uint32*			padBottom;
	Uint32*			padLeft;
	Uint32*			padRight;
	Boolean*		halfTexel;
	Boolean*		priority;
	Uint32*			alphaDefringe;
	Float32*		saturation;

	TextureDefinition();
	~TextureDefinition();
};

typedef struct
{
	TextureDefinition*	texDef;
} TextureDefinitionPtr;

class TextureDetail
{
public:	//	All made public for convenience
	bool					placed;
	bool					master;
	TextureArea2			origArea;
	TextureArea2			workArea;
	TextureDefinitionPtr	texDef;
	Uint32					vertices[4];
	Uint32					uvs[4];
	unsigned char*			origImage;	//	Original source data
	unsigned char*			workImage;	//	Modified after all padding and other operations

	TextureDetail();
	~TextureDetail();

	void copy(TextureDetail& src);
};

typedef struct
{
	TextureDetail*	texture;
} TextureDetailPtr;

class SubTexNode
{
public:	//	All made public for convenience
	TextureArea2	area;
	TextureDetail*	texture;
	SubTexNode*		child[2];

	SubTexNode();
	~SubTexNode();
};

class PackedTextureSheet
{
private:
	vector<TextureDetailPtr>		_textures;
	Sint32							_sourceTexArea;

	vector<Point2D>					_vertices;
	vector<Point2D>					_uvs;

	Sint32							_tsheet_width;
	Sint32							_tsheet_height;

	SubTexNode						_tsheetRoot;

	Uint8*							_image;

	HalfTexelMode					_halfTexelMode;

	TextureAtlasHeader				_exportHeader;
	SpriteTextureRefRAW*			_exportSubTex;
	Point2D*						_exportVertices;
	Point2D*						_exportUVs;
	Char*							_exportNames;
	Uint32*							_exportNamesOffsets;
	Uint32							_exportNamesSize;

	Boolean							_verbose;

	void							cleanupExportData(void);

	void							sortTexturesByDefID(void);

	EOSError						addExportName(Char* name, Char** nameList, Uint32& size, Uint32& offset);

	Uint32 							findVertice(Point2D& pt);
	Uint32 							findUV(Point2D& pt);

	void							kdtreeDestroy(SubTexNode* node);
	SubTexNode*						kdtreeInsert(TextureDetail* detail, SubTexNode* node);

	EOSError						instantiateTextureSheet(void);

	EOSError						generateGLCoordinates(TextureDetail* detail);

	EOSError						generateGLCoordinates(void);

	EOSError						generateExportData(Char* name, Sint32 texID);

	EOSError						exportTGA(Char* filename);
	EOSError						exportAtlasXML(Char* filename);
	EOSError						exportAtlasBIN(Char* filename);

public:
	PackedTextureSheet();
	~PackedTextureSheet();

	void				setTextures(vector<TextureDetailPtr> textures, TextureAreaBias bias);

	inline void			setTexSheetWidth(Uint32 width) { _tsheet_width = width; }
	inline Uint32		getTexSheetWidth(void) { return _tsheet_width; }

	inline void			setTexSheetHeight(Uint32 height) { _tsheet_height = height; }
	inline Uint32		getTexSheetHeight(void) { return _tsheet_height; }

	inline HalfTexelMode	getHalfTexelMode(void) { return _halfTexelMode; }
	inline void				setHalfTexelMode(HalfTexelMode half) { _halfTexelMode = half; }

	inline Boolean		getVerbose(void) { return _verbose; }
	inline void			setVerbose(Boolean verbose) { _verbose = verbose; }

	Boolean				isSourceAreaSufficient(void);

	void				sortTextures(void);

	EOSError			buildTextureSheet(void);

	EOSError			exportTextureSheet(Char* prefix, Char* name, Sint32 texID, Boolean tga, Boolean atlas, Boolean xml);
};

class TextureBuilder
{
private:
	vector<TextureDefinitionPtr>	_textureDefs;
	vector<TextureDetailPtr>		_masterTextures;

	PackedTextureSheet				_horizBiasedTexSheet;
	PackedTextureSheet				_vertBiasedTexSheet;

	Char*							_exportName;
	Char*							_exportPrefix;
	Sint32							_exportID;

	Uint32							_textureCount;

	Boolean							_verbose;

	Sint32							_tsheet_width;
	Sint32							_tsheet_height;

	ColorIntRGBA					_transColor;

	PadEdgeColor					_padEdgeColor;

	TransparentColorMode			_transColorMode;
	HalfTexelMode					_halfTexelMode;
	AlphaDefringe					_alphaDefringe;
	Saturation						_saturation;

	Boolean							_power2;
	Boolean							_smallest;
	Boolean							_optimize;

	Boolean		isEmptyLine(string line);
	Sint32		readLine(Uint8* buffer, Uint8* end, string& str);

	void		defringeTextureImage(Uint8* image, Uint32 width, Uint32 height, Uint32 amount);
	void		saturateTextureImage(Uint8* image, Uint32 width, Uint32 height, Double64 param);

	Boolean 	parseLineForTextureDef(string line, TextureDefinitionPtr& def, EOSError& error);
	EOSError 	parseTextureDefFile(Uint8* buffer, Uint32 size);

	EOSError 	loadTextureTGAFile(TextureDefinitionPtr& def, TextureDetail& tex);

	EOSError	buildWorkTexture(TextureDetail& tex);

public:
	TextureBuilder();
	~TextureBuilder();

	inline void			setVerbose(Boolean verbose) { _verbose = verbose; }

	inline void			setTexSheetWidth(Uint32 width) { _tsheet_width = width; }
	inline Uint32		getTexSheetWidth(void) { return _tsheet_width; }

	inline void			setTexSheetHeight(Uint32 height) { _tsheet_height = height; }
	inline Uint32		getTexSheetHeight(void) { return _tsheet_height; }

	inline void			getTransparentColor(ColorIntRGBA& color) { color = _transColor; }
	inline void			setTransparentColor(ColorIntRGBA& color) { _transColor = color; }

	inline void			getPadEdgeColor(PadEdgeColor& pad) { pad = _padEdgeColor; }
	inline void			setPadEdgeColor(PadEdgeColor& pad) { _padEdgeColor = pad; }

	inline void					setTransparentColorMode(TransparentColorMode mode) { _transColorMode = mode; }
	inline TransparentColorMode	getTransparentColorMode(void) { return _transColorMode; }

	inline void				setHalfTexelMode(HalfTexelMode mode) { _halfTexelMode = mode; }
	inline HalfTexelMode	getHalfTexelMode(void) { return _halfTexelMode; }

	inline void				setAlphaDefringe(AlphaDefringe defringe) { _alphaDefringe = defringe; }
	inline AlphaDefringe	getAlphaDefringe(void) { return _alphaDefringe; }

	inline void				setSaturation(Saturation saturation) { _saturation = saturation; }
	inline Saturation		getSaturation(void) { return _saturation; }

	inline void			setPower2(Boolean pow2) { _power2 = pow2; }
	inline Boolean		getPower2(void) { return _power2; }

	inline void			setSmallest(Boolean smallest) { _smallest = smallest; }
	inline Boolean		getSmallest(void) { return _smallest; }

	inline void			setOptimize(Boolean opt) { _optimize = opt; }
	inline Boolean		getOptimize(void) { return _optimize; }

	inline Char*		getExportName(void) { return _exportName; }
	void				setExportName(Char* name);

	inline Char*		getExportPrefix(void) { return _exportPrefix; }
	void				setExportPrefix(Char* name);

	inline Uint32		getExportID(void) { return _exportID; }
	inline void			setExportID(Sint32 expID) { _exportID = expID; }

	EOSError			assignTexPropToTextureDefinition(XMLElementSubTex* subtex);	
	TextureDefinition*	findTextureDefinitionFromLabel(string label);

	EOSError			loadTextureDefFile(char* filename);
	EOSError			loadTextureFiles(void);

	EOSError			buildWorkTextures(void);

	EOSError			validateBaseConfig(void);

	EOSError			generateBestTextureSheet(void);
};

#endif /* __TEXTUREBUILDER_H__ */
