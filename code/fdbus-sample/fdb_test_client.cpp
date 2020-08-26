#include <common_base/fdbus.h>
#include <common_base/CFdbProtoMsgBuilder.h>
#include "CFdbIfPerson.h"

static CBaseWorker main_worker;

class CMediaClient;
class CInvokeTimer : public CMethodLoopTimer<CMediaClient>
{
public:
    CInvokeTimer(CMediaClient *client);
};

class CMediaClient : public CBaseClient
{
public:
    CMediaClient(const char *name, CBaseWorker *worker = 0);
    void callServer(CMethodLoopTimer<CMediaClient> *timer);

protected:
    void onOnline(FdbSessionId_t sid, bool is_first) override;
    void onOffline(FdbSessionId_t sid, bool is_last) override;
    void onBroadcast(CBaseJob::Ptr &msg_ref) override;
    void onReply(CBaseJob::Ptr &msg_ref) override;

private:
    CInvokeTimer *mTimer;
};

int main(int argc, char **argv)
{
    FDB_CONTEXT->start();
    main_worker.start(); // start the worker thread

    std::cout << ">> Media client startup..." << std::endl;

    CMediaClient client("my_media_client", &main_worker);
    client.connect("svc://media_server");

    /* convert main thread into worker */
    CBaseWorker background_worker;
    background_worker.start(FDB_WORKER_EXE_IN_PLACE);
}

CInvokeTimer::CInvokeTimer(CMediaClient *client) : CMethodLoopTimer<CMediaClient>(1000, true, client, &CMediaClient::callServer)
{
}

CMediaClient::CMediaClient(const char *name, CBaseWorker *worker)
    : CBaseClient(name, worker)
{
    mTimer = new CInvokeTimer(this);
    mTimer->attach(&main_worker, false);
}

void CMediaClient::callServer(CMethodLoopTimer<CMediaClient> *timer)
{
    CFdbRawMsgBuilder builder;

    invoke(REQ_RAWDATA, builder);
}

void CMediaClient::onOnline(FdbSessionId_t sid, bool is_first)
{
    if (is_first)
    {
        mTimer->enable();
        CFdbMsgSubscribeList subscribe_list;
        addNotifyItem(subscribe_list, NTF_ELAPSE_TIME);
        subscribe(subscribe_list);
    }
}

void CMediaClient::onOffline(FdbSessionId_t sid, bool is_last)
{
    if (is_last)
    {
        mTimer->disable();
    }
}

void CMediaClient::onBroadcast(CBaseJob::Ptr &msg_ref)
{
    auto *msg = castToMessage<CBaseMessage *>(msg_ref);
    switch (msg->code())
    {
    case NTF_ELAPSE_TIME:
    {
        int8_t hour;
        int16_t minute;
        int32_t second;
        CFdbRawMsgParser parser(msg->getPayloadBuffer(), msg->getPayloadSize());
        parser.deserializer() >> hour >> minute >> second;
        std::cout << "Receive server broadcast:" << static_cast<int16_t>(hour) << " - " << minute << " - " << second << std::endl;
    }
    break;
    default:
        std::cout << "No Found MSG CODE:" << msg->code() << " - onBroadcast" << std::endl;
        break;
    }
}

void CMediaClient::onReply(CBaseJob::Ptr &msg_ref)
{
    auto *msg = castToMessage<CBaseMessage *>(msg_ref);
    switch (msg->code())
    {
    case REQ_RAWDATA:
    {
        CFdbParcelableArray<CPerson> persions;
        CFdbParcelableParser parser(persions);
        if (!msg->deserialize(parser))
        {
            FDB_LOG_E("onReply: fail to decode from simple parser!\n");
            return;
        }
        std::ostringstream stream;
        (void)persions.format(stream);
        printf("onReply: %s\n", stream.str().c_str());
        for (auto pit = persions.pool().begin(); pit != persions.pool().end(); ++pit)
        {
            FDB_LOG_I("name: %s; age: %d; salary: %d; address: %s.\n",
                      pit->mName.c_str(), pit->mAge, pit->mSalary, pit->mAddress.c_str());
            for (auto cit = pit->mCars.pool().begin(); cit != pit->mCars.pool().end(); ++cit)
            {
                FDB_LOG_I("    brand: %s, model: %s, price: %d.\n",
                          cit->mBrand.c_str(), cit->mModel.c_str(), cit->mPrice);
            }
        }
    }
    break;
    default:
        std::cout << "No Found MSG CODE - onReply" << std::endl;
        break;
    }
}