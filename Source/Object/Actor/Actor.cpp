﻿#include "Actor.h"
#include "Object/USceneComponent.h"
#include "Debug/DebugConsole.h"
#include "Object/World/World.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Static/FEditorManager.h"

AActor::AActor() : Depth{ 0 }
{
	RootComponent = AddComponent<USceneComponent>();
}

void AActor::BeginPlay()
{
	for (auto& Component : Components)
	{
		Component->BeginPlay();

		if (UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component))
		{
			PrimitiveComponent->RegisterComponentWithWorld(World);
		}
	}
}

void AActor::Tick(float DeltaTime)
{
	for (auto& Component : Components)
	{
		if (Component->CanEverTick())
		{
			Component->Tick(DeltaTime);
		}
	}
}

void AActor::LateTick(float DeltaTime)
{
}

void AActor::Destroyed()
{
	EndPlay(EEndPlayReason::Destroyed);
}


void AActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (auto& Component : Components)
	{		
		Component->EndPlay(EndPlayReason);
		if (const auto PrimitiveComp = dynamic_cast<UPrimitiveComponent*>(Component))
		{
			if (World->ContainsZIgnoreComponent(PrimitiveComp))
			{
				World->RemoveZIgnoreComponent(PrimitiveComp);
			}
			
			GetWorld()->RemoveRenderComponent(PrimitiveComp);
		}
		if (FEditorManager::Get().GetSelectedActor() == this)
		{
			FEditorManager::Get().SelectActor(nullptr);
		}
		UEngine::Get().GObjects.Remove(Component->GetUUID());
	}
	Components.Empty();
}

void AActor::Pick()
{
	if (RootComponent)
	{
		bIsPicked = true;
		RootComponent->Pick(true);
	}
}

void AActor::UnPick()
{
	if (RootComponent)
	{
		bIsPicked = false;
		RootComponent->Pick(false);
	}	
}

FMatrix AActor::GetActorTransformMatrix()
{
	if (Parent)
	{
		// 부모가 있을 경우 부모 월드 * 내 로컬
		FMatrix ParentWorld = Parent->GetActorTransformMatrix();
		FMatrix MyLocal = GetActorRelativeTransform().GetMatrix();

		FMatrix NewMatrix = MyLocal * ParentWorld;
		return NewMatrix;
	}

	return GetActorRelativeTransformMatrix();
}

// actor의 root component는 actor의 (아직은) 월드상 좌표입니다.
// @TODO : actor가 parent를 갖게되면 actor의 parent의 transform을 따라가야합니다.
FTransform AActor::GetActorRelativeTransform() const
{
	return RootComponent != nullptr ? RootComponent->GetRelativeTransform() : FTransform();
}

FMatrix AActor::GetActorRelativeTransformMatrix() const
{
	return this->GetActorRelativeTransform().GetMatrix();
}

// actor의 root component는 actor의 (아직은) 월드상 좌표입니다.
void AActor::SetActorRelatvieTransform(const FTransform& InTransform)
{
	// InTransform은 월드 기준임
	if (RootComponent)
	{
		RootComponent->SetRelativeTransform(InTransform);
	}
	else
	{
		UE_LOG("RootComponent is nullptr");
	}
}

const char* AActor::GetTypeName()
{
	return "Actor";
}

bool AActor::Destroy()
{
	return GetWorld()->DestroyActor(this);
}

void AActor::SetColor(FVector4 InColor)
{
	if (RootComponent == nullptr)
	{
		return;
	}

	UPrimitiveComponent* RootPrimitive = dynamic_cast<UPrimitiveComponent*>(RootComponent);
	if (RootPrimitive)
	{
		RootPrimitive->SetCustomColor(InColor);
	}

	for (auto& Component : Components)
	{
		UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component);
		if (PrimitiveComponent)
		{
			PrimitiveComponent->SetCustomColor(InColor);
		}
	}
}

void AActor::SetUseVertexColor(bool bUseVertexColor)
{
	if (RootComponent == nullptr)
	{
		return;
	}

	UPrimitiveComponent* RootPrimitive = dynamic_cast<UPrimitiveComponent*>(RootComponent);
	if (RootPrimitive)
	{
		RootPrimitive->SetUseVertexColor(bUseVertexColor);
	}

	for (auto& Component : Components)
	{
		UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component);
		if (PrimitiveComponent)
		{
			PrimitiveComponent->SetUseVertexColor(bUseVertexColor);
		}
	}
}

void AActor::SetupAttachment(AActor* InParent)
{
	if (InParent)
	{
		Parent = InParent;
		InParent->Children.Add(this);
	}
	else
	{
		UE_LOG("Parent is nullptr");
	}
}