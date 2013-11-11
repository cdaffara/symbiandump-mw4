/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Batch ext plugin implementation
*
*/


#include "alfbatchextplugin.h"
#include "alfbatchextpluginconstants.h"
#include <implementationproxy.h>
#include <alf/alfconstants.h>
#include <alf/alflayouthandlers.h>

class CAlfBatchExtPluginExtension : public CBase, public MAlfExtension
    {
    public:
        ~CAlfBatchExtPluginExtension();
        virtual void Release();
        virtual TAny* GetInterface( const THuiInterfaceSupport& aInterface );
        virtual void HandleCmdL( TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse );
    private:
        RArray<TInt> iReceived;
    };

MAlfExtension* TAlfBatchExtPlugin::CreateExtensionL(
        const TInt aObjectId, 
        const TDesC8& /*aInitialParams*/, 
        MAlfInterfaceProvider& /*aResolver*/ )
    {
    MAlfExtension* result = 0;
    switch( aObjectId )
        {
        case EAlfBatchExtPluginCreateExtension:
            {
            result = new (ELeave) CAlfBatchExtPluginExtension;
            break;
            }  

        default:
            User::Leave( KErrNotSupported );
        }
    return result;
    }

void TAlfBatchExtPlugin::Release()
    {
    delete this;
    }

CAlfBatchExtPluginExtension::~CAlfBatchExtPluginExtension()
    {
    iReceived.Close();
    }

void CAlfBatchExtPluginExtension::Release()
    {
    delete this;
    }

TAny* CAlfBatchExtPluginExtension::GetInterface( const THuiInterfaceSupport& /*aInterface*/ )
    {
    return NULL;
    }

void CAlfBatchExtPluginExtension::HandleCmdL( TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse )
    {
    switch ( aCommandId )
        {
        case EAlfBatchExtensionCmdReset:
            iReceived.Close();
            break;

        case EAlfBatchExtensionCmdGetCount:
            {
            TPckgBuf<TInt> countPckg( iReceived.Count() );
            aResponse.Copy( countPckg );
            }
            break;

        case EAlfBatchExtensionCmdGetIndex:
            {
            TInt* param = (TInt*)aInputBuffer.Ptr();

            if ( *param >= 0 && *param < iReceived.Count() )
                {
                TPckgBuf<TInt> result( iReceived[ *param ] );
                aResponse.Copy( result );
                }
            else
                {
                User::Leave( KErrArgument );
                }
            }
            break;

        case EAlfBatchExtensionCmdInteger:
            {
            TInt* param = (TInt*)aInputBuffer.Ptr();
            iReceived.AppendL( *param );
            }
            break;

        case EAlfBatchExtensionCmdReal:
            {
            TReal32* param = (TReal32*)aInputBuffer.Ptr();
            TInt asInteger = *param;
            iReceived.AppendL( asInteger );
            }
            break;

        case EAlfBatchExtensionCmdNotSupported:
            User::Leave( KErrNotSupported );
            break;

        default:
            {
            if ( aCommandId >= EAlfBatchExtensionCmdBufferMin && 
                 aCommandId <= EAlfBatchExtensionCmdBufferMax )
                {
                const TInt length = aCommandId - EAlfBatchExtensionCmdBufferMin;
                
                if ( aInputBuffer.Length() == length )
                    {
                    for ( TInt ii = 0; ii < length; ii++ )
                        {
                        TUint8 val = (TUint8)ii;
                        if ( aInputBuffer[ ii ] != val )
                            {
                            User::Leave( KErrArgument );
                            }
                        }
                    iReceived.AppendL( aCommandId );
                    }
                else
                    {
                    User::Leave( KErrArgument );
                    }
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            }
            break;
        }
    }

// Global functions:

MAlfExtensionFactory* Instance()
    {
    TAlfBatchExtPlugin* me = NULL;
    me = new TAlfBatchExtPlugin;
    return me;
    }

const TImplementationProxy ImplementationTable[] =
    {
#ifdef __EABI__
    {{KAlfBatchExtPluginImplementationId}, (TFuncPtr)Instance}
#else
    {{KAlfBatchExtPluginImplementationId}, Instance}
#endif
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy) ;
    return ImplementationTable;
    }
