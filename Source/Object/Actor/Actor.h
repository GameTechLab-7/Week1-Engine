﻿#pragma once
#include "Core/EngineTypes.h"
#include "Object/UObject.h"
#include "Object/ActorComponent/ActorComponent.h"
#include "Core/Math/Transform.h"
#include "Core/Container/Set.h"
#include "Object/ObjectFactory.h"
#include "Object/USceneComponent.h"

class UWorld;

class AActor : public UObject
{
	friend class FEditorManager;
public:
	AActor() = default;
	virtual ~AActor() override = default;

public:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void LateTick (float DeltaTime); // 렌더 후 호출
	
	virtual void Destroyed();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	TSet<UActorComponent*>& GetComponents() { return Components; }

	UWorld* GetWorld() const { return World; }
	void SetWorld(UWorld* InWorld) { World = InWorld; }

private:
	virtual void Pick();
	virtual void UnPick();

public:
	template<typename T>
		requires std::derived_from<T, UActorComponent>
	T* AddComponent()
	{
		T* ObjectInstance = FObjectFactory::ConstructObject<T>();
		Components.Add(ObjectInstance);
		ObjectInstance->SetOwner(this);

		return ObjectInstance;
	}

	// delete
	template<typename T>
		requires std::derived_from<T, UActorComponent>
	void RemoveComponent(T* Object)
	{
		Components.Remove(Object);
	}

	const FTransform& GetActorTransform() const;
	void SetActorTransform(const FTransform& InTransform);
	bool CanEverTick() const { return bCanEverTick; }
	virtual const char* GetTypeName();

	bool Destroy();

public:
	USceneComponent* GetRootComponent() const { return RootComponent; }
	void SetRootComponent(USceneComponent* InRootComponent) { RootComponent = InRootComponent; }

public:
	void SetColor(FVector4 InColor);
	void SetUseVertexColor(bool bUseVertexColor);

protected:
	bool bCanEverTick = true;
	USceneComponent* RootComponent = nullptr;

private:
	UWorld* World = nullptr;
	TSet<UActorComponent*> Components;
};

