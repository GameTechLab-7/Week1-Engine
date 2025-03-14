﻿#include "Core/HAL/PlatformType.h"
#include "Core/Rendering/URenderer.h"
#include "Picker.h"

#include "Core/Input/PlayerInput.h"
#include "Object/Gizmo/GizmoHandle.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Static/FEditorManager.h"
#include "ImGui/imgui.h"
#include "Camera.h"

#include "../URaycastSystem.h"

APicker::APicker()
{    
    bIsGizmo = true;
}

FVector4 APicker::EncodeUUID(unsigned int UUID)
{
    float a = (UUID >> 24) & 0xff;
    float b = (UUID >> 16) & 0xff;
    float g = (UUID >> 8) & 0xff;
    float r = UUID & 0xff;
	
    FVector4 color = {r, g, b, a};
    
    return color;
}

uint32_t APicker::DecodeUUID(FVector4 color)
{
    uint32_t W = static_cast<uint32_t>(color.W) << 24;
    uint32_t Z = static_cast<uint32_t>(color.Z) << 16;
    uint32_t Y = static_cast<uint32_t>(color.Y) << 8;
    uint32_t X = static_cast<uint32_t>(color.X);

    return W | Z | Y | X;
}

void APicker::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APicker::LateTick(float DeltaTime)
{
    AActor::LateTick(DeltaTime);

    if (APlayerInput::Get().GetMouseDown(false) && !ImGui::GetIO().WantCaptureMouse) {
        POINT pt;//TODO 하나로 합치기
        GetCursorPos(&pt);
        ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);

        float ratioX = UEngine::Get().GetInitializedScreenWidth() / (float)UEngine::Get().GetScreenWidth();
        float ratioY = UEngine::Get().GetInitializedScreenHeight() / (float)UEngine::Get().GetScreenHeight();
        pt.x = pt.x * ratioX;
        pt.y = pt.y * ratioY;

        FHitResult result;
        FVector location = FEditorManager::Get().GetCamera()->GetActorTransform().GetPosition();
        FVector dir = UEngine::Get().GetRenderer()->GetRayDirectionFromClick(FVector(pt.x, pt.y, 0));
        URaycastSystem::Raycast(location, dir, 100, result);
        UE_LOG("result: %d", result.bBlockingHit);
    }

    if(APlayerInput::Get().GetMouseDown(false) && !ImGui::GetIO().WantCaptureMouse)
    {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);


        float ratioX = UEngine::Get().GetInitializedScreenWidth() / (float)UEngine::Get().GetScreenWidth();
        float ratioY = UEngine::Get().GetInitializedScreenHeight() / (float)UEngine::Get().GetScreenHeight();
        pt.x = pt.x * ratioX;
        pt.y = pt.y * ratioY;
        
        FVector4 color = UEngine::Get().GetRenderer()->GetPixel(FVector(pt.x, pt.y, 0));

        uint32_t UUID = DecodeUUID(color);

        UActorComponent* PickedComponent = UEngine::Get().GetObjectByUUID<UActorComponent>(UUID);

        if (PickedComponent != nullptr)
        {
            AActor* PickedActor = PickedComponent->GetOwner();

            if (PickedActor == nullptr) return;
            if (PickedComponent->GetOwner()->IsGizmoActor() == false)
            {
                if (PickedActor == FEditorManager::Get().GetSelectedActor())
                {
                    FEditorManager::Get().SelectActor(nullptr);   
                }
                else
                {
                    FEditorManager::Get().SelectActor(PickedActor);
                }
            }
        }
        UE_LOG("Pick - UUID: %u", UUID);
    }

    if (APlayerInput::Get().IsPressedMouse(false))
    {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);
        FVector4 color = UEngine::Get().GetRenderer()->GetPixel(FVector(pt.x, pt.y, 0));
        uint32_t UUID = DecodeUUID(color);

        UActorComponent* PickedComponent = UEngine::Get().GetObjectByUUID<UActorComponent>(UUID);\
        if (PickedComponent != nullptr)
        {
            if (AGizmoHandle* Gizmo = dynamic_cast<AGizmoHandle*>(PickedComponent->GetOwner()))
            {
                if (Gizmo->GetSelectedAxis() != ESelectedAxis::None) return;
                UCylinderComp* CylinderComp = static_cast<UCylinderComp*>(PickedComponent);
                FVector4 CompColor = CylinderComp->GetCustomColor();
                if (1.0f - FMath::Abs(CompColor.X) < KINDA_SMALL_NUMBER) // Red - X축
                {
                    Gizmo->SetSelectedAxis(ESelectedAxis::X);
                }
                else if (1.0f - FMath::Abs(CompColor.Y) < KINDA_SMALL_NUMBER) // Green - Y축
                {
                    Gizmo->SetSelectedAxis(ESelectedAxis::Y);
                }
                else  // Blue - Z축
                {
                    Gizmo->SetSelectedAxis(ESelectedAxis::Z);
                }
            }
        }
    }
    else
    {
        if (AGizmoHandle* Handle = FEditorManager::Get().GetGizmoHandle())
        {
            Handle->SetSelectedAxis(ESelectedAxis::None);
        }
    }
}

const char* APicker::GetTypeName()
{
    return "Picker";
}
