#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReclaimZonaObjetivoQuest.generated.h"


class UBoxComponent;
class UReclaimQuestData;


UCLASS()
class PRACRICACPP_API AReclaimZonaObjetivoQuest : public AActor
{
	GENERATED_BODY()

public:

	AReclaimZonaObjetivoQuest();
	
	UPROPERTY(EditInstanceOnly, Category = "Reclaim|Guardado")
	FName IdGuardado = NAME_None;

	bool EstaActivadaParaGuardado() const;
	bool RestaurarActivada();

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName IdMision = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName IdObjetivo = NAME_None;
	
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

	FTimerHandle TimerRevision;

	bool bObjetivoActivado = false;

	bool HayJugadorEnLaZona() const;

	void RevisarGrupo();
};

