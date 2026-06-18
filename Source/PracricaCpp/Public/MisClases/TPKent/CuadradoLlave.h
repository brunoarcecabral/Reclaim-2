#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StructData.h"
#include "ComunicacionPuzzle.h"
#include "Kismet/GameplayStatics.h"  // <-- Agregado
#include "CuadradoLlave.generated.h"

UCLASS()
class PRACRICACPP_API ACuadradoLlave : public AActor, public IComunicacionPuzzle
{
	GENERATED_BODY()
    
public: 
	ACuadradoLlave();

protected:
	virtual void BeginPlay() override;

public: 
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StructData")
	FStructData MyData;

	virtual void ComunicarPuzzle_Implementation(bool llave) override;
};