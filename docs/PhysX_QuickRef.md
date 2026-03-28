# PhysX 核心概念速查

## ??? 基础架构

### 1. **Foundation** - 基础系统
- 所有PhysX功能的基础
- 管理内存分配、错误回调、日志
```cpp
PxFoundation* foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, errorCallback);
```

### 2. **Physics SDK** - 物理引擎核心
- 创建所有物理对象的工厂
```cpp
PxPhysics* physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, scale);
```

### 3. **Scene** - 物理场景
- 包含所有物理对象
- 执行模拟计算
```cpp
PxScene* scene = physics->createScene(sceneDesc);
scene->simulate(deltaTime);
scene->fetchResults(true);
```

---

## ?? Actor 类型

### **PxRigidStatic** - 静态刚体
- 不移动的物体（地面、墙壁）
- 只参与碰撞检测
```cpp
PxRigidStatic* ground = physics->createRigidStatic(transform);
```

### **PxRigidDynamic** - 动态刚体
- 受力影响、有质量
- 完全由物理模拟控制
```cpp
PxRigidDynamic* box = physics->createRigidDynamic(transform);
box->setMass(10.0f);
box->addForce(PxVec3(0, 100, 0));
```

### **Kinematic Actor** - 运动学刚体
- 由代码直接控制位置
- 可以推动其他动态物体
```cpp
actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
actor->setKinematicTarget(newTransform);
```

---

## ?? Shape - 碰撞形状

### 基础几何体
```cpp
// Box
PxBoxGeometry box(halfX, halfY, halfZ);

// Sphere
PxSphereGeometry sphere(radius);

// Capsule
PxCapsuleGeometry capsule(radius, halfHeight);

// Plane
PxPlaneGeometry plane();
```

### 创建Shape
```cpp
PxShape* shape = physics->createShape(geometry, *material);
actor->attachShape(*shape);
```

---

## ?? Material - 物理材质

控制摩擦和弹性：
```cpp
// (静摩擦, 动摩擦, 恢复系数)
PxMaterial* mat = physics->createMaterial(0.5f, 0.5f, 0.6f);

// 静摩擦：0=冰面，1=橡胶
// 动摩擦：运动时的摩擦
// 恢复系数：0=完全不弹，1=完全弹回
```

---

## ?? 施加力

### Force Modes
```cpp
// 持续力（需要每帧调用）
actor->addForce(force, PxForceMode::eFORCE);

// 冲量（瞬间改变速度）
actor->addForce(force, PxForceMode::eIMPULSE);

// 加速度
actor->addForce(force, PxForceMode::eACCELERATION);

// 速度变化
actor->addForce(force, PxForceMode::eVELOCITY_CHANGE);
```

---

## ?? 实用API

### 查询位置/速度
```cpp
PxTransform transform = actor->getGlobalPose();
PxVec3 velocity = actor->getLinearVelocity();
PxVec3 angularVel = actor->getAngularVelocity();
```

### 设置位置/速度
```cpp
actor->setGlobalPose(newTransform);
actor->setLinearVelocity(velocity);
actor->setAngularVelocity(angularVel);
```

### 质量相关
```cpp
actor->setMass(10.0f);
actor->setMassSpaceInertiaTensor(inertia);
PxRigidBodyExt::updateMassAndInertia(*actor, density);
```

### 约束
```cpp
// 锁定旋转轴
actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);

// 锁定位移轴
actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, true);
```

---

## ?? 射线检测

```cpp
PxRaycastBuffer hit;
bool hasHit = scene->raycast(origin, direction, maxDistance, hit);
if (hasHit) {
    PxVec3 hitPoint = hit.block.position;
    PxVec3 hitNormal = hit.block.normal;
    PxRigidActor* hitActor = hit.block.actor;
}
```

---

## ?? 性能优化

### 碰撞过滤
```cpp
// 设置过滤数据
PxFilterData filterData;
filterData.word0 = 1; // 自己的层
filterData.word1 = 2; // 可碰撞的层
shape->setSimulationFilterData(filterData);
```

### 休眠阈值
```cpp
// 速度低于此值时自动休眠
actor->setSleepThreshold(0.1f);
actor->setWakeCounter(0.4f);
```

---

## ?? 调试工具 PVD

连接到 PhysX Visual Debugger：
```cpp
PxPvd* pvd = PxCreatePvd(*foundation);
PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
```

启动 PVD 工具可以实时查看：
- 所有物理对象
- 碰撞形状
- 力和速度向量
- 性能统计

---

## ?? 典型使用流程

```cpp
// 1. 初始化
PxFoundation* foundation = PxCreateFoundation(...);
PxPhysics* physics = PxCreatePhysics(...);
PxScene* scene = physics->createScene(sceneDesc);

// 2. 创建材质
PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.6f);

// 3. 创建形状
PxShape* shape = physics->createShape(PxBoxGeometry(1,1,1), *material);

// 4. 创建Actor
PxRigidDynamic* actor = physics->createRigidDynamic(transform);
actor->attachShape(*shape);
PxRigidBodyExt::updateMassAndInertia(*actor, 10.0f);

// 5. 添加到场景
scene->addActor(*actor);

// 6. 主循环
scene->simulate(1.0f/60.0f);
scene->fetchResults(true);
PxTransform newTransform = actor->getGlobalPose();

// 7. 清理
scene->release();
physics->release();
foundation->release();
```

---

## ?? 学习建议

1. **先实现静态+动态物体碰撞**
2. **再加入力/冲量控制**
3. **然后尝试射线检测**
4. **最后研究关节和约束**

每个概念都有对应的示例代码在 `examples/physicsExample.cpp` 中。
