#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StructData.h"
#include "MisClases/TPKent/ComunicacionPuzzle.h"
#include "MisClases/TPKent/InterfaceColores.h"  // <-- Agregado
#include "Kismet/KismetSystemLibrary.h"
#include "TilesDeColores.generated.h"

UCLASS()
class PRACRICACPP_API ATilesDeColores : public AActor, public IComunicacionPuzzle
{
	GENERATED_BODY()
    
public: 
	ATilesDeColores();

protected:
	virtual void BeginPlay() override;

public: 
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StructData")
	FStructData MyData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Puzzle")
	bool PuzzleActivad = false;

	virtual void ComunicarPuzzle_Implementation(bool llave) override;

private:
	FTimerHandle CooldownTimer;
};