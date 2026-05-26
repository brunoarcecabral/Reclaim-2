  #pragma once

#include "CoreMinimal.h"
#include "Itemdrop/ItemDrop.h"
#include "GameFramework/Actor.h"


UCLASS()
class PRACRICACPP_API AMyItemDroop : public AActor
{
	GENERATED_BODY()

public:
	AMyItemDroop();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	/** Clase del actor a spawnear cuando este actor se destruye */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Droop")
	TSubclassOf<AActor> ItemToSpawn;

	/** Offset desde la ubicación del actor para spawnear el item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Droop")
	FVector SpawnOffset;
};
