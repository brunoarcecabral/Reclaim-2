#include "ReclaimPickupComponent.h"

#include "GameFramework/Actor.h"

UReclaimPickupComponent::UReclaimPickupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UReclaimPickupComponent::SolicitarRecoleccion(AActor* Objeto)
{
	if (!GetOwner() || !IsValid(Objeto))
	{
		return;
	}

	ServidorSolicitarRecoleccion(Objeto);
}

void UReclaimPickupComponent::
ServidorSolicitarRecoleccion_Implementation(AActor* Objeto)
{
	AActor* Personaje = GetOwner();

	if (!IsValid(Personaje) || !IsValid(Objeto))
	{
		return;
	}

	if (Objeto->IsActorBeingDestroyed())
	{
		return;
	}

	if (!ClaseRecolectable || !Objeto->IsA(ClaseRecolectable))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ReclaimPickup: el objeto no es una clase recolectable válida.")
		);

		return;
	}

	if (!Objeto->IsOverlappingActor(Personaje))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ReclaimPickup: el personaje no está dentro del área del objeto.")
		);

		return;
	}

	OnRecolectarEnServidor.Broadcast(Objeto);
}

void UReclaimPickupComponent::SolicitarSoltarItem(int32 IndiceSlot)
{
	if (!GetOwner())
	{
		return;
	}

	ServidorSolicitarSoltarItem(IndiceSlot);
}

void UReclaimPickupComponent::
ServidorSolicitarSoltarItem_Implementation(int32 IndiceSlot)
{
	if (!IsValid(GetOwner()) || IndiceSlot < 0)
	{
		return;
	}

	// El Blueprint del inventario deberá comprobar que el índice existe
	// y que el slot tiene al menos una unidad antes de crear el objeto.
	OnSoltarEnServidor.Broadcast(IndiceSlot);
}

