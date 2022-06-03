/******************************************************************************
 *
 * File: FontSetXML.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * 
 *****************************************************************************/

#include "FontSetXML.h"
#include "File.h"

Char* FontScriptXMLParser::_xmlElementTypeNamespace[FontScriptXMLParser::FontScriptXMLElementTypeLast] = 
{
	"fontscript",
	"range",
	"symbols",
	"symbol",
	"name",
	"min",
	"max",
	"height",
	"nextline",
	"spacewidth",
	"leading",
	"trailing",
	"scalex",
	"scaley",
	"dy",
};

FontScriptXMLParser::FontScriptXMLParser()
{
	setXMLElementTypeNamespace(FontScriptXMLElementTypeLast, _xmlElementTypeNamespace);
	memset(&_exportDB, 0, sizeof(FontSetScriptDBExport));

	_exportDB.scaleX = _exportDB.scaleY = 1.0F;
}

FontScriptXMLParser::~FontScriptXMLParser()
{
	destroyXMLElementNamespace();

	if (_exportDB.symbols)
		delete _exportDB.symbols;

	if (_exportDB.names)
		delete _exportDB.names;
}

EOSError FontScriptXMLParser::_addName(FontSetScriptDBExport& db, Char* name, Uint32& offset)
{
	Char*		newlist;
	EOSError 	error = EOSErrorNone;
	size_t		strsize;
	Boolean		found = false;
	size_t		curr = 0;

	//	Try and see if it exists already
	if (db.names)
	{
		strsize = strlen(db.names);

		while (curr < db.namesSize)
		{
			if (!strcmp(name, &db.names[curr]))
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
	
		newlist = new Char[db.namesSize + strsize + 1];
	
		if (newlist)
		{
			if (db.names)
			{
				memcpy(newlist, db.names, db.namesSize);
				delete db.names;
			}
	
			strcpy(&newlist[db.namesSize], name);
			db.names = newlist;
			offset = db.namesSize;
			db.namesSize += (Uint32) strsize + 1;
		}
		else
			error = EOSErrorNoMemory;
	}

	return error;
}

XMLElement* FontScriptXMLParser::_buildXMLElementFromName(const xmlChar* name)
{
	XMLElement*		element = NULL;
	XMLElementType	type;

	type = nameToXMLElementType(name);

	if (type != XMLElementTypeIllegal)
	{
		switch (type)
		{
			case FontScriptXMLElementTypeFontScript:
				element = new FontScriptXMLElementFontScript;
				break;

			case FontScriptXMLElementTypeRange:
				element = new FontScriptXMLElementRange;
				break;

			case FontScriptXMLElementTypeSymbols:
				element = new FontScriptXMLElementSymbols;
				break;

			case FontScriptXMLElementTypeSymbol:
				element = new FontScriptXMLElementSymbol;
				break;

			case FontScriptXMLElementTypeName:
				element = new FontScriptXMLElementName;
				break;

			case FontScriptXMLElementTypeMin:
				element = new FontScriptXMLElementMin;
				break;

			case FontScriptXMLElementTypeMax:
				element = new FontScriptXMLElementMax;
				break;

			case FontScriptXMLElementTypeHeight:
				element = new FontScriptXMLElementHeight;
				break;

			case FontScriptXMLElementTypeNextLine:
				element = new FontScriptXMLElementNextLine;
				break;

			case FontScriptXMLElementTypeSpaceWidth:
				element = new FontScriptXMLElementSpaceWidth;
				break;

			case FontScriptXMLElementTypeLeading:
				element = new FontScriptXMLElementLeading;
				break;

			case FontScriptXMLElementTypeTrailing:
				element = new FontScriptXMLElementTrailing;
				break;

			case FontScriptXMLElementTypeScaleX:
				element = new FontScriptXMLElementScaleX;
				break;

			case FontScriptXMLElementTypeScaleY:
				element = new FontScriptXMLElementScaleY;
				break;

			case FontScriptXMLElementTypeDY:
				element = new FontScriptXMLElementDY;
				break;
		}
	}

	return element;
}

FontSymbolExport* FontScriptXMLParser::findFontSymbolExport(Char* name)
{
	FontSymbolExport*	exp = NULL;
	Uint32				i;
	Char*				str;

	for (i=0;i<_exportDB.numSymbols;i++)
	{
		str = &_exportDB.names[_exportDB.symbols[i].nameOffset];

		if (!strcmp(str, name))
		{
			exp = &_exportDB.symbols[i];
			break;
		}
	}

	return exp;
}

FontSymbolExport* FontScriptXMLParser::getFontSymbolExportAtIndex(Uint32 index)
{
	if (_exportDB.symbols && index < _exportDB.numSymbols)
		return &_exportDB.symbols[index];
	else 
		return NULL;
}

void FontScriptXMLParser::setTexName(Char* name)
{
	_addName(_exportDB, name, _exportDB.texNameOffset);
}

FontScriptXMLElementRange* FontScriptXMLParser::getFontScriptRange(void)
{
	FontScriptXMLElementRange* range = NULL;
	FontScriptXMLElementFontScript*		fontScript;

	if (getRoot() && getRoot()->getType() == FontScriptXMLElementTypeFontScript)
	{
		fontScript = (FontScriptXMLElementFontScript*) getRoot();
		range = fontScript->getRange();
	}

	return range;
}

void FontScriptXMLParser::setFontScriptRange(FontScriptXMLElementRange* range)
{
	FontScriptXMLElementFontScript*		fontScript;

	if (getRoot() && getRoot()->getType() == FontScriptXMLElementTypeFontScript)
	{
		fontScript = (FontScriptXMLElementFontScript*) getRoot();
		fontScript->setRange(range);
	}
}

EOSError FontScriptXMLParser::buildDB(void)
{
	EOSError							error = EOSErrorNone;
	FontScriptXMLElementFontScript*		fontScript;
	FontScriptXMLElementSymbol*			symbol;
	FontSymbolExport*					fontSymbolExport;
	Uint32								i;
	Float32								scaleX = 1.0F;
	Float32								scaleY = 1.0F;

	if (getRoot() && getRoot()->getType() == FontScriptXMLElementTypeFontScript)
	{
		fontScript = (FontScriptXMLElementFontScript*) getRoot();

		error = _addName(_exportDB, fontScript->getName()->getString(), _exportDB.nameOffset);

		if (error == EOSErrorNone)
		{
			_exportDB.min = fontScript->getRange()->getMin()->getValue();
			_exportDB.max = fontScript->getRange()->getMax()->getValue();
	
			_exportDB.height = fontScript->getHeight()->getValue();
			_exportDB.nextLine = fontScript->getNextLine()->getValue();
			_exportDB.spaceWidth = fontScript->getSpaceWidth()->getValue();

			if (fontScript->getScaleX())
				scaleX = fontScript->getScaleX()->getValue();
			else
				scaleX = 1.0F;

			if (fontScript->getScaleY())
				scaleY = fontScript->getScaleY()->getValue();
			else
				scaleY = 1.0F;

			_exportDB.scaleX = scaleX;
			_exportDB.scaleY = scaleY;

			_exportDB.symbols = new FontSymbolExport[fontScript->getSymbols()->getNumSymbols()];
	
			if (_exportDB.symbols)
			{
				_exportDB.numSymbols = fontScript->getSymbols()->getNumSymbols();
				memset(_exportDB.symbols, 0, sizeof(FontSymbolExport) * _exportDB.numSymbols);
	
				for (i=0;i<_exportDB.numSymbols;i++)
				{
					symbol = fontScript->getSymbols()->getSymbolAtIndex(i);
					fontSymbolExport = &_exportDB.symbols[i];
	
					error = _addName(_exportDB, symbol->getName()->getString(), fontSymbolExport->nameOffset);
	
					if (error == EOSErrorNone)
					{
						if (symbol->getLeading())
						{
							fontSymbolExport->leadingSet = true;
							fontSymbolExport->leading = symbol->getLeading()->getValue();
						}
						else
						{
							fontSymbolExport->leadingSet = false;
							fontSymbolExport->leading = 0;
						}

						if (symbol->getTrailing())
						{
							fontSymbolExport->trailingSet = true;
							fontSymbolExport->trailing = symbol->getTrailing()->getValue();
						}
						else
						{
							fontSymbolExport->trailingSet = false;
							fontSymbolExport->trailing = 0;
						}

						if (symbol->getDY())
							fontSymbolExport->dy = symbol->getDY()->getValue();
						else
							fontSymbolExport->dy = 0;
					}
					else
						break;
				}
			}
		}
	}

	return error;
}

EOSError FontScriptXMLParser::exportDB(Char* filename)
{
	return EOSErrorUnsupported;
}

FontScriptXMLElementSymbol::FontScriptXMLElementSymbol() : _name(NULL), _leading(NULL), _trailing(NULL), _dy(NULL)
{
	setType(FontScriptXMLParser::FontScriptXMLElementTypeSymbol);
}

FontScriptXMLElementSymbol::~FontScriptXMLElementSymbol()
{
	_name = NULL;
	_leading = NULL;
	_trailing = NULL;
	_dy = NULL;
}

EOSError FontScriptXMLElementSymbol::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case FontScriptXMLParser::FontScriptXMLElementTypeName:
			_name = (FontScriptXMLElementName*) child;
			break;

		case FontScriptXMLParser::FontScriptXMLElementTypeLeading:
			_leading = (FontScriptXMLElementLeading*) child;
			break;

		case FontScriptXMLParser::FontScriptXMLElementTypeTrailing:
			_trailing = (FontScriptXMLElementTrailing*) child;
			break;

		case FontScriptXMLParser::FontScriptXMLElementTypeDY:
			_dy = (FontScriptXMLElementDY*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError FontScriptXMLElementSymbol::end(const xmlChar* element)
{
	if (_name)
	{
		return XMLElementNode::end(element);
	}
	else
		return EOSErrorIllegalGrammar;
}

FontScriptXMLElementRange::FontScriptXMLElementRange() : _min(NULL), _max(NULL)
{
	setType(FontScriptXMLParser::FontScriptXMLElementTypeRange);
}

FontScriptXMLElementRange::~FontScriptXMLElementRange()
{
	_min = NULL;
	_max = NULL;
}

EOSError FontScriptXMLElementRange::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case FontScriptXMLParser::FontScriptXMLElementTypeMin:
			_min = (FontScriptXMLElementMin*) child;
			break;

		case FontScriptXMLParser::FontScriptXMLElementTypeMax:
			_max = (FontScriptXMLElementMax*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError FontScriptXMLElementRange::end(const xmlChar* element)
{
	if (_min && _max)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

FontScriptXMLElementSymbols::FontScriptXMLElementSymbols() : _numSymbols(0), _symbols(NULL)
{
	setType(FontScriptXMLParser::FontScriptXMLElementTypeSymbols);
}

FontScriptXMLElementSymbols::~FontScriptXMLElementSymbols()
{
	if (_symbols)
		delete _symbols;

	_symbols = NULL;
	_numSymbols = 0;
}

FontScriptXMLElementSymbol* FontScriptXMLElementSymbols::getSymbolAtIndex(Uint32 index)
{
	if (_symbols && index < _numSymbols)
		return _symbols[index];
	else
		return NULL;
}

EOSError FontScriptXMLElementSymbols::addChild(XMLElement* child)
{
	EOSError 					error = EOSErrorNone;
	FontScriptXMLElementSymbol**	newlist;
	Uint32						i;

	switch (child->getType())
	{
		case FontScriptXMLParser::FontScriptXMLElementTypeSymbol:
			newlist = new FontScriptXMLElementSymbol*[_numSymbols + 1];

			if (newlist)
			{
				if (_symbols)
				{
					for (i=0;i<_numSymbols;i++)
					{
						newlist[i] = _symbols[i];
					}

					delete _symbols;
				}

				newlist[_numSymbols] = (FontScriptXMLElementSymbol*) child;
				_symbols = newlist;
				_numSymbols++;
			}
			else
				error = EOSErrorNoMemory;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError FontScriptXMLElementSymbols::end(const xmlChar* element)
{
	if (_symbols)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

FontScriptXMLElementFontScript::FontScriptXMLElementFontScript() : _name(NULL), _range(NULL), _height(NULL), _scaleX(NULL), _scaleY(NULL), _nextLine(NULL), _spaceWidth(NULL), _symbols(NULL)
{
	setType(FontScriptXMLParser::FontScriptXMLElementTypeFontScript);
}

FontScriptXMLElementFontScript::~FontScriptXMLElementFontScript()
{
	_name = NULL;
	_range = NULL;
	_height = NULL;
	_scaleX = NULL;
	_scaleY = NULL;
	_nextLine = NULL;
	_spaceWidth = NULL;
	_symbols = NULL;
}

EOSError FontScriptXMLElementFontScript::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case FontScriptXMLParser::FontScriptXMLElementTypeName:
			_name = (FontScriptXMLElementName*) child;
			break;

		case FontScriptXMLParser::FontScriptXMLElementTypeRange:
			_range = (FontScriptXMLElementRange*) child;
			break;

		case FontScriptXMLParser::FontScriptXMLElementTypeHeight:
			_height = (FontScriptXMLElementHeight*) child;
			break;

		case FontScriptXMLParser::FontScriptXMLElementTypeScaleX:
			_scaleX = (FontScriptXMLElementScaleX*) child;
			break;

		case FontScriptXMLParser::FontScriptXMLElementTypeScaleY:
			_scaleY = (FontScriptXMLElementScaleY*) child;
			break;

		case FontScriptXMLParser::FontScriptXMLElementTypeNextLine:
			_nextLine = (FontScriptXMLElementNextLine*) child;
			break;

		case FontScriptXMLParser::FontScriptXMLElementTypeSpaceWidth:
			_spaceWidth = (FontScriptXMLElementSpaceWidth*) child;
			break;

		case FontScriptXMLParser::FontScriptXMLElementTypeSymbols:
			_symbols = (FontScriptXMLElementSymbols*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError FontScriptXMLElementFontScript::end(const xmlChar* element)
{
	if (_name && _height && _nextLine && _spaceWidth && _symbols)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

Char* FontSetXMLParser::_xmlElementTypeNamespace[FontSetXMLParser::FontSetXMLElementTypeLast] = 
{
	"fontset",
	"fontscripts",
	"fontscript",
	"name",
	"textureatlas",
	"templatename",
	"scriptname",
};

FontSetXMLParser::FontSetXMLParser() : _numFontScripts(0), _fontTemplateScripts(NULL), _fontScripts(NULL), _orderedFontScripts(NULL)
{
	setXMLElementTypeNamespace(FontSetXMLElementTypeLast, _xmlElementTypeNamespace);

	memset(&_exportDB, 0, sizeof(FontSetDBExport));
}

FontSetXMLParser::~FontSetXMLParser()
{
	destroyXMLElementNamespace();

	if (_orderedFontScripts)
		delete _orderedFontScripts;

	if (_fontTemplateScripts)
		delete [] _fontTemplateScripts;

	if (_fontScripts)
		delete [] _fontScripts;
}

EOSError FontSetXMLParser::_addName(Char** namelist, Uint32& namesSize, Char* name, Uint32& offset)
{
	Char*		newlist;
	Char*		names = *namelist;
	EOSError 	error = EOSErrorNone;
	size_t		strsize;
	Boolean		found = false;
	size_t		curr = 0;

	//	Try and see if it exists already
	if (names)
	{
		strsize = strlen(names);

		while (curr < namesSize)
		{
			if (!strcmp(name, &names[curr]))
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
	
		newlist = new Char[namesSize + strsize + 1];
	
		if (newlist)
		{
			if (names)
			{
				memcpy(newlist, names, namesSize);
				delete names;
			}
	
			strcpy(&newlist[namesSize], name);
			*namelist = newlist;
			offset = namesSize;
			namesSize += (Uint32) strsize + 1;
		}
		else
			error = EOSErrorNoMemory;
	}

	return error;
}

EOSError FontSetXMLParser::_addName(FontSetDBExport& db, Char* name, Uint32& offset)
{
	Char*		newlist;
	EOSError 	error = EOSErrorNone;
	size_t		strsize;
	Boolean		found = false;
	size_t		curr = 0;

	//	Try and see if it exists already
	if (db.names)
	{
		strsize = strlen(db.names);

		while (curr < db.namesSize)
		{
			if (!strcmp(name, &db.names[curr]))
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
	
		newlist = new Char[db.namesSize + strsize + 1];
	
		if (newlist)
		{
			if (db.names)
			{
				memcpy(newlist, db.names, db.namesSize);
				delete db.names;
			}
	
			strcpy(&newlist[db.namesSize], name);
			db.names = newlist;
			offset = db.namesSize;
			db.namesSize += (Uint32) strsize + 1;
		}
		else
			error = EOSErrorNoMemory;
	}

	return error;
}

Uint32 FontSetXMLParser::_getNumValidSymbols(FontScriptXMLParser* parser)
{
	Uint32	i;
	Uint32	num = 0;

	for (i=0;i<parser->getNumSymbols();i++)
	{
		if (parser->getFontSymbolExportAtIndex(i)->valid)
			num++;
	}

	return num;
}

XMLElement* FontSetXMLParser::_buildXMLElementFromName(const xmlChar* name)
{
	XMLElement*		element = NULL;
	XMLElementType	type;

	type = nameToXMLElementType(name);

	if (type != XMLElementTypeIllegal)
	{
		switch (type)
		{
			case FontSetXMLElementTypeFontSet:
				element = new FontSetXMLElementFontSet;
				break;

			case FontSetXMLElementTypeFontScripts:
				element = new FontSetXMLElementFontScripts;
				break;

			case FontSetXMLElementTypeName:
				element = new FontSetXMLElementName;
				break;

			case FontSetXMLElementTypeFontScript:
				element = new FontSetXMLElementFontScript;
				break;

			case FontSetXMLElementTypeTemplateName:
				element = new FontSetXMLElementTemplateName;
				break;

			case FontSetXMLElementTypeTextureAtlas:
				element = new FontSetXMLElementTextureAtlas;
				break;

			case FontSetXMLElementTypeScriptName:
				element = new FontSetXMLElementScriptName;
				break;
		}
	}

	return element;
}

EOSError FontSetXMLParser::buildDB(void)
{
	EOSError							error = EOSErrorNone;
	FontSetXMLElementFontSet*			fontSet;
	Uint32								i, j, k;
	FontSetScriptDBExport*					db;
	FontSetScriptDBExport*					templateDB;
	FontSymbolExport*				   	templateSymbol;
	FontSymbolExport*					symbol;
	SubTextureExport*					subtex;
	TextureAtlasDBExport*				texDB;

	if (getRoot() && getRoot()->getType() == FontSetXMLElementTypeFontSet)
	{
		fontSet = (FontSetXMLElementFontSet*) getRoot();

		error = _addName(_exportDB, fontSet->getName()->getString(), _exportDB.nameOffset);

		if (error == EOSErrorNone)
		{
			_textureAtlases = new TextureAtlasXMLParser[fontSet->getFontScripts()->getNumFontScripts()];
			_fontTemplateScripts = new FontScriptXMLParser[fontSet->getFontScripts()->getNumFontScripts()];
			_fontScripts = new FontScriptXMLParser[fontSet->getFontScripts()->getNumFontScripts()];

			if (_textureAtlases && _fontTemplateScripts && _fontScripts)
			{
				_numFontScripts = fontSet->getFontScripts()->getNumFontScripts();
				_exportDB.numFontScripts = _numFontScripts;

				for (i=0;i<_numFontScripts;i++)
				{
					error = _textureAtlases[i].parse(fontSet->getFontScripts()->getFontScriptAtIndex(i)->getTextureAtlas()->getString());

					if (error == EOSErrorNone)
						error = _textureAtlases[i].buildDB();
					else
						break;

					texDB = _textureAtlases[i].getTextureAtlasDBExport();

					error = _fontTemplateScripts[i].parse(fontSet->getFontScripts()->getFontScriptAtIndex(i)->getTemplateName()->getString());

					if (error == EOSErrorNone)
						error = _fontTemplateScripts[i].buildDB();
					else
						break;

					error = _fontScripts[i].parse(fontSet->getFontScripts()->getFontScriptAtIndex(i)->getScriptName()->getString());

					if (error == EOSErrorNone)
					{
						if (_fontScripts[i].getFontScriptRange() == NULL)
						{
							FontScriptXMLElementRange*	tmplRange = _fontTemplateScripts[i].getFontScriptRange();

							if (tmplRange)
							{
								_fontScripts[i].setFontScriptRange(tmplRange);
							}
							else
							{
								fprintf(stderr, "ERROR: No defined range in template XML\n");
								error = EOSErrorResourceDoesNotExist;
							}
						}

						error = _fontScripts[i].buildDB();
						_fontScripts[i].setTexName(&texDB->names[texDB->nameOffset]);

						templateDB = _fontTemplateScripts[i].getFontSetScriptDBExport();
						db = _fontScripts[i].getFontSetScriptDBExport();

						templateDB->height = db->height;
						templateDB->nextLine = db->nextLine;
						templateDB->spaceWidth = db->spaceWidth;

						for (j=0;j<templateDB->numSymbols;j++)
						{
							templateSymbol = &templateDB->symbols[j];

							symbol = _fontScripts[i].findFontSymbolExport(&templateDB->names[templateSymbol->nameOffset]);

							if (symbol)
							{
								templateSymbol->valid = 1;

								if (symbol->leadingSet)
								{
									templateSymbol->leading = symbol->leading * db->scaleX;
								}
								else if (templateDB->scaleX != db->scaleX)
								{
									templateSymbol->leading *= db->scaleX;
								}

								if (symbol->trailingSet)
								{
									templateSymbol->trailing = symbol->trailing * db->scaleX;
								}
								else if (templateDB->scaleX != db->scaleX)
								{
									templateSymbol->trailing *= db->scaleX;
								}

								templateSymbol->dy = symbol->dy;

								subtex = _textureAtlases[i].findSubTextureExport(&templateDB->names[templateSymbol->nameOffset]);

								if (subtex)
								{
									for (k=0;k<4;k++)
									{
										templateSymbol->vertices[k] = subtex->vertices[k];
										templateSymbol->uvs[k] = subtex->uvs[k];
									}
								}
								else
									printf("NO SUBTEXT\n");
							}
							else
								printf("Could not find symbol for %s\n", &templateDB->names[templateSymbol->nameOffset]);
						}
					}
					else
						break;
				}
			}
			else
				error = EOSErrorNoMemory;
		}
	}

	return error;
}

EOSError FontSetXMLParser::exportDB(Char* filename)
{
	Uint32					i, j, k;
	Uint32					numValid; 
	Uint32					numVerts = 0;
	Uint32					numUVs = 0;
	Uint32					numSymbols = 0;
	FontSetHeader			header;
	FontSetScriptHeader*		scriptHeaders;
	Uint32					totalsize;
	FontSetScriptDBExport*		db;
	FontSetScriptDBExport*		templateDB;
	Uint32					vertsOffset = 0;
	Uint32					uvsOffset = 0;
	Uint32					namesOffset = 0;
	Uint32					symbolsOffset = 0;
	Uint8*					image;
	Point2D*				vertices;
	FontSymbolRAW*			symbols;
	Point2D*				uvs;
	EOSError				error = EOSErrorNone;
	File					file;
	FileDescriptor			fd;
	Char*					names = NULL;
	Uint32					namesSize = 0;
	Uint32					offset = 0;

	fd.setFilename(filename);
	fd.setFileAccessType(FileAccessTypeWriteOnly);

	file.setFileDescriptor(fd);

	error = file.open();

	if (error == EOSErrorNone)
	{
		totalsize = sizeof(FontSetHeader) + sizeof(FontSetScriptHeader) * _numFontScripts;
	
		for (i=0;i<_numFontScripts;i++)
		{
			templateDB = _fontTemplateScripts[i].getFontSetScriptDBExport();
	
			totalsize += sizeof(FontSymbolRAW) * templateDB->numSymbols;
	
			numValid = _getNumValidSymbols(&_fontTemplateScripts[i]);
	
			numSymbols += templateDB->numSymbols;
			numVerts += numValid * 4;
			numUVs += numValid * 4;
		}
	
		totalsize += (numVerts + numUVs) * sizeof(Point2D);

		scriptHeaders = new FontSetScriptHeader[_exportDB.numFontScripts];
		vertices = new Point2D[numVerts];
		uvs = new Point2D[numUVs];
		symbols = new FontSymbolRAW[numSymbols];

		image = new Uint8[totalsize];
	
		if (image && scriptHeaders && vertices && uvs && symbols)
		{
			memset(scriptHeaders, 0, sizeof(FontSetScriptHeader) * _exportDB.numFontScripts);
			memset(vertices, 0, sizeof(Point2D) * numVerts);
			memset(uvs, 0, sizeof(Point2D) * numUVs);
			memset(symbols, 0, sizeof(FontSymbolRAW) * numSymbols);

			header.endian = 0x01020304;
			header.version = 0;
			_addName(&names, namesSize, &_exportDB.names[_exportDB.nameOffset], header.nameOffset);
			header.numFontScripts = _exportDB.numFontScripts;
			header.numVertices = numVerts;
			header.numUVs = numUVs;

			offset += sizeof(FontSetHeader);

			header.fontScripts = offset;

			offset += sizeof(FontSetScriptHeader) * _numFontScripts;

			header.symbols = offset;

			offset += sizeof(FontSymbolRAW) * numSymbols;

			header.vertices = offset;

			offset += sizeof(Point2D) * numVerts;

			header.uvs = offset;

			offset += sizeof(Point2D) * numUVs;

			header.names = offset;

			for (i=0;i<_numFontScripts;i++)
			{
				templateDB = _fontTemplateScripts[i].getFontSetScriptDBExport();
				db = _fontScripts[i].getFontSetScriptDBExport();

				scriptHeaders[i].min = templateDB->min;
				scriptHeaders[i].max = templateDB->max;
				scriptHeaders[i].height = templateDB->height;
				scriptHeaders[i].nextLine = templateDB->nextLine;
				scriptHeaders[i].spaceWidth = templateDB->spaceWidth;
				scriptHeaders[i].numSymbols = templateDB->numSymbols;
				_addName(&names, namesSize, &db->names[db->nameOffset], scriptHeaders[i].nameOffset);
				_addName(&names, namesSize, &db->names[db->texNameOffset], scriptHeaders[i].texNameOffset);
				scriptHeaders[i].symbols = symbolsOffset;
				scriptHeaders[i].vertices = vertsOffset;
				scriptHeaders[i].uvs = uvsOffset;

				for (j=0;j<templateDB->numSymbols;j++)
				{
					symbols[symbolsOffset + j].valid = templateDB->symbols[j].valid;
					symbols[symbolsOffset + j].leading = templateDB->symbols[j].leading;
					symbols[symbolsOffset + j].trailing = templateDB->symbols[j].trailing;
					symbols[symbolsOffset + j].dy = templateDB->symbols[j].dy;

					if (templateDB->symbols[j].valid)
					{
						symbols[symbolsOffset + j].vertices = vertsOffset - scriptHeaders[i].vertices;
						symbols[symbolsOffset + j].uvs = uvsOffset - scriptHeaders[i].uvs;
	
						for (k=0;k<4;k++)
						{
							vertices[vertsOffset].x = templateDB->symbols[j].vertices[k].x;
							vertices[vertsOffset].y = templateDB->symbols[j].vertices[k].y;
							uvs[uvsOffset].x = templateDB->symbols[j].uvs[k].x;
							uvs[uvsOffset].y = templateDB->symbols[j].uvs[k].y;

							vertsOffset++;
							uvsOffset++;
						}
					}
				}

				symbolsOffset += templateDB->numSymbols;
			}

			memcpy(image, &header, sizeof(FontSetHeader));
			memcpy(&image[header.fontScripts], scriptHeaders, sizeof(FontSetScriptHeader) * _numFontScripts);
			memcpy(&image[header.symbols], symbols, sizeof(FontSymbolRAW) * numSymbols);
			memcpy(&image[header.vertices], vertices, sizeof(Point2D) * numVerts);
			memcpy(&image[header.uvs], uvs, sizeof(Point2D) * numUVs);

			file.writeUint8(image, totalsize);
			file.writeUint8((Uint8*) names, namesSize);
		}
		else
			error = EOSErrorNoMemory;

		if (image)
			delete image;

		if (scriptHeaders)
			delete scriptHeaders;

		if (vertices)
			delete vertices;

		if (uvs)
			delete uvs;

		if (symbols)
			delete symbols;

		file.close();
	}

	return error;
}

FontSetXMLElementFontScripts::FontSetXMLElementFontScripts() : _numScripts(0), _scripts(NULL)
{
	setType(FontSetXMLParser::FontSetXMLElementTypeFontScripts);
}

FontSetXMLElementFontScripts::~FontSetXMLElementFontScripts()
{
	if (_scripts)
		delete _scripts;

	_scripts = NULL;
	_numScripts = 0;
}

FontSetXMLElementFontScript* FontSetXMLElementFontScripts::getFontScriptAtIndex(Uint32 index)
{
	if (_scripts && index < _numScripts)
		return _scripts[index];
	else
		return NULL;
}

EOSError FontSetXMLElementFontScripts::addChild(XMLElement* child)
{
	EOSError 						error = EOSErrorNone;
	FontSetXMLElementFontScript**	newlist;
	Uint32							i;

	switch (child->getType())
	{
		case FontSetXMLParser::FontSetXMLElementTypeFontScript:
			newlist = new FontSetXMLElementFontScript*[_numScripts + 1];

			if (newlist)
			{
				if (_scripts)
				{
					for (i=0;i<_numScripts;i++)
					{
						newlist[i] = _scripts[i];
					}

					delete _scripts;
				}

				newlist[_numScripts] = (FontSetXMLElementFontScript*) child;
				_scripts = newlist;
				_numScripts++;
			}
			else
				error = EOSErrorNoMemory;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError FontSetXMLElementFontScripts::end(const xmlChar* element)
{
	if (_scripts)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

FontSetXMLElementFontScript::FontSetXMLElementFontScript() : _textureAtlas(NULL), _templateName(NULL), _scriptName(NULL)
{
	setType(FontSetXMLParser::FontSetXMLElementTypeFontScript);
}

FontSetXMLElementFontScript::~FontSetXMLElementFontScript()
{
	_textureAtlas = NULL;
	_templateName = NULL;
	_scriptName = NULL;
}

EOSError FontSetXMLElementFontScript::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case FontSetXMLParser::FontSetXMLElementTypeTextureAtlas:
			_textureAtlas = (FontSetXMLElementTextureAtlas*) child;
			break;

		case FontSetXMLParser::FontSetXMLElementTypeTemplateName:
			_templateName = (FontSetXMLElementTemplateName*) child;
			break;

		case FontSetXMLParser::FontSetXMLElementTypeScriptName:
			_scriptName = (FontSetXMLElementScriptName*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError FontSetXMLElementFontScript::end(const xmlChar* element)
{
	if (_textureAtlas && _templateName && _scriptName)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

FontSetXMLElementFontSet::FontSetXMLElementFontSet() : _name(NULL), _scripts(NULL)
{
	setType(FontSetXMLParser::FontSetXMLElementTypeFontSet);
}

FontSetXMLElementFontSet::~FontSetXMLElementFontSet()
{
	_name = NULL;
	_scripts = NULL;
}

EOSError FontSetXMLElementFontSet::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case FontSetXMLParser::FontSetXMLElementTypeName:
			_name = (FontSetXMLElementName*) child;
			break;

		case FontSetXMLParser::FontSetXMLElementTypeFontScripts:
			_scripts = (FontSetXMLElementFontScripts*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError FontSetXMLElementFontSet::end(const xmlChar* element)
{
	if (_name && _scripts)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

