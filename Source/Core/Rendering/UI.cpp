﻿#include "UI.h"

#include <algorithm>

#include "Camera.h"
#include "URenderer.h"
#include "Core/HAL/PlatformMemory.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "Debug/DebugConsole.h"
#include "ImGui/imgui_internal.h"
#include "Object/Actor/Actor.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Object/Actor/Sphere.h"
#include "Object/Actor/Cube.h"
#include "Object/Actor/Arrow.h"
#include "Static/FEditorManager.h"
#include "Object/World/World.h"



void UI::Initialize(HWND hWnd, const URenderer& Renderer, UINT ScreenWidth, UINT ScreenHeight)
{
    // ImGui 초기화
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // 기본 폰트 크기 설정
    io.FontGlobalScale = 1.0f;
    io.DisplaySize = ScreenSize;
    //io.WantSetMousePos = true;
    // ImGui Backend 초기화
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(Renderer.GetDevice(), Renderer.GetDeviceContext());

	ScreenSize = ImVec2(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight));
    InitialScreenSize = ScreenSize;
    bIsInitialized = true;

    UEngine::Get().GetWorld()->SpawnActor<AArrow>();
    
    io.DisplaySize = ScreenSize;
}

void UI::Update()
{
    POINT mousePos;
    if (GetCursorPos(&mousePos)) {
        HWND hwnd = GetActiveWindow();
        ScreenToClient(hwnd, &mousePos);

        ImVec2 CalculatedMousePos = ResizeToScreen(ImVec2(mousePos.x, mousePos.y));
        ImGui::GetIO().MousePos = CalculatedMousePos;
        //UE_LOG("MousePos: (%.1f, %.1f), DisplaySize: (%.1f, %.1f)\n",CalculatedMousePos.x, CalculatedMousePos.y, GetRatio().x, GetRatio().y);
    }

    
    // ImGui Frame 생성
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    RenderControlPanel();
    RenderPropertyWindow();

    Debug::ShowConsole(&bIsConsoleOpen);

    // ImGui 렌더링
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


void UI::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void UI::OnUpdateWindowSize(UINT InScreenWidth, UINT InScreenHeight)
{
    // ImGUI 리소스 다시 생성
    ImGui_ImplDX11_InvalidateDeviceObjects();
    ImGui_ImplDX11_CreateDeviceObjects();
   // ImGui 창 크기 업데이트
	ScreenSize = ImVec2(static_cast<float>(InScreenWidth), static_cast<float>(InScreenHeight));
}

void UI::RenderControlPanel()
{
    ImGui::Begin("Jungle Control Panel");
    ImGui::Text("Hello, Jungle World!");
    ImGui::Text("FPS: %.3f (what is that ms)", ImGui::GetIO().Framerate);

    RenderMemoryUsage();
    RenderPrimitiveSelection();
    RenderCameraSettings();
    
    ImGui::End();
}

void UI::RenderMemoryUsage()
{
    const uint64 ContainerAllocByte = FPlatformMemory::GetAllocationBytes<EAT_Container>();
    const uint64 ContainerAllocCount = FPlatformMemory::GetAllocationCount<EAT_Container>();
    const uint64 ObjectAllocByte = FPlatformMemory::GetAllocationBytes<EAT_Object>();
    const uint64 ObjectAllocCount = FPlatformMemory::GetAllocationCount<EAT_Object>();
    ImGui::Text(
        "Container Memory Uses: %llubyte, Count: %llu",
        ContainerAllocByte,
        ContainerAllocCount
    );
    ImGui::Text(
        "Object Memory Uses: %llubyte, Count: %llu Objects",
        ObjectAllocByte,
        ObjectAllocCount
    );
    ImGui::Text(
        "Total Memory Uses: %llubyte, Count: %llu",
        ContainerAllocByte + ObjectAllocByte,
        ContainerAllocCount + ObjectAllocCount
    );

    ImGui::Separator();
}

void UI::RenderPrimitiveSelection()
{
    const char* items[] = { "Sphere", "Cube", "Triangle" };

    ImGui::Combo("Primitive", &currentItem, items, IM_ARRAYSIZE(items));

    if (ImGui::Button("Spawn"))
    {
        UWorld* World = UEngine::Get().GetWorld();
            for (int i = 0 ;  i < NumOfSpawn; i++)
            {
                if (strcmp(items[currentItem], "Sphere") == 0)
                {
                    World->SpawnActor<ASphere>();
                }
            else if (strcmp(items[currentItem], "Cube") == 0)
                {
					World->SpawnActor<ACube>();
            }
            //else if (strcmp(items[currentItem], "Triangle") == 0)
            //{
            //    Actor->AddComponent<UTriangleComp>();   
            //}
        }
    }
    ImGui::SameLine();
    ImGui::InputInt("Number of spawn", &NumOfSpawn, 0);

    ImGui::Separator();

    UWorld* World = UEngine::Get().GetWorld();
    uint32 bufferSize = 100;
    char* SceneNameInput = new char[bufferSize];
    strcpy_s(SceneNameInput, bufferSize, World->SceneName.c_str());
    if (ImGui::Button("New Scene"))
    {
        World->SaveWorld();   
    }
    if (ImGui::Button("Save Scene"))
    {
        World->SaveWorld();   
    }
    if (ImGui::Button("Load Scene"))
    {
        World->LoadWorld(SceneNameInput);
    }
    ImGui::Separator();
}

void UI::RenderCameraSettings()
{
    ImGui::Text("Camera");

    FCamera& Camera = FCamera::Get();

    bool IsOrthogonal;
    if (Camera.ProjectionMode == ECameraProjectionMode::Orthographic)
    {
        IsOrthogonal = true;
    }
    else if (Camera.ProjectionMode == ECameraProjectionMode::Perspective)
    {
        IsOrthogonal = false;
    }

    if (ImGui::Checkbox("Orthogonal", &IsOrthogonal))
    {
        if (IsOrthogonal)
        {
            Camera.ProjectionMode = ECameraProjectionMode::Orthographic;
        }
        else
        {
            Camera.ProjectionMode = ECameraProjectionMode::Perspective;
        }
    }

    float FOV = Camera.GetFieldOfView();
    if (ImGui::DragFloat("FOV", &FOV, 0.1f))
    {
        FOV = std::clamp(FOV, 0.01f, 179.99f);
        Camera.SetFieldOfVew(FOV);
    }

    float NearFar[2] = { Camera.GetNear(), Camera.GetFar() };
    if (ImGui::DragFloat2("Near, Far", NearFar, 0.1f))
    {
        NearFar[0] = FMath::Max(0.01f, NearFar[0]);
        NearFar[1] = FMath::Max(0.01f, NearFar[1]);

        if (NearFar[0] < NearFar[1])
        {
            Camera.SetNear(NearFar[0]);
            Camera.SetFar(NearFar[1]);
        }
        else
        {
            if (abs(NearFar[0] - Camera.GetNear()) < 0.00001f)
            {
                Camera.SetFar(NearFar[0] + 0.01f);
            }
            else if (abs(NearFar[1] - Camera.GetFar()) < 0.00001f)
            {
                Camera.SetNear(NearFar[1] - 0.01f);
            }
        }
    }

    float CameraLocation[] = { Camera.GetTransform().GetPosition().X, Camera.GetTransform().GetPosition().Y, Camera.GetTransform().GetPosition().Z };
    if (ImGui::DragFloat3("Camera Location", CameraLocation, 0.1f))
    {
        Camera.GetTransform().SetPosition(CameraLocation[0], CameraLocation[1], CameraLocation[2]);
    }

    float CameraRotation[] = { Camera.GetTransform().GetRotation().X, Camera.GetTransform().GetRotation().Y, Camera.GetTransform().GetRotation().Z };
    if (ImGui::DragFloat3("Camera Rotation", CameraRotation, 0.1f))
    {
        Camera.GetTransform().SetRotation(CameraRotation[0], CameraRotation[1], CameraRotation[2]);

    }
    ImGui::DragFloat("Camera Speed", &Camera.CameraSpeed, 0.1f);

    FVector Forward = Camera.GetForward();
    FVector Up = Camera.GetUp();
    FVector Right = Camera.GetRight();

    ImGui::Text("Camera Forward: (%.2f %.2f %.2f)", Forward.X, Forward.Y, Forward.Z);
    ImGui::Text("Camera Up: (%.2f %.2f %.2f)", Up.X, Up.Y, Up.Z);
    ImGui::Text("Camera Right: (%.2f %.2f %.2f)", Right.X, Right.Y, Right.Z);
}

void UI::RenderPropertyWindow()
{
    AActor* selectedActor = FEditorManager::Get().GetSelectedActor();

    ImGui::Begin("Properties");
    
    if (selectedActor != nullptr)
    {
        FTransform selectedTransform = selectedActor->GetActorTransform();
        float position[] = { selectedTransform.GetPosition().X, selectedTransform.GetPosition().Y, selectedTransform.GetPosition().Z };
        float rotation[] = { selectedTransform.GetRotation().X, selectedTransform.GetRotation().Y, selectedTransform.GetRotation().Z };
        float scale[] = { selectedTransform.GetScale().X, selectedTransform.GetScale().Y, selectedTransform.GetScale().Z };

        if (ImGui::DragFloat3("Translation", position, 0.1f))
        {
            selectedTransform.SetPosition(position[0], position[1], position[2]);
            selectedActor->SetActorTransform(selectedTransform);
        }
        if (ImGui::DragFloat3("Rotation", rotation, 0.1f))
        {
            selectedTransform.SetRotation(rotation[0], rotation[1], rotation[2]);
            selectedActor->SetActorTransform(selectedTransform);
        }
        if (ImGui::DragFloat3("Scale", scale, 0.1f))
        {
            selectedTransform.SetScale(scale[0], scale[1], scale[2]);
            selectedActor->SetActorTransform(selectedTransform);
        }
    }
    ImGui::End();
}

ImVec2 UI::ResizeToScreen(const ImVec2& vec2) const
{
    return {vec2.x / GetRatio().x, vec2.y / GetRatio().y };
}

ImVec2 UI::GetRatio() const
{
    return {ScreenSize.x / InitialScreenSize.x, ScreenSize.y / InitialScreenSize.y};
}

