#include "SistemaQuest/ReclaimZonaCampamento.h"

#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "SistemaQuest/ReclaimAvisoQuestWidget.h"
#include "SistemaQuest/ReclaimQuestComponent.h"
#include "SistemaQuest/ReclaimQuestData.h"
#include "TimerManager.h"


AReclaimZonaCampamento::AReclaimZonaCampamento()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    bAlwaysRelevant = true;

    Zona = CreateDefaultSubobject<UBoxComponent>(TEXT("Zona"));
    SetRootComponent(Zona);

    Zona->SetBoxExtent(FVector(400.0f, 400.0f, 200.0f));
    Zona->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Zona->SetCollisionObjectType(ECC_WorldDynamic);
    Zona->SetCollisionResponseToAllChannels(ECR_Ignore);
    Zona->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    Zona->SetGenerateOverlapEvents(true);

    Barrera = CreateDefaultSubobject<UBoxComponent>(TEXT("Barrera"));
    Barrera->SetupAttachment(Zona);

    Barrera->SetBoxExtent(FVector(30.0f, 400.0f, 250.0f));
    Barrera->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Barrera->SetCollisionObjectType(ECC_WorldDynamic);
    Barrera->SetCollisionResponseToAllChannels(ECR_Ignore);
    Barrera->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    Barrera->SetGenerateOverlapEvents(false);
}


void AReclaimZonaCampamento::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AReclaimZonaCampamento, bAbierta);
}

bool AReclaimZonaCampamento::EstaAbiertaParaGuardado() const
{
    return bAbierta;
}

bool AReclaimZonaCampamento::RestaurarAbierta()
{
    if (!HasAuthority())
    {
        return false;
    }

    bAbierta = true;

    ActualizarBarrera();
    ActualizarAvisosLocales();

    GetWorldTimerManager().ClearTimer(TimerRevision);
    ForceNetUpdate();

    return true;
}

void AReclaimZonaCampamento::BeginPlay()
{
    Super::BeginPlay();

    ActualizarBarrera();

    if (GetNetMode() != NM_DedicatedServer)
    {
        GetWorldTimerManager().SetTimer(
            TimerAvisos,
            this,
            &AReclaimZonaCampamento::ActualizarAvisosLocales,
            0.25f,
            true
        );
    }

    if (!HasAuthority())
    {
        return;
    }

    if (IdMisionActual.IsNone() ||
        IdObjetivoLlegada.IsNone() ||
        !IsValid(MisionSiguiente))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("CAMPAMENTO: Faltan los IDs o la mision siguiente en %s."),
            *GetName()
        );

        return;
    }

    GetWorldTimerManager().SetTimer(
        TimerRevision,
        this,
        &AReclaimZonaCampamento::RevisarGrupo,
        FMath::Max(0.1f, IntervaloRevision),
        true
    );
}


void AReclaimZonaCampamento::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    GetWorldTimerManager().ClearTimer(TimerRevision);
    GetWorldTimerManager().ClearTimer(TimerAvisos);

    QuitarAvisosLocales();

    Super::EndPlay(EndPlayReason);
}


bool AReclaimZonaCampamento::EstanTodosEnLaZona() const
{
    const UWorld* Mundo = GetWorld();

    if (!Mundo || !IsValid(Zona))
    {
        return false;
    }

    int32 JugadoresParticipantes = 0;

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

        ++JugadoresParticipantes;

        const APawn* Personaje = Controlador->GetPawn();

        if (!IsValid(Personaje) ||
            !Zona->IsOverlappingActor(Personaje))
        {
            return false;
        }
    }

    return JugadoresParticipantes > 0;
}


void AReclaimZonaCampamento::RevisarGrupo()
{
    if (!HasAuthority() || bAbierta || !EstanTodosEnLaZona())
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
            TEXT("CAMPAMENTO: No se encontro el componente de misiones.")
        );

        GetWorldTimerManager().ClearTimer(TimerRevision);
        return;
    }
    
    if (!IsValid(MisionSiguiente) ||
        MisionSiguiente->IdMision.IsNone() ||
        MisionSiguiente->Objetivos.IsEmpty())
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("CAMPAMENTO: La mision siguiente no es valida.")
        );

        GetWorldTimerManager().ClearTimer(TimerRevision);
        return;
    }

    const TArray<FEstadoQuest> Misiones =
        ComponenteQuest->ObtenerMisiones();

    bool bMisionActualEncontrada = false;
    bool bMisionSiguienteYaExiste = false;

    for (const FEstadoQuest& Estado : Misiones)
    {
        if (!IsValid(Estado.DatosMision))
        {
            continue;
        }

        if (Estado.DatosMision->IdMision == IdMisionActual &&
            !Estado.bCompletada)
        {
            bMisionActualEncontrada = true;
        }

        if (Estado.DatosMision->IdMision == MisionSiguiente->IdMision)
        {
            bMisionSiguienteYaExiste = true;
        }
    }
    
    if (!bMisionActualEncontrada)
    {
        return;
    }

    if (bMisionSiguienteYaExiste)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("CAMPAMENTO: La mision siguiente ya existe.")
        );

        GetWorldTimerManager().ClearTimer(TimerRevision);
        return;
    }

    if (!ComponenteQuest->AvanzarObjetivo(
        IdMisionActual,
        IdObjetivoLlegada,
        1
    ))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("CAMPAMENTO: No se pudo completar el objetivo de llegada.")
        );

        GetWorldTimerManager().ClearTimer(TimerRevision);
        return;
    }

    if (!ComponenteQuest->IniciarMision(MisionSiguiente))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("CAMPAMENTO: Se completo la llegada, pero no se pudo iniciar la siguiente mision.")
        );

        GetWorldTimerManager().ClearTimer(TimerRevision);
        return;
    }

    bAbierta = true;

    ActualizarBarrera();
    ActualizarAvisosLocales();

    ForceNetUpdate();
    GetWorldTimerManager().ClearTimer(TimerRevision);

    UE_LOG(
        LogTemp,
        Log,
        TEXT("CAMPAMENTO: Grupo reunido. Entrada desbloqueada.")
    );
}


void AReclaimZonaCampamento::OnRep_Abierta()
{
    ActualizarBarrera();
    ActualizarAvisosLocales();
}


void AReclaimZonaCampamento::ActualizarBarrera()
{
    if (!IsValid(Barrera))
    {
        return;
    }

    Barrera->SetCollisionEnabled(
        bAbierta
            ? ECollisionEnabled::NoCollision
            : ECollisionEnabled::QueryOnly
    );
}


void AReclaimZonaCampamento::ActualizarAvisosLocales()
{
    UWorld* Mundo = GetWorld();

    if (!Mundo || !IsValid(Zona))
    {
        return;
    }

    if (bAbierta)
    {
        QuitarAvisosLocales();
        return;
    }

    TSet<APlayerController*> JugadoresQueEsperan;

    for (
        FConstPlayerControllerIterator It =
            Mundo->GetPlayerControllerIterator();
        It;
        ++It
    )
    {
        APlayerController* Controlador = It->Get();

        if (!IsValid(Controlador) ||
            !Controlador->IsLocalController())
        {
            continue;
        }

        const APawn* Personaje = Controlador->GetPawn();

        if (!IsValid(Personaje) ||
            !Zona->IsOverlappingActor(Personaje))
        {
            continue;
        }

        JugadoresQueEsperan.Add(Controlador);

        if (!AvisosPorJugador.Contains(Controlador))
        {
            UReclaimAvisoQuestWidget* Aviso =
                CreateWidget<UReclaimAvisoQuestWidget>(
                    Controlador,
                    UReclaimAvisoQuestWidget::StaticClass()
                );

            if (IsValid(Aviso) && Aviso->AddToPlayerScreen())
            {
                AvisosPorJugador.Add(Controlador, Aviso);
            }
        }
    }

    for (auto It = AvisosPorJugador.CreateIterator(); It; ++It)
    {
        APlayerController* Controlador = It.Key().Get();

        if (!IsValid(Controlador) ||
            !JugadoresQueEsperan.Contains(Controlador))
        {
            if (IsValid(It.Value()))
            {
                It.Value()->RemoveFromParent();
            }

            It.RemoveCurrent();
        }
    }
}


void AReclaimZonaCampamento::QuitarAvisosLocales()
{
    for (const auto& Entrada : AvisosPorJugador)
    {
        if (IsValid(Entrada.Value))
        {
            Entrada.Value->RemoveFromParent();
        }
    }

    AvisosPorJugador.Empty();
}

