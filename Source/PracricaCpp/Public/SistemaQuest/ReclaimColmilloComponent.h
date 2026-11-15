#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimColmilloComponent.generated.h"


class UReclaimQuestComponent;


// Avisa a la tienda cuando debe revisar la disponibilidad.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAlCambiarColmillo);


UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimColmilloComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UReclaimColmilloComponent();

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	// El widget de la tienda podrá escuchar este evento.
	UPROPERTY(BlueprintAssignable, Category = "Reclaim|Colmillo")
	FAlCambiarColmillo AlCambiarColmillo;

	// Verdadero cuando el objeto debe ofrecerse en la tienda.
	UFUNCTION(BlueprintPure, Category = "Reclaim|Colmillo")
	bool EstaDisponibleEnTienda() const;

	// Indica si ya se realizó la compra compartida.
	UFUNCTION(BlueprintPure, Category = "Reclaim|Colmillo")
	bool YaFueComprado() const;

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
	) override;

private:

	UPROPERTY()
	TObjectPtr<UReclaimQuestComponent> ComponenteQuest;

	// Más adelante la compra en el servidor cambiará este estado.
	UPROPERTY(ReplicatedUsing = OnRep_ColmilloComprado)
	bool bColmilloComprado = false;

	UFUNCTION()
	void OnRep_ColmilloComprado();

	UFUNCTION()
	void AlCambiarMisiones();
};

