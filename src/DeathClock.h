#ifndef DEATH_CLOCK
#define DEATH_CLOCK 1

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <assert.h>
#include <tr1/unordered_map>

#include <uv.h>

using namespace std;
using namespace std::tr1;

#define PROCESS_OK 0
#define PROCESS_FAIL 1
extern unsigned long DEATH_CLOCK_ID;


#define BEGIN_ASYNC(_data, async, after) \
    uv_work_t *_req = new uv_work_t; \
    _req->data = _data; \
    uv_queue_work(uv_default_loop(), _req, async, (uv_after_work_cb)after);
    typedef void async_rtn;
#define RETURN_ASYNC
#define RETURN_ASYNC_AFTER delete req;

struct DeathClockData {
    DeathClockData() {};

    unsigned long   m_Counter;
    unsigned long   m_NMaxChecks;
    int             m_uSecSleep;
    string          m_sErrorMessage;
    unsigned int    m_clockID;
};

class DeathClock : public node::ObjectWrap {
public:
    static void Init(v8::Handle<v8::Object> exports);

protected:
    DeathClock() { m_ID = DEATH_CLOCK_ID++; };
    ~DeathClock() { stopDeathClock(); };

    void start(double TimeOutFailureSeconds, const string &sErrorMessage, int uSecSleep = 10000);


    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    static v8::Handle<v8::Value> Stop(const v8::Arguments& args);

    void stopDeathClock();

    unsigned int m_ID; 
    int m_uSecSleep;
    string m_sErrorMessage;
};




#endif