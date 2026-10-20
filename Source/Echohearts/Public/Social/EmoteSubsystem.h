// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EmoteSubsystem.generated.h"

// Player emotes: the wheel.
UENUM(BlueprintType)
enum class EPlayerEmote : uint8
{
    Wave     UMETA(DisplayName = "Wave"),
    Bow      UMETA(DisplayName = "Bow"),
    Cheer    UMETA(DisplayName = "Cheer"),
    Sit      UMETA(DisplayName = "Sit"),
    Meditate UMETA(DisplayName = "Meditate"),
    Dance    UMETA(DisplayName = "Dance"),
    Point    UMETA(DisplayName = "Point"),
    Laugh    UMETA(DisplayName = "Laugh"),
    RallyCry UMETA(DisplayName = "Rally Cry")
};

// Co-op quick-chat: emoji-first, no voice needed.
UENUM(BlueprintType)
enum class EQuickChat : uint8
{
    OnMyWay     UMETA(DisplayName = "On my way!"),
    NeedHealing UMETA(DisplayName = "Need healing!"),
    FallBack    UMETA(DisplayName = "Fall back!"),
    Nice        UMETA(DisplayName = "Nice!"),
    FollowMe    UMETA(DisplayName = "Follow me"),
    RestUp      UMETA(DisplayName = "Rest up")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerEmote, EPlayerEmote, Emote, int32, CompanionWatching);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuickChat, EQuickChat, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCompanionBondMoment, int32, EcoKinID, FName, MomentId);

// Emotes: the feeling system. Companion bubbles are auto-read from
// real game state (tired, sick, shaken, joyful) — communication, not
// decoration. Player emotes, pet/praise/comfort moments, and emoji
// quick-chat for co-op.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UEmoteSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Reads the companion's own emoji bubble from their condition:
    // weary, ailing, shaken, sleepy, hungry, joyful, curious, alert.
    UFUNCTION(BlueprintPure, Category = "Echohearts|Emote")
    FText GetCompanionBubble(int32 EcoKinID) const;

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Emote")
    void PlayPlayerEmote(EPlayerEmote Emote);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Emote")
    void SendQuickChat(EQuickChat Message);

    // Small Kindling moments: pet, praise, comfort. The bond subsystem
    // listens on OnCompanionBondMoment.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Emote")
    void PetCompanion(int32 EcoKinID);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Emote")
    void PraiseCompanion(int32 EcoKinID);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Emote")
    void ComfortCompanion(int32 EcoKinID);

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Emote")
    FOnPlayerEmote OnPlayerEmote;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Emote")
    FOnQuickChat OnQuickChat;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Emote")
    FOnCompanionBondMoment OnCompanionBondMoment;
};
