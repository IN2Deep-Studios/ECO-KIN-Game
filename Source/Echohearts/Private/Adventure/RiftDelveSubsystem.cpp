// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Adventure/RiftDelveSubsystem.h"

int32 URiftDelveSubsystem::StartDelve()
{
    const int32 DelveId = NextDelveId++;
    DelveDepths.Add(DelveId, 0);
    return DelveId;
}

bool URiftDelveSubsystem::DescendLevel(int32 DelveId, float WardStrength)
{
    int32* Depth = DelveDepths.Find(DelveId);
    if (!Depth)
    {
        return false;
    }
    // The dark deepens faster than courage: required ward = depth^2.
    const float Required = static_cast<float>((*Depth + 1) * (*Depth + 1));
    if (WardStrength < Required)
    {
        return false; // The ward gutters. Withdraw or bring more light.
    }
    ++(*Depth);
    OnDelveDepth.Broadcast(DelveId, *Depth);
    return true;
}

void URiftDelveSubsystem::EndDelve(int32 DelveId, bool bWithdrew)
{
    DelveDepths.Remove(DelveId);
}
