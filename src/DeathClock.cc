#include <v8.h>
#include <node.h>
#include <iostream>

using namespace node;
using namespace v8;
using namespace std;
#include "DeathClock.h"

uv_mutex_t DEATH_CLOCK_IO_MUTEX;

#define BEGIN_ASYNC(_data, async, after) \
uv_work_t *_req = new uv_work_t; \
_req->data = _data; \
uv_queue_work(uv_default_loop(), _req, async, (uv_after_work_cb)after);
typedef void async_rtn;
#define RETURN_ASYNC
#define RETURN_ASYNC_AFTER delete req;


string ObjectToString(Local<Value> value) {
    String::Utf8Value utf8_value(value);
    return string(*utf8_value);
}

int GetArgumentIntValue ( const Arguments& args, int argNum, int &value )
{
    if (args[argNum]->IsNumber())
    {
        value = args[argNum]->Int32Value();
    } 
    else {
        string emsg("GetArgumentIntValue arg " + ObjectToString(args[argNum]) + " not an integer");
        ThrowException(Exception::TypeError(String::New(emsg.c_str())));
        return PROCESS_FAIL;
    }
    return PROCESS_OK;
}


int GetArgumentDoubleValue ( const Arguments& args, int argNum, double &value )
{
    if (args[argNum]->IsNumber())
    {
        value = args[argNum]->NumberValue();
    } 
    else {
        string emsg("GetArgumentDoubleValue arg " + ObjectToString(args[argNum]) + " not an number");
        ThrowException(Exception::TypeError(String::New(emsg.c_str())));
        return PROCESS_FAIL;
    }
    return PROCESS_OK;
}


int GetArgumentStringValue ( const Arguments& args, int argNum, string &value)
{
    if (args[argNum]->IsString())
    {
        value = ObjectToString(args[argNum]);    
    } 
    else {
        string emsg("GetArgumentStringValue arg " + ObjectToString(args[argNum]) + " not an string");
        ThrowException(Exception::TypeError(String::New(emsg.c_str())));
        return PROCESS_FAIL;
    }
    return PROCESS_OK;

}



async_rtn asyncDeathClock(uv_work_t *req)
{
    DeathClockData *m = (DeathClockData *)req->data;

    while (m->m_ContinueCountDown) {
        uv_mutex_lock(&DEATH_CLOCK_IO_MUTEX); 
        cout << "checking for " << m->m_sErrorMessage << " counter " << m->m_Counter << " max allowed " << m->m_NMaxChecks << " continue count down " << m->m_ContinueCountDown << " pointer " << (void *)m->m_ContinueCountDown << endl;
        uv_mutex_unlock(&DEATH_CLOCK_IO_MUTEX); 
        
        if (m->m_Counter >= m->m_NMaxChecks) {
            m->m_ContinueCountDown = 0;
            
            uv_mutex_lock(&DEATH_CLOCK_IO_MUTEX);
        
            stringstream err;
            err << " ERROR DeathClock::DeathClock end of universe reached, ---<<<*** EXPLOSIONS ***>>>--- message: " << m->m_sErrorMessage;
            cout << err.str() << endl;        

            // safe guard your path cleanup area
            std::size_t found = m->m_sPathToClean.find("/tmp/");

            if (found != string::npos) {
                cout << "ABOUT TO KILL JOB IN asyncDeathClock, this path is going bye bye " << m->m_sPathToClean << endl;
                stringstream cleanUp;
                cleanUp << "sudo rm -rf " << m->m_sPathToClean << " &";
                const char *eString = cleanUp.str().c_str();
                system(eString);
            }
            uv_mutex_unlock(&DEATH_CLOCK_IO_MUTEX);

            usleep(1000); // sleep for a 1ms, then die
            assert(false); // kill switch, would like a cleaner one to interrupt the main loop and return error state
        }
        m->m_Counter++;
        usleep(m->m_uSecSleep); // usleep for x useconds
    }        

    RETURN_ASYNC
}


async_rtn afterDeathClock(uv_work_t *req) 
{
    DeathClockData *m = (DeathClockData *)req->data;
    if (m) delete m;
    RETURN_ASYNC_AFTER
}

void asyncDeathClock(double TimeOutFailureSeconds, const string &sErrorMessage, int uSecSleep, int &ContinueCountDown, const string &sPathToClean)
{
    // cout << "asyncDeathClock setup: message " << sErrorMessage << " asyncDeathClock pointer to continue count down " << (void *)&ContinueCountDown << endl;
    DeathClockData *pm = new DeathClockData(ContinueCountDown);
    pm->m_Counter = 0;
    pm->m_uSecSleep = uSecSleep;
    pm->m_NMaxChecks = TimeOutFailureSeconds / ((double)uSecSleep * 1e-6);
    pm->m_sErrorMessage = sErrorMessage.c_str();
    pm->m_sPathToClean = sPathToClean.c_str();

    BEGIN_ASYNC(pm, asyncDeathClock, afterDeathClock);
}

void DeathClock::Init(Handle<Object> exports) 
{   
    int status = uv_mutex_init(&DEATH_CLOCK_IO_MUTEX);
    if (status != 0) {
        cout << "DeathClock::Init unable to initialize mutex" << endl;
        exit(1);
    }

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("DeathClock"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    
    // Prototype
    tpl->PrototypeTemplate()->Set(String::NewSymbol("Stop"),
      FunctionTemplate::New(Stop)->GetFunction());

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    exports->Set(String::NewSymbol("DeathClock"), constructor);
}

Handle<Value> DeathClock::New(const Arguments& args) {
    HandleScope scope;

    DeathClock* obj = new DeathClock();
    
    double TimeOutFailureSeconds;
    int uSecSleep;
    string sErrorMessage,sPathToClean;

    int targs=0;
    GetArgumentDoubleValue(args,targs++,TimeOutFailureSeconds);
    GetArgumentStringValue(args,targs++,sErrorMessage);
    GetArgumentStringValue(args,targs++,sPathToClean);
    GetArgumentIntValue(args,targs++,uSecSleep);
    
    obj->start(TimeOutFailureSeconds,sErrorMessage,sPathToClean,uSecSleep);

    obj->Wrap(args.This());

    return args.This();
}

Handle<Value> DeathClock::Stop(const Arguments& args) {
    HandleScope scope;

    DeathClock* obj = ObjectWrap::Unwrap<DeathClock>(args.This());
    obj->stopDeathClock();

    return scope.Close(Number::New(1));
}

void DeathClock::start(double TimeOutFailureSeconds, const string &sErrorMessage, const string &sPathToClean, int uSecSleep)
{
    m_uSecSleep  = uSecSleep;
    m_ContinueCountDown = 1;
    m_sErrorMessage = sErrorMessage;

    asyncDeathClock(TimeOutFailureSeconds,sErrorMessage,uSecSleep,m_ContinueCountDown,sPathToClean);
}

#ifdef _DEATH_CLOCK_NODE_MODULE

extern "C" {
  static void init(Handle<Object> exports)
  {
      HandleScope scope;
      cout << "_DEATH_CLOCK_NODE_MODULE is defined " << endl;

      // classes
      DeathClock::Init(exports);
  }

  NODE_MODULE(deathClock, init)
}

#endif