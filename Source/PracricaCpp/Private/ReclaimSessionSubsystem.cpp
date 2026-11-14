#include "ReclaimSessionSubsystem.h"

#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"

void UReclaimSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CreateSessionCompleteDelegate =
		FOnCreateSessionCompleteDelegate::CreateUObject(
			this,
			&UReclaimSessionSubsystem::OnCreateSessionComplete
		);

	FindSessionsCompleteDelegate =
		FOnFindSessionsCompleteDelegate::CreateUObject(
			this,
			&UReclaimSessionSubsystem::OnFindSessionsComplete
		);

	JoinSessionCompleteDelegate =
		FOnJoinSessionCompleteDelegate::CreateUObject(
			this,
			&UReclaimSessionSubsystem::OnJoinSessionComplete
		);

	if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = OnlineSubsystem->GetSessionInterface();
	}
}

void UReclaimSessionSubsystem::Deinitialize()
{
	if (SessionInterface.IsValid())
	{
		if (CreateSessionCompleteDelegateHandle.IsValid())
		{
			SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(
				CreateSessionCompleteDelegateHandle
			);
		}

		if (FindSessionsCompleteDelegateHandle.IsValid())
		{
			SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(
				FindSessionsCompleteDelegateHandle
			);
		}

		if (JoinSessionCompleteDelegateHandle.IsValid())
		{
			SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(
				JoinSessionCompleteDelegateHandle
			);
		}
	}

	SessionInterface.Reset();

	Super::Deinitialize();
}

void UReclaimSessionSubsystem::HostSession(
	const FString& MapPath,
	int32 PublicConnections
)
{
	if (!SessionInterface.IsValid())
	{
		OnSessionCreated.Broadcast(false);
		return;
	}

	if (SessionInterface->GetNamedSession(NAME_GameSession) != nullptr)
	{
		OnSessionCreated.Broadcast(false);
		return;
	}

	const ULocalPlayer* LocalPlayer =
		GetWorld()->GetFirstLocalPlayerFromController();

	if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
	{
		OnSessionCreated.Broadcast(false);
		return;
	}

	PendingMapPath = MapPath;
	PendingPublicConnections = FMath::Clamp(PublicConnections, 1, 4);

	LastSessionSettings = MakeShared<FOnlineSessionSettings>();

	LastSessionSettings->bIsLANMatch = false;
	LastSessionSettings->NumPublicConnections = PendingPublicConnections;
	LastSessionSettings->NumPrivateConnections = 0;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowInvites = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bUseLobbiesIfAvailable = true;

	LastSessionSettings->Set(
		FName("SessionName"),
		FString("ReclaimViejo"),
		EOnlineDataAdvertisementType::ViaOnlineServiceAndPing
	);

	CreateSessionCompleteDelegateHandle =
		SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
			CreateSessionCompleteDelegate
		);

	const bool bStarted = SessionInterface->CreateSession(
		*LocalPlayer->GetPreferredUniqueNetId(),
		NAME_GameSession,
		*LastSessionSettings
	);

	if (!bStarted)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(
			CreateSessionCompleteDelegateHandle
		);

		OnSessionCreated.Broadcast(false);
	}
}

void UReclaimSessionSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid())
	{
		OnSessionsFound.Broadcast(TArray<FBlueprintSessionResult>(), false);
		return;
	}

	const ULocalPlayer* LocalPlayer =
		GetWorld()->GetFirstLocalPlayerFromController();

	if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
	{
		OnSessionsFound.Broadcast(TArray<FBlueprintSessionResult>(), false);
		return;
	}

	LastSessionSearch = MakeShared<FOnlineSessionSearch>();

	LastSessionSearch->MaxSearchResults = FMath::Max(1, MaxSearchResults);
	LastSessionSearch->bIsLanQuery = false;

	LastSessionSearch->QuerySettings.Set(
	SEARCH_LOBBIES,
	true,
	EOnlineComparisonOp::Equals
);


	FindSessionsCompleteDelegateHandle =
		SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
			FindSessionsCompleteDelegate
		);

	const bool bStarted = SessionInterface->FindSessions(
		*LocalPlayer->GetPreferredUniqueNetId(),
		LastSessionSearch.ToSharedRef()
	);

	if (!bStarted)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(
			FindSessionsCompleteDelegateHandle
		);

		OnSessionsFound.Broadcast(TArray<FBlueprintSessionResult>(), false);
	}
}

void UReclaimSessionSubsystem::JoinSession(
	const FBlueprintSessionResult& SessionResult
)
{
	if (!SessionInterface.IsValid())
	{
		OnSessionJoined.Broadcast(false);
		return;
	}

	const ULocalPlayer* LocalPlayer =
		GetWorld()->GetFirstLocalPlayerFromController();

	if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
	{
		OnSessionJoined.Broadcast(false);
		return;
	}

	JoinSessionCompleteDelegateHandle =
		SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
			JoinSessionCompleteDelegate
		);

	const bool bStarted = SessionInterface->JoinSession(
		*LocalPlayer->GetPreferredUniqueNetId(),
		NAME_GameSession,
		SessionResult.OnlineResult
	);

	if (!bStarted)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(
			JoinSessionCompleteDelegateHandle
		);

		OnSessionJoined.Broadcast(false);
	}
}

void UReclaimSessionSubsystem::OnCreateSessionComplete(
	FName SessionName,
	bool bWasSuccessful
)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(
			CreateSessionCompleteDelegateHandle
		);
	}

	OnSessionCreated.Broadcast(bWasSuccessful);

	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(
			GetWorld(),
			FName(*PendingMapPath),
			true,
			"listen"
		);
	}
}

void UReclaimSessionSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(
			FindSessionsCompleteDelegateHandle
		);
	}

	TArray<FBlueprintSessionResult> BlueprintResults;

	if (bWasSuccessful && LastSessionSearch.IsValid())
	{
		for (const FOnlineSessionSearchResult& Result :
			LastSessionSearch->SearchResults)
		{
			FBlueprintSessionResult BlueprintResult;
			BlueprintResult.OnlineResult = Result;

			BlueprintResults.Add(BlueprintResult);
		}
	}

	OnSessionsFound.Broadcast(BlueprintResults, bWasSuccessful);
}

void UReclaimSessionSubsystem::OnJoinSessionComplete(
	FName SessionName,
	EOnJoinSessionCompleteResult::Type Result
)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(
			JoinSessionCompleteDelegateHandle
		);
	}

	bool bWasSuccessful =
		Result == EOnJoinSessionCompleteResult::Success;

	if (bWasSuccessful)
	{
		FString ConnectString;

		if (SessionInterface->GetResolvedConnectString(
			NAME_GameSession,
			ConnectString
		))
		{
			if (APlayerController* PlayerController =
				GetWorld()->GetFirstPlayerController())
			{
				PlayerController->ClientTravel(
					ConnectString,
					ETravelType::TRAVEL_Absolute
				);
			}
		}
		else
		{
			bWasSuccessful = false;
		}
	}

	OnSessionJoined.Broadcast(bWasSuccessful);
}

