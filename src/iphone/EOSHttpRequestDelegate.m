//
//  EOSHttpRequestDelegate.m
//  eosTest
//
//  Created by Benjamin Lee on 10/22/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#import "EOSHttpRequestDelegate.h"
#include "EOSHttp.h"

@implementation EOSHttpRequestDelegate

- (void) setEOSHttpRequest:(EOSHttpRequest*)http
{
	_request = http;
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response {
	if (_request)
		_request->didReceiveResponse(response);
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
	if (_request)
		_request->didReceiveData(data);
}

-(void)connection:(NSURLConnection*) connection didSendBodyData:(NSInteger)bytesWritten totalBytesWritten:(NSInteger)totalBytesWritten totalBytesExpectedToWrite:(NSInteger)totalBytesExpectedToWrite
{
	if (_request)
		_request->didSendBodyData(bytesWritten, totalBytesWritten, totalBytesExpectedToWrite);
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
	if (_request)
		_request->connectionDidFinishLoading(connection);
}

-(void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	if (_request)
		_request->connectionDidFailWithError(connection, error);
}

@end