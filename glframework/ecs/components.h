#pragma once
#include "../mesh.h"
#include "../core.h"
#include <vector>
#include <cstddef>

// Forward declare PhysX types
namespace physx {
    class PxRigidActor;
    class PxRigidDynamic;
    class PxShape;
}

namespace ecs {
struct TransformComponent {
	glm::vec3 position{ 0.0f };
	glm::vec3 rotation{ 0.0f };
	glm::vec3 scale{ 1.0f };
};

struct MeshComponent {
	Mesh* mesh{ nullptr };
};

struct MovementComponent {
	float speed{ 1.0f };
};

struct PathComponent {
	std::vector<glm::vec3> points{};
	std::size_t currentIndex{ 0 };
};

struct PathRequestComponent {
	glm::ivec2 start{ 0 };
	glm::ivec2 goal{ 0 };
	bool dirty{ true };
};

struct GridComponent {
	int width{ 0 };
	int height{ 0 };
	float cellSize{ 1.0f };
	glm::vec3 origin{ 0.0f };
	std::vector<int> cells{};

	bool isWalkable(const glm::ivec2& cell) const {
		if (cell.x < 0 || cell.y < 0 || cell.x >= width || cell.y >= height) {
			return false;
		}
		if (cells.empty()) {
			return true;
		}
		return cells[cell.y * width + cell.x] != 0;
	}

	glm::vec3 cellToWorld(const glm::ivec2& cell) const {
		return origin + glm::vec3((cell.x + 0.5f) * cellSize, 0.0f, (cell.y + 0.5f) * cellSize);
	}
};

struct ColliderComponent {
	glm::vec3 halfExtents{ 0.5f };
	bool isTrigger{ false };
};

// PhysX Components
struct RigidbodyComponent {
	physx::PxRigidActor* actor{ nullptr };
	float mass{ 1.0f };
	bool isKinematic{ false };
};

enum class PhysicsShapeType {
	Box,
	Sphere,
	Capsule
};

struct PhysicsShapeComponent {
	physx::PxShape* shape{ nullptr };
	PhysicsShapeType type{ PhysicsShapeType::Box };
	glm::vec3 size{ 0.5f };
};

}
