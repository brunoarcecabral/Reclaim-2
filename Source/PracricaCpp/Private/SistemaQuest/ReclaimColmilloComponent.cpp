#include "SistemaQuest/ReclaimColmilloComponent.h"

#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Inventario/ReclaimInventarioColmillo.h"
#include "Net/UnrealNetwork.h"
#include "SistemaQuest/ReclaimQuestComponent.h"
#include "SistemaQuest/ReclaimQuestData.h"


UReclaimColmilloComponent::UReclaimColmilloComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}


void UReclaimColmilloComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Propietario = GetOwner();

    if (!IsValid(Propietario))
    {
        return;
    }

    ComponenteQuest =
        Propietario->FindComponentByClass<UReclaimQuestComponent>();

    if (!IsValid(ComponenteQuest))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("COLMILLO: No se encontro ReclaimQuestComponent.")
        );

        return;
    }

    ComponenteQuest->AlCambiarMisiones.AddDynamic(
        this,
        &UReclaimColmilloComponent::AlCambiarMisiones
    );
}


void UReclaimColmilloComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    if (IsValid(ComponenteQuest))
    {
        ComponenteQuest->AlCambiarMisiones.RemoveDynamic(
            this,
            &UReclaimColmilloComponent::AlCambiarMisiones
        );
    }

    ComponenteQuest = nullptr;

    Super::EndPlay(EndPlayReason);
}


void UReclaimColmilloComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(
        UReclaimColmilloComponent,
        bColmilloComprado
    );
}


bool UReclaimColmilloComponent::EstaDisponibleEnTienda() const
{
    if (bColmilloComprado || !IsValid(ComponenteQuest))
    {
        return false;
    }

    const TArray<FEstadoQuest> Misiones =
        ComponenteQuest->ObtenerMisiones();

    for (const FEstadoQuest& Estado : Misiones)
    {
        if (IsValid(Estado.DatosMision) &&
            Estado.DatosMision->IdMision == FName(TEXT("BuscarHabilidad")) &&
            !Estado.bCompletada)
        {
            return true;
        }
    }

    return false;
}


bool UReclaimColmilloComponent::YaFueComprado() const
{
    return bColmilloComprado;
}


UActorComponent* UReclaimColmilloComponent::BuscarInventario(
    APawn* Personaje
) const
{
    if (!IsValid(Personaje))
    {
        return nullptr;
    }

    TInlineComponentArray<UActorComponent*> Componentes;
    Personaje->GetComponents(Componentes);

    for (UActorComponent* Componente : Componentes)
    {
        if (IsValid(Componente) &&
            Componente->GetClass()->ImplementsInterface(
                UReclaimInventarioColmillo::StaticClass()
            ))
        {
            return Componente;
        }
    }

    return nullptr;
}


bool UReclaimColmilloComponent::EntregarColmilloAlGrupo()
{
    AActor* Propietario = GetOwner();
    UWorld* Mundo = GetWorld();

    if (!IsValid(Propietario) ||
        !Propietario->HasAuthority() ||
        !Mundo ||
        !EstaDisponibleEnTienda())
    {
        return false;
    }

    TArray<UActorComponent*> Inventarios;

    // Primero comprobamos todos los inventarios.
    for (
        FConstPlayerControllerIterator It =
            Mundo->GetPlayerControllerIterator();
        It;
        ++It
    )
    {
        APlayerController* Controlador = It->Get();

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

        APawn* Personaje = Controlador->GetPawn();
        UActorComponent* Inventario = BuscarInventario(Personaje);

        if (!IsValid(Inventario) ||
            !IReclaimInventarioColmillo::Execute_PuedeRecibirColmillo(
                Inventario
            ))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("COLMILLO: Un jugador no puede recibir el objeto.")
            );

            return false;
        }

        Inventarios.Add(Inventario);
    }

    if (Inventarios.IsEmpty())
    {
        return false;
    }

    TArray<UActorComponent*> InventariosEntregados;

    // Agregamos una unidad a cada inventario.
    for (UActorComponent* Inventario : Inventarios)
    {
        const bool bAgregado =
            IReclaimInventarioColmillo::Execute_AgregarColmillo(
                Inventario
            );

        if (!bAgregado)
        {
            // Si falla, intentamos deshacer las entregas anteriores.
            for (UActorComponent* Anterior : InventariosEntregados)
            {
                const bool bRevertido =
                    IReclaimInventarioColmillo::Execute_QuitarColmillo(
                        Anterior
                    );

                if (!bRevertido)
                {
                    UE_LOG(
                        LogTemp,
                        Error,
                        TEXT("COLMILLO: No se pudo revertir una entrega.")
                    );
                }
            }

            return false;
        }

        InventariosEntregados.Add(Inventario);
    }

    // La misión se completa cuando el grupo recibe el objeto.
    if (!ComponenteQuest->AvanzarObjetivo(
        FName(TEXT("BuscarHabilidad")),
        FName(TEXT("ConseguirHabilidad")),
        1
    ))
    {
        for (UActorComponent* Inventario : InventariosEntregados)
        {
            const bool bRevertido =
                IReclaimInventarioColmillo::Execute_QuitarColmillo(
                    Inventario
                );

            if (!bRevertido)
            {
                UE_LOG(
                    LogTemp,
                    Error,
                    TEXT("COLMILLO: No se pudo revertir una entrega.")
                );
            }
        }

        return false;
    }

    bColmilloComprado = true;

    AlCambiarColmillo.Broadcast();
    Propietario->ForceNetUpdate();

    UE_LOG(
        LogTemp,
        Log,
        TEXT("COLMILLO: Objeto entregado al grupo.")
    );

    return true;
}


void UReclaimColmilloComponent::OnRep_ColmilloComprado()
{
    AlCambiarColmillo.Broadcast();
}


void UReclaimColmilloComponent::AlCambiarMisiones()
{
    AlCambiarColmillo.Broadcast();
}

