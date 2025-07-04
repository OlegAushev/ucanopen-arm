#if defined(MCUDRV_STM32) || defined(MCUDRV_APM32)
#if defined(STM32F4xx) || defined(APM32F4xx)

#include <ucanopen-arm/f4/server/services/tpdo_service/tpdo_service.hpp>

namespace ucanopen {

TpdoService::TpdoService(impl::Server& server) : server_(server) {
  for (size_t i = 0; i < messages_.size(); ++i) {
    messages_[i].id = calculate_cob_id(to_cob(CobTpdo(i)), server_.node_id());
    messages_[i].period = std::chrono::milliseconds{0};
    messages_[i].timepoint = {};
    messages_[i].creator = nullptr;
  }
}

void TpdoService::register_tpdo(CobTpdo tpdo,
                                std::chrono::milliseconds period,
                                canpayload_t (*creator)()) {
  messages_[std::to_underlying(tpdo)].period = period;
  messages_[std::to_underlying(tpdo)].timepoint =
      emb::chrono::steady_clock::now();
  messages_[std::to_underlying(tpdo)].creator = creator;
}

void TpdoService::send() {
  auto now = emb::chrono::steady_clock::now();
  for (auto& msg : messages_) {
    if (!msg.creator || msg.period.count() <= 0) {
      continue;
    }
    if (now < msg.timepoint + msg.period) {
      continue;
    }

    canpayload_t payload = msg.creator();
    server_.can_module_.put_frame({msg.id, msg.len, payload});
    msg.timepoint = now;
  }
}

} // namespace ucanopen

#endif
#endif
