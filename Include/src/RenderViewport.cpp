#include "RenderViewport.h"
#include <QWindow>
#include <QDebug>
#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

RenderViewport::RenderViewport(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    initD3D();

    vp_context->OMSetRenderTargets(1, vp_renderTargetView.GetAddressOf(), nullptr);

    D3D11_VIEWPORT vp = {};
    vp.Width = (FLOAT)width();
    vp.Height = (FLOAT)height();
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp_context->RSSetViewports(1, &vp);

    render_timer = new QTimer(this);
    connect(render_timer, &QTimer::timeout, this, &RenderViewport::render);
    render_timer->start(16);

}

RenderViewport::~RenderViewport()
{
    cleanup();
}

void RenderViewport::resizeEvent(QResizeEvent* event)
{
    if (vp_swap_chain)
    {
        vp_renderTargetView.Reset();

        HRESULT hr = vp_swap_chain->ResizeBuffers(
            0,
            width(),   
            height(),  
            DXGI_FORMAT_UNKNOWN,
            0);

        if (FAILED(hr)) {
            qDebug() << "Failed to resize swap chain buffers";
            return;
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;
        hr = vp_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
        if (FAILED(hr)) return;

        hr = vp_device->CreateRenderTargetView(back_buffer.Get(), nullptr, &vp_renderTargetView);
        if (FAILED(hr)) return;

        vp_context->OMSetRenderTargets(1, vp_renderTargetView.GetAddressOf(), nullptr);

        D3D11_VIEWPORT vp = {};
        vp.Width = (FLOAT)width();
        vp.Height = (FLOAT)height();
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        vp_context->RSSetViewports(1, &vp);



    }
}

void RenderViewport::paintEvent(QPaintEvent*)
{
    render();
}

void RenderViewport::initD3D()
{
    DXGI_SWAP_CHAIN_DESC desc_swapchain = {};



    desc_swapchain.BufferDesc.Width = width();
    desc_swapchain.BufferDesc.Height = height(); 
    desc_swapchain.BufferDesc.RefreshRate.Numerator = 60;
    desc_swapchain.BufferDesc.RefreshRate.Denominator = 1;
    desc_swapchain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc_swapchain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc_swapchain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc_swapchain.SampleDesc.Count = 1;
    desc_swapchain.SampleDesc.Quality = 0;
    desc_swapchain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc_swapchain.BufferCount = 1;
    desc_swapchain.OutputWindow = reinterpret_cast<HWND>(winId());
    desc_swapchain.Windowed = TRUE;
    desc_swapchain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; 
    desc_swapchain.Flags = 0;

    HWND hwnd = reinterpret_cast<HWND>(winId());
    qDebug() << "HWND:" << hwnd;
    if (!IsWindow(hwnd)) {
        qDebug() << "Invalid window handle!";

    }

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &desc_swapchain,
        &vp_swap_chain, &vp_device, nullptr, &vp_context);

    if (FAILED(hr)) {
        qDebug() << "D3D11CreateDeviceAndSwapChain failed with HRESULT:" << QString::number(hr, 16);
    }


    Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;
    vp_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));

    vp_device->CreateRenderTargetView(back_buffer.Get(), nullptr, &vp_renderTargetView);


    //Vertex vertices_traingle[] = { {{0.0f, 0.5f, 0.0f}}, {{0.5,-0.5, 0.0}}, {{ -0.5,-0.5,0.0}}, {{0.0f, 0.5f, 0.0f}} };
    auto vertices = generateWireframeSphere(0.5f, 20, 20);

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices.data();

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(vertices.size());
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    hr = vp_device->CreateBuffer(&bufferDesc, &vertexData, &vp_vertex_buffer);
    if (FAILED(hr)) {
        qDebug() << "Failed to create sphere vertex buffer.";
    }
    sphere_vertex_count = static_cast<UINT>(vertices.size());


    compileShaders();

}

void RenderViewport::compileShaders() {

    ID3DBlob* vs_blob = nullptr;
    ID3DBlob* ps_blob = nullptr;
    ID3DBlob* blob_error = nullptr;

    HRESULT hr = D3DCompileFromFile(L"Include\\shaders\\VertexShader.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vs_blob, &blob_error);

    if (hr != S_OK) {
        if (blob_error) {
            const char* errorMsg = reinterpret_cast<const char*>(blob_error->GetBufferPointer());
            qDebug() << "Shader compile error:" << QString::fromLocal8Bit(errorMsg);
            blob_error->Release();
            blob_error = nullptr;
        }
        else {
            qDebug() << "Vertex Shader compile failed, but no error message.";
        }
    }

    hr = D3DCompileFromFile(L"Include\\shaders\\PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &ps_blob, &blob_error);

    if (hr != S_OK) {
        if (blob_error) {
            const char* errorMsg = reinterpret_cast<const char*>(blob_error->GetBufferPointer());
            qDebug() << "Shader compile error:" << QString::fromLocal8Bit(errorMsg);
            blob_error->Release();
            blob_error = nullptr;
        }
        else {
            qDebug() << "Pixel Shader compile failed, but no error message.";
        }
    }

    hr = vp_device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &vp_vertexShader);

    if (hr != S_OK) {
        qDebug() << "Error al crear el vertex Shader ";
    }
    hr = vp_device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &vp_pixelShader);

    if (hr != S_OK) {
        qDebug() << "Error al crear el Pixel Shader ";
    }

    D3D11_INPUT_ELEMENT_DESC in_dsc[] = {"position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0};

    hr = vp_device->CreateInputLayout(in_dsc, 1, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &vp_input_lay);
    
    if (hr != S_OK) {
        qDebug() << "Error al crear el Pixel Shader ";
    }

    vp_context->IASetInputLayout(vp_input_lay.Get());

    vp_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

}


void RenderViewport::render()
{
    FLOAT col[] = {0.5,0.2,0.1,1};
    vp_context->ClearRenderTargetView(vp_renderTargetView.Get(), col);
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    if (!vp_vertex_buffer) {
        qDebug() << "Vertex buffer is null in render()!";
        return; 
    }

    vp_context->IASetVertexBuffers(0, 1, vp_vertex_buffer.GetAddressOf(), &stride, &offset);

    vp_context->VSSetShader(vp_vertexShader.Get(), nullptr, 0);
    vp_context->PSSetShader(vp_pixelShader.Get(), nullptr, 0);

    vp_context->Draw(sphere_vertex_count, 0);
    vp_swap_chain->Present(1, 0);
}

void RenderViewport::cleanup()
{
    vp_renderTargetView.Reset();
    //vp_vertex_buffer.Reset();
    vp_pixelShader.Reset();
    vp_vertexShader.Reset();
    vp_swap_chain.Reset();
    vp_context.Reset();
    vp_device.Reset();
}

void RenderViewport::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);


}



std::vector<Vertex> RenderViewport::generateWireframeSphere(float radius, int slices, int stacks) {
    std::vector<Vertex> vertices;

    for (int i = 0; i <= stacks; ++i) {
        float lat = DirectX::XM_PI * i / stacks;
        float y = cos(lat);
        float r = sin(lat);
        for (int j = 0; j < slices; ++j) {
            float lon1 = 2 * DirectX::XM_PI * j / slices;
            float lon2 = 2 * DirectX::XM_PI * (j + 1) / slices;

            Vertex v1 = { {radius * r * cos(lon1), radius * y, radius * r * sin(lon1)} };
            Vertex v2 = { {radius * r * cos(lon2), radius * y, radius * r * sin(lon2)} };

            vertices.push_back(v1); 
            vertices.push_back(v2);
        }
    }

    for (int j = 0; j < slices; ++j) {
        float lon = 2 * DirectX::XM_PI * j / slices;
        for (int i = 0; i < stacks; ++i) {
            float lat1 = DirectX::XM_PI * i / stacks;
            float lat2 = DirectX::XM_PI * (i + 1) / stacks;

            Vertex v1 = { {radius * sin(lat1) * cos(lon), radius * cos(lat1), radius * sin(lat1) * sin(lon)} };
            Vertex v2 = { {radius * sin(lat2) * cos(lon), radius * cos(lat2), radius * sin(lat2) * sin(lon)} };

            vertices.push_back(v1); 
            vertices.push_back(v2);
        }
    }

    return vertices;
}
