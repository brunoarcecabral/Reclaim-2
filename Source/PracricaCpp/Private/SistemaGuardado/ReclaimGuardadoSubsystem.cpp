#include "SistemaGuardado/ReclaimGuardadoSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "SistemaGuardado/ReclaimSaveGame.h"

namespace
{
	const FString RanuraSingleplayer = TEXT("Reclaim_Singleplayer");
	constexpr int32 IndiceUsuario = 0;
}

UReclaimSaveGame*
UReclaimGuardadoSubsystem::CrearDatosGuardado() const
{
	return Cast<UReclaimSaveGame>(
		UGameplayStatics::CreateSaveGameObject(
			UReclaimSaveGame::StaticClass()
		)
	);
}

bool UReclaimGuardadoSubsystem::GuardarPartida(
	UReclaimSaveGame* DatosGuardado
) const
{
	if (!IsValid(DatosGuardado))
	{
		return false;
	}

	return UGameplayStatics::SaveGameToSlot(
		DatosGuardado,
		RanuraSingleplayer,
		IndiceUsuario
	);
}

bool UReclaimGuardadoSubsystem::ExistePartidaGuardada() const
{
	return UGameplayStatics::DoesSaveGameExist(
		RanuraSingleplayer,
		IndiceUsuario
	);
}

bool UReclaimGuardadoSubsystem::PrepararPartidaGuardada()
{
	DatosPendientes = nullptr;

	if (!ExistePartidaGuardada())
	{
		return false;
	}

	USaveGame* ObjetoCargado =
		UGameplayStatics::LoadGameFromSlot(
			RanuraSingleplayer,
			IndiceUsuario
		);

	UReclaimSaveGame* Guardado =
		Cast<UReclaimSaveGame>(ObjetoCargado);

	if (!IsValid(Guardado) || Guardado->VersionGuardado != 1)
	{
		return false;
	}

	DatosPendientes = Guardado;
	return true;
}

UReclaimSaveGame*
UReclaimGuardadoSubsystem::ObtenerDatosPendientes() const
{
	return DatosPendientes.Get();
}

void UReclaimGuardadoSubsystem::LimpiarDatosPendientes()
{
	DatosPendientes = nullptr;
}

