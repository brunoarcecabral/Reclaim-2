#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimTarjetaComponent.generated.h"

class AReclaimTarjetaRecogible;
class AReclaimPuertaSeguridad;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAlCambiarTarjeta);

UCLASS(Blueprintable, ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimTarjetaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UReclaimTarjetaComponent();

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Reclaim|Tarjeta")
	void MostrarMensajePuerta(const FText& Mensaje);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reclaim|Tarjeta")
	TObjectPtr<UTexture2D> IconoTarjeta;
	
	UFUNCTION(BlueprintPure, Category = "Reclaim|Tarjeta")
	AReclaimPuertaSeguridad* BuscarPuertaCercana() const;

	UFUNCTION(BlueprintCallable, Category = "Reclaim|Tarjeta")
	void SolicitarAbrirPuertaCercana();
	
	UPROPERTY(BlueprintAssignable, Category = "Reclaim|Tarjeta")
	FAlCambiarTarjeta AlCambiarTarjeta;

	UFUNCTION(BlueprintPure, Category = "Reclaim|Tarjeta")
	bool TieneTarjeta() const;
	
	UFUNCTION(BlueprintPure, Category = "Reclaim|Tarjeta")
	AReclaimTarjetaRecogible* BuscarTarjetaCercana() const;
	
	UFUNCTION(BlueprintCallable, Category = "Reclaim|Tarjeta")
	void SolicitarTarjetaCercana();
	
	bool RecibirTarjeta();

	bool ConsumirTarjeta();
	
	UFUNCTION(BlueprintCallable, Category = "Reclaim|Tarjeta")
	void RestaurarTarjetaGuardada(bool bGuardada);

private:
	UPROPERTY(ReplicatedUsing = OnRep_Tarjeta)
	bool bTieneTarjeta = false;
	
	UFUNCTION(Client, Reliable)
	void ClienteRecibirMensajePuerta(const FText& Mensaje);
	
	UFUNCTION(Server, Reliable)
	void ServidorAbrirPuerta(AReclaimPuertaSeguridad* Puerta);
	
	UFUNCTION()
	void OnRep_Tarjeta();
	
	UFUNCTION(Server, Reliable)
	void ServidorRecogerTarjeta(AReclaimTarjetaRecogible* Tarjeta);
	
};

