// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "VbGameInstance.generated.h"

class UVbLoadingScreenWidget;
class UVbImpactConfig;

UCLASS()
class UVbGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    void CreateSession();
    void LeaveSession();
    void StartSession() const;
    void TryJoinSession(const FOnlineSessionSearchResult& SearchResult);
    void OpenSteamInviteOverlay() const;

    // ==== Accessors ==== //
    FORCEINLINE UVbImpactConfig* GetImpactConfig() const { return ImpactConfig; }
    FORCEINLINE int32 GetMaxPlayers() const { return MaxPlayers; }
    UTexture2D* GetSteamAvatar(FUniqueNetIdRepl UniqueNetId) const;

    FORCEINLINE UDataTable* GetAbilityParamsRow() const { return AbilityParamsRow; }
    FORCEINLINE UDataTable* GetKeyIconRow() const { return KeyIconRow; }
    FORCEINLINE const FRealCurve* GetExperienceCurveTable() const { return ExperienceCurveTable->FindCurve(ExperienceRowName, ""); }
    FORCEINLINE const TSoftObjectPtr<UWorld>& GetLobbyMap() const { return LobbyMap; }

private:
    void OnSessionCreated(FName SessionName, bool bWasSuccessful);
    void OnJoinSession(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId,
        const FOnlineSessionSearchResult& InInviteResult);
    void HandleDestroyedSessionOnInvite(FName SessionName, bool bWasSuccessful);
    void HandleDestroyedSessionOnLeave(FName SessionName, bool bWasSuccessful);
    void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type NetworkFailure, const FString& ErrorString);
    void StartLoadingScreen(const FString& MapName);
    void EndLoadingScreen(UWorld* LoadedWorld);

    bool HasSession() const;
    FOnlineSessionSettings BuildSessionSettings() const;

    // ==== Utils ==== //
    IOnlineSessionPtr GetOnlineSessionPtr() const;

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact", meta=(AllowPrivateAccess="true"))
    UVbImpactConfig* ImpactConfig;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact", meta=(AllowPrivateAccess="true"))
    UDataTable* AbilityParamsRow;

    UPROPERTY(EditDefaultsOnly)
    UDataTable* KeyIconRow;

    UPROPERTY(EditDefaultsOnly, Category = "Session|Settings")
    int32 MaxPlayers = 3;

    UPROPERTY(EditDefaultsOnly, Category="Session|Map")
    TSoftObjectPtr<UWorld> LobbyMap;

    UPROPERTY(EditDefaultsOnly, Category="Session|Map")
    TSoftObjectPtr<UWorld> GameMap;

    UPROPERTY(EditDefaultsOnly, Category="Loading")
    TSubclassOf<UVbLoadingScreenWidget> LoadingScreenWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Level")
    UCurveTable* ExperienceCurveTable;

    UPROPERTY(EditDefaultsOnly, Category = "Level")
    FName ExperienceRowName = "Experience";

    // ==== Runtime ==== //
    FOnlineSessionSearchResult InviteResult;
};