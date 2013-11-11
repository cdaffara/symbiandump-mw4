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

#ifndef MATRIX_H
#define MATRIX_H

#include "Util.h"
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>

namespace shadergen
{

static const float s_identityMatrix[16] =
{
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
};

#define EPSILON (.01f)

/*!
 * \brief
 * Simple class for simulating OpenGL's matrix operations.
 */
class Matrix
{

public:

	/*!
	 * \brief
	 * Default constructor.
	 *
	 * \remarks
	 * Does nothing. Could initialize the matrix to identity, but then
	 * again, why would it? It only hides errors.
	 */
	Matrix (void)
	{
	}

	Matrix (const Matrix& mat)
	{
		(*this) = mat;
	}

	const Matrix& operator= (const Matrix& mat)
	{
		memcpy(m, mat.m, 16 * sizeof(float));
		return *this;
	}

	/*!
	 * \brief
	 * glLoadMatrix() implementation.
	 * 
	 * \param nm
	 * Array of 16 floats.
	 */
	void loadMatrix (const float* nm)
	{
		memcpy(m, nm, 16 * sizeof(float));
	}

	/*!
	 * \brief
	 * glLoadIdentity() implementation
	 */
	void loadIdentity (void)
	{
		memcpy(m, s_identityMatrix, 16 * sizeof(float));
	}

	/*!
	 * \brief
	 * Check if the matrix is identity or not.
	 * 
	 * \returns
	 * Returns true if the matrix is the identity matrix, false otherwise.
	 */
	bool isIdentity (void) const
	{
		return !memcmp(m, s_identityMatrix, 16 * sizeof(float));
	}

	/*!
	 * \brief
	 * glMultMatrix() implementation.
	 * 
	 * \param b
	 * The other matrix, as floats.
	 */
	void multiply (const float* b)
	{
		float dst[16];
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				float sum = 0.f;
				for (int k = 0; k < 4; k++)
					sum += m[i + 4 * k] * b[k + 4 * j];

				dst[i + 4 * j] = sum;
			}
		}

		memcpy(m, dst, 16 * sizeof(float));
	}

	/*!
	 * \brief
	 * Sums given matrix (4x4) to this matrix (4x4).
	 * 
	 * \param b
	 * The other matrix, as floats.
	 */
	void plus (const float* b)
	{
	    for (int i = 0; i < 4; i++)
	    {
	        for (int j = 0; j < 4; j++)
	        {
	            m[i + 4 * j] += b[i + 4 * j];
	        }
	    }
	}

	/*!
	 * \brief
	 * glScalef() implementation.
	 * 
	 * \param sx
	 * X scale
	 * 
	 * \param sy
	 * Y scale
	 * 
	 * \param sz
	 * Z scale
	 */
	void scale (const float sx, const float sy, const float sz)
	{
		const float sm[16] =
		{
			sx, 0.f, 0.f, 0.f,
			0.f,  sy, 0.f, 0.f,
			0.f, 0.f,  sz, 0.f,
			0.f, 0.f, 0.f, 1.f
		};

		multiply(sm);
	}

	/*!
	 * \brief
	 * glTranslatef() implementation.
	 * 
	 * \param tx
	 * X component of the translation vector
	 * 
	 * \param ty
	 * Y component of the translation vector
	 * 
	 * \param tz
	 * Z component of the translation vector
	 */
	void translate (const float tx, const float ty, const float tz)
	{
		const float tm[16] =
		{
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			tx,  ty,  tz,  1.f
		};

		multiply(tm);
	}

	/*!
	 * \brief
	 * glRotatef() implementation
	 * 
	 * \param angle
	 * The amount of rotation
	 * 
	 * \param x
	 * X component of the axis about which to rotate
	 * 
	 * \param y
	 * Y component of the axis about which to rotate
	 * 
	 * \param z
	 * Z component of the axis about which to rotate
	 */
	void rotate (const float angle, const float x, const float y, const float z)
	{
		static const float deg_to_rad = 2.f * (float)M_PI / 360.f;
		// normalised vector components
		float x_n;
		float y_n;
		float z_n;
		float angle_rad = angle * deg_to_rad;

		// normalise if needed
		const float length = (float)sqrt(double(x * x) + 
					  double(y * y) + 
					  double(z * z));
		if(fabs(length - 1.0f) > EPSILON) {
			// in fp calculations, division by zero -> (+/-)inf
			// can't really help if it's the case.
			const float inv_length = 1.f / length;
			x_n = x * inv_length;
			y_n = y * inv_length;
			z_n = z * inv_length;
		} else {
			x_n = x;
			y_n = y;
			z_n = z;
		}

		const float c = cos(angle_rad);
		const float s = sin(angle_rad);
		const float c_1 = 1.f - c;

		const float rm[16] =
		{
	x_n * x_n * c_1 + c,	   y_n * x_n * c_1 + z_n * s, x_n * z_n * c_1 - y_n * s, 0.f,
	x_n * y_n * c_1 - z_n * s, y_n * y_n * c_1 + c,	      y_n * z_n * c_1 + x_n * s, 0.f,
	x_n * z_n * c_1 + y_n * s, y_n * z_n * c_1 - x_n * s, z_n * z_n * c_1 + c,	 0.f,
	0.f, 			   0.f,			      0.f,		   	 1.f

		};

		multiply(rm);
	}

	/*!
	 * \brief
	 * glFrustumf() implementation
	 * 
	 * \param l
	 * Left
	 * 
	 * \param r
	 * Right
	 * 
	 * \param b
	 * Bottom
	 * 
	 * \param t
	 * Top
	 * 
	 * \param n
	 * Near
	 * 
	 * \param f
	 * Far
	 */
	void frustum (const float l, const float r, const float b, const float t, const float n, const float f)
	{
		SG_ASSERT(r != l);
		SG_ASSERT(t != b);
		SG_ASSERT(n != f);

		const float rl = 1.f / (r - l);
		const float tb = 1.f / (t - b);
		const float fn = 1.f / (f - n);

		const float A = (r + l) * rl;
		const float B = (t + b) * tb;
		const float C = -(f + n) * fn; 
		const float D = -2.f * f * n * fn;

		const float fm[16] =
		{
			2.f * n * rl,	0.f, 		0.f,	0.f,
			0.f,		2.f * n * tb,	0.f,	0.f,
			A,		B,		C,	-1.f,
			0.f,		0.f,		D,	0.f
		};
		
		multiply(fm);
	}

	/*!
	 * \brief
	 * glOrthof() implementation
	 * 
	 * \param l
	 * Left
	 * 
	 * \param r
	 * Right
	 * 
	 * \param b
	 * Bottom
	 * 
	 * \param t
	 * Top
	 * 
	 * \param n
	 * Near
	 * 
	 * \param f
	 * Far
	 */
	void ortho (const float l, const float r, const float b, const float t, const float n, const float f)
	{
		SG_ASSERT(r != l);
		SG_ASSERT(t != b);
		SG_ASSERT(n != f);

		const float rl = 1.f / (r - l);
		const float tb = 1.f / (t - b);
		const float fn = 1.f / (f - n);

		const float tx = -(r + l) * rl;
		const float ty = -(t + b) * tb;
		const float tz = -(f + n) * fn;

		const float om[16] = 
		{
			2.f * rl,	0.f,		0.f,		0.f,
			0.f,		2.f * tb,	0.f,		0.f,
			0.f,		0.f,		-2.f * fn,	0.f,
			tx,		ty,		tz,		1.f
		};

		multiply(om);
	}


	/*!
	 * \brief
	 * Returns pointer to floats making up the matrix. Suitable for 
	 * glLoadMatrix().
	 */
	const float *getMatrix(void) const {
		return m;
	}


private:
	
	float m[16];
};


} /* namespace shadergen */

#endif /* MATRIX_H */
