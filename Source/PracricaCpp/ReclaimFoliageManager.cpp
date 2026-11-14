#include "ReclaimFoliageManager.h"

#include "Net/UnrealNetwork.h"

AReclaimFoliageManager::AReclaimFoliageManager()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bAlwaysRelevant = true;
}

void AReclaimFoliageManager::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AReclaimFoliageManager, EstadosFoliage);
}

int32 AReclaimFoliageManager::BuscarIndice(FVector Ubicacion) const
{
	for (int32 Indice = 0; Indice < EstadosFoliage.Num(); ++Indice)
	{
		if (EstadosFoliage[Indice].Ubicacion.Equals(Ubicacion, 1.0))
		{
			return Indice;
		}
	}

	return INDEX_NONE;
}

int32 AReclaimFoliageManager::RegistrarGolpe(FVector Ubicacion)
{
	// El servidor es el único que modifica el contador.
	if (!HasAuthority())
	{
		return 0;
	}

	int32 Indice = BuscarIndice(Ubicacion);

	if (Indice == INDEX_NONE)
	{
		FReclaimFoliageEstado NuevoEstado;
		NuevoEstado.Ubicacion = Ubicacion;
		NuevoEstado.Golpes = 0;

		Indice = EstadosFoliage.Add(NuevoEstado);
	}

	EstadosFoliage[Indice].Golpes += 1;

	// Solicita que el estado actualizado se envíe pronto.
	ForceNetUpdate();

	return EstadosFoliage[Indice].Golpes;
}

int32 AReclaimFoliageManager::ObtenerGolpes(FVector Ubicacion) const
{
	const int32 Indice = BuscarIndice(Ubicacion);

	if (Indice == INDEX_NONE)
	{
		return 0;
	}

	return EstadosFoliage[Indice].Golpes;
}

