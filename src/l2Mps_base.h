#ifndef L2MPS_BASE_H
#define L2MPS_BASE_H

#define _GLIBCXX_USE_NANOSLEEP    // Workaround to use std::this_thread::sleep_for

#include <thread>
#include <chrono>
#include <boost/atomic.hpp>
#include "l2Mps_thr.h"

template <typename T>
struct appThr_channel_t
{
    T           appCh;
    thr_table_t thrTb;
};

template <typename T>
class IMpsAppBase
{
private:
    typedef std::function<void(int, std::map<T, thr_ch_t>)> p_appCBFunc_t;
 
public:
    IMpsAppBase(const uint8_t amc) : amc(amc), run(false) {};
    virtual ~IMpsAppBase() 
    {
        if (run)
        {
            run = false;
            scanThread.join();
        }
    };

    // Threhold channel information
    uint32_t const  getChannel  ( const T& ch) const                { return findThrChannel(ch)->getChannel();  };
    bool     const  getIdleEn   ( const T& ch) const                { return findThrChannel(ch)->getIdleEn();   };
    void            setIdleEn   ( const T& ch, const bool en) const { findThrChannel(ch)->setIdleEn(en);        };
    bool     const  getAltEn    ( const T& ch) const                { return findThrChannel(ch)->getAltEn();    };
    bool     const  getLcls1En  ( const T& ch) const                { return findThrChannel(ch)->getLcls1En();  };
    uint32_t const  getByteMap  ( const T& ch) const                { return findThrChannel(ch)->getByteMap();  };
    uint32_t const  getThrCount ( const T& ch) const                { return findThrChannel(ch)->getThrCount(); };


    // Threshold set enable methods
    void            setThresholdMinEn   ( const appThr_channel_t<T>& ch, const bool val) const { findThrChannel(ch.appCh)->setThresholdMinEn(ch.thrTb, val); };
    void            setThresholdMaxEn   ( const appThr_channel_t<T>& ch, const bool val) const { findThrChannel(ch.appCh)->setThresholdMaxEn(ch.thrTb, val); };

    // Threshold get enable methods
    const bool      getThresholdMinEn   ( const appThr_channel_t<T>& ch) const { return findThrChannel(ch.appCh)->getThresholdMinEn(ch.thrTb); };
    const bool      getThresholdMaxEn   ( const appThr_channel_t<T>& ch) const { return findThrChannel(ch.appCh)->getThresholdMaxEn(ch.thrTb); };

    // Threshold set methods
    void            setThresholdMin     ( const appThr_channel_t<T>& ch, const float val) const { findThrChannel(ch.appCh)->setThresholdMin(ch.thrTb, val); };
    void            setThresholdMax     ( const appThr_channel_t<T>& ch, const float val) const { findThrChannel(ch.appCh)->setThresholdMax(ch.thrTb, val); };

    // Threshold get methods
    const float     getThresholdMin     ( const appThr_channel_t<T>& ch) const { return findThrChannel(ch.appCh)->getThresholdMin(ch.thrTb); };
    const float     getThresholdMax     ( const appThr_channel_t<T>& ch) const { return findThrChannel(ch.appCh)->getThresholdMax(ch.thrTb); };

    // Scale factors set/get methods for thresholds
    void            setScaleFactor      ( const T& ch, const float sf) const { findThrChannel(ch)->setScaleFactor(sf);          };
    const float     getScaleFactor      ( const T& ch) const                 { return findThrChannel(ch)->getScaleFactor();     };

    // Set polling thread with callback function
    const void      startPollThread     ( unsigned int poll, p_appCBFunc_t callBack)
    {
        if (poll == 0)
        {
            std::cout << "Error creating poll thread: poll time must be greater than 0" << std::endl;
            return;
        }
        poll   = poll;
        appCB  = callBack;

        std::cout << "    Starting MPS app scan thread..." << std::endl;
        run = true;
        scanThread = std::thread( &IMpsAppBase::pollThread, this );
        if ( pthread_setname_np( scanThread.native_handle(), "mpsAppScan" ) )
            perror( "pthread_setname_np failed for MpsApp scanThread" );
    };

    // Find ThrChannel in the App-ThrChannel map
    ThrChannel      findThrChannel(const T& appCh) const
    {
        typename std::map<T, ThrChannel>::const_iterator it;
        it = appThrMap.find(appCh);

        if (it == appThrMap.end())
            throw std::runtime_error("Channel not defined\n");

        return it->second;
    };

    // Print APP channel information
    virtual void            printChInfo     ( void ) const = 0;

protected:
    std::map<T, ThrChannel>  appThrMap;
    uint8_t                  amc;
    unsigned int             poll;
    p_appCBFunc_t            appCB;
    std::thread              scanThread;
    boost::atomic<bool>      run;

    // Polling functions
    void        pollThread()
    {
        std::cout << "    MPS app scan thread created succesfully." << std::endl;

        for(;;)
        {
             if (!run)
             {
                 std::cout << "    Mps app scan thread interrupted." << std::endl;
                 return;
             }

             std::map<T, thr_ch_t> dataMap;
             typename std::map<T, ThrChannel>::const_iterator it;
            for (it = appThrMap.begin() ; it != appThrMap.end(); ++it)
            {
                thr_ch_t data;
                (it->second)->readAll(data);

                dataMap.insert(std::make_pair(it->first, data));
            }

            appCB(amc, dataMap);
            dataMap.clear();

            std::this_thread::sleep_for( std::chrono::seconds( poll ) );
        }
    };
};

#endif
