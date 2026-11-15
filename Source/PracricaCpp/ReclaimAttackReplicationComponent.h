#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimAttackReplicationComponent.generated.h"

class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReclaimAtaqueEnServidor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReclaimAvisoMadera);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimAttackReplicationComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:
	UReclaimAttackReplicationComponent();

	UFUNCTION(BlueprintCallable, Category="Reclaim|Ataque")
	void CompartirAnimacionAtaque(UAnimMontage* Montaje);

	UFUNCTION(BlueprintCallable, Category="Reclaim|Ataque")
	void SolicitarAtaqueAlServidor();

	// El servidor llama a esta función para avisar al dueño del personaje.
	UFUNCTION(
		BlueprintCallable,
		BlueprintAuthorityOnly,
		Category="Reclaim|Ataque"
	)
	void AvisarMaderaAlJugador();

	UPROPERTY(BlueprintAssignable, Category="Reclaim|Ataque")
	FReclaimAtaqueEnServidor OnAtaqueEnServidor;

	// Este evento se dispara en la pantalla del jugador dueño.
	UPROPERTY(BlueprintAssignable, Category="Reclaim|Ataque")
	FReclaimAvisoMadera OnAvisoMadera;

private:
	UFUNCTION(Server, Reliable)
	void ServidorCompartirAtaque(UAnimMontage* Montaje);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastReproducirAtaque(UAnimMontage* Montaje);

	UFUNCTION(Server, Reliable)
	void ServidorProcesarAtaque();

	UFUNCTION(Client, Reliable)
	void ClienteMostrarAvisoMadera();
};

