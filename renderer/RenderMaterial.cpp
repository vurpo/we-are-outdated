#include "RenderMaterial.h"
#include "RenderTexture.h"
#include "CafeGLSLCompiler.h"
#include "ShaderManager.h"
#include <coreinit/memdefaultheap.h>
#include <cstdlib>
#include <gx2/shaders.h>
#include <whb/file.h>
#include <whb/gfx.h>
#include <whb/log.h>
#include <whb/sdcard.h>

RenderMaterial::RenderMaterial(const std::string &vertexShaderPath,
                               const std::string &fragmentShaderPath,
                               const std::vector<AttribSpec> &attribs) {
  group = getShaderManager()->shader(vertexShaderPath.c_str(),
                                     fragmentShaderPath.c_str(), attribs);
  for (uint32_t i = 0; i < BufferType::_MAX; ++i) {
    bindingForBuffer[i] = -1;
  }
  for (uint32_t i = 0; i < attribs.size(); ++i) {
    bindingForBuffer[attribs[i].type] = i;
  }
}

void RenderMaterial::renderUsing() const {
  GX2SetFetchShader(&group->fetchShader);
  GX2SetVertexShader(group->vertexShader);
  GX2SetPixelShader(group->pixelShader);
  GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);

  if (texture) {
    texture->renderUsing(group, 0);
  }
  if (texture2) {
    texture2->renderUsing(group, 1);
  }
}

void RenderMaterial::setTexture(std::unique_ptr<RenderTexture> _texture, size_t id) {
  if (id==0) {
    texture = std::move(_texture);
  } else {
    texture2 = std::move(_texture);
  }
}

RenderMaterial::~RenderMaterial() {}
