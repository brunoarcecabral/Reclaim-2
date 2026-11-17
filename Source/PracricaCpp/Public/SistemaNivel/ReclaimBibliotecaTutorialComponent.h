#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimBibliotecaTutorialComponent.generated.h"


class APlayerController;
class UInputComponent;
class UReclaimBibliotecaTutorialWidget;
class UReclaimIndicadorTutorialWidget;
class UReclaimNotaQuestData;


UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimBibliotecaTutorialComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:

	UReclaimBibliotecaTutorialComponent();

	UFUNCTION(BlueprintPure, Category = "Reclaim|Tutoriales")
	TArray<UReclaimNotaQuestData*> ObtenerTutorialesDisponibles() const;

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
	) override;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Reclaim|Tutoriales"
	)
	TObjectPtr<UReclaimNotaQuestData> TutorialControles;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Reclaim|Tutoriales"
	)
	TObjectPtr<UReclaimNotaQuestData> TutorialNivel;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Reclaim|Tutoriales"
	)
	TObjectPtr<UReclaimNotaQuestData> TutorialWallRun;

private:

	UPROPERTY()
	TObjectPtr<APlayerController> ControladorLocal;

	UPROPERTY()
	TObjectPtr<UInputComponent> EntradaTutorial;

	UPROPERTY()
	TObjectPtr<UReclaimIndicadorTutorialWidget> Indicador;

	UPROPERTY()
	TObjectPtr<UReclaimBibliotecaTutorialWidget> VentanaBiblioteca;

	FTimerHandle TimerInicializacion;

	bool bCursorEstabaVisible = false;

	void IntentarInicializar();

	void AlternarBiblioteca();

	void AbrirBiblioteca();

	UFUNCTION()
	void CerrarBiblioteca();
};

