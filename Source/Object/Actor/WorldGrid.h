﻿#pragma once
#include "Actor.h"

class AWorldGrid : public AActor
{
    using Super = AActor;

    float Spacing = 1.f;
    
public:
    AWorldGrid();

    void SetSpacing(float InSpacing);
    float GetSpacing() { return Spacing; }
    
    virtual ~AWorldGrid() = default;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual const char* GetTypeName() override { return "AWorldGrid"; }
};
