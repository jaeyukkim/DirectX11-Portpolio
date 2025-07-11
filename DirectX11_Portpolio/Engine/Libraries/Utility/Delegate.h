#pragma once

#include <iostream>
#include <functional>
#include <unordered_map>
#include <memory>
#include <string>
#include <mutex>
#include <atomic>

using namespace std;


class FDelegateHandle
{
public:

    FDelegateHandle()
    {
        static atomic<uint64_t> GlobalCounter{ 0 }; // thread-safe
        ID = GlobalCounter++;
    }

    uint64_t GetID() const { return ID; }

private:
    uint64_t ID;
};

// 멤버 함수 바인딩을 지원하는 Delegate
template<typename... Args>
class FDynamicDelegate
{
public:
    FDynamicDelegate() = default;

    // 복사 금지
    FDynamicDelegate(const FDynamicDelegate&) = delete;
    FDynamicDelegate& operator=(const FDynamicDelegate&) = delete;

    // 이동도 막거나 필요시 정의
    FDynamicDelegate(FDynamicDelegate&&) = delete;
    FDynamicDelegate& operator=(FDynamicDelegate&&) = delete;

    class ICallable
    {
    public:
        virtual ~ICallable() = default;
        virtual void Invoke(Args... args) = 0;
    };

    template<typename T>
    class TCallable final : public ICallable
    {
    public:
        TCallable(T* instance, void (T::* method)(Args...))
            : Instance(instance), Method(method) {}

        void Invoke(Args... args) override
        {
            (Instance->*Method)(args...);
        }

    private:
        T* Instance;
        void (T::* Method)(Args...);
    };

    template<typename T>
    FDelegateHandle Add(T* instance, void (T::* method)(Args...))
    {
        FDelegateHandle handle;
        Bindings.emplace_back(handle.GetID(), new TCallable<T>(instance, method));
        return handle;
    }

    void Remove(const FDelegateHandle& handle)
    {
        for (auto it = Bindings.begin(); it != Bindings.end(); ++it)
        {
            if (it->first == handle.GetID())
            {
                delete it->second;
                Bindings.erase(it);
                break;
            }
        }
    }

    void Clear()
    {
        for (auto& pair : Bindings)
            delete pair.second;
        Bindings.clear();
    }

    void Broadcast(Args... args)
    {
        for (auto& pair : Bindings)
            pair.second->Invoke(args...);
    }

    ~FDynamicDelegate()
    {
        Clear();
    }

private:
    std::vector<std::pair<uint64_t, ICallable*>> Bindings;
};

// 매크로 정의
#define DECLARE_DYNAMIC_DELEGATE(DelegateName) \
    class DelegateName : public FDynamicDelegate<> {}

#define DECLARE_DYNAMIC_DELEGATE_OneParam(DelegateName, Param1Type) \
    class DelegateName : public FDynamicDelegate<Param1Type> {}

#define DECLARE_DYNAMIC_DELEGATE_TwoParams(DelegateName, Param1Type, Param2Type) \
    class DelegateName : public FDynamicDelegate<Param1Type, Param2Type> {}