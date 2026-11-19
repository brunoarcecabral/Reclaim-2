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
	
	void PrepararNota(AReclaimNotaQuestActor* Nota);
	
	UFUNCTION()
	void SolicitarContinuarNota();
	
	void LimpiarNota();

private:

	UPROPERTY()
	TObjectPtr<AReclaimNotaQuestActor> NotaActual;

	UFUNCTION(Server, Reliable)
	void ServidorContinuarNota(AReclaimNotaQuestActor* Nota);
};

