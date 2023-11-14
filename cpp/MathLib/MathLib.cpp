#include "pch.h" 
#include "MathLib.h"

#include <DirectXMesh.h>

bool ComputeTangentFrame(
	const uint32_t* indices, size_t numFacees,
	Vector3* positions, Vector3* normals, Vector2* texcoords,
	size_t numVertices, Vector3* tangents, Vector3* biTangents)
{
	HRESULT hr = DirectX::ComputeTangentFrame(indices, numFacees,
		positions, normals, texcoords, numVertices, tangents, biTangents);
	return hr == S_OK ? true : false;
}