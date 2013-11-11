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
#include <stdio.h>
#include <stdlib.h>
#include <GLES2/gl2.h>
#include <math.h>
#include <string.h>

#include "plugininterface.h"
#include "shader.h"


static char* plugindir = 0;

static GLuint vertexShader = 0;
static GLuint fragmentShader = 0;
static GLuint program = 0;

static GLuint vbo = 0;
static GLuint ibo = 0;

static GLint blob1;
static GLint blob2;
static GLint blob3;
                  
static GLint offset;
static GLint threshold;
static GLint aspect;

static float time = 0.0f;

static int display_w = 0;
static int display_h = 0;

static const GLushort indices[6] = 
    {
    0,1,2,0,2,3
    };
    
static const GLfloat vertexattribs[] = 
    {
    -1.0f,1.0f,0.0f,
    -1.0f,-1.0f,0.0f,
    1.0f,-1.0f,0.0f,
    1.0f,1.0f,0.0f,
    };


const EGLint attrib_list[] =
        {
        EGL_BUFFER_SIZE,     32,
        EGL_DEPTH_SIZE,      16,
        EGL_STENCIL_SIZE,    0,
        EGL_SAMPLE_BUFFERS,  0,
        EGL_SAMPLES,         0,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
        };

const EGLint* getpreferredconfig (void)
    {
    return attrib_list;
    }
    
void setdisplaydimensions(int width, int height)
    {
    display_w = width;
    display_h = height;
    }

void produceframe(void)
    {
    glUniform3f(blob1,0.76000f+((sin(time))/2.0f),0.30000f+((cos(time*0.4f))/2.0f),4.34000f);
    glUniform3f(blob2,0.44000f+((sin(0.4f*time))/2.0f),0.64000f+((cos(time))/2.0f),2.26000f);
    glUniform3f(blob3,0.30000f+((sin(0.7f*time))/2.0f),0.34000f+((cos(time*0.9f))/2.0f),1.84000f);

    time+=0.02f;
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,0);
    }


int gpuresourcesavailable(int available)
    {
    GLint linkStatus = 0;
    float aspectratio = 0.0f;
    if (available)
        {

        vertexShader = LoadAndCompileShader(plugindir, "refvertexshader.vsh",GL_VERTEX_SHADER);
        fragmentShader = LoadAndCompileShader(plugindir, "reffragmentshader.fsh",GL_FRAGMENT_SHADER);
        if (!vertexShader || !fragmentShader)
            {
            return -1;
            }

        program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glBindAttribLocation(program, 0, "vPosition");

        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (!linkStatus)
            {
            glDeleteProgram(program);           

            return -1;
            }
        
        glClearColor(0,0,0,0);
        glViewport (0, 0, display_w, display_h);

        
        glGenBuffers( 1, &vbo);
        glGenBuffers( 1, &ibo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        
        glBufferData(GL_ARRAY_BUFFER,3*4*sizeof(GLfloat),vertexattribs,GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(GLushort),indices,GL_STATIC_DRAW);
    
        glVertexAttribPointer(	0, 3, GL_FLOAT, GL_FALSE, 0, 0); 
        
        glEnableVertexAttribArray(0);
      
        glUseProgram(program);
        glEnableVertexAttribArray(0);
        
        blob1 = glGetUniformLocation(program, "blob1");
        blob2 = glGetUniformLocation(program, "blob2");
        blob3 = glGetUniformLocation(program, "blob3");
        aspect = glGetUniformLocation(program, "aspect");
    
        offset = glGetUniformLocation(program, "offset");
        threshold = glGetUniformLocation(program, "threshold");
    
        aspectratio = ((float)display_w)/((float)display_h);
    
        glUniform1f(offset,34.78935f);
        glUniform1f(threshold,2.49860f);
        glUniform1f(aspect,aspectratio);
        }
    else
        {
        // TODO: free resources...
        }
    return 0;
    }

int initialize(const char* path, unsigned int maxgpumemusage)
    {
    plugindir = strdup(path);
    return 0;
    }
    
void destroy()
    {
    free(plugindir);
    }     

int extension(int value, void* ptr)
    {
    return 0;
    }

EXPORT_C void* getinterface(int version)
    {
    plugin_export_v1_t* interface = NULL;
    if (version == 1)
        {
        interface = (plugin_export_v1_t*)malloc(sizeof(plugin_export_v1_t));
        if (interface)
            {
            memset(interface, 0, sizeof(plugin_export_v1_t));
            
            // bind our functions to the interface
            interface->getpreferredconfig = getpreferredconfig;
            interface->setdisplaydimensions = setdisplaydimensions;
            interface->produceframe = produceframe;
            interface->gpuresourcesavailable = gpuresourcesavailable;
            interface->initialize = initialize;
            interface->destroy = destroy;
            interface->desiredsensors = 0;
            interface->receivesensordata = 0;
            interface->setfaded = 0;
            interface->extension = extension;
            }
        }
    return interface;
    }
