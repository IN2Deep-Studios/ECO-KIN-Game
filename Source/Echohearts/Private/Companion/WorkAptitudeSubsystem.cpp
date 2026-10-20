// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Companion/WorkAptitudeSubsystem.h"

void UWorkAptitudeSubsystem::RegisterSpeciesProfile(const FSpeciesWorkProfile& Profile)
{
    if (Profile.DexID <= 0)
    {
        return;
    }
    Profiles.Add(Profile.DexID, Profile);
}

int32 UWorkAptitudeSubsystem::GetAptitudeTier(int32 DexID, EWorkAptitude Aptitude) const
{
    const FSpeciesWorkProfile* Profile = Profiles.Find(DexID);
    return Profile ? Profile->GetTier(Aptitude) : 0;
}

TMap<EWorkAptitude, int32> UWorkAptitudeSubsystem::GetSpeciesAptitudes(int32 DexID) const
{
    const FSpeciesWorkProfile* Profile = Profiles.Find(DexID);
    return Profile ? Profile->AptitudeTiers : TMap<EWorkAptitude, int32>();
}

bool UWorkAptitudeSubsystem::DeepenAptitudeViaRite(int32 DexID, EWorkAptitude Aptitude)
{
    FSpeciesWorkProfile* Profile = Profiles.Find(DexID);
    if (!Profile)
    {
        return false;
    }
    int32* Tier = Profile->AptitudeTiers.Find(Aptitude);
    if (!Tier || *Tier >= 5)
    {
        return false;
    }
    ++(*Tier);
    return true;
}

float UWorkAptitudeSubsystem::EfficiencyForTier(int32 Tier)
{
    if (Tier <= 0)
    {
        return 0.6f; // Willing hands, wrong work — still counts for something.
    }
    return 0.6f + 0.2f * static_cast<float>(FMath::Clamp(Tier, 1, 5));
}
