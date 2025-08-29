#pragma once

#include <ucanopen-arm/f4/server/impl/impl_server.hpp>

#include <emb/chrono.hpp>

namespace ucanopen {

class TpdoService {
private:
  impl::Server& server_;

  struct Message {
    canid_t id;
    static constexpr uint8_t len = 8;
    std::chrono::milliseconds period;
    std::chrono::time_point<emb::chrono::steady_clock> timepoint;
    canpayload_t (*creator)();
  };

  std::array<Message, 4> messages_;
public:
  TpdoService(impl::Server& server);
  void register_tpdo(CobTpdo tpdo,
                     std::chrono::milliseconds period,
                     canpayload_t (*creator)());
  void send();
};

} // namespace ucanopen
