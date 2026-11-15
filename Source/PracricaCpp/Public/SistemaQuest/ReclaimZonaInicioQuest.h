#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReclaimZonaInicioQuest.generated.h"


class UBoxComponent;
class UReclaimQuestData;


UCLASS()
class PRACRICACPP_API AReclaimZonaInicioQuest : public AActor
{
	GENERATED_BODY()

public:

	AReclaimZonaInicioQuest();

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
	) override;

	// Área donde debe reunirse el grupo.
	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Quest"
	)
	TObjectPtr<UBoxComponent> Zona;

	// Bloquea el paso hasta que se inicie la misión.
	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Quest"
	)
	TObjectPtr<UBoxComponent> Barrera;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Quest"
	)
	TObjectPtr<UReclaimQuestData> MisionAIniciar;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Quest",
		meta = (ClampMin = "0.1", UIMin = "0.1")
	)
	float IntervaloRevision = 0.25f;

private:

	FTimerHandle TimerRevision;

	// Se replica para abrir la barrera también en los clientes.
	UPROPERTY(ReplicatedUsing = OnRep_EventoActivado)
	bool bEventoActivado = false;

	UFUNCTION()
	void OnRep_EventoActivado();

	void ActualizarBarrera();

	void RevisarJugadores();

	bool EstanTodosEnLaZona() const;
};

