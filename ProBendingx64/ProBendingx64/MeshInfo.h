#pragma once
#include "OgreVector3.h"
#include "OgreAxisAlignedBox.h"

class MeshInfo
{
public:
	size_t vertex_count,index_count;
	Ogre::Vector3* vertices;
	unsigned long* indices;
	float sphereRadius;
	Ogre::AxisAlignedBox aaBB;

	MeshInfo()
	{
		vertex_count = index_count = 0;
		vertices = NULL;
		indices = NULL;
	}

	MeshInfo(size_t _vertex_count, size_t _index_count, Ogre::Vector3* _vertices, unsigned long* _indices,
		float _sphereRadius, Ogre::AxisAlignedBox& _aaBB)
		:vertex_count(_vertex_count), index_count(_index_count), vertices(_vertices), indices(_indices),
		sphereRadius(_sphereRadius), aaBB(_aaBB)
	{

	}

	~MeshInfo()
	{
		if(vertices)
			delete[] vertices;

		if(indices)
			delete[] indices;
	}
};