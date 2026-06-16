#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InterfaceColores.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInterfaceColores : public UInterface
{
	GENERATED_BODY()
};

class PRACRICACPP_API IInterfaceColores
{
	GENERATED_BODY()

public:
	// Funcion que envia la informacion del color al character
	// Color: el nombre del color del tile (ej: "Rojo")
	// ColorActivado: si el tile esta activo o no
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Color")
	void InformacionColor(const FString& Color, bool ColorActivado);
};  // <- faltaba este punto y coma