#pragma once

#include "SceneNode.hpp"
#include <GL/gl.h>

class GeometryNode : public SceneNode {
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);

	Material material;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;

	GLuint depthMap;
	GLuint depthMapFB;
};
