#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ReclaimGuardadoSubsystem.generated.h"

class UReclaimSaveGame;

UCLASS()
class PRACRICACPP_API UReclaimGuardadoSubsystem
	: public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	UReclaimSaveGame* CrearDatosGuardado() const;
	
	bool GuardarPartida(UReclaimSaveGame* DatosGuardado) const;
	
	bool ExistePartidaGuardada() const;
	
	bool PrepararPartidaGuardada();
	
	UReclaimSaveGame* ObtenerDatosPendientes() const;
	
	void LimpiarDatosPendientes();

private:

	UPROPERTY()
	TObjectPtr<UReclaimSaveGame> DatosPendientes;
};

