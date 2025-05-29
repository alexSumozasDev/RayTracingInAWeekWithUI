#pragma once

#include <QWidget>
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <qtimer.h>
#include <vector>
#include <cmath>

struct Vertex {
    DirectX::XMFLOAT3 position;
};




class RenderViewport : public QWidget
{
    Q_OBJECT

    QTimer* render_timer = nullptr;

public:
    RenderViewport(QWidget* parent = nullptr);
    ~RenderViewport();

protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    //void generateWireSphere(float radius, int slices, int stacks);

private:

    void initD3D();
    void compileShaders();
    void render();
    void cleanup();
    std::vector<Vertex> generateWireframeSphere(float radius, int slices, int stacks);

    UINT sphere_vertex_count = 0;

    Microsoft::WRL::ComPtr<ID3D11Device> vp_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> vp_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> vp_swap_chain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> vp_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vp_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> vp_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vp_vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> vp_input_lay;
};
