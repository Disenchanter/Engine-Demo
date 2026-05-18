#include "object.h"

Object::Object() {
	mType = ObjectType::Object;
}

Object::~Object() {

}

void Object::setPosition(glm::vec3 pos) {
	mPosition = pos;
}

void Object::setRotation(glm::vec3 rotation) {
	mAngleX = rotation.x;
	mAngleY = rotation.y;
	mAngleZ = rotation.z;
}

// Rotate around axes / 軸回転
void Object::rotateX(float angle) {
	mAngleX += angle;
}

void Object::rotateY(float angle) {
	mAngleY += angle;
}

void Object::rotateZ(float angle) {
	mAngleZ += angle;
}

void Object::setScale(glm::vec3 scale) {
	mScale = scale;
}

glm::mat4 Object::getModelMatrix() {
	// Get parent matrix if exists / 親の行列を取得
	glm::mat4 parentMatrix{ 1.0f };
	if (mParent != nullptr) {
		parentMatrix = mParent->getModelMatrix();
	}


	// Build transform: scale, rotate (pitch/yaw/roll), translate
	glm::mat4 transform{ 1.0f };

	transform = glm::scale(transform, mScale);

	// Rotation order: pitch, yaw, roll (Unity convention)
	transform = glm::rotate(transform, glm::radians(mAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(mAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(mAngleZ), glm::vec3(0.0f, 0.0f, 1.0f));

	transform = parentMatrix * glm::translate(glm::mat4(1.0f),mPosition) * transform;

	return transform;
}


void Object::addChild(Object* obj) {
	// 1: avoid duplicate child / 重複した子を避ける
	auto iter = std::find(mChildren.begin(), mChildren.end(), obj);
	if (iter != mChildren.end()) {
		std::cerr << "Duplicated Child added" << std::endl;
		return;
	}

	// 2: add to list / リストに追加
	mChildren.push_back(obj);

	// 3: set parent pointer / 親ポインタを設定
	obj->mParent = this;
}

std::vector<Object*>  Object::getChildren() {
	return mChildren;
}

Object* Object::getParent() {
	return mParent;
}