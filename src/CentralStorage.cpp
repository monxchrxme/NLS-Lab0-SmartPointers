#include "CentralStorage.hpp"

namespace custom {

CentralStorage& CentralStorage::instance() {
    static CentralStorage instance_;
    return instance_;
}

void CentralStorage::add_ref(const void* ptr) {
    if (!ptr) return;
    ++table_[ptr];
}

bool CentralStorage::release_ref(const void* ptr) {
    if (!ptr) return false;

    auto it = table_.find(ptr);
    if (it == table_.end()) {
        return false;
    }

    --it->second;
    if (it->second == 0) {
        table_.erase(it);
        return true; // Счетчик достиг нуля значит объект пора удалить
    }

    return false;
}

std::size_t CentralStorage::get_ref_count(const void* ptr) const {
    if (!ptr) return 0;

    auto it = table_.find(ptr);
    if (it != table_.end()) {
        return it->second;
    }
    return 0;
}

std::size_t CentralStorage::tracked_objects_count() const {
    return table_.size();
}

void CentralStorage::clear() {
    table_.clear();
}

} // namespace custom