/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __PLUGININTERFACE_H__
#define __PLUGININTERFACE_H__

#include <EGL/egl.h>


typedef struct
    {
    // the returned egl config that the plugin would preferably use
    // the config actually chosen may differ from what the plugin prefers.
    // The API selection is done according to the value specified by EGL_RENDERABLE_TYPE field in the given config.
    // If given config does not specify EGL_RENDERABLE_TYPE, opengl 2 is assumed by default
    const EGLint* (*getpreferredconfig)(void);

    // sets the display dimensions for the plugin, can be called at any point of time.
    // guaranteed to be once called rigth after initialize
    void (*setdisplaydimensions)(int, int);

    // produces the next frame, eglSwapbuffers will be called immediately after this function by the host.
    void (*produceframe)(void);

    // informs the plugin when it can have gpu resources allocated or not. the plugin is not allowed to make any gpu allocations
    // before this function has been called. also it should release all gpu resources when the parameter is false.
    // return 0 if resource (re)allocation was succesfull. The surface/context can be assumed to be created when this function is called with true
    // if this function is called with false, the surface/context will be destroyed immediately after this call by the host
    int (*gpuresourcesavailable)(int);

    // initializes the plugin, allocated all non gpu related resources
    // should return 0 if initialization was successfull. the full path to the directory where the external data for this plugin resides
    // can assume that EGL is initialized(but the context/surface is not)
    // the second parameter is the maximum amount of graphics memory the plugin is allowed to use (excluding surface)
    int (*initialize)(const char*, unsigned int);
   

    // destroys the plugin instance, plugin should free all resources it has reserved
    void (*destroy)(void);  

    // reports the desired sensor id:s to the host, the host will listen to the specified sensors and sensor data via
    // receivesensordata function. The return value is an array of unsigned ints (sensor id:s). The list must be terminated with a value 0
    // If plugin does not want to use sensors, the function pointer must to be initialized to NULL
    const unsigned int* (*desiredsensors)(void);
    
    // Function for receiving the sensor data. The funtion is essentially the same as MSensrvDataListener::DataReceived( CSensrvChannel& aChannel, TInt aCount, TInt aDataLost ) with different parameter types
    // the function should cast the first void ptr to CSensrvChannel* in order to figure out what kind of data is sent. The 2 int parameters are aDataCount and aDataLost (same as inSensrvDataListener::DataReceived)
    // If plugin does not want to use sensors, the function pointer must to be initialized to NULL
    void (*receivesensordata)(void*, int, int);

    void (*setfaded)(int);
    
    // this function can be used to convey any additional data from/to the plugin.
    int (*extension)(int, void*);
    } plugin_export_v1_t;


#ifdef __cplusplus
extern "C" {
#endif
// returns the plugin interface. the return parameter should be cast to <plugin_export_vX_t*> where X should match the version given as a parameter to this function
IMPORT_C void* getinterface(int version);

#ifdef __cplusplus
}
#endif

#endif
