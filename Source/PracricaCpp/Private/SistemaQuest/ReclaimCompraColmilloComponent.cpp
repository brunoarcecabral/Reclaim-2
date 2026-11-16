#include "SistemaQuest/ReclaimCompraColmilloComponent.h"

#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
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
	bool bExito = false;

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
				// Pendiente: validar aquí el nivel 2 del comprador.
				bExito = ComponenteColmillo->EntregarColmilloAlGrupo();
			}
		}
	}

	ClienteRecibirResultado(bExito);
}


void UReclaimCompraColmilloComponent::ClienteRecibirResultado_Implementation(
	bool bExito
)
{
	AlRecibirResultado.Broadcast(bExito);
}

