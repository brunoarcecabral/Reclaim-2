#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimNivelComponent.generated.h"


class UReclaimNivelWidget;


USTRUCT(BlueprintType)
struct FEstadoNivelReclaim
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Nivel")
    int32 Nivel = 1;

    UPROPERTY(BlueprintReadOnly, Category = "Nivel")
    int32 ExperienciaActual = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Nivel")
    int32 ExperienciaRequerida = 100;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAlCambiarNivelReclaim);


UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimNivelComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    UReclaimNivelComponent();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

    UPROPERTY(BlueprintAssignable, Category = "Reclaim|Nivel")
    FAlCambiarNivelReclaim AlCambiarNivel;

    UFUNCTION(BlueprintPure, Category = "Reclaim|Nivel")
    FEstadoNivelReclaim ObtenerEstadoNivel() const;

    UFUNCTION(BlueprintPure, Category = "Reclaim|Nivel")
    int32 ObtenerNivel() const;

    // Debe ejecutarse en el servidor.
    UFUNCTION(
        BlueprintCallable,
        BlueprintAuthorityOnly,
        Category = "Reclaim|Nivel"
    )
    bool AgregarExperiencia(int32 Cantidad);
    bool RestaurarEstadoNivel(const FEstadoNivelReclaim& EstadoGuardado);

protected:

    virtual void BeginPlay() override;

    virtual void EndPlay(
        const EEndPlayReason::Type EndPlayReason
    ) override;

    UPROPERTY(
        EditDefaultsOnly,
        BlueprintReadOnly,
        Category = "Reclaim|Nivel",
        meta = (ClampMin = "1", UIMin = "1")
    )
    int32 ExperienciaParaNivel2 = 100;

    UPROPERTY(
        EditDefaultsOnly,
        BlueprintReadOnly,
        Category = "Reclaim|Nivel",
        meta = (ClampMin = "1.0", UIMin = "1.0")
    )
    float MultiplicadorExperiencia = 1.25f;

    UPROPERTY(
        EditDefaultsOnly,
        BlueprintReadOnly,
        Category = "Reclaim|Nivel",
        meta = (ClampMin = "2", UIMin = "2")
    )
    int32 NivelMaximo = 100;

private:

    UPROPERTY(ReplicatedUsing = OnRep_EstadoNivel)
    FEstadoNivelReclaim EstadoNivel;

    UPROPERTY()
    TObjectPtr<UReclaimNivelWidget> InterfazNivel;

    FTimerHandle TimerCrearInterfaz;

    UFUNCTION()
    void OnRep_EstadoNivel();

    void NotificarCambio();

    void IntentarCrearInterfaz();
};

