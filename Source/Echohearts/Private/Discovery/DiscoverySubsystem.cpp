// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Discovery/DiscoverySubsystem.h"

void UDiscoverySubsystem::RegisterTreasureHunt(const FTreasureHuntDef& Def)
{
    if (Def.HuntId.IsNone() || Def.Clues.IsEmpty())
    {
        return;
    }
    Hunts.Add(Def.HuntId, Def);
}

bool UDiscoverySubsystem::BeginHunt(FName HuntId)
{
    if (!Hunts.Contains(HuntId) || ActiveHunts.Contains(HuntId) || CompletedHunts.Contains(HuntId))
    {
        return false;
    }
    ActiveHunts.Add(HuntId);
    return true;
}

bool UDiscoverySubsystem::SolveClue(FName HuntId)
{
    if (!ActiveHunts.Contains(HuntId))
    {
        return false;
    }
    FTreasureHuntDef* Def = Hunts.Find(HuntId);
    if (!Def)
    {
        return false;
    }

    for (int32 i = 0; i < Def->Clues.Num(); ++i)
    {
        if (!Def->Clues[i].bSolved)
        {
            Def->Clues[i].bSolved = true;
            OnHuntClueSolved.Broadcast(HuntId, i);

            if (i == Def->Clues.Num() - 1)
            {
                ActiveHunts.Remove(HuntId);
                CompletedHunts.Add(HuntId);
                OnHuntCompleted.Broadcast(HuntId, Def->CacheReward);
                if (!Def->CacheReward.SecretItemId.IsNone())
                {
                    UnlockSecretItem(Def->CacheReward.SecretItemId);
                }
            }
            return true;
        }
    }
    return false;
}

bool UDiscoverySubsystem::IsHuntComplete(FName HuntId) const
{
    return CompletedHunts.Contains(HuntId);
}

void UDiscoverySubsystem::RegisterQuizTopic(const FQuizTopicDef& Def)
{
    if (Def.TopicId.IsNone() || Def.Questions.IsEmpty())
    {
        return;
    }
    QuizTopics.Add(Def.TopicId, Def);
}

int32 UDiscoverySubsystem::SubmitQuizAnswers(FName TopicId, const TArray<int32>& Answers)
{
    const FQuizTopicDef* Def = QuizTopics.Find(TopicId);
    if (!Def || Answers.Num() != Def->Questions.Num())
    {
        return INDEX_NONE;
    }

    int32 Score = 0;
    for (int32 i = 0; i < Def->Questions.Num(); ++i)
    {
        if (Answers[i] == Def->Questions[i].CorrectOptionIndex)
        {
            ++Score;
        }
    }

    // Completion — not perfection — earns the loot. The Chronicler explains
    // every answer either way; that's the point.
    const bool bPerfect = (Score == Def->Questions.Num());
    OnQuizCompleted.Broadcast(TopicId, Score, bPerfect);
    return Score;
}

void UDiscoverySubsystem::RegisterSecretItem(const FSecretItemDef& Def)
{
    if (Def.ItemId.IsNone())
    {
        return;
    }
    SecretItems.Add(Def.ItemId, Def);
}

bool UDiscoverySubsystem::UnlockSecretItem(FName ItemId)
{
    if (!SecretItems.Contains(ItemId) || UnlockedSecretItems.Contains(ItemId))
    {
        return false;
    }
    UnlockedSecretItems.Add(ItemId);
    OnSecretItemUnlocked.Broadcast(ItemId);
    return true;
}

bool UDiscoverySubsystem::HasSecretItem(FName ItemId) const
{
    return UnlockedSecretItems.Contains(ItemId);
}
