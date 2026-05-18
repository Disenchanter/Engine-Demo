#pragma once
#include <vector>
#include "../core.h"
#include "../mesh.h"
#include "../../application/camera/camera.h"
#include "../light/directionalLight.h"
#include "../light/pointLight.h"
#include "../light/spotLight.h"
#include "../light/ambientLight.h"
#include "../shader.h"
#include "../scene.h"

class Renderer {
public:
	Renderer();
	~Renderer();

	// Render frame root function / •’•Ï©`•‡√Ëª≠§Œ•®•Û•»•Í•›•§•Û•»
	// Render the scene for a single frame
	void render(
		Scene* scene,
		Camera* camera,
		DirectionalLight* dirLight,
		AmbientLight* ambLight
	);

	void renderObject(
		Object* object,
		Camera* camera,
		DirectionalLight* dirLight,
		AmbientLight* ambLight
	);


	void setClearColor(glm::vec3 color);

private:
	// Choose shader based on material type / •ﬁ•∆•Í•¢•Î§ÀèÍ§∏§∆•∑•ß©`•¿§Úﬂxík
	Shader* pickShader(MaterialType type);

private:
	// Shader instances for different material flows / •ﬁ•∆•Í•¢•ÎÑe•∑•ß©`•¿
	Shader* mPhongShader{ nullptr };
	Shader* mWhiteShader{ nullptr };
	Shader* mPbrShader{ nullptr };
};