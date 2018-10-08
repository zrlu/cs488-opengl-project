#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "ShaderProgram.hpp"
#include "MeshConsolidator.hpp"

#include "SceneNode.hpp"
#include "Light.hpp"

#include <glm/glm.hpp>
#include <memory>

#include <map>
#include <vector>
#include <algorithm>
#include <tuple>
#include <set>
#include <list>
#include <unordered_map>
#include <tuple>
#include <glob.h>
#include "Texture.hpp"
#include "GeometryNode.hpp"
#include "CubeMapTexture.hpp"

#define MAX_LIGHTS 4
#define PERLIN_TEXTURE_SIZE 512
#define MOUSE_SENSITIVITY 4.0

#define WASD_W 1
#define WASD_S 2
#define WASD_A 3
#define WASD_D 4

typedef std::unordered_map<std::string, Texture> texturestore_t;

const glm::mat4 BIAS_MATRIX (
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
);

class A5;

typedef void (A5::*a3voidmethod_t)(void);
typedef std::tuple<const char *, const char *, bool *> toggle_mapping_t;
typedef std::tuple<const char *, const char *, a3voidmethod_t> func_mapping_t;

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

class A5 : public CS488Window {
public:
	A5(const std::string & luaSceneFile);
	virtual ~A5();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void calculateViewMatrix();
	void initLightSources();
	void initLightSources(const std::list<Light *> & lights);

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderShadowMap(const SceneNode &node);
	void renderShadowMapSubtree(const SceneNode & root, const glm::mat4 matrix_acc);
	void renderSceneGraph(const SceneNode &node);
	void renderSceneGraphSubtree(const SceneNode & root, const glm::mat4 matrix_acc, bool opaque);

	glm::mat4 m_perpsective;
	glm::mat4 m_view;

	std::list<Light *> m_lights;
	unsigned int m_lights_size;


	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLuint m_vbo_vertexUVs;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	GLint m_uvAttribLocation;
	ShaderProgram m_shader;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;


	// My states
	std::vector<toggle_mapping_t> TOGGLE_NAMES;
	std::vector<toggle_mapping_t> TOGGLE_NAMES_CTRL;
	std::map<int, bool *> TOGGLE_KEYMAP;
	std::map<int, bool *> TOGGLE_KEYMAP_CTRL;
	std::vector<func_mapping_t> FUNCTION_NAMES;
	std::map<int, a3voidmethod_t> FUNCTION_KEYMAP;
	std::vector<func_mapping_t> FUNCTION_NAMES_CTRL;
	std::map<int, a3voidmethod_t> FUNCTION_KEYMAP_CTRL;
	glm::mat4 m_rotation;
	glm::mat4 m_translation;
	bool zbufferEnabled;
	bool backfaceCulling;
	bool frontfaceCulling;
	bool enableNormalShading;
	bool enableNormalMapping;
	bool enableTransparency;
	bool enableUVShading;
	bool enableTangentShading;
	bool lCtrlPressed;
	bool lShiftPressed;
	bool enableTessellation;
	bool enableSpecularMapping;
	bool enableAmbientOcclusionMapping;
	bool enableShadow;
	bool enableWireframeMode;
	bool enableTextureMapping;
	bool enableFreeMode;
	glm::vec3 eye;

	ShaderProgram m_shader_depth;
	ShaderProgram m_shader_skybox;
	ShaderProgram m_shader_particle;
	ShaderProgram m_shader_sun;
	ShaderProgram m_shader_lensFlare;
	GLuint m_fbo[MAX_LIGHTS];
	GLuint m_depth_map[MAX_LIGHTS];
	GLint maxTextureSize;

	glm::mat4 depthMVP;
	glm::mat4 depthBiasMVP[MAX_LIGHTS];

	texturestore_t textures;

	float hour;
	bool time_autorun;
	GeometryNode *ground;

	GLuint m_vao_skyboxData;
	GLuint m_vbo_skyboxVertexPositions;

	GLuint m_vao_quadData;
	GLuint m_vbo_quadPositions;
	GLuint m_quadPositionAttribLocation;
	
	// My methods
	void reset();
	void close();
	void initNodes(const SceneNode & root);
	GLuint createAndUploadTextureBMP(const char *imagepath, unsigned int &width, unsigned int &height);
	GLuint createAndUploadTexturePNG(const char *imagepath, unsigned int &width, unsigned int &height);
	GLuint createAndUploadPerlinTexture(float frequency);
	void initShadowFrameBuffer();
	void calculateDepthMVP(Light const &light, int lightIdx);
	glm::vec4 calculateSunDirection();
	glm::vec3 getAmbientIntensity();
	float getBlendFactor();
	void genParticles();
	void renderParticles();
	void renderSun();
	void renderLensFlare();
	void renderSkybox();
	bool handleWASDKeys(int key, int action);
	void handleWASDMovement();
	GLuint flare_textures[9];

	float viewAnglePhi, viewAngleTheta;

	CubemapTexture m_cubemap;
	CubemapTexture m_cubemap_night;

	glm::vec3 cameraVector;
	int wasdState;
	glm::vec3 particleOrigin;
	Light *sun;
	float particleOriginTurbulance;
	float particleSize;
	glm::vec3 particleVelocityBias;
	float particleGravity;
	float particleLifetime;
	float particleTurbulance;
	int particleNumberPerFrame;
	glm::vec3 particleColor1;
	glm::vec3 particleColor2;
};
