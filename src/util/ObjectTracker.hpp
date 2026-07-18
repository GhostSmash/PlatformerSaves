#pragma once
#include <Geode/Geode.hpp>
#include <unordered_map>
#include <vector>
#include <string>

void writeCustomLog(const std::string& message);

class ObjectTracker {
public:
    struct Entry {
        std::string label;
        void* ptr;
        int lastKnownRetain;
        std::vector<std::string> history;
    };

    static ObjectTracker& get() {
        static ObjectTracker instance;
        return instance;
    }

    void registerObject(const std::string& label, cocos2d::CCObject* obj, const std::string& where) {
        if (!obj) {
            writeCustomLog(fmt::format("[TRACK] {} РЕГИСТРАЦИЯ NULL объекта в {}", label, where));
            return;
        }
        void* ptr = (void*)obj;
        Entry& e = m_entries[ptr];
        e.label = label;
        e.ptr = ptr;
        e.lastKnownRetain = obj->retainCount();
        e.history.push_back(fmt::format("СОЗДАН в {} (retain={})", where, e.lastKnownRetain));
        writeCustomLog(fmt::format("[TRACK] {} @{} СОЗДАН в {} retain={}", label, ptr, where, e.lastKnownRetain));
    }

    void track(const std::string& label, cocos2d::CCObject* obj, const std::string& where) {
        if (!obj) {
            writeCustomLog(fmt::format("[TRACK] {} @NULL в {}", label, where));
            return;
        }
        void* ptr = (void*)obj;
        int current = obj->retainCount();
        auto it = m_entries.find(ptr);
        if (it == m_entries.end()) {
            Entry& e = m_entries[ptr];
            e.label = label;
            e.ptr = ptr;
            e.lastKnownRetain = current;
            e.history.push_back(fmt::format("ПЕРВОЕ ОБНАРУЖЕНИЕ в {} (retain={})", where, current));
            writeCustomLog(fmt::format("[TRACK] {} @{} ПЕРВОЕ ОБНАРУЖЕНИЕ в {} retain={}", label, ptr, where, current));
            return;
        }
        Entry& e = it->second;
        int delta = current - e.lastKnownRetain;
        std::string deltaStr = delta > 0 ? fmt::format("+{}", delta) : std::to_string(delta);
        e.history.push_back(fmt::format("{} retain={} (изменение {})", where, current, deltaStr));
        writeCustomLog(fmt::format("[TRACK] {} @{} в {} retain={} (изменение {})", label, ptr, where, current, deltaStr));
        e.lastKnownRetain = current;
    }

    void dumpAll(const std::string& context) {
        writeCustomLog(fmt::format("=== ObjectTracker DUMP ({}) — всего объектов: {} ===", context, m_entries.size()));
        for (auto& pair : m_entries) {
            Entry& e = pair.second;
            writeCustomLog(fmt::format("--- {} @{} (текущий retain={}) ---", e.label, e.ptr, e.lastKnownRetain));
            for (auto& h : e.history) {
                writeCustomLog(fmt::format("    {}", h));
            }
        }
        writeCustomLog("=== КОНЕЦ DUMP ===");
    }

    void reset() {
        writeCustomLog(fmt::format("[TRACK] reset() — было объектов: {}", m_entries.size()));
        m_entries.clear();
    }

private:
    std::unordered_map<void*, Entry> m_entries;
};
