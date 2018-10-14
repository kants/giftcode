#pragma once

#include <vector>
#include <map>
#include <set>
#include <time.h>
#include <boost/smart_ptr.hpp>
#include "boost/smart_ptr/shared_ptr.hpp"
#include "protocol/message/club.pb.h"
#ifdef SERVER_SIDE
#include "boost/date_time/posix_time/time_period.hpp"
#include "thread.h"
#include "model.h"

#include "protocol/message/checkstatus.pb.h"
#include "protocol/message/dream_team.pb.h"
#include "protocol/message/inbox.pb.h"
#include "protocol/message/message.pb.h"
#endif

#include "ai/AIConstant.h"
#include "player/ClubPlayer.h"
#include "messages/IMessage.h"
#include "util/InGameName.h"
#include "util/GeneralRecovery.h"

#define EMPTY -1 
typedef unsigned long long uint64;

enum RESPONSE_TYPE
{
	SUCCESS = 0,
	ERROR_DISCONNECT = 1,
	ERROR_NOT_EXIST = 2,
	ERROR_EXPIRED = 3,
	ERROR_USED = 4,
	ERROR_SAME = 5,
	ERROR_NCHANNEL = 6,//channel discorrect
	ERROR_NVERSION = 7
};

enum EXCHANGE_PRIZE_MODE
{
	UNITY = 0,
	SINGLE
};

class ExchangePrize
{
public:
	int createExchangePrize(const time_t beginT, const time_t endT, const uint32_t batchID, const std::string& giftId, const uint32_t count, const uint16_t mode, const std::string& channel, const std::string& user);
	int createExchangePrize(const boost::shared_ptr<ExchangePrize> val);
	int createExchangePrizes(const std::vector<boost::shared_ptr<ExchangePrize>> vals);
	int delExchangePrize(const std::string& objId);
	int upExchangePrize(const std::string& objId, ExchangePrize& val);

	static int createJExchangePrize();

	std::vector<boost::shared_ptr<ExchangePrize>> GetExchangePrize(const std::string& exchangeObjId);
	static int consumeExchangePrize(uint32_t account, const std::string& exchangeObjId, const std::string& clientversion, const time_t& consumetime);
	static int checkConsume(boost::shared_ptr<Account> acc, const std::string& exchangeId, const std::string& clientversion, const time_t& consumetime, boost::shared_ptr<ExchangePrize> ptr);

	void setExchangeId(const std::string& exchangeId){ exchange_id = exchangeId; }
	std::string getExchangeId(){ return exchange_id; }

	void setName(const std::string& name){ this->name = name; }
	std::string getName(){ return name; }

	void setDesr(const std::string& desr){ this->desr = desr; }
	std::string getDesr(){ return desr; }

	void setMode(const uint8_t mode){ this->mode = mode; }
	uint8_t getMode(){ return mode; }

	void setBatchId(const std::string& batchId){ batch_id = batchId; }
	std::string getBatchId(){ return batch_id; }

	void setGift(const std::string& gift){ this->gift = gift; }
	std::string getGift(){ return gift; }

	void setCount(const uint32_t count){ this->count = count; }
	uint32_t getCount(){ return count; }

	void setReservedCnt(const uint32_t reservedCnt){ reserved_cnt = reservedCnt; }
	uint32_t getReservedCnt(){ return reserved_cnt; }

	void setBeginTm(const std::string& beginTm){ begin_tm = beginTm; }
	std::string getBeginTm(){ return begin_tm; }

	void setEndTm(const std::string& endTm){ end_tm = endTm; }
	std::string getEndTm(){ return end_tm; }

	void setChannel(const std::string& channel){ this->channel = channel; }
	std::string getChannel(){ return channel; }

	void setVersion(const std::string& version){ this->version = version; }
	std::string getVersion(){ return version; }

	void setAccount(const int account){ this->acount = account; }
	int getAccount(){ return acount; }

	void setCreateTm(const time_t& createTm){ create_tm = createTm; }
	time_t  getCreateTm(){ return create_tm; }

	void setCreateAcc(const std::string& createAccount){ create_account = createAccount; }
	std::string getCreateAcc(){ return create_account; }
protected:
	static int pushMail(const uint32_t account, const std::string& giftCode, const std::string& title, const std::string& content, const std::string& gift);
	std::map<int, std::set<std::string>> getGiftCodeM();
private:
	//std::vector<boost::shared_ptr<ExchangePrize>> exchangeprizes;//storage exchangeprizes
	std::string exchange_id;
	std::string name;
	std::string desr;
	uint8_t mode;
	std::string batch_id;
	std::string gift;
	uint32_t count;
	uint32_t reserved_cnt;
	std::string begin_tm;
	std::string end_tm;
	std::string channel;
	std::string version;
	int32_t acount;
	time_t create_tm;
	std::string create_account;
};


class ExchangeGift//gift
{
public:
	int addExchangeGift(const std::string& name, const std::string& gifts);
	int delExchangeGift(const std::string& giftId);
	int upExchangeGift(const std::string& giftId, const std::string& name, const std::string& gifts);
	std::vector<boost::shared_ptr<ExchangeGift>> getExchangeGift();
	std::vector<boost::shared_ptr<ExchangeGift>> getExchangeGift(const std::string& name);

	std::string getName() const { return name; }
	void setName(const std::string& name) { this->name = name; }

	std::string getGifts() const { return gifts; }
	void setGifts(const std::string& gifts) { this->gifts = gifts; }
private:
	std::string name;
	std::string gifts;//gift assemble exp 10 coins: coinsid:10
	std::string giftId;
};

