#include "ReclaimEnemigoBase.h"

#include "AIController.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "Navigation/PathFollowingComponent.h"
#include "Net/UnrealNetwork.h"
#include "PracricaCppCharacter.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "Sound/SoundBase.h"
#include "SistemaNivel/ReclaimNivelComponent.h"


AReclaimEnemigoBase::AReclaimEnemigoBase()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(true);

    AIControllerClass = AAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AReclaimEnemigoBase::BeginPlay()
{
    Super::BeginPlay();

    if (!HasAuthority())
    {
        return;
    }

    SaludMaxima = FMath::Max(1.0f, SaludMaxima);
    SaludActual = SaludMaxima;

    GetCharacterMovement()->MaxWalkSpeed = VelocidadPersecucion;
    PuntoInicialPatrulla = GetActorLocation();

    GetWorldTimerManager().SetTimer(
        TimerRevisionObjetivos,
        this,
        &AReclaimEnemigoBase::RevisarObjetivos,
        0.25f,
        true
    );
}

void AReclaimEnemigoBase::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    GetWorldTimerManager().ClearTimer(TimerRevisionObjetivos);

    Super::EndPlay(EndPlayReason);
}

bool AReclaimEnemigoBase::JugadorDisponible(
    APracricaCppCharacter* Jugador
) const
{
    if (!IsValid(Jugador))
    {
        return false;
    }

    const UCharacterMovementComponent* Movimiento =
        Jugador->GetCharacterMovement();

    // La muerte del jugador desactiva su movimiento.
    return Movimiento && Movimiento->MovementMode != MOVE_None;
}

bool AReclaimEnemigoBase::PuedeDetectarJugador(
    APracricaCppCharacter* Jugador
) const
{
    if (!JugadorDisponible(Jugador))
    {
        return false;
    }

    const FVector HaciaJugador =
        Jugador->GetActorLocation() - GetActorLocation();

    if (HaciaJugador.SizeSquared() >
        FMath::Square(DistanciaVision))
    {
        return false;
    }

    const FVector Direccion = HaciaJugador.GetSafeNormal2D();

    const float ProductoPunto = FVector::DotProduct(
        GetActorForwardVector().GetSafeNormal2D(),
        Direccion
    );

    const float AnguloMedio = FMath::DegreesToRadians(
        FMath::Clamp(AnguloVision, 0.0f, 360.0f) * 0.5f
    );

    if (ProductoPunto < FMath::Cos(AnguloMedio))
    {
        return false;
    }

    FHitResult Impacto;
    FCollisionQueryParams Parametros;
    Parametros.AddIgnoredActor(this);

    const FVector Inicio = GetActorLocation();
    const FVector Fin = Jugador->GetActorLocation();

    const bool bImpacto = GetWorld()->LineTraceSingleByChannel(
        Impacto,
        Inicio,
        Fin,
        ECC_Visibility,
        Parametros
    );

    return !bImpacto || Impacto.GetActor() == Jugador;
}

void AReclaimEnemigoBase::RevisarObjetivos()
{
    if (!HasAuthority() || bMuerto)
    {
        return;
    }

    AAIController* Controlador = Cast<AAIController>(GetController());

    if (!Controlador)
    {
        return;
    }

    APracricaCppCharacter* MejorObjetivo = nullptr;
    float MejorDistancia = TNumericLimits<float>::Max();

    for (
        FConstPlayerControllerIterator It =
            GetWorld()->GetPlayerControllerIterator();
        It;
        ++It
    )
    {
        APlayerController* ControladorJugador = It->Get();

        if (!IsValid(ControladorJugador))
        {
            continue;
        }

        APracricaCppCharacter* Jugador =
            Cast<APracricaCppCharacter>(
                ControladorJugador->GetPawn()
            );

        if (!PuedeDetectarJugador(Jugador))
        {
            continue;
        }

        const float Distancia = FVector::DistSquared(
            GetActorLocation(),
            Jugador->GetActorLocation()
        );

        if (Distancia < MejorDistancia)
        {
            MejorDistancia = Distancia;
            MejorObjetivo = Jugador;
        }
    }
    
    if (JugadorDisponible(UltimoAtacante.Get()) &&
        FVector::DistSquared(
            GetActorLocation(),
            UltimoAtacante->GetActorLocation()
        ) <= FMath::Square(DistanciaVision))
    {
        MejorObjetivo = UltimoAtacante.Get();
    }
    else
    {
        UltimoAtacante = nullptr;
    }

    if (!MejorObjetivo)
    {
        if (ObjetivoActual)
        {
            Controlador->StopMovement();
            Controlador->ClearFocus(EAIFocusPriority::Gameplay);
            ObjetivoActual = nullptr;
        }

        Patrullar(Controlador);
        return;
    }
    

    if (ObjetivoActual != MejorObjetivo ||
        Controlador->GetMoveStatus() == EPathFollowingStatus::Idle)
    {
        ObjetivoActual = MejorObjetivo;

        Controlador->SetFocus(ObjetivoActual);

        const auto ResultadoMovimiento = Controlador->MoveToActor(
            ObjetivoActual,
            DistanciaDetencion
        );

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("IA Enemigo=%s Objetivo=%s Distancia=%.0f Resultado=%d"),
            *GetNameSafe(this),
            *GetNameSafe(ObjetivoActual.Get()),
            FVector::Dist2D(
                GetActorLocation(),
                ObjetivoActual->GetActorLocation()
            ),
            static_cast<int32>(ResultadoMovimiento)
        );
    }

    IntentarDisparar(MejorObjetivo);
}

void AReclaimEnemigoBase::IntentarDisparar(
    APracricaCppCharacter* Jugador
)
{
    if (!HasAuthority() || bMuerto || !JugadorDisponible(Jugador))
    {
        return;
    }

    const float Distancia = FVector::Dist(
        GetActorLocation(),
        Jugador->GetActorLocation()
    );

    if (Distancia > AlcanceDisparo)
    {
        return;
    }

    const float TiempoActual = GetWorld()->GetTimeSeconds();

    if (TiempoActual - UltimoDisparo < TiempoEntreDisparos)
    {
        return;
    }

    const FVector Inicio =
        GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

    const FVector FinDeseado = Jugador->GetActorLocation();

    FHitResult Impacto;
    FCollisionQueryParams Parametros;
    Parametros.AddIgnoredActor(this);

    const bool bImpacto = GetWorld()->LineTraceSingleByChannel(
        Impacto,
        Inicio,
        FinDeseado,
        ECC_Visibility,
        Parametros
    );

    // Una pared u otro objeto impide el disparo.
    if (bImpacto && Impacto.GetActor() != Jugador)
    {
        return;
    }

    UltimoDisparo = TiempoActual;

    const FVector FinLaser = bImpacto
        ? Impacto.ImpactPoint
        : FinDeseado;

    UGameplayStatics::ApplyDamage(
        Jugador,
        DanioDisparo,
        GetController(),
        this,
        UDamageType::StaticClass()
    );

    MulticastMostrarLaser(Inicio, FinLaser);
}

void AReclaimEnemigoBase::MulticastMostrarLaser_Implementation(
    FVector Inicio,
    FVector Fin
)
{
    UStaticMesh* MallaLaser = LoadObject<UStaticMesh>(
        nullptr,
        TEXT("/Engine/BasicShapes/Cube.Cube")
    );

    if (!MallaLaser)
    {
        return;
    }

    const FVector Diferencia = Fin - Inicio;
    const float Longitud = Diferencia.Size();

    if (Longitud <= KINDA_SMALL_NUMBER)
    {
        return;
    }

    UStaticMeshComponent* Laser =
        NewObject<UStaticMeshComponent>(this);

    if (!Laser)
    {
        return;
    }

    Laser->SetStaticMesh(MallaLaser);
    Laser->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Laser->SetMobility(EComponentMobility::Movable);

    if (MaterialLaser)
    {
        Laser->SetMaterial(0, MaterialLaser);
    }

    Laser->RegisterComponent();

    Laser->SetWorldLocation((Inicio + Fin) * 0.5f);
    Laser->SetWorldRotation(Diferencia.Rotation());

    // El cubo básico de Unreal mide 100 unidades por lado.
    Laser->SetWorldScale3D(
        FVector(Longitud / 100.0f, 0.04f, 0.04f)
    );

    TWeakObjectPtr<UStaticMeshComponent> LaserDebil(Laser);
    FTimerHandle TimerLaser;

    GetWorldTimerManager().SetTimer(
        TimerLaser,
        FTimerDelegate::CreateLambda([LaserDebil]()
        {
            if (LaserDebil.IsValid())
            {
                LaserDebil->DestroyComponent();
            }
        }),
        0.15f,
        false
    );
}

float AReclaimEnemigoBase::TakeDamage(
    float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser
)
{
    if (!HasAuthority() || bMuerto || DamageAmount <= 0.0f)
    {
        return 0.0f;
    }

    const float DamageAplicado = Super::TakeDamage(
        DamageAmount,
        DamageEvent,
        EventInstigator,
        DamageCauser
    );

    if (DamageAplicado <= 0.0f)
    {
        return 0.0f;
    }
    
    APracricaCppCharacter* Atacante =
    Cast<APracricaCppCharacter>(DamageCauser);

    if (JugadorDisponible(Atacante))
    {
        UltimoAtacante = Atacante;
    }
    
    MulticastSonidoImpacto();
    SaludActual = FMath::Clamp(
        SaludActual - DamageAplicado,
        0.0f,
        SaludMaxima
    );

    AlCambiarSalud(SaludActual, SaludMaxima);

    if (SaludActual <= 0.0f)
    {
        bMuerto = true;
        
        APawn* PersonajeAtacante = Cast<APawn>(DamageCauser);
        
        if (!IsValid(PersonajeAtacante) && IsValid(DamageCauser))
        {
            PersonajeAtacante = DamageCauser->GetInstigator();
        }
        
        if (!IsValid(PersonajeAtacante) && IsValid(EventInstigator))
        {
            PersonajeAtacante = EventInstigator->GetPawn();
        }

        if (IsValid(PersonajeAtacante) &&
            PersonajeAtacante->IsPlayerControlled())
        {
            UReclaimNivelComponent* ComponenteNivel =
                PersonajeAtacante->FindComponentByClass<UReclaimNivelComponent>();

            if (IsValid(ComponenteNivel))
            {
                ComponenteNivel->AgregarExperiencia(15);
            }
        }

        Destroy();
    }



    return DamageAplicado;
}

void AReclaimEnemigoBase::OnRep_SaludActual()
{
    AlCambiarSalud(SaludActual, SaludMaxima);
}

void AReclaimEnemigoBase::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AReclaimEnemigoBase, SaludMaxima);
    DOREPLIFETIME(AReclaimEnemigoBase, SaludActual);
}
void AReclaimEnemigoBase::Patrullar(AAIController* Controlador)
{
    if (!HasAuthority() || !PatrullaActivada || !IsValid(Controlador))
    {
        return;
    }

    // Si ya está caminando hacia un punto, lo dejamos continuar.
    if (Controlador->GetMoveStatus() != EPathFollowingStatus::Idle)
    {
        return;
    }

    const float TiempoActual = GetWorld()->GetTimeSeconds();

    if (TiempoActual < ProximoIntentoPatrulla)
    {
        return;
    }

    ProximoIntentoPatrulla =
        TiempoActual + FMath::Max(0.5f, EsperaEntrePatrullas);

    UNavigationSystemV1* Navegacion =
        FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

    if (!Navegacion)
    {
        return;
    }

    FNavLocation PuntoDestino;

    const bool bEncontroPunto =
        Navegacion->GetRandomReachablePointInRadius(
            PuntoInicialPatrulla,
            RadioPatrulla,
            PuntoDestino
        );

    if (bEncontroPunto)
    {
        Controlador->MoveToLocation(PuntoDestino.Location);
    }
}
void AReclaimEnemigoBase::MulticastSonidoImpacto_Implementation()
{
    if (SonidoImpacto)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            SonidoImpacto,
            GetActorLocation()
        );
    }
}



