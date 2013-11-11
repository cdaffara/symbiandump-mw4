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

#ifndef MATRIX_STACK_H
#define MATRIX_STACK_H

#include "Matrix.h"

namespace shadergen
{


class MatrixStack
{
public:
	static const int MATRIX_STACK_DEPTH = 16;

	/*!
	 * \brief
	 * Default constructor. Initializes sp to 0.
	 */
	MatrixStack (void)
	{
		m_currDepth = 0;
	}

	/*!
	 * \brief
	 * glPushMatrix() implementation.
	 */
	void push (void)
	{
		SG_ASSERT(SG_IN_RANGE<int>(m_currDepth, 0, MATRIX_STACK_DEPTH - 1));
		memcpy(&m_data[m_currDepth + 1], &m_data[m_currDepth], sizeof(Matrix));
		m_currDepth++;
	}

	/*!
	 * \brief
	 * glPopMatrix() implementation
	 */
	void pop (void)
	{
		SG_ASSERT(SG_IN_RANGE<int>(m_currDepth, 1, MATRIX_STACK_DEPTH - 0));
		m_currDepth--;
	}

	/*!
	 * \brief
	 * Get the topmost matrix in the stack.
	 * 
	 * \returns
	 * Returns the topmost matrix in the stack.
	 */
	Matrix& top (void)
	{
		SG_ASSERT(SG_IN_RANGE<int>(m_currDepth, 0, MATRIX_STACK_DEPTH - 1));
		return m_data[m_currDepth];
	}

private:

	int		m_currDepth;
	Matrix	m_data[MATRIX_STACK_DEPTH];
};

} /* namespace shadergen */

#endif /* MATRIX_STACK_H */
