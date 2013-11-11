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
#include <string.h>
#include <GLES2/gl2.h>
#include <sys/syslimits.h>

#include "shader.h"


GLuint LoadAndCompileShader(char* path, char* shadername, GLenum type)
    {
    char filename[PATH_MAX];
    int len = 0;
    GLuint shader = 0;
    GLint compiled = 0;
    char* shaderdata = NULL;
    FILE* file;
    
    strcpy(filename,path);
    strcat(filename,"\\");
    strcat(filename,shadername);
    file = fopen(filename,"rb");
    if (!file)
        {
        return 0;
        }
        

    fseek (file, 0, SEEK_END);
    len = ftell (file);
    fseek(file, 0, SEEK_SET);
    if (len<0)
        {
        fclose(file);
        return 0;
        }
    shaderdata = (char*)malloc(len+1);
    if (!shaderdata)
        {
        fclose(file);
        return 0;
        }
    
    fread(shaderdata, 1, len, file);
    fclose(file);
    shaderdata[len] = 0;

    
    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const char**)&shaderdata,NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);    
    
    if (!compiled)
        {
        char infobuf[ 256 ];

        glGetShaderInfoLog( shader, 256, NULL, infobuf );

        glDeleteShader(shader);
        shader = 0;
        }
    free(shaderdata);
    return shader;
    }
