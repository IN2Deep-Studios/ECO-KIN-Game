// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DialogueTypes.generated.h"

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FDialogueLine
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Dialogue")
    FText SpeakerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Dialogue")
    FText Text;
};

USTRUCT(BlueprintType)
struct ECHOHEARTS_API FDialogueNode
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Dialogue")
    FName NodeId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Dialogue")
    TArray<FDialogueLine> Lines;

    // (Choice text, next node id). Empty next id ends the conversation.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Dialogue")
    TMap<FText, FName> Choices;
};
