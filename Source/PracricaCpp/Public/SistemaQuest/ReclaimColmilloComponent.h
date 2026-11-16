#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimColmilloComponent.generated.h"


class APawn;
class UActorComponent;
class UReclaimQuestComponent;


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

	UPROPERTY(BlueprintAssignable, Category = "Reclaim|Colmillo")
	FAlCambiarColmillo AlCambiarColmillo;

	UFUNCTION(BlueprintPure, Category = "Reclaim|Colmillo")
	bool EstaDisponibleEnTienda() const;

	UFUNCTION(BlueprintPure, Category = "Reclaim|Colmillo")
	bool YaFueComprado() const;

	// Se ejecutará en el servidor después de validar la compra.
	bool EntregarColmilloAlGrupo();

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
	) override;

private:

	UPROPERTY()
	TObjectPtr<UReclaimQuestComponent> ComponenteQuest;

	UPROPERTY(ReplicatedUsing = OnRep_ColmilloComprado)
	bool bColmilloComprado = false;

	UFUNCTION()
	void OnRep_ColmilloComprado();

	UFUNCTION()
	void AlCambiarMisiones();

	UActorComponent* BuscarInventario(APawn* Personaje) const;
};

