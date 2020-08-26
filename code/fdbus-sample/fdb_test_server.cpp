#include <common_base/fdbus.h>
#include <common_base/CFdbProtoMsgBuilder.h>
#include "CFdbIfPerson.h"

static CBaseWorker main_worker;

class CMediaServer;
class CBroadcastTimer : public CMethodLoopTimer<CMediaServer>
{
public:
    CBroadcastTimer(CMediaServer *server);
};

class CMediaServer : public CBaseServer
{
public:
    CMediaServer(const char *name, CBaseWorker *worker = 0);
    void broadcastElapseTime(CMethodLoopTimer<CMediaServer> *timer);

protected:
    void onOnline(FdbSessionId_t sid, bool is_first) override;
    void onOffline(FdbSessionId_t sid, bool is_last) override;
    void onInvoke(CBaseJob::Ptr &msg_ref) override;
    void onSubscribe(CBaseJob::Ptr &msg_ref) override;

private:
    CBroadcastTimer *mTimer;
};

int main(int argc, char **argv)
{
    FDB_CONTEXT->start();
    main_worker.start();

    std::cout << ">> Media server startup..." << std::endl;

    CMediaServer server("my_media_server", &main_worker);
    server.bind("svc://media_server");

    CBaseWorker background_worker;
    background_worker.start(FDB_WORKER_EXE_IN_PLACE);
}

CBroadcastTimer::CBroadcastTimer(CMediaServer *server)
    : CMethodLoopTimer<CMediaServer>(1500, true, server, &CMediaServer::broadcastElapseTime)
{
}

CMediaServer::CMediaServer(const char *name, CBaseWorker *worker)
    : CBaseServer(name, worker)
{
    mTimer = new CBroadcastTimer(this);
    mTimer->attach(&main_worker, false);
}

void CMediaServer::broadcastElapseTime(CMethodLoopTimer<CMediaServer> *timer)
{
    int8_t hour = 1;
    int16_t minute = 2;
    int32_t second = 3;
    CFdbRawMsgBuilder builder;
    builder.serializer() << hour << minute << second;
    broadcast(NTF_ELAPSE_TIME, builder);
}

void CMediaServer::onOnline(FdbSessionId_t sid, bool is_first)
{
    if (is_first)
    {
        mTimer->enable();
    }
}

void CMediaServer::onOffline(FdbSessionId_t sid, bool is_last)
{
    if (is_last)
    {
        mTimer->disable();
    }
}

void CMediaServer::onInvoke(CBaseJob::Ptr &msg_ref)
{
    auto *msg = castToMessage<CBaseMessage *>(msg_ref);

    switch (msg->code())
    {
    case REQ_RAWDATA:
    {
        CFdbParcelableArray<CPerson> persions;

        CPerson *p = nullptr;
        CCar *c = nullptr;
        p = persions.Add();
        p->mAddress = "Shanghai";
        p->mAge = 22;
        p->mName = "Zhang San";

        c = p->mCars.Add();
        c->mBrand = "Hongqi";
        c->mModel = "H5";
        c->mPrice = 400000;

        c = p->mCars.Add();
        c->mBrand = "ROEWE";
        c->mModel = "X5";
        c->mPrice = 200000;

        p = persions.Add();
        p->mAddress = "Guangzhou";
        p->mAge = 22;
        p->mName = "Li Si";

        c = p->mCars.Add();
        c->mBrand = "Chuanqi";
        c->mModel = "H5";
        c->mPrice = 400000;

        c = p->mCars.Add();
        c->mBrand = "Toyoto";
        c->mModel = "X5";
        c->mPrice = 200000;
        CFdbParcelableBuilder builder(persions);
        msg->reply(msg_ref, builder);
    }
    break;
    default:
        std::cout << "No Found MSG CODE - onInvoke" << std::endl;
        break;
    }
}

void CMediaServer::onSubscribe(CBaseJob::Ptr &msg_ref)
{
    auto *msg = castToMessage<CFdbMessage *>(msg_ref);
    switch (msg->code())
    {
    case REQ_METADATA:
    {
        std::cout << "Server received subscribe." << std::endl;
    }
    break;
    default:
        std::cout << "No Found MSG CODE:" << msg->code() << " - onSubscribe" << std::endl;
        break;
    }
}