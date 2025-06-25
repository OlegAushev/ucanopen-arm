#pragma once

#if defined(MCUDRV_STM32) || defined(MCUDRV_APM32)
#if defined(STM32F4xx) || defined(APM32F4xx)

#include <ucanopen-arm/f4/server/impl/impl_server.hpp>

#include <emblib/chrono.hpp>

namespace ucanopen {

class RpdoService : public impl::FrameReceiver {
private:
  impl::Server& server_;

  struct Message {
    ucan::RxMessageAttribute attr;
    std::chrono::milliseconds timeout;
    std::chrono::time_point<emb::chrono::steady_clock> timepoint;
    bool unhandled;
    can_frame frame;
    void (*handler)(canpayload_t const&);
  };

  std::array<Message, 4> messages_;
public:
  RpdoService(impl::Server& server);

  void register_rpdo(CobRpdo rpdo,
                     std::chrono::milliseconds timeout,
                     void (*handler)(canpayload_t const&),
                     canid_t id = 0);

  virtual std::vector<ucan::RxMessageAttribute> get_rx_attr() const override;
  virtual void recv(ucan::RxMessageAttribute const& attr,
                    can_frame const& frame) override;
  virtual void handle() override;

  bool good(CobRpdo rpdo) {
    if (messages_[std::to_underlying(rpdo)].timeout.count() <= 0) {
      return true;
    }
    if (emb::chrono::steady_clock::now() <=
        messages_[std::to_underlying(rpdo)].timepoint +
            messages_[std::to_underlying(rpdo)].timeout) {
      return true;
    }
    return false;
  }
};

} // namespace ucanopen

#endif
#endif
