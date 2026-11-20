#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReclaimArmaRecogible.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UReclaimArmaComponent;
class UTextRenderComponent;

UCLASS()
class PRACRICACPP_API AReclaimArmaRecogible : public AActor
{
	GENERATED_BODY()

public:
	AReclaimArmaRecogible();
	
	virtual void Tick(float DeltaTime) override;
	
	bool EntregarAlJugador(UReclaimArmaComponent* ArmaJugador);
	bool EstaDisponibleEnMision() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reclaim|Arma")
	TObjectPtr<USceneComponent> Raiz;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reclaim|Arma")
	TObjectPtr<UStaticMeshComponent> ModeloArma;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reclaim|Arma")
	TObjectPtr<UTextRenderComponent> CartelRecoger;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reclaim|Arma")
	FName IdArma;
};

