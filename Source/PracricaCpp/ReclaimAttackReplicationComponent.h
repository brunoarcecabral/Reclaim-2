#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimAttackReplicationComponent.generated.h"

class UAnimMontage;

// Se dispara en el servidor cuando llega una solicitud de ataque.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReclaimAtaqueEnServidor);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimAttackReplicationComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:
	UReclaimAttackReplicationComponent();

	// Comparte con los demás la animación iniciada localmente.
	UFUNCTION(BlueprintCallable, Category="Reclaim|Ataque")
	void CompartirAnimacionAtaque(UAnimMontage* Montaje);

	// Solicita al servidor procesar el golpe.
	UFUNCTION(BlueprintCallable, Category="Reclaim|Ataque")
	void SolicitarAtaqueAlServidor();

	// Se ejecuta únicamente en el servidor.
	UPROPERTY(BlueprintAssignable, Category="Reclaim|Ataque")
	FReclaimAtaqueEnServidor OnAtaqueEnServidor;

private:
	UFUNCTION(Server, Reliable)
	void ServidorCompartirAtaque(UAnimMontage* Montaje);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastReproducirAtaque(UAnimMontage* Montaje);

	UFUNCTION(Server, Reliable)
	void ServidorProcesarAtaque();
};

