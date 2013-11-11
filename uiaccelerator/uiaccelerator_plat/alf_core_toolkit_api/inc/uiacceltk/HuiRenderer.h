/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Defines THuiRenderer, Hitchcock renderer enumeration.
*
*/



#ifndef __HUIRENDERER_H__
#define __HUIRENDERER_H__
 
//-----------------------------------------------------------

/*! \page cfbsbitmapsupport Input bitmaps supported

Not all CFbsBitmap formats are supported by the Hitchcock UI toolkit. 
The following tables describe which CFbsBitmap formats can be used with 
CHuiTexture::UploadL, and bitmap providers 
(CHuiTextureManager::CreateTextureL() with a MHuiBitmapProvider).

\section cfbsbitmapsupport_gles OpenGL ES Renderer bitmap support

<table cellspacing="1" cellpadding="1" border="0"><tr><td bgcolor="#eaeaea" > &nbsp; </td><th bgcolor="#dadada" align="center">  <strong>Mask Mode:<br/>NULL/Enone</strong>  </th><th bgcolor="#dadada" align="center">  <strong>EGray2</strong>  </th><th bgcolor="#dadada" align="center">  <strong>EGray4</strong>  </th><th bgcolor="#dadada" align="center">  <strong>EGray256</strong>  </th><th bgcolor="#dadada">  <strong>EColorXY-modes</strong>  </th></tr>

<tr><th bgcolor="#dadada">  <strong>Bitmap Mode:<br/> EGray2</strong>  </th><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td></tr>
<tr><th bgcolor="#dadada" align="center">  <strong>EGray4</strong>  </th><td bgcolor="#eaeaea"> &nbsp; </td><td bgcolor="#eaeaea"> &nbsp; </td><td bgcolor="#eaeaea"> &nbsp; </td><td bgcolor="#eaeaea"> &nbsp; </td><td bgcolor="#eaeaea"> &nbsp; </td></tr>
<tr><th bgcolor="#dadada" align="center">  <strong>EGray16</strong>  </th><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td></tr>
<tr><th bgcolor="#dadada" align="center">  <strong>EGray256</strong>  </th><td bgcolor="#eaeaea"> &nbsp; </td><td bgcolor="#eaeaea"> &nbsp; </td><td bgcolor="#eaeaea"> &nbsp; </td><td bgcolor="#eaeaea"> &nbsp; </td><td bgcolor="#eaeaea"> &nbsp; </td></tr>
<tr><th bgcolor="#dadada" align="center">  <strong>EColor16</strong>  </th><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td></tr>
<tr><th bgcolor="#dadada" align="center">  <strong>EColor256</strong>  </th><td bgcolor="#eaeaea"> &nbsp; </td><td bgcolor="#eaeaea"> &nbsp; </td><td bgcolor="#eaeaea"> &nbsp; </td><td bgcolor="#eaeaea" align="center"> Yes </td><td bgcolor="#eaeaea"> &nbsp; </td></tr>
<tr><th bgcolor="#dadada" align="center">  <strong>EColor4K</strong>  </th><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff"> &nbsp; </td></tr>
<tr><th bgcolor="#dadada" align="center">  <strong>EColor64K</strong>  </th><td bgcolor="#eaeaea" align="center"> Yes * </td><td bgcolor="#eaeaea" align="center"> Yes </td><td bgcolor="#eaeaea"> &nbsp; </td><td bgcolor="#eaeaea" align="center"> Yes </td><td bgcolor="#eaeaea"> &nbsp; </td></tr>
<tr><th bgcolor="#dadada" align="center">  <strong>EColor16M</strong>  </th><td bgcolor="#ffffff" align="center"> Yes </td><td bgcolor="#ffffff" align="center"> Yes </td><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff" align="center"> Yes </td><td bgcolor="#ffffff"> &nbsp; </td></tr>
<tr><th bgcolor="#dadada" align="center">  <strong>EColor16MU</strong>  </th><td bgcolor="#eaeaea" align="center"> Yes </td><td bgcolor="#eaeaea" align="center"> Yes </td><td bgcolor="#eaeaea"> &nbsp; </td><td bgcolor="#eaeaea" align="center"> Yes </td><td bgcolor="#eaeaea"> &nbsp; </td></tr>

<tr><th bgcolor="#dadada" align="center">  <strong>EColor16MA</strong>  </th><td bgcolor="#ffffff" align="center"> Yes </td><td bgcolor="#ffffff" align="center"> Yes </td><td bgcolor="#ffffff"> &nbsp; </td><td bgcolor="#ffffff" align="center"> Yes </td><td bgcolor="#ffffff"> &nbsp; </td></tr>
</table>

(asterisk) = No conversion required (fastest upload)										

You can also check the latest information from https://cwiki.nokia.com/HUIToolkit/HUITKBitmapSupport

*/

//-----------------------------------------------------------

/**
 * @enum THuiRenderer
 * 
 * Enumerates Hitchcock renderers. The renderer is specified by the 
 * application when an environment is created.
 * 
 * Different renderers may not support all HuiGc methods available.
 * For example the EHuiRendererBitgdi does not support any 3D-related
 * methods, whereas EHuiRendererGles11 supports most of them.
 * 
 * To compare the features of different renderers see the Hitchcock
 * UI programmers guide.
 * 
 * Also the support for input CFbsBitmaps varies between different
 * renderers. See \ref cfbsbitmapsupport renderer bitmap support
 * table. 
 * 
 * @see CHuiEnv::NewL()
 */
enum THuiRenderer
    {

    /**
     * Automatically selects the best renderer plugin available.
     *
     * @todo Later, a system configuration variable may be defined
     * that selects the  designated renderer plugin for a device.
     */
    EHuiRendererDefault      = 0,

    /** Symbian BITGDI renderer. */
    EHuiRendererBitgdi      = 1,

    /** OpenVG 1.0 renderer (Not implemented). */
    EHuiRendererVg10        = 2,

    /** OpenGL ES 1.0 renderer. */
    EHuiRendererGles10      = 3,

    /** OpenGL ES 1.1 renderer. */
    EHuiRendererGles11      = 4,

    /** OpenGL ES 1.1 DSA renderer. Implemented, but please use EHuiRendererGles11. */
    EHuiRendererGles11DSA   = 5,

    /** OpenGL ES 2.0 renderer (Not implemented). */
    EHuiRendererGles20      = 6
    };

    
#endif // __HUIRENDERER_H__
