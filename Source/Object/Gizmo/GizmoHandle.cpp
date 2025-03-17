﻿#include "GizmoHandle.h"

#include "Object/Actor/Camera.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Object/World/World.h"
#include "Static/FEditorManager.h"
#include <Core/Input/PlayerInput.h>

AGizmoHandle::AGizmoHandle()
{
	bIsGizmo = true;
	
	// !NOTE : Z방향으로 서있음
	// z
	UCylinderComp* ZArrow = AddComponent<UCylinderComp>();
	ZArrow->SetupAttachment(RootComponent);
	ZArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1, 1, 1)));
	ZArrow->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));
	CylinderComponents.Add(ZArrow);
	
	// x
	UCylinderComp* XArrow = AddComponent<UCylinderComp>();
	XArrow->SetupAttachment(RootComponent);
	XArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 90.0f, 0.0f), FVector(1, 1, 1)));
	XArrow->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));
	CylinderComponents.Add(XArrow);
	
	// y
	UCylinderComp* YArrow = AddComponent<UCylinderComp>();
	YArrow->SetupAttachment(RootComponent);
	YArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(90.0f, 0.0f, 0.0f), FVector(1, 1, 1)));
	YArrow->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));
	CylinderComponents.Add(YArrow);
	
	UEngine::Get().GetWorld()->AddZIgnoreComponent(ZArrow);
	UEngine::Get().GetWorld()->AddZIgnoreComponent(XArrow);
	UEngine::Get().GetWorld()->AddZIgnoreComponent(YArrow);

	SetActive(false);
}

void AGizmoHandle::Tick(float DeltaTime)
{
	AActor* SelectedActor  = FEditorManager::Get().GetSelectedActor();
	if (SelectedActor != nullptr && bIsActive)
	{
		// Gizmo의 위치정보를 받아옵니다.
		FTransform GizmoTr = SelectedActor->GetRootComponent()->GetRelativeTransform();
		//GizmoTr.SetPosition(SelectedActor->GetActorTransform().GetPosition());
		GizmoTr.SetRotation(FVector(0.0f, 0.0f, 0.0f));
		GizmoTr.SetScale(FVector(1,1,1));
		// Actor의 Root component == 위치정보를 수정합니다.
		SetActorRelatvieTransform(GizmoTr);
	}

	SetScaleByDistance();
	
	AActor::Tick(DeltaTime);

	if (APlayerInput::Get().GetKeyDown(EKeyCode::Space))
	{
 		int type = static_cast<int>(GizmoType);
		type = (type + 1) % static_cast<int>(EGizmoType::Max);
		GizmoType = static_cast<EGizmoType>(type);
	}

}

void AGizmoHandle::SetScaleByDistance()
{
	FTransform MyTransform = GetActorRelativeTransform();
	
	// 액터의 월드 위치
	FVector actorWorldPos = MyTransform.GetPosition();

	FTransform CameraTransform = FEditorManager::Get().GetCamera()->GetActorRelativeTransform();
	
	// 카메라의 월드 위치
	FVector cameraWorldPos = CameraTransform.GetPosition();

	// 거리 계산
	float distance = (actorWorldPos - cameraWorldPos).Length();

	float baseScale = 1.0f;    // 기본 스케일
	float scaleFactor = distance * 0.1f; // 거리에 비례하여 스케일 증가

	// float minScale = 1.0f;     // 최소 스케일
	// float maxScale = 1.0f;     // 최대 스케일
	// float scaleFactor = clamp(1.0f / distance, minScale, maxScale);

	MyTransform.SetScale(scaleFactor, scaleFactor, scaleFactor);
}

void AGizmoHandle::SetActive(bool bActive)
{
	bIsActive = bActive;
	for (auto& Component : Components)
	{
		if (UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component))
		{
			PrimitiveComponent->SetCanBeRendered(bActive);
		}
	}
}

const char* AGizmoHandle::GetTypeName()
{
	return "GizmoHandle";
}

void AGizmoHandle::DoTransform(FTransform& AT, FVector Result, AActor* Actor )
{
	const FVector& AP = AT.GetPosition();

	if (SelectedAxis == ESelectedAxis::X)
	{
		switch (GizmoType)
		{
		case EGizmoType::Translate:
			AT.SetPosition({ Result.X, AP.Y, AP.Z });
			break;
		case EGizmoType::Rotate:
			AT.RotatePitch(Result.X);
			break;
		case EGizmoType::Scale:
			AT.AddScale({ Result.X * .1f, 0, AP.Z * .1f });
			break;
		}
	}
	else if (SelectedAxis == ESelectedAxis::Y)
	{
		switch (GizmoType)
		{
		case EGizmoType::Translate:
			AT.SetPosition({ AP.X, Result.Y, AP.Z });
			break;
		case EGizmoType::Rotate:
			AT.RotateRoll(Result.Y);
			break;
		case EGizmoType::Scale:
			AT.AddScale({ 0, Result.Y * .1f, 0 });
			break;
		}
	}
	else if (SelectedAxis == ESelectedAxis::Z)
	{
		switch (GizmoType)
		{
		case EGizmoType::Translate:
			AT.SetPosition({ AP.X, AP.Y, Result.Z });
			break;
		case EGizmoType::Rotate:
			AT.RotatePitch(-Result.Z);
			break;
		case EGizmoType::Scale:
			AT.AddScale({0, 0, Result.Z * .1f });
			break;
		}
	}
	Actor->SetActorRelatvieTransform(AT);
}

