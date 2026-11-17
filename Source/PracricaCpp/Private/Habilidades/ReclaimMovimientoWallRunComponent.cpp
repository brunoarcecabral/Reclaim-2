#include "Habilidades/ReclaimMovimientoWallRunComponent.h"

#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Habilidades/ReclaimHabilidadWallRunComponent.h"
#include "InputCoreTypes.h"


UReclaimMovimientoWallRunComponent::UReclaimMovimientoWallRunComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    PrimaryComponentTick.TickGroup = TG_PrePhysics;

    SetIsReplicatedByDefault(true);
}


void UReclaimMovimientoWallRunComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction
)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    ACharacter* Personaje = Cast<ACharacter>(GetOwner());

    if (!IsValid(Personaje))
    {
        return;
    }

    UCharacterMovementComponent* Movimiento =
        Personaje->GetCharacterMovement();

    UReclaimHabilidadWallRunComponent* Habilidad =
        Personaje->FindComponentByClass<
            UReclaimHabilidadWallRunComponent
        >();

    if (!IsValid(Movimiento) || !IsValid(Habilidad))
    {
        TerminarWallRun(Movimiento);
        return;
    }

    const bool bEsLocal = Personaje->IsLocallyControlled();
    const bool bEsServidor = Personaje->HasAuthority();
    
    if (!bEsLocal && !bEsServidor)
    {
        return;
    }

    bool bEspacioPresionado = false;

    if (bEsLocal)
    {
        APlayerController* Controlador =
            Cast<APlayerController>(Personaje->GetController());

        if (IsValid(Controlador))
        {
            bEspacioPresionado =
                Controlador->IsInputKeyDown(EKeys::SpaceBar);
        }
        
        if (bEspacioPresionado != bUltimoEspacioLocal)
        {
            bUltimoEspacioLocal = bEspacioPresionado;

            if (bEsServidor)
            {
                bEspacioEnServidor = bEspacioPresionado;
            }
            else
            {
                ServidorCambiarEspacio(bEspacioPresionado);
            }
        }
    }
    else
    {
        bEspacioPresionado = bEspacioEnServidor;
    }

    ActualizarMovimiento(
        Personaje,
        Movimiento,
        Habilidad,
        bEspacioPresionado
    );
}


void UReclaimMovimientoWallRunComponent::ServidorCambiarEspacio_Implementation(
    bool bPresionado
)
{
    bEspacioEnServidor = bPresionado;
}


bool UReclaimMovimientoWallRunComponent::BuscarPared(
    const ACharacter* Personaje,
    FHitResult& Resultado
) const
{
    if (!IsValid(Personaje) || !IsValid(GetWorld()))
    {
        return false;
    }

    const FVector Inicio = Personaje->GetActorLocation();
    const FVector Derecha = Personaje->GetActorRightVector();

    FCollisionQueryParams Parametros;
    Parametros.AddIgnoredActor(Personaje);

    const FCollisionShape Esfera = FCollisionShape::MakeSphere(
        FMath::Max(RadioDeteccion, 1.0f)
    );
    
    for (int32 Lado : {1, -1})
    {
        FHitResult Impacto;

        const FVector Fin =
            Inicio +
            Derecha * DistanciaDeteccion * static_cast<float>(Lado);

        const bool bImpacto = GetWorld()->SweepSingleByChannel(
            Impacto,
            Inicio,
            Fin,
            FQuat::Identity,
            ECC_Visibility,
            Esfera,
            Parametros
        );
        
        if (bImpacto &&
            Impacto.bBlockingHit &&
            FMath::Abs(Impacto.ImpactNormal.Z) < 0.3f)
        {
            Resultado = Impacto;
            return true;
        }
    }

    return false;
}


void UReclaimMovimientoWallRunComponent::ActualizarMovimiento(
    ACharacter* Personaje,
    UCharacterMovementComponent* Movimiento,
    UReclaimHabilidadWallRunComponent* Habilidad,
    bool bEspacioPresionado
)
{
    if (!IsValid(Personaje) ||
        !IsValid(Movimiento) ||
        !IsValid(Habilidad))
    {
        TerminarWallRun(Movimiento);
        return;
    }

    const bool bHabilidadActiva =
        Habilidad->EstaDesbloqueado() &&
        Habilidad->ObtenerEstadoWallRun() ==
            EEstadoHabilidadWallRun::Activa;

    if (!bHabilidadActiva ||
        !bEspacioPresionado ||
        !Movimiento->IsFalling())
    {
        TerminarWallRun(Movimiento);
        return;
    }

    FHitResult Pared;

    if (!BuscarPared(Personaje, Pared))
    {
        TerminarWallRun(Movimiento);
        return;
    }
    
    FVector NormalPared = Pared.ImpactNormal;
    NormalPared.Z = 0.0f;
    NormalPared = NormalPared.GetSafeNormal();

    if (NormalPared.IsNearlyZero())
    {
        TerminarWallRun(Movimiento);
        return;
    }
    
    FVector DireccionWallRun =
        FVector::CrossProduct(FVector::UpVector, NormalPared)
            .GetSafeNormal();
    
    FVector Referencia =
        Movimiento->Velocity.GetSafeNormal2D();

    if (Referencia.IsNearlyZero())
    {
        Referencia =
            Personaje->GetActorForwardVector().GetSafeNormal2D();
    }

    if (FVector::DotProduct(DireccionWallRun, Referencia) < 0.0f)
    {
        DireccionWallRun *= -1.0f;
    }

    if (!bEstaCorriendoEnPared)
    {
        GravedadAnterior = Movimiento->GravityScale;
        bEstaCorriendoEnPared = true;
    }
    
    Movimiento->GravityScale = 0.0f;
    
    const float VelocidadParalela = FMath::Abs(
        FVector::DotProduct(
            Movimiento->Velocity,
            DireccionWallRun
        )
    );

    const float VelocidadFinal = FMath::Max(
        VelocidadParalela,
        VelocidadWallRun
    );
    
    const FVector HaciaPared = -NormalPared * 70.0f;

    const FVector VelocidadHorizontal =
        DireccionWallRun * VelocidadFinal + HaciaPared;

    Movimiento->Velocity.X = VelocidadHorizontal.X;
    Movimiento->Velocity.Y = VelocidadHorizontal.Y;
    
    Movimiento->Velocity.Z = 0.0f;
}


void UReclaimMovimientoWallRunComponent::TerminarWallRun(
    UCharacterMovementComponent* Movimiento
)
{
    if (!bEstaCorriendoEnPared)
    {
        return;
    }

    if (IsValid(Movimiento))
    {
        Movimiento->GravityScale = GravedadAnterior;
    }

    bEstaCorriendoEnPared = false;
}


void UReclaimMovimientoWallRunComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    ACharacter* Personaje = Cast<ACharacter>(GetOwner());

    if (IsValid(Personaje))
    {
        TerminarWallRun(Personaje->GetCharacterMovement());
    }

    Super::EndPlay(EndPlayReason);
}

