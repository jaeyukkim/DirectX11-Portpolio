#pragma once
//#include "Render/RenderDefinition.h"
#include "RenderProxy/AnimationRenderProxy.h"
#include "RenderProxy/LightSceneRenderProxy.h"
#include "RenderProxy/MirrorRenderProxy.h"
#include "RenderProxy/SkeletalMeshRenderProxy.h"
#include "RenderProxy/SkyBoxRenderProxy.h"
#include "RenderProxy/StaticMeshRenderProxy.h"
#include "RenderProxy/ViewRenderProxy.h"


class PostEffect;
enum ERenderProxyType : UINT8;
class RenderProxy;
class LightSceneRenderProxy;
class ViewRenderProxy;

class FSceneRender
{
public:

    static void Create();
    static void Destroy();
    static FSceneRender* Get();
    
    template <typename ProxyType, typename... Args>
    void CreateRenderProxy(Args&&... args);
    template <typename ProxyType, typename... Args>
    void CreateMeshRenderProxy(const string& meshName, Args&&... args);
    template <typename ProxyType, typename... Args>
    void CreateAnimRenderProxy(const string& meshName, Args&&... args);
    template <typename ProxyType>
    void DestroyMeshProxy(const string& meshName, const int instanceID);
 

    
    void Render();
    void BeginRender();
    void RenderDepthOnly();
    void RenderShadowMap();
    void RenderMirror();
    void MainRender();
    
    void EndRender();
    void RenderObjects(FRenderOption option);
    
    
public:
    FRenderOption GetDefaultRenderType();
    FRenderOption GetMirrorRenderType();
    FRenderOption GetStencilRenderType();
    FRenderOption GetBlendRenderType();
    FRenderOption GetDepthOnlyRenderType();


    void SetRenderTypeWire() {bWireRender = true;}
    void SetRenderTypeDefault() {bWireRender = false;}
    PostEffect* GetPostEffect() { return PostEffectEntity.get(); }
    bool StaticMeshHasCreated(const string& meshName);
    bool SkeletalMeshHasCreated(const string& meshName);
    bool AnimProxyHasCreated(const string& meshName);

private:
    FSceneRender() = default;
    ~FSceneRender() = default;
    FSceneRender(const FSceneRender&) = delete;
    FSceneRender(FSceneRender&&) = delete;
    FSceneRender& operator=(const FSceneRender&) = delete;
    FSceneRender& operator=(FSceneRender&&) = delete;

    
    static FSceneRender* Instance;
    unordered_map<string, shared_ptr<StaticMeshRenderProxy>> MeshProxies;
    unordered_map<string, shared_ptr<SkeletalMeshRenderProxy>> SkeletalMeshProxies;
    unordered_map<string, shared_ptr<AnimationRenderProxy>> AnimProxies;

    shared_ptr<LightSceneRenderProxy> LightSceneProxy;
    shared_ptr<ViewRenderProxy> ViewProxy;
    vector<shared_ptr<MirrorRenderProxy>> MirrorProxy;
    shared_ptr<SkyBoxRenderProxy> SkyBoxProxy;

    shared_ptr<PostEffect> PostEffectEntity;
    bool bWireRender = false;
};



/**
 * @tparam ProxyType 프록시의 클래스 타입 전달
 * @param args 생성 할 프록의 생성자 매개변수
 */
template <typename ProxyType, typename... Args>
void FSceneRender::CreateRenderProxy(Args&&... args)
{
    static_assert(std::is_base_of_v<RenderProxy, ProxyType>, "프록시 타입은 RenderProxy로 부터 상속되어야 합니다.");

    auto proxy = std::make_shared<ProxyType>(std::forward<Args>(args)...);

   
    if constexpr(std::is_same_v<ProxyType, LightSceneRenderProxy>)
        LightSceneProxy = proxy;
    else if constexpr(std::is_same_v<ProxyType, ViewRenderProxy>)
        ViewProxy = proxy;
    else if constexpr(std::is_same_v<ProxyType, MirrorRenderProxy>)
        MirrorProxy.push_back(proxy);
    else if constexpr(std::is_same_v<ProxyType, SkyBoxRenderProxy>)
        SkyBoxProxy = proxy;
    else
        Assert(true, "지원되지 않는 프록시 타입입니다.");
}


/**
 * @tparam ProxyType 프록시의 클래스 타입 전달
 * @param args 생성 할 프록시의 메시 이름
 * @param args 생성 할 프록시의 생성자 매개변수
 */
template <typename ProxyType, typename... Args>
void FSceneRender::CreateMeshRenderProxy(const string& meshName, Args&&... args)
{
    static_assert(std::is_base_of_v<RenderProxy, ProxyType>, "프록시 타입은 RenderProxy로 부터 상속되어야 합니다.");


    if constexpr (std::is_same_v<ProxyType, StaticMeshRenderProxy>)
    {
        if(StaticMeshHasCreated(meshName))
        {
            MeshProxies[meshName]->AddInstance(std::forward<Args>(args)...);
        }
        else
        {
            MeshProxies[meshName] = std::make_shared<StaticMeshRenderProxy>(std::forward<Args>(args)...);
        }
    }
    else if constexpr (std::is_same_v<ProxyType, SkeletalMeshRenderProxy>)
    {
        if(SkeletalMeshHasCreated(meshName))
        {
            SkeletalMeshProxies[meshName]->AddInstance(std::forward<Args>(args)...);
        }
        else
        {
            SkeletalMeshProxies[meshName] = std::make_shared<SkeletalMeshRenderProxy>(std::forward<Args>(args)...);
        }
    }
   
}

template <typename ProxyType, typename ... Args>
void FSceneRender::CreateAnimRenderProxy(const string& meshName, Args&&... args)
{
    if constexpr (std::is_same_v<ProxyType, AnimationRenderProxy>)
    {
        if(AnimProxyHasCreated(meshName))
        {
            AnimProxies[meshName]->AddInstance(std::forward<Args>(args)...);
        }
        else
        {
            AnimProxies[meshName] = std::make_shared<AnimationRenderProxy>(std::forward<Args>(args)...);
        }
    }
}

template <typename ProxyType>
void FSceneRender::DestroyMeshProxy(const string& meshName, const int instanceID)
{
    static_assert(std::is_base_of_v<RenderProxy, ProxyType>, "프록시 타입은 RenderProxy로 부터 상속되어야 합니다.");


    if (std::is_same_v<ProxyType, StaticMeshRenderProxy>)
    {
      
        try
        {
            auto mesh = MeshProxies.at(meshName);
            if(mesh->GetNumOfInstance() == 1)
            {
                MeshProxies.erase(meshName);
            }
            else
            {
                mesh->DeleteInstance(instanceID);
            }
        }
        catch (exception& e)
        {
            Assert(true, "DestroyMeshProxy 실패");
        }
        
    }
    else if (std::is_same_v<ProxyType, SkeletalMeshRenderProxy>)
    {
        try
        {
            auto mesh = SkeletalMeshProxies.at(meshName);
            if(mesh->GetNumOfInstance() == 1)
            {
                SkeletalMeshProxies.erase(meshName);

                if (AnimProxies.find(meshName) != AnimProxies.end())
                {
                    AnimProxies.erase(meshName);
                }
              
            }
            else
            {
                mesh->DeleteInstance(instanceID);
                if(AnimProxies.size() > 0)
                {
                    AnimProxies[meshName]->DeleteInstance(instanceID);
                }
            }
        }
        catch (exception& e)
        {
            Assert(true, "DestroyMeshProxy 실패");
        }
    }
}

// 헬퍼 static_assert fallback용
template <typename>
inline constexpr bool always_false = false;