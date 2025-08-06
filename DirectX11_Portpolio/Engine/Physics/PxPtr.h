#pragma once


template <typename pxResource>
class PxPtr
{
public:
    PxPtr() = default;
    explicit PxPtr(pxResource* ptr)
        : Resource(ptr)
    {
    }
    
    static PxPtr<pxResource> make_ptr(pxResource* raw)
    {
        return PxPtr<pxResource>(raw);
    }

    // 소멸자
    ~PxPtr()
    {
        if (Resource)
        {
            Resource->release();
            Resource = nullptr;
        }
    }

    // 이동 생성자
    PxPtr(PxPtr&& other) noexcept
        : Resource(other.Resource)
    {
        other.Resource = nullptr;
    }

    // 이동 대입 연산자
    PxPtr& operator=(PxPtr&& other) noexcept
    {
        if (this != &other)
        {
            //if (Resource)
            //    Resource->release();
            Resource = other.Resource;
            other.Resource = nullptr;
        }
        return *this;
    }

    // 복사 금지
    PxPtr(const PxPtr&) = delete;
    PxPtr& operator=(const PxPtr&) = delete;

    // 접근 연산자
    pxResource* get() const { return Resource; }
    pxResource* operator->() const { return Resource; }
    pxResource& operator*() const { return *Resource; }
    operator bool() const { return Resource != nullptr; }

private:
   
    pxResource* Resource = nullptr;

};

