// gfx
// 
// Copyright (c) 2016 Robin Southern -- github.com/betajaen/gfx
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "gfx.h"

namespace GFX_NS
{
  namespace
  {
    Context* _ctx;
  }

  const State State::DEFAULT = State(BGFX_STATE_DEFAULT);

  Context::Context()
  {
    view.push_back(Matrix());
    projection.push_back(Matrix());
    model.push_back(Matrix());
    state.push_back(State::DEFAULT);
    views.push_back(0);

    modelVersion = 1;
    viewVersion = 1;
    projectionVersion = 1;
    stateVersion = 1;

    lastModelVersion = 0;
    lastViewVersion = 0;
    lastProjectionVersion = 0;
    lastStateVersion = 0;
  }

  Context::~Context()
  {
  }

  void setContext(Context* ctx)
  {
    _ctx = ctx;
  }

  Context* getContext()
  {
    return _ctx;
  }

  void pushView(uint8_t view)
  {
    auto ctx = getContext();
    ctx->views.push_back(0);
    ctx->viewsVersion++;
  }

  void setView(uint8_t view)
  {
    auto ctx = getContext();
    ctx->views.back() = view;
    ctx->viewsVersion++;
  }

  void popView()
  {
    auto ctx = getContext();
    if (ctx->views.empty() == false)
    {
      ctx->views.pop_back();
      ctx->viewsVersion++;
    }
  }

  uint8_t getView()
  {
    auto ctx = getContext();
    return ctx->views.back();
  }

  void setViewRect(uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
  {
    bgfx::setViewRect(id, x, y, w, h);
  }

  void touch(uint8_t id)
  {
    bgfx::touch(0);
  }

  void setProgram(const bgfx::ProgramHandle& program)
  {
    auto ctx = getContext();
    ctx->currentProgram = program;
  }

  void setMatrices(const Camera& camera, const Matrix& model)
  {
    auto ctx = getContext();
    ctx->projection.back() = camera.projection;
    ctx->projectionVersion++;

    ctx->view.back() = camera.view;
    ctx->viewVersion++;

    ctx->model.back() = model;
    ctx->modelVersion++;
  }

  void pushProjection(const Matrix& m)
  {
    auto ctx = getContext();
    ctx->projection.push_back(m);
    ctx->projectionVersion++;
  }

  void pushProjectionMatrix()
  {
    auto ctx = getContext();
    ctx->projection.push_back(ctx->projection.back());
    ctx->projectionVersion++;
  }

  void popProjectionMatrix()
  {
    auto ctx = getContext();
    if (ctx->projection.empty() == false)
    {
      ctx->projection.pop_back();
      ctx->projectionVersion++;
    }
  }

  void setProjectionMatrix(const Matrix& m)
  {
    auto ctx = getContext();
    if (ctx->projection.empty())
    {
      ctx->projection.push_back(m);
      ctx->projectionVersion++;
    }
    else
    {
      ctx->projection.back() = m;
      ctx->projectionVersion++;
    }
  }

  Matrix getProjectionMatrix()
  {
    auto ctx = getContext();
    return ctx->projection.back();
  }

  void multiplyProjectionMatrix(const Matrix& m)
  {
    auto ctx = getContext();
    ctx->projection.back() *= m;
    ctx->projectionVersion++;
  }

  void pushViewMatrix(const Matrix& m)
  {
    auto ctx = getContext();
    ctx->view.push_back(m);
    ctx->viewVersion++;
  }

  void pushViewMatrix()
  {
    auto ctx = getContext();
    ctx->view.push_back(ctx->view.back());
    ctx->viewVersion++;
  }

  void popViewMatrix()
  {
    auto ctx = getContext();
    if (ctx->view.empty() == false)
    {
      ctx->view.pop_back();
      ctx->viewVersion++;
    }
  }

  void setViewMatrix(const Matrix& m)
  {
    auto ctx = getContext();
    if (ctx->view.empty())
    {
      ctx->view.push_back(m);
      ctx->viewVersion++;
    }
    else
    {
      ctx->view.back() = m;
      ctx->viewVersion++;
    }
  }

  Matrix getViewMatrix()
  {
    auto ctx = getContext();
    return ctx->view.back();
  }

  void multiplyViewMatrix(const Matrix& m)
  {
    auto ctx = getContext();
    ctx->view.back() *= m;
    ctx->viewVersion++;
  }

  void setModelMatrix(const Matrix& m)
  {
    auto ctx = getContext();
    if (ctx->model.empty())
    {
      ctx->model.push_back(m);
      ctx->modelVersion++;
    }
    else
    {
      ctx->model.back() = m;
      ctx->modelVersion++;
    }
  }

  void pushModelMatrix(const Matrix& m)
  {
    auto ctx = getContext();
    ctx->model.push_back(m);
    ctx->modelVersion++;
  }

  void pushModelMatrix()
  {
    auto ctx = getContext();
    ctx->model.push_back(ctx->model.back());
    ctx->modelVersion++;
  }

  void popModelMatrix()
  {
    auto ctx = getContext();
    if (ctx->model.empty() == false)
    {
      ctx->model.pop_back();
      ctx->modelVersion++;
    }
  }

  Matrix getModelMatrix()
  {
    auto ctx = getContext();
    return ctx->model.back();
  }

  void multiplyModelMatrix(const Matrix& m)
  {
    auto ctx = getContext();
    ctx->model.back() *= m;
    ctx->modelVersion++;
  }

  void popState()
  {
    auto ctx = getContext();
    if (ctx->state.empty() == false)
    {
      ctx->state.pop_back();
      ctx->stateVersion++;
    }
  }

  void pushState()
  {
    auto ctx = getContext();
    ctx->state.push_back(ctx->state.back());
    ctx->stateVersion++;
  }

  void pushState(State state)
  {
    auto ctx = getContext();
    ctx->state.push_back(state);
    ctx->stateVersion++;
  }

  void setState(State state)
  {
    auto ctx = getContext();
    if (ctx->state.empty())
    {
      ctx->state.push_back(state);
      ctx->stateVersion++;
    }
    else
    {
      ctx->state.back() = state;
      ctx->stateVersion++;
    }
  }

  State getState()
  {
    auto ctx = getContext();
    return ctx->state.back();
  }

  void draw(const Mesh& mesh)
  {
    draw(mesh.vertexBuffer, mesh.indexBuffer);
  }

  void draw(const bgfx::VertexBufferHandle& vertexBuffer)
  {
    draw(vertexBuffer, BGFX_INVALID_HANDLE);
  }

  void draw(const bgfx::VertexBufferHandle& vertexBuffer, const bgfx::IndexBufferHandle& indexBuffer)
  {
    auto ctx = getContext();
    
    if (ctx->projectionVersion != ctx->lastProjectionVersion || ctx->viewVersion != ctx->lastViewVersion)
    {
      Matrix v = ctx->view.back();
      Matrix p = ctx->projection.back();

      bgfx::setViewTransform(0, v.ptr(), p.ptr());
    }

    if (ctx->modelVersion != ctx->lastModelVersion)
    {
      Matrix m = ctx->model.back();

      bgfx::setTransform(m.ptr());
    }

    if (ctx->stateVersion != ctx->lastStateVersion)
    {
      State state = ctx->state.back();
      
      bgfx::setState(state.value);
    }

    if (vertexBuffer.idx != bgfx::invalidHandle)
    {
      bgfx::setVertexBuffer(vertexBuffer);
    }

    if (indexBuffer.idx != bgfx::invalidHandle)
    {
      bgfx::setIndexBuffer(indexBuffer);
    }

    bgfx::submit(0, ctx->currentProgram);

    ctx->projectionVersion = ctx->lastProjectionVersion;
    ctx->viewVersion = ctx->lastViewVersion;
    ctx->modelVersion = ctx->lastModelVersion;

  }

  void frame()
  {
    bgfx::frame();
  }
}
