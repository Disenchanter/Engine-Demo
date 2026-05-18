#include "renderer.h"
#include <iostream>
#include "../material/phongMaterial.h"
#include "../material/whiteMaterial.h"
#include "../material/pbrMaterial.h"
#include <string>//stl string

Renderer::Renderer() {
	mPhongShader = new Shader("assets/shaders/phong.vert", "assets/shaders/phong.frag");
	mWhiteShader = new Shader("assets/shaders/white.vert", "assets/shaders/white.frag");
	mPbrShader = new Shader("assets/shaders/pbr.vert", "assets/shaders/pbr.frag");
}

Renderer::~Renderer() {

}

void Renderer::setClearColor(glm::vec3 color) {
	glClearColor(color.r, color.g, color.b, 1.0);
}

void Renderer::render(
	Scene* scene, 
	Camera* camera,
	DirectionalLight* dirLight,
	AmbientLight* ambLight
) {
	// 1: Set OpenGL state for rendering this frame / フレーム描画のためのOpenGL状態を設定
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// 2: Clear buffers / バッファのクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 3: Render starting from the scene root (recursive) / シーンルートから再帰的に描画
	renderObject(scene, camera, dirLight, ambLight);
}

Shader* Renderer::pickShader(MaterialType type) {
	Shader* result = nullptr;

	switch (type) {
	case MaterialType::PhongMaterial:
		result = mPhongShader;
		break;
	case MaterialType::WhiteMaterial:
		result = mWhiteShader;
		break;
	case MaterialType::PBRMaterial:
		result = mPbrShader;
		break;
	default:
		std::cout << "Unknown material type to pick shader" << std::endl;
		break;
	}

	return result;
}


// Render a single object (recursive for children) / 単一オブジェクトを描画（子は再帰的に処理）
void Renderer::renderObject(
	Object* object,
	Camera* camera,
	DirectionalLight* dirLight,
	AmbientLight* ambLight
) {
	// 1: If object is a Mesh, draw it / オブジェクトがメッシュなら描画
	if (object->getType() == ObjectType::Mesh) {
		auto mesh = (Mesh*)object;
		auto geometry = mesh->mGeometry;
		auto material = mesh->mMaterial;

		// 1: Choose shader / 使用するシェーダを選択
		Shader* shader = pickShader(material->mType);

		// 2: Update shader uniforms / シェーダのuniformを更新
		shader->begin();

		switch (material->mType) {
		case MaterialType::PhongMaterial: {
			PhongMaterial* phongMat = (PhongMaterial*)material;

			// Bind diffuse texture to unit 0 / ディフューズテクスチャをユニット0にバインド
			shader->setInt("sampler", 0);
			phongMat->mDiffuse->bind();

			// Bind specular mask to unit 1 / スペキュラマスクをユニット1にバインド
			shader->setInt("specularMaskSampler", 1);
			phongMat->mSpecularMask->bind();

			// MVP matrices / モデル?ビュー?プロジェクション行列
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			// Update light uniforms (directional) / ライト情報を更新（方向光）
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->mDirection);
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);

			shader->setFloat("shiness", phongMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);

			// Camera info / カメラ情報
			shader->setVector3("cameraPosition", camera->mPosition);

		}
					break;
		case MaterialType::PBRMaterial: {
			PBRMaterial* pbrMat = (PBRMaterial*)material;

			// Bind albedo map to unit 0 / アルベドマップをユニット0にバインド
			shader->setInt("albedoMap", 0);
			pbrMat->mAlbedo->bind();

			// Set PBR factor uniforms (metallic/roughness/ao) / PBRの係数をセット
			shader->setFloat("metallicFactor", pbrMat->mMetallicFactor);
			shader->setFloat("roughnessFactor", pbrMat->mRoughnessFactor);
			shader->setFloat("aoFactor", pbrMat->mAOFactor);

			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->mDirection);

			shader->setVector3("ambientColor", ambLight->mColor);
			shader->setVector3("cameraPosition", camera->mPosition);
		}
					break;
		case MaterialType::WhiteMaterial: {
			// MVP for white material / White material のMVP
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
		}
					break;
		default:
			break;
		}

		// 3: Bind VAO and draw / VAOをバインドして描画
		glBindVertexArray(geometry->getVao());

		// 4: Draw call / 描画コール
		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
	}


	// 2: Iterate children and render each recursively / 子ノードを列挙して再帰的に描画
	auto children = object->getChildren();
	for (int i = 0; i < children.size(); i++) {
		renderObject(children[i], camera, dirLight, ambLight);
	}
}




// (Old alternative render implementations commented out)
