#include "UI/ReclaimVisibilidadBarraVidaComponent.h"

#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"


UReclaimVisibilidadBarraVidaComponent::
UReclaimVisibilidadBarraVidaComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}


void UReclaimVisibilidadBarraVidaComponent::BeginPlay()
{
    Super::BeginPlay();

    UWorld* Mundo = GetWorld();
    AActor* Enemigo = GetOwner();

    if (!IsValid(Mundo) || !IsValid(Enemigo))
    {
        return;
    }

    // Un servidor dedicado no necesita mostrar interfaces.
    if (Mundo->GetNetMode() == NM_DedicatedServer)
    {
        return;
    }

    TInlineComponentArray<UWidgetComponent*> Componentes;
    Enemigo->GetComponents(Componentes);

    for (UWidgetComponent* Componente : Componentes)
    {
        if (IsValid(Componente) &&
            Componente->GetFName() == NombreComponenteBarra)
        {
            BarraDeVida = Componente;
            break;
        }
    }

    if (!IsValid(BarraDeVida))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("BARRA VIDA: No se encontro el Widget Component '%s' en %s."),
            *NombreComponenteBarra.ToString(),
            *GetNameSafe(Enemigo)
        );

        return;
    }

    // Comienza oculta hasta comprobar la posición del jugador.
    BarraDeVida->SetVisibility(false);

    ActualizarVisibilidadBarra();

    Mundo->GetTimerManager().SetTimer(
        TimerRevisionBarra,
        this,
        &UReclaimVisibilidadBarraVidaComponent::ActualizarVisibilidadBarra,
        0.2f,
        true
    );
}


void UReclaimVisibilidadBarraVidaComponent::ActualizarVisibilidadBarra()
{
    UWorld* Mundo = GetWorld();
    AActor* Enemigo = GetOwner();

    if (!IsValid(Mundo) ||
        !IsValid(Enemigo) ||
        !IsValid(BarraDeVida))
    {
        return;
    }

    APlayerController* Controlador =
        Mundo->GetFirstPlayerController();

    APawn* Jugador =
        IsValid(Controlador) && Controlador->IsLocalController()
            ? Controlador->GetPawn()
            : nullptr;

    if (!IsValid(Jugador))
    {
        BarraDeVida->SetVisibility(false);
        return;
    }

    const float DistanciaMaxima = FMath::Max(
        0.0f,
        DistanciaVisible
    );

    const bool bEstaCerca =
        FVector::DistSquared(
            Jugador->GetActorLocation(),
            Enemigo->GetActorLocation()
        ) <= FMath::Square(DistanciaMaxima);

    BarraDeVida->SetVisibility(bEstaCerca);
}


void UReclaimVisibilidadBarraVidaComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    if (UWorld* Mundo = GetWorld())
    {
        Mundo->GetTimerManager().ClearTimer(TimerRevisionBarra);
    }

    Super::EndPlay(EndPlayReason);
}

