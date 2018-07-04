// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "DataTableUtilBPLibrary.h"
#include "DataTableUtil.h"

UDataTableUtilBPLibrary::UDataTableUtilBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

FTableRowBase UDataTableUtilBPLibrary::GetDataTableRowFromIndex(UDataTable* table, int index) {
	TArray<FTableRowBase*> Rows;
	table->GetAllRows<FTableRowBase>("", Rows);
	return *(Rows[index]);
}

/*bool UDataTableUtilBPLibrary::Generic_GetDataTableRows(UDataTable* table, void* OutRows) {

	table->GetAllRows<FTableRowBase>("hoge", OutRows);
	return true;
}*/

