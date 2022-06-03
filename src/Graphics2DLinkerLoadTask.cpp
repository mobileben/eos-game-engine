/******************************************************************************
 *
 * File: Graphics2DLinkerTask.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Task to resolve sprite anim set data
 * 
 *****************************************************************************/

#include "Platform.h"
#include "EOSError.h"
#include "TaskManager.h"
#include "TextureManager.h"
#include "SpriteSetManager.h"
#include "SpriteAnimSetManager.h"
#include "App.h"
#include "TextureAtlasManager.h"
#include "Graphics2DLinkerLoadTask.h"

Graphics2DLinkerLoadTask::Graphics2DLinkerLoadTask() : _named(false), _needsFree(false), _numAtlasLinks(0), _numSetLinks(0), _numSetAtlasMappingLinks(0), _numAnimLinks(0), _atlasLinks(NULL), _setLinks(NULL), _setAtlasMappingLinks(NULL), _animLinks(NULL), _namedAtlasLinks(NULL), _namedSetLinks(NULL), _namedSetAtlasMappingLinks(NULL), _namedAnimLinks(NULL)
{
}

Graphics2DLinkerLoadTask::~Graphics2DLinkerLoadTask()
{
	destroy();
}

void Graphics2DLinkerLoadTask::destroy(void)
{
	Uint32	i;

	if (_needsFree)
	{
		for (i=0;i<_numAtlasLinks;i++)
		{
			if (_namedAtlasLinks[i].textureName)
				delete _namedAtlasLinks[i].textureName;

			if (_namedAtlasLinks[i].atlasName)
				delete _namedAtlasLinks[i].atlasName;
		}

		for (i=0;i<_numSetLinks;i++)
		{
			if (_namedSetLinks[i].atlasName)
				delete _namedSetLinks[i].atlasName;

			if (_namedSetLinks[i].spriteSetName)
				delete _namedSetLinks[i].spriteSetName;
		}

		for (i=0;i<_numSetAtlasMappingLinks;i++)
		{
			if (_namedSetAtlasMappingLinks[i].atlasMappingName)
				delete _namedSetAtlasMappingLinks[i].atlasMappingName;

			if (_namedSetAtlasMappingLinks[i].spriteSetName)
				delete _namedSetAtlasMappingLinks[i].spriteSetName;
		}

		for (i=0;i<_numAnimLinks;i++)
		{
			if (_namedAnimLinks[i].spriteSetName)
				delete _namedAnimLinks[i].spriteSetName;

			if (_namedAnimLinks[i].spriteAnimSetName)
				delete _namedAnimLinks[i].spriteAnimSetName;
		}

		if (_atlasLinks)
		{
			delete _atlasLinks;
			_atlasLinks = NULL;
		}

		if (_setLinks)
		{
			delete _setLinks;
			_setLinks = NULL;
		}

		if (_setAtlasMappingLinks)
		{
			delete _setAtlasMappingLinks;
			_setAtlasMappingLinks = NULL;
		}

		if (_animLinks)
		{
			delete _animLinks;
			_animLinks = NULL;
		}

		if (_namedAtlasLinks)
		{
			delete _namedAtlasLinks;
			_namedAtlasLinks = NULL;
		}

		if (_namedSetLinks)
		{
			delete _namedSetLinks;
			_namedSetLinks = NULL;
		}

		if (_namedSetAtlasMappingLinks)
		{
			delete _namedSetAtlasMappingLinks;
			_namedSetAtlasMappingLinks = NULL;
		}

		if (_namedAnimLinks)
		{
			delete _namedAnimLinks;
			_namedAnimLinks = NULL;
		}

		_numAtlasLinks = 0;
		_numSetLinks = 0;
		_numSetAtlasMappingLinks = 0;
		_numAnimLinks = 0;
	}
	else
	{
		_numAtlasLinks = 0;
		_numSetLinks = 0;
		_numSetAtlasMappingLinks = 0;
		_numAnimLinks = 0;

		_atlasLinks = NULL;
		_setLinks = NULL;
		_setAtlasMappingLinks = NULL;
		_animLinks = NULL;
		_namedAtlasLinks = NULL;
		_namedSetLinks = NULL;
		_namedSetAtlasMappingLinks = NULL;
		_namedAnimLinks = NULL;
	}

	_named = false;
}

void Graphics2DLinkerLoadTask::setLinks(Uint32 numAtlas, TextureAtlasToTextureLink* atlasLink, Uint32 numSet, SpriteSetToTextureAtlasLink* setLink, Uint32 numSetAtlasMapping, SpriteSetToTextureAtlasMappingLink* setAtlasMappingLink, Uint32 numAnim, SpriteAnimSetToSpriteSetLink* animLink, Boolean persistent)
{
	EOSError	error = EOSErrorNone;

	destroy();

	_needsFree = !persistent;
	_named = false;
	_numAtlasLinks = numAtlas;
	_numSetLinks = numSet;
	_numSetAtlasMappingLinks = numSetAtlasMapping;
	_numAnimLinks = numAnim;

	if (!persistent)
	{
		if (_numAtlasLinks)
		{
			AssertWDesc(atlasLink != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL atlasLink");

			if (atlasLink)
			{
				_atlasLinks = new TextureAtlasToTextureLink[_numAtlasLinks];
	
				AssertWDesc(_atlasLinks != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL atlasLink");
	
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
			AssertWDesc(setLink != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL setLink");

			if (setLink)
			{
				_setLinks = new SpriteSetToTextureAtlasLink[_numSetLinks];
	
				AssertWDesc(_setLinks != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL setLink");
	
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
			AssertWDesc(setAtlasMappingLink != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL setLink");

			if (setAtlasMappingLink)
			{
				_setAtlasMappingLinks = new SpriteSetToTextureAtlasMappingLink[_numSetAtlasMappingLinks];

				AssertWDesc(_setAtlasMappingLinks != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL setLink");

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
			AssertWDesc(animLink != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL animLink");

			if (animLink)
			{
				_animLinks = new SpriteAnimSetToSpriteSetLink[_numAnimLinks];
	
				AssertWDesc(_animLinks != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL animLink");
	
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
		_animLinks = animLink;
	}

	if (error != EOSErrorNone)
	{
		AssertWDesc(1 == 0, "Graphics2DLinkerLoadTask::setLinks(): error building links");
	}
}

void Graphics2DLinkerLoadTask::setLinks(Uint32 numAtlas, NamedTextureAtlasToNamedTextureLink* atlasLink, Uint32 numSet, NamedSpriteSetToNamedTextureAtlasLink* setLink, Uint32 numSetAtlasMapping, NamedSpriteSetToNamedTextureAtlasMappingLink* setAtlasMappingLink, Uint32 numAnim, NamedSpriteAnimSetToNamedSpriteSetLink* animLink, Boolean persistent)
{
	EOSError	error = EOSErrorNone;
	Uint32		i;

	destroy();

	_needsFree = !persistent;
	_named = true;
	_numAtlasLinks = numAtlas;
	_numSetLinks = numSet;
	_numSetAtlasMappingLinks = numSetAtlasMapping;
	_numAnimLinks = numAnim;

	if (!persistent)
	{
		if (_numAtlasLinks)
		{
			AssertWDesc(atlasLink != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL atlasLink");

			if (atlasLink)
			{
				_namedAtlasLinks = new NamedTextureAtlasToNamedTextureLink[_numAtlasLinks];
	
				AssertWDesc(_namedAtlasLinks != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL atlasLink");
	
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
			AssertWDesc(setLink != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL setLink");

			if (setLink)
			{
				_namedSetLinks = new NamedSpriteSetToNamedTextureAtlasLink[_numSetLinks];
	
				AssertWDesc(_namedSetLinks != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL setLink");
	
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
			AssertWDesc(setAtlasMappingLink != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL setLink");

			if (setAtlasMappingLink)
			{
				_namedSetAtlasMappingLinks = new NamedSpriteSetToNamedTextureAtlasMappingLink[_numSetAtlasMappingLinks];

				AssertWDesc(_namedSetAtlasMappingLinks != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL setLink");

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
			AssertWDesc(animLink != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL animLink");

			if (animLink)
			{
				_namedAnimLinks = new NamedSpriteAnimSetToNamedSpriteSetLink[_numAnimLinks];
	
				AssertWDesc(_namedAnimLinks != NULL, "Graphics2DLinkerLoadTask::setLinks(): NULL animLink");
	
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
		AssertWDesc(1 == 0, "Graphics2DLinkerLoadTask::setLinks(): error building links");
	}
}

EOSError Graphics2DLinkerLoadTask::epilogue(void)
{
	EOSError		error = EOSErrorNone;
	Uint32			i;
	Texture*		texture; 
	TextureAtlas*	textureAtlas;
	SpriteAnimSet*	spriteAnimSet;
	SpriteSet*		spriteSet;
	SpriteSetTextureAtlasMapping*		spriteSetAtlasMapping;

	AssertWDesc(getTaskManager() != NULL, "Task Manager is NULL.");

	if (getTaskManager())
	{
		AssertWDesc(getTaskManager()->getApp() != NULL, "App is NULL.");

		if (getTaskManager()->getApp())
		{
			if (_named)
			{
				for (i=0;i<_numAtlasLinks;i++)
				{
					textureAtlas = getTaskManager()->getApp()->getTextureAtlasManager()->findTextureAtlasFromName(_namedAtlasLinks[i].atlasName);

					if (textureAtlas)
					{
						texture = getTaskManager()->getApp()->getTextureManager()->findTextureFromName(_namedAtlasLinks[i].textureName);

						if (texture)
						{
							textureAtlas->setTexture(texture);
						}
						else
							error = EOSErrorNULL;
					}
					else
						error = EOSErrorNULL;
				}

				for (i=0;i<_numSetLinks;i++)
				{
					textureAtlas = getTaskManager()->getApp()->getTextureAtlasManager()->findTextureAtlasFromName(_namedSetLinks[i].atlasName);

					if (textureAtlas)
					{
						spriteSet = getTaskManager()->getApp()->getSpriteSetManager()->findSpriteSetFromName(_namedSetLinks[i].spriteSetName);

						if (spriteSet)
						{
							spriteSet->addTextureAtlas(textureAtlas);
						}
						else
							error = EOSErrorNULL;
					}
					else
						error = EOSErrorNULL;
				}

				for (i=0;i<_numSetAtlasMappingLinks;i++)
				{
					spriteSetAtlasMapping = getTaskManager()->getApp()->getSpriteSetManager()->findSpriteSetTextureAtlasMappingFromName(_namedSetAtlasMappingLinks[i].atlasMappingName);
					
					if (spriteSetAtlasMapping)
					{
						spriteSet = getTaskManager()->getApp()->getSpriteSetManager()->findSpriteSetFromName(_namedSetAtlasMappingLinks[i].spriteSetName);

						if (spriteSet)
						{
							spriteSet->setTextureAtlasMapping(spriteSetAtlasMapping);
						}
						else
							error = EOSErrorNULL;
					}
					else
						error = EOSErrorNULL;
				}

				for (i=0;i<_numAnimLinks;i++)
				{
					spriteSet = getTaskManager()->getApp()->getSpriteSetManager()->findSpriteSetFromName(_namedAnimLinks[i].spriteSetName);

					if (spriteSet)
					{
						spriteAnimSet = getTaskManager()->getApp()->getSpriteAnimSetManager()->findSpriteAnimSetFromName(_namedAnimLinks[i].spriteAnimSetName);

						if (spriteAnimSet)
						{
							spriteAnimSet->setSpriteSet(spriteSet);
						}
						else
							error = EOSErrorNULL;
					}
					else
						error = EOSErrorNULL;
				}
			}
			else
			{
				for (i=0;i<_numAtlasLinks;i++)
				{
					textureAtlas = getTaskManager()->getApp()->getTextureAtlasManager()->findTextureAtlasFromRefID(_atlasLinks[i].atlasID);

					if (textureAtlas)
					{
						texture = getTaskManager()->getApp()->getTextureManager()->findTextureFromRefID(_atlasLinks[i].textureID);

						if (texture)
						{
							textureAtlas->setTexture(texture);
						}
						else
							error = EOSErrorNULL;
					}
					else
						error = EOSErrorNULL;
				}

				for (i=0;i<_numSetLinks;i++)
				{
					textureAtlas = getTaskManager()->getApp()->getTextureAtlasManager()->findTextureAtlasFromRefID(_setLinks[i].atlasID);

					if (textureAtlas)
					{
						spriteSet = getTaskManager()->getApp()->getSpriteSetManager()->findSpriteSetFromRefID(_setLinks[i].spriteSetID);

						if (spriteSet)
						{
							spriteSet->addTextureAtlas(textureAtlas);
						}
						else
							error = EOSErrorNULL;
					}
					else
						error = EOSErrorNULL;
				}

				for (i=0;i<_numSetAtlasMappingLinks;i++)
				{
					spriteSetAtlasMapping = getTaskManager()->getApp()->getSpriteSetManager()->findSpriteSetTextureAtlasMappingFromRefID(_setAtlasMappingLinks[i].atlasMappingID);

					if (spriteSetAtlasMapping)
					{
						spriteSet = getTaskManager()->getApp()->getSpriteSetManager()->findSpriteSetFromRefID(_setAtlasMappingLinks[i].spriteSetID);

						if (spriteSet)
						{
							spriteSet->setTextureAtlasMapping(spriteSetAtlasMapping);
						}
						else
							error = EOSErrorNULL;
					}
					else
						error = EOSErrorNULL;
				}

				for (i=0;i<_numAnimLinks;i++)
				{
					spriteSet = getTaskManager()->getApp()->getSpriteSetManager()->findSpriteSetFromRefID(_animLinks[i].spriteSetID);

					if (spriteSet)
					{
						spriteAnimSet = getTaskManager()->getApp()->getSpriteAnimSetManager()->findSpriteAnimSetFromRefID(_animLinks[i].spriteAnimSetID);

						if (spriteAnimSet)
						{
							spriteAnimSet->setSpriteSet(spriteSet);
						}
						else
							error = EOSErrorNULL;
					}
					else
						error = EOSErrorNULL;
				}
			}
		}
		else
			error = EOSErrorNULL;
	}
	else
		error = EOSErrorNULL;


	return error;
}

