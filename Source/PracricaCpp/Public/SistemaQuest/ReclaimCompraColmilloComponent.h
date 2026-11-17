#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimCompraColmilloComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FResultadoCompraColmillo,
	bool,
	bExito,
	bool,
	bNivelInsuficiente
);


UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimCompraColmilloComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UReclaimCompraColmilloComponent();

	// El jugador que solicitó el colmillo recibe el resultado.
	UPROPERTY(BlueprintAssignable, Category = "Reclaim|Colmillo")
	FResultadoCompraColmillo AlRecibirResultado;

	UFUNCTION(BlueprintCallable, Category = "Reclaim|Colmillo")
	void SolicitarDesbloqueo();

private:

	UFUNCTION(Server, Reliable)
	void ServidorSolicitarDesbloqueo();

	UFUNCTION(Client, Reliable)
	void ClienteRecibirResultado(
		bool bExito,
		bool bNivelInsuficiente
	);
};

