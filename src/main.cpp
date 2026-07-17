#if defined(GEODE_IS_IOS) || defined(GEODE_IS_ANDROID) || defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_MACOS)
#include <Geode/Geode.hpp>
#include <sabe.persistenceapi/include/PersistenceAPI.hpp>

using namespace geode::prelude;
using namespace persistenceAPI;

$on_mod(Loaded) {
    // Кейбинды удалены, мод успешно загружается и работает на чистом SDK!
}
#endif

