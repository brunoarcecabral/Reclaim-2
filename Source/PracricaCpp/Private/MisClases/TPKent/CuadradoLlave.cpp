#include "MisClases/TPKent/CuadradoLlave.h"
#include "GameFramework/Character.h"

ACuadradoLlave::ACuadradoLlave()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACuadradoLlave::BeginPlay()
{
    Super::BeginPlay();
}

void ACuadradoLlave::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ACuadradoLlave::ComunicarPuzzle_Implementation(bool llave)
{
    // PrintString -> imprime MyData.MyString en pantalla
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, MyData.MyString);
    }

    // Branch -> chequea MyData.bMyBool
    if (MyData.bMyBool)
    {
        // Llama la interfaz en el PlayerCharacter con Llave = true
        ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
        if (PlayerCharacter && PlayerCharacter->GetClass()->ImplementsInterface(UComunicacionPuzzle::StaticClass()))
        {
            IComunicacionPuzzle::Execute_ComunicarPuzzle(PlayerCharacter, true);
        }

        // DestroyActor
        Destroy();
    }
}