#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimTutorialNivelComponent.generated.h"


class UReclaimNotaQuestData;


UENUM(BlueprintType)
enum class ETutorialReclaim : uint8
{
    Ninguno,
    Nivel,
    WallRun
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAlCambiarTutorialNivel);


UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimTutorialNivelComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    UReclaimTutorialNivelComponent();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

    UPROPERTY(BlueprintAssignable, Category = "Reclaim|Tutorial")
    FAlCambiarTutorialNivel AlCambiarTutorial;

    UFUNCTION(BlueprintPure, Category = "Reclaim|Tutorial")
    bool EstaAbierto() const;

    UFUNCTION(BlueprintPure, Category = "Reclaim|Tutorial")
    UReclaimNotaQuestData* ObtenerDatosTutorial() const;

    // Estas funciones se ejecutan en el servidor.
    bool AbrirTutorial();

    bool AbrirTutorialWallRun();

    bool CerrarTutorial();

protected:

    virtual void EndPlay(
        const EEndPlayReason::Type EndPlayReason
    ) override;

    // Conserva la asignación de DA_TutorialNivel.
    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Reclaim|Tutorial"
    )
    TObjectPtr<UReclaimNotaQuestData> DatosTutorial;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Reclaim|Tutorial"
    )
    TObjectPtr<UReclaimNotaQuestData> DatosTutorialWallRun;

    // Tiempo en segundos que permanece abierto cada tutorial.
    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Reclaim|Tutorial",
        meta = (ClampMin = "1.0", UIMin = "1.0")
    )
    float DuracionTutorial = 30.0f;

private:

    UPROPERTY(ReplicatedUsing = OnRep_TutorialActual)
    ETutorialReclaim TutorialActual = ETutorialReclaim::Ninguno;

    // La cola se administra en el servidor.
    TArray<ETutorialReclaim> TutorialesPendientes;

    FTimerHandle TimerCerrarTutorial;

    UFUNCTION()
    void OnRep_TutorialActual();

    bool SolicitarTutorial(ETutorialReclaim Tutorial);

    void IniciarTemporizador();

    void CerrarPorTiempo();

    void NotificarCambio();
};

