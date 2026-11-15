#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimNotaJugadorComponent.generated.h"


class AReclaimNotaQuestActor;


UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimNotaJugadorComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UReclaimNotaJugadorComponent();

	// La interfaz llamará a esta función para recordar qué nota está abierta.
	void PrepararNota(AReclaimNotaQuestActor* Nota);

	// La interfaz llamará a esta función cuando el jugador pulse E.
	UFUNCTION()
	void SolicitarContinuarNota();

	// Limpia la referencia cuando se cierra la nota.
	void LimpiarNota();

private:

	UPROPERTY()
	TObjectPtr<AReclaimNotaQuestActor> NotaActual;

	UFUNCTION(Server, Reliable)
	void ServidorContinuarNota(AReclaimNotaQuestActor* Nota);
};

