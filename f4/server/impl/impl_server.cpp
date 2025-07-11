#if defined(MCUDRV_STM32) || defined(MCUDRV_APM32)
#if defined(STM32F4xx) || defined(APM32F4xx)

#include <ucanopen-arm/f4/server/impl/impl_server.hpp>

namespace ucanopen {

impl::Server::Server(ucan::Module& can_module,
                     NodeId node_id,
                     std::vector<ODEntry>& object_dictionary)
    : node_id_(node_id),
      can_module_(can_module),
      dictionary_(object_dictionary) {
  nmt_state_ = NmtState::initializing;
  init_object_dictionary();
}

void impl::Server::init_object_dictionary() {
  std::sort(dictionary_.begin(), dictionary_.end());

  // Check OBJECT DICTIONARY correctness
  for (size_t i = 0; i < dictionary_.size(); ++i) {
    // OD is sorted
    if (i < (dictionary_.size() - 1)) {
      assert(dictionary_[i] < dictionary_[i + 1]);
    }

    for (size_t j = i + 1; j < dictionary_.size(); ++j) {
      // no od-entries with equal {index, subinex}
      assert((dictionary_[i].key.index != dictionary_[j].key.index) ||
             (dictionary_[i].key.subindex != dictionary_[j].key.subindex));

      // no od-entries with equal {category, subcategory, name}
      [[maybe_unused]] bool categoryEqual =
          ((strcmp(dictionary_[i].object.category,
                   dictionary_[j].object.category) == 0) ?
               true :
               false);
      [[maybe_unused]] bool subcategoryEqual =
          ((strcmp(dictionary_[i].object.subcategory,
                   dictionary_[j].object.subcategory) == 0) ?
               true :
               false);
      [[maybe_unused]] bool nameEqual =
          ((strcmp(dictionary_[i].object.name, dictionary_[j].object.name) ==
            0) ?
               true :
               false);
      assert(!categoryEqual || !subcategoryEqual || !nameEqual);
    }

    if (dictionary_[i].object.has_read_permission()) {
      assert((dictionary_[i].object.read_func != OD_NO_INDIRECT_READ_ACCESS) ||
             (dictionary_[i].object.ptr != OD_NO_DIRECT_ACCESS));
    }

    if (dictionary_[i].object.has_write_permission()) {
      assert(dictionary_[i].object.write_func != OD_NO_INDIRECT_WRITE_ACCESS ||
             (dictionary_[i].object.ptr != OD_NO_DIRECT_ACCESS));
    }
  }
}

} // namespace ucanopen

#endif
#endif
