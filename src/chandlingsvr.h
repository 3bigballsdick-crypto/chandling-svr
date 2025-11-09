#include "../lib/omp-raknet/Include/raknet/BitStream.h"
#include "../lib/omp-raknet/Include/raknet/StringCompressor.h"
#include "../lib/omp-raknet/Include/raknet/PluginInterface.h"
#include "../lib/omp-raknet/Include/raknet/PacketEnumerations.h"

#include "../lib/omp-sdk/include/sdk.hpp"
#include "../lib/omp-sdk/include/Server/Components/Pawn/pawn.hpp"
#include "../lib/omp-sdk/include/Impl/network_impl.hpp"
#include "../lib/omp-sdk/include/Server/Components/Vehicles/vehicles.hpp"

#define CHANDLING_PHASE_DEV true
#define CHANDLING_VERSION_MAJOR 1
#define CHANDLING_VERSION_MINOR 0
#define CHANDLING_VERSION_PATCH 0

/*
 * The compatibility version number decides if the client supports our CHandling version or not
 * Clients with smaller compat version won't be able to use CHandling
 *
 * Increase this number only if doing things that break the compatibility with older client
 */
#define CHANDLING_COMPAT_VERSION 0x1001D

#define MAX_VEHICLE_MODELS (212)
#ifndef MAX_VEHICLES
#define MAX_VEHICLES (2000)
#endif

// keep in mind  vehicle ids in samp start from 1
#define IS_VALID_VEHICLEID(id) \
	(id >= 1 && id <= MAX_VEHICLES)

#define IS_VALID_VEHICLE_MODEL(modelid) \
	(modelid >= 400 && modelid < MAX_VEHICLE_MODELS+400)

#define VEHICLE_MODEL_INDEX(modelid) \
	(modelid - 400)

class CHandlingCompo final : public IComponent,
							 public PawnEventHandler,
							 public CoreEventHandler,
							 public NetworkInEventHandler,
							 public NetworkOutEventHandler,
							 public PoolEventHandler<IVehicle>
{
public:
	PROVIDE_UID(0xFBE076EB9EA67E4C);

	StringView componentName() const override;

	SemanticVersion componentVersion() const override;

	void onLoad(ICore *c) override;

	void onInit(IComponentList *components) override;

	void onAmxLoad(IPawnScript &script) override;

	void onAmxUnload(IPawnScript &script) override;

	void onTick(Microseconds elapsed, TimePoint now) override;

	bool onReceivePacket(IPlayer &peer, int id, NetworkBitStream &bs) override;

	void onFree(IComponent *component) override;

	void reset() override;

	void free() override;

	static ICore *&getCore();

	static CHandlingCompo *&get();

	static IVehicle *GetVehicleByID(int vehicleid);
	static bool IsValidVehicle(int vehicleid);
	static IPlayer *GetPlayerByID(int playerid);

private:
	ICore *core_{};
	IPawnComponent *pawn_component_{};
	IVehiclesComponent *vehicles_ = nullptr;

public:
	void **AMX_EXPORTS_DTA = nullptr;
};
