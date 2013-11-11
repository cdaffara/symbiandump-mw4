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

#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include "Util.h"
#include "GLState.h"

using namespace std;

namespace shadergen
{




class Variable
{
public:

	/* Variable class */
	typedef enum Class
	    {
		CLASS_UNIFORM = 0,
		CLASS_ATTRIBUTE,
		CLASS_VARYING,

		CLASS_MAX
	    } Class;

	/* Variable precision */
	typedef enum Precision
	    {
		PRECISION_LOW = 0,
		PRECISION_MEDIUM,
		PRECISION_HIGH,

		PRECISION_MAX
	    } Precision;

	/* Variable type */
	typedef enum Type
	    {
		TYPE_FLOAT = 0,
		TYPE_VEC2,
		TYPE_VEC3,
		TYPE_VEC4,

		TYPE_INT,
		TYPE_IVEC2,
		TYPE_IVEC3,
		TYPE_IVEC4,

		TYPE_MAT2,
		TYPE_MAT3,
		TYPE_MAT4,

		TYPE_SAMPLER,

		TYPE_MAX
	    } Type;

	/* variable data's type tag --- an ugly hack for now */
	typedef enum Tag
	    {
		DATAPTR = 0,
		DATAPTRPTR,
		TEXTUREUNIT
	    } Tag;

	union variableData 
	    {
		const void *		dataPtr;
		const void * const *	dataPtrPtr;
		int			textureUnit;

		variableData(const void *dataPointer) 
		    {
			dataPtr = dataPointer;
		    }

		variableData(const void * const *dataPointerPointer) 
		    {
			dataPtrPtr = dataPointerPointer;
		    }

		variableData(int unit) 
		    { 
		    textureUnit = unit; 
		    }
	};



	Variable (const Class vclass,
		const char* name,
		const Type type,
		const GLenum format,
		int dim,
		const Precision precision,
		const void* dataPtr)
		: m_class(vclass),
		m_name(name),
		m_type(type),
		m_format(format),
		m_dimension(dim),
		m_precision(precision),
		m_tag(DATAPTR),
		m_variableData(dataPtr),
		m_location(0)
	{
		SG_ASSERT(type != TYPE_SAMPLER);
		SG_ASSERT(format == GL_BYTE			||
			      format == GL_UNSIGNED_BYTE	||
			      format == GL_SHORT			||
			      format == GL_UNSIGNED_SHORT	||
			      format == GL_INT		||
			      format == GL_FLOAT			||
			      format == GL_FIXED);
//		SG_ASSERT((vclass != CLASS_VARYING) == (funcPtr != NULL));
	}


	Variable (const Class vclass,
		const char* name,
		const Type type,
		const GLenum format,
		int dim,
		const Precision precision,
		const void* const* dataPtrPtr)
		: m_class(vclass),
		m_name(name),
		m_type(type),
		m_format(format),
		m_dimension(dim),
		m_precision(precision),
		m_tag(DATAPTRPTR),
		m_variableData(dataPtrPtr),
		m_location(0)
	{
		SG_ASSERT(type != TYPE_SAMPLER);
		SG_ASSERT(format == GL_BYTE			||
			   format == GL_UNSIGNED_BYTE	||
			   format == GL_SHORT			||
			   format == GL_UNSIGNED_SHORT	||
			   format == GL_INT		||
			   format == GL_FLOAT			||
			   format == GL_FIXED);
//		SG_ASSERT((vclass != CLASS_VARYING) == (funcPtr != NULL));
	}



	Variable (const Class vclass,
		const char* name,
		const Type type,
		const GLenum format,
		int dim,
		const Precision precision,
		int textureUnit)
		: m_class(vclass),
		m_name(name),
		m_type(type),
		m_format(format),
		m_dimension(dim),
		m_precision(precision),
		m_tag(TEXTUREUNIT),
		m_variableData(textureUnit),
		m_location(0)
	{
		SG_ASSERT((type == TYPE_SAMPLER) && (format == GL_INT) ||
		          (vclass == CLASS_VARYING && textureUnit == 0));
	}



	string getDeclaration (void) const
	{
		string decl = classToStr() + " " + precisionToStr() + " " + typeToStr() + " " + m_name;

		if (m_dimension != 1)
		{
			SG_ASSERT(SG_IN_RANGE<int>(m_dimension, 1, 9));
			decl += "[";
			decl += (char)('0' + m_dimension);
			decl += "]";
		}

		decl += ";\n";
		return decl;
	}

	const string& getName (void) const
	{
		return m_name;
	}

	Class getClass (void) const
	{
		return m_class;
	}

	Type getType (void) const
	{
		return m_type;
	}

	GLenum getFormat (void) const
	{
		return m_format;
	}

	int getDimension (void) const
	{
		return m_dimension;
	}

	Precision getPrecision (void) const
	{
		return m_precision;
	}

	const void* getPtr(void) const
	{
		switch(m_tag) {
			case DATAPTR : 
				return m_variableData.dataPtr;

			case DATAPTRPTR :
				return *m_variableData.dataPtrPtr;

			case TEXTUREUNIT : 
				return (void *)(&m_variableData.textureUnit);

			default : 
				SG_ASSERT(0);
		}
		return NULL;
	}

    void setLocation(int location)
        {
        m_location = location;
        }

    int location(void) const
        {
        return m_location;
        }

private:

	string typeToStr (void) const
	{
		const char* s_typeStr[] =
		{
			"float",	// TYPE_FLOAT = 0,
			"vec2",		// TYPE_VEC2,
			"vec3",		// TYPE_VEC3,
			"vec4",		// TYPE_VEC4,

			"int",		// TYPE_INT,
			"ivec2",	// TYPE_IVEC2,
			"ivec3",	// TYPE_IVEC3,
			"ivec4",	// TYPE_IVEC4,

			"mat2",		// TYPE_MAT2,
			"mat3",		// TYPE_MAT3,
			"mat4",		// TYPE_MAT4,

			"sampler2D"	// TYPE_SAMPLER
		};

		SG_CT_ASSERT(SG_LENGTH_OF_ARRAY(s_typeStr) == TYPE_MAX);
		return s_typeStr[m_type];
	}

	string classToStr (void) const
	{
		const char* s_classStr[] =
		{
			"uniform",		// CLASS_UNIFORM = 0
			"attribute",		// CLASS_ATTRIBUTE,
			"varying"		// CLASS_VARYING,
		};

		SG_CT_ASSERT(SG_LENGTH_OF_ARRAY(s_classStr) == CLASS_MAX);
		return s_classStr[m_class];
	}

	string precisionToStr (void) const
	{
		const char* s_precStr[] =
		{
			"lowp",		// PRECISION_LOW = 0
			"mediump",	// PRECISION_MEDIUM,
			"highp"		// PRECISION_HIGH,
		};

		SG_CT_ASSERT(SG_LENGTH_OF_ARRAY(s_precStr) == PRECISION_MAX);
		return s_precStr[m_precision];
	}

   

	Class		m_class;
	string		m_name;
	Type		m_type;
	GLenum		m_format;
	int		m_dimension;
	Precision	m_precision;
	Tag		m_tag;
	variableData	m_variableData;
    int m_location;
};

}

#endif /* VARIABLE_H */
