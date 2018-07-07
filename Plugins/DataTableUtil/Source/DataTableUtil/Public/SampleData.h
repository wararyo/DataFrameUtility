#pragma once

#include "Object.h"
#include "SampleData.generated.h"

////////////////////////////////////////////////////////////////////////////////
// サンプルデータ基底

UCLASS()
class DATATABLEUTIL_API USampleData : public UObject
{
	GENERATED_BODY()

public:
	// 設定クラス取得
	virtual UClass* GetSettingClass() const { return nullptr; }
};


////////////////////////////////////////////////////////////////////////////////
// サンプルデータA

UCLASS()
class DATATABLEUTIL_API USampleDataSettingA : public UObject
{
	GENERATED_BODY()

public:
	int32 PropertyA;
};

UCLASS()
class DATATABLEUTIL_API USampleDataA : public USampleData
{
	GENERATED_BODY()

		virtual void PostInitProperties() override
	{
		Super::PostInitProperties();

		if (!HasAnyFlags(RF_ClassDefaultObject))
		{
			Setting = NewObject<USampleDataSettingA>(GetTransientPackage(), USampleDataSettingA::StaticClass());
		}
	}

public:
	// 設定コピー関数
	void CopySettingA(const USampleDataSettingA& InSetting)
	{
		UE_LOG(LogTemp, Log, TEXT("******* Copy SettingA"));
		Setting->PropertyA = InSetting.PropertyA;
	}

	// 静的設定コピー関数（IntermediateNodeによる呼び出し用）
	UFUNCTION(BlueprintCallable, Category = "Sample", meta = (BlueprintInternalUseOnly = "true"))
		static USampleDataA* CopySettingA_Static(USampleDataA* Target, const USampleDataSettingA* Setting)
	{
		if (Target == nullptr || Setting == nullptr)
		{
			return Target;
		}

		Target->CopySettingA(*Setting);

		return Target;
	}

	// 設定クラス取得
	virtual UClass* GetSettingClass() const override
	{
		return USampleDataSettingA::StaticClass();
	}

private:
	UPROPERTY()
		USampleDataSettingA* Setting;
};


////////////////////////////////////////////////////////////////////////////////
// サンプルデータB

UCLASS()
class DATATABLEUTIL_API USampleDataSettingB : public UObject
{
	GENERATED_BODY()

public:
	float PropertyB;
};

UCLASS()
class DATATABLEUTIL_API USampleDataB : public USampleData
{
	GENERATED_BODY()

		virtual void PostInitProperties() override
	{
		Super::PostInitProperties();

		if (!HasAnyFlags(RF_ClassDefaultObject))
		{
			Setting = NewObject<USampleDataSettingB>(GetTransientPackage(), USampleDataSettingB::StaticClass());
		}
	}

public:
	// 設定コピー関数
	void CopySettingB(const USampleDataSettingB& InSetting)
	{
		UE_LOG(LogTemp, Log, TEXT("******* Copy SettingB"));
		Setting->PropertyB = InSetting.PropertyB;
	}

	// 静的設定コピー関数（IntermediateNodeによる呼び出し用）
	UFUNCTION(BlueprintCallable, Category = "Sample", meta = (BlueprintInternalUseOnly = "true"))
		static USampleDataB* CopySettingB_Static(USampleDataB* Target, const USampleDataSettingB* Setting)
	{
		if (Target == nullptr || Setting == nullptr)
		{
			return Target;
		}

		Target->CopySettingB(*Setting);

		return Target;
	}

	// 設定クラス取得
	virtual UClass* GetSettingClass() const override
	{
		return USampleDataSettingB::StaticClass();
	}

private:
	UPROPERTY()
		USampleDataSettingB* Setting;
};