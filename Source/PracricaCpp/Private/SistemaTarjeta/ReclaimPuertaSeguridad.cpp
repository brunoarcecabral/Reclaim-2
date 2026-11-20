#include "SistemaTarjeta/ReclaimPuertaSeguridad.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "SistemaTarjeta/ReclaimTarjetaComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "SistemaQuest/ReclaimQuestComponent.h"
#include "SistemaQuest/ReclaimQuestData.h"



AReclaimPuertaSeguridad::AReclaimPuertaSeguridad()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	Raiz = CreateDefaultSubobject<USceneComponent>(TEXT("Raiz"));
	SetRootComponent(Raiz);

	ModeloPuerta =
		CreateDefaultSubobject<UStaticMeshComponent>(
			TEXT("ModeloPuerta")
		);

	ModeloPuerta->SetupAttachment(Raiz);
}

void AReclaimPuertaSeguridad::BeginPlay()
{
	Super::BeginPlay();

	PosicionCerrada = ModeloPuerta->GetRelativeLocation();
}

void AReclaimPuertaSeguridad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bAbierta)
	{
		return;
	}

	const FVector PosicionAbierta =
		PosicionCerrada + DesplazamientoApertura;

	const FVector NuevaPosicion = FMath::VInterpConstantTo(
		ModeloPuerta->GetRelativeLocation(),
		PosicionAbierta,
		DeltaTime,
		VelocidadApertura
	);

	ModeloPuerta->SetRelativeLocation(NuevaPosicion);
}

void AReclaimPuertaSeguridad::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AReclaimPuertaSeguridad, bAbierta);
}

bool AReclaimPuertaSeguridad::EstaAbierta() const
{
	return bAbierta;
}

bool AReclaimPuertaSeguridad::AbrirConTarjeta(
	UReclaimTarjetaComponent* TarjetaJugador
)
{
	if (!HasAuthority() ||
		bAbierta ||
		!IsValid(TarjetaJugador) ||
		!TarjetaJugador->TieneTarjeta())
	{
		return false;
	}

	UWorld* Mundo = GetWorld();

	AGameStateBase* EstadoJuego = Mundo
		? Mundo->GetGameState()
		: nullptr;

	UReclaimQuestComponent* Quest = IsValid(EstadoJuego)
		? EstadoJuego->FindComponentByClass<UReclaimQuestComponent>()
		: nullptr;

	if (!IsValid(Quest) || !IsValid(MisionEntrarSala))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("PUERTA: Falta el componente de misiones o asignar la mision de entrar a la sala.")
		);

		return false;
	}
	
	if (!TarjetaJugador->ConsumirTarjeta())
	{
		return false;
	}

	const bool bObjetivoAvanzado = Quest->AvanzarObjetivo(
		FName(TEXT("Mision_AbrirPuertaSeguridad")),
		FName(TEXT("Abrir_PuertaSeguridad")),
		1
	);

	if (!bObjetivoAvanzado)
	{
		
		TarjetaJugador->RecibirTarjeta();

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("PUERTA: No se pudo avanzar el objetivo de abrir la puerta.")
		);

		return false;
	}
	if (!Quest->IniciarMision(MisionEntrarSala))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("PUERTA: Se completo la mision de abrir, pero no se pudo iniciar la mision de entrar a la sala.")
		);
	}
	
	bAbierta = true;
	ForceNetUpdate();

	return true;
}

void AReclaimPuertaSeguridad::RestaurarPuertaAbierta(bool bGuardada)
{
	UWorld* Mundo = GetWorld();

	if (!HasAuthority() ||
		!Mundo ||
		Mundo->GetNetMode() != NM_Standalone)
	{
		return;
	}

	bAbierta = bGuardada;

	ModeloPuerta->SetRelativeLocation(
		bAbierta
			? PosicionCerrada + DesplazamientoApertura
			: PosicionCerrada
	);

	ForceNetUpdate();
}
