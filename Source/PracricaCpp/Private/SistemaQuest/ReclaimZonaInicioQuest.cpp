#include "SistemaQuest/ReclaimZonaInicioQuest.h"

#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "SistemaQuest/ReclaimQuestComponent.h"
#include "SistemaQuest/ReclaimQuestData.h"
#include "TimerManager.h"
#include "SistemaQuest/ReclaimAvisoQuestWidget.h"


AReclaimZonaInicioQuest::AReclaimZonaInicioQuest()
{
    PrimaryActorTick.bCanEverTick = false;
    
    bReplicates = true;

    Zona = CreateDefaultSubobject<UBoxComponent>(TEXT("Zona"));
    SetRootComponent(Zona);

    Zona->SetBoxExtent(FVector(300.0f, 300.0f, 150.0f));
    Zona->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Zona->SetCollisionObjectType(ECC_WorldDynamic);
    Zona->SetCollisionResponseToAllChannels(ECR_Ignore);
    Zona->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    Zona->SetGenerateOverlapEvents(true);

    Barrera = CreateDefaultSubobject<UBoxComponent>(TEXT("Barrera"));
    Barrera->SetupAttachment(Zona);
    
    Barrera->SetBoxExtent(FVector(30.0f, 300.0f, 200.0f));

    Barrera->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Barrera->SetCollisionObjectType(ECC_WorldDynamic);
    Barrera->SetCollisionResponseToAllChannels(ECR_Ignore);
    Barrera->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    Barrera->SetGenerateOverlapEvents(false);
}


void AReclaimZonaInicioQuest::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(
        AReclaimZonaInicioQuest,
        bEventoActivado
    );
}


void AReclaimZonaInicioQuest::BeginPlay()
{
    Super::BeginPlay();

    ActualizarBarrera();
    
    if (GetNetMode() != NM_DedicatedServer)
    {
        GetWorldTimerManager().SetTimer(
            TimerAviso,
            this,
            &AReclaimZonaInicioQuest::ActualizarAvisosLocales,
            0.25f,
            true
        );
    }
    
    if (!HasAuthority())
    {
        return;
    }

    if (!IsValid(MisionAIniciar))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("La zona %s no tiene una mision asignada."),
            *GetName()
        );

        return;
    }

    GetWorldTimerManager().SetTimer(
        TimerRevision,
        this,
        &AReclaimZonaInicioQuest::RevisarJugadores,
        FMath::Max(0.1f, IntervaloRevision),
        true
    );

    RevisarJugadores();
}


void AReclaimZonaInicioQuest::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    GetWorldTimerManager().ClearTimer(TimerRevision);
    GetWorldTimerManager().ClearTimer(TimerAviso);

    QuitarAvisosLocales();

    Super::EndPlay(EndPlayReason);
}

bool AReclaimZonaInicioQuest::EstanTodosEnLaZona() const
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


void AReclaimZonaInicioQuest::RevisarJugadores()
{
    if (!HasAuthority() || bEventoActivado)
    {
        return;
    }

    if (!EstanTodosEnLaZona())
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("QUEST DEBUG: Todavia no estan todos dentro de la zona.")
        );

        return;
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("QUEST DEBUG: Todos estan dentro. Buscando GameState y componente.")
    );



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
            TEXT("No se encontro ReclaimQuestComponent en el GameState.")
        );

        GetWorldTimerManager().ClearTimer(TimerRevision);
        return;
    }
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("QUEST DEBUG: Se intentara iniciar la mision %s."),
        *MisionAIniciar->IdMision.ToString()
    );


    if (ComponenteQuest->IniciarMision(MisionAIniciar))
    {
        bEventoActivado = true;
        
        ActualizarBarrera();

        ForceNetUpdate();
        GetWorldTimerManager().ClearTimer(TimerRevision);

        UE_LOG(
            LogTemp,
            Log,
            TEXT("Mision iniciada: %s"),
            *MisionAIniciar->IdMision.ToString()
        );
    }
}


void AReclaimZonaInicioQuest::OnRep_EventoActivado()
{
    ActualizarBarrera();
    ActualizarBarrera();
}


void AReclaimZonaInicioQuest::ActualizarBarrera()
{
    if (!IsValid(Barrera))
    {
        return;
    }

    if (bEventoActivado)
    {
        Barrera->SetCollisionEnabled(
            ECollisionEnabled::NoCollision
        );
    }
    else
    {
        Barrera->SetCollisionEnabled(
            ECollisionEnabled::QueryOnly
        );
    }
}
void AReclaimZonaInicioQuest::ActualizarAvisosLocales()
{
    UWorld* Mundo = GetWorld();

    if (!Mundo || !IsValid(Zona))
    {
        return;
    }
    
    if (bEventoActivado)
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


void AReclaimZonaInicioQuest::QuitarAvisosLocales()
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
bool AReclaimZonaInicioQuest::EstaActivadaParaGuardado() const
{
    return bEventoActivado;
}

bool AReclaimZonaInicioQuest::RestaurarActivada()
{
    if (!HasAuthority())
    {
        return false;
    }

    bEventoActivado = true;

    ActualizarBarrera();
    ActualizarAvisosLocales();

    GetWorldTimerManager().ClearTimer(TimerRevision);
    ForceNetUpdate();

    return true;
}


