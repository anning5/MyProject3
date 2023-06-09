// Fill out your copyright notice in the Description page of Project Settings.


#include "RelaxedConeSteppingMapGenerator.h"

// Sets default values
ARelaxedConeSteppingMapGenerator::ARelaxedConeSteppingMapGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ARelaxedConeSteppingMapGenerator::OnConstruction(const FTransform& Transform)
{
	if(m_reliefMapRenderingMaterialInstance != nullptr)
	{
		m_reliefMapRenderingMaterialInstance->SetScalarParameterValue(FName(TEXT("DepthScale")), m_depthScale);
	}
	if(!m_generateConeSteppingMap)
	{
		return;
	}

	m_reliefMapRenderingMaterialInstance = nullptr;
	m_generateConeSteppingMap = false;
	m_drawCallIndex = 0;
	m_tempRT = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 256, 256, RTF_RG8);
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

	//Two render tagets are used in the process in which each render target will take turn to be the shader input or the shader output. When a render target is used as a shader input it contains the result from the previous draw call. When a render target is used as a shader output it will store the result of the current draw call.
	//Make sure the last draw call's result is stored in m_renderTargetForConeSteppingMap so that the final result can be referred to by external user always via this variable
	if((m_drawCallCount + (m_remainingTexelCount == 0 ? 0 : 1)) % 2 == 0)
	{
		m_rTs[0] = m_renderTargetForConeSteppingMap;
		m_rTs[1] = m_tempRT;
	}
	else
	{
		m_rTs[0] = m_tempRT;
		m_rTs[1] = m_renderTargetForConeSteppingMap;
	}

	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), m_rTs[0], FLinearColor(FColor(255, 255, 255, 255)));
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), m_rTs[1], FLinearColor(FColor(255, 255, 255, 255)));
	while(m_drawCallIndex < m_drawCallCount)
	{
		if(m_drawCallIndex < m_drawCallCount)
		{
			bool rTSelector = m_drawCallIndex % 2 == 0;
			m_coneSteppingMapGenerationMaterialInstance->SetTextureParameterValue(FName(TEXT("ConeSteppingMap")), m_rTs[static_cast<int>(rTSelector)]);
			//With StartTexelIndex and EndTexelIndex we can process a batch of texels in each draw call
			int startTexelIndex = m_drawCallIndex * m_texelsToProcessPerDrawCall;
			m_coneSteppingMapGenerationMaterialInstance->SetScalarParameterValue(FName(TEXT("StartTexelIndex")), startTexelIndex);
			m_coneSteppingMapGenerationMaterialInstance->SetScalarParameterValue(FName(TEXT("EndTexelIndex")), startTexelIndex + m_texelsToProcessPerDrawCall);
			UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), m_rTs[static_cast<int>(!rTSelector)], m_coneSteppingMapGenerationMaterialInstance);

			m_drawCallIndex++;
			//Process the last draw call
			if(m_drawCallIndex == m_drawCallCount)
			{
				if(m_remainingTexelCount != 0)
				{
					UE_LOG(LogTemp, Display, TEXT("Finished generating the cone stepping map with %d draw calls"), m_drawCallIndex + 1);
					m_coneSteppingMapGenerationMaterialInstance->SetTextureParameterValue(FName(TEXT("ConeSteppingMap")), m_rTs[static_cast<int>(!rTSelector)]);
					startTexelIndex = m_drawCallIndex * m_texelsToProcessPerDrawCall;
					m_coneSteppingMapGenerationMaterialInstance->SetScalarParameterValue(FName(TEXT("StartTexelIndex")), startTexelIndex);
					m_coneSteppingMapGenerationMaterialInstance->SetScalarParameterValue(FName(TEXT("EndTexelIndex")), startTexelIndex + m_remainingTexelCount);
					UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), m_rTs[static_cast<int>(rTSelector)], m_coneSteppingMapGenerationMaterialInstance);
				}
			}
		}
	}

	UStaticMeshComponent* meshComponent = m_actorToRenderReliefMapOn->FindComponentByClass<UStaticMeshComponent>();
	m_reliefMapRenderingMaterialInstance = UMaterialInstanceDynamic::Create(m_materialForReliefMapRendering, nullptr);
	m_reliefMapRenderingMaterialInstance->SetTextureParameterValue(FName(TEXT("ReliefColorMap")), m_reliefColorMap);
	m_reliefMapRenderingMaterialInstance->SetTextureParameterValue(FName(TEXT("RelaxedConeReliefMap")), m_renderTargetForConeSteppingMap);
	m_reliefMapRenderingMaterialInstance->SetScalarParameterValue(FName(TEXT("DepthScale")), m_depthScale);

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

