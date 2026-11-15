#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimFoliageInteractionComponent.generated.h"

class UStaticMesh;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FReclaimResultadoGolpeFoliage,
	bool, bDetectado,
	int32, GolpesAcumulados
);

// Se ejecuta en el servidor tras validar un golpe de foliage.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FReclaimGolpeFoliageValidado,
	int32, GolpesAcumulados
);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimFoliageInteractionComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:
	UReclaimFoliageInteractionComponent();

	UFUNCTION(BlueprintCallable, Category="Reclaim|Foliage")
	void SolicitarGolpeFoliage();

	// Resultado enviado al jugador que solicitó el golpe.
	UPROPERTY(BlueprintAssignable, Category="Reclaim|Foliage")
	FReclaimResultadoGolpeFoliage OnResultadoGolpeFoliage;

	// Evento para conectar la entrega de madera en el servidor.
	UPROPERTY(BlueprintAssignable, Category="Reclaim|Foliage")
	FReclaimGolpeFoliageValidado OnGolpeFoliageValidado;

	UPROPERTY(EditAnywhere, Category="Reclaim|Foliage")
	TArray<TObjectPtr<UStaticMesh>> MallasTalables;

private:
	UFUNCTION(Server, Reliable)
	void ServidorRegistrarGolpeFoliage();

	UFUNCTION(Client, Reliable)
	void ClienteRecibirResultadoFoliage(
		bool bDetectado,
		int32 GolpesAcumulados
	);
};

