#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimFoliageInteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FReclaimResultadoGolpeFoliage,
	bool, bDetectado,
	int32, GolpesAcumulados
);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimFoliageInteractionComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:
	UReclaimFoliageInteractionComponent();

	// Llamar desde el personaje que realiza el golpe.
	UFUNCTION(BlueprintCallable, Category="Reclaim|Foliage")
	void SolicitarGolpeFoliage();

	// Avisa al jugador si el servidor detectó foliage y cuántos golpes lleva.
	UPROPERTY(BlueprintAssignable, Category="Reclaim|Foliage")
	FReclaimResultadoGolpeFoliage OnResultadoGolpeFoliage;

private:
	UFUNCTION(Server, Reliable)
	void ServidorRegistrarGolpeFoliage();

	UFUNCTION(Client, Reliable)
	void ClienteRecibirResultadoFoliage(
		bool bDetectado,
		int32 GolpesAcumulados
	);
};

