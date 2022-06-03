/******************************************************************************
 *
 * File: Vector2D.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Vector 2D
 * 
 *****************************************************************************/

#include "Vector2D.h"
#include "App.h"

Sint32	vector2DGetDirection(Vector2D& v0, Vector2D& v1)
{
	if ((v0.x * v1.y - v0.y * v1.x) < 0.0)
		return -1;
	else
		return 1;
}

Float32	vector2DGetVectorAngle(Vector2D& ray0, Vector2D& ray1)
{
	float	dx, dy;
	float	slope = 0.0;
	float	theta = 0.0;

	dx = ray1.x - ray0.x;
	dy = ray1.y - ray0.y;

	if (dx != 0.0)
	{
		slope = dy / dx;
		theta = atan(slope) * 180.0 / M_PI;
	}
	else
	{
		//	DX == 0.0, vertical line
		if (dy < 0.0)
			theta = -90.0F;
		else
			theta = 90.0F;
	}

	//	Find the qudrants
	if (dx < 0.0)
	{
		theta += 180.0F;
	}

	return theta;
}

void	vector2DDrawRay(ColorRGBA& rgba, Point2D& pt0, Point2D& pt1)
{
	_appRefPtr->getRenderer()->drawLine(rgba, pt0, pt1);
}

