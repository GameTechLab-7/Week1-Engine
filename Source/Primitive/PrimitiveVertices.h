﻿#pragma once
#include "Core/HAL/PlatformType.h"
#include "Core/Container/Array.h"

struct FVertexSimple
{
    float X, Y, Z;    // Position
    float R, G, B, A; // Color
};

struct FGeometryData
{
	TArray<FVertexSimple> Vertices;
	TArray<uint32> Indices;
};

enum class EPrimitiveType : uint8
{
	EPT_None,
	EPT_Triangle,
	EPT_Cube,
	EPT_Sphere,
	EPT_Line,
	EPT_Cylinder,
	EPT_Cone,
	EPT_Max,
};

extern FVertexSimple LineVertices[2];
extern FVertexSimple CubeVertices[36];
extern FVertexSimple SphereVertices[2400];
extern FVertexSimple TriangleVertices[3];
