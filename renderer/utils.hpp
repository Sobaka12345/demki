#pragma once

#include "assert.hpp"

#include <functional>
#include <filesystem>
#include <fstream>
#include <type_traits>
#include <vector>

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

struct ReferenceBlock
{
    void* obj = nullptr;
    size_t count = 1;
    std::vector<std::function<void(void*)>> deleteCallbacks;
};

template <typename T>
class FragileSharedPtr
{
    template <typename>
    friend class FragileSharedPtr;

public:
    explicit FragileSharedPtr(T* obj = nullptr)
        : m_referenceBlock(new ReferenceBlock{ .obj = obj })
    {}

    template <typename DT>
        requires std::is_base_of_v<T, DT>
    FragileSharedPtr(const FragileSharedPtr<DT>& other)
        : m_referenceBlock(other.m_referenceBlock)
    {
        m_referenceBlock->count++;
    }

    template <typename DT>
        requires std::is_base_of_v<T, DT>
    FragileSharedPtr(FragileSharedPtr<DT>&& other)
        : m_referenceBlock(other.m_referenceBlock)
    {
        other.m_referenceBlock = nullptr;
    }

    bool isAlive() const { return m_referenceBlock->obj != nullptr; }

    bool isDead() const { return m_referenceBlock->obj == nullptr; }

    void registerDeleteCallback(std::function<void(T*)> callback)
    {
        m_referenceBlock->deleteCallbacks.push_back([callback](void* obj) {
            callback(static_cast<T*>(obj));
        });
    }

    T* operator->() { return static_cast<T*>(m_referenceBlock->obj); }

    const T* operator->() const { return static_cast<const T*>(m_referenceBlock->obj); }

    ~FragileSharedPtr()
    {
        if (!m_referenceBlock) return;

        if (m_referenceBlock->obj)
        {
            T* obj = static_cast<T*>(m_referenceBlock->obj);
            m_referenceBlock->obj = nullptr;

            for (auto& callback : m_referenceBlock->deleteCallbacks)
            {
                callback(obj);
            }

            std::default_delete<T>{}(obj);
        }

        if (!--m_referenceBlock->count) delete m_referenceBlock;
    }

private:
    ReferenceBlock* m_referenceBlock = nullptr;
};
