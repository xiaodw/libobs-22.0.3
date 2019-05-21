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
    m_queue.push(msg);
}

bool CMsgQueue::PopMsg(Msg* msg)
{
    std::lock_guard<std::mutex> lock(m_lock);
    if (!m_queue.empty())
    {
        *msg = m_queue.front();
        m_queue.pop();
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

