#include <common_base/fdbus.h>
#include <iostream>
// Define two worker to send job from "sender" to "receiver" periodically
static CBaseWorker sender("peer1");
static CBaseWorker receiver("peer2");

/*
 * Customize a job to convey user data and perform specific task. All jobs should inhert
 * from CBaseJob
 */
class CGreetingJob : public CBaseJob
{
public:
   CGreetingJob(const char *greeting_word)
   {
       mGreetingWord = greeting_word;
   }
protected:
   /*
    * This is the task to be performed by the worker. It can be regarded as callback and
    * will be executed at the worker thread it is scheduled to
    * "worker": the worker where the timer is running
    * "ref": reference to the job itself. Don't care
    */
   void run(CBaseWorker *worker, Ptr &ref)
   {
       std::cout << mGreetingWord << std::endl;
       FDB_LOG_I("Job is received at %s.\n", worker->name().c_str());
   }
private:
   std::string mGreetingWord;
};

/*
 * Customize a timer to throw job from one worker to another. All timers should
 * inherit from CBaseLoopTimer
 */
class CSenderTimer : public CBaseLoopTimer
{
public:
   /*
    * "interval" specify how long the timer expires in ms; "true" means it is a cyclic
    * timer (run periodically) rather than one-shot
    */
   CSenderTimer(CBaseWorker &receiver, int32_t interval, bool sync, bool urgent)
       : CBaseLoopTimer(interval, true)
       , mReceiver(receiver)
       , mSync(sync)
       , mUrgent(urgent)
   {}
protected:
   /*
    * This is the task to be performed by the timer. It can be regarded as callback
    * and will be executed at the worker thread it is attached with
    */
   void run()
   {
       if (mSync)
       {
           /*
            * send job to the worker "mReceiver" stands for synchronously. The call will block until
            * CGreetingJob::run() is executed by mReceiver. "0" means no timeout and block infinately;
            * "mUrgent" is "true" means the job is urgent and will be processed before normal jobs;
            * "false" means a job with normal priority.
            */
           mReceiver.sendSync(new CGreetingJob("hello, world!"), 0, mUrgent);
       }
       else
       {
           /*
            * send job to the worker "mReceiver" stands for asynchronously. The call will return
            * immediately after the job is queued in worker.
            */
           mReceiver.sendAsync(new CGreetingJob("hello, world!"), mUrgent);
       }
       FDB_LOG_I("Job is sent.\n");
   }
private:
   CBaseWorker &mReceiver;
   bool mSync;
   bool mUrgent;
};

int main(int argc, char **argv)
{
   FDB_CONTEXT->start();
   sender.start(); /* start sender worker thread */
   receiver.start(); /* start receiver worker thread */
   auto *sender_timer = new CSenderTimer(receiver, 100, false, false);
   /*
    * attach the time with worker "sender". Once time expired, CSenderTimer::run() will be
    * executed in the context of "sender" thread. "true" means start the timer immediately;
    * If it is 'false', the timer is not started and can start later.
    */
   sender_timer->attach(&sender, true);
   /*
    * convert main thread into a worker so that it can run event loop to handle job/timer.
    * In this case "FDB_WORKER_EXE_IN_PLACE" should be set which means no new thread is created
    * and instead current thread (here is the main thread) will be used to run event loop.
    */
   CBaseWorker background_worker;
   background_worker.start(FDB_WORKER_EXE_IN_PLACE);
}
