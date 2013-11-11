/**
 * Provides general constants for test cases.
 */

#include "alfmoduletestconf.h"
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF

#ifndef ALFMODULETESTCONSTS_H
#define ALFMODULETESTCONSTS_H

#include <e32def.h>


/**
 * Constants
 */
namespace AlfModuleTestConsts
    {
    /**
     * When flush is requested, the flow should wait long enough.
     * Then, a test case should have plenty of time to finish itself.
     */
    const TInt KFlushLongWaitTime( 2000000 );
    
    /**
     * When flush is requested, the flow should wait.
     * Then, a normal test case should have enough time to finish itself.
     */
    const TInt KFlushWaitTime( 200000 );
    }


#endif // ALFMODULETESTCONSTS_H

#endif // USE_MODULE_TEST_HOOKS_FOR_ALF

// End of File
