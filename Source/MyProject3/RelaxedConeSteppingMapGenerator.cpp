// Fill out your copyright notice in the Description page of Project Settings.


#include "RelaxedConeSteppingMapGenerator.h"
#pragma optimize("", off)
// Sets default values
ARelaxedConeSteppingMapGenerator::ARelaxedConeSteppingMapGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ARelaxedConeSteppingMapGenerator::OnConstruction(const FTransform& Transform)
{
	if(!m_generateConeSteppingMap)
	{
		return;
	}

	m_reliefMapRenderingMaterialInstance = nullptr;
	m_generateConeSteppingMap = false;
	m_drawCallIndex = 0;
	m_coneSteppingMapGenerationMaterialInstance = UMaterialInstanceDynamic::Create(m_materialForConeSteppingMapGeneration, nullptr);
	m_coneSteppingMapGenerationMaterialInstance->SetVectorParameterValue(FName(TEXT("TexelSize")), FLinearColor(1.0f / m_reliefBumpMap->GetSizeX(), 1.0f / m_reliefBumpMap->GetSizeY(), m_reliefBumpMap->GetSizeX(), 0));
	m_coneSteppingMapGenerationMaterialInstance->SetTextureParameterValue(FName(TEXT("ReliefBumpMap")), m_reliefBumpMap);

	//Assuming the resulting cone stepping map has a resolution of 256x256, each resulting texel needs to sample all 256 * 256 texels which is of an O(n^2) time complexity, so to speed up the process unlike the original implementation from the GPU Gem paper that only samples one texel per pixel shader which means it will need 256x256 draw calls to finish the process, we instead sample multiple texels in the pixel shader in one draw call which wil reduce the total draw call count
	int texelCountPerRow = m_reliefBumpMap->GetSizeX();
	int rowCount = m_reliefBumpMap->GetSizeY();
	int texelCount = texelCountPerRow * rowCount;
	m_drawCallCount = texelCount / m_texelsToProcessPerDrawCall;
	//The last draw call may not necessarily have the same number of texels to process as previous draw calls
	m_remainingTexelCount = texelCount % m_texelsToProcessPerDrawCall;

	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), m_renderTargetForConeSteppingMap, FLinearColor(FColor(255, 255, 255, 255)));
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), m_renderTargetForConeSteppingMap, FLinearColor(FColor(255, 255, 255, 255)));
	while(m_drawCallIndex < m_drawCallCount)
	{
		m_coneSteppingMapGenerationMaterialInstance->SetTextureParameterValue(FName(TEXT("ConeSteppingMap")), m_renderTargetForConeSteppingMap);
		//With StartTexelIndex and EndTexelIndex we can process a batch of texels in each draw call
		int startTexelIndex = m_drawCallIndex * m_texelsToProcessPerDrawCall;
		m_coneSteppingMapGenerationMaterialInstance->SetScalarParameterValue(FName(TEXT("StartTexelIndex")), startTexelIndex);
		m_coneSteppingMapGenerationMaterialInstance->SetScalarParameterValue(FName(TEXT("EndTexelIndex")), startTexelIndex + m_texelsToProcessPerDrawCall);
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), m_renderTargetForConeSteppingMap, m_coneSteppingMapGenerationMaterialInstance);

		m_drawCallIndex++;
		//Process the last draw call
		if(m_drawCallIndex == m_drawCallCount)
		{
			if(m_remainingTexelCount != 0)
			{
				UE_LOG(LogTemp, Display, TEXT("Finished generating the cone stepping map with %d draw calls"), m_drawCallIndex + 1);
				m_coneSteppingMapGenerationMaterialInstance->SetTextureParameterValue(FName(TEXT("ConeSteppingMap")), m_renderTargetForConeSteppingMap);
				startTexelIndex = m_drawCallIndex * m_texelsToProcessPerDrawCall;
				m_coneSteppingMapGenerationMaterialInstance->SetScalarParameterValue(FName(TEXT("StartTexelIndex")), startTexelIndex);
				m_coneSteppingMapGenerationMaterialInstance->SetScalarParameterValue(FName(TEXT("EndTexelIndex")), startTexelIndex + m_remainingTexelCount);
				UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), m_renderTargetForConeSteppingMap, m_coneSteppingMapGenerationMaterialInstance);
			}
		}
	}

	UStaticMeshComponent* meshComponent = m_actorToRenderReliefMapOn->FindComponentByClass<UStaticMeshComponent>();
	m_reliefMapRenderingMaterialInstance = UMaterialInstanceDynamic::Create(m_materialForReliefMapRendering, nullptr);
	m_reliefMapRenderingMaterialInstance->SetTextureParameterValue(FName(TEXT("ReliefColorMap")), m_reliefColorMap);
	m_reliefMapRenderingMaterialInstance->SetTextureParameterValue(FName(TEXT("ReliefBumpMap")), m_reliefBumpMap);
	m_reliefMapRenderingMaterialInstance->SetTextureParameterValue(FName(TEXT("RelaxedConeReliefMap")), m_renderTargetForConeSteppingMap);

	meshComponent->SetMaterial(0, m_reliefMapRenderingMaterialInstance);
}

// Called when the game starts or when spawned
void ARelaxedConeSteppingMapGenerator::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARelaxedConeSteppingMapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

