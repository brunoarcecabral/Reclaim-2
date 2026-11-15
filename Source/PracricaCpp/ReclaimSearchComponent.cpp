#include "ReclaimSearchComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

UReclaimSearchComponent::UReclaimSearchComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}

void UReclaimSearchComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    LimpiarBusquedaEnServidor();

    Super::EndPlay(EndPlayReason);
}

void UReclaimSearchComponent::SolicitarInicioBusqueda(AActor* Objeto)
{
    if (!GetOwner() || !IsValid(Objeto))
    {
        return;
    }

    ServidorIniciarBusqueda(Objeto);
}

void UReclaimSearchComponent::SolicitarCancelarBusqueda()
{
    if (!GetOwner())
    {
        return;
    }

    ServidorCancelarBusqueda();
}

bool UReclaimSearchComponent::EsClasePermitida(AActor* Objeto) const
{
    if (!IsValid(Objeto))
    {
        return false;
    }

    for (const TSubclassOf<AActor>& Clase : ClasesBuscables)
    {
        if (Clase && Objeto->IsA(Clase))
        {
            return true;
        }
    }

    return false;
}

void UReclaimSearchComponent::
ServidorIniciarBusqueda_Implementation(AActor* Objeto)
{
    AActor* Personaje = GetOwner();
    UWorld* Mundo = GetWorld();

    if (!IsValid(Personaje) || !Mundo || !IsValid(Objeto))
    {
        return;
    }

    if (Objeto->IsActorBeingDestroyed() || !EsClasePermitida(Objeto))
    {
        return;
    }

    // La comprobación se hace en el servidor.
    if (!Objeto->IsOverlappingActor(Personaje))
    {
        return;
    }

    // Si había una búsqueda anterior, se cancela.
    LimpiarBusquedaEnServidor();

    ObjetoEnBusqueda = Objeto;

    Mundo->GetTimerManager().SetTimer(
        TemporizadorBusqueda,
        this,
        &UReclaimSearchComponent::CompletarBusquedaEnServidor,
        FMath::Max(DuracionBusqueda, 0.1f),
        false
    );
}

void UReclaimSearchComponent::
ServidorCancelarBusqueda_Implementation()
{
    LimpiarBusquedaEnServidor();
}

void UReclaimSearchComponent::CompletarBusquedaEnServidor()
{
    AActor* Personaje = GetOwner();
    AActor* Objeto = ObjetoEnBusqueda.Get();

    // Limpiamos la referencia antes de continuar.
    ObjetoEnBusqueda.Reset();

    if (!IsValid(Personaje) || !IsValid(Objeto))
    {
        return;
    }

    if (Objeto->IsActorBeingDestroyed() || !EsClasePermitida(Objeto))
    {
        return;
    }

    // Si el jugador ya salió del área, no se entrega el recurso.
    if (!Objeto->IsOverlappingActor(Personaje))
    {
        return;
    }

    OnBusquedaCompletadaServidor.Broadcast(Objeto);
}

void UReclaimSearchComponent::LimpiarBusquedaEnServidor()
{
    if (UWorld* Mundo = GetWorld())
    {
        Mundo->GetTimerManager().ClearTimer(TemporizadorBusqueda);
    }

    ObjetoEnBusqueda.Reset();
}

