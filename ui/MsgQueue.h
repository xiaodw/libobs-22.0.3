#pragma once
#include <deque>
#include <mutex>
#include <memory>

class CMsgData
{
public:
    virtual ~CMsgData() {}
};


template<class T>
class CTypedMsgData :public CMsgData
{
public:
    CTypedMsgData(T d)
        :data(d)
    {
    }
    T data;
};


class CMsgHandler
{
public:
    virtual void OnMsg(unsigned int msgid, CMsgData* data) = 0;
};

class CMsgQueue
{
public:
    struct Msg {
        unsigned int msgid;
        CMsgHandler* handler;
        std::shared_ptr<CMsgData> data;
    };

    CMsgQueue();
    ~CMsgQueue();

    bool CheckMsg(unsigned int msgid, CMsgHandler* handler);
    void PostMsg(unsigned int msgid, CMsgHandler* handler,
        std::shared_ptr<CMsgData> data);

    void PostOnce(unsigned int msgid, CMsgHandler* handler,
        std::shared_ptr<CMsgData> data);
    void HandleMsg();
private:
    bool PopMsg(Msg* msg);


    std::deque<Msg> m_queue;
    std::mutex m_lock;
};

