#pragma once


class RenderProxy
{
public:
    RenderProxy(ERenderProxyType InProxyType);
    virtual ~RenderProxy() = default;
    virtual void Render(const FRenderOption& option) = 0;

private:
    ERenderProxyType ProxyType = RPT_None;
};
