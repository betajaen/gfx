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
#include "gfx_mesh.h"

#include <stdio.h>
#include <bx/readerwriter.h>
#include <locale>

namespace GFX_NS
{

  namespace
  {
    const char kNewLine[] = { '\r', '\n' };
    const char kWhiteSpaceToken = ' ';
    const char* kAttribNames[bgfx::Attrib::Count] = {
      "position",
      "normal",
      "tangent",
      "bitangent",
      "color0",
      "color1",
      "indices",
      "weight",
      "texcoord0",
      "texcoord1",
      "texcoord2",
      "texcoord3",
      "texcoord4",
      "texcoord5",
      "texcoord6",
      "texcoord7"
    };
    
    const char* kTypeNames[bgfx::AttribType::Count] = {
      "uint8",
      "uint10",
      "int16",
      "half",
      "float"
    };

    void writeStr(bx::FileWriterI* writer, const char* str)
    {
      writer->write(str, strlen(str));
    }

    void writeNewLine(bx::FileWriterI* writer)
    {
      writer->write(&kNewLine, 2);
    }

    void writeChar(bx::FileWriterI* writer, const char k)
    {
      writer->write(&k, 1);
    }

    void writeWhiteSpace(bx::FileWriterI* writer)
    {
      writer->write(&kWhiteSpaceToken, 1);
    }

    template<typename T>
    void writeInt(bx::FileWriterI* writer, const T& value, char buffer[512])
    {
      sprintf(buffer, "%i", value);
      writer->write(buffer, strlen(buffer));
    }

    void saveVertexDecl(const bgfx::VertexDecl& decl, bx::FileWriterI* writer, char buffer[512])
    {

      const char k0 = '=';

      for(size_t i=0;i < bgfx::Attrib::Count;i++)
      {

        bgfx::Attrib::Enum attribName = static_cast<bgfx::Attrib::Enum>(i);
        
        if (decl.has(attribName) == false)
          continue;

        writeStr(writer, kAttribNames[i]);
        writeChar(writer, k0);

        uint8_t num;
        bgfx::AttribType::Enum type;
        bool normalised;
        bool asInt;

        decl.decode(attribName, num, type, normalised, asInt);

        writeInt(writer, num, buffer); writeWhiteSpace(writer);

        switch(type)
        {
          default: break;
          case bgfx::AttribType::Uint8:  writeStr(writer, "uint8");  break;
          case bgfx::AttribType::Uint10: writeStr(writer, "uint10"); break;
          case bgfx::AttribType::Int16:  writeStr(writer, "int16");  break;
          case bgfx::AttribType::Half:   writeStr(writer, "half");   break;
          case bgfx::AttribType::Float:  writeStr(writer, "float");  break;
        }

        writeWhiteSpace(writer);

        if (normalised)
        {
          writeStr(writer, "normalised"); writeWhiteSpace(writer);
        }

        if (asInt)
        {
          writeStr(writer, "asInt"); writeWhiteSpace(writer);
        }

        writeNewLine(writer);

      }
    }

    const char* skipWhiteSpace(const char* str)
    {
      while(isspace((*str)))
        str++;
      return str;
    }

    bool readLine(bx::FileReaderI* reader, char line[512])
    {
      char c = 0;
      size_t it = 0;

      while(reader->read(&c, 1) == 1)
      {
        if (it == 0 && isspace(c))
          continue; // skip whitespace.

        if (c == '\r' || c == '\n')
        {
          break;
        }

        line[it++] = c;
      }

      line[it] = '\0';
      
      if (it > 0)
      {
        // reverse trim.
        for (int i = it; i != -1; i--)
        {
          char k = line[i];
          if (isspace(k))
            line[i] = '\0';
          else if (k != '\0')
            break;
        }
      }

      return it > 0;
    }

    const char* readAlphaNumToken(const char* str, char token[64])
    {
      const char* s = skipWhiteSpace(str);
      size_t it = 0;
      while(true)
      {
        char c = *s;
        if (isalnum(c) == false)
        {
          break;
        }

        token[it++] = c;
        s++;
      }
      token[bx::uint32_min(63, it)] = '\0';

      return s;
    }

    int matchToken(const char* m, const char* n[], size_t size)
    {
      for(size_t i=0;i < size;i++)
      {
        if (strcmp(m, n[i]) == 0)
          return i;
      }
      return -1;
    }

    const char* readAlphaToken(const char* str, char token[64])
    {
      const char* s = skipWhiteSpace(str);
      size_t it = 0;
      while (true)
      {
        char c = *s;
        if (isalpha(c) == false)
        {
          break;
        }

        token[it++] = c;
        s++;
      }
      token[bx::uint32_min(63, it)] = '\0';

      return s;
    }

    void makeLowercase(char token[64])
    {
      for(size_t i=0;i < 64;i++)
      {
        if (token[i] == '\0')
          return;
        token[i] = tolower(token[i]);
      }
    }

    int readHexDigit(char c)
    {
      c = tolower(c);
      if (c >= '0' && c <= '9')
      {
        return c - '0';
      }
      else if (c >= 'a' && c <= 'f')
      {
        return c - 'a' + 10;
      }
      else
      {
        return 0;
      }
    }

    template<typename T> const char* readHex(const char* s, int* value)
    {
      (*value) = 0;
      for(int i=0;i < (sizeof(T) * 2);i++)
      {
        char c = (*s++);
        if (c == '\0' || isalnum(c) == false)
          break;
        int x = readHexDigit(c);
        (*value) = ((*value) * 16) + x;
      }
      return s;
    }

    const char* readInt(const char* s, int* value)
    {
      (*value) = 0;

      while(true)
      {
        char c = (*s++);
        if (c == '\0' || isdigit(c) == false)
          break;

        (*value) = ((*value) * 10) + (c - '0');
      }

      return s;
    }

    const char* readDecimal(const char* s, float* value)
    {
      char* e;
      *value = strtof(s, &e);
      return e;
    }


    union AttributeData
    {
      static const size_t size = 16;
      char           _char[16];
      unsigned char  _uchar[16];
      uint16_t       _ushort[size / sizeof(uint16_t)];
      float          _float[size / sizeof(float)];
    };

    void insertVertexData(bx::MemoryWriter& memWriter, size_t offset, size_t index, size_t num, size_t stride, bgfx::AttribType::Enum type, const AttributeData& data)
    {
      switch(type)
      {
        case bgfx::AttribType::Uint8:
        {
          for(size_t i=0;i < num;i++)
            printf("%02X ", data._uchar[i]);
        }
        break;
        case bgfx::AttribType::Uint10:
        case bgfx::AttribType::Int16:
        case bgfx::AttribType::Half:
        {
          for (size_t i = 0; i < num; i++)
            printf("%04X ", data._ushort[i]);
        }
        break;
        case bgfx::AttribType::Float:
        {
          for (size_t i = 0; i < num; i++)
            printf("%f ", data._float[i]);
        }
        break;
      }

      printf("\n");

      size_t at = (index * stride) + offset;

      memWriter.seek(at, bx::Whence::Begin);

      switch(type)
      {
        default: break;
        case bgfx::AttribType::Uint8:
        {
          memWriter.write(&data._uchar[0], num);
        }
        break;
        case bgfx::AttribType::Uint10:
        case bgfx::AttribType::Int16:
        case bgfx::AttribType::Half:
        {
          memWriter.write(&data._uchar[0], sizeof(uint16_t) * num);
        }
        break;
        case bgfx::AttribType::Float:
        {
          memWriter.write(&data._uchar[0], sizeof(float) * num);
        }
        break;
      }

    }

  }

  void loadTextMesh(const char* path, MeshData& meshData, bx::FileReaderI* reader)
  {
    bx::CrtAllocator* alloc = new bx::CrtAllocator();

    bool ownReader = false;

#if BX_CONFIG_CRT_FILE_READER_WRITER
    if (reader == nullptr)
    {
      reader = new bx::CrtFileReader();
      ownReader = true;
    }
#endif

    size_t vertexIndex[(bgfx::Attrib::Count)] = { 0 };

    if (reader->open(path) == 0)
    {
      // First pass.
      //  1. Scan through the entire file looking for decl (i.e. anything with a line with = ) in it.
      //    T. parse that line and add it to the decl, or to the index type
      //    F. ignore the line

      meshData.decl.begin();

      char line[512];
      char token[64];

      while (readLine(reader, line))
      {
        if (strchr(line, '=') == nullptr)
          continue;
        
        // read:
        //  'attribute=' (where attribute is one of kAttributeNames)
        //  or 'index='
        // space between '=' is optional
        const char* s = readAlphaNumToken(line, token);
        
        s = skipWhiteSpace(s);

        if ((*s) != '=')
          continue;

        int attrib = matchToken(token, kAttribNames, bgfx::Attrib::Count);
        
        if (attrib == -1)
        {
          if (strcmp(token, "index") != 0)
            continue;

          // TODO: Read index type.
          continue;
        }

        s++; // skip '='

        // read num (usually 2, 3, 4, 8), only one digit.

        s = skipWhiteSpace(s);
        if (isdigit(*s) == false)
          continue;

        int attribNum = (*s) - '0';
        s++;

        // read type
        s = skipWhiteSpace(s);
        s = readAlphaNumToken(s, token);

        int attribType = matchToken(token, kTypeNames, bgfx::AttribType::Count);


        bool attribNormalised = false;
        bool attribAsInt = false;

        // read optionally; in normalised or asInt
        s = skipWhiteSpace(s);
        if (*s != '\0')
        {
          s = readAlphaToken(s, token);
          makeLowercase(token);

          if (strcmp(token, "normalised") == 0 || strcmp(token, "normalized") == 0)
          {
            attribNormalised = true;
          }
          else
          {
            if (strcmp(token, "asint") == 0)
            {
              attribAsInt = true;
            }
          }
        }

        // read optionally; in asInt or normalised
        s = skipWhiteSpace(s);
        if (*s != '\0')
        {
          s = readAlphaToken(s, token);
          makeLowercase(token);
          if (strcmp(token, "asint") == 0)
          {
            attribAsInt = true;
          }
          else
          {
            if (strcmp(token, "normalised") == 0 || strcmp(token, "normalized") == 0)
            {
              attribNormalised = true;
            }
          }
        }

        meshData.decl.add((bgfx::Attrib::Enum) attrib, attribNum, (bgfx::AttribType::Enum) attribType, attribNormalised, attribAsInt);

      }

      // 1.5 pass
      // Finalise the decl.

      meshData.decl.end();

      size_t stride = meshData.decl.getStride();

      bx::MemoryBlock vertexMem(alloc);
      bx::MemoryWriter vertexMemWriter(&vertexMem);

      // Second pass
      //  For each attribute:
      //    2. Scan through the entire file looking for anything that isn't the decl (anything without a =) in it and is the attribute.
      //      T. parse that line and add it to vertexData
      //      F. ignore line
      for (size_t attrib=0;attrib <  bgfx::Attrib::Count;attrib++)
      {

        if (meshData.decl.has((bgfx::Attrib::Enum)attrib) == false)
          continue;

        // Just keep 
        reader->seek(0, bx::Whence::Enum::Begin);

        uint8_t num;
        bgfx::AttribType::Enum type;
        bool normalised;
        bool asInt;
        meshData.decl.decode((bgfx::Attrib::Enum) attrib, num, type, normalised, asInt);
        size_t offset = meshData.decl.getOffset((bgfx::Attrib::Enum)attrib);

        while (readLine(reader, line))
        {
          if (strchr(line, '=') != nullptr)
            continue;
        
          const char* s = readAlphaNumToken(line, token);

          if (matchToken(token, kAttribNames, bgfx::Attrib::Count) != attrib)
            continue;
          
          AttributeData attributeData;
          size_t idx = 0;

          while(true)
          {
            if ((*s) == '\0')
              break;

            s = skipWhiteSpace(s);

            switch(type)
            {
              case bgfx::AttribType::Uint8:
              {
                int x = 0;
                s = readHex<uint8_t>(s, &x);
                attributeData._uchar[idx] = x;
              }
              break;
              case bgfx::AttribType::Uint10:
              case bgfx::AttribType::Int16:
              case bgfx::AttribType::Half:
              {
                int x = 0;
                s = readHex<uint16_t>(s, &x);
                attributeData._ushort[idx] = x;
              }
              break;
              case bgfx::AttribType::Float:
              {
                float x = 0;
                s = readDecimal(s, &x);
                attributeData._float[idx] = x;
              }
              break;
            }

            idx++;

            if (idx == num)
            {
              insertVertexData(vertexMemWriter, offset, vertexIndex[attrib], num, stride, type, attributeData);
              vertexIndex[attrib]++;
              idx = 0;
            }

          }

        }
      }

      // Third pass, read all indexes
      bx::MemoryBlock indexMem(alloc);
      bx::MemoryWriter indexMemWriter(&indexMem);
      size_t indexCount = 0;
      AttributeData triangleData;

      // Just keep 
      reader->seek(0, bx::Whence::Enum::Begin);
      while (readLine(reader, line))
      {
        if (strchr(line, '=') != nullptr)
          continue;

        const char* s = readAlphaNumToken(line, token);

        if (strcmp(token, "index") != 0)
          continue;

        size_t triangleIdx = 0;

        while (true)
        {
          if ((*s) == '\0')
            break;

          int x = 0;

          s = skipWhiteSpace(s);
          s = readInt(s, &x);

          triangleData._ushort[triangleIdx] = x;
          
          triangleIdx++;

          if (triangleIdx == 3)
          {
            printf("Triangle = %i %i %i\n", triangleData._ushort[0], triangleData._ushort[1], triangleData._ushort[2]);
            //insertVertexData(indexMemWriter, indexCountoffset, vertexIndex[attrib], num, stride, type, attributeData);

            indexMemWriter.write(&triangleData._uchar[0], sizeof(uint16_t) * 3);

            triangleIdx = 0;
            indexCount += 3;

          }

        }

      }



      size_t vertexDataSize = vertexIndex[bgfx::Attrib::Position] * meshData.decl.getStride();
      meshData.vertexData.data = (uint8_t*) BX_ALLOC(alloc, vertexDataSize);
      meshData.vertexData.size = vertexDataSize;
      memcpy(meshData.vertexData.data, vertexMem.more() , vertexDataSize);

      size_t indexDataSize = indexCount * sizeof(uint16_t);
      meshData.indexData.data = (uint8_t*) BX_ALLOC(alloc, indexDataSize);
      meshData.indexData.size = indexDataSize;
      memcpy(meshData.indexData.data, indexMem.more(), indexDataSize);
      
      printf("Verts = %i, Stride = %i\n", vertexIndex[bgfx::Attrib::Position], meshData.decl.getStride());
      printf("Indexes = %i\n", indexDataSize);

    }

#if BX_CONFIG_CRT_FILE_READER_WRITER
    if (ownReader)
    {
      delete reader;
    }
#endif
    
    delete alloc;

  }

  void saveTextMesh(const MeshData& meshData, const char* path, bx::FileWriterI* writer)
  {
    saveTextMesh(meshData.decl, meshData.vertexData.data, (uint16_t*) meshData.indexData.data, meshData.vertexData.size, meshData.indexData.size, path, writer);
  }

  void saveTextMesh(const bgfx::VertexDecl& decl, const void* vertexData, const uint16_t* indexData, size_t vertexDataSize, size_t indexDataSize, const char* path, bx::FileWriterI* writer)
  {
    char buffer[512] = { 0 };
    
    bool ownWriter = false;

#if BX_CONFIG_CRT_FILE_READER_WRITER
    if (writer == nullptr)
    {
      writer = new bx::CrtFileWriter();
      ownWriter = true;
    }
#endif

    if (writer->open(path) == 0)
    {
      // human friendly vertex decl
      saveVertexDecl(decl, writer, buffer);

      writeNewLine(writer);

      size_t vertexCount = vertexDataSize / decl.getStride();

      uint8_t num;
      bgfx::AttribType::Enum type;
      bool normalised;
      bool asInt;

      const size_t unpackMaxSize = sizeof(float) * 4;

      bool wroteMarker = false;
      size_t stride = decl.getStride();

      for(size_t a=0;a < bgfx::Attrib::Count;a++)
      {
        bgfx::Attrib::Enum attribName = static_cast<bgfx::Attrib::Enum>(a);

        if (decl.has(attribName) == false)
          continue;

        decl.decode(attribName, num, type, normalised, asInt);
        size_t offset = decl.getOffset(attribName);

        bool newLine = true;
          
        size_t lineLength = 0;
        for (size_t i = 0; i < vertexCount; i++)
        {
          union
          {
            uint8_t*  data;
            float*    _float;
            uint16_t* _uint16;
          };

          data = (uint8_t*)vertexData + i * stride + offset;

          if (newLine)
          {
            writeStr(writer, kAttribNames[a]);
            writeWhiteSpace(writer);
            newLine = false;
            lineLength = 0;
          }

          switch(type)
          {
            case bgfx::AttribType::Uint8:
            {
              for(size_t k=0; k < num;k++)
              {
                uint8_t c = data[k];
                sprintf(buffer, "%02X", c);
                writeStr(writer, buffer);
                lineLength += strlen(buffer);
              }
              writeWhiteSpace(writer);
            }
            break;
            case bgfx::AttribType::Uint10:
            {
              for (size_t k = 0; k < num; k++)
              {
                if (k > 0)
                  writeWhiteSpace(writer);
                int16_t c = (int16_t)_uint16[k];
                sprintf(buffer, "%04X", c);
                writeStr(writer, buffer);
                lineLength += strlen(buffer);
              }
              writeWhiteSpace(writer);
            }
            break;
            case bgfx::AttribType::Int16:
            {
              for (size_t k = 0; k < num; k++)
              {
                if (k > 0)
                  writeWhiteSpace(writer);
                int16_t c = (int16_t) _uint16[k];
                sprintf(buffer, "%04X", c);
                writeStr(writer, buffer);
                lineLength += strlen(buffer);
              }
              writeWhiteSpace(writer);
            }
            break;
            case bgfx::AttribType::Half:
            {
              for (size_t k = 0; k < num; k++)
              {
                if (k > 0)
                  writeWhiteSpace(writer);
                int16_t c = (int16_t)_uint16[k];
                sprintf(buffer, "%04X", c);
                writeStr(writer, buffer);
                lineLength += strlen(buffer);
              }
              writeWhiteSpace(writer);
            }
            break;
            case bgfx::AttribType::Float:
            {
              for (size_t k = 0; k < num; k++)
              {
                if (k > 0)
                  writeWhiteSpace(writer);
                float c = _float[k];
                sprintf(buffer, "%g", c);
                writeStr(writer, buffer);
                lineLength += strlen(buffer);
              }
              writeWhiteSpace(writer);
              writeWhiteSpace(writer);
            }
            break;
          }

          if (lineLength > 64)
          {
            writeNewLine(writer);
            newLine = true;
          }

        }

        if (newLine == false)
        {
          writeNewLine(writer);
        }
          
      }

      if (indexData != nullptr && indexDataSize > 0)
      {
        writeStr(writer, "index=uint16"); writeNewLine(writer);

        size_t indexCount = indexDataSize / sizeof(uint16_t);

        bool newLine = true;
        size_t lineLength = 0;

        for(size_t i=0;i < indexCount;i++)
        {

          if (newLine)
          {
            writeStr(writer, "index");
            writeWhiteSpace(writer);
            newLine = false;
            lineLength = 0;
          }

          if (i > 0)
            writeWhiteSpace(writer);

          int v = indexData[i];
          sprintf(buffer, "%i", v);
          writeStr(writer, buffer);
          lineLength += strlen(buffer);
          
          if (lineLength > 64)
          {
            writeNewLine(writer);
            newLine = true;
          }

        }

      }

      writer->close();
    }

#if BX_CONFIG_CRT_FILE_READER_WRITER
    if (ownWriter)
    {
      delete writer;
    }
#endif

  }

}
