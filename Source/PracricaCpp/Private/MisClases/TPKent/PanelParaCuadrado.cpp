#include "MisClases/TPKent/PanelParaCuadrado.h"
#include "Kismet/GameplayStatics.h"

APanelParaCuadrado::APanelParaCuadrado()
{
    PrimaryActorTick.bCanEverTick = true;
}

void APanelParaCuadrado::BeginPlay()
{
    Super::BeginPlay();
}

void APanelParaCuadrado::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APanelParaCuadrado::ComunicarPuzzle_Implementation(bool llave)
{
    // Mensaje de debug
    if (GEngine)
    {
        FString Mensaje = llave ? TEXT("Acceso concedido") : TEXT("Se necesita llave");
        GEngine->AddOnScreenDebugMessage(-1, 5.f, llave ? FColor::Green : FColor::Red, Mensaje);
    }

    // Branch 1: si no tiene llave, corta acá (ya mostró el mensaje)
    if (!llave) return;

    // Branch 2: chequea si es la primera vez
    if (!PrimeraVe) return;

    // SpawnActor en SpawnTransformLocatio con escala 0.2
    if (Clas)
    {
        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SpawnTransformLocatio);

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Clas, SpawnTransform, SpawnParams);
        if (SpawnedActor)
        {
            SpawnedActor->SetActorScale3D(FVector(0.2f, 0.2f, 0.2f));
        }

        // GetAllActorsOfClass y llama la interfaz en cada uno con Llave = true
        TArray<AActor*> OutActors;
        UGameplayStatics::GetAllActorsOfClass(this, Clas, OutActors);

        // Set PrimeraVe = false
        PrimeraVe = false;

        for (AActor* Actor : OutActors)
        {
            if (Actor && Actor->GetClass()->ImplementsInterface(UComunicacionPuzzle::StaticClass()))
            {
                IComunicacionPuzzle::Execute_ComunicarPuzzle(Actor, true);
            }
        }
    }

    // CreateWidget y AddToViewport
    if (WidgetClas)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (PC)
        {
            UUserWidget* Widget = CreateWidget<UUserWidget>(PC, WidgetClas);
            if (Widget)
            {
                Widget->AddToViewport();
            }
        }
    }
}