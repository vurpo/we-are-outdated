#include "Assets.h"

#include "../graphics/LoadUFBX.h"
#include "../graphics/Model.h"
#include "../renderer/RenderMaterial.h"
#include "../renderer/RenderTexture.h"

namespace {
using RenderMaterial_p = std::unique_ptr<RenderMaterial>;

RenderMaterial_p createTextureMaterial(const char *path) {
  std::string path_str(path);
  std::vector<AttribSpec> attribs;
  attribs.push_back(AttribSpec{"in_position", BufferType::VERTEX,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  attribs.push_back(AttribSpec{"in_color", BufferType::COLOR,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32});
  attribs.push_back(AttribSpec{"in_texcoord", BufferType::TEXCOORD,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32});
  attribs.push_back(AttribSpec{"in_normal", BufferType::NORMAL,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  auto renderMaterial = std::make_unique<RenderMaterial>(
      "shaders/projected.vert", "shaders/bones.frag", attribs);
  renderMaterial->setTexture(std::make_unique<RenderTexture>(path_str));
  return renderMaterial;
}

RenderMaterial_p createBoneMaterial(const char *path) {
  std::string path_str(path);
  std::vector<AttribSpec> attribs;
  attribs.push_back(AttribSpec{"in_position", BufferType::VERTEX,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  attribs.push_back(AttribSpec{"in_texcoord", BufferType::TEXCOORD,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32});
  attribs.push_back(AttribSpec{"in_normal", BufferType::NORMAL,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  attribs.push_back(AttribSpec{"in_bone_idx", BufferType::BONE_IDX,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32});
  attribs.push_back(AttribSpec{"in_bone_weight", BufferType::BONE_WEIGHT,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32});
  auto renderMaterial = std::make_unique<RenderMaterial>(
      "shaders/bones.vert", "shaders/bones.frag", attribs);
  renderMaterial->setTexture(std::make_unique<RenderTexture>(path_str));
  return renderMaterial;
}
}  // namespace

Assets::~Assets() = default;

Assets::Assets() {
  // Load materials
  materials.push_back(createBoneMaterial("assets/train_small.png"));
}

std::vector<Model> Assets::createModels() const {
  std::vector<Model> models;

  // Load objects
  models.push_back(ModelFactory::load("assets/train.fbx", NULL,
                                        materials[ID_mat_train].get()));

  return models;
}
