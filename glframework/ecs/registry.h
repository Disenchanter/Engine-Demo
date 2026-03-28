#pragma once
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>
#include <typeindex>
#include <type_traits>
#include <cstdint>

namespace ecs {
using Entity = std::uint32_t;
constexpr Entity InvalidEntity = 0;

class Registry {
public:
	Entity create();
	void destroy(Entity entity);

	template<typename T, typename... Args>
	T& add(Entity entity, Args&&... args) {
		auto& storage = getStorage<T>();
		return storage.data.emplace(entity, T{ std::forward<Args>(args)... }).first->second;
	}

	template<typename T>
	bool has(Entity entity) const {
		auto storage = tryStorage<T>();
		return storage && storage->has(entity);
	}

	template<typename T>
	T& get(Entity entity) {
		auto& storage = getStorage<T>();
		return storage.data.at(entity);
	}

	template<typename T>
	void remove(Entity entity) {
		auto storage = tryStorage<T>();
		if (storage) {
			storage->remove(entity);
		}
	}

	template<typename... Ts>
	std::vector<Entity> view() const {
		std::vector<Entity> result;
		const auto* primary = tryStorage<std::tuple_element_t<0, std::tuple<Ts...>>>();
		if (!primary) {
			return result;
		}

		for (const auto& pair : primary->data) {
			Entity entity = pair.first;
			if ((has<Ts>(entity) && ...)) {
				result.push_back(entity);
			}
		}
		return result;
	}

private:
	struct IStorage {
		virtual ~IStorage() = default;
		virtual void remove(Entity entity) = 0;
		virtual bool has(Entity entity) const = 0;
	};

	template<typename T>
	struct Storage : IStorage {
		std::unordered_map<Entity, T> data;

		void remove(Entity entity) override {
			data.erase(entity);
		}

		bool has(Entity entity) const override {
			return data.find(entity) != data.end();
		}
	};

	Entity mNext{ 1 };
	std::unordered_set<Entity> mEntities;
	std::unordered_map<std::size_t, std::unique_ptr<IStorage>> mStorages;

	template<typename T>
	Storage<T>& getStorage() {
		auto typeId = typeid(T).hash_code();
		auto iter = mStorages.find(typeId);
		if (iter == mStorages.end()) {
			auto storage = std::make_unique<Storage<T>>();
			iter = mStorages.emplace(typeId, std::move(storage)).first;
		}
		return *static_cast<Storage<T>*>(iter->second.get());
	}

	template<typename T>
	const Storage<T>* tryStorage() const {
		auto typeId = typeid(T).hash_code();
		auto iter = mStorages.find(typeId);
		if (iter == mStorages.end()) {
			return nullptr;
		}
		return static_cast<const Storage<T>*>(iter->second.get());
	}
};

inline Entity Registry::create() {
	Entity entity = mNext++;
	mEntities.insert(entity);
	return entity;
}

inline void Registry::destroy(Entity entity) {
	if (mEntities.erase(entity) == 0) {
		return;
	}
	for (auto& pair : mStorages) {
		pair.second->remove(entity);
	}
}

}
