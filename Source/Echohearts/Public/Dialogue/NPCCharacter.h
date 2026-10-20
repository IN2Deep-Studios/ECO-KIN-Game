// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Dialogue/DialogueTypes.h"
#include "NPCCharacter.generated.h"

// A person of Rebearth: settler, researcher, keeper of a sanctuary. NPCs talk,
// trade, and give missions — they never fight. Dialogue is data-driven so
// writers work in nodes, not code.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-15).
UCLASS()
class ECHOHEARTS_API ANPCCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ANPCCharacter();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|NPC")
    FText DisplayName;

    // Full conversation tree, keyed by node id.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Dialogue")
    TMap<FName, FDialogueNode> DialogueTree;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echohearts|Dialogue")
    FName EntryNodeId = FName(TEXT("Greeting"));

    // Begins a conversation; returns the entry node (false when none).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Dialogue")
    bool BeginDialogue(FDialogueNode& OutNode);

    // Follows a player choice; returns the next node (false = conversation ends).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Dialogue")
    bool ChooseDialogueOption(const FDialogueNode& CurrentNode, int32 ChoiceIndex, FDialogueNode& OutNode);

protected:
    virtual void BeginPlay() override;
};
