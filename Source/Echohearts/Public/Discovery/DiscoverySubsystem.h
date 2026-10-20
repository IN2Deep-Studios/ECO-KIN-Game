// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Discovery/DiscoveryTypes.h"
#include "DiscoverySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHuntClueSolved, FName, HuntId, int32, ClueIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHuntCompleted, FName, HuntId, const FDiscoveryReward&, CacheReward);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnQuizCompleted, FName, TopicId, int32, Score, bool, bPerfect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSecretItemUnlocked, FName, ItemId);

// Owns the Discovery layer: treasure hunts strung across regions, the
// Chronicler's EcoKin quizzes, and secret items. All optional side content —
// nothing here gates main progression. Rewards are granted via delegates so
// the inventory system can hook in; this subsystem never touches currency.
// IMPLEMENTED / NOT COMPILED / NOT TESTED / NOT RUNTIME VERIFIED (2026-10-19).
UCLASS()
class ECHOHEARTS_API UDiscoverySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // ---- Treasure hunts ----
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Discovery")
    void RegisterTreasureHunt(const FTreasureHuntDef& Def);

    UFUNCTION(BlueprintCallable, Category = "Echohearts|Discovery")
    bool BeginHunt(FName HuntId);

    // Solves the next unsolved clue. Returns false if the hunt isn't active
    // or all clues are solved. Completing the final clue completes the hunt
    // and broadcasts the cache reward (one-per-tamer).
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Discovery")
    bool SolveClue(FName HuntId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Discovery")
    bool IsHuntComplete(FName HuntId) const;

    // ---- Quizzes ----
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Discovery")
    void RegisterQuizTopic(const FQuizTopicDef& Def);

    // Submits answers (option indices, one per question, in order). Returns
    // the score; broadcasts completion with the perfect flag. Completion —
    // not perfection — earns the loot.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Discovery")
    int32 SubmitQuizAnswers(FName TopicId, const TArray<int32>& Answers);

    // ---- Secret items ----
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Discovery")
    void RegisterSecretItem(const FSecretItemDef& Def);

    // Unlocks a secret item. One per tamer — duplicates are ignored.
    UFUNCTION(BlueprintCallable, Category = "Echohearts|Discovery")
    bool UnlockSecretItem(FName ItemId);

    UFUNCTION(BlueprintPure, Category = "Echohearts|Discovery")
    bool HasSecretItem(FName ItemId) const;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Discovery")
    FOnHuntClueSolved OnHuntClueSolved;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Discovery")
    FOnHuntCompleted OnHuntCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Discovery")
    FOnQuizCompleted OnQuizCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Echohearts|Discovery")
    FOnSecretItemUnlocked OnSecretItemUnlocked;

private:
    UPROPERTY()
    TMap<FName, FTreasureHuntDef> Hunts;

    UPROPERTY()
    TSet<FName> ActiveHunts;

    UPROPERTY()
    TSet<FName> CompletedHunts;

    UPROPERTY()
    TMap<FName, FQuizTopicDef> QuizTopics;

    UPROPERTY()
    TMap<FName, FSecretItemDef> SecretItems;

    UPROPERTY()
    TSet<FName> UnlockedSecretItems;
};
