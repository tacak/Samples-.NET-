#include <windows.h>
#include <stdio.h>
#include <fwpmu.h>

#pragma comment(lib, "Fwpuclnt.lib")
#pragma comment(lib, "rpcrt4.lib")

typedef struct SNIPIT_STRUCT_
{
    UINT16 fieldId;
    UINT16 reserved;
} SNIPIT_STRUCT;


DWORD wmain(int argc,
              wchar_t* argv[])
{
   UNREFERENCED_PARAMETER(argc);
   UNREFERENCED_PARAMETER(argv);

   DWORD status = ERROR_SUCCESS;

   /// Creating a session and opening a handle to the engine
   HANDLE engineHandle = 0;
   FWPM_SESSION0 session;
   ZeroMemory(&session,sizeof(session));

   session.displayData.name = L"Snipit Session";
   session.displayData.description = L"Session created by Snipit.exe";
         
   status = FwpmEngineOpen0(0,
                            RPC_C_AUTHN_DEFAULT,
                            0,
                            &session,
                            &engineHandle);
   if(status != ERROR_SUCCESS)
   {
      wprintf(L"Error:FwpmEngineOpen0() [%d]\n", status);
      goto cleanup;
   }
   else
      wprintf(L"Info:FwpmEngineOpen0() [%d]\n", status);


   /// Begin Transaction
   FwpmTransactionBegin0(engineHandle, 0);

   /// Add a Sublayer
   FWPM_SUBLAYER0 sublayer;
   ZeroMemory(&sublayer,sizeof(sublayer));

   UuidCreate(&sublayer.subLayerKey);
   sublayer.displayData.name = L"Snipit Sublayer";
   sublayer.displayData.description = L"Sublayer added by Snipit.exe";
   sublayer.weight = 1; 

   status = FwpmSubLayerAdd0(engineHandle,
                            &sublayer,
                             0);
   if(status != ERROR_SUCCESS)
   {
      wprintf(L"Error:FwpmSubLayerAdd0() [%d]\n", status);
      goto cleanup;
   }
   else
      wprintf(L"Info:FwpmSubLayerAdd0() [%d]\n", status);

   /// Add a Filter
   FWPM_FILTER0 blockFilter;
   ZeroMemory(&blockFilter,sizeof(blockFilter));
   FWPM_FILTER_CONDITION0 tcpCondition;
   ZeroMemory(&tcpCondition,sizeof(tcpCondition));

   UuidCreate(&blockFilter.filterKey);
   blockFilter.displayData.name = L"Snipit TCP block filter";
   blockFilter.displayData.description = L"Filter added by Snipit.exe";
   blockFilter.layerKey = FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4;
   blockFilter.action.type = FWP_ACTION_BLOCK;
   blockFilter.subLayerKey = sublayer.subLayerKey;
   blockFilter.numFilterConditions = 1;
   blockFilter.filterCondition = &tcpCondition;

   tcpCondition.fieldKey = FWPM_CONDITION_IP_PROTOCOL;
   tcpCondition.matchType = FWP_MATCH_EQUAL;
   tcpCondition.conditionValue.type = FWP_UINT8;
   tcpCondition.conditionValue.uint8 = 0x06; /// TCP

   status = FwpmFilterAdd0(engineHandle,
                          &blockFilter,
                           0,
                          &blockFilter.filterId);
   if(status != ERROR_SUCCESS)
   {
      wprintf(L"Error:FwpmFilterAdd0() [%d]\n", status);
      goto cleanup;
   }
   else
      wprintf(L"Info:FwpmFilterAdd0() [%d]\n", status);

   /// Commit add transactions
   FwpmTransactionCommit0(engineHandle);

   /// Begin next transactions
   FwpmTransactionBegin0(engineHandle, 0);
   
   /// Delete a Filter
   status = FwpmFilterDeleteByKey0(engineHandle,
                                  &blockFilter.filterKey);
   if(status != ERROR_SUCCESS)
   {
      wprintf(L"Error:FwpmFilterDeleteByKey0() [%d]\n", status);
      goto cleanup;
   }
   else
      wprintf(L"Info:FwpmFilterDeleteByKey0() [%d]\n", status);

   /// Delete a Sublayer
   status = FwpmSubLayerDeleteByKey0(engineHandle,
                                    &sublayer.subLayerKey);
   if(status != ERROR_SUCCESS)
   {
      wprintf(L"Error:FwpmSubLayerDeleteByKey0() [%d]\n", status);
      goto cleanup;
   }
   else
      wprintf(L"Info:FwpmSubLayerDeleteByKey0() [%d]\n", status);

   /// Commit delete transactions
   FwpmTransactionCommit0(engineHandle);

   /// Add a Provider Context
   FWPM_PROVIDER_CONTEXT0 providerContext;
   ZeroMemory(&providerContext, sizeof(providerContext));

   FWP_BYTE_BLOB dataBuffer;
   ZeroMemory(&dataBuffer, sizeof(dataBuffer));
   
   SNIPIT_STRUCT snipitStruct;
   ZeroMemory(&snipitStruct, sizeof(snipitStruct));
   
   UuidCreate(&providerContext.providerContextKey);
   providerContext.type = FWPM_GENERAL_CONTEXT;
   providerContext.displayData.name = L"Snipit provider context";
   providerContext.displayData.description = L"Provider context added by Snipit.exe";
   providerContext.dataBuffer = &dataBuffer;
   providerContext.dataBuffer->data = (UINT8*) &snipitStruct;
   providerContext.dataBuffer->size = sizeof(snipitStruct);

   status = FwpmProviderContextAdd0(engineHandle,
                                   &providerContext,
                                    0,
                                   &providerContext.providerContextId);
   if(status != ERROR_SUCCESS)
   {
      wprintf(L"Error:ProviderContextAdd00() [%d]\n", status);
      goto cleanup;
   }
   else
      wprintf(L"Info:ProviderContextAdd0() [%d]\n", status);

   /// Delete a Provider Context
   status = FwpmProviderContextDeleteById0(engineHandle,
                                           providerContext.providerContextId);
   if(status != ERROR_SUCCESS)
   {
      wprintf(L"Error:FwpmProviderContextDeleteById0() [%d]\n", status);
      goto cleanup;
   }
   else
      wprintf(L"Info:FwpmProviderContextDeleteById0() [%d]\n", status);

   /// Get IKE Statistics
   IKEEXT_STATISTICS0 ikeStats;
   ZeroMemory(&ikeStats,sizeof(ikeStats));

   status = IkeextGetStatistics0(engineHandle,
                              &ikeStats);
   if(status != ERROR_SUCCESS)
   {
      wprintf(L"Error:IkeGetStatistics0() [%d]\n", status);
      goto cleanup;
   }
   else
      wprintf(L"Info:IkeGetStatistics0() [%d]\n", status);

cleanup:

   FwpmEngineClose0(engineHandle);
   
   return status;
}