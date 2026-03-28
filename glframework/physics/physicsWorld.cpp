#include "physicsWorld.h"

PhysicsWorld& PhysicsWorld::getInstance() {
    static PhysicsWorld instance;
    return instance;
}

bool PhysicsWorld::initialize() {
    return true;
}

void PhysicsWorld::shutdown() {
}

void PhysicsWorld::update(float deltaTime) {
    (void)deltaTime;
}

physx::PxRigidDynamic* PhysicsWorld::createDynamicActor(const glm::vec3& position, physx::PxShape* shape) {
    (void)position;
    (void)shape;
    return nullptr;
}

physx::PxRigidStatic* PhysicsWorld::createStaticActor(const glm::vec3& position, physx::PxShape* shape) {
    (void)position;
    (void)shape;
    return nullptr;
}

physx::PxShape* PhysicsWorld::createBoxShape(const glm::vec3& halfExtents) {
    (void)halfExtents;
    return nullptr;
}

physx::PxShape* PhysicsWorld::createSphereShape(float radius) {
    (void)radius;
    return nullptr;
}

physx::PxShape* PhysicsWorld::createCapsuleShape(float radius, float halfHeight) {
    (void)radius;
    (void)halfHeight;
    return nullptr;
}
