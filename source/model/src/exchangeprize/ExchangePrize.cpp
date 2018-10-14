#include "ExchangePrize.h"
#ifdef SERVER_SIDE
#include "database/ExchangePrizeD.h"
#include "database/AccountDatabase.h"
#include "database/ClubD.h"
#include <boost/algorithm/string.hpp>
#include "GameServer/GameModel/InboxManager.h"
#include "util/StringUtil.h"
#include "GameServer/GameModel/LoggerManager.h"
#include "GameServer/GameModel/ActivityManager.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/filestream.h"
#include "rapidjson/document.h"

#define CODE_LEN 14
typedef std::vector<boost::shared_ptr<ExchangePrize>>::iterator exchangeprize_iter;

extern std::string getClubActivityLabel(boost::shared_ptr<Club>& club);
int ExchangePrize::consumeExchangePrize(uint32_t account, const std::string& exchangeId, const std::string& clientversion, const time_t& consumetime)
{
	int iret = ERROR_NOT_EXIST;
	auto acc = AccountD::dbFindAccountById(account);
	if (createJExchangePrize() == -1) iret = ERROR_DISCONNECT;
	else
	{
		std::vector<boost::shared_ptr<ExchangePrize>> tvec = ExchangePrizeD::GetExchangePrize(exchangeId);

		if (tvec.size() > 0)
		{
			//if (tvec.size() > 1) 
			//logerror;
			exchangeprize_iter pvalue = tvec.begin();
			iret = checkConsume(acc, exchangeId, clientversion, consumetime, *pvalue);
		}
	}
	auto club = ClubD::GetByAccount(account);
	std::string clabel = "";
	if (club)
		clabel = getClubActivityLabel(club);
	LoggerManager::getSingleton().logGiftCode(consumetime, account, exchangeId, acc->getChannel(), clientversion, club->getGrade(),clabel, iret);
	return iret;
}

int ExchangePrize::checkConsume(boost::shared_ptr<Account> acc, const std::string& exchangeId, const std::string& clientversion, const time_t& consumetime, boost::shared_ptr<ExchangePrize> ptr)
{
	uint32_t account = acc->getUid();
	if (ptr->acount != EMPTY)
	{
		if (account != ptr->acount) return ERROR_NOT_EXIST;
	}

	time_t beginT = ParseDateTime(ptr->begin_tm);
	time_t endT = ParseDateTime(ptr->end_tm);
	if (beginT > consumetime || endT < consumetime) return ERROR_EXPIRED;

	uint8_t iMode = ptr->getMode();
	if (iMode == UNITY)
	{
		if (ptr->getReservedCnt() <= 0) return ERROR_USED;
	}

	std::string ccontent = acc->getLanguagesWithKey("MAIL_GIFTCODE_CONTENT");
	char cbuf[500] = { 0 };

	std::string cgiftcode;
	cgiftcode.append(exchangeId).append(gift_code_alias);
	sprintf(cbuf, ccontent.c_str(), cgiftcode.c_str());

	std::string content(cbuf);

	std::string title = acc->getLanguagesWithKey("MAIL_GIFTCODE_TITTLE");

	if (ExchangeConsumeD::bcheckConsumed(account, exchangeId)) return ERROR_SAME;

	std::string channel = acc->getChannel();
	std::string prizechannel = ptr->getChannel();
	if (!prizechannel.empty())
	{
		if (channel.compare(prizechannel) != 0) return ERROR_NCHANNEL;
	}

	if (clientversion.compare(ptr->getVersion()) < 0) return ERROR_NVERSION;

	if (pushMail(account, exchangeId, title, content, ptr->getGift()) != 0) return ERROR_DISCONNECT;

	if (ExchangePrizeD::minusPrizeCnt(ptr->exchange_id) == -1) return ERROR_DISCONNECT;

	return SUCCESS;
}

int ExchangePrize::pushMail(const uint32_t account, const std::string& giftCode, const std::string& title, const std::string& content ,const std::string& gift)
{
	//std::string gift = ExchangeGiftD::getExchangeGift(giftCode);
	if (gift.empty()) return -1;
	std::vector<IMessage> mailVec;
	IMessage msg;
	msg.setSendId(0);
	msg.setReceiverId(account);
	msg.setContent(content);
	msg.setTitle(title);
	msg.setDetailType(MSG_GIFTCODE);

	if (gift.find("13:") == std::string::npos)
	{
		msg.setAttachment(gift);
		mailVec.push_back(msg);
		InboxManager::getSingleton().sendSystemMessages(mailVec);
		return 0;
	}

	std::string giftnplayer;
	std::vector<std::string> gifts;
	boost::algorithm::split(gifts, gift, boost::is_any_of(","));

	for (auto iter_begin = gifts.begin(); iter_begin != gifts.end(); ++iter_begin)
	{
		std::size_t pos = (*iter_begin).find("13:");
		if (pos != std::string::npos)
		{
			msg.setAttachment(*iter_begin);
			mailVec.push_back(msg);
		}
		else
		{
			giftnplayer.append(*iter_begin).append(",");
		}
	}
	msg.setAttachment(giftnplayer.substr(0, giftnplayer.length() - 1));
	mailVec.push_back(msg);

	InboxManager::getSingleton().sendSystemMessages(mailVec);
	return 0;
}

//reserved for gift json
int LoadGiftArray(const rapidjson::Value& val)
{
	if (val.IsArray())
	{
		for (auto it = val.Begin(); it != val.End(); it++)
		{
			if (it->IsArray() && it->Size() == 2)
			{
				ExchangeGift tgift;
				tgift.setName((*it)[0].GetString());
				tgift.setGifts((*it)[1].GetString());
				//ExchangeGiftD::createExchangeGift(tgift);
			}
		}
	}
	return 0;
}

std::set<std::string> giftcodeSet;
std::vector<std::string> newcodes;

std::string rand_str(const std::string batch,int cnt)
{
	newcodes.clear();
	srand(time(nullptr));
	char strcode[CODE_LEN + 1] = { 0 };
	while (cnt>0)
	{
		char str[CODE_LEN] = { 0 };
		for (int i = 0; i < CODE_LEN - 4; i++)
		{
			switch (rand() % 4)
			{
			case 0:
				str[i] = 'A' + rand() % 8;
				break;
			case 1:
				str[i] = 'J' + rand() % 5;
				break;
			case 2:
				str[i] = 'P' + rand() % 11;
				break;
			case 3:
				str[i] = '1' + rand() % 9;
				break;
			default:
				break;
			}
		}
		sprintf(strcode, "%s%s", batch.c_str(), str);
		if (giftcodeSet.find(strcode) != giftcodeSet.end()) continue;
		else 
		{
			giftcodeSet.insert(strcode);
			newcodes.push_back(strcode);
			--cnt;
		}
	}
	return strcode;
}

std::string createGiftCode(const std::string& batchId,int cnt)
{
	if (giftcodeSet.size() <= 0)
	{
		giftcodeSet = ExchangePrizeD::getGiftCodeSet(batchId);
	}

	return rand_str(batchId, cnt);
}

int LoadPrizeArray(const rapidjson::Value& val)
{
	std::vector<boost::shared_ptr<ExchangePrize>> prizeVec;
	if (val.IsArray())
	{
		for (auto it = val.Begin(); it != val.End(); it++)
		{
			if (it->IsArray() && it->Size() <= 13)
			{
				ExchangePrize tprize;
				tprize.setName((*it)[0].GetString());
				tprize.setDesr((*it)[1].GetString());
				tprize.setMode((*it)[2].GetInt());
				std::string batch = (*it)[3].GetString();
				tprize.setBatchId(batch);
				tprize.setGift((*it)[4].GetString());
				tprize.setCount((*it)[5].GetInt());
				tprize.setReservedCnt((*it)[5].GetInt());

				std::string beginTm = (*it)[6].GetString();
				tprize.setBeginTm(beginTm);

				std::string endTm = (*it)[7].GetString();
				tprize.setEndTm(endTm);

				tprize.setChannel((*it)[8].GetString());
				tprize.setVersion((*it)[9].GetString());
				tprize.setAccount((*it)[10].GetInt());
				tprize.setCreateAcc((*it)[11].GetString());
				tprize.setCreateTm(0);

				int batch_count = (*it)[12].GetInt();
				createGiftCode(batch, batch_count);
				for (int i = 0; i < batch_count;i++)
				{
					tprize.setExchangeId(newcodes[i]);
					prizeVec.emplace_back(boost::make_shared<ExchangePrize>(tprize));
				}
			}
		}
	}

	return ExchangePrizeD::createExchangePrize(prizeVec);
}

int ExchangePrize::createJExchangePrize()
{
	const char *filename = "data/exchange_prize.json";
	FILE *fp = fopen(filename, "r");
	if (!fp)
		return -1;
	rapidjson::Document document;
	rapidjson::FileStream fs(fp);
	document.ParseStream<0>(fs);
	if (document.IsObject()) {
		rapidjson::Value& prize_node = document["exchange_prize"];
		LoadPrizeArray(prize_node);
	}
	fclose(fp);
	return 0;
}

#endif