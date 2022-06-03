/******************************************************************************
 *
 * File: EOSHttp.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * EOSHttp. Basic file for the particular platform.
 * 
 *****************************************************************************/

#include "Platform.h"
#include "EOSHttp.h"

#ifdef _PLATFORM_PC

void CALLBACK EOSHttpRequestStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	EOSHttpRequest*	http = (EOSHttpRequest*) dwContext;
	Uint32			error;

	switch (dwInternetStatus)
	{
		case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
			if (http->readHeaders() == EOSErrorNone)
			{
				if (http->queryDataAvailable() != EOSErrorNone)
					http->close();
			}
			break;

		case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
			http->dataAvailable(*((LPDWORD) lpvStatusInformation));
			break;

		case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
			if (http->getHTTPRequestMethod() == EOSHttpRequest::HTTPRequestMethodGet)
			{
				if (http->receiveResponse() != EOSErrorNone)
				{
					http->close();
				}
			}
			else
			{
				http->writeData();
			}
			break;

		case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
			if ((Uint32) dwStatusInformationLength != 0)
			{
				if (http->fillDataBuffer((Uint8*) lpvStatusInformation, (Uint32) dwStatusInformationLength) == EOSErrorNone)
				{
					if (http->queryDataAvailable() != EOSErrorNone)
						http->close();
				}
				else
					http->close();
			}
			break;

		case WINHTTP_CALLBACK_STATUS_RECEIVING_RESPONSE:
			break;

		case WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED:
			break;

		case WINHTTP_CALLBACK_STATUS_SENDING_REQUEST:
			break;

		case WINHTTP_CALLBACK_STATUS_REQUEST_SENT:
			break;

		case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
			if (http->receiveResponse() != EOSErrorNone)
			{
				http->close();
			}
			else
				http->readyForResponseForPostOrPut();
			break;

		case WINHTTP_CALLBACK_STATUS_REDIRECT:
//			http->redirect((UTF16*) lpvStatusInformation);
			break;

		case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
			error = GetLastError();
			http->error(error);
			http->close();
			break;

		default:
			break;
	}
}

#endif /* _PLATFORM_PC */

EOSHttpSession::EOSHttpSession() : _async(false)
{
#ifdef _PLATFORM_PC
	_session = NULL;
#endif /* _PLATFORM_PC */
}

EOSHttpSession::~EOSHttpSession()
{
	close();
}

Boolean EOSHttpSession::isAsync(void)
{
	return _async;
}

Boolean EOSHttpSession::isActive(void)
{
	Boolean active = false;

#ifdef _PLATFORM_PC
	if (_session)
		active = true;
#elif defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
	active = true;
#endif /* _PLATFORM_PC */

	return active;
}

#ifdef _PLATFORM_PC
HINTERNET EOSHttpSession::getSession(void)
{
	return _session;
}
#endif /* _PLATFORM_PC */

EOSError EOSHttpSession::open(Boolean async)
{
	EOSError	error = EOSErrorNone;

#ifdef _PLATFORM_PC
	if (_session == NULL)
	{
		_async = async;

		Uint32	flags = 0;

		if (async)
			flags |= WINHTTP_FLAG_ASYNC;

		_session = WinHttpOpen(L"EOSHttpSession", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
												WINHTTP_NO_PROXY_NAME,
												WINHTTP_NO_PROXY_BYPASS, flags);

		if (_session)
		{
			if (async)
			{

			}
		}
		else
			error = EOSErrorCouldNotOpenResource;
	}
	else
		error = EOSErrorAlreadyInitialized;
#endif /* _PLATFORM_PC */

	return error;
}

EOSError EOSHttpSession::close(void)
{
	EOSError	error = EOSErrorNone;

#ifdef _PLATFORM_PC
	if (_session)
	{
		WinHttpCloseHandle(_session);
		_session = NULL;
		_async = false;
	}
#endif /* _PLATFORM_PC */

	return error;
}

EOSHttpConnection::EOSHttpConnection() : _https(false), _session(NULL)
{
#ifdef _PLATFORM_PC
	_connection = NULL;
#elif defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
	_serverURL = nil;
#endif /* _PLATFORM_PC */
}

EOSHttpConnection::~EOSHttpConnection()
{
	close();
}

Boolean EOSHttpConnection::isActive(void)
{
	Boolean active = false;

#ifdef _PLATFORM_PC
	if (_connection)
		active = true;
#elif defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
	if (_serverURL)
		active = true;
#endif /* _PLATFORM_PC */

	return active;
}

Boolean EOSHttpConnection::isHTTPS(void)
{
	return _https;
}

#ifdef _PLATFORM_PC
HINTERNET EOSHttpConnection::getConnection(void)
{
	return _connection;
}
#endif /* _PLATFORM_PC */

EOSHttpSession* EOSHttpConnection::getSession(void)
{
	return _session;
}

#if defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
EOSError EOSHttpConnection::open(EOSHttpSession* session, NSString* url, Boolean https)
#else
EOSError EOSHttpConnection::open(EOSHttpSession* session, UTF16* url, Boolean https)
#endif
{
	EOSError	error = EOSErrorNone;

	_https = https;

#ifdef _PLATFORM_PC
	if (_connection == NULL)
	{
		Uint32			port = INTERNET_DEFAULT_HTTP_PORT;

		if (session)
		{
#ifdef _NOT_YET
				URL_COMPONENTS	actualURL;
				WCHAR* 	wurl;
				DWORD	len;

				memset(&actualURL, 0, sizeof(URL_COMPONENTS));

				actualURL.dwStructSize = sizeof(URL_COMPONENTS);

				actualURL.dwSchemeLength    = -1;
				actualURL.dwHostNameLength  = -1;
				actualURL.dwUrlPathLength   = -1;
				actualURL.dwExtraInfoLength = -1;

				WinHttpCrackUrl((LPCWSTR) url, wcslen((LPCWSTR) url), 0, &actualURL);

				WinHttpCreateUrl(&actualURL, ICU_ESCAPE, NULL, &len);

				wurl = new WCHAR[len];

				WinHttpCreateUrl(&actualURL, ICU_ESCAPE, wurl, &len);
#endif

				_session = session;

				if (https)
					port = INTERNET_DEFAULT_HTTPS_PORT;

				_connection = WinHttpConnect(_session->getSession(), url, port, 0);

				if (_connection)
				{
				}
				else
					error = EOSErrorCouldNotOpenResource;
		}
		else
			error = EOSErrorResourceNotOpened;
	}
	else
		error = EOSErrorAlreadyInitialized;
#elif defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
	if (session)
	{
		_session = session;
		
		if (_serverURL == nil)
		{
			_serverURL = [[NSURL alloc] initWithString:url];
			
			if (_serverURL == nil)
				error = EOSErrorNoMemory;
			else
				[_serverURL retain];
		}
		else
			error = EOSErrorAlreadyInitialized;
	}
	else
		error = EOSErrorResourceNotOpened;
#endif
	
	return error;
}

EOSError EOSHttpConnection::close(void)
{
	EOSError	error = EOSErrorNone;

#ifdef _PLATFORM_PC
	if (_connection)
	{
		WinHttpCloseHandle(_connection);
		_connection = NULL;
		_session = NULL;
		_https = false;
	}
#elif defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
	if (_serverURL)
	{
		[_serverURL release];
		_serverURL = nil;
	}
#endif /* _PLATFORM_PC */

	return error;
}

#ifdef _PLATFORM_PC
static const WCHAR*	_headerNames[EOSHttpHeaderNameLast] = 
{
	L"Accept",
	L"Accept-Charset",
	L"Accept-Encoding",
	L"Content-Encoding",
	L"Content-Language",
	L"Content-MD5",
	L"Content-Type",
	L"Content-Length",
};
#elif defined(_PLATFORM_IPHONE)
static const NSString* _headerNames[EOSHttpHeaderNameLast] =
{
	@"Accept",
	@"Accept-Charset",
	@"Accept-Encoding",
	@"Content-Encoding",
	@"Content-Language",
	@"Content-MD5",
	@"Content-Type",
	@"Content-Length",
};
#endif /* _PLATFORM_PC */

EOSHttpHeader::EOSHttpHeader()
{
#ifdef _PLATFORM_PC
	Uint32	i;

	for (i=0;i<EOSHttpHeaderNameLast;i++)
	{
		_headerValueWCHAR[i] = NULL;
		_headerValue[i] = NULL;
	}
#elif defined(_PLATFORM_IPHONE)
	Uint32	i;

	for (i=0;i<EOSHttpHeaderNameLast;i++)
	{
		_headerValue[i] = nil;
	}
#endif
}

EOSHttpHeader::~EOSHttpHeader()
{
	reset();
}


EOSHttpHeader& EOSHttpHeader::operator=(const EOSHttpHeader& rhs)
{
	if (this != &rhs)
	{
		reset();

#ifdef _PLATFORM_PC
		Uint32	i;

		for (i=0;i<EOSHttpHeaderNameLast;i++)
		{
			if (rhs._headerValueWCHAR[i])
			{
				_headerValueWCHAR[i] = new WCHAR[wcslen(rhs._headerValueWCHAR[i]) + 1];

				if (_headerValueWCHAR[i])
				{
					wcscpy(_headerValueWCHAR[i], rhs._headerValueWCHAR[i]);
				}
			}

			if (rhs._headerValue[i])
			{
				_headerValue[i] = new Char[strlen(rhs._headerValue[i]) + 1];

				if (_headerValue[i])
				{
					strcpy(_headerValue[i], rhs._headerValue[i]);
				}
			}
		}
#elif defined(_PLATFORM_IPHONE)
		Uint32	i;

		for (i=0;i<EOSHttpHeaderNameLast;i++)
		{
			if (rhs._headerValue[i])
			{
				_headerValue[i] = [[NSString alloc] initWithString:rhs._headerValue[i]];
			}
		}
#endif /* _PLATFORM_PC */

	}

	return *this;
}

void EOSHttpHeader::reset(void)
{
#ifdef _PLATFORM_PC
	Uint32	i;

	for (i=0;i<EOSHttpHeaderNameLast;i++)
	{
		if (_headerValueWCHAR[i])
		{
			delete _headerValueWCHAR[i];
			_headerValueWCHAR[i] = NULL;
		}

		if (_headerValue[i])
		{
			delete _headerValue[i];
			_headerValue[i] = NULL;
		}
	}
#elif defined(_PLATFORM_IPHONE)
	Uint32	i;

	for (i=0;i<EOSHttpHeaderNameLast;i++)
	{
		if (_headerValue[i])
		{
			[_headerValue[i] release];
			_headerValue[i] = nil;
		}
	}
#endif
}

#ifdef _PLATFORM_IPHONE
EOSError EOSHttpHeader::setHeader(EOSHttpHeaderName header, NSString* value)
{
	EOSError	error = EOSErrorNone;
	
	if (_headerValue[header])
	{
		[_headerValue[header] release];
		_headerValue[header] = nil;
	}
	
	if (value)
	{
		_headerValue[header] = [[NSString alloc] initWithString:value];
		
		if (_headerValue[header] == nil)
			error = EOSErrorNoMemory;
	}
	
	return error;
}
#endif

EOSError EOSHttpHeader::setHeader(EOSHttpHeaderName header, const Char* value)
{
	EOSError	error = EOSErrorUnsupported;

#ifdef _PLATFORM_PC
	Uint32	len = strlen(value);

	if (_headerValueWCHAR[header])
	{
		delete _headerValueWCHAR[header];
		_headerValueWCHAR[header] = NULL;
	}

	if (_headerValue[header])
	{
		delete _headerValue[header];
		_headerValue[header] = NULL;
	}

	if (len > 0)
	{
		_headerValueWCHAR[header] = new WCHAR[len + 1];
		_headerValue[header] = new Char[len + 1];

		if (_headerValueWCHAR[header])
		{
			wsprintf(_headerValueWCHAR[header], L"%hs", value);
		}
		else
			error = EOSErrorNoMemory;
		
		if (_headerValue[header])
		{
			strcpy(_headerValue[header], value);
		}
		else
			error = EOSErrorNoMemory;
	}
#elif defined(_PLATFORM_IPHONE)
	if (_headerValue[header])
	{
		[_headerValue[header] release];
		_headerValue[header] = nil;
	}

	if (value)
	{
		_headerValue[header] = [[NSString alloc] initWithCString:value encoding:NSUTF8StringEncoding];
		
		if (_headerValue[header] == nil)
			error = EOSErrorNoMemory;
	}
#endif /* _PLATFORM_PC */

	return error;
}

#ifdef _PLATFORM_IPHONE
NSString* EOSHttpHeader::getHeaderValue(EOSHttpHeaderName header)
#else
Char* EOSHttpHeader::getHeaderValue(EOSHttpHeaderName header)
#endif
{
#ifdef _PLATFORM_PC
	Char*	value = NULL;

	value = _headerValue[header];
#elif defined (_PLATFORM_IPHONE)
	NSString*	value = NULL;

	value = _headerValue[header];
#endif /* _PLATFORM_PC */

	return value;
}

#ifdef _PLATFORM_IPHONE
NSString* EOSHttpHeader::getHeaderAsHeaderString(void)
#else
Char* EOSHttpHeader::getHeaderAsHeaderString(void)
#endif
{
	Uint32	count = 0;
	Uint32	i;
	Uint32	len = 0;

	for (i=0;i<EOSHttpHeaderNameLast;i++)
	{
		if (_headerValue[i])
		{
#ifdef _PLATFORM_IPHONE
			len += [_headerValue[i] length];
#else
			len += strlen(_headerValue[i]);
#endif

			count++;
		}
	}

#ifdef _PLATFORM_PC
	if (count)
	{
		Char*	header;

		header = new Char[len + count * 2 + 1];

		if (header)
		{
			count = 0;

			for (i=0;i<EOSHttpHeaderNameLast;i++)
			{
				if (_headerValue[i])
				{
					sprintf(&header[count], "%s\r\n", _headerValue[i]);

					count += strlen(_headerValue[i]) + 2;
				}
			}

			return header;
		}
	}

	return NULL;
#elif defined (_PLATFORM_IPHONE)
	if (count)
	{
		NSString*	header = nil;

		count = 0;

		for (i=0;i<EOSHttpHeaderNameLast;i++)
		{
			if (_headerValue[i])
			{
				if (header == nil)
				{
					header = [[NSString alloc] initWithFormat:@"%@\r\n", _headerValue[i]];
				}
				else
				{
					[header stringByAppendingFormat:@"%@\r\n", _headerValue[i]];
				}
			}
		}

		return header;
	}

	return nil;
#else
	return NULL;
#endif /* _PLATFORM_PC */
}

#ifdef _PLATFORM_IPHONE
void EOSHttpHeader::setHeaderInNSMutableRequest(NSMutableURLRequest* request)
{
	Uint32	i;
	
	for (i=0;i<EOSHttpHeaderNameLast;i++)
	{
		if (_headerValue[i])
		{
			[request setValue:_headerValue[i] forHTTPHeaderField:_headerNames[i]];
		}
	}
}
#endif

#ifdef _PLATFORM_PC
WCHAR* EOSHttpHeader::getHeaderAsWideHeaderString(void)
{
	Char* 	utf8Header = getHeaderAsHeaderString();
	WCHAR*	header = NULL;

	if (utf8Header)
	{
		header = new WCHAR[strlen(utf8Header) + 1];

		if (header)
		{
			wsprintf(header, L"%hs", utf8Header);
		}

		delete utf8Header;
	}

	return header;
}
#endif

EOSHttpRequest::EOSHttpRequest() : _requestMethod(HTTPRequestMethodGet), _responseToPostOrPut(false), _connection(NULL), _started(false), _completed(false), _error(false), _errorCode(0), _callback(NULL), _callbackObject(NULL), _callbackData(0)
{
#ifdef _PLATFORM_PC
	_request = NULL;
	_asyncCallback = NULL;
	_currXferSize = 0;
	_headerString = NULL;
	_headerStringLength = 0;

#ifdef _DEBUG
	memset(_memo, 0, sizeof(_memo));
#endif

#endif

#if defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
	_delegate = [EOSHttpRequestDelegate alloc];
	
	[_delegate retain];
	[_delegate setEOSHttpRequest:this];
	
	_request = nil;
	_responseData = nil;
	_url = nil;
	_urlConnection = nil;
	_postData = nil;
#endif
	
	_contentType = NULL;
	_data = NULL;
	_dataSize = 0;
}

EOSHttpRequest::~EOSHttpRequest()
{
	close();

	if (_contentType)
		delete [] _contentType;
	
	if (_data)
		delete [] _data;

#ifdef _PLATFORM_PC
	if (_headerString)
		delete [] _headerString;
#endif /* _PLATFORM_PC */
	
#if defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
	if (_delegate)
	{
		[_delegate release];
		_delegate = nil;
	}
	
	if (_url)
	{
		[_url release];
		_url = nil;
	}
	
	if (_urlConnection)
	{
		[_urlConnection release];
		_urlConnection = nil;
	}
	
	if (_request)
	{
		[_request release];
		_request = nil;
	}
	
	if (_responseData)
	{
		[_responseData release];
		_responseData = nil;
	}
	
	if (_postData)
	{
		[_postData release];
		_postData = nil;
	}
#endif
}

#ifdef _PLATFORM_PC

WCHAR* EOSHttpRequest::_buildHeaderFromPair(EOSHttpHeaderName header, const Char* value)
{
	const WCHAR* 	key = NULL;
	WCHAR*			headerPair = NULL;

	key = _headerNames[header];

	if (key && value)
	{
		Uint32	len;

		len = strlen(value);

		if (len > 0)
		{
			headerPair = new WCHAR[wcslen(key) + len + 4];	//	2 for ':', ' ', newline and \0
		}

		if (headerPair)
			wsprintf(headerPair, L"%ls: %hs\n", key, value);
	}

	return headerPair;
}

#endif /* _PLATFORM_PC */

void EOSHttpRequest::_buildHeaderForRequest(void)
{
#ifdef _PLATFORM_PC
	Uint32	i;
	WCHAR*	keyValue;

	for (i=0;i<EOSHttpHeaderNameLast;i++)
	{
		if (_header.getHeaderValue((EOSHttpHeaderName) i))
		{
			keyValue = _buildHeaderFromPair((EOSHttpHeaderName) i, _header.getHeaderValue((EOSHttpHeaderName) i));

			if (keyValue)
			{
				WinHttpAddRequestHeaders(_request, keyValue, -1L, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);

				delete keyValue;
			}
		}
	}
#endif /* _PLATFORM_PC */
}

EOSHttpConnection* EOSHttpRequest::getConnection(void)
{
	return _connection;
}

Boolean EOSHttpRequest::hasCompleted(void)
{
	return _completed;
}

EOSHttpRequest::HTTPRequestMethod EOSHttpRequest::getHTTPRequestMethod(void)
{
	return _requestMethod;
}

Uint16* EOSHttpRequest::getHeader(void)
{
#ifdef _PLATFORM_PC
	return (Uint16*) _headerString;
#else
	return NULL;
#endif
}

Uint32 EOSHttpRequest::getHeaderSize(void)
{
#ifdef _PLATFORM_PC
	return _headerStringLength;
#else
	return 0;
#endif
}

Uint8* EOSHttpRequest::getBodyData(void)
{
	return _data;
}

Uint32 EOSHttpRequest::getBodyDataSize(void)
{
	return _dataSize;
}

void EOSHttpRequest::setCallback(EOSHttpCallback callback, void* object, Uint32 data)
{
	_callback = callback;
	_callbackObject = object;
	_callbackData = data;
}

EOSError EOSHttpRequest::setHeader(EOSHttpHeader& header)
{
	EOSError	error = EOSErrorUnsupported;

	_header = header;

	return error;
}

EOSError EOSHttpRequest::setContentType(const Char* type)
{
	EOSError	error = EOSErrorNone;
	
	if (_contentType)
		delete _contentType;
	
	_contentType = new Char[strlen(type) + 1];
	
	if (_contentType)
		strcpy(_contentType, type);
	else
		error = EOSErrorNoMemory;
	
	return error;
}

EOSError EOSHttpRequest::setHTTPBody(Uint8* body, Uint32 length)
{
	EOSError	error = EOSErrorNone;
	
	if (_data)
		delete _data;
	
	_dataSize = length;
	
	_data = new Uint8[length];
	
	if (_data)
	{
		memcpy(_data, body, _dataSize);
	}
	else
		error = EOSErrorNoMemory;
	
	return error;
}

void EOSHttpRequest::resetDataBuffer(void)
{
	if (_data)
		delete [] _data;

	if (_contentType)
		delete [] _contentType;
	
	_contentType = NULL;
	_data = NULL;
	_dataSize = 0;

	_error = false;
	_errorCode = 0;

#ifdef _PLATFORM_PC
	_currXferSize = 0;

	if (_headerString)
	{
		delete [] _headerString;
		_headerString = NULL;
		_headerStringLength = 0;
	}
#endif /* _PLATFORM_PC */
}

void EOSHttpRequest::error(Uint32 errCode)
{
	_error = true;
	_errorCode = errCode;

	if (_callback)
	{
		_callback(this, _callbackObject, EOSHttpRequestEventError, _callbackData);
	}
}

#if defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
void EOSHttpRequest::didReceiveResponse(NSURLResponse* response)
{
	if (_responseData)
		[_responseData setLength:0];
}

void EOSHttpRequest::didReceiveData(NSData* data)
{
	if (_responseData)
		[_responseData appendData:data];
}

void EOSHttpRequest::didSendBodyData(Uint32 bytesWritten, Uint32 totalBytesWritten, Uint32 totalExpectedBytesWritten)
{
}

void EOSHttpRequest::connectionDidFinishLoading(NSURLConnection* connection)
{
	NSString *dataString = [[NSString alloc] initWithData:_responseData encoding:NSUTF8StringEncoding];
	Uint32	length = [dataString lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
	
	if (length)
	{			
		if (_data)
		{
			delete [] _data;
			_data = NULL;
			_dataSize = 0;
		}

		_data = new Uint8[length + 1];
		
		if (_data)
		{
			strcpy((char*) _data, (const char*) [dataString UTF8String]);
			_dataSize = length;
		}
	}
	
	[dataString release];
	
	if (_callback)
	{
		_callback(this, _callbackObject, EOSHttpRequestEventComplete, _callbackData);
	}
}

void EOSHttpRequest::connectionDidFailWithError(NSURLConnection* connection, NSError* error)
{	
	if (_callback)
	{
		_callback(this, _callbackObject, EOSHttpRequestEventError, _callbackData);
	}
}

#endif

void EOSHttpRequest::cancel(void)
{
#ifdef _PLATFORM_IPHONE
	if (_urlConnection)
	{
		[_urlConnection cancel];
	}
#endif
}

#if defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
EOSError EOSHttpRequest::getRequest(EOSHttpConnection* connection, NSString* resource)
#else
EOSError EOSHttpRequest::getRequest(EOSHttpConnection* connection, Uint16* resource)
#endif
{
	EOSError	error = EOSErrorNone;

	if (connection)
	{
#ifdef _PLATFORM_PC
		if (_request == NULL)
		{
			Uint32	flags = 0;
			BOOL	requested;

			resetDataBuffer();

			_connection = connection;
			_completed = false;
			_started = false;
			_requestMethod = HTTPRequestMethodGet;
			_responseToPostOrPut = false;

			if (connection->getSession()->isAsync())
			{
				if (connection->isHTTPS())
					flags |= WINHTTP_FLAG_SECURE;
				
				_request = WinHttpOpenRequest(connection->getConnection(), L"GET", (WCHAR*) resource, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags); 

				if (_request)
				{
					_asyncCallback = WinHttpSetStatusCallback(_request, (WINHTTP_STATUS_CALLBACK) EOSHttpRequestStatusCallback, WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS | WINHTTP_CALLBACK_FLAG_REDIRECT, NULL);

					//	Should be no pre-set callback
					if (_asyncCallback == NULL)
					{
						_buildHeaderForRequest();

						//	This starts our Async GET
						requested = WinHttpSendRequest(_request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, (DWORD_PTR) this);
					}
				}
				else
					error = EOSErrorCouldNotOpenResource;
			}
			else
			{
				if (connection->isHTTPS())
					flags |= WINHTTP_FLAG_SECURE;

				_request = WinHttpOpenRequest(connection->getConnection(), L"GET", (WCHAR*) resource, NULL, WINHTTP_NO_REFERER, WINHTTP_NO_REFERER, flags); 

				if (_request)
				{
					DWORD	size;
					DWORD	dwDownloaded = 0;
					LPSTR	outBuffer;

					_buildHeaderForRequest();

					requested = WinHttpSendRequest(_request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

					if (requested)
					{
						requested = WinHttpReceiveResponse(_request, NULL);

						if (requested)
						{
							do
							{
								size = 0;

								if( !WinHttpQueryDataAvailable(_request, &size))
									error = EOSErrorUnknown;

								outBuffer = new char[size+1];

								if (outBuffer)
								{
									// Read the data.
									memset(outBuffer, 0, size+1);

									if( !WinHttpReadData(_request, (LPVOID)outBuffer, size, &dwDownloaded ) )
									{
										error = EOSErrorUnknown;
									}
									else
									{
										if (_data == NULL)
										{
											_data = new Uint8[dwDownloaded];

											if (_data)
											{
												memcpy(_data, outBuffer, dwDownloaded);
											}
											else
												error = EOSErrorNoMemory;
										}
										else
										{
											//	Now transfer to our main buffer
											Uint8*	tbuffer = _data;

											_data = new Uint8[_dataSize + dwDownloaded];

											if (_data)
											{
												memcpy(_data, tbuffer, _dataSize);
												memcpy(_data + _dataSize, outBuffer, dwDownloaded);
											}
											else
												error = EOSErrorNoMemory;

											delete [] tbuffer;
										}

										_dataSize += dwDownloaded;
									}

									// Free the memory allocated to the buffer.
									delete [] outBuffer;
								}
								else
									error = EOSErrorNoMemory;

								if (error != EOSErrorNone)
									break;

							} while (size > 0);
						}
					}

					if (error != EOSErrorNone)
					{
						_error = true;
						_errorCode = GetLastError();
					}
				}
				else
					error = EOSErrorCouldNotOpenResource;
			}
		}
		else
			error = EOSErrorAlreadyInitialized;
#elif defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
		if (_request == nil)
		{
			resetDataBuffer();
			
			if (resource)
				_url = [NSURL URLWithString:resource relativeToURL:connection->getServerURL()];
			else
				_url = connection->getServerURL();
			
			[_url retain];
			
			_connection = connection;
			_completed = false;
			_started = false;
			_requestMethod = HTTPRequestMethodGet;
			_responseToPostOrPut = false;
			
			_responseData = [[NSMutableData data] retain];

			_request = [[NSMutableURLRequest alloc] initWithURL:_url cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:45];
			
			[_request retain];

			[_request setHTTPMethod:@"GET"];
			_header.setHeaderInNSMutableRequest(_request);
			
#if 0
//			_urlConnection = [[NSURLConnection alloc] initWithRequest:_request delegate:_delegate];		
			NSURLResponse* response = nil;
			NSError* error = nil;
			NSData* data = [NSURLConnection sendSynchronousRequest:_request returningResponse:&response error:&error];
			
			NSString *dataString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
			Uint32	length = [dataString lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
			
			if (length)
			{				
				_data = new Uint8[length + 1];
				
				if (_data)
				{
					strcpy((char*) _data, (const char*) [dataString UTF8String]);
					_dataSize = length;
				}
			}
			
			NSLog(@"YYY %d %@", [dataString length], dataString);
			[dataString release];
//			[data release];
			
			if (_callback)
			{
				_callback(this, _callbackObject, EOSHttpRequestEventComplete, _callbackData);
			}
#else
			_urlConnection = [[NSURLConnection alloc] initWithRequest:_request delegate:_delegate startImmediately:YES];
#endif
		}
		else
			error = EOSErrorAlreadyInitialized;
#endif /* _PLATFORM_PC */
	}
	else
		error = EOSErrorResourceNotOpened;

	return error;
}

#if defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
EOSError EOSHttpRequest::postRequest(EOSHttpConnection* connection, NSString* resource)
#else
EOSError EOSHttpRequest::postRequest(EOSHttpConnection* connection, Uint16* resource)
#endif
{
	EOSError	error = EOSErrorNone;
	if (connection)
	{
#ifdef _PLATFORM_PC
		if (_request == NULL)
		{
			Uint32	flags = 0;
			BOOL	requested;

			_connection = connection;
			_completed = false;
			_started = false;
			_requestMethod = HTTPRequestMethodPost;
			_responseToPostOrPut = false;

			if (connection->getSession()->isAsync())
			{
				if (connection->isHTTPS())
					flags |= WINHTTP_FLAG_SECURE;

				_request = WinHttpOpenRequest(connection->getConnection(), L"POST", (WCHAR*) resource, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags); 

				if (_request)
				{
					_asyncCallback = WinHttpSetStatusCallback(_request, (WINHTTP_STATUS_CALLBACK) EOSHttpRequestStatusCallback, WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS | WINHTTP_CALLBACK_FLAG_REDIRECT, NULL);

					//	Should be no pre-set callback
					if (_asyncCallback == NULL)
					{
						_buildHeaderForRequest();

						//	This starts our Async GET

						requested = WinHttpSendRequest(_request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, _dataSize, (DWORD_PTR) this);
					}
				}
				else
					error = EOSErrorCouldNotOpenResource;
			}
			else
			{
				if (connection->isHTTPS())
					flags |= WINHTTP_FLAG_SECURE;

				_request = WinHttpOpenRequest(connection->getConnection(), L"POST", (WCHAR*) resource, NULL, WINHTTP_NO_REFERER, WINHTTP_NO_REFERER, flags); 

				if (_request)
				{
					DWORD	size;
					DWORD	dwDownloaded = 0;
					LPSTR	outBuffer;

					_buildHeaderForRequest();

					requested = WinHttpSendRequest(_request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

					if (requested)
					{
						if (WinHttpWriteData(_request, _data, _currXferSize, NULL) == TRUE)
						{
							requested = WinHttpReceiveResponse(_request, NULL);
	
							if (requested)
							{
							}
						}
					}

					if (error != EOSErrorNone)
					{
						_error = true;
						_errorCode = GetLastError();
					}
				}
				else
					error = EOSErrorCouldNotOpenResource;
			}
		}
		else
			error = EOSErrorAlreadyInitialized;
#elif defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
		if (resource)
			_url = [[NSURL alloc] initWithString:resource relativeToURL:connection->getServerURL()];
		else
			_url = connection->getServerURL();
		
		[_url retain];
		
		_connection = connection;
		_completed = false;
		_started = false;
		_requestMethod = HTTPRequestMethodPost;
		_responseToPostOrPut = false;
		
		_responseData = [[NSMutableData data] retain];
		
		_request = [[NSMutableURLRequest alloc] initWithURL:_url cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:45];
		
		[_request retain];
		
		[_request setHTTPMethod:@"POST"];
		_header.setHeaderInNSMutableRequest(_request);
		
		_postData = [NSData dataWithBytes:_data length:_dataSize];
		[_postData retain];
		
		NSString *postLength = [NSString stringWithFormat:@"%d", [_postData length]];
		
		[_request setValue:postLength forHTTPHeaderField:@"Content-Length"];  
		[_request setValue:@"text/xml; charset=utf-8" forHTTPHeaderField:@"Content-Type"];  

		[_request setHTTPBody:_postData];
		
		[postLength release];
		//[postData release];
		
#if 0
		NSURLResponse* response = nil;
		NSError* error = nil;
		NSData* data = [NSURLConnection sendSynchronousRequest:_request returningResponse:&response error:&error];
		
		NSString *dataString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
		NSLog(@"YYY %d %@", [dataString length], dataString);
		
		[dataString release];
		[postData release];
//		[data release];

		if (_callback)
		{
			_callback(this, _callbackObject, EOSHttpRequestEventComplete, _callbackData);
		}
#else
		_urlConnection = [[NSURLConnection alloc] initWithRequest:_request delegate:_delegate startImmediately:YES];
#endif
		
#endif /* _PLATFORM_PC */
	}
	else
		error = EOSErrorResourceNotOpened;

	return error;
}

void EOSHttpRequest::readyForResponseForPostOrPut(void)
{
	_responseToPostOrPut = true;
	resetDataBuffer();
}

EOSError EOSHttpRequest::receiveResponse(void)
{
	EOSError	error = EOSErrorNone;

#ifdef _PLATFORM_PC
	if (WinHttpReceiveResponse(_request, NULL) == FALSE)
		error = EOSErrorUnknown;
#endif /* _PLATFORM_PC */

	return error;
}

EOSError EOSHttpRequest::readHeaders(void)
{
	EOSError	error = EOSErrorNone;

#ifdef _PLATFORM_PC
	DWORD	dwSize = 0;

	if (!WinHttpQueryHeaders(_request, WINHTTP_QUERY_RAW_HEADERS_CRLF,
                              WINHTTP_HEADER_NAME_BY_INDEX, NULL, &dwSize, WINHTTP_NO_HEADER_INDEX))
	{
		//	We are inquiring about size, so we expect an ERROR_INSUFFICIENT_BUFFER error
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			if (_headerString)
				delete [] _headerString;

			_headerString = new WCHAR[dwSize];
			_headerStringLength = dwSize;

			if (_headerString)
			{
				if (WinHttpQueryHeaders(_request, WINHTTP_QUERY_RAW_HEADERS_CRLF,
								WINHTTP_HEADER_NAME_BY_INDEX, _headerString, &_headerStringLength, WINHTTP_NO_HEADER_INDEX))
				{
				}
				else
					error = EOSErrorResourceRead;
			}
		}
	}
	else
		error = EOSErrorResourceRead;
#endif /* _PLATFORM_PC */

	return error;
}

EOSError EOSHttpRequest::queryDataAvailable(void)
{
	EOSError	error = EOSErrorNone;

#ifdef _PLATFORM_PC
	if (_request)
	{
		if (WinHttpQueryDataAvailable(_request, NULL) == FALSE)
		{
			error = EOSErrorResourceNotAvailable;
		}
	}
	else
		error = EOSErrorNotInitialized;
#endif /* _PLATFORM_PC */

	return error;
}

EOSError EOSHttpRequest::dataAvailable(Uint32 size)
{
	EOSError	error = EOSErrorNone;

#ifdef _PLATFORM_PC
	_currXferSize = size;

	if (_currXferSize == 0)
	{
		//	We are completely done!!
		if (_data == NULL || _dataSize)
		{
			if (_callback)
			{
				_callback(this, _callbackObject, EOSHttpRequestEventComplete, _callbackData);
			}
		}
	}
	else
	{
		switch (_requestMethod)
		{
			case HTTPRequestMethodPost:
			case HTTPRequestMethodPut:
				if (_responseToPostOrPut)
					error = readData();
				else
					error = writeData();
				break;

			default:
				error = readData();
				break;
		}

		if (error != EOSErrorNone)
			close();
	}
#endif /* _PLATFORM_PC */

	return error;
}

EOSError EOSHttpRequest::readData(void)
{
	EOSError	error = EOSErrorNone;

#ifdef _PLATFORM_PC
	if (_request)
	{
		Uint8*		buffer = new Uint8[_currXferSize + 1];
		Uint8*		tbuffer;

		if (buffer)
		{
			memset(buffer, 0, _currXferSize + 1);
		
			if (WinHttpReadData(_request, buffer, _currXferSize, NULL) == FALSE)
			{
				if (_data)
				{
					tbuffer = new Uint8[_dataSize + _currXferSize + 1];
				}
				else
				{
					tbuffer = new Uint8[_currXferSize + 1];
					_dataSize = 0;
				}

				if (tbuffer)
				{
					memcpy(tbuffer, _data, _dataSize);
					memcpy(&tbuffer[_dataSize], buffer, _currXferSize);

					_dataSize += _currXferSize;
					tbuffer[_dataSize] = '\0';

					delete  _data;

					_data = tbuffer;
				}

				delete [] buffer;
				error = EOSErrorResourceRead;
			}
		}
		else
			error = EOSErrorNoMemory;
	}
	else
		error = EOSErrorNotInitialized;
#endif /* _PLATFORM_PC */

	return error;
}

EOSError EOSHttpRequest::writeData(void)
{
	EOSError	error = EOSErrorNone;

#ifdef _PLATFORM_PC
	if (_request)
	{
		_currXferSize = _dataSize;

		if (WinHttpWriteData(_request, _data, _currXferSize, NULL) == FALSE)
		{
			error = (EOSError) GetLastError();
			error = EOSErrorResourceRead;
		}
	}
	else
		error = EOSErrorNotInitialized;
#endif /* _PLATFORM_PC */

	return error;
}

EOSError EOSHttpRequest::fillDataBuffer(Uint8* data, Uint32 size)
{
	EOSError	error = EOSErrorNone;

#ifdef _PLATFORM_PC
	if (size != 0)
	{
		_currXferSize = size;

#ifdef _DEBUG
		Char*	tbuffer = new Char[size+1];;
	
		memcpy(tbuffer, data, size);
		tbuffer[size] = 0;
    	OutputDebugStringA(tbuffer);

		delete [] tbuffer;
#endif

		if (_data == NULL)
		{
			_data = data;
		}
		else
		{
			Uint8*	oldBuffer = _data;
	
			_data = new Uint8[_dataSize + _currXferSize + 1];
	
			if (_data)
			{
				memcpy(_data, oldBuffer, _dataSize);
				memcpy(_data + _dataSize, data, _currXferSize);
			}
			else
			{
				error = EOSErrorNoMemory;
			}
	
			delete [] oldBuffer;
			delete [] data;
		}
	
		_dataSize += _currXferSize;

		if (_data)
			_data[_dataSize] = '\0';
	}
#endif

	return error;
}

EOSError EOSHttpRequest::close(void)
{
	EOSError	error = EOSErrorNone;

	_header.reset();

#ifdef _PLATFORM_PC
	if (_request)
	{
		WinHttpSetStatusCallback(_request, NULL, NULL, NULL);

		WinHttpCloseHandle(_request);

		_request = NULL;
		_connection = NULL;
	}
#elif defined(_PLATFORM_IPHONE) || defined(_PLATFORM_MAC)
	
	if (_url)
	{
//		[_url release];
		_url = nil;
	}

	if (_urlConnection)
	{
		[_urlConnection release];
		_urlConnection = nil;
	}
	
	if (_request)
	{
		[_request release];		
		_request = nil;
	}
	
	if (_responseData)
	{
		[_responseData release];
		_responseData = nil;
	}
		
	if (_postData)
	{
		[_postData release];
		_postData = nil;
	}
	
	_connection = NULL;
#endif /* _PLATFORM_PC */

	return error;
}

