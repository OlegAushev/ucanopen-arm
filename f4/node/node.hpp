#pragma once

#if defined(MCUDRV_STM32) || defined(MCUDRV_APM32)
#if defined(STM32F4xx) || defined(APM32F4xx)

#include <ucanopen-arm/f4/server/impl/impl_server.hpp>

#include <emb/chrono.hpp>
#include <vector>

namespace ucanopen {

class Server;

class Node : public impl::FrameReceiver {
private:
  ucan::Module& can_module_;

  struct RxMessage {
    ucan::RxMessageAttribute attr;
    std::chrono::milliseconds timeout;
    std::chrono::time_point<emb::chrono::steady_clock> timepoint;
    bool unhandled;
    can_frame frame;
    void (*handler)(canpayload_t const&);
  };

  std::vector<RxMessage> rx_messages_;

  struct TxMessage {
    std::chrono::milliseconds period;
    std::chrono::time_point<emb::chrono::steady_clock> timepoint;
    canid_t id;
    uint8_t len;
    canpayload_t (*creator)();
  };

  std::vector<TxMessage> tx_messages_;
public:
  Node(Server& server);
#if defined(MCUDRV_STM32)
  void register_rx_message(CAN_FilterTypeDef& filter,
                           std::chrono::milliseconds timeout,
                           void (*handler)(const canpayload_t&));
#elif defined(MCUDRV_APM32)
  void register_rx_message(CAN_FilterConfig_T& filter,
                           std::chrono::milliseconds timeout,
                           void (*handler)(const canpayload_t&));
#endif
  void register_tx_message(canid_t id,
                           uint8_t len,
                           std::chrono::milliseconds period,
                           canpayload_t (*creator)());

  virtual std::vector<ucan::RxMessageAttribute> get_rx_attr() const override;
  virtual void recv(ucan::RxMessageAttribute const& attr,
                    can_frame const& frame) override;
  virtual void handle() override;

  void send();

  virtual void inspect() {}

  bool good();
};

} // namespace ucanopen

#endif
#endif
