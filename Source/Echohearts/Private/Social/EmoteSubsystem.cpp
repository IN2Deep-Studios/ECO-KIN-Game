// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Social/EmoteSubsystem.h"
#include "Companion/CompanionCareSubsystem.h"

FText UEmoteSubsystem::GetCompanionBubble(int32 EcoKinID) const
{
    UGameInstance* GI = GetGameInstance();
    UCompanionCareSubsystem* Care = GI ? GI->GetSubsystem<UCompanionCareSubsystem>() : nullptr;
    if (!Care)
    {
        return FText::FromString(TEXT("😊"));
    }
    const FCompanionCondition Cond = Care->GetCondition(EcoKinID);
    // Every bubble maps to a real state the player can act on.
    if (Cond.HasAilment(ECompanionAilment::Ailing)) return FText::FromString(TEXT("🤒"));
    if (Cond.HasAilment(ECompanionAilment::Shaken)) return FText::FromString(TEXT("😰"));
    if (Cond.HasAilment(ECompanionAilment::Weary))  return FText::FromString(TEXT("😮‍💨"));
    if (Cond.Energy < 25.0f)                        return FText::FromString(TEXT("😴"));
    if (Cond.Energy > 85.0f)                        return FText::FromString(TEXT("😊"));
    return FText::FromString(TEXT("🙂"));
}

void UEmoteSubsystem::PlayPlayerEmote(EPlayerEmote Emote)
{
    OnPlayerEmote.Broadcast(Emote, 0);
}

void UEmoteSubsystem::SendQuickChat(EQuickChat Message)
{
    OnQuickChat.Broadcast(Message);
}

void UEmoteSubsystem::PetCompanion(int32 EcoKinID)
{
    OnCompanionBondMoment.Broadcast(EcoKinID, FName("pet"));
}

void UEmoteSubsystem::PraiseCompanion(int32 EcoKinID)
{
    OnCompanionBondMoment.Broadcast(EcoKinID, FName("praise"));
}

void UEmoteSubsystem::ComfortCompanion(int32 EcoKinID)
{
    OnCompanionBondMoment.Broadcast(EcoKinID, FName("comfort"));
}
