/******************************************************************************
 *
 * File: animBuilder.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * animBuilder
 * Generates anim data from generated from Flash
 * 
 *****************************************************************************/

#include "Platform.h"
#include "toolkit\GraphicsToolKit.h"
#include "EOSError.h"
#include "toolkit/SpriteAnimDataSet.h"

#include <wchar.h>
#include <iterator>
#include <vector>
#include <map>
#include <algorithm>

char*	_app_name = "animBuilder";
char*	_path = NULL;
char*	_outprefix = NULL;	

Boolean _comments = true;

Uint32	_version = 0;
Uint32	_spriteSetID = 0;

void copyright(void)
{
	fprintf(stdout, "%s\n", _app_name);
	fprintf(stdout, "Copyright (C) 2009 by 2n Productions, All Rights Reserved.\n");
	fprintf(stdout, "EOS Engine\n");
}

void usage(void)
{
	fprintf(stdout, "Usage: %s tmpl outprefix path\n", _app_name);
}

int main(int argc, char** argv)
{
	int					error = 0;
	SpriteAnimDataSet	sprAnimDataSet;
	char				fname[256];

	copyright();

	if (argc == 4)
	{
		_outprefix = argv[2];
		_path = argv[3];

		sprAnimDataSet.setComments(_comments);

		error = sprAnimDataSet.loadDataSet(argv[1]);

		if (error == EOSErrorNone)
		{
			sprintf(fname, "%s.ctext", _outprefix);
			error = sprAnimDataSet.exportDataSetAsText(fname, _outprefix);

			sprintf(fname, "%s.bin", _outprefix);
			error = sprAnimDataSet.exportDataSetAsBIN(fname);
		}
	}
	else
	{
		usage();
		error = EOSErrorIllegalArguments;
	}

	return error;
}
