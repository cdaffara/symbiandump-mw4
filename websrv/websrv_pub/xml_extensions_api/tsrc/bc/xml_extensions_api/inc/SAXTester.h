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
#ifndef ADDRESSBOOKENGINE_H
#define ADDRESSBOOKENGINE_H

#include <SenBaseFragment.h>
//#include <RXMLReader.h>
#include <SenXmlReader.h>

// FORWARD DECLARATIONS
class CSAXTester : public CSenBaseFragment
	{
	public:
		static CSAXTester* NewL( const TDesC8& aLocalName );
		static CSAXTester* NewL( const TDesC8& aNsUri, const TDesC8& aLocalName );
		static CSAXTester* NewL( const TDesC8& aNsUri, const TDesC8& aLocalName, const TDesC8& aQName );
		static CSAXTester* NewL( const TDesC8& aNsUri, const TDesC8& aLocalName, const TDesC8& aQName, const RAttributeArray& apAttrs );
		static CSAXTester* NewL( const TDesC8& aNsUri, const TDesC8& aLocalName, const TDesC8& aQName, const RAttributeArray& apAttrs, CSenElement& aParent );
		static CSAXTester* NewL( CSenElement& aParent );

		void ConstructL();
		void ConstructL( const TDesC8& aLocalName );
		void ConstructL( const TDesC8& aNsUri, const TDesC8& aLocalName );
		void ConstructL( const TDesC8& aNsUri, const TDesC8& aLocalName, const TDesC8& aQName );
		void ConstructL( const TDesC8& aNsUri, const TDesC8& aLocalName, const TDesC8& aQName, const RAttributeArray& apAttrs );
		void ConstructL( const TDesC8& aNsUri, const TDesC8& aLocalName, const TDesC8& aQName, const RAttributeArray& apAttrs, CSenElement& aParent );
		void ConstructL( CSenElement& aParent );
		
		virtual ~CSAXTester();
		TBool CheckParsingStatus(TInt aStart, TInt aEnd, TInt aContent );
		void SetRunCase(TInt aCase);
   		CSenBaseFragment* GetDetachedDelegate();
		CSenBaseFragment* GetDelegate();
	
		HBufC8* iAsXml;
	protected:
		virtual void StartElementL(
			const TDesC8& aNsUri,
			const TDesC8& aLocalName,
			const TDesC8& aQName,
			const RAttributeArray& aAttrs
			);
		virtual void EndElementL(
			const TDesC8& aNsUri,
			const TDesC8& aLocalName,
			const TDesC8& aQName
			);

		virtual void CharactersL(
			const TDesC8& aChars,
			TInt aStart,
			TInt aLength
			);
		
		CSAXTester();
		
	private:

		CSenXmlReader* iXmlReader;
		TInt iStartElem;
		TInt iEndElem;
		TInt iContent;
		TInt iRunCase;
		CSenBaseFragment* iDelegate;
		

};

#endif // ADDRESSBOOKENGINE_H

// End of File
