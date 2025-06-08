#pragma once

#if defined(MCUDRV_STM32) || defined(MCUDRV_APM32)
#if defined(STM32F4xx) || defined(APM32F4xx)

#include <ucanopen-arm/f4/node/node.hpp>
#include <ucanopen-arm/f4/server/impl/impl_server.hpp>
#include <ucanopen-arm/f4/server/services/heartbeat_service/heartbeat_service.hpp>
#include <ucanopen-arm/f4/server/services/rpdo_service/rpdo_service.hpp>
#include <ucanopen-arm/f4/server/services/sdo_service/sdo_service.hpp>
#include <ucanopen-arm/f4/server/services/sync_service/sync_service.hpp>
#include <ucanopen-arm/f4/server/services/tpdo_service/tpdo_service.hpp>
#include <ucanopen-arm/f4/ucanopen_def.hpp>

#include <vector>

namespace ucanopen {

struct ServerConfig {
  uint32_t node_id;
  uint32_t heartbeat_period_ms; // 0 - is inactive
  uint32_t sync_period_ms;      // 0 - is inactive
  uint32_t tpdo1_period_ms;     // 0 - TPDO is inactive
  uint32_t tpdo2_period_ms;
  uint32_t tpdo3_period_ms;
  uint32_t tpdo4_period_ms;
  uint32_t rpdo1_timeout_ms; // 0 - no RPDO timeout
  uint32_t rpdo2_timeout_ms;
  uint32_t rpdo3_timeout_ms;
  uint32_t rpdo4_timeout_ms;
  uint32_t rpdo1_id; // 0 - use default RPDO ID
  uint32_t rpdo2_id;
  uint32_t rpdo3_id;
  uint32_t rpdo4_id;
};

class Server : public impl::Server,
               public emb::singleton_array<Server, ucan::periph_num> {
protected:
  HeartbeatService* heartbeat_service;
  SyncService* sync_service;
  TpdoService* tpdo_service;
  RpdoService* rpdo_service;
  SdoService* sdo_service;

  std::vector<Node*> nodes;

  std::vector<std::pair<ucan::RxMessageAttribute, impl::FrameReceiver*>>
      rxattr_map_;

  virtual void inspect() {}
public:
  Server(ucan::Module& can_module,
         ServerConfig const& config,
         std::vector<ODEntry>& object_dictionary);

  virtual ~Server() = default;

  static Server* instance(ucan::Peripheral peripheral) {
    return emb::singleton_array<Server, ucan::periph_num>::instance(
        std::to_underlying(peripheral));
  }

  void add_node(Node* node_);

  void start();
  void stop();
  void run();
public:
  void on_frame_received(ucan::Module& can_module,
                         ucan::RxMessageAttribute const& attr,
                         can_frame const& frame);
};

} // namespace ucanopen

#endif
#endif
