#pragma once

#include "Object.h"
#include "SampleData.generated.h"

////////////////////////////////////////////////////////////////////////////////
// �T���v���f�[�^���

UCLASS()
class DATATABLEUTIL_API USampleData : public UObject
{
	GENERATED_BODY()

public:
	// �ݒ�N���X�擾
	virtual UClass* GetSettingClass() const { return nullptr; }
};


////////////////////////////////////////////////////////////////////////////////
// �T���v���f�[�^A

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
	// �ݒ�R�s�[�֐�
	void CopySettingA(const USampleDataSettingA& InSetting)
	{
		UE_LOG(LogTemp, Log, TEXT("******* Copy SettingA"));
		Setting->PropertyA = InSetting.PropertyA;
	}

	// �ÓI�ݒ�R�s�[�֐��iIntermediateNode�ɂ��Ăяo���p�j
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

	// �ݒ�N���X�擾
	virtual UClass* GetSettingClass() const override
	{
		return USampleDataSettingA::StaticClass();
	}

private:
	UPROPERTY()
		USampleDataSettingA* Setting;
};


////////////////////////////////////////////////////////////////////////////////
// �T���v���f�[�^B

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
	// �ݒ�R�s�[�֐�
	void CopySettingB(const USampleDataSettingB& InSetting)
	{
		UE_LOG(LogTemp, Log, TEXT("******* Copy SettingB"));
		Setting->PropertyB = InSetting.PropertyB;
	}

	// �ÓI�ݒ�R�s�[�֐��iIntermediateNode�ɂ��Ăяo���p�j
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

	// �ݒ�N���X�擾
	virtual UClass* GetSettingClass() const override
	{
		return USampleDataSettingB::StaticClass();
	}

private:
	UPROPERTY()
		USampleDataSettingB* Setting;
};