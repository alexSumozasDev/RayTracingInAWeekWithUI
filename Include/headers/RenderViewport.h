#pragma once

#include <QWidget>
#include <d3d11.h>
#include <wrl/client.h>
#include <qtimer.h>

struct Vertex {
    float x, y, z;
};




class RenderViewport : public QWidget
{
    Q_OBJECT

    QTimer* renderTimer = nullptr;
    std::vector<Vertex> sphereVertices;
    std::vector<UINT> sphereIndices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

public:
    RenderViewport(QWidget* parent = nullptr);
    ~RenderViewport();

protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void generateWireSphere(float radius, int slices, int stacks);

private:

    void initD3D();
    void render();
    void cleanup();

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
};
