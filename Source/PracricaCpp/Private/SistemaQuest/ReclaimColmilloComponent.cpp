#include "SistemaQuest/ReclaimColmilloComponent.h"

#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Inventario/ReclaimInventarioColmillo.h"
#include "Net/UnrealNetwork.h"
#include "SistemaNivel/ReclaimNivelComponent.h"
#include "SistemaNivel/ReclaimTutorialNivelComponent.h"
#include "SistemaQuest/ReclaimQuestComponent.h"
#include "SistemaQuest/ReclaimQuestData.h"
#include "TimerManager.h"
#include "SistemaArmas/ReclaimArmaComponent.h"


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

    if (Propietario->HasAuthority())
    {
        if (UWorld* Mundo = GetWorld())
        {
            Mundo->GetTimerManager().SetTimer(
                TimerRevisionNivel,
                this,
                &UReclaimColmilloComponent::RevisarNivelDelGrupo,
                0.5f,
                true
            );
        }
    }
}


void UReclaimColmilloComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    if (UWorld* Mundo = GetWorld())
    {
        Mundo->GetTimerManager().ClearTimer(TimerRevisionNivel);
    }

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
        if (!IsValid(Estado.DatosMision) || Estado.bCompletada)
        {
            continue;
        }

        const FName IdMision = Estado.DatosMision->IdMision;

        if (IdMision == FName(TEXT("BuscarHabilidad")) ||
            IdMision == FName(TEXT("SubirNivelDos")) ||
            IdMision == FName(TEXT("ObtenerHabilidadTienda")))
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

bool UReclaimColmilloComponent::IniciarMisionNivel()
{
    AActor* Propietario = GetOwner();

    if (!IsValid(Propietario) ||
        !Propietario->HasAuthority() ||
        bColmilloComprado ||
        !IsValid(ComponenteQuest) ||
        !IsValid(MisionConseguirArma))
    {
        return false;
    }

    const TArray<FEstadoQuest> Misiones =
        ComponenteQuest->ObtenerMisiones();

    bool bMisionBusquedaActiva = false;

    for (const FEstadoQuest& Estado : Misiones)
    {
        if (!IsValid(Estado.DatosMision))
        {
            continue;
        }
        
        if (Estado.DatosMision->IdMision == MisionConseguirArma->IdMision)
        {
            return !Estado.bCompletada;
        }
        
        if (IsValid(MisionSubirNivel) &&
            Estado.DatosMision->IdMision == MisionSubirNivel->IdMision)
        {
            return !Estado.bCompletada;
        }

        if (Estado.DatosMision->IdMision == FName(TEXT("BuscarHabilidad")) &&
            !Estado.bCompletada)
        {
            bMisionBusquedaActiva = true;
        }
    }

    if (!bMisionBusquedaActiva)
    {
        return false;
    }

    if (!ComponenteQuest->IniciarMision(MisionConseguirArma))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("COLMILLO: No se pudo iniciar la mision de conseguir arma.")
        );

        return false;
    }

    if (!ComponenteQuest->AvanzarObjetivo(
        FName(TEXT("BuscarHabilidad")),
        FName(TEXT("ConseguirHabilidad")),
        1
    ))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("COLMILLO: Se inicio la mision del arma, pero no se pudo completar la busqueda.")
        );

        return false;
    }

    UE_LOG(
        LogTemp,
        Log,
        TEXT("COLMILLO: Mision de conseguir arma iniciada.")
    );

    return true;
}

void UReclaimColmilloComponent::RevisarNivelDelGrupo()
{
    RevisarArmasDelGrupo();
    AActor* Propietario = GetOwner();
    UWorld* Mundo = GetWorld();

    if (!IsValid(Propietario) ||
        !Propietario->HasAuthority() ||
        !Mundo ||
        !IsValid(ComponenteQuest) ||
        !IsValid(MisionObtenerHabilidad))
    {
        return;
    }

    if (MisionObtenerHabilidad->IdMision !=
        FName(TEXT("ObtenerHabilidadTienda")))
    {
        return;
    }

    const TArray<FEstadoQuest> Misiones =
        ComponenteQuest->ObtenerMisiones();

    bool bMisionNivelActiva = false;

    for (const FEstadoQuest& Estado : Misiones)
    {
        if (IsValid(Estado.DatosMision) &&
            Estado.DatosMision->IdMision == FName(TEXT("SubirNivelDos")) &&
            !Estado.bCompletada)
        {
            bMisionNivelActiva = true;
            break;
        }
    }

    if (!bMisionNivelActiva)
    {
        return;
    }

    bool bAlguienAlcanzoNivelDos = false;

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

        APawn* Personaje = Controlador->GetPawn();

        if (!IsValid(Personaje))
        {
            continue;
        }

        UReclaimNivelComponent* Nivel =
            Personaje->FindComponentByClass<UReclaimNivelComponent>();

        if (IsValid(Nivel) && Nivel->ObtenerNivel() >= 2)
        {
            bAlguienAlcanzoNivelDos = true;
            break;
        }
    }

    if (!bAlguienAlcanzoNivelDos)
    {
        return;
    }

    if (!ComponenteQuest->IniciarMision(MisionObtenerHabilidad))
    {
        return;
    }

    if (!ComponenteQuest->AvanzarObjetivo(
        FName(TEXT("SubirNivelDos")),
        FName(TEXT("AlcanzarNivelDos")),
        1
    ))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("COLMILLO: Se inicio la mision de tienda, pero no se pudo completar la de nivel.")
        );

        return;
    }

    UE_LOG(
        LogTemp,
        Log,
        TEXT("COLMILLO: Un jugador alcanzo el nivel 2.")
    );
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

    FName IdMisionCompra = NAME_None;
    FName IdObjetivoCompra = NAME_None;

    const TArray<FEstadoQuest> Misiones =
        ComponenteQuest->ObtenerMisiones();

    for (const FEstadoQuest& Estado : Misiones)
    {
        if (!IsValid(Estado.DatosMision) || Estado.bCompletada)
        {
            continue;
        }

        if (Estado.DatosMision->IdMision == FName(TEXT("BuscarHabilidad")))
        {
            IdMisionCompra = FName(TEXT("BuscarHabilidad"));
            IdObjetivoCompra = FName(TEXT("ConseguirHabilidad"));
            break;
        }

        if (Estado.DatosMision->IdMision ==
            FName(TEXT("ObtenerHabilidadTienda")))
        {
            IdMisionCompra = FName(TEXT("ObtenerHabilidadTienda"));
            IdObjetivoCompra = FName(TEXT("DesbloquearColmillo"));
            break;
        }
    }

    if (IdMisionCompra.IsNone() || IdObjetivoCompra.IsNone())
    {
        return false;
    }

    TArray<UActorComponent*> Inventarios;
    
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
    
    for (UActorComponent* Inventario : Inventarios)
    {
        const bool bAgregado =
            IReclaimInventarioColmillo::Execute_AgregarColmillo(
                Inventario
            );

        if (!bAgregado)
        {
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
    
    if (!ComponenteQuest->AvanzarObjetivo(
        IdMisionCompra,
        IdObjetivoCompra,
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
    
    UReclaimTutorialNivelComponent* Tutorial =
        Propietario->FindComponentByClass<UReclaimTutorialNivelComponent>();

    if (IsValid(Tutorial))
    {
        Tutorial->AbrirTutorialWallRun();
    }

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
void UReclaimColmilloComponent::RevisarArmasDelGrupo()
{
    AActor* Propietario = GetOwner();
    UWorld* Mundo = GetWorld();

    if (!IsValid(Propietario) ||
        !Propietario->HasAuthority() ||
        !Mundo ||
        !IsValid(ComponenteQuest) ||
        !IsValid(MisionConseguirArma) ||
        !IsValid(MisionSubirNivel))
    {
        return;
    }
    
    bool bMisionArmaActiva = false;

    const TArray<FEstadoQuest> Misiones =
        ComponenteQuest->ObtenerMisiones();

    for (const FEstadoQuest& Estado : Misiones)
    {
        if (IsValid(Estado.DatosMision) &&
            Estado.DatosMision->IdMision == MisionConseguirArma->IdMision &&
            !Estado.bCompletada)
        {
            bMisionArmaActiva = true;
            break;
        }
    }

    if (!bMisionArmaActiva)
    {
        return;
    }

    int32 JugadoresRevisados = 0;

    for (FConstPlayerControllerIterator It =
             Mundo->GetPlayerControllerIterator();
         It;
         ++It)
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
        
        if (!IsValid(Personaje))
        {
            return;
        }

        UReclaimArmaComponent* Arma =
            Personaje->FindComponentByClass<UReclaimArmaComponent>();

        if (!IsValid(Arma) || !Arma->TieneArma())
        {
            return;
        }

        ++JugadoresRevisados;
    }

    if (JugadoresRevisados == 0)
    {
        return;
    }
    
    if (!ComponenteQuest->IniciarMision(MisionSubirNivel))
    {
        return;
    }

    if (!ComponenteQuest->AvanzarObjetivo(
        MisionConseguirArma->IdMision,
        FName(TEXT("Elegir_Arma")),
        1
    ))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("ARMA: Se inicio la mision de nivel, pero no se pudo completar el objetivo del arma.")
        );

        return;
    }

    UReclaimTutorialNivelComponent* Tutorial =
        Propietario->FindComponentByClass<UReclaimTutorialNivelComponent>();

    if (IsValid(Tutorial))
    {
        Tutorial->AbrirTutorialArma();
        Tutorial->AbrirTutorial();
    }
    
    UE_LOG(
        LogTemp,
        Log,
        TEXT("ARMA: Todos los jugadores tienen arma. Mision de nivel iniciada.")
    );
}
bool UReclaimColmilloComponent::EstaPendienteElegirArma() const
{
    if (!IsValid(ComponenteQuest))
    {
        return false;
    }

    const TArray<FEstadoQuest> Misiones =
        ComponenteQuest->ObtenerMisiones();

    for (const FEstadoQuest& Estado : Misiones)
    {
        if (!IsValid(Estado.DatosMision) || Estado.bCompletada)
        {
            continue;
        }
        
        if (Estado.DatosMision->IdMision == FName(TEXT("BuscarHabilidad")))
        {
            return true;
        }
        
        if (IsValid(MisionConseguirArma) &&
            Estado.DatosMision->IdMision == MisionConseguirArma->IdMision)
        {
            return true;
        }
    }

    return false;
}

