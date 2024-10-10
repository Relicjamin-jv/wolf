#include <helpers/logger.hpp>
#include <helpers/utils.hpp>

namespace wolf::core::kubernetes {
    void init() {
        logs::log(logs::info, "[Kubernetes] Init");
    }
}