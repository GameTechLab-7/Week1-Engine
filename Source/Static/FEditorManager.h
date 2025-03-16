﻿#pragma once
#include "Object/Actor/Actor.h"
#include "Data/FEngineShowFlags.h"

class AGizmoHandle;
class AWorldGrid;

class FEditorManager : public TSingleton<FEditorManager>
{
public:
    
    inline AActor* GetSelectedActor() const {return SelectedActor;}
    
    void SelectActor(AActor* NewActor);

    inline ACamera* GetCamera() const {return Camera;}

    void SetCamera(ACamera* NewCamera);

    AGizmoHandle* GetGizmoHandle() const {return GizmoHandle;}

    void SetWorldGrid(AWorldGrid* NewWorldGrid) { WorldGrid = NewWorldGrid; };

    AWorldGrid* GetWorldGrid() const {return WorldGrid;};
    
    bool IsShowFlagSet(EEngineShowFlags Flag) const { return EngineShowFlags.IsSet(Flag);}
    void SetShowFlag(EEngineShowFlags Flag, bool bEnabled) { EngineShowFlags.Set(Flag, bEnabled);}
    void ToggleShowFlag(EEngineShowFlags Flag) { EngineShowFlags.Toggle(Flag); }
    void PrintShowFlags() const { EngineShowFlags.Print(); }
    
private:
    ACamera* Camera = nullptr;
    AActor* SelectedActor = nullptr;
    AGizmoHandle* GizmoHandle = nullptr;
    AWorldGrid* WorldGrid = nullptr;
    FEngineShowFlags EngineShowFlags;
};
