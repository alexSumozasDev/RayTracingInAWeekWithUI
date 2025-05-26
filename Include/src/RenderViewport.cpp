#include "RenderViewport.h"
#include <QWindow>
#include <QDebug>
#include <windows.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

RenderViewport::RenderViewport(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    initD3D();

    renderTimer = new QTimer(this);
    connect(renderTimer, &QTimer::timeout, this, &RenderViewport::render);
    renderTimer->start(16);

}

RenderViewport::~RenderViewport()
{
    cleanup();
}

void RenderViewport::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

}

void RenderViewport::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (swapChain)
    {
        renderTargetView.Reset();
        swapChain->ResizeBuffers(0, width(), height(), DXGI_FORMAT_UNKNOWN, 0);
        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
        swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);
    }
    render();
}

void RenderViewport::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    render();
}

void RenderViewport::initD3D()
{
    if (device)
        return;

    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = reinterpret_cast<HWND>(winId());
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION, &scd,
        &swapChain, &device, nullptr, &context);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);

    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(Vertex) * sphereVertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vinitData = { sphereVertices.data() };

    device->CreateBuffer(&vbd, &vinitData, &vertexBuffer);

    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(UINT) * sphereIndices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA iinitData = { sphereIndices.data() };

    device->CreateBuffer(&ibd, &iinitData, &indexBuffer);

    D3D11_RASTERIZER_DESC wireDesc = {};
    wireDesc.FillMode = D3D11_FILL_WIREFRAME;
    wireDesc.CullMode = D3D11_CULL_NONE;
    wireDesc.DepthClipEnable = true;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeRS;
    device->CreateRasterizerState(&wireDesc, &wireframeRS);

    context->RSSetState(wireframeRS.Get());



    generateWireSphere(10.0f, 20, 20);

    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, psBlob;


    D3DCompileFromFile(L"Include/shaders/VertexShader.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, nullptr);
    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);

    D3DCompileFromFile(L"Include/shaders/PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, nullptr);
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);

    context->VSSetShader(vertexShader.Get(), nullptr, 0);
    context->PSSetShader(pixelShader.Get(), nullptr, 0);

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    device->CreateInputLayout(layout, 1, vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(), &inputLayout);
    context->IASetInputLayout(inputLayout.Get());



}

void RenderViewport::render()
{
    if (!context || !renderTargetView) return;

    float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f }; 

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST); 

    context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);
    context->ClearRenderTargetView(renderTargetView.Get(), clearColor);

    context->DrawIndexed(sphereIndices.size(), 0, 0);
    swapChain->Present(1, 0);
}

void RenderViewport::cleanup()
{
    renderTargetView.Reset();
    swapChain.Reset();
    context.Reset();
    device.Reset();
}

void RenderViewport::generateWireSphere(float radius, int slices, int stacks)
{
    sphereVertices.clear();
    sphereIndices.clear();

    for (int i = 0; i <= stacks; ++i)
    {
        float phi = DirectX::XM_PI * i / stacks;
        for (int j = 0; j <= slices; ++j)
        {
            float theta = DirectX::XM_2PI * j / slices;
            float x = radius * sinf(phi) * cosf(theta);
            float y = radius * cosf(phi);
            float z = radius * sinf(phi) * sinf(theta);

            sphereVertices.push_back({ x, y, z });
        }
    }

    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < slices; ++j)
        {
            int current = i * (slices + 1) + j;
            int next = current + slices + 1;

            sphereIndices.push_back(current);
            sphereIndices.push_back(current + 1);

            sphereIndices.push_back(current);
            sphereIndices.push_back(next);
        }
    }
}
