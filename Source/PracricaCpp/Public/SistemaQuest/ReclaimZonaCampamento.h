#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReclaimZonaCampamento.generated.h"


class APlayerController;
class UBoxComponent;
class UReclaimAvisoQuestWidget;
class UReclaimQuestData;


UCLASS()
class PRACRICACPP_API AReclaimZonaCampamento : public AActor
{
	GENERATED_BODY()

public:

	AReclaimZonaCampamento();

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
	) override;

	// Zona en la que deben reunirse todos.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<UBoxComponent> Zona;

	// Muro temporal que bloquea la entrada al campamento.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<UBoxComponent> Barrera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName IdMisionActual = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName IdObjetivoLlegada = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<UReclaimQuestData> MisionSiguiente;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Quest",
		meta = (ClampMin = "0.1", UIMin = "0.1")
	)
	float IntervaloRevision = 0.25f;

private:

	UPROPERTY(ReplicatedUsing = OnRep_Abierta)
	bool bAbierta = false;

	UPROPERTY()
	TMap<
		TObjectPtr<APlayerController>,
		TObjectPtr<UReclaimAvisoQuestWidget>
	> AvisosPorJugador;

	FTimerHandle TimerRevision;
	FTimerHandle TimerAvisos;

	UFUNCTION()
	void OnRep_Abierta();

	bool EstanTodosEnLaZona() const;

	void RevisarGrupo();

	void ActualizarBarrera();

	void ActualizarAvisosLocales();

	void QuitarAvisosLocales();
};

