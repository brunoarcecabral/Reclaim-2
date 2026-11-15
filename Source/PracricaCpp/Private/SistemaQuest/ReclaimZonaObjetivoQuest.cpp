#include "SistemaQuest/ReclaimZonaObjetivoQuest.h"

#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "SistemaQuest/ReclaimQuestComponent.h"
#include "TimerManager.h"


AReclaimZonaObjetivoQuest::AReclaimZonaObjetivoQuest()
{
    PrimaryActorTick.bCanEverTick = false;

    Zona = CreateDefaultSubobject<UBoxComponent>(TEXT("Zona"));
    SetRootComponent(Zona);

    Zona->SetBoxExtent(
        FVector(400.0f, 400.0f, 200.0f)
    );

    Zona->SetCollisionEnabled(
        ECollisionEnabled::QueryOnly
    );

    Zona->SetCollisionObjectType(ECC_WorldDynamic);
    Zona->SetCollisionResponseToAllChannels(ECR_Ignore);
    Zona->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    Zona->SetGenerateOverlapEvents(true);
}


void AReclaimZonaObjetivoQuest::BeginPlay()
{
    Super::BeginPlay();

    // Solo el servidor actualiza la misión compartida.
    if (!HasAuthority())
    {
        return;
    }

    if (IdMision.IsNone() || IdObjetivo.IsNone())
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("ZONA QUEST: %s necesita una mision y un objetivo."),
            *GetName()
        );

        return;
    }

    GetWorldTimerManager().SetTimer(
        TimerRevision,
        this,
        &AReclaimZonaObjetivoQuest::RevisarGrupo,
        FMath::Max(0.1f, IntervaloRevision),
        true
    );
}


void AReclaimZonaObjetivoQuest::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    GetWorldTimerManager().ClearTimer(TimerRevision);

    Super::EndPlay(EndPlayReason);
}


bool AReclaimZonaObjetivoQuest::HayJugadorEnLaZona() const
{
    const UWorld* Mundo = GetWorld();

    if (!Mundo || !IsValid(Zona))
    {
        return false;
    }

    for (
        FConstPlayerControllerIterator It =
            Mundo->GetPlayerControllerIterator();
        It;
        ++It
    )
    {
        const APlayerController* Controlador = It->Get();

        if (!IsValid(Controlador))
        {
            continue;
        }

        const APlayerState* EstadoJugador =
            Controlador->GetPlayerState<APlayerState>();

        if (EstadoJugador && EstadoJugador->IsOnlyASpectator())
        {
            continue;
        }

        const APawn* Personaje = Controlador->GetPawn();

        if (IsValid(Personaje) &&
            Zona->IsOverlappingActor(Personaje))
        {
            return true;
        }
    }

    return false;
}


void AReclaimZonaObjetivoQuest::RevisarGrupo()
{
    if (!HasAuthority() ||
        bObjetivoActivado ||
        !HayJugadorEnLaZona())
    {
        return;
    }

    UWorld* Mundo = GetWorld();

    if (!Mundo)
    {
        return;
    }

    AGameStateBase* EstadoJuego = Mundo->GetGameState();

    if (!IsValid(EstadoJuego))
    {
        return;
    }

    UReclaimQuestComponent* ComponenteQuest =
        EstadoJuego->FindComponentByClass<UReclaimQuestComponent>();

    if (!IsValid(ComponenteQuest))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("ZONA QUEST: No se encontro el componente de misiones.")
        );

        GetWorldTimerManager().ClearTimer(TimerRevision);
        return;
    }

    if (ComponenteQuest->AvanzarObjetivo(IdMision, IdObjetivo, 1))
    {
        bObjetivoActivado = true;

        GetWorldTimerManager().ClearTimer(TimerRevision);

        UE_LOG(
            LogTemp,
            Log,
            TEXT("ZONA QUEST: Objetivo completado: %s"),
            *IdObjetivo.ToString()
        );
    }
}

