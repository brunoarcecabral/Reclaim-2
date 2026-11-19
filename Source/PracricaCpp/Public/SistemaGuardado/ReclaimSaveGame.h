#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ReclaimSaveGame.generated.h"

class UStaticMesh;
class UTexture2D;
class UReclaimQuestData;

USTRUCT(BlueprintType)
struct FReclaimItemGuardado
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reclaim|Guardado")
	FText Nombre;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reclaim|Guardado")
	bool bApilable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reclaim|Guardado")
	int32 Cantidad = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reclaim|Guardado")
	TSoftObjectPtr<UTexture2D> Miniatura;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reclaim|Guardado")
	TSoftObjectPtr<UStaticMesh> Malla;
};

USTRUCT(BlueprintType)
struct FReclaimObjetivoGuardado
{
	GENERATED_BODY()

	UPROPERTY()
	FName IdObjetivo = NAME_None;

	UPROPERTY()
	int32 CantidadActual = 0;
};

USTRUCT(BlueprintType)
struct FReclaimMisionGuardada
{
	GENERATED_BODY()
	
	UPROPERTY()
	TSoftObjectPtr<UReclaimQuestData> DatosMision;

	UPROPERTY()
	TArray<FReclaimObjetivoGuardado> Objetivos;

	UPROPERTY()
	bool bCompletada = false;
};

UCLASS()
class PRACRICACPP_API UReclaimSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reclaim|Guardado")
	int32 VersionGuardado = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reclaim|Guardado")
	FVector PosicionJugador = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reclaim|Guardado")
	FRotator RotacionJugador = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reclaim|Guardado")
	TArray<FReclaimItemGuardado> Objetos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reclaim|Guardado")
	int32 Creditos = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reclaim|Guardado")
	int32 Nivel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reclaim|Guardado")
	int32 ExperienciaActual = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reclaim|Guardado")
	int32 ExperienciaRequerida = 100;
	
	UPROPERTY()
	TArray<FReclaimMisionGuardada> Misiones;
	
	UPROPERTY()
	TArray<FName> NotasFinalizadas;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reclaim|Guardado")
	bool bTieneCheckpoint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reclaim|Guardado")
	FTransform TransformCheckpoint = FTransform::Identity;
	
	UPROPERTY()
	TArray<FName> ZonasInicioActivadas;

	UPROPERTY()
	TArray<FName> ZonasObjetivoActivadas;

	UPROPERTY()
	TArray<FName> ZonasCampamentoAbiertas;
	
};
