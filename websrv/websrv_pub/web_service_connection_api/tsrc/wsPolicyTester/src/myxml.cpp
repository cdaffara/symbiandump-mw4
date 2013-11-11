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







#include <e32std.h>
#include <f32file.h>
#include "myxml.h"
#include "SenBaseAttribute.h"

namespace
{
  _LIT8( KWsuId , "wsu:Id");                       // Prefix ID
}
CMyXmlEle* CMyXmlEle::NewL()
{
     CMyXmlEle* pSelf = CMyXmlEle::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;
}
CMyXmlEle* CMyXmlEle::NewLC()
{
       CMyXmlEle* pSelf = new (ELeave) CMyXmlEle();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;
  
}
CMyXmlEle::~CMyXmlEle()
{
  delete iXmlReader;
}
CMyXmlEle::CMyXmlEle()
{
    
}
void CMyXmlEle::ConstructL()
{
  iXmlReader = CSenXmlReader::NewL();
}

CSenElement* CMyXmlEle::XMLDocL()
{
CSenElement* ele = &AsElement();
    if(ele)
        return ele;
    else
        return NULL;
}
void CMyXmlEle::ReadFileL(TFileName aPath)
{
      RFs fss;
       User::LeaveIfError(fss.Connect());
        CleanupClosePushL(fss);

    TFileName iFileName1(aPath);
    RFile  xmlFile;
    xmlFile.Open(fss , iFileName1, EFileRead );


    TInt size;
    xmlFile.Size(size);

    HBufC8 *iBuf=HBufC8::NewL(size);

    TPtr8 buf8ptr(iBuf->Des());
    xmlFile.Read(buf8ptr,size);
    xmlFile.Close();
    size = iBuf->Length();
    if(size > 0)
        {
        TPtr8 buf8ptr(iBuf->Des());
        
        TRAPD (err, ParseL(buf8ptr));
        
        }
    delete iBuf;        
    CleanupStack::PopAndDestroy(1); // fss        
}

void CMyXmlEle::WriteAllL(CSenElement* aXml)
{
      RFs fss;
       User::LeaveIfError(fss.Connect());
        CleanupClosePushL(fss);
        
    TBuf<255> file;
    _LIT( KFileName, "e:\\testing\\data\\normalizer%d.xml");
    file.Format(KFileName, count);
    count++;
    TPtrC OutputFile(file);
    
   RFile  xmlFile;
   xmlFile.Replace(fss , OutputFile, EFileWrite );
   HBufC8* temp = aXml->AsXmlL();
   xmlFile.Write(temp->Des());
   xmlFile.Close();
   delete temp;
    
   CleanupStack::PopAndDestroy(1); // fss

}
