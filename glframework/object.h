#pragma once 
#include "core.h"

enum class ObjectType {
	Object,
	Mesh,
	Scene
};

class Object {
public:
	Object();
	~Object();
	
	void setPosition(glm::vec3 pos);

	void setRotation(glm::vec3 rotation);

	// Rotate around axes / 軸回転
	void rotateX(float angle); 
	void rotateY(float angle); 
	void rotateZ(float angle); 

	void setScale(glm::vec3 scale);

	glm::vec3 getPosition()const { return mPosition; }

	glm::mat4 getModelMatrix();

	// Hierarchy management / 子ノード管理
	void addChild(Object* obj);
	std::vector<Object*>  getChildren();
	Object* getParent();

	// Get object type / オブジェクトタイプを取得
	ObjectType getType()const { return mType; }

protected:
	glm::vec3 mPosition{ 0.0f };
	
	// Rotation angles: pitch, yaw, roll (Unity convention) / 回転角（ピッチ、ヨー、ロール）
	float mAngleX{ 0.0f };
	float mAngleY{ 0.0f };
	float mAngleZ{ 0.0f };

	glm::vec3 mScale{ 1.0f };

	// Children list and parent pointer / 子ノードリストと親ポインタ
	std::vector<Object*>	mChildren{};
	Object*				mParent{ nullptr };

	// Object type record / オブジェクトタイプ
	ObjectType	mType;
};