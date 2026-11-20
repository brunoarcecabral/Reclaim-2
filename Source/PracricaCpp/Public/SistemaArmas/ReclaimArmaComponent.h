#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimArmaComponent.generated.h"

class AReclaimArmaRecogible;
class UReclaimRanurasWidget;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAlCambiarArma);

UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimArmaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reclaim|Arma")
	TMap<FName, TObjectPtr<UTexture2D>> IconosArmas;

	UFUNCTION(BlueprintPure, Category = "Reclaim|Arma")
	UTexture2D* ObtenerIconoArma() const;
	
	UFUNCTION(BlueprintPure, Category = "Reclaim|Arma")
	AReclaimArmaRecogible* BuscarArmaCercana() const;
	
	UFUNCTION(BlueprintCallable, Category = "Reclaim|Arma")
	void RecogerArmaCercana();
	
	UReclaimArmaComponent();

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;
	
	UPROPERTY(BlueprintAssignable, Category = "Reclaim|Arma")
	FAlCambiarArma AlCambiarArma;

	UFUNCTION(BlueprintPure, Category = "Reclaim|Arma")
	bool TieneArma() const;

	UFUNCTION(BlueprintPure, Category = "Reclaim|Arma")
	bool EstaEquipada() const;
	
	UFUNCTION(BlueprintPure, Category = "Reclaim|Arma")
	float CalcularDanioConArma(float DanioBase) const;
	
	UFUNCTION(BlueprintPure, Category = "Reclaim|Arma")
	FName ObtenerIdArma() const;
	
	UFUNCTION(BlueprintCallable, Category = "Reclaim|Arma")
	void RestaurarArmaGuardada(FName IdGuardado, bool bEstabaEquipada);
	
	bool EntregarArma(FName NuevoIdArma);
	
	UFUNCTION(BlueprintCallable, Category = "Reclaim|Arma")
	void AlternarArma();
	
	UFUNCTION(BlueprintCallable, Category = "Reclaim|Arma")
	void SolicitarRecogerArma(AReclaimArmaRecogible* ArmaRecogible);
	
protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
	) override;

private:
	UPROPERTY(ReplicatedUsing = OnRep_Arma)
	FName IdArma = NAME_None;

	UPROPERTY(ReplicatedUsing = OnRep_Arma)
	bool bArmaEquipada = false;

	UFUNCTION()
	void OnRep_Arma();
	
	UFUNCTION(Server, Reliable)
	void ServidorAlternarArma();
	
	UFUNCTION(Server, Reliable)
	void ServidorRecogerArma(AReclaimArmaRecogible* ArmaRecogible);
	
	UPROPERTY()
	TObjectPtr<UReclaimRanurasWidget> WidgetRanuras;

	FTimerHandle TimerCrearRanuras;

	void IntentarCrearRanuras();
	
};

