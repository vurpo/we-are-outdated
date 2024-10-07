#include "Renderer.h"
#include <cstdlib>
#include <gfd.h>
#include <gx2/draw.h>
#include <gx2/event.h>
#include <gx2/mem.h>
#include <gx2/registers.h>
#include <gx2/shaders.h>
#include <gx2/state.h>
#include <gx2r/buffer.h>
#include <gx2r/draw.h>
#include <stdio.h>
#include <string.h>
#include <whb/file.h>
#include <whb/gfx.h>
#include <whb/log.h>
#include <whb/log_udp.h>

#include <coreinit/cache.h>
#include <coreinit/systeminfo.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>

#include "../graphics/Model.h"
#include "../scenes/SceneBase.h"
#include "../util/memory.h"
#include "PostProcessing.h"
#include "RenderBuffer.h"
#include "RenderMaterial.h"
#include "RenderObject.h"
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include "../sync/Sync.h"

namespace {
static const float sPositionDataQuad[] = {
    // tri 1
    -1.0f,
    1.0f,
    0.0f,
    1.0f,
    1.0f,
    0.0f,
    -1.0f,
    -1.0f,
    0.0f,

    // tri 2
    1.0f,
    1.0f,
    0.0f,
    -1.0f,
    -1.0f,
    0.0f,
    1.0f,
    -1.0f,
    0.0f,
};

static const float sTexcoordData[] = {
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
};

/**
 * Generate a quad object.
 */
std::unique_ptr<RenderObject> createQuad(RenderMaterial *material) {
  auto obj = RenderObject::create();

  obj->setMaterial(material);
  obj->setAttribBuffer(BufferType::VERTEX, sPositionDataQuad, 4 * 3, 6);
  obj->setAttribBuffer(BufferType::TEXCOORD, sTexcoordData, 4 * 2, 6);

  return obj;
}
}  // namespace

#define EFFECTS_W 640
#define EFFECTS_H 360

Renderer::Renderer() {
  composeMaterial = createComposeMaterial();
  composeQuad = createQuad(composeMaterial.get());

  blurMaterial = createBlurMaterial();
  blurQuad = createQuad(blurMaterial.get());

  effectsMaterial = createEffectsMaterial();
  effectsQuad = createQuad(effectsMaterial.get());

  bufferA = std::make_unique<RenderBuffer>(false, 1280 / 2, 720 / 2);
  bufferB = std::make_unique<RenderBuffer>(false, 1280 / 2, 720 / 2);
  effectsBuffer = std::make_unique<RenderBuffer>(false, EFFECTS_W, EFFECTS_H);

  quadInstance = std::make_unique<RenderInstance>();
  view = std::make_unique<RenderView>();
  postView = std::make_unique<RenderView>();
}

void Renderer::renderFrame(SceneBase const &scene, RenderBuffer &rb, std::vector<std::unique_ptr<RenderTexture>> &effectTextures)
{
  GX2SetDepthOnlyControl(GX2_DISABLE, GX2_DISABLE, GX2_COMPARE_FUNC_LESS);

  // Render TV effects to effectsBuffer
  effectsBuffer->bindTarget(true);

    float effectParams[4] = {syncVal("Effects:X"),syncVal("Effects:Effect"),syncVal("Effects:Fade"),0.0};

    postView->setUniformLightColor(&(effectParams[0]));

    effectTextures[syncVal("Effects:Texture")]->renderUsing(effectsMaterial->group, 0);
    effectTextures[0]->renderUsing(effectsMaterial->group, 1);

    effectsQuad->render(*quadInstance, *postView);

  effectsBuffer->unbindTarget();

  // Calculate the average color of the effects buffer
  GX2ColorBuffer *effectBuf = effectsBuffer->getColorBuffer();
  uint32_t r_total = 0;
  uint32_t g_total = 0;
  uint32_t b_total = 0;
  for (size_t i = 0; i < 640*360; i++)
  {
    r_total += ((uint8_t *)effectBuf->surface.image)[i * 4];
    g_total += ((uint8_t *)effectBuf->surface.image)[i * 4 + 1];
    b_total += ((uint8_t *)effectBuf->surface.image)[i * 4 + 2];
  }
  r_total /= 640 * 360;
  g_total /= 640 * 360;
  b_total /= 640 * 360;
  float average_color[4] = {r_total / 255.f, g_total / 255.f, b_total / 255.f, 1.0f};

  // Render the scene itself
  rb.bindTarget(true);

    // Turn on culling, depth test and write
    GX2SetCullOnlyControl(GX2_FRONT_FACE_CCW, GX2_DISABLE, GX2_ENABLE);
    GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_LESS);

    float *cameraProjection = (float *)glm::value_ptr(scene.cameraProjection);

    if (scene.instances.size() > instances.size()) {
      instances.resize(scene.instances.size());
    }
    auto rit = instances.begin();

    view->setUniformFloatMat(UniformType::CAMERA_PROJECTION,
                             (float *)glm::value_ptr(scene.cameraProjection), 16);
    view->setUniformFloatMat(UniformType::CAMERA_VIEW,
                             (float *)glm::value_ptr(scene.cameraView), 16);
    view->setExtraUniform(0, scene.cameraOptions);
    view->setUniformLightColor(&(average_color[0]));

    auto const numObjects = objectList.size();
    for (auto const &instance : scene.instances) {
      if (instance.id >= numObjects) {
        // just in case..
        continue;
      }
      auto const &object = *objectList[instance.id];
      auto &ri = *rit++;

      float *mat = (float *)glm::value_ptr(instance.transform);
      ri.setUniformFloatMat(UniformType::TRANSFORM, mat, 16);
      effectsBuffer->renderUsing(object.getMaterial()->group);
      object.render(ri, *view);
    }
  rb.unbindTarget();

  //  Render to screen with post processing
  WHBGfxBeginRender();
  WHBGfxBeginRenderTV();
  WHBGfxClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  rb.renderUsing(composeQuad->getMaterial()->group, 0);
  postView->setExtraUniform(0, scene.processOptions);
  composeQuad->render(*quadInstance, *postView);
  WHBGfxFinishRenderTV();

  WHBGfxBeginRenderDRC();
  WHBGfxClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  rb.renderUsing(composeQuad->getMaterial()->group, 0);
  postView->setExtraUniform(0, scene.processOptions);
  composeQuad->render(*quadInstance, *postView);
  WHBGfxFinishRenderDRC();

  WHBGfxFinishRender();
}

Renderer::~Renderer() { WHBLogPrintf("Exiting..."); }

void Renderer::addModel(Model const &model) {
  objectList.push_back(RenderObject::create(model));
}

void Renderer::addModel(Model &&model) {
  objectList.push_back(RenderObject::create(std::move(model)));
}

void Renderer::addModels(std::vector<Model> const &models) {
  objectList.reserve(objectList.size() + models.size());
  for (auto const &model : models) {
    objectList.push_back(RenderObject::create(model));
  }
}

void Renderer::addModels(std::vector<Model> &&models) {
  objectList.reserve(objectList.size() + models.size());
  for (auto &model : models) {
    objectList.push_back(std::move(RenderObject::create(model)));
  }
}

void Renderer::reserve(size_t n) {
  if (instances.size() < n) {
    instances.resize(n);
  }
}