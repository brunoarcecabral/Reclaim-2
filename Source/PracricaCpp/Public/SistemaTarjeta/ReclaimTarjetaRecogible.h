#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReclaimTarjetaRecogible.generated.h"


class USceneComponent;
class UStaticMeshComponent;
class UReclaimTarjetaComponent;
class UTextRenderComponent;
class UReclaimQuestData;

UCLASS()
class PRACRICACPP_API AReclaimTarjetaRecogible : public AActor
{
	GENERATED_BODY()

public:
	AReclaimTarjetaRecogible();
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;
	
	UFUNCTION(BlueprintCallable, Category = "Reclaim|Tarjeta")
	void RestaurarRecogida(bool bGuardada);
	
	UFUNCTION(BlueprintPure, Category = "Reclaim|Tarjeta")
	bool EstaRecogida() const;
	
	bool EstaDisponibleParaRecoger() const;
	
	bool EntregarAlJugador(UReclaimTarjetaComponent* TarjetaJugador);

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reclaim|Tarjeta")
	TObjectPtr<UReclaimQuestData> MisionAbrirPuerta;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reclaim|Tarjeta")
	TObjectPtr<USceneComponent> Raiz;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reclaim|Tarjeta")
	TObjectPtr<UStaticMeshComponent> ModeloTarjeta;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reclaim|Tarjeta")
	TObjectPtr<UTextRenderComponent> CartelRecoger;

private:
	UPROPERTY(ReplicatedUsing = OnRep_Recogida)
	bool bRecogida = false;

	UFUNCTION()
	void OnRep_Recogida();

	void ActualizarApariencia();
};

