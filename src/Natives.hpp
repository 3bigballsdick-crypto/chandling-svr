#pragma once

#include "CPlayer.h"
#include "Hooks.hpp"
#include "HandlingEnum.h"
#include "HandlingManager.h"
#include <cstring>
#include "../lib/omp-sdk/include/sdk.hpp"
#include "../lib/omp-sdk/include/Server/Components/Pawn/pawn.hpp"

// Vehicle related funcs hooks
using namespace NativeHook;

void RegisterNativeHooks();
void RegisterNativeHooks()
{
	NativeHookManager::Instance().RegisterHookByName("AddStaticVehicle", [](AMX *amx, cell *params, NativeHook::amx_native_fn_t orig) -> cell
	{
		auto core_ = CHandlingCompo::getCore();
		if (!core_)
		{
			return static_cast<cell>(INVALID_VEHICLE_ID);
		}
		core_->logLn(LogLevel::Debug, "[CHandling] Hooked CreateVehicle");
		int vehicleid = orig(amx, params);
		HandlingMgr::OnCreateVehicle(vehicleid);
		return static_cast<cell>(vehicleid);
	});

	NativeHookManager::Instance().RegisterHookByName("AddStaticVehicle", [](AMX *amx, cell *params, NativeHook::amx_native_fn_t orig) -> cell
	{
		auto core_ = CHandlingCompo::getCore();
		if (!core_)
		{
			return static_cast<cell>(INVALID_VEHICLE_ID);
		}
		core_->logLn(LogLevel::Debug, "[CHandling] Hooked AddStaticVehicle");
		int vehicleid = orig(amx, params);
		HandlingMgr::OnCreateVehicle(vehicleid);
		return static_cast<cell>(vehicleid);
	});

	NativeHookManager::Instance().RegisterHookByName("AddStaticVehicleEx", [](AMX *amx, cell *params, NativeHook::amx_native_fn_t orig) -> cell
	{
		auto core_ = CHandlingCompo::getCore();
		if (!core_)
		{
			return static_cast<cell>(INVALID_VEHICLE_ID);
		}
		core_->logLn(LogLevel::Debug, "[CHandling] Hooked AddStaticVehicleEx");
		int vehicleid = orig(amx, params);
		HandlingMgr::OnCreateVehicle(vehicleid);
		return static_cast<cell>(vehicleid);
	});
}

// Vehicle handling related funcs
SCRIPT_API(GetHandlingAttribType, CHandlingAttribType(CHandlingAttrib attr))
{
	return GetHandlingAttribType(attr);
}

SCRIPT_API(IsPlayerUsingCHandling, bool(IPlayer &player))
{
	int playerid = player.getID();
	if (IsPlayerConnected(playerid))
		return gPlayers[playerid].hasCHandling();

	return false;
}

SCRIPT_API(ResetModelHandling, bool(int modelid))
{
	return HandlingMgr::ResetModelHandling(modelid);
}

SCRIPT_API(ResetVehicleHandling, bool(int vehicleid))
{
	return HandlingMgr::ResetVehicleHandling(vehicleid);
}

SCRIPT_API(SetVehicleHandlingFloat, bool(int vehicleid, CHandlingAttrib attrib, float value))
{
	return HandlingMgr::SetVehicleHandling(vehicleid, attrib, value);
}

SCRIPT_API(SetVehicleHandlingInt, bool(int vehicleid, CHandlingAttrib attrib, int value))
{
	if (GetHandlingAttribType(attrib) == TYPE_BYTE)
		return HandlingMgr::SetVehicleHandling(vehicleid, attrib, (uint8_t)value);

	return HandlingMgr::SetVehicleHandling(vehicleid, attrib, (unsigned int)value);
}

SCRIPT_API(SetModelHandlingFloat, bool(int modelid, CHandlingAttrib attrib, float value))
{
	return HandlingMgr::SetModelHandling((uint16_t)modelid, attrib, value);
}

SCRIPT_API(SetModelHandlingInt, bool(int modelid, CHandlingAttrib attrib, int value))
{
	if (GetHandlingAttribType(attrib) == TYPE_BYTE)
		return HandlingMgr::SetModelHandling((uint16_t)modelid, attrib, (uint8_t)value);

	return HandlingMgr::SetModelHandling((uint16_t)modelid, attrib, (unsigned int)value);
}

SCRIPT_API(GetVehicleHandlingFloat, bool(int vehicleid, CHandlingAttrib attrib, float &value))
{
	float val = 0.0;
	bool ret = HandlingMgr::GetVehicleHandling((uint16_t)vehicleid, attrib, val);

	cell *ref = NULL;
	amx_GetAddr(amx, value, &ref);
	if (!ref)
		return false;
	*ref = amx_ftoc(val);
	return ret;
}

SCRIPT_API(GetVehicleHandlingInt, bool(int vehicleid, CHandlingAttrib attrib, int &value))
{
	bool ret = false;
	cell *ref = NULL;
	amx_GetAddr(amx, value, &ref);
	if (!ref)
		return false;

	if (GetHandlingAttribType(attrib) == TYPE_BYTE)
	{
		uint8_t val = 0;
		ret = HandlingMgr::GetVehicleHandling((uint16_t)vehicleid, attrib, val);
		*ref = (cell)val;
	}
	else
	{
		unsigned int val = 0;
		ret = HandlingMgr::GetVehicleHandling((uint16_t)vehicleid, attrib, val);
		*ref = (cell)val;
	}
	return ret;
}

SCRIPT_API(GetModelHandlingFloat, bool(int modelid, CHandlingAttrib attrib, float &value))
{
	float val = 0.0;
	bool ret = HandlingMgr::GetModelHandling((uint16_t)modelid, attrib, val);

	cell *ref = NULL;
	amx_GetAddr(amx, value, &ref);
	if (!ref)
		return false;
	*ref = amx_ftoc(val);
	return ret;
}

SCRIPT_API(GetModelHandlingInt, bool(int modelid, CHandlingAttrib attrib, int &value))
{
	bool ret = false;
	cell *ref = NULL;
	amx_GetAddr(amx, value, &ref);
	if (!ref)
		return false;

	if (GetHandlingAttribType(attrib) == TYPE_BYTE)
	{
		uint8_t val = 0;
		ret = HandlingMgr::GetModelHandling((uint16_t)modelid, attrib, val);
		*ref = (cell)val;
	}
	else
	{
		unsigned int val = 0;
		ret = HandlingMgr::GetModelHandling((uint16_t)modelid, attrib, val);

		*ref = (cell)val;
	}
	return ret;
}

SCRIPT_API(GetDefaultHandlingFloat, bool(int modelid, CHandlingAttrib attrib, float &value))
{
	cell *ref = NULL;
	amx_GetAddr(amx, value, &ref);
	if (!ref)
		return false;

	float val = 0.0;
	bool ret = HandlingMgr::GetDefaultHandling((uint16_t)modelid, attrib, val);

	*ref = amx_ftoc(val);
	return ret;
}

SCRIPT_API(GetDefaultHandlingInt, bool(int modelid, CHandlingAttrib attrib, int &value))
{
	bool ret = false;
	cell *ref = NULL;
	amx_GetAddr(amx, value, &ref);
	if (!ref)
		return false;

	if (GetHandlingAttribType(attrib) == TYPE_BYTE)
	{
		uint8_t val = 0;
		ret = HandlingMgr::GetModelHandling((uint16_t)modelid, attrib, val);
		*ref = (cell)val;
	}
	else
	{
		unsigned int val = 0;
		ret = HandlingMgr::GetModelHandling((uint16_t)modelid, attrib, val);
		*ref = (cell)val;
	}
	return ret;
}
