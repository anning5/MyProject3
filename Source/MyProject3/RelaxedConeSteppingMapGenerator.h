// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/Canvas.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "RelaxedConeSteppingMapGenerator.generated.h"

UCLASS()
class MYPROJECT3_API ARelaxedConeSteppingMapGenerator : public AActor
{
	GENERATED_BODY()

	FDrawToRenderTargetContext m_context;
	FVector2D m_size;
	FTimerHandle m_timerHandle;
	UTextureRenderTarget2D* m_tempRT = nullptr;
	UMaterialInstanceDynamic* m_materialInstance = nullptr;
	UTextureRenderTarget2D* m_rTs[2] = {nullptr, nullptr};
	UCanvas *m_canvas = nullptr;
	int m_drawCallCount = 0;
	int m_drawCallIndex = 0;
	int m_remainingTexelCount = 0;
	void OnConstruction(const FTransform& Transform);

public:	
	UPROPERTY(EditAnywhere)
	bool m_generate = false;
	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* m_textureRenderTarget;
	UPROPERTY(EditAnywhere)
	UTexture2D* m_reliefMap;
	UPROPERTY(EditAnywhere)
	UMaterialInterface* m_material;
	UPROPERTY(EditAnywhere)
	int m_texelsToProcessPerDrawCall = 20;

	// Sets default values for this actor's properties
	ARelaxedConeSteppingMapGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
