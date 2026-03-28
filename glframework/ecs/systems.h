#pragma once
#include "registry.h"
#include "components.h"
#include <queue>
#include <limits>
#include <utility>
#include <cmath>
#include <algorithm>

namespace ecs {
inline void syncTransforms(Registry& registry) {
	auto entities = registry.view<TransformComponent, MeshComponent>();
	for (auto entity : entities) {
		auto& transform = registry.get<TransformComponent>(entity);
		auto& meshComp = registry.get<MeshComponent>(entity);
		if (meshComp.mesh == nullptr) {
			continue;
		}

		meshComp.mesh->setPosition(transform.position);
		meshComp.mesh->setRotation(transform.rotation);
		meshComp.mesh->setScale(transform.scale);
	}
}

inline void syncPhysicsToTransform(Registry& registry) {
    (void)registry;
}

inline void syncTransformToPhysics(Registry& registry) {
    (void)registry;
}

inline std::vector<glm::ivec2> findPath(const GridComponent& grid, const glm::ivec2& start, const glm::ivec2& goal) {
	if (!grid.isWalkable(start) || !grid.isWalkable(goal)) {
		return {};
	}

	const int total = grid.width * grid.height;
	std::vector<float> gScore(total, std::numeric_limits<float>::infinity());
	std::vector<int> cameFrom(total, -1);
	auto index = [grid](const glm::ivec2& cell) {
		return cell.y * grid.width + cell.x;
	};
	auto heuristic = [](const glm::ivec2& a, const glm::ivec2& b) {
		return static_cast<float>(std::abs(a.x - b.x) + std::abs(a.y - b.y));
	};

	using Node = std::pair<float, glm::ivec2>;
	auto compare = [](const Node& a, const Node& b) { return a.first > b.first; };
	std::priority_queue<Node, std::vector<Node>, decltype(compare)> openSet(compare);

	gScore[index(start)] = 0.0f;
	openSet.emplace(heuristic(start, goal), start);

	const glm::ivec2 directions[] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

	while (!openSet.empty()) {
		auto current = openSet.top().second;
		if (current == goal) {
			break;
		}
		openSet.pop();

		for (const auto& dir : directions) {
			glm::ivec2 neighbor = current + dir;
			if (!grid.isWalkable(neighbor)) {
				continue;
			}
			int currentIndex = index(current);
			int neighborIndex = index(neighbor);
			float tentative = gScore[currentIndex] + 1.0f;
			if (tentative < gScore[neighborIndex]) {
				cameFrom[neighborIndex] = currentIndex;
				gScore[neighborIndex] = tentative;
				float fScore = tentative + heuristic(neighbor, goal);
				openSet.emplace(fScore, neighbor);
			}
		}
	}

	std::vector<glm::ivec2> path;
	int currentIndex = index(goal);
	if (cameFrom[currentIndex] == -1 && start != goal) {
		return path;
	}
	path.push_back(goal);
	while (currentIndex != index(start)) {
		currentIndex = cameFrom[currentIndex];
		if (currentIndex < 0) {
			return {};
		}
		glm::ivec2 cell{ currentIndex % grid.width, currentIndex / grid.width };
		path.push_back(cell);
	}
	std::reverse(path.begin(), path.end());
	return path;
}

inline void updatePathRequests(Registry& registry, const GridComponent& grid) {
	auto entities = registry.view<PathRequestComponent, PathComponent>();
	for (auto entity : entities) {
		auto& request = registry.get<PathRequestComponent>(entity);
		auto& path = registry.get<PathComponent>(entity);
		if (!request.dirty) {
			continue;
		}
		request.dirty = false;
		path.points.clear();
		path.currentIndex = 0;
		auto cells = findPath(grid, request.start, request.goal);
		for (const auto& cell : cells) {
			path.points.push_back(grid.cellToWorld(cell));
		}
	}
}

inline void followPaths(Registry& registry, float deltaTime) {
	auto entities = registry.view<TransformComponent, PathComponent, MovementComponent>();
	for (auto entity : entities) {
		auto& transform = registry.get<TransformComponent>(entity);
		auto& path = registry.get<PathComponent>(entity);
		auto& movement = registry.get<MovementComponent>(entity);
		if (path.currentIndex >= path.points.size()) {
			continue;
		}

		glm::vec3 target = path.points[path.currentIndex];
		glm::vec3 toTarget = target - transform.position;
		float distance = glm::length(toTarget);
		if (distance < 0.01f) {
			path.currentIndex++;
			continue;
		}

		glm::vec3 direction = toTarget / distance;
		float step = movement.speed * deltaTime;
		transform.position += direction * std::min(step, distance);
	}
}

inline bool intersectsAabb(const TransformComponent& a, const ColliderComponent& ac,
	const TransformComponent& b, const ColliderComponent& bc) {
	glm::vec3 delta = glm::abs(a.position - b.position);
	glm::vec3 limits = ac.halfExtents + bc.halfExtents;
	return delta.x <= limits.x && delta.y <= limits.y && delta.z <= limits.z;
}

inline std::vector<std::pair<Entity, Entity>> detectCollisions(Registry& registry) {
	std::vector<std::pair<Entity, Entity>> collisions;
	auto entities = registry.view<TransformComponent, ColliderComponent>();
	for (std::size_t i = 0; i < entities.size(); ++i) {
		for (std::size_t j = i + 1; j < entities.size(); ++j) {
			auto a = entities[i];
			auto b = entities[j];
			const auto& ta = registry.get<TransformComponent>(a);
			const auto& tb = registry.get<TransformComponent>(b);
			const auto& ca = registry.get<ColliderComponent>(a);
			const auto& cb = registry.get<ColliderComponent>(b);
			if (intersectsAabb(ta, ca, tb, cb)) {
				collisions.emplace_back(a, b);
			}
		}
	}
	return collisions;
}
}
