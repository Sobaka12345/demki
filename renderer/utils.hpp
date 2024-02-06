#pragma once

#include "assert.hpp"

#include <functional>
#include <filesystem>
#include <fstream>
#include <type_traits>
#include <vector>
#include <list>
#include <map>

#ifdef _WIN32
#	include <windows.h>    //GetModuleFileNameW
#else
#	include <limits.h>
#	include <unistd.h>    //readlink
#endif

inline std::filesystem::path executablePath()
{
    static std::filesystem::path s_executablePath =
        []() -> std::filesystem::path {
#ifdef _WIN32
        wchar_t path[MAX_PATH] = { 0 };
        GetModuleFileNameW(NULL, path, MAX_PATH);
        return path;
#else
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        return std::string(result, (count > 0) ? count : 0);
#endif
    }()
                    .parent_path();

    return s_executablePath;
}

inline std::vector<char> readFile(std::filesystem::path filePath)
{
    if (filePath.is_relative())
    {
        filePath = executablePath() / filePath;
    }

    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    ASSERT(file.is_open(), ("failed to open file: " + filePath.string()).c_str());

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

namespace std {
template <typename T, typename = void>
struct is_iterable : std::false_type
{};

//  this gets used only when we can call std::begin() and std::end() on that type
template <typename T>
struct is_iterable<T,
    std::void_t<decltype(std::begin(std::declval<T&>())), decltype(std::end(std::declval<T&>()))>>
    : std::true_type
{};

//  Here is a helper:
template <typename T>
constexpr bool is_iterable_v = is_iterable<T>::value;

}


template <typename KeyT, typename PtrT>
class FragileSharedPtrMap;

template <typename T>
class FragileSharedPtr
{
    struct ReferenceBlock
    {
        bool suddenDeath() const { return ptrList.size() <= suddenDeathThreshold; }

        T* obj = nullptr;
        size_t suddenDeathThreshold = 2;
        std::list<FragileSharedPtr<T>*> ptrList;
    };

    template <typename>
    friend class FragileSharedPtr;

    template <typename K, typename V>
    friend class FragileSharedPtrMap;

public:
    explicit FragileSharedPtr(T* obj = nullptr, size_t suddenDeathThreshold = 2)
        : m_referenceBlock(
              new ReferenceBlock{ .obj = obj, .suddenDeathThreshold = suddenDeathThreshold })
        , m_isFragile(false)
    {
        m_listPos = m_referenceBlock->ptrList.insert(m_referenceBlock->ptrList.end(), this);
    }

    FragileSharedPtr(const FragileSharedPtr& other)
        : m_referenceBlock(other.m_referenceBlock)
        , m_isFragile(false)
    {
        if (m_referenceBlock)
            m_listPos = m_referenceBlock->ptrList.insert(m_referenceBlock->ptrList.end(), this);
    }

    FragileSharedPtr(FragileSharedPtr&& other)
        : m_referenceBlock(other.m_referenceBlock)
        , m_isFragile(false)
        , m_listPos(other.m_listPos)
    {
        other.m_referenceBlock = nullptr;
        *m_listPos = this;
    }

    void setFragile(bool value) { m_isFragile = value; }

    bool isFragile() const { return m_isFragile; }

    bool isAlive() const { return m_referenceBlock->obj != nullptr; }

    bool isDead() const { return m_referenceBlock->obj == nullptr; }

    T* operator->() { return static_cast<T*>(m_referenceBlock->obj); }

    const T* operator->() const { return static_cast<const T*>(m_referenceBlock->obj); }

    ~FragileSharedPtr()
    {
        if (!m_referenceBlock) return;

        m_referenceBlock->ptrList.erase(m_listPos);

        if (m_referenceBlock->obj && (m_referenceBlock->suddenDeath() || isFragile()))
        {
            T* obj = m_referenceBlock->obj;
            m_referenceBlock->obj = nullptr;

            auto ptrIter = m_referenceBlock->ptrList.begin();
            while (ptrIter != m_referenceBlock->ptrList.end())
            {
                auto toDelete = ptrIter++;
                (*toDelete)->m_deleteCallback(obj);
            }

            std::default_delete<T>{}(obj);
            delete m_referenceBlock;
        }
    }

private:
    void registerDeleteCallback(std::function<void(T*)> callback) { m_deleteCallback = callback; }

private:
    bool m_isFragile;
    std::function<void(T*)> m_deleteCallback;
    ReferenceBlock* m_referenceBlock = nullptr;
    std::list<FragileSharedPtr<T>*>::iterator m_listPos;
};

template <typename KeyT, typename PtrT>
class FragileSharedPtrMap
{
    using MapType = std::map<KeyT, FragileSharedPtr<PtrT>>;
    using Iterator = typename MapType::iterator;
    using ConstIterator = typename MapType::const_iterator;

public:
    FragileSharedPtrMap()
        : m_isInDestruction(false)
    {}

    ~FragileSharedPtrMap()
    {
        m_isInDestruction = true;
        m_map.clear();
    }

    [[nodiscard]] Iterator end() noexcept { return m_map.end(); }

    [[nodiscard]] ConstIterator end() const noexcept { return m_map.end(); }

    [[nodiscard]] Iterator begin() noexcept { return m_map.begin(); }

    [[nodiscard]] ConstIterator begin() const noexcept { return m_map.begin(); }

    void clear()
    {
        m_isInDestruction = true;
        m_map.clear();
        m_isInDestruction = false;
    }

    Iterator find(const KeyT& key) noexcept { return m_map.find(key); }

    ConstIterator find(const KeyT& key) const noexcept { return m_map.find(key); }

    template <typename... Args>
    std::pair<Iterator, bool> emplace(Args... args) noexcept
    {
        auto [mapIter, emplaced] = m_map.emplace(std::forward<Args>(args)...);

        if (emplaced)
        {
            mapIter->second.registerDeleteCallback([mapIter, this](auto _) {
                if (!m_isInDestruction) m_map.erase(mapIter);
            });
        }

        return { mapIter, emplaced };
    }

    void erase(const KeyT& key) noexcept
    {
        m_isInDestruction = true;
        m_map.erase(key);
        m_isInDestruction = false;
    }

    const FragileSharedPtr<PtrT>& at(const KeyT& key) const { return *m_map.at(key); }

    FragileSharedPtr<PtrT>& at(const KeyT& key) { return *m_map.at(key); }

    [[nodiscard]] size_t size() const noexcept { return m_map.size(); }

private:
    bool m_isInDestruction;
    MapType m_map;
};
