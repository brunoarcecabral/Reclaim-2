#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReclaimIndicadorTutorialWidget.generated.h"


UCLASS()
class PRACRICACPP_API UReclaimIndicadorTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;
};

