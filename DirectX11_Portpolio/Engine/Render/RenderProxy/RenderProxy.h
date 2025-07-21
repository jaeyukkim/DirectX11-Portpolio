#pragma once


class RenderProxy
{
public:
    RenderProxy(ERenderProxyType InProxyType);
    virtual ~RenderProxy() = default;
    virtual void Render(const FRenderOption& option) = 0;
    virtual void Update();
private:
    ERenderProxyType ProxyType = RPT_None;
};
