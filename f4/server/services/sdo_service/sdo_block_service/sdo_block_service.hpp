#pragma once

#include <ucanopen-arm/f4/server/impl/impl_server.hpp>
#include <ucanopen-arm/f4/server/services/sdo_service/sdo_block_service/fsm/fsm.hpp>
#include <ucanopen-arm/f4/server/services/sdo_service/sdo_block_service/sdo_block_service_def.hpp>

#include <emb/static_vector.hpp>
#include <memory>

namespace ucanopen {

using PayloadBlock = emb::static_vector<canpayload_t, 127>;

class SdoBlockConsumer {
public:
  virtual void process(PayloadBlock const& block) = 0;
};

class SdoBlockService final : public emb::fsm::sp1::abstract_object<
                                  blk_fsm::State,
                                  blk_fsm::AbstractState,
                                  blk_fsm::state_count> {
private:
  impl::Server& server_;
  SdoBlockConsumer* consumer_{nullptr};
  std::unique_ptr<PayloadBlock> block_;
public:
  SdoBlockService(impl::Server& server);

  void handle(canpayload_t const& payload) { state_->handle(this, payload); }
};

} // namespace ucanopen
