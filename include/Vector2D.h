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

#ifndef __VECTOR2D_H__
#define __VECTOR2D_H__

#include "Graphics.h"

typedef Point2D Vector2D;

Sint32	vector2DGetDirection(Vector2D& v0, Vector2D& v1);
Float32	vector2DGetVectorAngle(Vector2D& ray0, Vector2D& ray1);
void	vector2DDrawRay(Point2D& pt0, Point2D& pt1, ColorRGBA& rgba);

#endif /* __VECTOR2D_H__ */
