#pragma once

#include "CoreMinimal.h"
#include "FindSessionsCallbackProxy.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ReclaimSessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnReclaimSessionCreated,
	bool,
	bWasSuccessful
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnReclaimSessionsFound,
	const TArray<FBlueprintSessionResult>&,
	SessionResults,
	bool,
	bWasSuccessful
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnReclaimSessionJoined,
	bool,
	bWasSuccessful
);

UCLASS()
class PRACRICACPP_API UReclaimSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void HostSession(const FString& MapPath, int32 PublicConnections = 4);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void FindSessions(int32 MaxSearchResults = 50);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void JoinSession(const FBlueprintSessionResult& SessionResult);

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer")
	FOnReclaimSessionCreated OnSessionCreated;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer")
	FOnReclaimSessionsFound OnSessionsFound;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer")
	FOnReclaimSessionJoined OnSessionJoined;

private:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(
		FName SessionName,
		EOnJoinSessionCompleteResult::Type Result
	);

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<class FOnlineSessionSearch> LastSessionSearch;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;

	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FDelegateHandle JoinSessionCompleteDelegateHandle;

	FString PendingMapPath;
	int32 PendingPublicConnections = 4;
};

