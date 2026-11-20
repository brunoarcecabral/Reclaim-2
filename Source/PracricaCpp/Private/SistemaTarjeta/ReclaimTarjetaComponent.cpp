#include "SistemaTarjeta/ReclaimTarjetaComponent.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "SistemaTarjeta/ReclaimPuertaSeguridad.h"
#include "SistemaTarjeta/ReclaimTarjetaRecogible.h"

UReclaimTarjetaComponent::UReclaimTarjetaComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UReclaimTarjetaComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UReclaimTarjetaComponent, bTieneTarjeta);
}

bool UReclaimTarjetaComponent::TieneTarjeta() const
{
    return bTieneTarjeta;
}

bool UReclaimTarjetaComponent::RecibirTarjeta()
{
    AActor* Propietario = GetOwner();

    if (!IsValid(Propietario) ||
        !Propietario->HasAuthority() ||
        bTieneTarjeta)
    {
        return false;
    }

    bTieneTarjeta = true;

    AlCambiarTarjeta.Broadcast();
    Propietario->ForceNetUpdate();

    return true;
}

bool UReclaimTarjetaComponent::ConsumirTarjeta()
{
    AActor* Propietario = GetOwner();

    if (!IsValid(Propietario) ||
        !Propietario->HasAuthority() ||
        !bTieneTarjeta)
    {
        return false;
    }

    bTieneTarjeta = false;

    AlCambiarTarjeta.Broadcast();
    Propietario->ForceNetUpdate();

    return true;
}

void UReclaimTarjetaComponent::OnRep_Tarjeta()
{
    AlCambiarTarjeta.Broadcast();
}

AReclaimTarjetaRecogible*
UReclaimTarjetaComponent::BuscarTarjetaCercana() const
{
    AActor* Propietario = GetOwner();
    UWorld* Mundo = GetWorld();

    if (!IsValid(Propietario) || !Mundo || TieneTarjeta())
    {
        return nullptr;
    }

    const float DistanciaMaxima = 250.0f;
    float MejorDistanciaCuadrada = FMath::Square(DistanciaMaxima);

    AReclaimTarjetaRecogible* TarjetaMasCercana = nullptr;

    for (TActorIterator<AReclaimTarjetaRecogible> It(Mundo); It; ++It)
    {
        AReclaimTarjetaRecogible* Tarjeta = *It;

        if (!IsValid(Tarjeta) || !Tarjeta->EstaDisponibleParaRecoger())
        {
            continue;
        }

        const float DistanciaCuadrada = FVector::DistSquared(
            Propietario->GetActorLocation(),
            Tarjeta->GetActorLocation()
        );

        if (DistanciaCuadrada <= MejorDistanciaCuadrada)
        {
            MejorDistanciaCuadrada = DistanciaCuadrada;
            TarjetaMasCercana = Tarjeta;
        }
    }

    return TarjetaMasCercana;
}

void UReclaimTarjetaComponent::SolicitarTarjetaCercana()
{
    AReclaimTarjetaRecogible* Tarjeta = BuscarTarjetaCercana();

    if (IsValid(Tarjeta))
    {
        ServidorRecogerTarjeta(Tarjeta);
    }
}

void UReclaimTarjetaComponent::ServidorRecogerTarjeta_Implementation(
    AReclaimTarjetaRecogible* Tarjeta
)
{
    AActor* Propietario = GetOwner();

    if (!IsValid(Propietario) ||
        !Propietario->HasAuthority() ||
        !IsValid(Tarjeta) ||
        Tarjeta->EstaRecogida() ||
        TieneTarjeta())
    {
        return;
    }

    const float DistanciaMaxima = 250.0f;

    const float DistanciaCuadrada = FVector::DistSquared(
        Propietario->GetActorLocation(),
        Tarjeta->GetActorLocation()
    );

    if (DistanciaCuadrada > FMath::Square(DistanciaMaxima))
    {
        return;
    }

    Tarjeta->EntregarAlJugador(this);
}

AReclaimPuertaSeguridad*
UReclaimTarjetaComponent::BuscarPuertaCercana() const
{
    AActor* Propietario = GetOwner();
    UWorld* Mundo = GetWorld();

    if (!IsValid(Propietario) || !Mundo)
    {
        return nullptr;
    }

    const float DistanciaMaxima = 250.0f;
    float MejorDistanciaCuadrada = FMath::Square(DistanciaMaxima);

    AReclaimPuertaSeguridad* PuertaMasCercana = nullptr;

    for (TActorIterator<AReclaimPuertaSeguridad> It(Mundo); It; ++It)
    {
        AReclaimPuertaSeguridad* Puerta = *It;

        if (!IsValid(Puerta) || Puerta->EstaAbierta())
        {
            continue;
        }

        const float DistanciaCuadrada = FVector::DistSquared(
            Propietario->GetActorLocation(),
            Puerta->GetActorLocation()
        );

        if (DistanciaCuadrada <= MejorDistanciaCuadrada)
        {
            MejorDistanciaCuadrada = DistanciaCuadrada;
            PuertaMasCercana = Puerta;
        }
    }

    return PuertaMasCercana;
}

void UReclaimTarjetaComponent::SolicitarAbrirPuertaCercana()
{
    AReclaimPuertaSeguridad* Puerta = BuscarPuertaCercana();

    if (IsValid(Puerta))
    {
        ServidorAbrirPuerta(Puerta);
    }
}

void UReclaimTarjetaComponent::ServidorAbrirPuerta_Implementation(
    AReclaimPuertaSeguridad* Puerta
)
{
    AActor* Propietario = GetOwner();

    if (!IsValid(Propietario) ||
        !Propietario->HasAuthority() ||
        !IsValid(Puerta) ||
        Puerta->EstaAbierta())
    {
        return;
    }

    const float DistanciaMaxima = 250.0f;

    const float DistanciaCuadrada = FVector::DistSquared(
        Propietario->GetActorLocation(),
        Puerta->GetActorLocation()
    );

    if (DistanciaCuadrada > FMath::Square(DistanciaMaxima))
    {
        return;
    }

    if (!TieneTarjeta())
    {
        ClienteRecibirMensajePuerta(
            FText::FromString(TEXT("Necesitás una tarjeta de acceso."))
        );

        return;
    }

    if (Puerta->AbrirConTarjeta(this))
    {
        ClienteRecibirMensajePuerta(
            FText::FromString(TEXT("Puerta de seguridad abierta."))
        );
    }
}

void UReclaimTarjetaComponent::RestaurarTarjetaGuardada(bool bGuardada)
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

    bTieneTarjeta = bGuardada;

    AlCambiarTarjeta.Broadcast();
    Propietario->ForceNetUpdate();
}

void UReclaimTarjetaComponent::ClienteRecibirMensajePuerta_Implementation(
    const FText& Mensaje
)
{
    MostrarMensajePuerta(Mensaje);
}
