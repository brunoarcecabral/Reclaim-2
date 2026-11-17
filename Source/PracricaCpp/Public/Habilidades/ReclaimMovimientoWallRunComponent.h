#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimMovimientoWallRunComponent.generated.h"

class ACharacter;
class UCharacterMovementComponent;
class UReclaimHabilidadWallRunComponent;


UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimMovimientoWallRunComponent
    : public UActorComponent
{
    GENERATED_BODY()

public:

    UReclaimMovimientoWallRunComponent();

    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction
    ) override;

    virtual void EndPlay(
        const EEndPlayReason::Type EndPlayReason
    ) override;

    // Distancia de búsqueda a cada lado del personaje.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reclaim|WallRun",
        meta = (ClampMin = "1.0"))
    float DistanciaDeteccion = 80.0f;

    // Radio de la esfera usada para detectar la pared.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reclaim|WallRun",
        meta = (ClampMin = "1.0"))
    float RadioDeteccion = 15.0f;

    // Velocidad mínima mientras corre por la pared.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reclaim|WallRun",
        meta = (ClampMin = "1.0"))
    float VelocidadWallRun = 650.0f;

    // Gravedad durante el wall run.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reclaim|WallRun",
        meta = (ClampMin = "0.0"))
    float GravedadWallRun = 0.15f;

private:

    bool bEspacioEnServidor = false;
    bool bUltimoEspacioLocal = false;
    bool bEstaCorriendoEnPared = false;

    float GravedadAnterior = 1.0f;

    UFUNCTION(Server, Reliable)
    void ServidorCambiarEspacio(bool bPresionado);

    bool BuscarPared(
        const ACharacter* Personaje,
        FHitResult& Resultado
    ) const;

    void ActualizarMovimiento(
        ACharacter* Personaje,
        UCharacterMovementComponent* Movimiento,
        UReclaimHabilidadWallRunComponent* Habilidad,
        bool bEspacioPresionado
    );

    void TerminarWallRun(UCharacterMovementComponent* Movimiento);
};

