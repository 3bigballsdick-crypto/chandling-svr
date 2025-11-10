#include "chandlingsvr.h"

#include "Hooks.hpp"
#include "CPlayer.h"
#include "Natives.hpp"
#include "HandlingManager.h"
#include "HandlingDefault.h"

using namespace NativeHook;

StringView CHandlingCompo::componentName() const
{
	return "CHandling";
}

SemanticVersion CHandlingCompo::componentVersion() const
{
	return SemanticVersion(CHANDLING_VERSION_MAJOR, CHANDLING_VERSION_MINOR, CHANDLING_VERSION_PATCH, 0);
}

void CHandlingCompo::onLoad(ICore *c)
{
	core_ = c;

	getCore() = c;
	get() = this;

	core_->getPlayers().getPlayerConnectDispatcher().addEventHandler(this);
}

IVehicle* CHandlingCompo::GetVehicleByID(int vehicleid)
{
	if (!IS_VALID_VEHICLEID(vehicleid))
		return nullptr;

	for (auto it = vehicles_->begin(); it != vehicles_->end(); ++it)
	{
		IVehicle &vehicle = *it;
		if (vehicle.getID() == vehicleid)
		{
			return &vehicle;
		}
	}
	return nullptr;
}

bool CHandlingCompo::IsValidVehicle(int vehicleid)
{
	if (!IS_VALID_VEHICLEID(vehicleid))
		return false;

	for (auto it = vehicles_->begin(); it != vehicles_->end(); ++it)
	{
		IVehicle &vehicle = *it;
		if (vehicle.getID() == vehicleid)
		{
			return true;
		}
	}
	return false;
}

IPlayer* CHandlingCompo::GetPlayerByID(int playerid)
{
	if (!IS_VALID_PLAYERID(playerid))
		return nullptr;

	for (auto it = players_->begin(); it != players_->end(); ++it)
	{
		IPlayer &player = *it;
		if (player.getID() == playerid)
		{
			return &player;
		}
	}
	return nullptr;
}

void CHandlingCompo::onInit(IComponentList *components)
{
	pawn_component_ = components->queryComponent<IPawnComponent>();
	if (!pawn_component_)
	{
		StringView name = componentName();

		core_->logLn(LogLevel::Error,
					 "Error loading component %.*s: Pawn component not loaded",
					 name.length(), name.data());

		return;
	}

	core_->getEventDispatcher().addEventHandler(this);
	if (pawn_component_)
	{
		setAmxFunctions(getAmxFunctions());
		setAmxLookups(components);
		pawn_component_->getEventDispatcher().addEventHandler(this);
	}

	vehicles_ = components->queryComponent<IVehiclesComponent>();
	if (!vehicles_)
	{
		StringView name = componentName();

		core_->logLn(LogLevel::Error,
					 "Error loading component %.*s: Vehicles component not loaded",
					 name.length(), name.data());

		return;
	}
	if (vehicles_)
	{
		vehicles_->getPoolEventDispatcher().addEventHandler(this);
	}

	for (auto network : core_->getNetworks())
	{
		network->getInEventDispatcher().addEventHandler(this);
		network->getOutEventDispatcher().addEventHandler(this);
	}

	AMX_EXPORTS_DTA = pawn_component_->getAmxFunctions().data();
}

void CHandlingCompo::onAmxLoad(IPawnScript &script)
{
	HandlingDefault::Initialize();
	HandlingMgr::InitializeModelHandlings();
	pawn_component_::AmxLoad(script.GetAMX());
	NativeHookManager::Instance().LoadAMX(script.GetAMX());
	RegisterNativeHooks();

	core_->logLn(LogLevel::Message, "");
	core_->logLn(LogLevel::Message, " =======================================================================");
	core_->logLn(LogLevel::Message, "  CHandlingSvr %.*d.%.*d.%.*d%.*s by .silent (adapted by Zorono) loaded!", CHANDLING_VERSION_MAJOR, CHANDLING_VERSION_MINOR, CHANDLING_VERSION_PATCH, (CHANDLING_PHASE_DEV ? "-dev" : ""));
	core_->logLn(LogLevel::Message, " =======================================================================");
	core_->logLn(LogLevel::Message, "");
};

void CHandlingCompo::onAmxUnload(IPawnScript &script)
{
	pawn_component_::AmxUnload();
	NativeHookManager::Instance().UnloadAMX(script.GetAMX());

	core_->logLn(LogLevel::Message, "");
	core_->logLn(LogLevel::Message, " =========================================================================");
	core_->logLn(LogLevel::Message, "  CHandlingSvr %.*d.%.*d.%.*d%.*s by .silent (adapted by Zorono) unloaded!", CHANDLING_VERSION_MAJOR, CHANDLING_VERSION_MINOR, CHANDLING_VERSION_PATCH, (CHANDLING_PHASE_DEV ? "-dev" : ""));
	core_->logLn(LogLevel::Message, " =========================================================================");
	core_->logLn(LogLevel::Message, "");
};

void CHandlingCompo::onTick(Microseconds elapsed, TimePoint now)
{
	HandlingMgr::ProcessTick();
}

bool CHandlingCompo::onReceivePacket(IPlayer &peer, int id, NetworkBitStream &bs)
{
	if (id == (uint8_t)ID_CHANDLING)
	{
		core_->logLn(LogLevel::Debug, "[CHandling] Received custom packet ID %.*d from player %.*d (size=%.*d)\n", id, peer.getID(), bs.GetNumberOfBytesUsed());
		if (bs.GetNumberOfUnreadBytes() > 1)
		{
			uint8_t action;
			bs.Read(action);

			Actions::Process((CHandlingAction)action, bs, peer);
		}
	}
	return true;
}

void CHandlingCompo::onFree(IComponent *component)
{
	if (component == pawn_component_)
	{
		if (pawn_component_)
		{
			pawn_component_->getEventDispatcher().removeEventHandler(this);
		}

		pawn_component_ = nullptr;
	}
	else if (component == vehicles_)
	{
		if (vehicles_)
		{
			vehicles_->getEventDispatcher().removeEventHandler(this);
		}

		vehicles_ = nullptr;
	}
	else if (component == this)
	{
		core_->getEventDispatcher().removeEventHandler(this);
	}
}

void CHandlingCompo::reset() {}

void CHandlingCompo::free()
{
	delete this;
}

ICore *&CHandlingCompo::getCore()
{
	static ICore *core{};

	return core;
}

CHandlingCompo *&CHandlingCompo::get()
{
	static CHandlingCompo *component{};

	return component;
}

COMPONENT_ENTRY_POINT()
{
	return new CHandlingCompo();
}

bool onIncomingConnection(IPlayer &player, const char *ip_addr, int port)
{
	int playerid = player.getID();
	gPlayers[playerid].Reset();
	return false;
}

bool onPlayerConnect(IPlayer &player)
{
	core_->logLn(LogLevel::Debug, "[CHandling] OnPlayerConnect");
	HandlingMgr::OnPlayerConnect(player);
	return true;
}

bool onVehicleStreamIn(IVehicle &vehicle, IPlayer &forplayer)
{
	core_->logLn(LogLevel::Debug, "[CHandling] OnVehicleStreamIn(%d,%d)", vehicle.getID(), forplayer.getID());

	// Send handling modifications for this vehicle
	HandlingMgr::OnVehicleStreamIn(vehicle, forplayer);
	return true;
}
