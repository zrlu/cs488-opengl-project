#include "A5.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "loadBMP.hpp"
#include "Texture.hpp"
#include "misc.hpp"
#include <string>
#include <random>
#include "Image.hpp"
#include "Perlin.hpp"
#include <glm/ext.hpp>
#include <cmath>
#include "skybox_data.h"
#include "ParticleMaster.hpp"
#include <random>
#include <lodepng/lodepng.h>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A5::A5(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_uvAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vbo_vertexUVs(0),
	  zbufferEnabled(1),
	  backfaceCulling(1),
	  frontfaceCulling(0),
	  enableTransparency(1),
	  enableNormalShading(0),
	  enableNormalMapping(1),
	  enableUVShading(0),
	  enableTangentShading(0),
	  enableTessellation(1),
	  enableSpecularMapping(1),
	  enableAmbientOcclusionMapping(1),
	  enableShadow(1),
	  enableWireframeMode(0),
	  enableTextureMapping(1),
	  lCtrlPressed(0),
	  lShiftPressed(0),
	  eye(vec3(0.0f, 1.0f, 0.0f)),
	  cameraVector(glm::normalize(vec3(0, 1.0, 9.0) - eye)),
	  hour(12.00),
	  time_autorun(0),
	  viewAnglePhi(0),
	  viewAngleTheta(0),
	  particleOrigin(vec3(-1, 4.0, 10)),
	  particleOriginTurbulance(0.2),
	  particleTurbulance(5.0),
	  particleSize(0.5),
	  particleVelocityBias(vec3(0.0, 15.0, 0.0)),
	  particleGravity(0.0),
	  particleLifetime(0.40),
	  particleNumberPerFrame(30),
	  particleColor1(vec3(1.0, 0.5, 0.0)),
	  particleColor2(vec3(1, 1, 1)),
	  enableFreeMode(0),
	  wasdState(0)	
	{
	TOGGLE_NAMES = {
		toggle_mapping_t("Show GUI", 			"G", 		&show_gui		 	 ),
		toggle_mapping_t("Play", 			    "SPACE", 	&time_autorun 	     )
	};
	TOGGLE_KEYMAP = {
		{ GLFW_KEY_G,		&show_gui            },
		{ GLFW_KEY_SPACE, 	&time_autorun        }
	};
	TOGGLE_NAMES_CTRL = {
		toggle_mapping_t("Z-buffer", 					"CTRL+Z", 	&zbufferEnabled		  			),
		toggle_mapping_t("Backface culling", 			"CTRL+B", 	&backfaceCulling	  			),
		toggle_mapping_t("Frontface culling",			"CTRL+F", 	&frontfaceCulling	  			),
		toggle_mapping_t("Transparency",				"CTRL+T", 	&enableTransparency	  			),
		toggle_mapping_t("Normal Shading",				"CTRL+N", 	&enableNormalShading  			),
		toggle_mapping_t("UV Shading",		    		"CTRL+U", 	&enableUVShading	  			),
		toggle_mapping_t("Tangent Shading",				"CTRL+G", 	&enableTangentShading 			),
		toggle_mapping_t("Normal Mapping",				"CTRL+M", 	&enableNormalMapping  			),
		toggle_mapping_t("Tessellation",	    		"CTRL+S", 	&enableTessellation   			),
		toggle_mapping_t("Specular Mapping",			"CTRL+P",   &enableSpecularMapping			),
		toggle_mapping_t("Ambient Occlusion Mapping",	"CTRL+A",   &enableAmbientOcclusionMapping	),
		toggle_mapping_t("Shadow",	        			"CTRL+H",   &enableShadow	                ),
		toggle_mapping_t("Wireframe Mode",	        	"CTRL+W",   &enableWireframeMode	        ),
		toggle_mapping_t("Texture Mapping",             "CTRL+X",   &enableTextureMapping	        ),
		toggle_mapping_t("Free Mode",                   "CTRL+R",   &enableFreeMode	                )
	};
	
	TOGGLE_KEYMAP_CTRL = {
		{ GLFW_KEY_Z, &zbufferEnabled      				},
		{ GLFW_KEY_B, &backfaceCulling     				},
		{ GLFW_KEY_F, &frontfaceCulling    				},
		{ GLFW_KEY_T, &enableTransparency  				},
		{ GLFW_KEY_N, &enableNormalShading 				},
		{ GLFW_KEY_M, &enableNormalMapping 				},
		{ GLFW_KEY_U, &enableUVShading     				},
		{ GLFW_KEY_G, &enableTangentShading				},
		{ GLFW_KEY_S, &enableTessellation  				},
		{ GLFW_KEY_P, &enableSpecularMapping  			},
		{ GLFW_KEY_A, &enableAmbientOcclusionMapping  	},
		{ GLFW_KEY_H, &enableShadow                     },
		{ GLFW_KEY_W, &enableWireframeMode              },
		{ GLFW_KEY_X, &enableTextureMapping             },
		{ GLFW_KEY_R, &enableFreeMode                   }

	};
	FUNCTION_NAMES = {
		func_mapping_t("Quit",					"Q", 	&A5::close				)
	};
	FUNCTION_KEYMAP = {
		{ GLFW_KEY_Q, &A5::close 				}
	};
	FUNCTION_NAMES_CTRL = {
	};
	FUNCTION_KEYMAP_CTRL = {
	};
}

//----------------------------------------------------------------------------------------
// Destructor
A5::~A5()
{

}

// static void cursor_enter_callback(GLFWwindow* window, int entered) {
//     if (entered)
//     {
//     }
//     else
//     {
// 		int width, height;
// 		glfwGetWindowSize(window, &width, &height);
// 		glfwSetCursorPos(window, width / 2, height / 2);
//     }
// }

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A5::init()
{
	// Set the background colour.
	// glClearColor(0.9, 0.35, 0.35, 1.0);
	// glfwSetCursorEnterCallback(m_window, cursor_enter_callback);
	glfwWindowHint(GLFW_SAMPLES, 4);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_meshData);
	glGenVertexArrays(1, &m_vao_skyboxData);
	glGenVertexArrays(1, &m_vao_quadData);

	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	glob_t *globdata = new glob_t;
	glob("Assets/Objects/*.obj", 0, nullptr, globdata);
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator(globdata->gl_pathc, globdata->gl_pathv));
	delete globdata;

	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	calculateViewMatrix();

	initLightSources(m_rootNode->lights);

	initShadowFrameBuffer();

	initNodes(*m_rootNode);

	m_cubemap = CubemapTexture{
		string("Assets/Textures"),
		string("posx.png"),
		string("negx.png"),
		string("posy.png"),
		string("negy.png"),
		string("posz.png"),
		string("negz.png"),				
	};

	m_cubemap_night = CubemapTexture{
		string("Assets/Textures"),
		string("nightRight.png"),
		string("nightLeft.png"),
		string("nightTop.png"),
		string("nightBottom.png"),
		string("nightBack.png"),
		string("nightFront.png"),				
	};

	m_cubemap.Load(glGenerateMipmap);
	CHECK_GL_ERRORS;
	
	m_cubemap_night.Load(glGenerateMipmap);
	CHECK_GL_ERRORS;

	int flareTextureID;
	unsigned int flareWidth, flareHeight;
	for (int i = 1; i <= 9; ++i)
	{
		flareTextureID = createAndUploadTexturePNG(("Assets/Textures/tex" + to_string(i) + ".png").c_str(), flareWidth, flareHeight);
		flare_textures[i - 1] = flareTextureID;
	}

	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	printf("Max supported patch vertices %d\n", MaxPatchVertices);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	CHECK_GL_ERRORS;

	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.

	reset();
}

//----------------------------------------------------------------------------------------
void A5::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could not open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A5::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( "Assets/Shaders/VertexShader.vert" );
	m_shader.attachGeometryShader( "Assets/Shaders/GeometryShader.geom" );
	m_shader.attachFragmentShader( "Assets/Shaders/FragmentShader.frag" );
	m_shader.attachTessellationControlShader( "Assets/Shaders/TessellationControlShader.tesc" );
	m_shader.attachTessellationEvaluationShader( "Assets/Shaders/TessellationEvaluationShader.tese" );
	m_shader.link();

	m_shader_depth.generateProgramObject();
	m_shader_depth.attachVertexShader("Assets/Shaders/ShadowMap.vert" );
	m_shader_depth.attachFragmentShader("Assets/Shaders/ShadowMap.frag" );
	m_shader_depth.link();

	m_shader_skybox.generateProgramObject();
	m_shader_skybox.attachVertexShader("Assets/Shaders/Skybox.vert" );
	m_shader_skybox.attachFragmentShader("Assets/Shaders/Skybox.frag" );
	m_shader_skybox.link();

	m_shader_particle.generateProgramObject();
	m_shader_particle.attachVertexShader("Assets/Shaders/Particle.vert" );
	m_shader_particle.attachFragmentShader("Assets/Shaders/Particle.frag" );
	m_shader_particle.link();

	m_shader_sun.generateProgramObject();
	m_shader_sun.attachVertexShader("Assets/Shaders/Sun.vert" );
	m_shader_sun.attachFragmentShader("Assets/Shaders/Sun.frag" );
	m_shader_sun.link();

	m_shader_lensFlare.generateProgramObject();
	m_shader_lensFlare.attachVertexShader("Assets/Shaders/LensFlare.vert" );
	m_shader_lensFlare.attachFragmentShader("Assets/Shaders/LensFlare.frag" );
	m_shader_lensFlare.link();
}

//----------------------------------------------------------------------------------------
void A5::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		// Enable the vertex shader attribute location for "uv" when rendering.
		m_uvAttribLocation = m_shader.getAttribLocation("uv");
		glEnableVertexAttribArray(m_uvAttribLocation);

		CHECK_GL_ERRORS;
	}

	{
		glBindVertexArray(m_vao_skyboxData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader_skybox.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	{
		glBindVertexArray(m_vao_quadData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_quadPositionAttribLocation = m_shader_particle.getAttribLocation("position");
		glEnableVertexAttribArray(m_quadPositionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A5::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex UV data
	{
		glGenBuffers(1, &m_vbo_vertexUVs);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexUVs);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexUVBytes(),
				meshConsolidator.getVertexUVDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

		// Generate VBO to store cubemap vertex data
	{
		glGenBuffers(1, &m_vbo_skyboxVertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_skyboxVertexPositions);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*108, SKYBOX_VERTEX_DATA, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	{

		float PARTICLE_VERTICES[12] = {-0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, -0.5};

		glGenBuffers(1, &m_vbo_quadPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_quadPositions);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*12, PARTICLE_VERTICES, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A5::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexUVs" into the
	// "uv" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexUVs);
	glVertexAttribPointer(m_uvAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);


	glBindVertexArray(m_vao_skyboxData);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_skyboxVertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);


	glBindVertexArray(m_vao_quadData);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_quadPositions);
	glVertexAttribPointer(m_quadPositionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	CHECK_GL_ERRORS;

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A5::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 10000.0f);
}


//----------------------------------------------------------------------------------------
void A5::calculateViewMatrix() {
	m_view = glm::lookAt(
		eye, 
		eye + cameraVector,
		vec3(0.0f, 1.0f, 0.0f)
	);
}

//----------------------------------------------------------------------------------------
void A5::initLightSources(const std::list<Light *> & lights) {
	m_lights = lights;
	for (auto &light: m_lights)
	{
		cerr << "Light " << *light << endl;
	}
	m_lights_size = std::min<int>(MAX_LIGHTS, m_lights.size());
}

//----------------------------------------------------------------------------------------
void A5::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

		// add light sources
		{
			location = m_shader.getUniformLocation("num_lights");
			glUniform1i(location, m_lights_size);
			CHECK_GL_ERRORS;

			int i = 0;
			for (auto &light : m_lights)
			{
				location = m_shader.getUniformLocation(("lights[" + to_string(i) + "].colour").c_str());
				glUniform3fv(location, 1, value_ptr(light->colour));
				CHECK_GL_ERRORS;
				location = m_shader.getUniformLocation(("lights[" + to_string(i) + "].position").c_str());
				glUniform4fv(location, 1, value_ptr(light->position));
				CHECK_GL_ERRORS;

				location = m_shader.getUniformLocation(("lights[" + to_string(i) + "].falloff[0]").c_str());
				glUniform1d(location, light->falloff[0]);
				CHECK_GL_ERRORS;
				location = m_shader.getUniformLocation(("lights[" + to_string(i) + "].falloff[1]").c_str());
				glUniform1d(location, light->falloff[1]);
				CHECK_GL_ERRORS;
				location = m_shader.getUniformLocation(("lights[" + to_string(i) + "].falloff[2]").c_str());
				glUniform1d(location, light->falloff[2]);
				CHECK_GL_ERRORS;

				location = m_shader.getUniformLocation(("lights[" + to_string(i) + "].coneAngle").c_str());
				glUniform1f(location, light->coneAngle);
				CHECK_GL_ERRORS;
				location = m_shader.getUniformLocation(("lights[" + to_string(i) + "].coneDirection").c_str());
				glUniform3fv(location, 1, value_ptr(light->coneDirection));
				CHECK_GL_ERRORS;
				++i;
			}
		}

		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity = getAmbientIntensity();
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}

		{
			location = m_shader.getUniformLocation("enableTransparency");
			glUniform1ui(location, enableTransparency);
			CHECK_GL_ERRORS;
		}
		
		{
			location = m_shader.getUniformLocation("enableNormalShading");
			glUniform1ui(location, enableNormalShading);
			CHECK_GL_ERRORS;
		}
		
		{
			location = m_shader.getUniformLocation("enableNormalMapping");
			glUniform1ui(location, enableNormalMapping);
			CHECK_GL_ERRORS;
		}

		{
			location = m_shader.getUniformLocation("enableUVShading");
			glUniform1ui(location, enableUVShading);
			CHECK_GL_ERRORS;
		}

		{
			location = m_shader.getUniformLocation("enableTangentShading");
			glUniform1ui(location, enableTangentShading);
			CHECK_GL_ERRORS;
		}

		{
			location = m_shader.getUniformLocation("enableTessellation");
			glUniform1ui(location, enableTessellation);
			CHECK_GL_ERRORS;
		}
		
		{
			location = m_shader.getUniformLocation("enableSpecularMapping");
			glUniform1ui(location, enableSpecularMapping);
			CHECK_GL_ERRORS;
		}

		{
			location = m_shader.getUniformLocation("enableAmbientOcclusionMapping");
			glUniform1ui(location, enableAmbientOcclusionMapping);
			CHECK_GL_ERRORS;
		}

		{
			location = m_shader.getUniformLocation("enableTextureMapping");
			glUniform1ui(location, enableTextureMapping);
			CHECK_GL_ERRORS;
		}

		{
			location = m_shader.getUniformLocation("enableShadow");
			glUniform1ui(location, enableShadow);
			CHECK_GL_ERRORS;
		}

		{
			location = m_shader.getUniformLocation("environMap");
			glUniform1i(location, 6);
			CHECK_GL_ERRORS;
		}

		{
			location = m_shader.getUniformLocation("environMapNight");
			glUniform1i(location, 7);
			CHECK_GL_ERRORS;
		}

		{
			location = m_shader.getUniformLocation("blendFactor");
			glUniform1f(location, getBlendFactor());
			CHECK_GL_ERRORS;
		}

		{
			// Texture Samplers
			glUniform1i(0, 0);
			glUniform1i(1, 1);
			glUniform1i(2, 2);
			glUniform1i(3, 3);
			glUniform1i(4, 4);
			glUniform1i(5, 5);
		}

		{
			// Shadow Samplers
			for (int i = 0; i < m_lights_size; ++i)
			{
				location = m_shader.getUniformLocation(("shadowMap[" + to_string(i) + "]").c_str());
				glUniform1i(location, 8 + i);
			}
		}
		
	}
	m_shader.disable();


	// Skybox

	m_shader_skybox.enable();

	GLuint location = m_shader_skybox.getUniformLocation("Perspective");
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
	CHECK_GL_ERRORS;

	location = m_shader_skybox.getUniformLocation("blendFactor");
	glUniform1f(location, getBlendFactor());
	CHECK_GL_ERRORS;

	location = m_shader_skybox.getUniformLocation("cubemapTexture");
	glUniform1i(location, 6);
	CHECK_GL_ERRORS;

	location = m_shader_skybox.getUniformLocation("cubemapTextureNight");
	glUniform1i(location, 7);
	CHECK_GL_ERRORS;

	m_shader_skybox.disable();


	{
		// Lens Flare Texture
		m_shader_lensFlare.enable();
		glUniform1i(0, 0);
		m_shader_lensFlare.disable();
	}

}

void A5::handleWASDMovement()
{
	float speed = lShiftPressed ? 0.1: 0.05;
	vec3 horizontal = glm::cross(vec3(0, 1, 0), cameraVector);
	horizontal = normalize(horizontal);

	glm::vec3 cameraVelocityFrontBack = vec3(0);
	glm::vec3 cameraVelocityHorizontal = vec3(0);

	if (( (wasdState >> WASD_W) & 1 ) && !( (wasdState >> WASD_S) & 1 ))
	{
		cameraVelocityFrontBack = speed * cameraVector;	
	} else if (( (wasdState >> WASD_S) & 1 ) && !( (wasdState >> WASD_W) & 1 ))
	{
		cameraVelocityFrontBack = -speed * cameraVector;	
	}
	if (( (wasdState >> WASD_A) & 1 ) && !( (wasdState >> WASD_D) & 1 ))
	{
		cameraVelocityHorizontal = speed * horizontal;	
	} else if (( (wasdState >> WASD_D) & 1 ) && !( (wasdState >> WASD_A) & 1 ))
	{
		cameraVelocityHorizontal = -speed * horizontal;	
	}
	eye += (cameraVelocityFrontBack + cameraVelocityHorizontal);
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A5::appLogic()
{
	// Place per frame, application logic here ...
	sun = m_lights.front();
	sun->position = calculateSunDirection();
	if (time_autorun)
	{
		assert(sun->position.w == 0);
		hour = std::fmod(hour + 0.05, 24.0);
		sun->position = calculateSunDirection();
	}

	handleWASDMovement();

	calculateViewMatrix();

	uploadCommonSceneUniforms();

	ParticleMaster::update();

	genParticles();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A5::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	if (enableFreeMode)
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	} else {
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);

		// Add more gui elements here here ...
		if (ImGui::BeginMainMenuBar()) {
	        if (ImGui::BeginMenu("Application")) {
				for (int i = 0; i < FUNCTION_NAMES.size(); ++i)
				{
					if (ImGui::MenuItem(std::get<0>(FUNCTION_NAMES[i]), std::get<1>(FUNCTION_NAMES[i]))) {
						(this->*std::get<2>(FUNCTION_NAMES[i]))();
					}
    			}
				for (int i = 0; i < FUNCTION_NAMES_CTRL.size(); ++i)
				{
					if (ImGui::MenuItem(std::get<0>(FUNCTION_NAMES_CTRL[i]), std::get<1>(FUNCTION_NAMES_CTRL[i]))) {
						(this->*std::get<2>(FUNCTION_NAMES_CTRL[i]))();
					}
    			}
	            ImGui::EndMenu();
	        }
	       	if (ImGui::BeginMenu("Options")) {
				for (int i = 0; i < TOGGLE_NAMES.size(); ++i)
				{
					if (ImGui::MenuItem(std::get<0>(TOGGLE_NAMES[i]), std::get<1>(TOGGLE_NAMES[i]), std::get<2>(TOGGLE_NAMES[i]), true)) {}
				}
				for (int i = 0; i < TOGGLE_NAMES_CTRL.size(); ++i)
				{
					if (ImGui::MenuItem(std::get<0>(TOGGLE_NAMES_CTRL[i]), std::get<1>(TOGGLE_NAMES_CTRL[i]), std::get<2>(TOGGLE_NAMES_CTRL[i]), true)) {}
				}
	            ImGui::EndMenu();
	        }
	        ImGui::EndMainMenuBar();
    	}

		ImGui::Text("Time Control");
        ImGui::SliderFloat("Time", &hour, 0.0f, 24.0f, "= %.1f");
		ImGui::Checkbox("Time Flow", &time_autorun);

		ImGui::Separator();

		ImGui::Text("Terrain");
        ImGui::SliderFloat("Perlin UV Scaling", &(ground->material.dparams.w), 0.01f, 0.05f, "= %.4f");
        ImGui::SliderFloat("Perlin Z Scaling", &(ground->material.dparams.y), 0.0000f, 100.0f, "= %.4f");

		ImGui::Separator();
		ImGui::Text("Particle System");

        ImGui::SliderFloat("Origin Turbulance", &(particleOriginTurbulance), 0.0f, 1.0f, "= %.1f");
        ImGui::SliderFloat("Velocity Turbulance", &(particleTurbulance), 0.0f, 5.0f, "%.1f");
        ImGui::SliderFloat("Velocity Bias X", &(particleVelocityBias.x), -100.0f, 100.0f, "%.1f");
        ImGui::SliderFloat("Velocity Bias Y", &(particleVelocityBias.y), -100.0f, 100.0f, "%.1f");
        ImGui::SliderFloat("Velocity Bias Z", &(particleVelocityBias.z), -100.0f, 100.0f, "%.1f");
        ImGui::SliderFloat("Size", &(particleSize), 0.0f, 5.0f, "= %.1f");
        ImGui::SliderFloat("Gravity", &(particleGravity), -50.0f, 50.0f, "= %.1f");
        ImGui::SliderFloat("Life Time", &(particleLifetime), 0.0f, 5.0f, "= %.1f");
        ImGui::SliderInt("Number per Frame", &(particleNumberPerFrame), 0, 50, "= %.1f");

		ImGui::ColorEdit3( "##Colour 1", &(particleColor1.r) );
		ImGui::ColorEdit3( "##Colour 2", &(particleColor2.r) );

		if ( ImGui::Button("Set Particles Origin to Camera"))
		{
			particleOrigin = eye;
		}

		ImGui::Separator();

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

		ImGui::Separator();
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix,
		const glm::vec3 & eye,
		texturestore_t &textures,
		GLuint *depthMap,
		const glm::mat4 *depthBiasMVP
) {

	shader.enable();
	{
		//-- Set Eye vector:
		GLint location = shader.getUniformLocation("eye");
		glUniform3fv(location, 1, value_ptr(eye));
		CHECK_GL_ERRORS;

		//-- Set View matrix:
		location = shader.getUniformLocation("View");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(viewMatrix));
		CHECK_GL_ERRORS;

		//-- Set Model matrix:
		location = shader.getUniformLocation("Model");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(node.trans));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(node.trans)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;


		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.ks");
		vec3 ks = node.material.ks;
		glUniform3fv(location, 1, value_ptr(ks));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.dparams");
		vec4 dparams = node.material.dparams;
		glUniform4fv(location, 1, value_ptr(dparams));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.shininess");
		glUniform1f(location, node.material.shininess);
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.alpha");
		glUniform1f(location, node.material.alpha);
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.texScaling");
		glUniform1f(location, node.material.texScaling);
		CHECK_GL_ERRORS;

		for (int i = 0; i < MAX_LIGHTS; ++i)
		{
			location = shader.getUniformLocation(("depthBiasMVP[" + to_string(i) + "]").c_str());
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(depthBiasMVP[i] * node.trans));
			CHECK_GL_ERRORS;
		}

		// Texture
		texflags_t flags = 0;
		if (node.material.hasTexture)
		{
			auto it = textures.find(node.material.tex);
			if (it != textures.end())
			{
				flags = it->second.flags;
				if ((flags & TEX_COLOUR) != 0)
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, it->second.colourTextureID);
					CHECK_GL_ERRORS;
				}
				if ((flags & TEX_NORMAL) != 0)
				{
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, it->second.normalTextureID);
					CHECK_GL_ERRORS;
				}
				if ((flags & TEX_DISPLACE) != 0)
				{
					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, it->second.displaceTextureID);
					CHECK_GL_ERRORS;
				}
				if ((flags & TEX_SPECULAR) != 0)
				{
					glActiveTexture(GL_TEXTURE3);
					glBindTexture(GL_TEXTURE_2D, it->second.specularTextureID);
					CHECK_GL_ERRORS;
				}
				if ((flags & TEX_AMBIENT_OCCLUSION) != 0)
				{
					glActiveTexture(GL_TEXTURE4);
					glBindTexture(GL_TEXTURE_2D, it->second.ambientOcclusionTextureID);
					CHECK_GL_ERRORS;
				}
			}
		}

		if (node.material.dparams.y != 0)
		{
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, node.material.perlin);
			CHECK_GL_ERRORS;
		}

		location = shader.getUniformLocation("texture_flags");
		glUniform1i(location, flags);
		CHECK_GL_ERRORS;

	}

	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A5::draw() {

	glViewport(0 , 0,m_framebufferWidth, m_framebufferHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (enableShadow) {
		renderShadowMap(*m_rootNode);
	}

	renderSceneGraph(*m_rootNode);

	renderSkybox();

	renderParticles();

	renderSun();

	renderLensFlare();
}

//----------------------------------------------------------------------------------------
void A5::renderSceneGraph(const SceneNode & root) {

	if (zbufferEnabled) {
		glEnable( GL_DEPTH_TEST );
	} else {
		glDisable( GL_DEPTH_TEST );
	}

	if (enableWireframeMode)
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	} else {
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}

	if (frontfaceCulling || backfaceCulling)
	{
		glEnable(GL_CULL_FACE);
	} else {
		glDisable(GL_CULL_FACE);		
	}
	if (frontfaceCulling && !backfaceCulling)
	{
		glCullFace(GL_FRONT);
	}
	if (backfaceCulling && !frontfaceCulling)
	{
		glCullFace(GL_BACK);
	}
	if (frontfaceCulling && backfaceCulling)
	{
		glCullFace(GL_FRONT_AND_BACK);
	}
	if (enableShadow)
	{
		for (int i = 0; i < m_lights_size; ++i) {
			glActiveTexture(GL_TEXTURE8 + i);
			glBindTexture(GL_TEXTURE_2D, m_depth_map[i]);
			CHECK_GL_ERRORS;
		}
	}
	glBindVertexArray(m_vao_meshData);
		glEnable(GL_MULTISAMPLE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				m_cubemap.Bind(GL_TEXTURE6);
				m_cubemap_night.Bind(GL_TEXTURE7);
				CHECK_GL_ERRORS;
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				if (enableShadow)
				{
					for (int i = 0; i < m_lights_size; ++i) {
						glActiveTexture(GL_TEXTURE8 + i);
						glBindTexture(GL_TEXTURE_2D, m_depth_map[i]);
						CHECK_GL_ERRORS;
					}
				}
			renderSceneGraphSubtree(root, m_view, true);
			renderSceneGraphSubtree(root, m_view, false);
			glDisable(GL_BLEND);
		glDisable(GL_MULTISAMPLE);
	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

void A5::renderShadowMap(const SceneNode & root) {
	glBindVertexArray(m_vao_meshData);
		glViewport(0 ,0, maxTextureSize, maxTextureSize);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glEnable(GL_POLYGON_OFFSET_FILL); 
					glPolygonOffset(0.0, 50.0);
						glEnable( GL_DEPTH_TEST );
							m_shader_depth.enable();
								int lightIdx = 0;
								for (auto &light: m_lights)
								{
									glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[lightIdx]);
										glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
										calculateDepthMVP(*light, lightIdx);
										renderShadowMapSubtree(root, depthMVP);
									glBindFramebuffer(GL_FRAMEBUFFER, 0);
									++lightIdx;
								}
							m_shader_depth.disable();
						glDisable( GL_DEPTH_TEST );
					glDisable(GL_POLYGON_OFFSET_FILL);	
				glDisable(GL_BLEND);
			glDisable(GL_CULL_FACE);
		glViewport(0 , 0,m_framebufferWidth, m_framebufferHeight);
	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

void A5::initNodes(const SceneNode & root) {
	for (SceneNode * node : root.children) {
		if (node->m_nodeType == NodeType::GeometryNode) {
			GeometryNode * geometryNode = static_cast<GeometryNode *>(node);

			if (node->m_name == "ground")
			{
				cerr << "Ground found" << endl;
				this->ground = geometryNode;
			}

			if (geometryNode->material.dparams.y != 0)
			{
				geometryNode->material.perlin = createAndUploadPerlinTexture(geometryNode->material.dparams.z);
			}

			if (geometryNode->material.hasTexture) {
				string tex = geometryNode->material.tex;
				if (textures.find(tex) == textures.end())
				{
					texflags_t flags = 0;
					unsigned int width, height;
					GLuint colourTextureID;
					GLuint normalTextureID;
					GLuint displaceTextureID;
					GLuint specularTextureID;
					GLuint ambientOcclusionTextureID;
					{
						string colour_fp = string("Assets/Textures/") + tex + string("_COLOR.png");
						if (file_exists(colour_fp))
						{
							flags |= TEX_COLOUR;
							colourTextureID = createAndUploadTexturePNG(colour_fp.c_str(), width, height);					
						}
					}
					{
						string normal_fp = string("Assets/Textures/") + tex + string("_NRM.png");
						if (file_exists(normal_fp))
						{
							flags |= TEX_NORMAL;
							normalTextureID = createAndUploadTexturePNG(normal_fp.c_str(), width, height);					
						}						
					}
					{
						string displace_fp = string("Assets/Textures/") + tex + string("_DISP.png");
						if (file_exists(displace_fp))
						{
							flags |= TEX_DISPLACE;
							displaceTextureID = createAndUploadTexturePNG(displace_fp.c_str(), width, height);					
						}
					}
					{
						string specular_fp = string("Assets/Textures/") + tex + string("_SPEC.png");
						if (file_exists(specular_fp))
						{
							specularTextureID = createAndUploadTexturePNG(specular_fp.c_str(), width, height);
							flags |= TEX_SPECULAR;
						}
					}
					{
						string occlusion_fp = string("Assets/Textures/") + tex + string("_OCC.png");
						if (file_exists(occlusion_fp))
						{
							ambientOcclusionTextureID = createAndUploadTexturePNG(occlusion_fp.c_str(), width, height);
							flags |= TEX_AMBIENT_OCCLUSION;
						}
					}
					if (flags > 0) {
						textures[tex] = Texture {
							width,
							height,
							flags,
							colourTextureID,
							normalTextureID,
							displaceTextureID,
							specularTextureID,
							ambientOcclusionTextureID
						};
					}
				}
			}
		}
		initNodes(*node);
	}
}

void A5::renderSceneGraphSubtree(const SceneNode & root, const mat4 matrix_acc, bool opaque) {
	for (SceneNode * node : root.children) {

		const mat4 viewMatrix = matrix_acc * root.trans;

		if (node->m_nodeType == NodeType::GeometryNode) {
			const GeometryNode * geometryNode = static_cast<const GeometryNode *>(node);

			updateShaderUniforms(m_shader, *geometryNode, viewMatrix, eye, textures, m_depth_map, depthBiasMVP);		


			// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
			BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

			if ((opaque && geometryNode->material.alpha == 1.0) || (!opaque && geometryNode->material.alpha < 1.0))
			{
				//-- Now render the mesh:
				m_shader.enable();
				glDrawArrays(GL_PATCHES, batchInfo.startIndex, batchInfo.numIndices);
				m_shader.disable();
			}


		}

		renderSceneGraphSubtree(*node, viewMatrix, opaque);

	}
}

void A5::renderShadowMapSubtree(const SceneNode & root, const mat4 matrix_acc) {
	for (SceneNode * node : root.children) {

		const mat4 product = matrix_acc * root.trans;

		if (node->m_nodeType == NodeType::GeometryNode) {
			const GeometryNode * geometryNode = static_cast<const GeometryNode *>(node);
			

			GLuint location = m_shader_depth.getUniformLocation("depthMVP");
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(product * node->trans));
			CHECK_GL_ERRORS;

			// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
			BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];
			
			//-- Now render the mesh:
			
			glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		}

		renderShadowMapSubtree(*node, product);

	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A5::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A5::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

bool A5::handleWASDKeys(int key, int action)
{
	
	bool eventHandled = false;

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_W)
		{
			wasdState |= (1 << WASD_W);;
			eventHandled = true;
		}
		if (key == GLFW_KEY_S)
		{
			wasdState |= (1 << WASD_S);
			eventHandled = true;
		}
		if (key == GLFW_KEY_A)
		{
			wasdState |= (1 << WASD_A);
			eventHandled = true;
		} 
		if (key == GLFW_KEY_D)
		{
			wasdState |= (1 << WASD_D);
			eventHandled = true;
		}
	}

	if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_W)
		{
			wasdState &= ~(1 << WASD_W);;
			eventHandled = true;
		}
		if (key == GLFW_KEY_S)
		{
			wasdState &= ~(1 << WASD_S);
			eventHandled = true;
		}
		if (key == GLFW_KEY_A)
		{
			wasdState &= ~(1 << WASD_A);
			eventHandled = true;
		} 
		if (key == GLFW_KEY_D)
		{
			wasdState &= ~(1 << WASD_D);
			eventHandled = true;
		}
	}
	
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A5::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);


	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if (key == GLFW_KEY_LEFT_CONTROL)
		{
			lCtrlPressed = true;
		}

		if (key == GLFW_KEY_LEFT_SHIFT)
		{
			lShiftPressed = true;
		}
	}

	if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_LEFT_CONTROL)
		{
			lCtrlPressed = false;
		} 
		if (key == GLFW_KEY_LEFT_SHIFT)
		{
			lShiftPressed = false;
		} 
	}

	if (enableFreeMode && !lCtrlPressed)
	{
		eventHandled = handleWASDKeys(key, action);
	}

	if (action == GLFW_PRESS)
	{
		if (lCtrlPressed)
		{
			if (TOGGLE_KEYMAP_CTRL.find(key) != TOGGLE_KEYMAP_CTRL.end())
			{
				*TOGGLE_KEYMAP_CTRL[key] = !*TOGGLE_KEYMAP_CTRL[key];
				eventHandled = true;
			}
			if (FUNCTION_KEYMAP_CTRL.find(key) != FUNCTION_KEYMAP_CTRL.end())
			{
				(this->*(FUNCTION_KEYMAP_CTRL[key]))();
				eventHandled = true;
			}
		}
		else
		{
			if (TOGGLE_KEYMAP.find(key) != TOGGLE_KEYMAP.end())
			{
				*TOGGLE_KEYMAP[key] = !*TOGGLE_KEYMAP[key];
				eventHandled = true;
			}
			if (FUNCTION_KEYMAP.find(key) != FUNCTION_KEYMAP.end())
			{
				(this->*(FUNCTION_KEYMAP[key]))();
				eventHandled = true;
			}
		}
	}
	// Fill in with event handling code...

	return eventHandled;
}

void A5::reset() {};

void A5::close() {
	glfwSetWindowShouldClose(m_window, GL_TRUE);
}

// Not actually used...
GLuint A5::createAndUploadTextureBMP(const char *imagepath, unsigned int &width, unsigned int &height) {
	unsigned char *bitmap;
	loadBMP(imagepath, bitmap, width, height);
	cerr << "BMP loaded: " << imagepath << " w: " << width << " h: " << height << endl;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	cerr << "Texture created: " << imagepath << endl;

	CHECK_GL_ERRORS;	
	delete [] bitmap;
	return textureID;
}

GLuint A5::createAndUploadTexturePNG(const char *imagepath, unsigned int &width, unsigned int &height) {
	unsigned char *bitmap;
	lodepng_decode_file(&bitmap, &width, &height, imagepath, LodePNGColorType::LCT_RGBA, 8);
	cerr << "PNG loaded: " << imagepath << " w: " << width << " h: " << height << endl;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	cerr << "Texture created: " << imagepath << endl;

	CHECK_GL_ERRORS;	
	delete [] bitmap;
	return textureID;
}

GLuint A5::createAndUploadPerlinTexture(float frequency) {
	
	const unsigned int width = PERLIN_TEXTURE_SIZE, height = PERLIN_TEXTURE_SIZE;

	unsigned char *bitmap = new unsigned char[width * height];

	Perlin perlin;

	Image im {PERLIN_TEXTURE_SIZE, PERLIN_TEXTURE_SIZE};

	float val;

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			val = 0;
			if (!(i == 0 || j == 0 || i == height - 1 || j == width - 1))
			{
				val = perlin.octaveNoise(i / frequency, j / frequency, 4);
			}
			unsigned char char_val = (unsigned char)glm::floor(122.5 + 122.5 * val);
			bitmap[height * i + j] = char_val;
			im(j,  height - i - 1, 0) = bitmap[height * i + j] / float(255.0);
			im(j,  height - i - 1, 1) = bitmap[height * i + j] / float(255.0);
			im(j,  height - i - 1, 2) = bitmap[height * i + j] / float(255.0);
		}
	}

	im.savePng( "perlin.png" );

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	CHECK_GL_ERRORS;	
	delete [] bitmap;
	return textureID;
}

void A5::initShadowFrameBuffer()
{

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

	cerr << "Max texture size: " << maxTextureSize << endl;

	maxTextureSize = glm::min(maxTextureSize, 8192);

	cerr << "Shadow map texture size used: " << maxTextureSize << endl;

	int i = 0;

	for (auto &light: m_lights)
	{

		glGenFramebuffers(1, m_fbo + i);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[i]);

		glGenTextures(1, m_depth_map + i);
		glBindTexture(GL_TEXTURE_2D, m_depth_map[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, maxTextureSize, maxTextureSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		float colour[4] = {1.0f, 1.0f, 1.0f, 1.0f};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, colour);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_map[i], 0);

		glDrawBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("Framebuffer error");
		}

		++i;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void A5::calculateDepthMVP(const Light &light, int lightIdx)
{
	mat4 depthProjectionMatrix;
	glm::mat4 depthViewMatrix;
	if (light.position.w == 0)
	{
		// This should be updated dynamically, use constant frustrum bounding box for now.
		depthProjectionMatrix = glm::ortho<float>(-30,30,-30,30,-30,60);
		depthViewMatrix = glm::lookAt(-glm::vec3(light.position), glm::vec3(0,0,0), glm::vec3(0,1,0));
	} 
	else
	{
		depthProjectionMatrix = glm::perspective<float>(glm::radians(light.coneAngle), 1.0, 1.0, 10000.0);
		depthViewMatrix = glm::lookAt(glm::vec3(light.position), glm::vec3(light.position) + light.coneDirection, glm::vec3(0,1,0));
	}
	depthMVP = depthProjectionMatrix * depthViewMatrix;
	depthBiasMVP[lightIdx] = BIAS_MATRIX * depthMVP;
}

void A5::renderSkybox()
{
	m_shader_skybox.enable();
		glDisable(GL_CULL_FACE);
			glBindVertexArray( m_vao_skyboxData );
				m_cubemap.Bind(GL_TEXTURE6);
				m_cubemap_night.Bind(GL_TEXTURE7);
				GLuint location = m_shader_skybox.getUniformLocation("View");
				glm::mat4 view = m_view;
				glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(view));
				glDrawArrays(GL_TRIANGLES, 0, 108);
		glBindVertexArray(0);
	m_shader_skybox.disable();
	CHECK_GL_ERRORS;
}

bool A5::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	if (enableFreeMode) 
	{
		vec2 direction = vec2(
			xPos - m_framebufferWidth / 2,
			-(yPos - m_framebufferHeight / 2)
		);

		cameraVector = glm::mat3(glm::rotate<float>(glm::mat4(), -glm::radians(direction.x) / 500 * MOUSE_SENSITIVITY, vec3(0, 1, 0))) * cameraVector;

		vec3 upVector = vec3(0, 1, 0) - glm::proj(vec3(0, 1, 0), cameraVector);

		if (std::fabs(upVector.y) < 0.01 && ((cameraVector.y < 0 && direction.y < 0) || (cameraVector.y > 0 && direction.y > 0)))
		{
			calculateViewMatrix();
			glfwSetCursorPos(m_window, m_framebufferWidth / 2, m_framebufferHeight / 2);
			eventHandled = true;		
			return eventHandled;
		}

		vec3 axis = glm::cross(cameraVector, upVector);
		cameraVector = glm::mat3(glm::rotate<float>(glm::mat4(), glm::radians(direction.y) / 500 * MOUSE_SENSITIVITY, axis)) * cameraVector;
		cameraVector = normalize(cameraVector);
		calculateViewMatrix();

		glfwSetCursorPos(m_window, m_framebufferWidth / 2, m_framebufferHeight / 2);

		eventHandled = true;
		return eventHandled;		
	}
}

vec4 A5::calculateSunDirection() {
	return glm::rotate<float>(glm::mat4(), (hour - 10.0) / 12.0 * PI, glm::vec3(1.0, 0.5, 0.2)) * vec4(0.8, -1.0, -0.2, 0.0);
}

vec3 A5::getAmbientIntensity()
{
	return vec3(0.15);
}


float A5::getBlendFactor()
{
	if (hour >= 8.0f && hour < 18.0f)
	{
		return 0.0f;
	}
	if (hour >= 18.0f && hour < 20.0f)
	{
		return (hour - 18.0f) / 2.0f;
	}
	if (hour >= 20.0f || hour < 6.0f)
	{
		return 1.0f;
	}
	if (hour >= 6.0f && hour < 8.0f)
	{
		return 1.0f - (hour - 6.0f) / 2.0f;
	}
}

static glm::mat4 calculateModelViewMatrixParticle(glm::vec3 position, float rotation, float scale, glm::mat4 viewMatrix)
{
	glm::mat4 modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix[0][0] = viewMatrix[0][0];
	modelMatrix[0][1] = viewMatrix[1][0];
	modelMatrix[0][2] = viewMatrix[2][0];
	modelMatrix[1][0] = viewMatrix[0][1];
	modelMatrix[1][1] = viewMatrix[1][1];
	modelMatrix[1][2] = viewMatrix[2][1];
	modelMatrix[2][0] = viewMatrix[0][2];
	modelMatrix[2][1] = viewMatrix[1][2];
	modelMatrix[2][2] = viewMatrix[2][2];
	modelMatrix = glm::rotate<float>(modelMatrix, rotation, vec3(0, 0, 1));
	modelMatrix = glm::scale<float>(modelMatrix, vec3(scale));
	glm::mat4 viewModelMatrix = viewMatrix * modelMatrix;
	return viewModelMatrix;
}

void A5::renderParticles()
{
	GLuint location;

	m_shader_particle.enable();

		location = m_shader_particle.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

		glBindVertexArray(m_vao_quadData);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(false);
				for (auto particle: ParticleMaster::particles)
				{
					location = m_shader_particle.getUniformLocation("ModelView");
					glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(calculateModelViewMatrixParticle(particle->position, particle->rotation, particle->scale, m_view)));
					
					location = m_shader_particle.getUniformLocation("lifeLength");
					glUniform1f(location, particle->lifeLength);

					location = m_shader_particle.getUniformLocation("elapsedTime");
					glUniform1f(location, particle->elapsedTime);
					
					location = m_shader_particle.getUniformLocation("particleColor1");
					glUniform3fv(location, 1, value_ptr(particleColor1));

					location = m_shader_particle.getUniformLocation("particleColor2");
					glUniform3fv(location, 1, value_ptr(particleColor2));

					CHECK_GL_ERRORS;
					glDrawArrays(GL_TRIANGLES, 0, 12);
				}
			glDepthMask(true);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glBindVertexArray(0);
	m_shader_particle.disable();

}

static glm::mat4 calculateModelViewMatrixSun(glm::vec3 position, float rotation, float scale, glm::mat4 viewMatrix)
{
	glm::mat4 modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix[0][0] = viewMatrix[0][0];
	modelMatrix[0][1] = viewMatrix[1][0];
	modelMatrix[0][2] = viewMatrix[2][0];
	modelMatrix[1][0] = viewMatrix[0][1];
	modelMatrix[1][1] = viewMatrix[1][1];
	modelMatrix[1][2] = viewMatrix[2][1];
	modelMatrix[2][0] = viewMatrix[0][2];
	modelMatrix[2][1] = viewMatrix[1][2];
	modelMatrix[2][2] = viewMatrix[2][2];
	modelMatrix = glm::rotate<float>(modelMatrix, rotation, vec3(0, 0, 1));
	modelMatrix = glm::scale<float>(modelMatrix, vec3(scale));
	glm::mat4 viewModelMatrix = mat4(viewMatrix[0], viewMatrix[1], viewMatrix[2], vec4(0, 0, 0, 1)) * modelMatrix;
	return viewModelMatrix;
}

void A5::renderSun()
{
	GLuint location;

	m_shader_sun.enable();

		location = m_shader_sun.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

		glBindVertexArray(m_vao_quadData);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(false);
	
				location = m_shader_sun.getUniformLocation("ModelView");
				glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(calculateModelViewMatrixSun(-500 * vec3(sun->position), 0.0, 2500.0, m_view)));

				location = m_shader_sun.getUniformLocation("sunPosition");
				glUniform3fv(location, 1, value_ptr(vec3(sun->position)));

				location = m_shader_sun.getUniformLocation("cameraVector");
				glUniform3fv(location, 1, value_ptr(cameraVector));

				CHECK_GL_ERRORS;
				glDrawArrays(GL_TRIANGLES, 0, 12);
				
			glDepthMask(true);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glBindVertexArray(0);
	m_shader_sun.disable();
}

void A5::genParticles()
{
	vec3 particleDirection;
	vec3 particleOriginChanged;
	for (int i = 0; i < particleNumberPerFrame; ++i)
	{
		particleDirection.x = (float)(rand() % 100) / 50 - 1;
		particleDirection.y = (float)(rand() % 100) / 50 - 1;
		particleDirection.z = (float)(rand() % 100) / 50 - 1;
		particleOriginChanged = particleOrigin + particleOriginTurbulance*particleDirection;
		particleDirection = particleTurbulance * particleDirection + particleVelocityBias;
		new Particle(particleOriginChanged, particleDirection, particleGravity, particleLifetime, 0, particleSize);
	}
}

static vec2 convertToScreenSpace(const vec3 &worldPos, const mat4 &viewMat, const mat4 &projectionMat)
{
	vec4 coords = vec4(worldPos, 1.0f);
	coords = viewMat * coords;
	coords = projectionMat * coords;
	if (coords.w <= 0)
	{
		return vec2(NAN, NAN);
	}
	float x = 1 - (coords.x / coords.w + 1) / 2.0;
	float y = (coords.y / coords.w + 1) / 2.0;
	return vec2(x, y);
}

void A5::renderLensFlare()
{
	GLuint location;
	vec2 sunCoords;
	vec2 sunToCenter;
	float brightness;
	vec2 flarePosition;
	vec2 direction;
	float xScale, yScale;
	vec2 centerPos;
	int lensFlareTextureUsed[9] = {
		5, 7, 3, 1, 2, 4, 6, 9, 4
	};

	float lensFlareScales[9] = {
		0.23, 0.4, 0.1, 0.1, 0.07, 0.2, 0.17, 0.2, 0.5
	};

	m_shader_lensFlare.enable();

		glBindVertexArray(m_vao_quadData);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glDepthMask(false);
				for (int i = 0; i < 9; ++i)
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, flare_textures[lensFlareTextureUsed[i]]);
					sunCoords = convertToScreenSpace(-500 * vec3(sun->position), m_view, m_perpsective);
					if (sunCoords.x != NAN && sunCoords.y != NAN)
					{
						sunToCenter = vec2(0.5, 0.5) - sunCoords;
						brightness = 1 - glm::length(sunToCenter / 0.6f);
						if (brightness > 0)
						{
							direction = i * 0.4 * sunToCenter;
							flarePosition = sunCoords + direction;
							xScale = lensFlareScales[i];
							yScale = xScale * (float) (m_framebufferWidth) / (float) (m_framebufferHeight);
							centerPos = flarePosition;

							location = m_shader_lensFlare.getUniformLocation("transform");
							glUniform4fv(location, 1, value_ptr(vec4(centerPos, xScale, yScale)));

							location = m_shader_lensFlare.getUniformLocation("brightness");
							glUniform1f(location, brightness);

							glDrawArrays(GL_TRIANGLES, 0, 12);
						}
					}
					glBindTexture(GL_TEXTURE_2D, 0);
					CHECK_GL_ERRORS;
				}
			glDepthMask(true);
		glDisable(GL_BLEND);
		glBindVertexArray(0);
	m_shader_lensFlare.disable();
}
