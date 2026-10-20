// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Audio/EchoheartsAudioManager.h"

void UEchoheartsAudioManager::SetMusicMood(EMusicMood NewMood)
{
    if (NewMood == CurrentMood)
    {
        return;
    }
    CurrentMood = NewMood;
    OnMusicMoodChanged.Broadcast(NewMood);
}

void UEchoheartsAudioManager::PlayEssenceStinger(EEcoKinEssence Essence)
{
    // Asset mapping lives in Blueprint/data tables keyed by essence. The
    // manager only routes the request; silence here means unmapped, not broken.
    (void)Essence;
}

void UEchoheartsAudioManager::PlayUIConfirm()
{
}

void UEchoheartsAudioManager::PlayUICancel()
{
}

void UEchoheartsAudioManager::PlayBindingChime()
{
}

void UEchoheartsAudioManager::PlayRiteChorus()
{
}
