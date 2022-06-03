/******************************************************************************
 *
 * File: EOSHttp.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * EOSHttp. Basic file for the particular platform.
 * 
 *****************************************************************************/

#ifndef __EOSHTTP_H__
#define __EOSHTTP_H__

#include "Platform.h"
#include "EOSError.h"
#ifdef _PLATFORM_PC
#include <WinHttp.h>
#endif /* _PLATFORM_PC */

#if defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
#import "EOSHttpRequestDelegate.h"
#endif

typedef enum
{
	EOSHttpRequestEventNone = 0,
	EOSHttpRequestEventError,
	EOSHttpRequestEventAborted,
	EOSHttpRequestEventComplete,
} EOSHttpRequestEvent;

typedef enum
{
	EOSHttpHeaderNameAccept = 0,
	EOSHttpHeaderNameAcceptCharset,
	EOSHttpHeaderNameAcceptEncoding,
	EOSHttpHeaderNameContentEncoding,
	EOSHttpHeaderNameContentLanguage,
	EOSHttpHeaderNameContentMD5,
	EOSHttpHeaderNameContentType,
	EOSHttpHeaderNameContentLength,
	EOSHttpHeaderNameLast
} EOSHttpHeaderName;

class EOSHttpRequest;
class EOSHttpConnection;

typedef void (*EOSHttpCallback)(EOSHttpRequest* http, void* object, Uint32 event, Uint32 data);

class EOSHttpSession : public EOSObject
{
public:

private:
	Boolean			_async;

#ifdef _PLATFORM_PC
	HINTERNET		_session;
#endif /* _PLATFORM_PC */

public:
	EOSHttpSession();
	~EOSHttpSession();

	Boolean		isAsync(void);

	Boolean		isActive(void);

#ifdef _PLATFORM_PC
	HINTERNET	getSession(void);
#endif /* _PLATFORM_PC */
	
	EOSError	open(Boolean async);
	EOSError	close(void);
};

class EOSHttpConnection : public EOSObject
{
public:

private:
	Boolean			_https;
	EOSHttpSession*	_session;

#ifdef _PLATFORM_PC
	HINTERNET	_connection;
#elif defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
	NSURL*		_serverURL;
#endif /* _PLATFORM_PC */

public:
	EOSHttpConnection();
	~EOSHttpConnection();

	Boolean			isActive(void);

	Boolean			isHTTPS(void);
	EOSHttpSession*	getSession(void);

#ifdef _PLATFORM_PC
	HINTERNET		getConnection(void);
#elif defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
	inline NSURL*	getServerURL(void) { return _serverURL; }
#endif /* _PLATFORM_PC */

#if defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
	EOSError		open(EOSHttpSession* session, NSString* url, Boolean https);
#else
	EOSError		open(EOSHttpSession* session, UTF16* url, Boolean https);
#endif /* _PLATFORM_IPHONE || _PLATFORM_MAC */
	EOSError		close(void);
};

class EOSHttpHeader : public EOSObject
{
private:
#ifdef _PLATFORM_PC
	WCHAR*	_headerValueWCHAR[EOSHttpHeaderNameLast];
	Char*	_headerValue[EOSHttpHeaderNameLast];
#elif defined(_PLATFORM_IPHONE)
	NSString*	_headerValue[EOSHttpHeaderNameLast]; 
#endif /* _PLATFORM_PC */

public:
	EOSHttpHeader();
	~EOSHttpHeader();

	EOSHttpHeader& operator=(const EOSHttpHeader& rhs);

	void		reset(void);

	EOSError	setHeader(EOSHttpHeaderName header, const Char* value);

#ifdef _PLATFORM_IPHONE
	EOSError	setHeader(EOSHttpHeaderName header, NSString* value);
	NSString*	getHeaderValue(EOSHttpHeaderName header);
	NSString*	getHeaderAsHeaderString(void);
	void		setHeaderInNSMutableRequest(NSMutableURLRequest* request);
#else
	Char*		getHeaderValue(EOSHttpHeaderName header);
	Char*		getHeaderAsHeaderString(void);
#endif

#ifdef _PLATFORM_PC
	WCHAR*		getHeaderAsWideHeaderString(void);
#endif
};

class EOSHttpRequest : public EOSObject
{
public:
	typedef enum
	{
		HTTPRequestMethodGet = 0,
		HTTPRequestMethodPost,
		HTTPRequestMethodPut,
	} HTTPRequestMethod;

private:
	HTTPRequestMethod		_requestMethod;
	Boolean					_responseToPostOrPut;
	EOSHttpConnection*		_connection;
	EOSHttpHeader			_header;
	Boolean					_started;
	Boolean					_completed;
	Boolean					_error;
	Uint32					_errorCode;

	EOSHttpCallback			_callback;
	void*					_callbackObject;
	Uint32					_callbackData;

#ifdef _PLATFORM_PC
	HINTERNET				_request;
	WINHTTP_STATUS_CALLBACK	_asyncCallback;
	DWORD					_currXferSize;
	WCHAR*					_headerString;
	DWORD					_headerStringLength;

#ifdef _DEBUG
	UTF16					_memo[256];
#endif

#elif defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
	EOSHttpRequestDelegate*		_delegate;
	NSMutableURLRequest*		_request;
	NSURL*						_url;
	NSURLConnection*			_urlConnection;
	NSMutableData*				_responseData;
	NSData*						_postData;
#endif /* _PLATFORM_PC */

	Char*					_targetResource;
	Char*					_contentType;
	
	Uint8*					_data;
	Uint32					_dataSize;

#ifdef _PLATFORM_PC
	WCHAR*					_buildHeaderFromPair(EOSHttpHeaderName header, const Char* name);
#endif /* _PLATFORM_PC */

	void					_buildHeaderForRequest(void);

public:
	EOSHttpRequest();
	~EOSHttpRequest();

	EOSHttpConnection*	getConnection(void);
	Boolean				hasCompleted(void);	
	HTTPRequestMethod	getHTTPRequestMethod(void);

	Uint16*				getHeader(void);
	Uint32				getHeaderSize(void);
	Uint8*				getBodyData(void);
	Uint32				getBodyDataSize(void);

	void				setCallback(EOSHttpCallback callback, void* object, Uint32 data = 0);
	EOSError			setHeader(EOSHttpHeader& header);
	EOSError			setContentType(const Char* type);
	EOSError			setHTTPBody(Uint8* body, Uint32 length);
	
	void				resetDataBuffer(void);

	void				error(Uint32 errCode);

	void				readyForResponseForPostOrPut(void);
	EOSError			receiveResponse(void);
	EOSError			readHeaders(void);
	EOSError			queryDataAvailable(void);
	EOSError			dataAvailable(Uint32 size);
	EOSError			readData(void);
	EOSError			writeData(void);
	EOSError			fillDataBuffer(Uint8* data, Uint32 size);

#if defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
	void				didReceiveResponse(NSURLResponse* response);
	void				didReceiveData(NSData* data);
	void				connectionDidFinishLoading(NSURLConnection* connection);
	void				connectionDidFailWithError(NSURLConnection* connection, NSError* error);
	void				didSendBodyData(Uint32 bytesWritten, Uint32 totalBytesWritten, Uint32 totalExpectedBytesWritten);
#endif

	void				cancel(void);

#if defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
	EOSError			getRequest(EOSHttpConnection* connection, NSString* resource = NULL);
	EOSError			postRequest(EOSHttpConnection* connection, NSString* resource = NULL);
#else
	EOSError			getRequest(EOSHttpConnection* connection, Uint16* resource = NULL);
	EOSError			postRequest(EOSHttpConnection* connection, Uint16* resource = NULL);
#endif
	
	EOSError			close(void);
};

#endif /* __EOSHTTP_H__ */
