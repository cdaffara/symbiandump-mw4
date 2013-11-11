@rem
@rem Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: Test script config file
@rem
@rem


call bldmake bldfiles
call abld test build armv5
call makesis usbuinotifapitest.pkg
call signsis usbuinotifapitest.sis usbuinotifapitest.sisx x:\rd.cer x:\rd-key.pem
