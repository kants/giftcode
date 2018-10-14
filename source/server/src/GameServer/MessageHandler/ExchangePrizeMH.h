#pragma once

#include "GameServer/GameMessageHandler.h"
#include <list>

class Player;
class GetExchangePrizeMH : public GameMessageHandler
{
public:
	GetExchangePrizeMH();
	virtual ~GetExchangePrizeMH();

public:
	virtual bool handleMessage(Event* evt, ClientSession* session);
};

class RepExchangePrizeMH : public GameMessageHandler
{
public:
	RepExchangePrizeMH();
	virtual ~RepExchangePrizeMH();

public:
	virtual bool handleMessage(Event* evt, ClientSession* session);
};


