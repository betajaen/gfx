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

#ifndef __GFX_H__
#define __GFX_H__

#define GFX_NS gfx

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#if BGFX_CONFIG_USE_TINYSTL
# include <tinystl/vector.h>
# define GFX_VECTOR tinystl::vector
#else
# include <vector>
# define GFX_VECTOR std::vector
#endif

namespace GFX_NS
{
  
  struct Vector
  {
    union
    {
      struct { float x, y, z, w; };
      struct { float i, j, k, l; };
      float e[4];
    };

    inline Vector(float S = 0.0f, float W = 1.0f)
    {
      x = S;
      y = S;
      z = S;
      w = W;
    }

    inline Vector(float X, float Y, float Z, float W = 1.0f)
    {
      x = X;
      y = Y;
      z = Z;
      w = W;
    }

    inline float* ptr()
    {
      return &e[0];
    }

    inline const float* ptr() const
    {
      return &e[0];
    }

    const static Vector Zero;
    const static Vector Identity;
    const static Vector PosX;
    const static Vector NegX;
    const static Vector PosY;
    const static Vector NegY;
    const static Vector PosZ;
    const static Vector NegZ;
  };

  struct Matrix
  {
    union
    {
      float e[16];
      float m[4][4];
    };

    Matrix(float s = 1.0f)
    {
      memset(e, 0, sizeof(float) * 16);
      e[0] = e[5] = e[10] = e[15] = s;
    }

    Matrix(float* m)
    {
      memcpy(&e[0], m, sizeof(float) * 16);
    }

    inline void copy(float* p) const
    {
      memcpy(p, &e[0], sizeof(float) * 16);
    }

    inline float* ptr()
    {
      return &e[0];
    }

    inline const float* ptr() const
    {
      return &e[0];
    }

    inline Matrix& operator*=(const Matrix& matrix)
    {
      float t[16];
      bx::mtxMul(&t[0], ptr(), matrix.ptr());
      memcpy(e, t, sizeof(float) * 16);
      return *this;
    }

    inline Matrix operator*(const Matrix& matrix)
    {
      Matrix m;
      bx::mtxMul(m.ptr(), ptr(), matrix.ptr());
      return m;
    }

    static inline Matrix Translate(const Vector& v)
    {
      Matrix m;
      bx::mtxTranslate(m.ptr(), v.x, v.y, v.z);
      return m;
    }

    static inline Matrix Translate(float x, float y, float z)
    {
      Matrix m;
      bx::mtxTranslate(m.ptr(), x, y, z);
      return m;
    }

    static inline Matrix Rotate(const Vector& quat)
    {
      Matrix m;
      bx::mtxQuat(m.ptr(), quat.ptr());
      return m;
    }

    static inline Matrix Rotate(float angle, const Vector& axis)
    {
      Matrix m;
      Vector q;
      bx::quatRotateAxis(q.ptr(), axis.ptr(), angle);
      bx::mtxQuat(m.ptr(), q.ptr());
      return m;
    }

    static inline Matrix RotateX(float ax)
    {
      Matrix m;
      bx::mtxRotateX(m.ptr(), ax);
      return m;
    }

    static inline Matrix RotateY(float ay)
    {
      Matrix m;
      bx::mtxRotateY(m.ptr(), ay);
      return m;
    }

    static inline Matrix RotateZ(float az)
    {
      Matrix m;
      bx::mtxRotateZ(m.ptr(), az);
      return m;
    }

    static inline Matrix RotateXY(float ax, float ay)
    {
      Matrix m;
      bx::mtxRotateXY(m.ptr(), ax, ay);
      return m;
    }

    static inline Matrix RotateXYZ(float ax, float ay, float az)
    {
      Matrix m;
      bx::mtxRotateXYZ(m.ptr(), ax, ay, az);
      return m;
    }

    static inline Matrix RotateZYX(float ax, float ay, float az)
    {
      Matrix m;
      bx::mtxRotateZYX(m.ptr(), ax, ay, az);
      return m;
    }

    static inline Matrix Scale(float s)
    {
      Matrix m;
      bx::mtxScale(m.ptr(), s, s, s);
      return m;
    }

    static inline Matrix Scale(float x, float y, float z)
    {
      Matrix m;
      bx::mtxScale(m.ptr(), x, y, z);
      return m;
    }

    static inline Matrix Scale(const Vector& v)
    {
      Matrix m;
      bx::mtxScale(m.ptr(), v.x, v.y, v.z);
      return m;
    }

    static inline Matrix SRT(const Vector& s, const Vector& a, const Vector& t)
    {
      Matrix m;
      bx::mtxSRT(m.ptr(), s.x, s.y, s.z, a.x, a.y, a.z, t.x, t.y, t.z);
      return m;
    }

    static inline Matrix SRT(float sx, float sy, float sz, float ax, float ay, float az, float tx, float ty, float tz)
    {
      Matrix m;
      bx::mtxSRT(m.ptr(), sx, sy, sz, ax, ay, az, tx, ty, tz);
      return m;
    }

  };

  struct Mesh
  {
    bgfx::VertexBufferHandle vertexBuffer;
    bgfx::IndexBufferHandle  indexBuffer;
  };

  struct Camera
  {
    //
    static Camera LookAtPerspective(const Vector& eye, const Vector& target, float width, float height, float fov = 75.0f, float near = 1.0f, float far = 100.0f);

    //
    static Camera LookAtOrthographic(const Vector& eye, const Vector& target, float aspect = 75.0f, float near = 1.0f, float far = 100.0f);

    Matrix projection, view;
  };

  struct Context;

  //
  void setContext(Context* ctx);

  //
  Context* getContext();

  void pushView(uint8_t view);

  void setView(uint8_t view);

  void popView();

  uint8_t getView();

  //
  void setViewRect(uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

  //
  void touch(uint8_t id);

  //
  void setProgram(const bgfx::ProgramHandle& program);
  
  //
  void setMatrices(const Camera& camera, const Matrix& model = Matrix());

  //
  void setMatrices(const Matrix& projection, const Matrix& view, const Matrix& model = Matrix());
  
  //
  void pushProjectionMatrix(const Matrix& m);

  //
  void pushProjectionMatrix();

  //
  void popProjectionMatrix();

  //
  void setProjectionMatrix(const Matrix& m);

  //
  Matrix getProjectionMatrix();

  //
  void multiplyProjectionMatrix(const Matrix& m);

  //
  void pushViewMatrix(const Matrix& m);

  //
  void pushViewMatrix();

  //
  void popViewMatrix();

  //
  void setViewMatrix(const Matrix& m);

  //
  Matrix getViewMatrix();

  //
  void multiplyViewMatrix(const Matrix& m);

  //
  void pushModelMatrix(const Matrix& m);

  //
  void pushModelMatrix();

  //
  void popModelMatrix();

  //
  void setModelMatrix(const Matrix& m);

  //
  Matrix getModelMatrix();

  //
  void multiplyModelMatrix(const Matrix& m);

  //
  inline void translate(float x, float y, float z)
  {
    multiplyModelMatrix(Matrix::Translate(x, y, z));
  }

  //
  inline void translate(const Vector& t)
  {
    multiplyModelMatrix(Matrix::Translate(t));
  }

  //
  inline void rotate(const Vector& quat)
  {
    multiplyModelMatrix(Matrix::Rotate(quat));
  }

  //
  inline void rotate(float angle, const Vector& axis)
  {
    multiplyModelMatrix(Matrix::Rotate(angle, axis));
  }

  //
  inline void rotateX(float angle)
  {
    multiplyModelMatrix(Matrix::RotateX(angle));
  }

  //
  inline void rotateY(float angle)
  {
    multiplyModelMatrix(Matrix::RotateY(angle));
  }

  //
  inline void rotateZ(float angle)
  {
    multiplyModelMatrix(Matrix::RotateZ(angle));
  }

  //
  inline void rotateXY(float ax, float ay)
  {
    multiplyModelMatrix(Matrix::RotateXY(ax, ay));
  }

  //
  inline void rotateXYZ(float ax, float ay, float az)
  {
    multiplyModelMatrix(Matrix::RotateXYZ(ax, ay, az));
  }

  //
  inline void rotateZYX(float ax, float ay, float az)
  {
    multiplyModelMatrix(Matrix::RotateZYX(ax, ay, az));
  }

  //
  inline void srt(float sx, float sy, float sz, float ax, float ay, float az, float tx, float ty, float tz)
  {
    multiplyModelMatrix(Matrix::SRT(sx, sy, sz, ax, ay, az, tx, ty, tz));
  }

  //
  inline void srt(const Vector& scale, const Vector& rotation, const Vector& translation)
  {
    multiplyModelMatrix(Matrix::SRT(scale, rotation, translation));
  }

  //
  inline void scale(float s)
  {
    multiplyModelMatrix(Matrix::Scale(s));
  }

  //
  inline void scale(float x, float y, float z)
  {
    multiplyModelMatrix(Matrix::Scale(x, y, z));
  }

  //
  inline void scale(const Vector& scale)
  {
    multiplyModelMatrix(Matrix::Scale(scale));
  }

  enum class Write : uint64_t
  {
    RGB   = BGFX_STATE_RGB_WRITE,
    Alpha = BGFX_STATE_ALPHA_WRITE,
    Depth = BGFX_STATE_DEPTH_WRITE
  };

  enum class DepthTest : uint64_t
  {
    Less         = BGFX_STATE_DEPTH_TEST_LESS,
    LessEqual    = BGFX_STATE_DEPTH_TEST_LEQUAL,
    Equal        = BGFX_STATE_DEPTH_TEST_EQUAL,
    GreaterEqual = BGFX_STATE_DEPTH_TEST_GEQUAL,
    Greater      = BGFX_STATE_DEPTH_TEST_GREATER,
    NotEqual     = BGFX_STATE_DEPTH_TEST_NOTEQUAL,
    Never        = BGFX_STATE_DEPTH_TEST_NEVER,
    Always       = BGFX_STATE_DEPTH_TEST_ALWAYS
  };

  enum class Blend : uint64_t
  {
    Zero        = BGFX_STATE_BLEND_ZERO,
    One         = BGFX_STATE_BLEND_ONE,
    SrcColor    = BGFX_STATE_BLEND_SRC_COLOR,
    InvSrcColor = BGFX_STATE_BLEND_INV_SRC_COLOR,
    SrcAlpha    = BGFX_STATE_BLEND_SRC_ALPHA,
    InvSrcAlpha = BGFX_STATE_BLEND_INV_SRC_ALPHA,
    DstAlpha    = BGFX_STATE_BLEND_DST_ALPHA,
    InvDstAlpha = BGFX_STATE_BLEND_INV_DST_ALPHA,
    DstColor    = BGFX_STATE_BLEND_DST_COLOR,
    InvDstColor = BGFX_STATE_BLEND_INV_DST_COLOR,
    SrcAlphaSat = BGFX_STATE_BLEND_SRC_ALPHA_SAT,
    Factor      = BGFX_STATE_BLEND_FACTOR,
    InvFactor   = BGFX_STATE_BLEND_INV_FACTOR
  };

  enum class BlendEquation : uint64_t
  {
    Add    = BGFX_STATE_BLEND_EQUATION_ADD,
    Sub    = BGFX_STATE_BLEND_EQUATION_SUB,
    RevSub = BGFX_STATE_BLEND_EQUATION_REVSUB,
    Min    = BGFX_STATE_BLEND_EQUATION_MIN,
    Max    = BGFX_STATE_BLEND_EQUATION_MAX,
  };

  enum class Cull : uint64_t
  {
    Clockwise        = BGFX_STATE_CULL_CW,
    CounterClockwise = BGFX_STATE_CULL_CCW
  };

  enum class Primitive : uint64_t
  {
    TriStrip  = BGFX_STATE_PT_TRISTRIP,
    Lines     = BGFX_STATE_PT_LINES,
    LineStrip = BGFX_STATE_PT_LINESTRIP,
    Points    = BGFX_STATE_PT_POINTS
  };

  struct State
  {
    uint64_t value;

    const static State DEFAULT;

    inline State()
    {
      value = 0;
    }

    inline State(uint64_t v)
    {
      value = v;
    }

    inline State& toggle(bool enabled, uint64_t flag)
    {
      if (enabled)
        value |= flag;
      else
        value &= ~flag;
      return *this;
    }

    inline State& set(uint64_t v)
    {
      value = v;
      return *this;
    }

    inline State& setDefault()
    {
      value = BGFX_STATE_DEFAULT;
      return *this;
    }

    inline State& write(Write a)
    {
      value = value | static_cast<uint64_t>(a); return *this;
    }
    
    inline State& write(Write a, Write b)
    {
      value = value | static_cast<uint64_t>(a) | static_cast<uint64_t>(b); return *this;
    }
    
    inline State& write(Write a, Write b, Write c)
    {
      value = value | static_cast<uint64_t>(a) | static_cast<uint64_t>(b) | static_cast<uint64_t>(c); return *this;
    }

    inline State& depthTest(DepthTest v)
    {
      value = value | static_cast<uint64_t>(v);
      return *this;
    }

    inline State& blend(Blend v)
    {
      value = value | static_cast<uint64_t>(v);
      return *this;
    }

    inline State& blendEquation(BlendEquation v)
    {
      value = value | static_cast<uint64_t>(v);
      return *this;
    }

    inline State& blendIndependent(bool v)
    {
      return toggle(v, BGFX_STATE_BLEND_INDEPENDENT);
    }

    inline State& culling(Cull v)
    {
      value = value  | static_cast<uint64_t>(v);
      return *this;
    }

    inline State& alphaRef(uint64_t ref)
    {
      value = value | BGFX_STATE_ALPHA_REF(ref);
      return *this;
    }

    inline State& pointSize(uint64_t size)
    {
      value = value | BGFX_STATE_POINT_SIZE(size);
    }

    inline State& primitive(Primitive primitive)
    {
      value = value | static_cast<uint64_t>(primitive);
    }

  };

  void popState();

  void pushState();

  void pushState(State state);

  void setState(State state);

  State getState();

  //
  void draw(const Mesh& mesh);

  //
  void draw(const bgfx::VertexBufferHandle& vertexBuffer);

  //
  void draw(const bgfx::VertexBufferHandle& vertexBuffer, const bgfx::IndexBufferHandle& indexBuffer);

  //
  void frame();


  struct Context
  {
    Context();
    ~Context();


      bgfx::ProgramHandle  currentProgram;
      
      GFX_VECTOR<Matrix>   model, view, projection;
      GFX_VECTOR<State>    state;
      GFX_VECTOR<uint8_t>  views;

      uint32_t modelVersion, viewVersion, projectionVersion;
      uint32_t lastModelVersion, lastViewVersion, lastProjectionVersion;
      uint32_t stateVersion;
      uint32_t lastStateVersion;

      uint32_t viewsVersion;
      uint32_t lastViewsVersion;

  };

}


#endif