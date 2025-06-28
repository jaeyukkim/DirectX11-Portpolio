#pragma once
//#include "Render/RenderDefinition.h"
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

private:
    FSceneRender() = default;
    ~FSceneRender() = default;
    FSceneRender(const FSceneRender&) = delete;
    FSceneRender(FSceneRender&&) = delete;
    FSceneRender& operator=(const FSceneRender&) = delete;
    FSceneRender& operator=(FSceneRender&&) = delete;

    
    static FSceneRender* Instance;
    vector<shared_ptr<StaticMeshRenderProxy>> MeshProxies;
    vector<shared_ptr<SkeletalMeshRenderProxy>> SkeletalMeshProxies;
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
    
    if constexpr (std::is_same_v<ProxyType, StaticMeshRenderProxy>)
        MeshProxies.push_back(proxy);
    else if constexpr (std::is_same_v<ProxyType, SkeletalMeshRenderProxy>)
        SkeletalMeshProxies.push_back(proxy);
    else if constexpr (std::is_same_v<ProxyType, LightSceneRenderProxy>)
        LightSceneProxy = proxy;
    else if constexpr (std::is_same_v<ProxyType, ViewRenderProxy>)
        ViewProxy = proxy;
    else if constexpr (std::is_same_v<ProxyType, MirrorRenderProxy>)
        MirrorProxy.push_back(proxy);
    else if constexpr (std::is_same_v<ProxyType, SkyBoxRenderProxy>)
        SkyBoxProxy = proxy;
    else
        Assert(true, "지원되지 않는 프록시 타입입니다.");
}

// 헬퍼 static_assert fallback용
template <typename>
inline constexpr bool always_false = false;