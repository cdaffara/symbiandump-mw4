/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Implementation source file
 *
*/


#include "SVGPathDataParser.h"
#include "SVGPointLexer.h"

#include "GfxFloatFixPt.h"
#include "GfxAffineTransform.h"
#include "GfxFlatteningPathIterator.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void TSvgPathDataParser::ParsePathData( const TDesC& aData,
                                          CGfxGeneralPath* aPath )
    {

		TRAPD( error, ParsePathDataL( aData, aPath ) );
		if ( error != KErrNone )
		     {
			 // error processing
			 return;
	         }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void TSvgPathDataParser::ParsePathDataL( const TDesC& aData,
                                           CGfxGeneralPath* aPath )
    {
    TUint8 lPrevCom = '\0';
	TFloatFixPt xy[6];
	TInt32 rslt = ETrue;
	TUint8 com;
    TUint8 lPrevCom2 = '\0';

    TSvgPointLexer svgplex( aData );

    TFloatFixPt KZero;
    TFloatFixPt KMinVal = 0.01f;
    TFloatFixPt KOne( KFloatFixOne );

    while ( svgplex.GetCommand ( com ) )
        {

			// Reset for command procesing
			if( com == '\0' )
				{
				if( lPrevCom == 'z' ||lPrevCom == 'Z' )
					{
					aPath->Reset();
					break ;
					}
				// As per 1.2  spec 8.3.2 The "moveto" commands
	            // If a 'moveto' is followed by multiple pairs of coordinates without explicit commands,
	            // the subsequent pairs shall be treated as implicit 'lineto' commands.

				else if( lPrevCom == 'm')
					{
					com = 'l';
					}
				else if( lPrevCom == 'M' )
					{
					com = 'L';
					}
				else
					{
					com = lPrevCom;
					}
				}
			else
				{
				lPrevCom = com;
				}

				switch ( com )
					{
					case 'M':
							rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (2) );
							if (rslt && lPrevCom2 == 'M')
								{
									//AJD do we want to remove this
									// i.e. d="M 20 20 M 10 10 L 15 15
									// has a bounding box of 5,5,10,10 or 10,10,10,10
									//aPath->RemoveLastPathCommand();
								}
							if(rslt)
                                {
                                aPath->MoveToL( xy[0], xy[1], ETrue );
                                }
                            break;
                    case 'm':
                            rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (2) );
                            // added to change values from 0.0 to 0.01f if both the co-oridnates
                            // are having value 0.0. 
                            // fix for bug "CGSR-78X9V2"
                            if(xy[0] == KZero && xy[1] == KZero)
                               {
                            	 xy[0] = KMinVal;
                            	 xy[1] = KMinVal;
                               }
                            if(rslt)
                               {
                               aPath->MoveToL( xy[0], xy[1], EFalse );
                               }
                            
							break;
					case 'L':
							rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (2) );
							if(rslt)
								{
								aPath->LineToL( xy[0], xy[1], ETrue );
								}
							break;
					case 'H':
							rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (1) );
							if(rslt)
								{
								aPath->HorizonToL( xy[0], ETrue );
								}
							break;
					case 'V':
							rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (1) );
							if(rslt)
                                {
                                aPath->VertToL( xy[0], ETrue );
                                }
                            break;
                    case 'l':
                            rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (2) );
                            // added to change values from 0.0 to 0.01f if both the co-oridnates
                            // are having value 0.0. 
                            // fix for bug "CGSR-78X9V2"
                            if(xy[0] == KZero && xy[1] == KZero)
                               {
                               xy[0] = KMinVal;
                               xy[1] = KMinVal;
                               }
                           	if(rslt)
							   {
							   aPath->LineToL( xy[0], xy[1], EFalse );
							   }
						    break;
                    case 'h':
                            rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (1) );
                            // added to change values from 0.0 to 0.01f if both the co-oridnates
                            // are having value 0.0. 
                            // fix for bug "CGSR-78X9V2"
                            if(xy[0] == KZero && xy[1] == KZero)
                               {
                               xy[0] = KMinVal;
                               xy[1] = KMinVal;
                               }
                            if(rslt)
                               {
                               aPath->HorizonToL( xy[0], EFalse );
                               }
                             
                            break;
                    case 'v':
                            rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (1) );
                            // added to change values from 0.0 to 0.01f if both the co-oridnates
                            // are having value 0.0. 
                            // fix for bug "CGSR-78X9V2"
                            if(xy[0] == KZero && xy[1] == KZero)
                               {
                               xy[0] = KMinVal;
                               xy[1] = KMinVal;
                               }
                            if(rslt)
                               {
                               aPath->VertToL( xy[0], EFalse );
                               }
                            
							break;
					case 'C':
							rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (6) );
							if( rslt )
								{
								aPath->CubicToL( xy[0],
										 xy[1],
										 xy[2],
										 xy[3],
										 xy[4],
										 xy[5],
										 ETrue );
								}
							break;
					case 'c':
							rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (6) );
							if(rslt)
								{
								aPath->CubicToL( xy[0],
												 xy[1],
												 xy[2],
												 xy[3],
												 xy[4],
												 xy[5],
												 EFalse );
								}
							break;
					case 'S':
							rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (4) );
							if(rslt)
								{
								aPath->ShortCubicToL( xy[0],
													  xy[1],
													  xy[2],
													  xy[3],
													  ETrue );
								}
							break;
					case 's':
							rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (4) );
							if(rslt)
								{
								aPath->ShortCubicToL( xy[0],
													  xy[1],
													  xy[2],
													  xy[3],
													  EFalse );
								}
							break;
					case 'Q':
							rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (4) );
							if( rslt )
								{
								aPath->QuadToL( xy[0],
												  xy[1],
												  xy[2],
												  xy[3],
												ETrue );
								}
							break;
					case 'T':
							rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (2) );
							if(rslt)
								{
								
								// If there is no previous command or if
								// the previous command was not a Q, q, T or t, the control point shall be current point.
								if ( lPrevCom2 !='Q' && lPrevCom2 != 'q' && lPrevCom2 != 'T'
								&& lPrevCom2 != 't' )
								   {
								   aPath->QuadToLWithNoControlPoint( xy[0], xy[1] );
							   }
								else
								   {
								   aPath->ShortQuadToL( xy[0], xy[1], ETrue );	
								   }
								}
							break;
					case 'q':
							rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (4) );
							if(rslt)
								{
								aPath->QuadToL( xy[0],
										  xy[1],
										  xy[2],
                                          xy[3],
                                        EFalse );
                                }
                            break;
                    case 't':
                            rslt = svgplex.GetNextWithNumOfPoints( xy ,TInt (2) );
                            if(xy[0] != KZero || xy[1] != KZero)
                                {
								if(rslt)
									{
									aPath->ShortQuadToL( xy[0], xy[1], EFalse );
									}
								}
							break;
					case 'Z':
					case 'z':
							aPath->ClosePathL();
							break;

					default:
							rslt=0;
							break;

					}// switch(lPrevCom)

				if( !rslt )
					{
					// This is a failing path case reset.
					aPath->Reset();
					break ;
					}

				lPrevCom2 = com;
        }
    svgplex.Cleanup();
    }

//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void TSvgPathDataParser::ParsePointDataL( const TDesC& aData,
                                           CGfxGeneralPath* aPath )
    {
    TSvgPointLexer svgplex( aData );
    TBool first = ETrue;
    while ( !svgplex.IsDone() )
        {
        TChar com;
        TFloatFixPt x, y;
        TInt32 rslt = svgplex.GetNextPoint( com, x, y );

        if ( rslt != KErrNone )
            {
            aPath->Reset();
            break;
            }
        if ( first )
            {
            aPath->MoveToL( x, y, ETrue );
            first = EFalse;
            }
        else
            {
            aPath->LineToL( x, y, ETrue );
            }
        }
    svgplex.Cleanup();
    }

//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt TSvgPathDataParser::PathLengthL( CGfxGeneralPath* aPath )
    {
    CGfxFlatteningPathIterator* pitr;
    TGfxAffineTransform ident;
    pitr = CGfxFlatteningPathIterator::NewL( aPath, & ident, 3 );


    CleanupStack::PushL( pitr );

    TFloatFixPt len;
    TFloatFixPt lx, ly;
    TFloatFixPt dx, dy, int0x7f( KMAXFLOATFIX );
    TFloatFixPt tmpcoords[6];
    while ( !pitr->IsDone() )
        {
        switch ( pitr->CurrentSegment( tmpcoords ) )
            {
            case EGfxSegMoveTo:
                lx = tmpcoords[0];
                ly = tmpcoords[1];
                break;
            case EGfxSegLineTo:
                dx = lx - tmpcoords[0];
                dy = ly - tmpcoords[1];
                if ( dx > int0x7f || dy > int0x7f )
                    {
                    // calculate as integer
                    TInt32 tmplen, tx, ty;
                    tx = ( TInt32 ) dx;
                    ty = ( TInt32 ) dy;
	#ifdef SVG_FLOAT_BUILD
                    tmplen = TFloatFixPt::Sqrt(tx * tx + ty * ty);
	#else
                    tmplen = TFloatFixPt::FixedSqrtGeneral( tx * tx + ty * ty, 0 );
	#endif
                    len += TFloatFixPt( tmplen );
                    }
                else
                    {
                    // calculate as TFixPt
	
                    len += TFloatFixPt::Sqrt( dx * dx + dy * dy );
                    }
                lx = tmpcoords[0];
                ly = tmpcoords[1];
                break;
            case EGfxSegClose:
                ;// Not support multiple path length
                break;
            default:
                break;
            }
        pitr->NextL();
        }

    CleanupStack::PopAndDestroy( 1 );

    return len;
    }
