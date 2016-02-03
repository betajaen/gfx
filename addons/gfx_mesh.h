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

#ifndef GFX_MESH_H
#define GFX_MESH_H

#include "gfx.h"

namespace bx
{
  struct FileWriterI;
  struct FileReaderI;
}

namespace GFX_NS
{

  struct MeshData
  {
    MeshData()
      : decl()
    {
      vertexData.data = nullptr;
      vertexData.size = 0;
      indexData.data = nullptr;
      indexData.size = 0;
    }

    bgfx::VertexDecl decl;
    bgfx::Memory vertexData;
    bgfx::Memory indexData;
  };

  //
  void loadTextMesh(const char* path, MeshData& meshData, bx::FileReaderI* _reader = nullptr);

  //
  void saveTextMesh(const bgfx::VertexDecl& decl, const void* vertexData, const uint16_t* indexData, size_t vertexDataSize, size_t indexDataSize, const char* path, bx::FileWriterI* _writer = nullptr);

  //
  void saveTextMesh(const MeshData& meshData, const char* path, bx::FileWriterI* _writer = nullptr);

}

#endif
