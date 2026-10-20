// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Dialogue/NPCCharacter.h"

ANPCCharacter::ANPCCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ANPCCharacter::BeginPlay()
{
    Super::BeginPlay();
}

bool ANPCCharacter::BeginDialogue(FDialogueNode& OutNode)
{
    const FDialogueNode* Found = DialogueTree.Find(EntryNodeId);
    if (!Found)
    {
        return false;
    }
    OutNode = *Found;
    return true;
}

bool ANPCCharacter::ChooseDialogueOption(const FDialogueNode& CurrentNode, int32 ChoiceIndex, FDialogueNode& OutNode)
{
    TArray<FName> NextIds;
    CurrentNode.Choices.GenerateValueArray(NextIds);
    if (!NextIds.IsValidIndex(ChoiceIndex))
    {
        return false;
    }
    const FName NextId = NextIds[ChoiceIndex];
    if (NextId.IsNone())
    {
        return false; // conversation ends
    }
    const FDialogueNode* Found = DialogueTree.Find(NextId);
    if (!Found)
    {
        return false;
    }
    OutNode = *Found;
    return true;
}
