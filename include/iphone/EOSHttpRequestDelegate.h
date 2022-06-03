//
//  EOSHttpRequestDelegate.h
//  eosTest
//
//  Created by Benjamin Lee on 10/22/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#pragma once

#include "App.h"

class EOSHttpRequest;

@interface EOSHttpRequestDelegate : NSObject
{
	EOSHttpRequest*	_request;
}

- (void) setEOSHttpRequest:(EOSHttpRequest*)http;

@end
