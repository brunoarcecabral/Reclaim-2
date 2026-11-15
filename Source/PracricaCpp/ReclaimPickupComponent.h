#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimPickupComponent.generated.h"

class AActor;

// Se ejecuta en el servidor después de validar la recolección.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FReclaimRecolectarEnServidor,
	AActor*, Objeto
);

// Se ejecuta en el servidor cuando un jugador solicita soltar un slot.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FReclaimSoltarEnServidor,
	int32, IndiceSlot
);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimPickupComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UReclaimPickupComponent();

	UPROPERTY(EditAnywhere, Category="Reclaim|Recoleccion")
	TSubclassOf<AActor> ClaseRecolectable;

	UFUNCTION(BlueprintCallable, Category="Reclaim|Recoleccion")
	void SolicitarRecoleccion(AActor* Objeto);

	UPROPERTY(BlueprintAssignable, Category="Reclaim|Recoleccion")
	FReclaimRecolectarEnServidor OnRecolectarEnServidor;

	// El índice será comprobado contra el inventario del servidor.
	UFUNCTION(BlueprintCallable, Category="Reclaim|Recoleccion")
	void SolicitarSoltarItem(int32 IndiceSlot);

	UPROPERTY(BlueprintAssignable, Category="Reclaim|Recoleccion")
	FReclaimSoltarEnServidor OnSoltarEnServidor;

private:
	UFUNCTION(Server, Reliable)
	void ServidorSolicitarRecoleccion(AActor* Objeto);

	UFUNCTION(Server, Reliable)
	void ServidorSolicitarSoltarItem(int32 IndiceSlot);
};

