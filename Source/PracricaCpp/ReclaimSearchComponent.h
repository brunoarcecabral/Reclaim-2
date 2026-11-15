#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimSearchComponent.generated.h"

class AActor;

// Se dispara en el servidor cuando una búsqueda válida termina.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FReclaimBusquedaCompletadaServidor,
	AActor*, Objeto
);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimSearchComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UReclaimSearchComponent();

	// Clases que pueden buscarse. Las configuraremos en el personaje.
	UPROPERTY(EditAnywhere, Category="Reclaim|Busqueda")
	TArray<TSubclassOf<AActor>> ClasesBuscables;

	// Coincide con la duración actual de ambos objetos.
	UPROPERTY(EditDefaultsOnly, Category="Reclaim|Busqueda",
		meta=(ClampMin="0.1"))
	float DuracionBusqueda = 2.0f;

	UFUNCTION(BlueprintCallable, Category="Reclaim|Busqueda")
	void SolicitarInicioBusqueda(AActor* Objeto);

	UFUNCTION(BlueprintCallable, Category="Reclaim|Busqueda")
	void SolicitarCancelarBusqueda();

	// El inventario y el objeto se actualizarán desde este evento
	// en el Blueprint del personaje.
	UPROPERTY(BlueprintAssignable, Category="Reclaim|Busqueda")
	FReclaimBusquedaCompletadaServidor OnBusquedaCompletadaServidor;

protected:
	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
	) override;

private:
	UFUNCTION(Server, Reliable)
	void ServidorIniciarBusqueda(AActor* Objeto);

	UFUNCTION(Server, Reliable)
	void ServidorCancelarBusqueda();

	bool EsClasePermitida(AActor* Objeto) const;
	void CompletarBusquedaEnServidor();
	void LimpiarBusquedaEnServidor();

	TWeakObjectPtr<AActor> ObjetoEnBusqueda;
	FTimerHandle TemporizadorBusqueda;
};

