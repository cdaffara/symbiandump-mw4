/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   
*
*/


#ifndef __HWATRANSEFFECT__
#define __HWATRANSEFFECT__

#include <e32std.h>
#include <cone.h>
#include <alf/alfvisual.h>

class HwaTransEffect
    {
public:

enum EFxGuiLibrary{
   EGuiNone,  // No connection to wserv   
   EGuiAvkon, // WServ, Cone & Avkon assumed within a thread
   EGuiQt
   }

enum EApplicationFxEvent
    {
    EFxApplicationStarting,
    EFxApplicationClosing,
    EFxApplicationActivated,
    EFxApplicationDeactivated,
    EFxApplicationEmbeddedStarting,
    EFxApplicationEmbeddedClosing
    // Todo: Should we have different events for embed cases where hos loses its focus ?
    }


   // should we enforce this somehow to AppFW (e.g. ref to CAknAppUi / CCoeEnv / ..)
   // or e.g. just enum 
   // Framework will call
    IMPORT_C static TInt InitStatic(TUid aAppUid, TInt aFxGuiLibrary = EGuiAvkon );
    IMPORT_C void Close();
   
    // Geter for thread singleton
    IMPORT_C HwaTransEffect* Static() 
   
    // --------------------------------------------------------------------------------------------------------------
    // Registering a component (Effect identifier & component identifier could be strings as well , should they even be??)
    // --------------------------------------------------------------------------------------------------------------
    IMPORT_C TInt Register(const CCoeControl& aControl, TInt aEffectIdentifier);
    IMPORT_C TInt RegisterUnique(const TDesC8&  aComponentIdentifier, const CCoeControl& aControl,  TInt aEffectIdentifier);
    IMPORT_C TInt Register(const CAlfVisual& aVisual,  TInt aEffectIdentifier);
    IMPORT_C TInt RegisterUnique(const TDesC8&  aComponentIdentifier, const CAlfVisual& aVisual,  TInt aEffectIdentifier);
   
    // ToDo: Should there be possibility to unregister elements / effects as well ?

    // -------------------------------------------------------------------------------------------------------------- 
    // Launching an effect
    // --------------------------------------------------------------------------------------------------------------
    IMPORT_C void Start(TInt aEffectIdentifier );
    IMPORT_C void End( TInt aEffectIdentifier );

    // ToDo: Other way to launch unique effects would be using just passing cntrl / visual reference here
    // On the other hand, if we assume that registering is the method that is allowed to fail, it shoul
    IMPORT_C void Start(const TDesC8& aComponentIdentifier, TInt aEffectIdentifier );
    IMPORT_C void End( const TDesC8& aComponentIdentifier, TInt aEffectIdentifier );

    IMPORT_C void StartApplicationEffect(const TUid& aAppUid, TInt aApplicationFxEvent);
    IMPORT_C void StartApplicationEffect(TInt aWindowGroupId, TInt aApplicationFxEvent);
    IMPORT_C void ResetApplicationEffect(TInt aApplicationFxEvent);
   
private:
   class HwaTransEffectData;
   HwaTransEffectData* iData
   };
   
#endif // #ifndef __HWATRANSEFFECT__

   
   