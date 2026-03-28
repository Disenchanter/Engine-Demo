#include "glframework/physics/physicsWorld.h"
#include "glframework/ecs/registry.h"
#include "glframework/ecs/components.h"
#include "glframework/ecs/systems.h"

// 在 main.cpp 中的使用示例

void setupPhysicsExample() {
	// 1. 初始化PhysX世界
	PhysicsWorld::getInstance().initialize();
	
	ecs::Registry registry;
	
	// 2. 创建地面（静态刚体）
	auto groundEntity = registry.create();
	auto& groundTransform = registry.add<ecs::TransformComponent>(groundEntity);
	groundTransform.position = glm::vec3(0.0f, -5.0f, 0.0f);
	
	auto groundShape = PhysicsWorld::getInstance().createBoxShape(glm::vec3(50.0f, 0.5f, 50.0f));
	auto groundActor = PhysicsWorld::getInstance().createStaticActor(groundTransform.position, groundShape);
	
	auto& groundRb = registry.add<ecs::RigidbodyComponent>(groundEntity);
	groundRb.actor = groundActor;
	
	// 3. 创建动态物体（Box）
	auto boxEntity = registry.create();
	auto& boxTransform = registry.add<ecs::TransformComponent>(boxEntity);
	boxTransform.position = glm::vec3(0.0f, 10.0f, 0.0f);
	
	auto boxShape = PhysicsWorld::getInstance().createBoxShape(glm::vec3(1.0f, 1.0f, 1.0f));
	auto boxActor = PhysicsWorld::getInstance().createDynamicActor(boxTransform.position, boxShape);
	
	auto& boxRb = registry.add<ecs::RigidbodyComponent>(boxEntity);
	boxRb.actor = boxActor;
	boxRb.mass = 10.0f;
	
	// 4. 创建球体
	auto sphereEntity = registry.create();
	auto& sphereTransform = registry.add<ecs::TransformComponent>(sphereEntity);
	sphereTransform.position = glm::vec3(5.0f, 15.0f, 0.0f);
	
	auto sphereShape = PhysicsWorld::getInstance().createSphereShape(1.0f);
	auto sphereActor = PhysicsWorld::getInstance().createDynamicActor(sphereTransform.position, sphereShape);
	
	auto& sphereRb = registry.add<ecs::RigidbodyComponent>(sphereEntity);
	sphereRb.actor = sphereActor;
	
	// 5. 主循环
	float deltaTime = 0.016f; // 60 FPS
	
	// 更新物理
	PhysicsWorld::getInstance().update(deltaTime);
	
	// 同步物理结果到Transform
	ecs::syncPhysicsToTransform(registry);
	
	// 同步Transform到渲染
	ecs::syncTransforms(registry);
}

// 在主循环中使用
void physicsMainLoop(ecs::Registry& registry, float deltaTime) {
	// 1. 如果有Kinematic物体，先同步Transform到Physics
	ecs::syncTransformToPhysics(registry);
	
	// 2. 执行物理模拟
	PhysicsWorld::getInstance().update(deltaTime);
	
	// 3. 同步Physics结果到Transform
	ecs::syncPhysicsToTransform(registry);
	
	// 4. 同步Transform到渲染Mesh
	ecs::syncTransforms(registry);
}

// 添加力的示例
void applyForceExample(ecs::Registry& registry, ecs::Entity entity) {
	if (registry.has<ecs::RigidbodyComponent>(entity)) {
		auto& rb = registry.get<ecs::RigidbodyComponent>(entity);
		if (rb.actor && rb.actor->is<physx::PxRigidDynamic>()) {
			auto* dynamic = static_cast<physx::PxRigidDynamic*>(rb.actor);
			dynamic->addForce(physx::PxVec3(0.0f, 100.0f, 0.0f), physx::PxForceMode::eIMPULSE);
		}
	}
}
