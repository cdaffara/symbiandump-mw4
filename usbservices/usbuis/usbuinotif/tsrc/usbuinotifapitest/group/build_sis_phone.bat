@rem
@rem Copyright (c) 2006 - 2010 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This material, including documentation and any related 
@rem computer programs, is protected by copyright controlled by 
@rem Nokia. All rights are reserved. Copying, including 
@rem reproducing, storing, adapting or translating, any 
@rem or all of this material requires the prior written consent of 
@rem Nokia. This material also contains confidential 
@rem information which may not be disclosed to others without the 
@rem prior written consent of Nokia.
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:  
@rem

call sbs -k -c armv5.test 
call makesis usbuinotifapitest_phone.pkg
call signsis usbuinotifapitest_phone.sis usbuinotifapitest_phone.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
