#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimHabilidadWallRunComponent.generated.h"

class UActorComponent;
class UTexture2D;
class UUserWidget;
class UReclaimQuestData;


UENUM(BlueprintType)
enum class EResultadoConsumirColmillo : uint8
{
    Exito,
    NivelInsuficiente,
    YaDesbloqueado,
    SinColmillo,
    InventarioNoEncontrado
};


UENUM(BlueprintType)
enum class EEstadoHabilidadWallRun : uint8
{
    Lista,
    Activa,
    Recargando
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FResultadoConsumirColmillo,
    EResultadoConsumirColmillo,
    Resultado
);


DECLARE_DYNAMIC_MULTICAST_DELEGATE(
    FAlCambiarWallRunDesbloqueado
);


UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimHabilidadWallRunComponent
    : public UActorComponent
{
    GENERATED_BODY()

public:

    UReclaimHabilidadWallRunComponent();

    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction
    ) override;

    virtual void EndPlay(
        const EEndPlayReason::Type EndPlayReason
    ) override;

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;
    
    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Reclaim|WallRun|Interfaz"
    )
    UTexture2D* IconoWallRun = nullptr;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reclaim|WallRun|Misiones")
    UReclaimQuestData* MisionAprenderWallRun = nullptr;
    
    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Reclaim|WallRun|Tiempos",
        meta = (ClampMin = "0.1")
    )
    float DuracionWallRun = 10.0f;
    
    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Reclaim|WallRun|Tiempos",
        meta = (ClampMin = "0.1")
    )
    float TiempoRecargaWallRun = 20.0f;

    UPROPERTY(BlueprintAssignable, Category = "Reclaim|WallRun")
    FResultadoConsumirColmillo AlRecibirResultadoConsumo;

    UPROPERTY(BlueprintAssignable, Category = "Reclaim|WallRun")
    FAlCambiarWallRunDesbloqueado AlCambiarDesbloqueo;

    UFUNCTION(BlueprintPure, Category = "Reclaim|WallRun")
    bool EstaDesbloqueado() const;

    UFUNCTION(BlueprintPure, Category = "Reclaim|WallRun")
    EEstadoHabilidadWallRun ObtenerEstadoWallRun() const;
    
    UFUNCTION(BlueprintCallable, Category = "Reclaim|WallRun")
    void SolicitarConsumirColmillo();
    
    UFUNCTION(BlueprintCallable, Category = "Reclaim|WallRun")
    void SolicitarActivarWallRun();

private:

    UPROPERTY(Transient)
    UUserWidget* WidgetIconoWallRun = nullptr;

    UPROPERTY(ReplicatedUsing = OnRep_WallRunDesbloqueado)
    bool bWallRunDesbloqueado = false;

    UPROPERTY(Replicated)
    EEstadoHabilidadWallRun EstadoWallRun =
        EEstadoHabilidadWallRun::Lista;

    FTimerHandle TimerDuracionWallRun;
    FTimerHandle TimerRecargaWallRun;

    UFUNCTION()
    void OnRep_WallRunDesbloqueado();

    UFUNCTION(Server, Reliable)
    void ServidorConsumirColmillo();

    UFUNCTION(Server, Reliable)
    void ServidorActivarWallRun();

    UFUNCTION(Client, Reliable)
    void ClienteRecibirResultadoConsumo(
        EResultadoConsumirColmillo Resultado
    );

    UActorComponent* BuscarInventario() const;

    void CrearIconoWallRun();
    void ActualizarColorIconoWallRun();

    void IniciarRecargaWallRun();
    void FinalizarRecargaWallRun();
};

