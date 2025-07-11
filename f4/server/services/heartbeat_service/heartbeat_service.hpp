#pragma once

#if defined(MCUDRV_STM32) || defined(MCUDRV_APM32)
#if defined(STM32F4xx) || defined(APM32F4xx)

#include <ucanopen-arm/f4/server/impl/impl_server.hpp>

#include <emblib/chrono.hpp>

namespace ucanopen {

class HeartbeatService {
private:
  impl::Server& server_;
  canid_t id_;
  static constexpr uint8_t len_ = cob_sizes[std::to_underlying(Cob::heartbeat)];
  std::chrono::milliseconds period_;
  std::chrono::time_point<emb::chrono::steady_clock> timepoint_;
public:
  HeartbeatService(impl::Server& server, std::chrono::milliseconds period);
  void send();
};

} // namespace ucanopen

#endif
#endif
