#include "MsgQueue.h"

CMsgQueue::CMsgQueue()
{

}

CMsgQueue::~CMsgQueue()
{

}

void CMsgQueue::PostMsg(unsigned int msgid, CMsgHandler* handler, std::shared_ptr<CMsgData> data)
{
    std::lock_guard<std::mutex> lock(m_lock);
    Msg msg;
    msg.data = data;
    msg.handler = handler;
    msg.msgid = msgid;
    m_queue.push_back(msg);
}

void CMsgQueue::PostOnce(unsigned int msgid, CMsgHandler* handler,
    std::shared_ptr<CMsgData> data)
{
    if (CheckMsg(msgid, handler))
        return;
    PostMsg(msgid, handler, data);
}

bool CMsgQueue::CheckMsg(unsigned int msgid, CMsgHandler* handler)
{
    std::lock_guard<std::mutex> lock(m_lock);
    for (auto& data : m_queue)
    {
        if (data.msgid == msgid && data.handler == handler)
            return true;
    }
    return false;
}

bool CMsgQueue::PopMsg(Msg* msg)
{
    std::lock_guard<std::mutex> lock(m_lock);
    if (!m_queue.empty())
    {
        *msg = m_queue.front();
        m_queue.pop_front();
        return true;
    }
    return false;
}

void CMsgQueue::HandleMsg()
{
    Msg msg;
    while (PopMsg(&msg))
    {
        msg.handler->OnMsg(msg.msgid, msg.data.get());
    }
}

