#include "SistemaQuest/ReclaimNotaJugadorComponent.h"

#include "GameFramework/Pawn.h"
#include "SistemaQuest/ReclaimNotaQuestActor.h"


UReclaimNotaJugadorComponent::UReclaimNotaJugadorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
}


void UReclaimNotaJugadorComponent::PrepararNota(
	AReclaimNotaQuestActor* Nota
)
{
	NotaActual = Nota;
}


void UReclaimNotaJugadorComponent::SolicitarContinuarNota()
{
	if (!IsValid(NotaActual))
	{
		return;
	}

	ServidorContinuarNota(NotaActual);
}


void UReclaimNotaJugadorComponent::LimpiarNota()
{
	NotaActual = nullptr;
}


void UReclaimNotaJugadorComponent::ServidorContinuarNota_Implementation(
	AReclaimNotaQuestActor* Nota
)
{
	APawn* Personaje = Cast<APawn>(GetOwner());

	if (!IsValid(Personaje) || !IsValid(Nota))
	{
		return;
	}
	
	if (!Nota->PuedeContinuar(Personaje))
	{
		return;
	}

	Nota->FinalizarLectura();
}

