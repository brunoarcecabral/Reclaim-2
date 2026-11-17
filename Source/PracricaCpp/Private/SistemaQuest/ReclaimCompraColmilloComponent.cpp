#include "SistemaQuest/ReclaimCompraColmilloComponent.h"

#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
#include "SistemaNivel/ReclaimNivelComponent.h"
#include "SistemaQuest/ReclaimColmilloComponent.h"


UReclaimCompraColmilloComponent::UReclaimCompraColmilloComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}


void UReclaimCompraColmilloComponent::SolicitarDesbloqueo()
{
    APawn* Personaje = Cast<APawn>(GetOwner());

    if (!IsValid(Personaje) || !Personaje->IsLocallyControlled())
    {
        return;
    }

    ServidorSolicitarDesbloqueo();
}


void UReclaimCompraColmilloComponent::ServidorSolicitarDesbloqueo_Implementation()
{
    APawn* Personaje = Cast<APawn>(GetOwner());

    if (!IsValid(Personaje) || !Personaje->IsPlayerControlled())
    {
        ClienteRecibirResultado(false, false);
        return;
    }

    UReclaimNivelComponent* ComponenteNivel =
        Personaje->FindComponentByClass<UReclaimNivelComponent>();

    if (!IsValid(ComponenteNivel))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("COLMILLO: El comprador no tiene componente de nivel.")
        );

        ClienteRecibirResultado(false, false);
        return;
    }

    if (ComponenteNivel->ObtenerNivel() < 2)
    {
        UWorld* Mundo = GetWorld();

        if (Mundo)
        {
            AGameStateBase* EstadoJuego = Mundo->GetGameState();

            if (IsValid(EstadoJuego))
            {
                UReclaimColmilloComponent* ComponenteColmillo =
                    EstadoJuego->FindComponentByClass<UReclaimColmilloComponent>();

                if (IsValid(ComponenteColmillo))
                {
                    ComponenteColmillo->IniciarMisionNivel();
                }
            }
        }

        ClienteRecibirResultado(false, true);
        return;
    }

    UWorld* Mundo = GetWorld();

    if (!Mundo)
    {
        ClienteRecibirResultado(false, false);
        return;
    }

    AGameStateBase* EstadoJuego = Mundo->GetGameState();

    if (!IsValid(EstadoJuego))
    {
        ClienteRecibirResultado(false, false);
        return;
    }

    UReclaimColmilloComponent* ComponenteColmillo =
        EstadoJuego->FindComponentByClass<UReclaimColmilloComponent>();

    if (!IsValid(ComponenteColmillo))
    {
        ClienteRecibirResultado(false, false);
        return;
    }

    const bool bExito =
        ComponenteColmillo->EntregarColmilloAlGrupo();

    ClienteRecibirResultado(bExito, false);
}


void UReclaimCompraColmilloComponent::ClienteRecibirResultado_Implementation(
    bool bExito,
    bool bNivelInsuficiente
)
{
    AlRecibirResultado.Broadcast(
        bExito,
        bNivelInsuficiente
    );
}

