#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReclaimZonaInicioQuest.generated.h"


class APlayerController;
class UBoxComponent;
class UReclaimAvisoQuestWidget;
class UReclaimQuestData;


UCLASS()
class PRACRICACPP_API AReclaimZonaInicioQuest : public AActor
{
	GENERATED_BODY()

public:

	AReclaimZonaInicioQuest();
	
	UPROPERTY(EditInstanceOnly, Category = "Reclaim|Guardado")
	FName IdGuardado = NAME_None;

	bool EstaActivadaParaGuardado() const;
	bool RestaurarActivada();
	
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
	) override;
	
	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Quest"
	)
	TObjectPtr<UBoxComponent> Zona;
	
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
	
	FTimerHandle TimerAviso;
	
	UPROPERTY()
	TMap<TObjectPtr<APlayerController>, TObjectPtr<UReclaimAvisoQuestWidget>>
		AvisosPorJugador;

	void ActualizarAvisosLocales();

	void QuitarAvisosLocales();
	
	UPROPERTY(ReplicatedUsing = OnRep_EventoActivado)
	bool bEventoActivado = false;

	UFUNCTION()
	void OnRep_EventoActivado();

	void ActualizarBarrera();

	void RevisarJugadores();

	bool EstanTodosEnLaZona() const;
};

