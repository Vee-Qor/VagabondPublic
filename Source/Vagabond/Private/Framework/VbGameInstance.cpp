// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Framework/VbGameInstance.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "MoviePlayer.h"
#include "Menu/UI/VbLoadingScreenWidget.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

#include "steam/steam_api.h"
#include "steam/isteamutils.h"
#include "steam/isteamfriends.h"

DEFINE_LOG_CATEGORY_STATIC(LogVbGameInstance, All, All);

void UVbGameInstance::Init()
{
    Super::Init();

#if !WITH_EDITOR
    FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UVbGameInstance::StartLoadingScreen);
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UVbGameInstance::EndLoadingScreen);

    if (GEngine)
    {
        GEngine->OnNetworkFailure().AddUObject(this, &UVbGameInstance::HandleNetworkFailure);
    }

    if (const IOnlineSessionPtr SessionPtr = GetOnlineSessionPtr())
    {
        SessionPtr->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &UVbGameInstance::OnSessionUserInviteAccepted);
    }

    CreateSession();
#endif
}

void UVbGameInstance::CreateSession()
{
    const IOnlineSessionPtr SessionPtr = GetOnlineSessionPtr();
    if (!SessionPtr) return;

    if (HasSession())
    {
        UE_LOG(LogVbGameInstance, Warning, TEXT("Session Already Created"));
        return;
    }

    SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
    SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UVbGameInstance::OnSessionCreated);

    const FOnlineSessionSettings SessionSettings = BuildSessionSettings();
    SessionPtr->CreateSession(0, NAME_GameSession, SessionSettings);
}

void UVbGameInstance::LeaveSession()
{
    const IOnlineSessionPtr SessionPtr = GetOnlineSessionPtr();
    if (!SessionPtr) return;

    if (HasSession())
    {
        UE_LOG(LogVbGameInstance, Warning, TEXT("Leave Session"));
        UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), LobbyMap, true);
        SessionPtr->OnDestroySessionCompleteDelegates.RemoveAll(this);
        SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UVbGameInstance::HandleDestroyedSessionOnLeave);
        SessionPtr->DestroySession(NAME_GameSession);
    }
}

void UVbGameInstance::StartSession() const
{
    UWorld* World = GetWorld();
    if (!World) return;

    World->ServerTravel(GameMap.GetLongPackageName() + "?listen", true);
}

void UVbGameInstance::TryJoinSession(const FOnlineSessionSearchResult& SearchResult)
{
    const IOnlineSessionPtr SessionPtr = GetOnlineSessionPtr();
    if (!SessionPtr || !SearchResult.IsValid()) return;

    SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
    SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UVbGameInstance::OnJoinSession);
    SessionPtr->JoinSession(0, NAME_GameSession, SearchResult);
}

void UVbGameInstance::OpenSteamInviteOverlay() const
{
    const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
    if (!Subsystem) return;

    const IOnlineExternalUIPtr ExternalUI = Subsystem->GetExternalUIInterface();
    if (!ExternalUI.IsValid()) return;

    ExternalUI->ShowInviteUI(0, NAME_GameSession);
}

void UVbGameInstance::OnSessionCreated(FName SessionName, bool bWasSuccessful)
{
    if (const IOnlineSessionPtr SessionPtr = GetOnlineSessionPtr())
    {
        SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
    }

    UE_LOG(LogVbGameInstance, Warning, TEXT("Session: %s try create | Result: %s"), *SessionName.ToString(), bWasSuccessful ? TEXT("Success") : TEXT("Failed"));

    if (!bWasSuccessful) return;

    UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), LobbyMap, true, "listen");
}

void UVbGameInstance::OnJoinSession(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    UE_LOG(LogVbGameInstance, Warning, TEXT("Try to join session: %s | Result: %s"), *SessionName.ToString(),
        Result == EOnJoinSessionCompleteResult::Success ? TEXT("Success") : TEXT("Failed"));

    if (IOnlineSessionPtr SessionPtr = GetOnlineSessionPtr())
    {
        SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
    }

    if (Result != EOnJoinSessionCompleteResult::Success) return;

    const IOnlineSessionPtr SessionPtr = GetOnlineSessionPtr();
    if (!SessionPtr) return;

    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if (!PlayerController) return;

    FString ConnectionInfo;
    SessionPtr->GetResolvedConnectString(NAME_GameSession, ConnectionInfo);

    UE_LOG(LogVbGameInstance, Warning, TEXT("Client Travel"));
    PlayerController->ClientTravel(ConnectionInfo, TRAVEL_Absolute);
}

void UVbGameInstance::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId,
    const FOnlineSessionSearchResult& InInviteResult)
{
    UE_LOG(LogVbGameInstance, Warning, TEXT("Try accepted Invite | Result: %s"), bWasSuccessful ? TEXT("Success") : TEXT("Failed"));

    if (!bWasSuccessful) return;

    InviteResult = InInviteResult;

    const IOnlineSessionPtr SessionPtr = GetOnlineSessionPtr();
    if (!SessionPtr) return;

    if (HasSession())
    {
        SessionPtr->OnDestroySessionCompleteDelegates.RemoveAll(this);
        SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UVbGameInstance::HandleDestroyedSessionOnInvite);
        SessionPtr->DestroySession(NAME_GameSession);
    }
    else
    {
        TryJoinSession(InInviteResult);
    }
}

void UVbGameInstance::HandleDestroyedSessionOnInvite(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogVbGameInstance, Warning, TEXT("Session: %s try destroyed. Result: %s"), *SessionName.ToString(),
        bWasSuccessful ? TEXT("Success") : TEXT("Failed"));

    if (const IOnlineSessionPtr SessionPtr = GetOnlineSessionPtr())
    {
        SessionPtr->OnDestroySessionCompleteDelegates.RemoveAll(this);
    }

    if (!bWasSuccessful) return;
    TryJoinSession(InviteResult);;
}

void UVbGameInstance::HandleDestroyedSessionOnLeave(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogVbGameInstance, Warning, TEXT("Session: %s try destroyed. Result: %s"), *SessionName.ToString(),
        bWasSuccessful ? TEXT("Success") : TEXT("Failed"));

    if (const IOnlineSessionPtr SessionPtr = GetOnlineSessionPtr())
    {
        SessionPtr->OnDestroySessionCompleteDelegates.RemoveAll(this);
    }

    if (!bWasSuccessful) return;
    CreateSession();
}

void UVbGameInstance::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type NetworkFailure, const FString& ErrorString)
{
    if (NetworkFailure == ENetworkFailure::ConnectionLost || NetworkFailure == ENetworkFailure::FailureReceived)
    {
        UE_LOG(LogVbGameInstance, Warning, TEXT("Network Failure: %s. Cleaning up session..."), *ErrorString);

        LeaveSession();
    }
}

void UVbGameInstance::StartLoadingScreen(const FString& MapName)
{
    if (!LoadingScreenWidgetClass) return;

    UVbLoadingScreenWidget* LoadingScreen = CreateWidget<UVbLoadingScreenWidget>(this, LoadingScreenWidgetClass);
    if (!LoadingScreen) return;

    FLoadingScreenAttributes LoadingScreenAttributes;
    LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = true;
    LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = 1.0f;
    LoadingScreenAttributes.WidgetLoadingScreen = LoadingScreen->TakeWidget();

    IGameMoviePlayer* GameMoviePlayer = GetMoviePlayer();
    if (!GameMoviePlayer) return;

    GameMoviePlayer->SetupLoadingScreen(LoadingScreenAttributes);
}

void UVbGameInstance::EndLoadingScreen(UWorld* LoadedWorld)
{
    IGameMoviePlayer* GameMoviePlayer = GetMoviePlayer();
    if (!GameMoviePlayer) return;

    GameMoviePlayer->StopMovie();
}

bool UVbGameInstance::HasSession() const
{
    const IOnlineSessionPtr SessionPtr = GetOnlineSessionPtr();
    if (!SessionPtr) return false;

    return SessionPtr->GetNamedSession(NAME_GameSession) != nullptr;
}

FOnlineSessionSettings UVbGameInstance::BuildSessionSettings() const
{
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bAllowInvites = true;
    SessionSettings.bIsLANMatch = false;
    SessionSettings.bAllowJoinViaPresence = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bUseLobbiesIfAvailable = true;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.NumPublicConnections = MaxPlayers;
    SessionSettings.Set(FName("VAGABOND_KEY"), true, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    return SessionSettings;
}

IOnlineSessionPtr UVbGameInstance::GetOnlineSessionPtr() const
{
    const IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());

    return OnlineSubsystem ? OnlineSubsystem->GetSessionInterface() : nullptr;
}

UTexture2D* UVbGameInstance::GetSteamAvatar(FUniqueNetIdRepl UniqueNetId) const
{
    if (!UniqueNetId.IsValid()) return nullptr;

    if (!SteamAPI_IsSteamRunning() || !SteamFriends() || !SteamUtils()) return nullptr;

    uint64 SteamID = *(uint64*)UniqueNetId->GetBytes();
    const CSteamID TargetSteamID(SteamID);

    const int PictureHandle = SteamFriends()->GetMediumFriendAvatar(TargetSteamID);
    if (PictureHandle <= 0) return nullptr;

    uint32 Width, Height;
    if (SteamUtils()->GetImageSize(PictureHandle, &Width, &Height))
    {
        TArray<uint8> Buffer;
        Buffer.SetNumUninitialized(Width * Height * 4);

        if (SteamUtils()->GetImageRGBA(PictureHandle, Buffer.GetData(), Buffer.Num()))
        {
            UTexture2D* AvatarTex = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);
            if (!AvatarTex) return nullptr;

            FTexture2DMipMap& Mip = AvatarTex->GetPlatformData()->Mips[0];
            void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
            FMemory::Memcpy(Data, Buffer.GetData(), Buffer.Num());
            Mip.BulkData.Unlock();

            AvatarTex->UpdateResource();
            return AvatarTex;
        }
    }
    return nullptr;
}