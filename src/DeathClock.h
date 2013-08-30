#ifndef DEATH_CLOCK
#define DEATH_CLOCK 1

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <assert.h>

#include <uv.h>

using namespace std;


#define BEGIN_ASYNC(_data, async, after) \
    uv_work_t *_req = new uv_work_t; \
    _req->data = _data; \
    uv_queue_work(uv_default_loop(), _req, async, (uv_after_work_cb)after);
    typedef void async_rtn;
#define RETURN_ASYNC
#define RETURN_ASYNC_AFTER delete req;

struct DeathClockData {
    DeathClockData(int &bContinueCountDown) : m_ContinueCountDown(bContinueCountDown) {
        // cout << "DeathClockData took in a reference to an int continue count down " << bContinueCountDown << " ";
        // cout << m_ContinueCountDown << " pointer " << (void *)&m_ContinueCountDown << endl;
    };

    unsigned long   m_Counter;
    unsigned long   m_NMaxChecks;
    int             m_uSecSleep;
    string          m_sErrorMessage;
    string          m_sPathToClean;
    int            &m_ContinueCountDown;
};

class DeathClock {
public:
    DeathClock(double TimeOutFailureSeconds, const string &sErrorMessage, const string &sPathToClean, int uSecSleep = 10000);
    ~DeathClock();
    void stopDeathClock() { 
        // cout << "setting continue countdown to zero for " << m_sErrorMessage << " ptr " << (void *)&m_ContinueCountDown << endl;
        if (m_ContinueCountDown) m_ContinueCountDown = 0; 
        usleep(m_uSecSleep); // wait for monitor thread to wake up and perceive continue count down, then shut itself down
        // cout << "                 countdown              " << m_ContinueCountDown << endl;
    };

    int m_uSecSleep;
    int m_ContinueCountDown;
    string m_sErrorMessage;
};


#endif