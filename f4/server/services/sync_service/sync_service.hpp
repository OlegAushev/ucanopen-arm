#pragma once

#include <ucanopen-arm/f4/server/impl/impl_server.hpp>

#include <emb/chrono.hpp>

namespace ucanopen {

class SyncService {
private:
  impl::Server& server_;
  canid_t id_;
  static constexpr uint8_t len_ = cob_sizes[std::to_underlying(Cob::sync)];
  std::chrono::milliseconds period_;
  std::chrono::time_point<emb::chrono::steady_clock> timepoint_;
public:
  SyncService(impl::Server& server, std::chrono::milliseconds period);
  void send();
};

} // namespace ucanopen
