/*
* ============================================================================
*  Name        : WidgetMMCHandler.h
*  Part of     : Widget Registry
*  Description : Acts on MMC card insertion/removal
*  Version     : 3.1
*
*  Copyright © 2008 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* =============================================================================
*/

#ifndef WIDGETMMCHANDLER_H
#define WIDGETMMCHANDLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include "wrtharvester.h"
//hb
#include "wrtharvesterpsnotifier.h"

// FORWARD DECLARATION
class CWrtHarvester;

/*
 * Inherited CActive, performs a asynchronous conversion operation
 *
 * @since 3.1
 */
class CWrtUsbHandler : public CActive
    {
public:

    static CWrtUsbHandler* NewL( CWrtHarvester* aHarvester, RFs& aFs );

    virtual ~CWrtUsbHandler();

    void Start();
    void ToggleUSBMode();
    void DoScanAndUpdate();
    void DeleteTimer();

protected:

    void RunL();

    TInt RunError( TInt aError );

    void DoCancel();

private:

    // no copy constructor
    CWrtUsbHandler( const CWrtUsbHandler& );
    // no assignment
    CWrtUsbHandler& operator=( const CWrtUsbHandler& );

    CWrtUsbHandler( CWrtHarvester* aHarvester, RFs& aFs );

    void ConstructL();

    TInt ScanDrives( TInt& aDriveFlags );

   // CWidgetRegistry* iRegistry; // not owned
    
    CWrtHarvester* iHarvester;
    RFs iFs;

    TInt iDriveFlags;
    CPeriodic* iCallBackTimer;

    };

#endif // #ifndef WIDGETMMCHANDLER_H
