#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShopActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UTextRenderComponent;
class UPrimitiveComponent;
class UUserWidget;

UCLASS()
class PRACRICACPP_API AShopActor : public AActor
{
	GENERATED_BODY()

public:
	AShopActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	UStaticMeshComponent* ShopMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	UBoxComponent* InteractionZone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	UTextRenderComponent* StatusText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|UI")
	TSubclassOf<UUserWidget> ShopWidgetClass;
	
	UPROPERTY()
	UUserWidget* ShopWidget;

	UFUNCTION()
	void OnPlayerEnteredShop(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerLeftShop(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	void HandleShopInput();
public:
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void Interact();

private:
	bool bPlayerInRange = false;
	bool bInputBound = false;
};
