#pragma once
#include "../core.h"
#include <memory>

namespace physx {
class PxPhysics;
class PxScene;
class PxMaterial;
class PxRigidDynamic;
class PxRigidStatic;
class PxShape;
}

class PhysicsWorld {
public:
    static PhysicsWorld& getInstance();

    bool initialize();
    void shutdown();
    void update(float deltaTime);

    physx::PxPhysics* getPhysics() { return mPhysics; }
    physx::PxScene* getScene() { return mScene; }
    physx::PxMaterial* getDefaultMaterial() { return mDefaultMaterial; }

    physx::PxRigidDynamic* createDynamicActor(const glm::vec3& position, physx::PxShape* shape);
    physx::PxRigidStatic* createStaticActor(const glm::vec3& position, physx::PxShape* shape);

    physx::PxShape* createBoxShape(const glm::vec3& halfExtents);
    physx::PxShape* createSphereShape(float radius);
    physx::PxShape* createCapsuleShape(float radius, float halfHeight);

private:
    PhysicsWorld() = default;
    ~PhysicsWorld() = default;
    PhysicsWorld(const PhysicsWorld&) = delete;
    PhysicsWorld& operator=(const PhysicsWorld&) = delete;

    physx::PxPhysics* mPhysics{ nullptr };
    physx::PxScene* mScene{ nullptr };
    physx::PxMaterial* mDefaultMaterial{ nullptr };
};
