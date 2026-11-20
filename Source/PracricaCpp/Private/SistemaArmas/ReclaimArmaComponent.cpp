#include "SistemaArmas/ReclaimArmaComponent.h"

#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "SistemaArmas/ReclaimArmaRecogible.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "UI/ReclaimRanurasWidget.h"
#include "Engine/Texture2D.h"


UReclaimArmaComponent::UReclaimArmaComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UReclaimArmaComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UReclaimArmaComponent, IdArma);
	DOREPLIFETIME(UReclaimArmaComponent, bArmaEquipada);
}

bool UReclaimArmaComponent::TieneArma() const
{
	return !IdArma.IsNone();
}

bool UReclaimArmaComponent::EstaEquipada() const
{
	return TieneArma() && bArmaEquipada;
}

FName UReclaimArmaComponent::ObtenerIdArma() const
{
	return IdArma;
}

bool UReclaimArmaComponent::EntregarArma(FName NuevoIdArma)
{
	AActor* Propietario = GetOwner();

	if (!IsValid(Propietario) ||
		!Propietario->HasAuthority() ||
		NuevoIdArma.IsNone() ||
		TieneArma())
	{
		return false;
	}

	IdArma = NuevoIdArma;
	bArmaEquipada = false;

	AlCambiarArma.Broadcast();
	Propietario->ForceNetUpdate();

	return true;
}

void UReclaimArmaComponent::OnRep_Arma()
{
	AlCambiarArma.Broadcast();
}
void UReclaimArmaComponent::AlternarArma()
{
	ServidorAlternarArma();
}

void UReclaimArmaComponent::ServidorAlternarArma_Implementation()
{
	AActor* Propietario = GetOwner();

	if (!IsValid(Propietario) ||
		!Propietario->HasAuthority() ||
		!TieneArma())
	{
		return;
	}

	bArmaEquipada = !bArmaEquipada;

	AlCambiarArma.Broadcast();
	Propietario->ForceNetUpdate();
}
void UReclaimArmaComponent::SolicitarRecogerArma(
	AReclaimArmaRecogible* ArmaRecogible
)
{
	if (!IsValid(ArmaRecogible))
	{
		return;
	}

	ServidorRecogerArma(ArmaRecogible);
}

void UReclaimArmaComponent::ServidorRecogerArma_Implementation(
	AReclaimArmaRecogible* ArmaRecogible
)
{
	AActor* Propietario = GetOwner();

	if (!IsValid(Propietario) ||
		!Propietario->HasAuthority() ||
		!IsValid(ArmaRecogible) ||
		TieneArma())
	{
		return;
	}

	const float DistanciaMaxima = 250.0f;

	const float DistanciaCuadrada = FVector::DistSquared(
		Propietario->GetActorLocation(),
		ArmaRecogible->GetActorLocation()
	);

	if (DistanciaCuadrada > FMath::Square(DistanciaMaxima))
	{
		return;
	}

	ArmaRecogible->EntregarAlJugador(this);
}
AReclaimArmaRecogible* UReclaimArmaComponent::BuscarArmaCercana() const
{
	AActor* Propietario = GetOwner();
	UWorld* Mundo = GetWorld();

	if (!IsValid(Propietario) || !Mundo || TieneArma())
	{
		return nullptr;
	}

	const float DistanciaMaxima = 250.0f;
	float MejorDistanciaCuadrada = FMath::Square(DistanciaMaxima);

	AReclaimArmaRecogible* ArmaMasCercana = nullptr;

	for (TActorIterator<AReclaimArmaRecogible> It(Mundo); It; ++It)
	{
		AReclaimArmaRecogible* Arma = *It;

		if (!IsValid(Arma) || !Arma->EstaDisponibleEnMision())
		{
			continue;
		}
		
		const float DistanciaCuadrada = FVector::DistSquared(
			Propietario->GetActorLocation(),
			Arma->GetActorLocation()
		);

		if (DistanciaCuadrada <= MejorDistanciaCuadrada)
		{
			MejorDistanciaCuadrada = DistanciaCuadrada;
			ArmaMasCercana = Arma;
		}
	}

	return ArmaMasCercana;
}

void UReclaimArmaComponent::RecogerArmaCercana()
{
	AReclaimArmaRecogible* Arma = BuscarArmaCercana();

	if (IsValid(Arma))
	{
		SolicitarRecogerArma(Arma);
	}
}
void UReclaimArmaComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* Mundo = GetWorld())
	{
		Mundo->GetTimerManager().SetTimer(
			TimerCrearRanuras,
			this,
			&UReclaimArmaComponent::IntentarCrearRanuras,
			0.25f,
			true
		);
	}
}

void UReclaimArmaComponent::EndPlay(
	const EEndPlayReason::Type EndPlayReason
)
{
	if (UWorld* Mundo = GetWorld())
	{
		Mundo->GetTimerManager().ClearTimer(TimerCrearRanuras);
	}

	if (IsValid(WidgetRanuras))
	{
		WidgetRanuras->RemoveFromParent();
		WidgetRanuras = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void UReclaimArmaComponent::IntentarCrearRanuras()
{
	APawn* Personaje = Cast<APawn>(GetOwner());

	if (!IsValid(Personaje) || !Personaje->IsLocallyControlled())
	{
		return;
	}

	APlayerController* Controlador =
		Cast<APlayerController>(Personaje->GetController());

	if (!IsValid(Controlador))
	{
		return;
	}

	WidgetRanuras = CreateWidget<UReclaimRanurasWidget>(
		Controlador,
		UReclaimRanurasWidget::StaticClass()
	);

	if (!IsValid(WidgetRanuras))
	{
		return;
	}

	WidgetRanuras->AddToViewport();

	if (UWorld* Mundo = GetWorld())
	{
		Mundo->GetTimerManager().ClearTimer(TimerCrearRanuras);
	}
}
float UReclaimArmaComponent::CalcularDanioConArma(float DanioBase) const
{
	if (EstaEquipada())
	{
		return DanioBase * 1.25f;
	}

	return DanioBase;
}
void UReclaimArmaComponent::RestaurarArmaGuardada(
	FName IdGuardado,
	bool bEstabaEquipada
)
{
	AActor* Propietario = GetOwner();
	UWorld* Mundo = GetWorld();

	if (!IsValid(Propietario) ||
		!Propietario->HasAuthority() ||
		!Mundo ||
		Mundo->GetNetMode() != NM_Standalone)
	{
		return;
	}

	IdArma = IdGuardado;
	
	bArmaEquipada = !IdArma.IsNone() && bEstabaEquipada;

	AlCambiarArma.Broadcast();
	Propietario->ForceNetUpdate();
}
UTexture2D* UReclaimArmaComponent::ObtenerIconoArma() const
{
	const TObjectPtr<UTexture2D>* Icono = IconosArmas.Find(IdArma);

	return Icono ? Icono->Get() : nullptr;
}



