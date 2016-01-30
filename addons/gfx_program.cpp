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

#include "gfx_program.h"
#include <bx/readerwriter.h>

namespace GFX_NS
{

  namespace
  {
    bgfx::ShaderHandle loadShader(const char* shaderName, bx::FileReaderI* reader)
    {
      char path[512];

      switch(bgfx::getRendererType())
      {
        default:
        case bgfx::RendererType::Null: break;
        case bgfx::RendererType::Direct3D9:
        {
          strcpy(path, "shaders/dx9/");
        }
        break;
        case bgfx::RendererType::Direct3D11:
        case bgfx::RendererType::Direct3D12:
        {
          strcpy(path, "shaders/dx11/");
        }
        break;
        case bgfx::RendererType::OpenGLES:
        {
          strcpy(path, "shaders/gles/");
        }
        break;
        case bgfx::RendererType::OpenGL:
        {
          strcpy(path, "shader/glsl/");
        }
        break;
        case bgfx::RendererType::Vulkan:
        {
          strcpy(path, "shader/vulkan/");
        }
        break;
        case bgfx::RendererType::Metal:
        {
          strcpy(path, "shader/vulkan/");
        }
        break;
      }

      strcat(path, shaderName);
      strcat(path, ".bin");

      if (bx::open(reader, path) == 0)
      {
        uint32_t size = (uint32_t) bx::getSize(reader);
        auto mem = bgfx::alloc(size + 1);
        bx::read(reader, mem->data, size);
        bx::close(reader);
        mem->data[mem->size - 1] = '\0';

        return bgfx::createShader(mem);
      }

      return BGFX_INVALID_HANDLE;
    }
  }

  bgfx::ProgramHandle loadProgram(const char* vertexShaderName, const char* fragmentShaderName, bx::FileReaderI* reader)
  {
    bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
    bgfx::ShaderHandle vertexShader, fragmentShader;

    bool ownReader = false;

#if BX_CONFIG_CRT_FILE_READER_WRITER
    if (reader == nullptr)
    {
      reader = new bx::CrtFileReader();
      ownReader = true;
    }
#endif
    
    vertexShader = loadShader(vertexShaderName, reader);
    if (vertexShader.idx != bgfx::invalidHandle)
    {
      fragmentShader = loadShader(fragmentShaderName, reader);
      if (fragmentShader.idx != bgfx::invalidHandle)
      {
        program = bgfx::createProgram(vertexShader, fragmentShader, true);
      }
    }
    
#if BX_CONFIG_CRT_FILE_READER_WRITER
    if (ownReader)
    {
      delete reader;
    }
#endif

    return program;
  }
}
