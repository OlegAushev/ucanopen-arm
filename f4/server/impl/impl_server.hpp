#pragma once

#if defined(MCUDRV_STM32) || defined(MCUDRV_APM32)
#if defined(STM32F4xx) || defined(APM32F4xx)

#include <ucanopen-arm/f4/ucanopen_def.hpp>
#if defined(MCUDRV_STM32)
#include <mcudrv/stm32/f4/can/can.h>
namespace ucan = mcu::stm32::can;
#elif defined(MCUDRV_APM32)
#include <mcudrv-apm32/f4/can/can.hpp>
namespace ucan = mcu::apm32::can;
#endif
#include <algorithm>
#include <emblib/algorithm.hpp>
#include <vector>

namespace ucanopen {

class HeartbeatService;
class SyncService;
class TpdoService;
class RpdoService;
class SdoService;
class Node;

namespace impl {

class Server {
  friend class ucanopen::HeartbeatService;
  friend class ucanopen::SyncService;
  friend class ucanopen::TpdoService;
  friend class ucanopen::RpdoService;
  friend class ucanopen::SdoService;
  friend class ucanopen::Node;
protected:
  NodeId node_id_;
  ucan::Module& can_module_;
  std::vector<ODEntry>& dictionary_;
  NmtState nmt_state_;
public:
  Server(ucan::Module& can_module,
         NodeId node_id,
         std::vector<ODEntry>& object_dictionary);

  NodeId node_id() const { return node_id_; }

  NmtState nmt_state() const { return nmt_state_; }
protected:
  virtual void on_sdo_overrun() {}

  virtual void on_rpdo_overrun() {}
private:
  void init_object_dictionary();
public:
  ODEntry const* find_od_entry(ODObjectKey key) {
    auto res = std::equal_range(dictionary_.begin(), dictionary_.end(), key);
    if (res.first == res.second) {
      return nullptr;
    }
    return &(*res.first);
  }
};

class FrameReceiver {
public:
  virtual std::vector<ucan::RxMessageAttribute> get_rx_attr() const = 0;
  virtual void recv(ucan::RxMessageAttribute const&, can_frame const&) = 0;
  virtual void handle() = 0;
};

} // namespace impl
} // namespace ucanopen

#endif
#endif
