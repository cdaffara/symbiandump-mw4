/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   interface for creating custom extensions
*
*/


#ifndef _ALFHINTPLUGIN_H_
#define _ALFHINTPLUGIN_H_

#include <s32mem.h> //RDesReadStream

/*
 * CAlfHintGraphic
 * 
 * This class is part of CAlfHintApi. Its enabes adding of custom markers 
 * into gc command stream. It has two parts: CAlfHintGraphic and CAlfHintGraphicDrawer 
 *
 * When CAlfHintGraphic is created, wserv creates automatically its counter part 
 * CAlfHintGraphicDrawer.
 * 
 * Use method CAlfHintGraphic::InsertTag for inserting a custom tags. These are 
 * played by CAlfHintGraphicDrawer and delivered to CAlfRenderStage 
 * (ResolveObjectInterface API used).
 */
NONSHARABLE_CLASS(CAlfHintGraphic): public CWsGraphic
    {
public:
    
    /*
     * Available tags. 
     */
    enum TAlfContentTag
        {
        EAlfOffscreenContentBegin, // for Java
        EAlfOffscreenContentEnd,   // for Java 
        EAlfOffscreenContentFlush, // for Java
        
        EAlfSkinContentBegin,
        EAlfSkinContentEnd,
                
        EAlfSetWindowId,
        // EAlfContentScale, etc..
        EAlfBeginSubWindow,
        EAlfEndSubWindow,
        EAlfContentLastTag
        };
    
    IMPORT_C static CAlfHintGraphic* NewL();

    IMPORT_C ~CAlfHintGraphic();

public: // protected virtuals from CWsGraphic promoted to public
    
    IMPORT_C TInt ShareGlobally();
    
    IMPORT_C TInt UnShareGlobally();
    
    IMPORT_C TInt Share(TSecureId aClientId);
    
    IMPORT_C TInt UnShare(TSecureId aClientId);
    
public: // Alf hinting API  

    IMPORT_C TInt InsertTag( TInt aTag );
    IMPORT_C TInt InsertTag( TInt aTag, TRect aBoundingRectangle, TInt aLayerId );
    
    IMPORT_C void SetGc( CWindowGc* aGc ); 
    
private:
    
    void ConstructL();
    
    CAlfHintGraphic();
    
    void HandleMessage(const TDesC8& aData);

    void OnReplace();
    
    TInt InsertTagL( TInt aTag  );
    TInt InsertTagL( TInt aTag, TRect aBoundingRectangle, TInt aLayerId );
    
private:
    
    CWindowGc* iGc;
    
    mutable RBuf8 iMsgDataBuf;
};
#endif // _ALFHINTPLUGIN_H_
