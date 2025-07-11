#if defined(MCUDRV_STM32) || defined(MCUDRV_APM32)
#if defined(STM32F4xx) || defined(APM32F4xx)

#include <ucanopen-arm/f4/server/services/sdo_service/sdo_block_service/fsm/fsm.hpp>

namespace ucanopen {
namespace blk_fsm {

AbstractState* AbstractState::create(State state) {
    switch (state) {
    case State::idle:
        return new IdleState;
    case State::download:
        return new DownloadState;
    case State::download_end:
        return new DownloadEndState;
    }
    return nullptr;
}

void AbstractState::destroy(State state, AbstractState* stateobj) {
    delete stateobj;
}

void IdleState::handle(SdoBlockService* _service, const canpayload_t& payload) {
}

void DownloadState::handle(SdoBlockService* _service,
                           const canpayload_t& payload) {}

void DownloadEndState::handle(SdoBlockService* _service,
                              const canpayload_t& payload) {}

} // namespace blk_fsm
} // namespace ucanopen

#endif
#endif
