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
* Description:  
*        Macro definition file for logging configuration.
*       (Note:This file has to be separate from the logging header file 
*             to be able to include it in mmp file. If not we will get warning about not used 
*             flogger.lib in other logging cases where flogger.lib is not used.)
*
*/




#ifndef ALFLOGGINGCONFIGURATION_H
#define ALFLOGGINGCONFIGURATION_H

/**
* - Have this line active if you want to have logging for this component
* - Comment this line out if you don't want any logging for this component
*/
//#define _ALF_LOGGING

/**
* - Have this lines active if you want to have logging for effects
* - Comment thess lines out if you don't want any logging for effects
*/
//#define _ALF_FXLOGGING
//#define HUIFX_TRACE
/**
* - Have this line active if you want to have logging to file (defined in AlfLogger.h)
* - Comment this line out if you want to have logging via RDebug::Print 
*/
//#define _ALF_LOGGING_TO_FILE

#endif // ALFLOGGINGCONFIGURATION_H

// End of File
