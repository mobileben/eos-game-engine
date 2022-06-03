/******************************************************************************
 *
 * File: Graphics2DBinder.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Binder to hold all the information related to shared 2D asset pieces
 * 
 *****************************************************************************/

#include "Platform.h"
#include "Graphics2DBinder.h"
#include "Graphics2DLinkerLoadTask.h"
#include "TextureFileLoadTask.h"
#include "TextureAtlasFileLoadTask.h"
#include "SpriteSetFileLoadTask.h"
#include "SpriteAnimSetFileLoadTask.h"
#include "SpriteSetTextureAtlasMappingFileLoadTask.h"
#include "App.h"

Graphics2DBinder::Graphics2DBinder() : _name(NULL), _named(false), _needsFree(false), 
										_numAtlasLinks(0), _maxAtlasLinks(0), 
										_numSetLinks(0), _maxSetLinks(0), 
										_numSetAtlasMappingLinks(0), _maxSetAtlasMappingLinks(0), 
										_numAnimLinks(0), _maxAnimLinks(0),
										_atlasLinks(NULL), _setLinks(NULL), _setAtlasMappingLinks(NULL), _animLinks(NULL),
										_namedAtlasLinks(NULL), _namedSetLinks(NULL), _namedSetAtlasMappingLinks(NULL), _namedAnimLinks(NULL)
									
{

}

Graphics2DBinder::~Graphics2DBinder()
{
	destroyLinks();
}

EOSError Graphics2DBinder::createLinks(Uint32 numAtlas, Uint32 numSetLinks, Uint32 numSetAtlasMappingLinks, Uint32 numAnimLinks, Boolean named)
{
	EOSError	error = EOSErrorNone;

	destroyLinks();

	_named = named;
	_needsFree = true;

	_maxAtlasLinks = numAtlas;
	_numAtlasLinks = 0;

	_maxSetLinks = numSetLinks;
	_numSetLinks = 0;

	_maxSetAtlasMappingLinks = numSetAtlasMappingLinks;
	_numSetAtlasMappingLinks = 0;

	_maxAnimLinks = numAnimLinks;
	_numAnimLinks = 0;

	if (_maxAtlasLinks)
	{
		if (_named)
		{
			_namedAtlasLinks = new NamedTextureAtlasToNamedTextureLink[_maxAtlasLinks];

			if (_namedAtlasLinks)
			{
				memset(_namedAtlasLinks, 0, sizeof(NamedTextureAtlasToNamedTextureLink) * _maxAtlasLinks);
			}
			else
				error = EOSErrorNoMemory;
		}
		else
		{
			_atlasLinks = new TextureAtlasToTextureLink[_maxAtlasLinks];

			if (_atlasLinks)
			{
				memset(_atlasLinks, 0, sizeof(TextureAtlasToTextureLink) * _maxAtlasLinks);
			}
			else
				error = EOSErrorNoMemory;
		}
	}

	if (_maxSetLinks)
	{
		if (_named)
		{
			_namedSetLinks = new NamedSpriteSetToNamedTextureAtlasLink[_maxSetLinks];

			if (_namedSetLinks)
			{
				memset(_namedSetLinks, 0, sizeof(NamedSpriteSetToNamedTextureAtlasLink) * _maxSetLinks);
			}
			else
				error = EOSErrorNoMemory;
		}
		else
		{
			_setLinks = new SpriteSetToTextureAtlasLink[_maxSetLinks];

			if (_setLinks)
			{
				memset(_setLinks, 0, sizeof(SpriteSetToTextureAtlasLink) * _maxSetLinks);
			}
			else
				error = EOSErrorNoMemory;
		}
	}

	if (_maxSetAtlasMappingLinks)
	{
		if (_named)
		{
			_namedSetAtlasMappingLinks = new NamedSpriteSetToNamedTextureAtlasMappingLink[_maxSetAtlasMappingLinks];

			if (_namedSetAtlasMappingLinks)
			{
				memset(_namedSetAtlasMappingLinks, 0, sizeof(NamedSpriteSetToNamedTextureAtlasMappingLink) * _maxSetAtlasMappingLinks);
			}
			else
				error = EOSErrorNoMemory;
		}
		else
		{
			_setAtlasMappingLinks = new SpriteSetToTextureAtlasMappingLink[_maxSetAtlasMappingLinks];

			if (_setAtlasMappingLinks)
			{
				memset(_setAtlasMappingLinks, 0, sizeof(SpriteSetToTextureAtlasMappingLink) * _maxSetAtlasMappingLinks);
			}
			else
				error = EOSErrorNoMemory;
		}
	}

	if (_maxAnimLinks)
	{
		if (_named)
		{
			_namedAnimLinks = new NamedSpriteAnimSetToNamedSpriteSetLink[_maxAnimLinks];

			if (_namedAnimLinks)
			{
				memset(_namedAnimLinks, 0, sizeof(NamedSpriteAnimSetToNamedSpriteSetLink) * _maxAnimLinks);
			}
			else
				error = EOSErrorNoMemory;
		}
		else
		{
			_animLinks = new SpriteAnimSetToSpriteSetLink[_maxAnimLinks];

			if (_animLinks)
			{
				memset(_animLinks, 0, sizeof(SpriteAnimSetToSpriteSetLink) * _maxAnimLinks);
			}
			else
				error = EOSErrorNoMemory;
		}
	}

	if (error != EOSErrorNone)
		destroyLinks();

	return error;
}

void Graphics2DBinder::destroyLinks(void)
{
	Uint32	i;

	if (_needsFree)
	{
		if (_namedAtlasLinks)
		{
			for (i=0;i<_maxAtlasLinks;i++)
			{
				if (_namedAtlasLinks[i].textureName)
					delete _namedAtlasLinks[i].textureName;

				if (_namedAtlasLinks[i].atlasName)
					delete _namedAtlasLinks[i].atlasName;
			}
		}

		if (_atlasLinks)
			delete [] _atlasLinks;

		if (_namedAtlasLinks)
			delete [] _namedAtlasLinks;

		if (_namedSetLinks)
		{
			for (i=0;i<_maxSetLinks;i++)
			{
				if (_namedSetLinks[i].atlasName)
					delete _namedSetLinks[i].atlasName;

				if (_namedSetLinks[i].spriteSetName)
					delete _namedSetLinks[i].spriteSetName;
			}
		}

		if (_setLinks)
			delete [] _setLinks;

		if (_namedSetLinks)
			delete [] _namedSetLinks;

		if (_namedSetAtlasMappingLinks)
		{
			for (i=0;i<_maxSetAtlasMappingLinks;i++)
			{
				if (_namedSetAtlasMappingLinks[i].atlasMappingName)
					delete _namedSetAtlasMappingLinks[i].atlasMappingName;

				if (_namedSetAtlasMappingLinks[i].spriteSetName)
					delete _namedSetAtlasMappingLinks[i].spriteSetName;
			}
		}

		if (_setAtlasMappingLinks)
			delete [] _setAtlasMappingLinks;

		if (_namedSetAtlasMappingLinks)
			delete [] _namedSetAtlasMappingLinks;

		if (_namedAnimLinks)
		{
			for (i=0;i<_maxAnimLinks;i++)
			{
				if (_namedAnimLinks[i].spriteSetName)
					delete _namedAnimLinks[i].spriteSetName;

				if (_namedAnimLinks[i].spriteAnimSetName)
					delete _namedAnimLinks[i].spriteAnimSetName;
			}
		}

		if (_animLinks)
			delete [] _animLinks;

		if (_namedAnimLinks)
			delete [] _namedAnimLinks;
	}

	_numAtlasLinks = _maxAtlasLinks = 0;
	_atlasLinks = NULL;
	_namedAtlasLinks = NULL;

	_numSetLinks = _maxSetLinks = 0;
	_setLinks = NULL;
	_namedSetLinks = NULL;

	_numSetAtlasMappingLinks = _maxSetAtlasMappingLinks = 0;
	_setAtlasMappingLinks = NULL;
	_namedSetAtlasMappingLinks = NULL;

	_numAnimLinks = _maxAnimLinks = 0;
	_animLinks = NULL;
	_namedAnimLinks = NULL;
}

EOSError Graphics2DBinder::setLinks(Uint32 numAtlas, TextureAtlasToTextureLink* atlasLink, Uint32 numSet, SpriteSetToTextureAtlasLink* setLink, Uint32 numSetAtlasMapping, SpriteSetToTextureAtlasMappingLink* setAtlasMappingLink, Uint32 numAnim, SpriteAnimSetToSpriteSetLink* animLink, Boolean persistent)
{
	EOSError	error = EOSErrorNone;

	destroyLinks();

	_needsFree = !persistent;
	_named = false;

	_numAtlasLinks = _maxAtlasLinks = numAtlas;
	_numSetLinks = _maxSetLinks = numSet;
	_numSetAtlasMappingLinks = _maxSetAtlasMappingLinks = numSetAtlasMapping;
	_numAnimLinks = _maxAnimLinks = numAnim;

	if (!persistent)
	{
		if (_numAtlasLinks)
		{
			AssertWDesc(atlasLink != NULL, "Graphics2DBinder::setLinks(): NULL atlasLink");

			if (atlasLink)
			{
				_atlasLinks = new TextureAtlasToTextureLink[_numAtlasLinks];
	
				AssertWDesc(_atlasLinks != NULL, "Graphics2DBinder::setLinks(): NULL atlasLink");
	
				if (_atlasLinks)
					memcpy(_atlasLinks, atlasLink, sizeof(TextureAtlasToTextureLink) * _numAtlasLinks);
				else
					error = EOSErrorNoMemory;
			}
			else
				error = EOSErrorNULL;
		}

		if (_numSetLinks)
		{
			AssertWDesc(setLink != NULL, "Graphics2DBinder::setLinks(): NULL setLink");

			if (setLink)
			{
				_setLinks = new SpriteSetToTextureAtlasLink[_numSetLinks];
	
				AssertWDesc(_setLinks != NULL, "Graphics2DBinder::setLinks(): NULL setLink");
	
				if (_setLinks)
					memcpy(_setLinks, setLink, sizeof(SpriteSetToTextureAtlasLink) * _numSetLinks);
				else
					error = EOSErrorNoMemory;
			}
			else
				error = EOSErrorNULL;
		}

		if (_numSetAtlasMappingLinks)
		{
			AssertWDesc(setAtlasMappingLink != NULL, "Graphics2DBinder::setLinks(): NULL setLink");

			if (setAtlasMappingLink)
			{
				_setAtlasMappingLinks = new SpriteSetToTextureAtlasMappingLink[_numSetAtlasMappingLinks];

				AssertWDesc(_setAtlasMappingLinks != NULL, "Graphics2DBinder::setLinks(): NULL setLink");

				if (_setAtlasMappingLinks)
					memcpy(_setAtlasMappingLinks, setAtlasMappingLink, sizeof(SpriteSetToTextureAtlasMappingLink) * _numSetAtlasMappingLinks);
				else
					error = EOSErrorNoMemory;
			}
			else
				error = EOSErrorNULL;
		}

		if (_numAnimLinks)
		{
			AssertWDesc(animLink != NULL, "Graphics2DBinder::setLinks(): NULL animLink");

			if (animLink)
			{
				_animLinks = new SpriteAnimSetToSpriteSetLink[_numAnimLinks];
	
				AssertWDesc(_animLinks != NULL, "Graphics2DBinder::setLinks(): NULL animLink");
	
				if (_animLinks)
					memcpy(_animLinks, animLink, sizeof(SpriteAnimSetToSpriteSetLink) * _numAnimLinks);
				else
					error = EOSErrorNoMemory;
			}
			else
				error = EOSErrorNULL;
		}

	}
	else
	{
		_atlasLinks = atlasLink;
		_setLinks = setLink;
		_setAtlasMappingLinks = setAtlasMappingLink;
		_animLinks = animLink;
	}

	if (error != EOSErrorNone)
	{
		destroyLinks();
		AssertWDesc(1 == 0, "Graphics2DBinder::setLinks(): error building links");
	}

	return error;
}

EOSError Graphics2DBinder::setLinks(Uint32 numAtlas, NamedTextureAtlasToNamedTextureLink* atlasLink, Uint32 numSet, NamedSpriteSetToNamedTextureAtlasLink* setLink, Uint32 numSetAtlasMapping, NamedSpriteSetToNamedTextureAtlasMappingLink* setAtlasMappingLink, Uint32 numAnim, NamedSpriteAnimSetToNamedSpriteSetLink* animLink, Boolean persistent)
{
	EOSError	error = EOSErrorNone;
	Uint32		i;

	destroyLinks();

	_needsFree = !persistent;
	_named = true;
	_numAtlasLinks = _maxAtlasLinks = numAtlas;
	_numSetLinks = _maxSetLinks = numSet;
	_numSetAtlasMappingLinks = _maxSetAtlasMappingLinks = numSetAtlasMapping;
	_numAnimLinks = _maxAnimLinks = numAnim;

	if (!persistent)
	{
		if (_numAtlasLinks)
		{
			AssertWDesc(atlasLink != NULL, "Graphics2DBinder::setLinks(): NULL atlasLink");

			if (atlasLink)
			{
				_namedAtlasLinks = new NamedTextureAtlasToNamedTextureLink[_numAtlasLinks];
	
				AssertWDesc(_namedAtlasLinks != NULL, "Graphics2DBinder::setLinks(): NULL atlasLink");
	
				if (_namedAtlasLinks)
				{
					for (i=0;i<_numAtlasLinks;i++)
					{
						if (atlasLink[i].textureName)
						{
							_namedAtlasLinks[i].textureName = new Char[strlen(atlasLink[i].textureName) + 1];
	
							if (_namedAtlasLinks[i].textureName)
							{
								strcpy(_namedAtlasLinks[i].textureName, atlasLink[i].textureName);
							}
							else
								error = EOSErrorNoMemory;
						}
						else
							_namedAtlasLinks[i].textureName = NULL;
	
						if (atlasLink[i].atlasName)
						{
							_namedAtlasLinks[i].atlasName = new Char[strlen(atlasLink[i].atlasName) + 1];
	
							if (_namedAtlasLinks[i].atlasName)
							{
								strcpy(_namedAtlasLinks[i].atlasName, atlasLink[i].atlasName);
							}
							else
								error = EOSErrorNoMemory;
						}
						else
							_namedAtlasLinks[i].atlasName = NULL;
	
						if (error != EOSErrorNone)
							break;
					}
				}
				else
					error = EOSErrorNoMemory;
			}
			else
				error = EOSErrorNULL;
		}

		if (_numSetLinks)
		{
			AssertWDesc(setLink != NULL, "Graphics2DBinder::setLinks(): NULL setLink");

			if (setLink)
			{
				_namedSetLinks = new NamedSpriteSetToNamedTextureAtlasLink[_numSetLinks];
	
				AssertWDesc(_namedSetLinks != NULL, "Graphics2DBinder::setLinks(): NULL setLink");
	
				if (_namedSetLinks)
				{
					for (i=0;i<_numSetLinks;i++)
					{
						if (setLink[i].atlasName)
						{
							_namedSetLinks[i].atlasName = new Char[strlen(setLink[i].atlasName) + 1];
	
							if (_namedSetLinks[i].atlasName)
							{
								strcpy(_namedSetLinks[i].atlasName, setLink[i].atlasName);
							}
							else
								error = EOSErrorNoMemory;
						}
						else
							_namedSetLinks[i].atlasName = NULL;
	
						if (setLink[i].spriteSetName)
						{
							_namedSetLinks[i].spriteSetName = new Char[strlen(setLink[i].spriteSetName) + 1];
	
							if (_namedSetLinks[i].spriteSetName)
							{
								strcpy(_namedSetLinks[i].spriteSetName, setLink[i].spriteSetName);
							}
							else
								error = EOSErrorNoMemory;
						}
						else
							_namedSetLinks[i].spriteSetName = NULL;
	
						if (error != EOSErrorNone)
							break;
					}
				}
				else
					error = EOSErrorNoMemory;
			}
			else
				error = EOSErrorNULL;
		}

		if (_numSetAtlasMappingLinks)
		{
			AssertWDesc(setLink != NULL, "Graphics2DBinder::setLinks(): NULL setLink");

			if (setAtlasMappingLink)
			{
				_namedSetAtlasMappingLinks = new NamedSpriteSetToNamedTextureAtlasMappingLink[_numSetAtlasMappingLinks];

				AssertWDesc(_namedSetAtlasMappingLinks != NULL, "Graphics2DBinder::setLinks(): NULL setLink");

				if (_namedSetAtlasMappingLinks)
				{
					for (i=0;i<_numSetAtlasMappingLinks;i++)
					{
						if (setAtlasMappingLink[i].atlasMappingName)
						{
							_namedSetAtlasMappingLinks[i].atlasMappingName = new Char[strlen(setAtlasMappingLink[i].atlasMappingName) + 1];

							if (_namedSetAtlasMappingLinks[i].atlasMappingName)
							{
								strcpy(_namedSetAtlasMappingLinks[i].atlasMappingName, setAtlasMappingLink[i].atlasMappingName);
							}
							else
								error = EOSErrorNoMemory;
						}
						else
							_namedSetAtlasMappingLinks[i].atlasMappingName = NULL;

						if (setAtlasMappingLink[i].spriteSetName)
						{
							_namedSetAtlasMappingLinks[i].spriteSetName = new Char[strlen(setAtlasMappingLink[i].spriteSetName) + 1];

							if (_namedSetAtlasMappingLinks[i].spriteSetName)
							{
								strcpy(_namedSetAtlasMappingLinks[i].spriteSetName, setAtlasMappingLink[i].spriteSetName);
							}
							else
								error = EOSErrorNoMemory;
						}
						else
							_namedSetAtlasMappingLinks[i].spriteSetName = NULL;

						if (error != EOSErrorNone)
							break;
					}
				}
				else
					error = EOSErrorNoMemory;
			}
			else
				error = EOSErrorNULL;
		}

		if (_numAnimLinks)
		{
			AssertWDesc(animLink != NULL, "Graphics2DBinder::setLinks(): NULL animLink");

			if (animLink)
			{
				_namedAnimLinks = new NamedSpriteAnimSetToNamedSpriteSetLink[_numAnimLinks];
	
				AssertWDesc(_namedAnimLinks != NULL, "Graphics2DBinder::setLinks(): NULL animLink");
	
				if (_namedAnimLinks)
				{
					for (i=0;i<_numAnimLinks;i++)
					{
						if (animLink[i].spriteSetName)
						{
							_namedAnimLinks[i].spriteSetName = new Char[strlen(animLink[i].spriteSetName) + 1];
	
							if (_namedAnimLinks[i].spriteSetName)
							{
								strcpy(_namedAnimLinks[i].spriteSetName, animLink[i].spriteSetName);
							}
							else
								error = EOSErrorNoMemory;
						}
						else
							_namedAnimLinks[i].spriteSetName = NULL;
	
						if (animLink[i].spriteAnimSetName)
						{
							_namedAnimLinks[i].spriteAnimSetName = new Char[strlen(animLink[i].spriteAnimSetName) + 1];
	
							if (_namedAnimLinks[i].spriteAnimSetName)
							{
								strcpy(_namedAnimLinks[i].spriteAnimSetName, animLink[i].spriteAnimSetName);
							}
							else
								error = EOSErrorNoMemory;
						}
						else
							_namedAnimLinks[i].spriteAnimSetName = NULL;
	
						if (error != EOSErrorNone)
							break;
					}
				}
				else
					error = EOSErrorNoMemory;
			}
			else
				error = EOSErrorNULL;
		}
	}
	else
	{
		_namedAtlasLinks = atlasLink;
		_namedSetLinks = setLink;
		_namedSetAtlasMappingLinks = setAtlasMappingLink;
		_namedAnimLinks = animLink;
	}

	if (error != EOSErrorNone)
	{
		destroyLinks();
		AssertWDesc(1 == 0, "Graphics2DBinder::setLinks(): error building links");
	}

	return error;
}

EOSError Graphics2DBinder::addAtlasLink(const Char* textureName, const Char* atlasName)
{
	EOSError	error = EOSErrorNone;

	if (_numAtlasLinks < _maxAtlasLinks)
	{
		_namedAtlasLinks[_numAtlasLinks].textureName = new Char[strlen(textureName) + 1];
	
		if (_namedAtlasLinks[_numAtlasLinks].textureName)
			strcpy(_namedAtlasLinks[_numAtlasLinks].textureName, textureName);
	
		_namedAtlasLinks[_numAtlasLinks].atlasName = new Char[strlen(atlasName) + 1];
	
		if (_namedAtlasLinks[_numAtlasLinks].atlasName)
			strcpy(_namedAtlasLinks[_numAtlasLinks].atlasName, atlasName);
	
		_numAtlasLinks++;
	}
	else
	{
		AssertWDesc(1 == 0, "Graphics2DBinder::addAtlasLink() list full.\n");
		error = EOSErrorOutOfBounds;
	}

	return error;
}

EOSError Graphics2DBinder::addSetLink(const Char* atlasName, const Char* spriteSetName)
{
	EOSError	error = EOSErrorNone;

	if (_numSetLinks < _maxSetLinks)
	{
		_namedSetLinks[_numSetLinks].atlasName = new Char[strlen(atlasName) + 1];
	
		if (_namedSetLinks[_numSetLinks].atlasName)
			strcpy(_namedSetLinks[_numSetLinks].atlasName, atlasName);
	
		_namedSetLinks[_numSetLinks].spriteSetName = new Char[strlen(spriteSetName) + 1];
	
		if (_namedSetLinks[_numSetLinks].spriteSetName)
			strcpy(_namedSetLinks[_numSetLinks].spriteSetName, spriteSetName);
	
		_numSetLinks++;
	}
	else
	{
		AssertWDesc(1 == 0, "Graphics2DBinder::addSetLink() list full.\n");
		error = EOSErrorOutOfBounds;
	}

	return error;
}

EOSError Graphics2DBinder::addSetTextureAtlasMappingLink(const Char* atlasMappingName, const Char* spriteSetName)
{
	EOSError	error = EOSErrorNone;

	if (_numSetAtlasMappingLinks < _maxSetAtlasMappingLinks)
	{
		_namedSetAtlasMappingLinks[_numSetAtlasMappingLinks].atlasMappingName = new Char[strlen(atlasMappingName) + 1];
	
		if (_namedSetAtlasMappingLinks[_numSetAtlasMappingLinks].atlasMappingName)
			strcpy(_namedSetAtlasMappingLinks[_numSetAtlasMappingLinks].atlasMappingName, atlasMappingName);
	
		_namedSetAtlasMappingLinks[_numSetAtlasMappingLinks].spriteSetName = new Char[strlen(spriteSetName) + 1];
	
		if (_namedSetAtlasMappingLinks[_numSetAtlasMappingLinks].spriteSetName)
			strcpy(_namedSetAtlasMappingLinks[_numSetAtlasMappingLinks].spriteSetName, spriteSetName);
	
		_numSetAtlasMappingLinks++;
	}
	else
	{
		AssertWDesc(1 == 0, "Graphics2DBinder::addSetTextureAtlasMappingLink() list full.\n");
		error = EOSErrorOutOfBounds;
	}

	return error;
}

EOSError Graphics2DBinder::addAnimLink(const Char* spriteSetName, const Char* spriteAnimSetName)
{
	EOSError	error = EOSErrorNone;

	if (_numAnimLinks < _maxAnimLinks)
	{
		_namedAnimLinks[_numAnimLinks].spriteSetName = new Char[strlen(spriteSetName) + 1];
	
		if (_namedAnimLinks[_numAnimLinks].spriteSetName)
			strcpy(_namedAnimLinks[_numAnimLinks].spriteSetName, spriteSetName);
	
		_namedAnimLinks[_numAnimLinks].spriteAnimSetName = new Char[strlen(spriteAnimSetName) + 1];
	
		if (_namedAnimLinks[_numAnimLinks].spriteAnimSetName)
			strcpy(_namedAnimLinks[_numAnimLinks].spriteAnimSetName, spriteAnimSetName);
	
		_numAnimLinks++;
	}
	else
	{
		AssertWDesc(1 == 0, "Graphics2DBinder::addAnimLink() list full.\n");
		error = EOSErrorOutOfBounds;
	}

	return error;
}

EOSError Graphics2DBinder::addAtlasLink(ObjectID textureID, ObjectID atlasID)
{
	EOSError	error = EOSErrorNone;

	if (_numAtlasLinks < _maxAtlasLinks)
	{
		_atlasLinks[_numAtlasLinks].textureID = textureID;
		_atlasLinks[_numAtlasLinks].atlasID = atlasID;
	
		_numAtlasLinks++;
	}
	else
	{
		AssertWDesc(1 == 0, "Graphics2DBinder::addAtlasLink() list full.\n");
		error = EOSErrorOutOfBounds;
	}

	return error;
}

EOSError Graphics2DBinder::addSetLink(ObjectID atlasID, ObjectID spriteSetID)
{
	EOSError	error = EOSErrorNone;

	if (_numSetLinks < _maxSetLinks)
	{
		_setLinks[_numSetLinks].atlasID = atlasID;
		_setLinks[_numSetLinks].spriteSetID = spriteSetID;
	
		_numSetLinks++;
	}
	else
	{
		AssertWDesc(1 == 0, "Graphics2DBinder::addSetLink() list full.\n");
		error = EOSErrorOutOfBounds;
	}

	return error;
}

EOSError Graphics2DBinder::addSetTextureAtlasMappingLink(ObjectID atlasMappingID, ObjectID spriteSetID)
{
	EOSError	error = EOSErrorNone;

	if (_numSetAtlasMappingLinks < _maxSetAtlasMappingLinks)
	{
		_setAtlasMappingLinks[_numSetAtlasMappingLinks].atlasMappingID = atlasMappingID;
		_setAtlasMappingLinks[_numSetAtlasMappingLinks].spriteSetID = spriteSetID;
	
		_numSetAtlasMappingLinks++;
	}
	else
	{
		AssertWDesc(1 == 0, "Graphics2DBinder::addSetTextureAtlasMappingLink() list full.\n");
		error = EOSErrorOutOfBounds;
	}

	return error;
}

EOSError Graphics2DBinder::addAnimLink(ObjectID spriteSetID, ObjectID spriteAnimSetID)
{
	EOSError	error = EOSErrorNone;

	if (_numAnimLinks < _maxAnimLinks)
	{
		_animLinks[_numAnimLinks].spriteSetID = spriteSetID;
		_animLinks[_numAnimLinks].spriteAnimSetID = spriteAnimSetID;
	
		_numAnimLinks++;
	}
	else
	{
		AssertWDesc(1 == 0, "Graphics2DBinder::addAnimLink() list full.\n");
		error = EOSErrorOutOfBounds;
	}

	return error;
}

EOSError Graphics2DBinder::resetLinkData(void)
{
	EOSError	error = EOSErrorNone;
	Uint32		i;

	for (i=0;i<_maxAtlasLinks;i++)
	{
		if (_namedAtlasLinks[i].textureName)
		{
			delete _namedAtlasLinks[i].textureName;
			_namedAtlasLinks[i].textureName = NULL;
		}

		if (_namedAtlasLinks[i].atlasName)
		{
			delete _namedAtlasLinks[i].atlasName;
			_namedAtlasLinks[i].atlasName = NULL;
		}
	}

	for (i=0;i<_maxSetLinks;i++)
	{
		if (_namedSetLinks[i].atlasName)
		{
			delete _namedSetLinks[i].atlasName;
			_namedSetLinks[i].atlasName = NULL;
		}

		if (_namedSetLinks[i].spriteSetName)
		{
			delete _namedSetLinks[i].spriteSetName;
			_namedSetLinks[i].spriteSetName = NULL;
		}
	}

	for (i=0;i<_maxSetAtlasMappingLinks;i++)
	{
		if (_namedSetAtlasMappingLinks[i].atlasMappingName)
		{
			delete _namedSetAtlasMappingLinks[i].atlasMappingName;
			_namedSetAtlasMappingLinks[i].atlasMappingName = NULL;
		}

		if (_namedSetAtlasMappingLinks[i].spriteSetName)
		{
			delete _namedSetAtlasMappingLinks[i].spriteSetName;
			_namedSetAtlasMappingLinks[i].spriteSetName = NULL;
		}
	}

	for (i=0;i<_maxAnimLinks;i++)
	{
		if (_namedAnimLinks[i].spriteSetName)
		{
			delete _namedAnimLinks[i].spriteSetName;
			_namedAnimLinks[i].spriteSetName = NULL;
		}

		if (_namedAnimLinks[i].spriteAnimSetName)
		{
			delete _namedAnimLinks[i].spriteAnimSetName;
			_namedAnimLinks[i].spriteAnimSetName = NULL;
		}
	}

	_numAtlasLinks = 0;
	_numSetLinks = 0;
	_numAnimLinks = 0;
	_numSetAtlasMappingLinks = 0;

	return error;
}

EOSError Graphics2DBinder::queueGraphics2DLinkerLoadTasks(TaskManager* loader, Boolean persistent, Task** loadtask)
{
	EOSError					error = EOSErrorNone;
	Graphics2DLinkerLoadTask*	task = new Graphics2DLinkerLoadTask;

	if (_named)
		task->setLinks(_numAtlasLinks, _namedAtlasLinks, _numSetLinks, _namedSetLinks, _numSetAtlasMappingLinks, _namedSetAtlasMappingLinks, _numAnimLinks, _namedAnimLinks, persistent);
	else
		task->setLinks(_numAtlasLinks, _atlasLinks, _numSetLinks, _setLinks, _numSetAtlasMappingLinks, _setAtlasMappingLinks, _numAnimLinks, _animLinks, persistent);

	error = loader->addTask(task);

	if (loadtask)
		*loadtask = task;

	return error;
}

EOSError Graphics2DBinder::queueLoadTasks(TaskManager* loader, Boolean texture, Boolean atlas, Boolean spriteSet, Boolean animSet, Uint8* buffer, Uint32 bufferSize, const Char* name, ObjectID objid)
{
	EOSError					error = EOSErrorNone;
	TextureFileLoadTask*		task;
	TextureAtlasFileLoadTask*	atlastask;
	SpriteSetFileLoadTask*		settask;
	SpriteAnimSetFileLoadTask* 	animsettask;
	TextureInfo*				info;
	FileDescriptor				fdesc;
	Char*						fname = NULL;

	if (texture)
	{
		info = _appRefPtr->getTextureManager()->findTextureInfoByRefName(name);
	
		AssertWDesc(info != NULL, "Graphics2DBinder::queueLoadTasks() NULL info");
	
		if (info)
		{
			task = new TextureFileLoadTask;
	
			if (task)
			{
				fdesc.setFileAccessType(FileAccessTypeReadOnly);
				fdesc.setFilename(info->filename);
	
				task->setLoadBuffer(buffer, bufferSize, false);
				task->setFileDescriptor(fdesc);
				task->setTextureInfo(*info);
	
				error = loader->addTask(task);		
			}
			else
				error = EOSErrorNoMemory;
		}
		else
			error = EOSErrorResourceDoesNotExist;
	}

	// Fname allocated just once using _sprite.bin since it is the longest part of the name
	fname = new Char[strlen(name) + strlen("_sprite.bin") + 1];

	if (atlas)
	{
		if (fname)
		{
			atlastask = new TextureAtlasFileLoadTask;
	
			if (atlastask)
			{
				sprintf(fname, "%s.bin", name);
	
				fdesc.setFileAccessType(FileAccessTypeReadOnly);
				fdesc.setFilename(fname);	
	
				atlastask->setAtlasID(objid);
				atlastask->setName(name);
				atlastask->setFileDescriptor(fdesc);
				atlastask->setLoadBuffer(buffer, bufferSize, false);
	
				error = loader->addTask(atlastask);

				if (error == EOSErrorNone)
					error = addAtlasLink(name, name);
			}
			else
				error = EOSErrorNoMemory;
		}
		else
			error = EOSErrorNoMemory;
	}

	if (spriteSet)
	{
		if (fname)
		{
			settask = new SpriteSetFileLoadTask;
	
			if (settask)
			{
				sprintf(fname, "%s_sprite.bin", name);
	
				fdesc.setFileAccessType(FileAccessTypeReadOnly);
				fdesc.setFilename(fname);	
	
				settask->setSpriteSetID(objid);
				settask->setName(name);
				settask->setFileDescriptor(fdesc);
				settask->setLoadBuffer(buffer, bufferSize, false);
	
				error = loader->addTask(settask);		
	
				if (error == EOSErrorNone)
					error = addSetLink(name, name);
			}
			else
				error = EOSErrorNoMemory;			
		}
		else
			error = EOSErrorNoMemory;
	}

	if (animSet)
	{
		if (fname)
		{
			animsettask = new SpriteAnimSetFileLoadTask;
	
			if (animsettask)
			{
				sprintf(fname, "%s_anim.bin", name);
	
				fdesc.setFileAccessType(FileAccessTypeReadOnly);
				fdesc.setFilename(fname);
	
				animsettask->setSpriteAnimSetID(objid);
				animsettask->setName(name);
				animsettask->setFileDescriptor(fdesc);
				animsettask->setLoadBuffer(buffer, bufferSize, false);
	
				error = loader->addTask(animsettask);
	
				if (error == EOSErrorNone)
					addAnimLink(name, name);
			}
			else
				error = EOSErrorNoMemory;			
		}
	}

	if (fname)
		delete fname;

	return error;
}


EOSError Graphics2DBinder::queueLoadTasks(TaskManager* loader, Uint32 numTextures, Boolean spriteSet, Boolean animSet, Uint8* buffer, Uint32 bufferSize, const Char* name, ObjectID objid)
{
	EOSError					error = EOSErrorNone;
	TextureFileLoadTask*		task;
	TextureAtlasFileLoadTask*	atlastask;
	SpriteSetFileLoadTask*		settask;
	SpriteAnimSetFileLoadTask* 	animsettask;
	SpriteSetTextureAtlasMappingFileLoadTask*		mappingtask;
	TextureInfo*				info;
	FileDescriptor				fdesc;
	Char*						fname = NULL;
	Uint32						i;

	// Fname allocated just once using _sprite.bin since it is the longest part of the name
	fname = new Char[strlen(name) + strlen("_mappings.bin") + 1];

	if (fname)
	{
		for (i=0;i<numTextures;i++)
		{
			sprintf(fname, "%s%d", name, i);

			info = _appRefPtr->getTextureManager()->findTextureInfoByRefName(fname);

			AssertWDesc(info != NULL, "Graphics2DBinder::queueLoadTasks() NULL info");

			if (info)
			{
				task = new TextureFileLoadTask;

				if (task)
				{
					fdesc.setFileAccessType(FileAccessTypeReadOnly);
					fdesc.setFilename(info->filename);

					task->setLoadBuffer(buffer, bufferSize, false);
					task->setFileDescriptor(fdesc);
					task->setTextureInfo(*info);

					error = loader->addTask(task);		
				}
				else
				{
					error = EOSErrorNoMemory;
					break;
				}
			}
			else
			{
				error = EOSErrorResourceDoesNotExist;
				break;
			}
		}

		for (i=0;i<numTextures;i++)
		{
			atlastask = new TextureAtlasFileLoadTask;

			if (atlastask)
			{
				sprintf(fname, "%s%d.bin", name, i);

				fdesc.setFileAccessType(FileAccessTypeReadOnly);
				fdesc.setFilename(fname);	

				sprintf(fname, "%s%d", name, i);

				atlastask->setAtlasID(objid);
				atlastask->setName(fname);
				atlastask->setFileDescriptor(fdesc);
				atlastask->setLoadBuffer(buffer, bufferSize, false);

				error = loader->addTask(atlastask);

				if (error == EOSErrorNone)
					error = addAtlasLink(fname, fname);
			}
			else
				error = EOSErrorNoMemory;
		}
	
		if (spriteSet)
		{
			settask = new SpriteSetFileLoadTask;

			if (settask)
			{
				sprintf(fname, "%s_sprite.bin", name);

				fdesc.setFileAccessType(FileAccessTypeReadOnly);
				fdesc.setFilename(fname);	

				settask->setSpriteSetID(objid);
				settask->setName(name);
				settask->setFileDescriptor(fdesc);
				settask->setLoadBuffer(buffer, bufferSize, false);

				error = loader->addTask(settask);		

				if (error == EOSErrorNone)
				{
					for (i=0;i<numTextures;i++)
					{
						sprintf(fname, "%s%d", name, i);

						error = addSetLink(fname, name);

						if (error != EOSErrorNone)
							break;
					}
				}
			}
			else
				error = EOSErrorNoMemory;			
		}

		if (numTextures > 0)
		{
			mappingtask = new SpriteSetTextureAtlasMappingFileLoadTask;

			if (mappingtask)
			{
				sprintf(fname, "%s_mappings.bin", name);

				fdesc.setFileAccessType(FileAccessTypeReadOnly);
				fdesc.setFilename(fname);	

				mappingtask->setName(name);
				mappingtask->setFileDescriptor(fdesc);
				mappingtask->setLoadBuffer(buffer, bufferSize, false);

				loader->addTask(mappingtask);		

				addSetTextureAtlasMappingLink(name, name);
			}
			else
				error = EOSErrorNoMemory;			
		}

		if (animSet)
		{
			animsettask = new SpriteAnimSetFileLoadTask;

			if (animsettask)
			{
				sprintf(fname, "%s_anim.bin", name);

				fdesc.setFileAccessType(FileAccessTypeReadOnly);
				fdesc.setFilename(fname);

				animsettask->setSpriteAnimSetID(objid);
				animsettask->setName(name);
				animsettask->setFileDescriptor(fdesc);
				animsettask->setLoadBuffer(buffer, bufferSize, false);

				error = loader->addTask(animsettask);

				if (error == EOSErrorNone)
					addAnimLink(name, name);
			}
			else
				error = EOSErrorNoMemory;			
		}
	}

	if (fname)
		delete fname;

	return error;
}

