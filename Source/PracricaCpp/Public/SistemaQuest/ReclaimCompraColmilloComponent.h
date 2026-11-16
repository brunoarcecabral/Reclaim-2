#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimCompraColmilloComponent.generated.h"


// Se ejecuta en el jugador que solicitó el desbloqueo.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FResultadoCompraColmillo,
	bool,
	bExito
);


UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimCompraColmilloComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UReclaimCompraColmilloComponent();

	// La tienda escuchará este evento para mostrar el resultado.
	UPROPERTY(BlueprintAssignable, Category = "Reclaim|Colmillo")
	FResultadoCompraColmillo AlRecibirResultado;

	// Se llama desde la tienda del jugador local.
	UFUNCTION(BlueprintCallable, Category = "Reclaim|Colmillo")
	void SolicitarDesbloqueo();

private:

	UFUNCTION(Server, Reliable)
	void ServidorSolicitarDesbloqueo();

	UFUNCTION(Client, Reliable)
	void ClienteRecibirResultado(bool bExito);
};

