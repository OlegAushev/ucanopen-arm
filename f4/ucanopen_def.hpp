#pragma once

#if defined(MCUDRV_STM32) || defined(MCUDRV_APM32)
#if defined(STM32F4xx) || defined(APM32F4xx)

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <utility>

#include <emblib/can.hpp>

namespace ucanopen {

template<typename T>
inline canpayload_t to_payload(T const& message) {
  static_assert(sizeof(T) <= 8);
  canpayload_t payload{};
  memcpy(payload.data(), &message, sizeof(T));
  return payload;
}

template<typename T>
inline T from_payload(canpayload_t const& payload) {
  static_assert(sizeof(T) <= 8);
  T message{};
  memcpy(&message, payload.data(), sizeof(T));
  return message;
}

class NodeId {
private:
  unsigned int id_;
public:
  explicit NodeId(unsigned int id) : id_(id) {}

  unsigned int get() const { return id_; }

  bool valid() const { return (id_ >= 1) && (id_ <= 127); }
};

enum class NmtState : uint8_t {
  initializing = 0x00,
  stopped = 0x04,
  operational = 0x05,
  pre_operational = 0x7F
};

enum class Cob : unsigned int {
  nmt,
  sync,
  emcy,
  time,
  tpdo1,
  rpdo1,
  tpdo2,
  rpdo2,
  tpdo3,
  rpdo3,
  tpdo4,
  rpdo4,
  tsdo,
  rsdo,
  heartbeat
};

constexpr size_t cob_count = 15;

constexpr std::array<canid_t, cob_count> cob_function_codes = {
    0x000, // NMT
    0x080, // SYNC
    0x080, // EMCY
    0x100, // TIME
    0x180, // TPDO1
    0x200, // RPDO1
    0x280, // TPDO2
    0x300, // RPDO2
    0x380, // TPDO3
    0x400, // RPDO3
    0x480, // TPDO4
    0x500, // RPDO4
    0x580, // TSDO
    0x600, // RSDO
    0x700  // HEARTBEAT
};

inline canid_t calculate_cob_id(Cob cob, NodeId node_id) {
  if ((cob == Cob::nmt) || (cob == Cob::sync) || (cob == Cob::time)) {
    return cob_function_codes[std::to_underlying(cob)];
  }
  return cob_function_codes[std::to_underlying(cob)] + node_id.get();
}

constexpr std::array<uint8_t, cob_count> cob_sizes = {
    2, // NMT
    0, // SYNC
    2, // EMCY
    6, // TIME
    8, // TPDO1
    8, // RPDO1
    8, // TPDO2
    8, // RPDO2
    8, // TPDO3
    8, // RPDO3
    8, // TPDO4
    8, // RPDO4
    8, // TSDO
    8, // RSDO
    1  // HEARTBEAT
};

enum class CobTpdo : unsigned int {
  tpdo1,
  tpdo2,
  tpdo3,
  tpdo4,
};

inline Cob to_cob(CobTpdo tpdo) {
  return static_cast<Cob>(std::to_underlying(Cob::tpdo1) +
                          2 * std::to_underlying(tpdo));
}

enum class CobRpdo : unsigned int {
  rpdo1,
  rpdo2,
  rpdo3,
  rpdo4,
};

inline Cob to_cob(CobRpdo rpdo) {
  return static_cast<Cob>(std::to_underlying(Cob::rpdo1) +
                          2 * std::to_underlying(rpdo));
}

namespace sdo_cs_codes {
uint32_t const client_init_write = 1;
uint32_t const server_init_write = 3;
uint32_t const client_init_read = 2;
uint32_t const server_init_read = 2;

uint32_t const abort = 4;

uint32_t const client_block_write = 6;
uint32_t const server_block_write = 5;

uint32_t const client_block_read = 5;
uint32_t const server_block_read = 6;
} // namespace sdo_cs_codes

inline uint32_t get_cs_code(can_frame const& frame) {
  return (frame.payload[0] >> 5) & 0x07;
}

union ExpeditedSdoData {
  bool bl;
  int16_t i16;
  int32_t i32;
  uint16_t u16;
  uint32_t u32;
  float f32;

  ExpeditedSdoData() : u32(0) {}

  ExpeditedSdoData(bool value) : u32(0) { bl = value; }

  ExpeditedSdoData(int16_t value) : u32(0) { i16 = value; }

  ExpeditedSdoData(int32_t value) : i32(value) {}

  ExpeditedSdoData(uint16_t value) : u32(0) { u16 = value; }

  ExpeditedSdoData(uint32_t value) : u32(value) {}

  ExpeditedSdoData(float value) : f32(value) {}
};

struct ExpeditedSdo {
  uint32_t data_size_indicated : 1;
  uint32_t expedited_transfer : 1;
  uint32_t data_empty_bytes : 2;
  uint32_t _reserved : 1;
  uint32_t cs : 3;
  uint32_t index : 16;
  uint32_t subindex : 8;
  ExpeditedSdoData data;

  ExpeditedSdo()
      : data_size_indicated(0),
        expedited_transfer(0),
        data_empty_bytes(0),
        _reserved(0),
        cs(0),
        index(0),
        subindex(0),
        data() {}
};

enum class SdoAbortCode : uint32_t {
  no_error = 0,
  invalid_cs = 0x05040001,
  unsupported_access = 0x06010000,
  read_access_wo = 0x06010001,
  write_access_ro = 0x06010002,
  object_not_found = 0x06020000,
  hardware_error = 0x06060000,
  value_range_exceeded = 0x06090030,
  value_too_high = 0x06090031,
  value_too_low = 0x06090032,
  general_error = 0x08000000,
  data_store_error = 0x08000020,
  local_control_error = 0x08000021,
  state_error = 0x08000022
};

struct AbortSdo {
  uint32_t _reserved : 5;
  uint32_t cs : 3;
  uint32_t index : 16;
  uint32_t subindex : 8;
  uint32_t error_code;
  AbortSdo() = default;

  AbortSdo(uint16_t index_, uint8_t subindex_, SdoAbortCode error_code_)
      : _reserved(0),
        cs(sdo_cs_codes::abort),
        index(index_),
        subindex(subindex_),
        error_code(std::to_underlying(error_code_)) {}

  bool valid() const { return cs == sdo_cs_codes::abort; }
};

enum ODObjectDataType {
  OD_BOOL,
  OD_INT8,
  OD_INT16,
  OD_INT32,
  OD_UINT8,
  OD_UINT16,
  OD_UINT32,
  OD_FLOAT32,
  OD_EXEC,
  OD_STRING
};

enum ODObjectAccessPermission {
  OD_ACCESS_RW,
  OD_ACCESS_RO,
  OD_ACCESS_WO,
  OD_ACCESS_CONST
};

// Used in OD-entries for default values definition
#define OD_NO_DEFAULT_VALUE std::nullopt
#define OD_DEFAULT_VALUE(value) ExpeditedSdoData(value)

// Used in OD-entries which doesn't have direct access to data through pointer.
#define OD_NO_DIRECT_ACCESS std::pair<uint32_t*, uint32_t**>(nullptr, nullptr)

// Used in OD-entries which have direct access to data through pointer.
#define OD_PTR(ptr) \
  std::pair<uint32_t*, uint32_t**>(reinterpret_cast<uint32_t*>(ptr), nullptr)
#define OD_DPTR(dptr) \
  std::pair<uint32_t*, uint32_t**>(nullptr, reinterpret_cast<uint32_t**>(dptr))

// Used in OD-entries which don't have read access to data through function.
inline SdoAbortCode OD_NO_INDIRECT_READ_ACCESS(ExpeditedSdoData& retval) {
  return SdoAbortCode::unsupported_access;
}

// Used in OD-entries which don't have write access to data through function.
inline SdoAbortCode OD_NO_INDIRECT_WRITE_ACCESS(ExpeditedSdoData val) {
  return SdoAbortCode::unsupported_access;
}

size_t const od_object_type_sizes[10] = {sizeof(bool),
                                         sizeof(int8_t),
                                         sizeof(int16_t),
                                         sizeof(int32_t),
                                         sizeof(uint8_t),
                                         sizeof(uint16_t),
                                         sizeof(uint32_t),
                                         sizeof(float),
                                         4,
                                         4};

struct ODObjectKey {
  uint16_t index;
  uint8_t subindex;
};

struct ODObject {
  char const* category;
  char const* subcategory;
  char const* name;
  char const* unit;
  ODObjectAccessPermission access_permission;
  ODObjectDataType data_type;
  std::optional<ExpeditedSdoData> default_value;
  std::pair<uint32_t*, uint32_t**> ptr;
  SdoAbortCode (*read_func)(ExpeditedSdoData& retval);
  SdoAbortCode (*write_func)(ExpeditedSdoData val);

  bool has_direct_access() const { return ptr != OD_NO_DIRECT_ACCESS; }

  bool has_read_permission() const { return access_permission != OD_ACCESS_WO; }

  bool has_write_permission() const {
    return (access_permission == OD_ACCESS_RW) ||
           (access_permission == OD_ACCESS_WO);
  }
};

struct ODEntry {
  ODObjectKey key;
  ODObject object;
};

inline bool operator<(ODEntry const& lhs, ODEntry const& rhs) {
  return (lhs.key.index < rhs.key.index) ||
         ((lhs.key.index == rhs.key.index) &&
          (lhs.key.subindex < rhs.key.subindex));
}

inline bool operator<(ODEntry const& lhs, ODObjectKey const& rhs) {
  return (lhs.key.index < rhs.index) ||
         ((lhs.key.index == rhs.index) && (lhs.key.subindex < rhs.subindex));
}

inline bool operator<(ODObjectKey const& lhs, ODEntry const& rhs) {
  return (lhs.index < rhs.key.index) ||
         ((lhs.index == rhs.key.index) && (lhs.subindex < rhs.key.subindex));
}

inline bool operator==(ODObjectKey const& lhs, ODEntry const& rhs) {
  return (lhs.index == rhs.key.index) && (lhs.subindex == rhs.key.subindex);
}

inline bool operator==(ODObjectKey const& lhs, ODObjectKey const& rhs) {
  return (lhs.index == rhs.index) && (lhs.subindex == rhs.subindex);
}

} // namespace ucanopen

#endif
#endif
