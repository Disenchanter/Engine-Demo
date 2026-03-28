#pragma once
#include "material.h"
#include "../texture.h"

class PBRMaterial :public Material {
public:
	PBRMaterial();
	~PBRMaterial();

public:
	Texture* mAlbedo{ nullptr };
	Texture* mMetallic{ nullptr };
	Texture* mRoughness{ nullptr };
	Texture* mAO{ nullptr };
	float mMetallicFactor{ 1.0f };
	float mRoughnessFactor{ 1.0f };
	float mAOFactor{ 1.0f };
};
