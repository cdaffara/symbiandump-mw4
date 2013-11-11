#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <sensrvchannelfinder.h> 
#include <sensrvchannel.h> 
#include <sensrvdatalistener.h>

NONSHARABLE_CLASS(CSensorListener) : public CBase
    {
    public:
        static CSensorListener* NewL(MSensrvDataListener* aDataListener, TUint32 aSensorType);
        virtual ~CSensorListener();
        void StopListening();
        void StartListeningL();
    protected:
        void ConstructL(TUint32 aSensorType);
        CSensorListener(MSensrvDataListener* aDataListener);
    private:
        MSensrvDataListener* iDataListener;
        CSensrvChannel* iSensrvChannel;
        TBool iListening;
    };

#endif