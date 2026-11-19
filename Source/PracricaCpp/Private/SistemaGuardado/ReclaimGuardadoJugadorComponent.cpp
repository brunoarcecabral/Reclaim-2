#include "SistemaGuardado/ReclaimGuardadoJugadorComponent.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"
#include "SistemaGuardado/ReclaimGuardadoSubsystem.h"
#include "SistemaGuardado/ReclaimInventarioGuardadoComponent.h"
#include "SistemaGuardado/ReclaimSaveGame.h"
#include "SistemaNivel/ReclaimNivelComponent.h"
#include "SistemaQuest/ReclaimQuestComponent.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "SistemaQuest/ReclaimNotaQuestActor.h"
#include "SistemaQuest/ReclaimNotaQuestData.h"
#include "SistemaQuest/ReclaimZonaInicioQuest.h"
#include "SistemaQuest/ReclaimZonaObjetivoQuest.h"
#include "SistemaQuest/ReclaimZonaCampamento.h"


UReclaimGuardadoJugadorComponent::UReclaimGuardadoJugadorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UReclaimGuardadoJugadorComponent::BeginPlay()
{
    Super::BeginPlay();

    ACharacter* Personaje = Cast<ACharacter>(GetOwner());

    if (!IsValid(Personaje) || !Personaje->IsLocallyControlled())
    {
        return;
    }

    UWorld* Mundo = GetWorld();

    if (!Mundo || Mundo->GetNetMode() != NM_Standalone)
    {
        return;
    }
    
    Mundo->GetTimerManager().SetTimerForNextTick(
        this,
        &UReclaimGuardadoJugadorComponent::AplicarGuardadoPendiente
    );
}

bool UReclaimGuardadoJugadorComponent::GuardarJugador()
{
    ACharacter* Personaje = Cast<ACharacter>(GetOwner());
    UWorld* Mundo = GetWorld();

    if (!IsValid(Personaje) ||
        !Personaje->IsLocallyControlled() ||
        !Personaje->HasAuthority() ||
        !Mundo ||
        Mundo->GetNetMode() != NM_Standalone)
    {
        return false;
    }

    UGameInstance* GameInstance = Personaje->GetGameInstance();

    if (!IsValid(GameInstance))
    {
        return false;
    }

    UReclaimGuardadoSubsystem* Guardado =
        GameInstance->GetSubsystem<UReclaimGuardadoSubsystem>();

    UReclaimNivelComponent* Nivel =
        Personaje->FindComponentByClass<UReclaimNivelComponent>();

    UReclaimInventarioGuardadoComponent* Inventario =
        Personaje->FindComponentByClass<
            UReclaimInventarioGuardadoComponent
        >();

    if (!IsValid(Guardado) ||
        !IsValid(Nivel) ||
        !IsValid(Inventario))
    {
        return false;
    }

    UReclaimSaveGame* Datos = Guardado->CrearDatosGuardado();

    if (!IsValid(Datos))
    {
        return false;
    }
    
    Datos->PosicionJugador = Personaje->GetActorLocation();
    Datos->RotacionJugador = Personaje->GetActorRotation();
    
    const FEstadoNivelReclaim EstadoNivel =
        Nivel->ObtenerEstadoNivel();

    Datos->Nivel = EstadoNivel.Nivel;
    Datos->ExperienciaActual = EstadoNivel.ExperienciaActual;
    Datos->ExperienciaRequerida = EstadoNivel.ExperienciaRequerida;
    
    Inventario->CopiarInventarioAlGuardado(Datos);
    Inventario->CopiarCheckpointAlGuardado(Datos);

    AGameStateBase* GameState = Mundo->GetGameState();

    UReclaimQuestComponent* Misiones =
        IsValid(GameState)
            ? GameState->FindComponentByClass<UReclaimQuestComponent>()
            : nullptr;

    if (!IsValid(Misiones) ||
        !Misiones->CopiarMisionesAlGuardado(Datos))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("GUARDADO: no se pudieron copiar las misiones.")
        );

        return false;
    }
    
    TSet<FName> IdsNotas;

    for (TActorIterator<AReclaimNotaQuestActor> It(Mundo); It; ++It)
    {
        AReclaimNotaQuestActor* Nota = *It;

        if (!IsValid(Nota))
        {
            continue;
        }

        UReclaimNotaQuestData* DatosNota = Nota->ObtenerDatosNota();

        if (!IsValid(DatosNota) || DatosNota->IdNota.IsNone())
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("GUARDADO: una nota no tiene datos o IdNota valido.")
            );

            return false;
        }

        if (IdsNotas.Contains(DatosNota->IdNota))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("GUARDADO: hay notas con IdNota repetido: %s"),
                *DatosNota->IdNota.ToString()
            );

            return false;
        }

        IdsNotas.Add(DatosNota->IdNota);

        if (Nota->EstaFinalizada())
        {
            Datos->NotasFinalizadas.Add(DatosNota->IdNota);
        }
    }
        
    TSet<FName> IdsZonas;

    for (TActorIterator<AReclaimZonaInicioQuest> It(Mundo); It; ++It)
    {
        AReclaimZonaInicioQuest* Zona = *It;

        if (!IsValid(Zona))
        {
            continue;
        }

        if (Zona->IdGuardado.IsNone() ||
            IdsZonas.Contains(Zona->IdGuardado))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("GUARDADO: zona de inicio sin ID o con ID repetido.")
            );

            return false;
        }

        IdsZonas.Add(Zona->IdGuardado);

        if (Zona->EstaActivadaParaGuardado())
        {
            Datos->ZonasInicioActivadas.Add(Zona->IdGuardado);
        }
    }

    for (TActorIterator<AReclaimZonaObjetivoQuest> It(Mundo); It; ++It)
    {
        AReclaimZonaObjetivoQuest* Zona = *It;

        if (!IsValid(Zona))
        {
            continue;
        }

        if (Zona->IdGuardado.IsNone() ||
            IdsZonas.Contains(Zona->IdGuardado))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("GUARDADO: zona de objetivo sin ID o con ID repetido.")
            );

            return false;
        }

        IdsZonas.Add(Zona->IdGuardado);

        if (Zona->EstaActivadaParaGuardado())
        {
            Datos->ZonasObjetivoActivadas.Add(Zona->IdGuardado);
        }
    }

    for (TActorIterator<AReclaimZonaCampamento> It(Mundo); It; ++It)
    {
        AReclaimZonaCampamento* Zona = *It;

        if (!IsValid(Zona))
        {
            continue;
        }

        if (Zona->IdGuardado.IsNone() ||
            IdsZonas.Contains(Zona->IdGuardado))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("GUARDADO: campamento sin ID o con ID repetido.")
            );

            return false;
        }

        IdsZonas.Add(Zona->IdGuardado);

        if (Zona->EstaAbiertaParaGuardado())
        {
            Datos->ZonasCampamentoAbiertas.Add(Zona->IdGuardado);
        }
    }
    
    return Guardado->GuardarPartida(Datos);
}

void UReclaimGuardadoJugadorComponent::AplicarGuardadoPendiente()
{
    ACharacter* Personaje = Cast<ACharacter>(GetOwner());
    UWorld* Mundo = GetWorld();

    if (!IsValid(Personaje) ||
        !Personaje->IsLocallyControlled() ||
        !Personaje->HasAuthority() ||
        !Mundo ||
        Mundo->GetNetMode() != NM_Standalone)
    {
        return;
    }

    UGameInstance* GameInstance = Personaje->GetGameInstance();

    if (!IsValid(GameInstance))
    {
        return;
    }

    UReclaimGuardadoSubsystem* Guardado =
        GameInstance->GetSubsystem<UReclaimGuardadoSubsystem>();

    UReclaimNivelComponent* Nivel =
        Personaje->FindComponentByClass<UReclaimNivelComponent>();

    UReclaimInventarioGuardadoComponent* Inventario =
        Personaje->FindComponentByClass<
            UReclaimInventarioGuardadoComponent
        >();

    if (!IsValid(Guardado) ||
        !IsValid(Nivel) ||
        !IsValid(Inventario))
    {
        return;
    }

    UReclaimSaveGame* Datos = Guardado->ObtenerDatosPendientes();

    if (!IsValid(Datos))
    {
        return;
    }
    
    AGameStateBase* GameState = Mundo->GetGameState();

    UReclaimQuestComponent* Misiones =
        IsValid(GameState)
            ? GameState->FindComponentByClass<UReclaimQuestComponent>()
            : nullptr;

    if (!IsValid(Misiones))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("GUARDADO: no se encontro el componente de misiones.")
        );

        return;
    }

    if (!Misiones->RestaurarMisionesDesdeGuardado(Datos))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("GUARDADO: no se pudieron restaurar las misiones.")
        );

        return;
    }
    
    FEstadoNivelReclaim EstadoNivel;
    EstadoNivel.Nivel = Datos->Nivel;
    EstadoNivel.ExperienciaActual = Datos->ExperienciaActual;
    EstadoNivel.ExperienciaRequerida = Datos->ExperienciaRequerida;

    if (!Nivel->RestaurarEstadoNivel(EstadoNivel))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("GUARDADO: no se pudo restaurar el nivel.")
        );

        return;
    }
    
    TMap<FName, AReclaimZonaInicioQuest*> ZonasInicio;
    TMap<FName, AReclaimZonaObjetivoQuest*> ZonasObjetivo;
    TMap<FName, AReclaimZonaCampamento*> ZonasCampamento;
    TSet<FName> IdsZonas;

    for (TActorIterator<AReclaimZonaInicioQuest> It(Mundo); It; ++It)
    {
        AReclaimZonaInicioQuest* Zona = *It;

        if (!IsValid(Zona))
        {
            continue;
        }

        if (Zona->IdGuardado.IsNone() ||
            IdsZonas.Contains(Zona->IdGuardado))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("CARGA: zona de inicio sin ID o con ID repetido.")
            );

            return;
        }

        IdsZonas.Add(Zona->IdGuardado);
        ZonasInicio.Add(Zona->IdGuardado, Zona);
    }

    for (TActorIterator<AReclaimZonaObjetivoQuest> It(Mundo); It; ++It)
    {
        AReclaimZonaObjetivoQuest* Zona = *It;

        if (!IsValid(Zona))
        {
            continue;
        }

        if (Zona->IdGuardado.IsNone() ||
            IdsZonas.Contains(Zona->IdGuardado))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("CARGA: zona de objetivo sin ID o con ID repetido.")
            );

            return;
        }

        IdsZonas.Add(Zona->IdGuardado);
        ZonasObjetivo.Add(Zona->IdGuardado, Zona);
    }

    for (TActorIterator<AReclaimZonaCampamento> It(Mundo); It; ++It)
    {
        AReclaimZonaCampamento* Zona = *It;

        if (!IsValid(Zona))
        {
            continue;
        }

        if (Zona->IdGuardado.IsNone() ||
            IdsZonas.Contains(Zona->IdGuardado))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("CARGA: campamento sin ID o con ID repetido.")
            );

            return;
        }

        IdsZonas.Add(Zona->IdGuardado);
        ZonasCampamento.Add(Zona->IdGuardado, Zona);
    }
    
    for (const FName& Id : Datos->ZonasInicioActivadas)
    {
        if (!IsValid(ZonasInicio.FindRef(Id)))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("CARGA: no se encontro la zona de inicio %s."),
                *Id.ToString()
            );

            return;
        }
    }

    for (const FName& Id : Datos->ZonasObjetivoActivadas)
    {
        if (!IsValid(ZonasObjetivo.FindRef(Id)))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("CARGA: no se encontro la zona de objetivo %s."),
                *Id.ToString()
            );

            return;
        }
    }

    for (const FName& Id : Datos->ZonasCampamentoAbiertas)
    {
        if (!IsValid(ZonasCampamento.FindRef(Id)))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("CARGA: no se encontro el campamento %s."),
                *Id.ToString()
            );

            return;
        }
    }
    
    for (const FName& Id : Datos->ZonasInicioActivadas)
    {
        ZonasInicio.FindRef(Id)->RestaurarActivada();
    }

    for (const FName& Id : Datos->ZonasObjetivoActivadas)
    {
        ZonasObjetivo.FindRef(Id)->RestaurarActivada();
    }

    for (const FName& Id : Datos->ZonasCampamentoAbiertas)
    {
        ZonasCampamento.FindRef(Id)->RestaurarAbierta();
    }


    TMap<FName, AReclaimNotaQuestActor*> NotasPorId;

    for (TActorIterator<AReclaimNotaQuestActor> It(Mundo); It; ++It)
    {
        AReclaimNotaQuestActor* Nota = *It;

        if (!IsValid(Nota))
        {
            continue;
        }

        UReclaimNotaQuestData* DatosNota = Nota->ObtenerDatosNota();

        if (!IsValid(DatosNota) ||
            DatosNota->IdNota.IsNone() ||
            NotasPorId.Contains(DatosNota->IdNota))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("GUARDADO: nota sin ID valido o con ID repetido.")
            );

            return;
        }

        NotasPorId.Add(DatosNota->IdNota, Nota);
    }
    
    for (const FName& IdNota : Datos->NotasFinalizadas)
    {
        if (!NotasPorId.Contains(IdNota))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("GUARDADO: no se encontro la nota %s."),
                *IdNota.ToString()
            );

            return;
        }
    }
    
    for (const FName& IdNota : Datos->NotasFinalizadas)
    {
        AReclaimNotaQuestActor* Nota = NotasPorId.FindRef(IdNota);

        if (!IsValid(Nota) || !Nota->RestaurarComoFinalizada())
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("GUARDADO: no se pudo restaurar la nota %s."),
                *IdNota.ToString()
            );

            return;
        }
    }
    
    Inventario->RestaurarInventarioDesdeGuardado(Datos);
    Inventario->RestaurarCheckpointDesdeGuardado(Datos);

    Personaje->SetActorLocationAndRotation(
        Datos->PosicionJugador,
        Datos->RotacionJugador,
        false,
        nullptr,
        ETeleportType::TeleportPhysics
    );

    Guardado->LimpiarDatosPendientes();
}
