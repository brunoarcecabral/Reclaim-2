#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SistemaQuest/ReclaimQuestData.h"
#include "ReclaimQuestComponent.generated.h"


class APlayerController;
class UReclaimMisionesWidget;
class UReclaimSaveGame;

USTRUCT(BlueprintType)
struct FProgresoObjetivoQuest
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FName IdObjetivo = NAME_None;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    int32 CantidadActual = 0;
};

USTRUCT(BlueprintType)
struct FEstadoQuest
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    TObjectPtr<UReclaimQuestData> DatosMision = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    TArray<FProgresoObjetivoQuest> ProgresoObjetivos;

    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    bool bCompletada = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAlCambiarMisiones);


UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimQuestComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    UReclaimQuestComponent();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FAlCambiarMisiones AlCambiarMisiones;

    UFUNCTION(BlueprintPure, Category = "Quest")
    TArray<FEstadoQuest> ObtenerMisiones() const;
    
    bool CopiarMisionesAlGuardado(UReclaimSaveGame* DatosGuardado) const;
    
    bool RestaurarMisionesDesdeGuardado(
        const UReclaimSaveGame* DatosGuardado
    );
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Quest")
    bool IniciarMision(UReclaimQuestData* DatosMision);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Quest")
    bool AvanzarObjetivo(
        FName IdMision,
        FName IdObjetivo,
        int32 Cantidad
    );

protected:

    virtual void BeginPlay() override;

    virtual void EndPlay(
        const EEndPlayReason::Type EndPlayReason
    ) override;

    UPROPERTY(ReplicatedUsing = OnRep_Misiones)
    TArray<FEstadoQuest> Misiones;

    UFUNCTION()
    void OnRep_Misiones();

private:

    FTimerHandle TimerInterfaces;

    UPROPERTY()
    TMap<
        TObjectPtr<APlayerController>,
        TObjectPtr<UReclaimMisionesWidget>
    > InterfacesPorJugador;

    void NotificarCambio();

    void RevisarInterfacesLocales();

    void QuitarInterfacesLocales();
};

