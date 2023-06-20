// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/Canvas.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "RelaxedConeSteppingMapGenerator.generated.h"

UCLASS()
class MYPROJECT3_API ARelaxedConeSteppingMapGenerator : public AActor
{
	GENERATED_BODY()

	UTextureRenderTarget2D* m_tempRT = nullptr;
	UMaterialInstanceDynamic* m_coneSteppingMapGenerationMaterialInstance = nullptr;
	UMaterialInstanceDynamic* m_reliefMapRenderingMaterialInstance = nullptr;
	UTextureRenderTarget2D* m_rTs[2] = {nullptr, nullptr};
	int m_drawCallCount = 0;
	int m_drawCallIndex = 0;
	int m_remainingTexelCount = 0;
	void OnConstruction(const FTransform& Transform);
	void CreateMaterialInstance();

public:	
	UPROPERTY(EditAnywhere)
	bool m_generateConeSteppingMap = false;
	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* m_renderTargetForConeSteppingMap;
	UPROPERTY(EditAnywhere)
	UTexture2D* m_reliefBumpMap;
	UPROPERTY(EditAnywhere)
	UTexture2D* m_reliefColorMap;
	UPROPERTY(EditAnywhere)
	UMaterial* m_materialForConeSteppingMapGeneration;
	UPROPERTY(EditAnywhere)
	UMaterial* m_materialForReliefMapRendering;
	UPROPERTY(EditAnywhere)
	AActor* m_actorToRenderReliefMapOn;
	UPROPERTY(EditAnywhere, Blueprintable)
	AActor* m_directinalLightActor;
	//It is ideal to keep it below 30 to prevent crashing the UE editor
	UPROPERTY(EditAnywhere)
	int m_texelsToProcessPerDrawCall = 20;
	UPROPERTY(EditAnywhere, meta=(ClampMin="0", ClampMax="1"))
	float m_depthScale = .5f;

	// Sets default values for this actor's properties
	ARelaxedConeSteppingMapGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
