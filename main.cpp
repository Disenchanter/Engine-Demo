#include <iostream>

#include "glframework/core.h"
#include "glframework/shader.h"
#include <string>
#include <assert.h>//断言
#include "wrapper/checkError.h"
#include "application/Application.h"
#include "glframework/texture.h"

//引入相机+控制器
#include "application/camera/perspectiveCamera.h"
#include "application/camera/orthographicCamera.h"
#include "application/camera/trackBallCameraControl.h"
#include "application/camera/GameCameraControl.h"

#include "glframework/geometry.h"
#include "glframework/material/phongMaterial.h"
#include "glframework/material/whiteMaterial.h"
#include "glframework/material/pbrMaterial.h"

#include "glframework/mesh.h"
#include "glframework/renderer/renderer.h"
#include "glframework/light/pointLight.h"
#include "glframework/light/spotLight.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glframework/scene.h"
#include "glframework/ecs/registry.h"
#include "glframework/ecs/components.h"
#include "glframework/ecs/systems.h"


Renderer* renderer = nullptr;
Scene* scene = nullptr;
Object* sceneRoot = nullptr;

//灯光们
DirectionalLight* dirLight = nullptr;

AmbientLight* ambLight = nullptr;

Camera* camera = nullptr;
CameraControl* cameraControl = nullptr;

glm::vec3 clearColor{};

//for dynamic mesh creation
int shapeType = 0; // 0: Box, 1: Sphere
float posX = 0.0f, posY = 0.0f, posZ = 0.0f;
float shapeSize = 1.0f;
Material* boxMaterial = nullptr;
Material* sphereMaterial = nullptr;

ecs::Registry registry;

void OnResize(int width, int height) {
	GL_CALL(glViewport(0, 0, width, height));
}

void OnKey(int key, int action, int mods) {
	cameraControl->onKey(key, action, mods);
}

//鼠标按下/抬起
void OnMouse(int button, int action, int mods) {
	double x, y;
	app->getCursorPosition(&x, &y);
	cameraControl->onMouse(button, action, x, y);
}

//鼠标移动
void OnCursor(double xpos, double ypos) {
	cameraControl->onCursor(xpos, ypos);
}

//鼠标滚轮
void OnScroll(double offset) {
	cameraControl->onScroll(offset);
}

void prepare() {
	renderer = new Renderer();
	scene = new Scene();
	sceneRoot = new Object();
	scene->addChild(sceneRoot);

	//1 创建geometry
	auto boxGeometry = Geometry::createBox(1.0f);
	auto spGeometry = Geometry::createSphere(1.0f);

	//2 创建不同的PBR材质参数
	boxMaterial = new PBRMaterial();
	((PBRMaterial*)boxMaterial)->mAlbedo = new Texture("assets/textures/earth.png", 0);
	((PBRMaterial*)boxMaterial)->mMetallic = new Texture("assets/textures/sp_mask.png", 1);
	((PBRMaterial*)boxMaterial)->mRoughness = new Texture("assets/textures/sp_mask.png", 2);
	((PBRMaterial*)boxMaterial)->mAO = new Texture("assets/textures/sp_mask.png", 3);
	((PBRMaterial*)boxMaterial)->mMetallicFactor = 0.15f;
	((PBRMaterial*)boxMaterial)->mRoughnessFactor = 1.2f;
	((PBRMaterial*)boxMaterial)->mAOFactor = 1.0f;

	sphereMaterial = new PBRMaterial();
	((PBRMaterial*)sphereMaterial)->mAlbedo = new Texture("assets/textures/earth.png", 0);
	((PBRMaterial*)sphereMaterial)->mMetallic = new Texture("assets/textures/sp_mask.png", 1);
	((PBRMaterial*)sphereMaterial)->mRoughness = new Texture("assets/textures/sp_mask.png", 2);
	((PBRMaterial*)sphereMaterial)->mAO = new Texture("assets/textures/sp_mask.png", 3);
	((PBRMaterial*)sphereMaterial)->mMetallicFactor = 0.95f;
	((PBRMaterial*)sphereMaterial)->mRoughnessFactor = 0.45f;
	((PBRMaterial*)sphereMaterial)->mAOFactor = 1.0f;

	//3 创建mesh
	auto mesh = new Mesh(boxGeometry, boxMaterial);
	auto spMesh01 = new Mesh(spGeometry, sphereMaterial);
	auto spMesh02 = new Mesh(spGeometry, sphereMaterial);
	spMesh01->setPosition(glm::vec3(2.0f, 0.0f, 0.0f));
	spMesh02->setPosition(glm::vec3(-2.0f, 0.0f, 0.0f));

	auto meshEntity = registry.create();
	registry.add<ecs::MeshComponent>(meshEntity, mesh);
	registry.add<ecs::TransformComponent>(meshEntity);

	auto spEntity01 = registry.create();
	auto& spTransform01 = registry.add<ecs::TransformComponent>(spEntity01);
	spTransform01.position = glm::vec3(2.0f, 0.0f, 0.0f);
	registry.add<ecs::MeshComponent>(spEntity01, spMesh01);

	auto spEntity02 = registry.create();
	auto& spTransform02 = registry.add<ecs::TransformComponent>(spEntity02);
	spTransform02.position = glm::vec3(-2.0f, 0.0f, 0.0f);
	registry.add<ecs::MeshComponent>(spEntity02, spMesh02);

	// 使用ECS同步位置时，保持同一个空节点下的并列关系
	sceneRoot->addChild(mesh);
	sceneRoot->addChild(spMesh01);
	sceneRoot->addChild(spMesh02);
	
	dirLight = new DirectionalLight();
	dirLight->mDirection = glm::normalize(glm::vec3(0.6f, -1.0f, 0.25f));
	dirLight->mColor = glm::vec3(2.8f, 2.6f, 2.4f);

	ambLight = new AmbientLight();
	ambLight->mColor = glm::vec3(0.03f);
}

void addMeshToScene() {
	Geometry* geometry = nullptr;
	Material* material = nullptr;

	//create geometry based on shape type
	if (shapeType == 0) {
		geometry = Geometry::createBox(shapeSize);
		material = boxMaterial;
	}
	else {
		geometry = Geometry::createSphere(shapeSize);
		material = sphereMaterial;
	}

	//create mesh with selected material
	auto newMesh = new Mesh(geometry, material);
	newMesh->setPosition(glm::vec3(posX, posY, posZ));

	auto entity = registry.create();
	auto& transform = registry.add<ecs::TransformComponent>(entity);
	transform.position = glm::vec3(posX, posY, posZ);
	registry.add<ecs::MeshComponent>(entity, newMesh);
	
	//add to scene root
	sceneRoot->addChild(newMesh);
}

void prepareCamera() {
	float size = 10.0f;
	//camera = new OrthographicCamera(-size, size, size, -size, size, -size);
	camera = new PerspectiveCamera(
		60.0f, 
		(float)app->getWidth() / (float)app->getHeight(),
		0.1f,
		1000.0f
	);

	cameraControl = new GameCameraControl();
	cameraControl->setCamera(camera);
	cameraControl->setSensitivity(0.4f);
}



void initIMGUI() {
	ImGui::CreateContext();//创建imgui上下文
	ImGui::StyleColorsDark(); // 选择一个主题

	// 设置ImGui与GLFW和OpenGL的绑定
	ImGui_ImplGlfw_InitForOpenGL(app->getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void renderIMGUI() {
	//1 开启当前的IMGUI 渲染
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//2 决定当前的GUI上面有哪些控件，从上到下
	ImGui::Begin("Scene Controls");
	ImGui::Text("ChangeColor Demo");
	ImGui::Button("Test Button", ImVec2(40, 20));
	ImGui::ColorEdit3("Clear Color", (float*)&clearColor);
	
	ImGui::Separator();
	ImGui::Text("Add Object to Scene");
	
	//shape type selection
	const char* shapeOptions[] = { "Box", "Sphere" };
	ImGui::Combo("Shape Type##shape", &shapeType, shapeOptions, IM_ARRAYSIZE(shapeOptions));
	
	//position input
	ImGui::DragFloat("Position X##x", &posX, 0.1f);
	ImGui::DragFloat("Position Y##y", &posY, 0.1f);
	ImGui::DragFloat("Position Z##z", &posZ, 0.1f);
	
	//size input
	ImGui::SliderFloat("Size", &shapeSize, 0.1f, 5.0f);
	
	//create button
	if (ImGui::Button("Create Object", ImVec2(100, 30))) {
		addMeshToScene();
	}
	
	ImGui::End();

	//3 执行UI渲染
	ImGui::Render();
	//获取当前窗体的宽高
	int display_w, display_h;
	glfwGetFramebufferSize(app->getWindow(), &display_w, &display_h);
	//重置视口大小
	glViewport(0, 0, display_w, display_h);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main() {
	if (!app->init(1600, 1200)) {
		return -1;
	}

	app->setResizeCallback(OnResize);
	app->setKeyBoardCallback(OnKey);
	app->setMouseCallback(OnMouse);
	app->setCursorCallback(OnCursor);
	app->setScrollCallback(OnScroll);

	//设置opengl视口以及清理颜色
	GL_CALL(glViewport(0, 0, 1600, 1200));
	GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

	prepareCamera();
	prepare();
	initIMGUI();

	while (app->update()) {
		cameraControl->update();
		ecs::syncTransforms(registry);
		renderer->setClearColor(clearColor);
		renderer->render(scene, camera, dirLight, ambLight);
		renderIMGUI();
	}

	app->destroy();

	return 0;
}