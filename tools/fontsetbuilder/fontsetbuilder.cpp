/******************************************************************************
 *
 * File: fontsetbuilder.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * fontsetbuilder
 * Build font set data
 * 
 *****************************************************************************/

#include "Platform.h"
#include "FontSetXML.h"
#include "toolkit/TextureAtlasXML.h"

#include <libxml/xmlreader.h>

FontSetXMLParser		_xmlParser;
char*					_app_name;

void copyright(void)
{
	fprintf(stdout, "%s xml output\n", _app_name);
	fprintf(stdout, "Copyright (C) 2009 by 2n Productions, All Rights Reserved.\n");
	fprintf(stdout, "EOS Engine\n");
}

void usage(void)
{
	fprintf(stdout, "Usage: %s xml output\n", _app_name);
}

int main(int argc, char** argv)
{
	int					error = 0;

	_app_name = argv[0];

	if (argc == 3)
	{
		error = _xmlParser.parse(argv[1]);

		if (error == EOSErrorNone)
			error = _xmlParser.buildDB();

		if (error == EOSErrorNone)
			error = _xmlParser.exportDB(argv[2]);
	}
	else
	{
		error = EOSErrorIllegalArguments;
	}

	return error;
}

