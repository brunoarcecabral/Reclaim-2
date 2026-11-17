
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimColmilloComponent.generated.h"


class APawn;
class UActorComponent;
class UReclaimQuestComponent;
class UReclaimQuestData;


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

    // Indica si el colmillo debe aparecer en la tienda.
    UFUNCTION(BlueprintPure, Category = "Reclaim|Colmillo")
    bool EstaDisponibleEnTienda() const;

    // Indica si ya se realizó el desbloqueo compartido.
    UFUNCTION(BlueprintPure, Category = "Reclaim|Colmillo")
    bool YaFueComprado() const;

    // El servidor inicia la misión de subir de nivel.
    bool IniciarMisionNivel();

    // El servidor entrega un colmillo a cada jugador conectado.
    bool EntregarColmilloAlGrupo();

protected:

    virtual void BeginPlay() override;

    virtual void EndPlay(
        const EEndPlayReason::Type EndPlayReason
    ) override;

    // Misión que se inicia cuando el comprador no tiene nivel 2.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reclaim|Colmillo")
    TObjectPtr<UReclaimQuestData> MisionSubirNivel;

    // Misión que se inicia cuando alguien alcanza nivel 2.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reclaim|Colmillo")
    TObjectPtr<UReclaimQuestData> MisionObtenerHabilidad;

private:

    UPROPERTY()
    TObjectPtr<UReclaimQuestComponent> ComponenteQuest;

    UPROPERTY(ReplicatedUsing = OnRep_ColmilloComprado)
    bool bColmilloComprado = false;

    FTimerHandle TimerRevisionNivel;

    UFUNCTION()
    void OnRep_ColmilloComprado();

    UFUNCTION()
    void AlCambiarMisiones();

    UActorComponent* BuscarInventario(APawn* Personaje) const;

    void RevisarNivelDelGrupo();
};

