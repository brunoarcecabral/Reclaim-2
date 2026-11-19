#include "SistemaGuardado/ReclaimFogataGuardadoComponent.h"

#include "GameFramework/Actor.h"

UReclaimFogataGuardadoComponent::UReclaimFogataGuardadoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UReclaimFogataGuardadoComponent::MarcarActivada()
{
	bActivada = true;
}

bool UReclaimFogataGuardadoComponent::EstaActivada() const
{
	return bActivada;
}

bool UReclaimFogataGuardadoComponent::RestaurarActivada()
{
	AActor* Propietario = GetOwner();

	if (!IsValid(Propietario) || !Propietario->HasAuthority())
	{
		return false;
	}

	bActivada = true;
	AlRestaurarActivada.Broadcast();

	return true;
}

