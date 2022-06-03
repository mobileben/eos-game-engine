/******************************************************************************
 *
 * File: texbuilder.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture builder. Builds texture atlases
 * 
 *****************************************************************************/

#include "Platform.h"
#include "toolkit\GraphicsToolKit.h"
#include "TextureAtlas.h"
#include "EOSError.h"
#include "File.h"
#include "Endian.h"
#include "TextureBuilder.h"
#include "TexturePropsDBXML.h"

#include <wchar.h>
#include <iterator>
#include <vector>
#include <map>
#include <algorithm>

char*	_app_name = "texbuilder";
Boolean	_verbose = true;
bool	_comments = false;
Uint32	_versionMajor = 0;
Uint32	_versionMinor = 1;

typedef enum
{
	ArgumentDataTypeFlag = 0,
	ArgumentDataTypeInt,
	ArgumentDataTypeRGBA,
	ArgumentDataTypeFloat,
	ArgumentDataTypeString,
	ArgumentDataTypeLast,
} ArgumentDataType;

typedef enum
{
	ArgumentNameTMPL = 0,
	ArgumentNameXML,
	ArgumentNameProperties,
	ArgumentNameHalfTexel,
	ArgumentNameIgnoreHalfTexel,
	ArgumentNamePadAllEdges,
	ArgumentNamePadTopEdge,
	ArgumentNamePadBottomEdge,
	ArgumentNamePadLeftEdge,
	ArgumentNamePadRightEdge,
	ArgumentNameForcePadAllEdges,
	ArgumentNameForcePadTopEdge,
	ArgumentNameForcePadBottomEdge,
	ArgumentNameForcePadLeftEdge,
	ArgumentNameForcePadRightEdge,
	ArgumentNameIgnorePadAllEdges,
	ArgumentNameIgnorePadTopEdge,
	ArgumentNameIgnorePadBottomEdge,
	ArgumentNameIgnorePadLeftEdge,
	ArgumentNameIgnorePadRightEdge,
	ArgumentNamePower2,
	ArgumentNameWidth,
	ArgumentNameHeight,
	ArgumentNameSmallest,
	ArgumentNameTransparentColor,
	ArgumentNamePadColor,
	ArgumentNameAllowRotates,
	ArgumentNameOptimize,
	ArgumentNameAlphaDefringe,
	ArgumentNameIgnoreAlphaDefringe,
	ArgumentNameExportPrefix,
	ArgumentNameExportName,
	ArgumentNameExportID,
	ArgumentNameSaturation,
	ArgumentNameLast,
} ArgumentName;

typedef Sint32 (*ArgumentDataTypeHandler)(int argsLeft, char* argv[], void* data);
typedef Sint32 (*ArgumentNameHandler)(ArgumentName name, void** var, void* data);

typedef struct
{
	ArgumentName		name;
	Char*				nameString;
	ArgumentDataType	dataType;
	void*				data;
	Uint32				flagValue;
	ArgumentNameHandler	handler;
} ArgumentList;

TransparentColorMode	_transColorMode = TransparentColorModeDefault;
HalfTexelMode			_halfTexelMode = HalfTexelModeOff;

AlphaDefringe			_alphaDefringe = 
{
	AlphaDefringeModeOff,
	0
};

Saturation				_saturation = 
{
	false,
	1.0
};

ColorIntRGBA			_transColor = { 0, 0, 0, 0 };

PadEdgeColor			_padEdgeColor = 
{
	{	//	Top
		PadColorModeDefault,
		{ 0, 0, 0, 0 },
		PadModeDefault,
		0
	},
	{	//	Bottom
		PadColorModeDefault,
		{ 0, 0, 0, 0 },
		PadModeDefault,
		0
	},
	{	//	Left
		PadColorModeDefault,
		{ 0, 0, 0, 0 },
		PadModeDefault,
		0
	},
	{	//	Right
		PadColorModeDefault,
		{ 0, 0, 0, 0 },
		PadModeDefault,
		0
	},
};

Sint32 FlagArgumentDataTypeHandler(int argsLeft, char* argv[], void* data)
{
	Sint32	consumed = -1;

	if (argsLeft >= 0)
		consumed = 0;

	return consumed;
}

Sint32 IntArgumentDataTypeHandler(int argsLeft, char* argv[], void* data)
{
	Sint32	consumed = -1;
	Sint32*	intData = (Sint32*) data;

	if (argsLeft >= 1)
	{
		if (argv[0][0] >= '0' && argv[0][0] <= '9')
			*intData = atoi(argv[0]);
		else
			return -1;	//	Error

		consumed = 1;
	}

	return consumed;
}

Sint32 RGBAArgumentDataTypeHandler(int argsLeft, char* argv[], void* data)
{
	Sint32			consumed = -1;
	Uint32*			colorData = (Uint32*) data;
	Sint32			i;

	if (argsLeft >= 4)
	{
		for (i=0;i<4;i++)
		{
			if (argv[i][0] >= '0' && argv[i][0] <= '9')
			{
				colorData[i] = atoi(argv[i]);
			}
			else
				return -1;	//	Error
		}

		consumed = 4;
	}

	return consumed;
}

Sint32 FloatArgumentDataTypeHandler(int argsLeft, char* argv[], void* data)
{
	Sint32		consumed = -1;
	Float32*	floatData = (Float32*) data;

	if (argsLeft >= 1)
	{
		if (argv[0][0] != '-' && !(argv[0][0] >= 'A' && argv[0][0] <= 'Z') && !(argv[0][0] >= 'a' && argv[0][0] >= 'z'))
			*floatData = atof(argv[0]);
		else
			return -1;	//	Error

		consumed = 1;
	}

	return consumed;
}

Sint32 StringArgumentDataTypeHandler(int argsLeft, char* argv[], void* data)
{
	Sint32	consumed = -1;
	Char*	str = (Char*) data;

	if (argsLeft >= 1)
	{
		strcpy(str, argv[0]);
		consumed = 1;
	}

	return consumed;
}

Uint32	_halfTexelModifiers = 0;
Uint32	_alphaDefringeModifiers = 0;
Uint32	_padEdgeModifiers = 0;
Uint32	_padTopModifiers = 0;
Uint32	_padBottomModifiers = 0;
Uint32	_padLeftModifiers = 0;
Uint32	_padRightModifiers = 0;

Char*	_tmplName = NULL;
Char*	_xmlName = NULL;
Char*	_propsName = NULL;
Char*	_exportName = NULL;
Char*	_exportPrefix = NULL;	
Boolean	_power2 = false;
Boolean	_smallest = false;
Boolean	_allowRotate = false;
Boolean	_optimize = false;
Sint32	_exportID = 0;
Uint32	_tsheet_width = 0;
Uint32	_tsheet_height = 0;

ArgumentDataTypeHandler	_argDataTypeHandlers[ArgumentDataTypeLast] = 
{
	FlagArgumentDataTypeHandler,
	IntArgumentDataTypeHandler,
	RGBAArgumentDataTypeHandler,
	FloatArgumentDataTypeHandler,
	StringArgumentDataTypeHandler,
};

Char			_workString[512];
Uint32			_workFlag;
Sint32			_workInt;
ColorIntRGBA	_workColor;
Float32			_workFloat;

void*	_argDataTypeWorkData[ArgumentDataTypeLast] = 
{
	&_workFlag,
	&_workInt,
	&_workColor,
	&_workFloat,
	_workString
};

Sint32 HalfTexelArgumentNameHandler(ArgumentName name, void** var, void* data)
{
	HalfTexelMode*	mode = *(HalfTexelMode**) var;
	HalfTexelMode*	value = (HalfTexelMode*) data;

	*mode = *value;

	_halfTexelModifiers++;

	return 1;
}

Sint32 AlphaDefringeIntArgumentNameHandler(ArgumentName name, void** var, void* data)
{
	AlphaDefringeMode	mode = AlphaDefringeModeOff;
	Sint32				done = 0;
	AlphaDefringe*		defringe = *(AlphaDefringe**) var;
	Sint32*				amount = (Sint32*) data;

	switch (name)
	{
		case ArgumentNameAlphaDefringe:
			mode = AlphaDefringeModeOn;
			done = 1;
			break;
	}

	if (done)
	{
		defringe->mode = mode;
		defringe->amount = *amount;

		_alphaDefringeModifiers++;
	}

	return done;
}

Sint32 AlphaDefringeBooleanArgumentNameHandler(ArgumentName name, void** var, void* data)
{
	AlphaDefringe*		defringe = *(AlphaDefringe**) var;
	AlphaDefringeMode*	value = (AlphaDefringeMode*) data;

	defringe->mode = *value;
	defringe->amount = 0;

	_alphaDefringeModifiers++;

	return 1;
}

Sint32 SaturationFloatArgumentNameHandler(ArgumentName name, void** var, void* data)
{
	Boolean				on = false;
	Sint32				done = 0;
	Saturation*			saturation = *(Saturation**) var;
	Float32*			amount = (Float32*) data;

	switch (name)
	{
		case ArgumentNameSaturation:
			on = true;
			done = 1;
			break;
	}

	if (done)
	{
		saturation->saturate = on;
		saturation->saturationParam = *amount;
	}

	return done;
}

Sint32 PadEdgeArgumentNameHandler(ArgumentName name, void** var, void* data)
{
	Sint32			done = 0;
	PadMode			mode = PadModeDefault;
	PadEdgeColor*	pad = *(PadEdgeColor**) var;
	Sint32*			padAmount = (Sint32*) data;

	switch (name)
	{
		case ArgumentNamePadAllEdges:
			mode = PadModeDefault;
			done = 1;
			break;

		case ArgumentNameForcePadAllEdges:
			mode = PadModeForceOn;
			done = 1;
			break;

		case ArgumentNameIgnorePadAllEdges:
			mode = PadModeIgnore;
			done = 1;
			break;
	}

	if (done)
	{
		pad->top.amountMode = mode;
		pad->top.amount = *padAmount;

		pad->bottom.amountMode = mode;
		pad->bottom.amount = *padAmount;

		pad->left.amountMode = mode;
		pad->left.amount = *padAmount;

		pad->right.amountMode = mode;
		pad->right.amount = *padAmount;

		_padEdgeModifiers++;
	}

	return done;
}

Sint32 PadEdgeColorArgumentNameHandler(ArgumentName name, void** var, void* data)
{
	Sint32			done = 0;
	PadColorMode	mode = PadColorModeDefault;
	PadEdgeColor*	pad = *(PadEdgeColor**) var;
	ColorIntRGBA*	color = NULL;
	Sint32*			padAmount = (Sint32*) data;

	switch (name)
	{
		case ArgumentNamePadColor:
			mode = PadColorModeDefault;
			color = (ColorIntRGBA*) data;
			done = 1;
			break;
	}

	if (done)
	{
		if (color)
		{
			pad->top.colorMode = mode;
			pad->top.color.r = color->r;
			pad->top.color.g = color->g;
			pad->top.color.b = color->b;
			pad->top.color.a = color->a;

			pad->bottom.colorMode = mode;
			pad->bottom.color.r = color->r;
			pad->bottom.color.g = color->g;
			pad->bottom.color.b = color->b;
			pad->bottom.color.a = color->a;

			pad->left.colorMode = mode;
			pad->left.color.r = color->r;
			pad->left.color.g = color->g;
			pad->left.color.b = color->b;
			pad->left.color.a = color->a;

			pad->right.colorMode = mode;
			pad->right.color.r = color->r;
			pad->right.color.g = color->g;
			pad->right.color.b = color->b;
			pad->right.color.a = color->a;
		}
	}

	return done;
}

Sint32 PadColorArgumentNameHandler(ArgumentName name, void** var, void* data)
{
	Sint32			done = 0;
	PadMode			mode = PadModeDefault;
	PadColor*		pad = *(PadColor**) var;
	Sint32			padAmount = *(Sint32*) data;

	switch (name)
	{
		case ArgumentNamePadTopEdge:
			mode = PadModeDefault;
			_padTopModifiers++;
			done = 1;
			break;

		case ArgumentNamePadBottomEdge:
			mode = PadModeDefault;
			_padBottomModifiers++;
			done = 1;
			break;

		case ArgumentNamePadLeftEdge:
			mode = PadModeDefault;
			_padLeftModifiers++;
			done = 1;
			break;

		case ArgumentNamePadRightEdge:
			mode = PadModeDefault;
			_padRightModifiers++;
			done = 1;
			break;

		case ArgumentNameForcePadTopEdge:
			mode = PadModeForceOn;
			_padTopModifiers++;
			done = 1;
			break;

		case ArgumentNameForcePadBottomEdge:
			mode = PadModeForceOn;
			_padBottomModifiers++;
			done = 1;
			break;

		case ArgumentNameForcePadLeftEdge:
			mode = PadModeForceOn;
			_padLeftModifiers++;
			done = 1;
			break;

		case ArgumentNameForcePadRightEdge:
			mode = PadModeForceOn;
			_padRightModifiers++;
			done = 1;
			break;

		case ArgumentNameIgnorePadTopEdge:
			mode = PadModeIgnore;
			_padTopModifiers++;
			padAmount = 0;
			done = 1;
			break;

		case ArgumentNameIgnorePadBottomEdge:
			mode = PadModeIgnore;
			_padBottomModifiers++;
			padAmount = 0;
			done = 1;
			break;

		case ArgumentNameIgnorePadLeftEdge:
			mode = PadModeIgnore;
			_padLeftModifiers++;
			padAmount = 0;
			done = 1;
			break;

		case ArgumentNameIgnorePadRightEdge:
			mode = PadModeIgnore;
			_padRightModifiers++;
			padAmount = 0;
			done = 1;
			break;
	}

	if (done)
	{
		pad->amountMode = mode;
		pad->amount = padAmount;
	}

	return done;
}

Sint32 StringArgumentNameHandler(ArgumentName name, void** var, void* data)
{
	Char**	dstPtr = *(Char***) var;
	Char*	dst;
	Char*	src = (Char*) data;

	dst = new Char[strlen(src) + 1];

	if (dst)
	{	
		strcpy(dst, src);
		*dstPtr = dst;
	}
	else
	{
		fprintf(stderr, "Memory allocation error.\n");
		return 0;
	}

	return 1;
}

Sint32 IntArgumentNameHandler(ArgumentName name, void** var, void* data)
{
	Sint32*	dst = *(Sint32**) var;
	Sint32*	src = (Sint32*) data;

	*dst = *src;

	return 1;
}

Sint32 ColorIntRGBAArgumentNameHandler(ArgumentName name, void** var, void* data)
{
	ColorIntRGBA*	dst = *(ColorIntRGBA**) var;
	ColorIntRGBA*	src = (ColorIntRGBA*) data;

	*dst = *src;

	return 1;
}

Sint32 BooleanArgumentNameHandler(ArgumentName name, void** var, void* data)
{
	Boolean*	dst = *(Boolean**) var;
	Boolean*	src = (Boolean*) data;

	*dst = *src;

	return 1;
}

ArgumentList	_argumentList[ArgumentNameLast] = 
{
	//	ArgumentNameTMPL
	{
		ArgumentNameTMPL,
		"-tmpl",
		ArgumentDataTypeString,
		&_tmplName,
		0,
		StringArgumentNameHandler
	},

	//	ArgumentNameXML
	{
		ArgumentNameXML,
		"-xml",
		ArgumentDataTypeString,
		&_xmlName,
		0,
		StringArgumentNameHandler
	},

	//	ArgumentNameProperties
	{
		ArgumentNameProperties,
		"-props",
		ArgumentDataTypeString,
		&_propsName,
		0,
		StringArgumentNameHandler
	},

	//	ArgumentNameHalfTexel
	{
		ArgumentNameHalfTexel,
		"-halftexel",
		ArgumentDataTypeFlag,
		&_halfTexelMode,
		HalfTexelModeOn,
		HalfTexelArgumentNameHandler
	},

	//	ArgumentNameIgnoreHalfTexel
	{
		ArgumentNameIgnoreHalfTexel,
		"-ignorehalftexel",
		ArgumentDataTypeFlag,
		&_halfTexelMode,
		HalfTexelModeIgnore,
		HalfTexelArgumentNameHandler
	},

	//	ArgumentNamePadAllEdges
	{
		ArgumentNamePadAllEdges,
		"-padalledges",
		ArgumentDataTypeInt,
		&_padEdgeColor,
		0,
		PadEdgeArgumentNameHandler
	},

	//	ArgumentNamePadTopEdge
	{
		ArgumentNamePadTopEdge,
		"-padtopedge",
		ArgumentDataTypeInt,
		&_padEdgeColor.top,
		0,
		PadColorArgumentNameHandler
	},

	//	ArgumentNamePadBottomEdge
	{
		ArgumentNamePadBottomEdge,
		"-padbottomedge",
		ArgumentDataTypeInt,
		&_padEdgeColor.bottom,
		0,
		PadColorArgumentNameHandler
	},

	//	ArgumentNamePadLeftEdge
	{
		ArgumentNamePadLeftEdge,
		"-padleftedge",
		ArgumentDataTypeInt,
		&_padEdgeColor.left,
		0,
		PadColorArgumentNameHandler
	},

	//	ArgumentNamePadRightEdge
	{
		ArgumentNamePadRightEdge,
		"-padrightedge",
		ArgumentDataTypeInt,
		&_padEdgeColor.right,
		0,
		PadColorArgumentNameHandler
	},

	//	ArgumentNameForcePadAllEdges
	{
		ArgumentNameForcePadAllEdges,
		"-forcepadalledges",
		ArgumentDataTypeInt,
		&_padEdgeColor,
		0,
		PadEdgeArgumentNameHandler
	},

	//	ArgumentNameForcePadTopEdge
	{
		ArgumentNameForcePadTopEdge,
		"-forcepadtopedge",
		ArgumentDataTypeInt,
		&_padEdgeColor.top,
		0,
		PadColorArgumentNameHandler
	},

	//	ArgumentNameForcePadBottomEdge
	{
		ArgumentNameForcePadBottomEdge,
		"-forcepadbottomedge",
		ArgumentDataTypeInt,
		&_padEdgeColor.bottom,
		0,
		PadColorArgumentNameHandler
	},

	//	ArgumentNameForcePadLeftEdge
	{
		ArgumentNameForcePadLeftEdge,
		"-forcepadleftedge",
		ArgumentDataTypeInt,
		&_padEdgeColor.left,
		0,
		PadColorArgumentNameHandler
	},

	//	ArgumentNameForcePadRightEdge
	{
		ArgumentNameForcePadRightEdge,
		"-forcepadrightedge",
		ArgumentDataTypeInt,
		&_padEdgeColor.right,
		0,
		PadColorArgumentNameHandler
	},

	//	ArgumentNameIgnorePadAllEdges
	{
		ArgumentNameIgnorePadAllEdges,
		"-ignorepadalledges",
		ArgumentDataTypeFlag,
		&_padEdgeColor,
		0,
		PadEdgeArgumentNameHandler
	},

	//	ArgumentNameIgnorePadTopEdge
	{
		ArgumentNameIgnorePadTopEdge,
		"-ignorepadtopedge",
		ArgumentDataTypeFlag,
		&_padEdgeColor.top,
		0,
		PadColorArgumentNameHandler
	},

	//	ArgumentNameIgnorePadBottomEdge
	{
		ArgumentNameIgnorePadBottomEdge,
		"-ignorepadbottomedge",
		ArgumentDataTypeFlag,
		&_padEdgeColor.bottom,
		0,
		PadColorArgumentNameHandler
	},

	//	ArgumentNameIgnorePadLeftEdge
	{
		ArgumentNameIgnorePadLeftEdge,
		"-ignorepadleftedge",
		ArgumentDataTypeFlag,
		&_padEdgeColor.left,
		0,
		PadColorArgumentNameHandler
	},

	//	ArgumentNameIgnorePadRightEdge
	{
		ArgumentNameIgnorePadRightEdge,
		"-ignorepadrightedge",
		ArgumentDataTypeFlag,
		&_padEdgeColor.right,
		0,
		PadColorArgumentNameHandler
	},

	//	ArgumentNamePower2
	{
		ArgumentNamePower2,
		"-power2",
		ArgumentDataTypeFlag,
		&_power2,
		true,
		BooleanArgumentNameHandler
	},

	//	ArgumentNameWidth
	{
		ArgumentNameWidth,
		"-width",
		ArgumentDataTypeInt,
		&_tsheet_width,
		0,
		IntArgumentNameHandler
	},

	//	ArgumentNameHeight
	{
		ArgumentNameHeight,
		"-height",
		ArgumentDataTypeInt,
		&_tsheet_height,
		0,
		IntArgumentNameHandler
	},

	//	ArgumentNameSmallest
	{
		ArgumentNameSmallest,
		"-smallest",
		ArgumentDataTypeFlag,
		&_smallest,
		true,
		BooleanArgumentNameHandler
	},

	//	ArgumentNameTransparentColor
	{
		ArgumentNameTransparentColor,
		"-transparentcolor",
		ArgumentDataTypeRGBA,
		&_transColor,
		0,
		ColorIntRGBAArgumentNameHandler
	},

	//	ArgumentNamePadColor
	{
		ArgumentNamePadColor,
		"-padcolor",
		ArgumentDataTypeRGBA,
		&_padEdgeColor,
		0,
		PadEdgeColorArgumentNameHandler
	},

	//	ArgumentNameAllowRotates
	{
		ArgumentNameAllowRotates,
		"-allowrotates",
		ArgumentDataTypeFlag,
		&_allowRotate,
		true,
		BooleanArgumentNameHandler
	},

	//	ArgumentNameOptimize
	{
		ArgumentNameOptimize,
		"-optimize",
		ArgumentDataTypeFlag,
		&_optimize,
		true,
		BooleanArgumentNameHandler
	},

	//	ArgumentNameAlphaDefringe
	{
		ArgumentNameAlphaDefringe,
		"-alphadefringe",
		ArgumentDataTypeInt,
		&_alphaDefringe,
		AlphaDefringeModeOn,
		AlphaDefringeIntArgumentNameHandler
	},

	//	ArgumentNameIgnoreAlphaDefringe
	{
		ArgumentNameIgnoreAlphaDefringe,
		"-ignorealphadefringe",
		ArgumentDataTypeFlag,
		&_alphaDefringe,
		AlphaDefringeModeIgnore,
		AlphaDefringeBooleanArgumentNameHandler
	},

	//	ArgumentNameExportPrefix
	{
		ArgumentNameExportPrefix,
		"-prefix",
		ArgumentDataTypeString,
		&_exportPrefix,
		0,
		StringArgumentNameHandler
	},

	//	ArgumentNameExportName
	{
		ArgumentNameExportName,
		"-name",
		ArgumentDataTypeString,
		&_exportName,
		0,
		StringArgumentNameHandler
	},

	//	ArgumentNameExportID
	{
		ArgumentNameExportID,
		"-id",
		ArgumentDataTypeInt,
		&_exportID,
		0,
		IntArgumentNameHandler
	},

	//	ArgumentNameSaturation
	{
		ArgumentNameSaturation,
		"-saturation",
		ArgumentDataTypeFloat,
		&_saturation,
		0,
		SaturationFloatArgumentNameHandler
	},
};

void copyright(void)
{
	fprintf(stdout, "%s\n", _app_name);
	fprintf(stdout, "Version %d.%d\n", _versionMajor, _versionMinor);
	fprintf(stdout, "Copyright (C) 2009 by 2n Productions, All Rights Reserved.\n");
	fprintf(stdout, "EOS Engine\n");
}

void usage(void)
{
	fprintf(stdout, "Usage: %s -tmpl textmpl | -xml texxml\n", _app_name);
	fprintf(stdout, "\t-prefix prefix\n");
	fprintf(stdout, "\t-width val -height val\n");
	fprintf(stdout, "\t[-name name]\n");
	fprintf(stdout, "\t[-id id]\n");
	fprintf(stdout, "\t[-props propertiesxml]\n");
	fprintf(stdout, "\t[-saturation val]\n");
	fprintf(stdout, "\t[-halftexel | -ignorehalftexel]\n");
	fprintf(stdout, "\t[-alphadefringe val | -ignorealphadefringe]\n");
	fprintf(stdout, "\t[-padalledges val | -forcepadalledges val | -ignorepadalledges]\n");
	fprintf(stdout, "\t[-padtopedge val | -forcepadtopedge val | -ignorepadtopedge]\n");
	fprintf(stdout, "\t[-padbottomedge val | -forcepadbottomedge val | -ignorepadbottomedge]\n");
	fprintf(stdout, "\t[-padleftedge val | -forcepadleftedge val | -ignorepadleftedge]\n");
	fprintf(stdout, "\t[-padrightedge val | -forcepadrightedge val | -ignorepadrightedge]\n");
	fprintf(stdout, "\t[-transparentcolor R G B A]\n");
	fprintf(stdout, "\t[-padcolor R G B A]\n");
	fprintf(stdout, "\t[-power2]\n");
	fprintf(stdout, "\t[-smallest]\n");
	fprintf(stdout, "\t[-optmize]\n");
	fprintf(stdout, "\t[-allowrotates (UNSUPPROTED)]\n");
}

void cleanUp(void)
{
	if (_tmplName)
	{
		delete _tmplName;
		_tmplName = NULL;
	}

	if (_xmlName)
	{
		delete _xmlName;
		_xmlName = NULL;
	}

	if (_propsName)
	{
		delete _propsName;
		_propsName = NULL;
	}

	if (_exportPrefix)
	{
		delete _exportPrefix;
		_exportPrefix = NULL;
	}

	if (_exportName)
	{
		delete _exportName;
		_exportName = NULL;
	}
}

ArgumentList*	getMatchingArgument(Char* arg)
{
	ArgumentList* 	argList = NULL;
	Uint32			i;

	for (i=0;i<ArgumentNameLast;i++)
	{
		if (!strcmp(_argumentList[i].nameString, arg))
		{
			argList = &_argumentList[i];
			break;
		}
	}

	return argList;
}

Sint32 parseArgument(ArgumentList* argList, int remainder, char* argv[])
{
	Sint32	consumed = -1;

	consumed = _argDataTypeHandlers[argList->dataType](remainder, argv, _argDataTypeWorkData[argList->dataType]);

	if (consumed > -1)
	{
		if (argList->dataType == ArgumentDataTypeFlag)
			_workFlag = argList->flagValue;

		if (argList->handler(argList->name, &argList->data, _argDataTypeWorkData[argList->dataType]) == 0)
		{
			consumed = -1;
		}
	}

	return consumed;
}

int parseArguments(int argc, char* argv[])
{
	int				error = 0;
	int				index = 0;
	ArgumentList*	argList;
	int				consumed;

	while (index < argc)
	{
		argList = getMatchingArgument(argv[index]);

		if (argList)
		{
			//	Eat the argument
			index++;

			if ((argc - index) >= 0)
			{
				consumed = parseArgument(argList, argc - index, &argv[index]);

				if (consumed == -1)
				{
					error = EOSErrorIllegalArguments;
					break;
				}
				else
				{
					index += consumed;
				}
			}
			else
			{
				error = EOSErrorIllegalArguments;
				break;
			}
		}
		else
		{
			error = EOSErrorIllegalArguments;
			break;
		}
	}

	return error;
}

TextureBuilder		_textureBuilder;
TexPropsXMLParser	_propsXMLParser;

EOSError injectXMLPropertiesIntoTextureBuilder(TexPropsXMLParser& parser)
{
	Uint32				num;
	Uint32				i;
	EOSError 			error = EOSErrorNone;
	XMLElementSubTex*	subtex;

	num = parser.getNumSubTex();

	for (i=0;i<num;i++)
	{
		subtex = parser.getSubTexAtIndex(i);

		if (subtex)
		{
			error = _textureBuilder.assignTexPropToTextureDefinition(subtex);

			if (error == EOSErrorResourceDoesNotExist)
			{
				printf("WARNING: %s not found in texture defintions. Ignoring.\n", subtex->getName()->getString());
				error = EOSErrorNone;
			}
		}
		else
			error = EOSErrorResourceDoesNotExist;

		if (error != EOSErrorNone)
			break;
	}

	return error;
}

int main(int argc, char* argv[])
{
	int	error = 0;

	error = parseArguments(argc - 1, &argv[1]);

	copyright();

	_textureBuilder.setVerbose(true);

	if (error == EOSErrorNone)
	{
		//	XMLNAME NOT SUPPORTED YET
		if ((_tmplName) && _exportPrefix && _tsheet_width > 0 && _tsheet_height > 0)
		{
			_textureBuilder.setTexSheetWidth(_tsheet_width);
			_textureBuilder.setTexSheetHeight(_tsheet_height);
			_textureBuilder.setTransparentColorMode(_transColorMode);
			_textureBuilder.setTransparentColor(_transColor);
			_textureBuilder.setPadEdgeColor(_padEdgeColor);
			_textureBuilder.setHalfTexelMode(_halfTexelMode);
			_textureBuilder.setAlphaDefringe(_alphaDefringe);
			_textureBuilder.setSaturation(_saturation);
			_textureBuilder.setPower2(_power2);
			_textureBuilder.setSmallest(_smallest);
			_textureBuilder.setOptimize(_optimize);
			_textureBuilder.setExportPrefix(_exportPrefix);
			_textureBuilder.setExportName(_exportName);
			_textureBuilder.setExportID(_exportID);

			error = _textureBuilder.loadTextureDefFile(_tmplName);

			if (error == EOSErrorNone)
			{
				if (_propsName)
				{
					error = _propsXMLParser.parse(_propsName);

					if (error == EOSErrorNone)
					{
						error = injectXMLPropertiesIntoTextureBuilder(_propsXMLParser);
					}
				}

				if (error == EOSErrorNone)
				{
					error = _textureBuilder.loadTextureFiles();

					if (error == EOSErrorNone)
					{
						error = _textureBuilder.buildWorkTextures();

						if (error == EOSErrorNone)
						{
							error = _textureBuilder.validateBaseConfig();

							if (error == EOSErrorNone)
							{
								error = _textureBuilder.generateBestTextureSheet();
							}
						}
					}
				}
			}
		}
		else
			error = EOSErrorIllegalArguments;
	}

	if (error != EOSErrorNone)
	{
		usage();
		fprintf(stderr, "Error: %d\n", error);
	}

	cleanUp();

	return error;
}

