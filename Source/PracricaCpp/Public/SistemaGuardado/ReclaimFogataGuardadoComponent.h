#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimFogataGuardadoComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReclaimFogataRestaurada);

UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimFogataGuardadoComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:

	UReclaimFogataGuardadoComponent();
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Reclaim|Guardado")
	FName IdGuardado = NAME_None;
	
	UFUNCTION(BlueprintCallable, Category = "Reclaim|Guardado")
	void MarcarActivada();

	bool EstaActivada() const;
	bool RestaurarActivada();
	
	UPROPERTY(BlueprintAssignable, Category = "Reclaim|Guardado")
	FReclaimFogataRestaurada AlRestaurarActivada;

private:

	bool bActivada = false;
};

