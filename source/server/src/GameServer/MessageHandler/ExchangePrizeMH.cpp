#include "stdafx.h"
#include "ExchangePrizeMH.h"
#include "protocol/message/exchange_prize.pb.h"
#include "account/Account.h"
#include "boost/shared_ptr.hpp"
#include "mission/MissionSetting.h"
#include "SessionManager/EventDispatcher.h"
#include "exchangeprize/ExchangePrize.h"

GetExchangePrizeMH::GetExchangePrizeMH()
{
}

GetExchangePrizeMH::~GetExchangePrizeMH()
{
}

bool GetExchangePrizeMH::handleMessage(Event* evt, ClientSession* session)
{
	GetExchangePrize msg;
	if (!parseMessage(evt, msg))
		return false;
	uint32_t account = msg.account();
	std::string exchangePrizeId = msg.exchangeid();
	std::string clientVersion = msg.version();
	time_t consumeTime = msg.time();

	int res = ExchangePrize::consumeExchangePrize(account,exchangePrizeId,clientVersion,consumeTime);

	RepExchangePrize rmsg;
	rmsg.set_responsetype(res);
	rmsg.set_account(account);
	rmsg.set_opticode(OP_REQUEST_EXCHANGE_PRIZE);
	rmsg.set_desr("");
	evt->setOpcode(OP_REQUEST_EXCHANGE_PRIZE);
	responseMessage(session, evt, rmsg);
	return true;
}

REGISTER_OPCODE_WITH_HANDLER(OP_SEND_EXCHANGE_PRIZE, GetExchangePrizeMH);
