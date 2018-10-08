#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <GL/gl.h>

// Material surface properties to be used as input into a local illumination model
// (e.g. the Phong Reflection Model).
struct Material {

	Material()
			: kd(glm::vec3(0.0f)),
			  ks(glm::vec3(0.0f)),
			  shininess(0.0f),
			  alpha(1.0f),
			  hasTexture(0)
			  {}

	std::string tex;
	bool hasTexture;

	// Diffuse reflection coefficient
	glm::vec3 kd;

	// Specular reflection coefficient
	glm::vec3 ks;

	// Material shininess constant.  Larger positive values model surfaces that
	// are smoother or mirror-like.  Smaller positive values model rougher surfaces.
	float shininess;
	float alpha;

	// Scale factor for texture, the more texScaling is, the more same texture repeats.
	float texScaling;

	// Displacement params:
	// x: displacement mapping factor
	// y: Perlin factor;
	// z: Perlin frequency;
	// w: Perlin scaling factor;
	glm::vec4 dparams;
	GLuint perlin;
};
