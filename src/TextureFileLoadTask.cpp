/******************************************************************************
 *
 * File: TextureFileLoadTask.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Texture File Load Task Base Class
 * 
 *****************************************************************************/

#include "TextureFileLoadTask.h"
#include "App.h"
#include "TaskManager.h"
#include "TextureManager.h"

TextureFileLoadTask::TextureFileLoadTask()
{
	_texture = NULL;

	_textureInfo.refID = InvalidObjectID;
	_textureInfo.refName = NULL;
	_textureInfo.filename = NULL;
	_textureInfo.width = 0;
	_textureInfo.height = 0;
	_textureInfo.colorFormat = TextureFormatTGA;
	_textureInfo.state = 0;
	_textureInfo.blending = 0;

}

TextureFileLoadTask::~TextureFileLoadTask()
{
	if (_textureInfo.refName)
	{
		delete _textureInfo.refName;
		_textureInfo.refName = NULL;
	}

	if (_textureInfo.filename)
	{
		delete _textureInfo.filename;
		_textureInfo.filename = NULL;
	}
}

void TextureFileLoadTask::setTextureInfo(TextureInfo& textureInfo)
{
	if (_textureInfo.refName)
	{
		delete _textureInfo.refName;
		_textureInfo.refName = NULL;
	}

	if (_textureInfo.filename)
	{
		delete _textureInfo.filename;
		_textureInfo.filename = NULL;
	}

	_textureInfo = textureInfo;

	//	We need to do an explicit set of the name, else any temp strings will get lost
	_textureInfo.refName = NULL;

	if (textureInfo.refName)
	{
		_textureInfo.refName = new Char[strlen(textureInfo.refName) + 1];

		if (_textureInfo.refName)
			strcpy(_textureInfo.refName, textureInfo.refName);
	}
	_textureInfo.filename = NULL;

	if (textureInfo.filename)
	{
		_textureInfo.filename = new Char[strlen(textureInfo.filename) + 1];

		if (_textureInfo.filename)
			strcpy(_textureInfo.filename, textureInfo.filename);
	}
}

EOSError TextureFileLoadTask::prologue(void)
{
	return FileLoadTask::prologue();
}

EOSError TextureFileLoadTask::epilogue(void)
{
	TextureManager*	textureMgr;
	EOSError 		error = EOSErrorNone;
#ifdef _USE_PVR_OPENGLES
	PVRTHeader*		header;
#endif /* _USE_PVR_OPENGLES */

	AssertWDesc(getTaskManager() != NULL, "Task Manager is NULL.");

	if (getTaskManager())
	{
		AssertWDesc(getTaskManager()->getApp() != NULL, "App is NULL.");

		if (getTaskManager()->getApp())
		{
			textureMgr = getTaskManager()->getApp()->getTextureManager();

			if (_textureInfo.refName)
				_texture = textureMgr->findTextureFromName(_textureInfo.refName);
			else
				_texture = textureMgr->findTextureFromRefID(_textureInfo.refID);

			if (_texture == NULL)
				_texture = textureMgr->getFreeTexture();

			AssertWDesc(_texture != NULL, "Could not allocate Texture.");

			if (_texture)
			{
				switch (_textureInfo.colorFormat)
				{
					case TextureFormatTGA:
#ifdef _USE_OPENGL
						getTaskManager()->getApp()->getGfx()->lockGLContext();
#endif /* _USE_OPENGL */
						error = _texture->createFromTGAImage((Uint8*) _buffer, getFileSize(), _textureInfo.state);
#ifdef _USE_OPENGL
						getTaskManager()->getApp()->getGfx()->unlockGLContext();
#endif /* _USE_OPENGL */
						break;

					case TextureFormatGL_RGB_888:
					case TextureFormatGL_RGBA_8888:
					case TextureFormatGL_RGB_565:
					case TextureFormatGL_RGBA_5551:
					case TextureFormatGL_RGBA_4444:
#ifdef _USE_OPENGL
						getTaskManager()->getApp()->getGfx()->lockGLContext();
						error = _texture->createFromGLImage((GLubyte*) _buffer, (GLint) _textureInfo.width, (GLint) _textureInfo.height, _texture->convertTextureFormatToGLFormat((TextureFormat) _textureInfo.colorFormat), _texture->convertTextureFormatToGLType((TextureFormat) _textureInfo.colorFormat), _textureInfo.state);
						getTaskManager()->getApp()->getGfx()->unlockGLContext();
#endif /* _USE_OPENGL */
						break;

#ifdef _USE_OPENGL
					case TextureFormatGL_P8_RGBA_8888:
						getTaskManager()->getApp()->getGfx()->lockGLContext();

						error = _texture->createPALTextureFromGLImage((GLubyte*) _buffer, (GLint) _textureInfo.width, (GLint) _textureInfo.height, 256, _texture->convertTextureFormatToGLFormat((TextureFormat) _textureInfo.colorFormat), _textureInfo.state);

						getTaskManager()->getApp()->getGfx()->unlockGLContext();
						break;

					case TextureFormatGL_P4_RGBA_4444:
						getTaskManager()->getApp()->getGfx()->lockGLContext();

						error = _texture->createPALTextureFromGLImage((GLubyte*) _buffer, (GLint) _textureInfo.width, (GLint) _textureInfo.height, 16, _texture->convertTextureFormatToGLFormat((TextureFormat) _textureInfo.colorFormat), _textureInfo.state);

						getTaskManager()->getApp()->getGfx()->unlockGLContext();
						break;

#ifdef _USE_PVR_OPENGLES
					case TextureFormatRAWPVRTC_4BPP_RGBA:
					case TextureFormatRAWPVRTC_4BPP_RGB:
						header = (PVRTHeader*) _buffer;

						getTaskManager()->getApp()->getGfx()->lockGLContext();

						error = _texture->createPALTextureFromGLImage((GLubyte*) _buffer, (GLint) header->width, (GLint) header->height, 16, _texture->convertTextureFormatToGLFormat((TextureFormat) _textureInfo.colorFormat), _textureInfo.state);

						getTaskManager()->getApp()->getGfx()->unlockGLContext();
						break;

					case TextureFormatPVRTC_4BPP_RGBA:
					case TextureFormatPVRTC_4BPP_RGB:
						header = (PVRTHeader*) _buffer;

						getTaskManager()->getApp()->getGfx()->lockGLContext();

						error = _texture->createPALTextureFromGLImage((GLubyte*) _buffer + sizeof(PVRTHeader), (GLint) header->width, (GLint) header->height, 16, _texture->convertTextureFormatToGLFormat((TextureFormat) _textureInfo.colorFormat), _textureInfo.state);

						getTaskManager()->getApp()->getGfx()->unlockGLContext();
						break;

					case TextureFormatPVRTC_2BPP_RGBA:
					case TextureFormatPVRTC_2BPP_RGB:
						header = (PVRTHeader*) _buffer;
						
						getTaskManager()->getApp()->getGfx()->lockGLContext();

						error = _texture->createPALTextureFromGLImage((GLubyte*) _buffer + sizeof(PVRTHeader), (GLint) header->width, (GLint) header->height, 4, _texture->convertTextureFormatToGLFormat((TextureFormat) _textureInfo.colorFormat), _textureInfo.state);

						getTaskManager()->getApp()->getGfx()->unlockGLContext();
						break;

					case TextureFormatRAWPVRTC_2BPP_RGBA:
					case TextureFormatRAWPVRTC_2BPP_RGB:
						header = (PVRTHeader*) _buffer;

						getTaskManager()->getApp()->getGfx()->lockGLContext();

						error = _texture->createPALTextureFromGLImage((GLubyte*) _buffer, (GLint) header->width, (GLint) header->height, 4, _texture->convertTextureFormatToGLFormat((TextureFormat) _textureInfo.colorFormat), _textureInfo.state);

						getTaskManager()->getApp()->getGfx()->unlockGLContext();
						break;
#endif /* _USE_PVR_OPENGLES */

#endif /* _USE_OPENGL */

					default:
						AssertWDesc(1 == 0, "Unsupported color format.");
						error = EOSErrorUnsupported;
						break;
				}

				if (error == EOSErrorNone)
				{
					_texture->setRefID(_textureInfo.refID);
					_texture->setName(_textureInfo.refName);
				}
			}
			else
				error = EOSErrorResourceNotAvailable;
		}
		else
			error = EOSErrorNULL;
	}
	else
		error = EOSErrorNULL;

	return error;
}

