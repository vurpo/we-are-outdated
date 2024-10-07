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
      "shaders/projected.vert", "shaders/plain.frag", attribs);
  renderMaterial->setTexture(std::make_unique<RenderTexture>(path_str), 0);
  return renderMaterial;
}

RenderMaterial_p createLitMaterial(const char *path, const char *path2) {
  std::string path_str(path);
  std::string path2_str(path2);
  std::vector<AttribSpec> attribs;
  attribs.push_back(AttribSpec{"in_position", BufferType::VERTEX,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  attribs.push_back(AttribSpec{"in_texcoord", BufferType::TEXCOORD,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32});
  attribs.push_back(AttribSpec{"in_normal", BufferType::NORMAL,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  auto renderMaterial = std::make_unique<RenderMaterial>(
      "shaders/projected.vert", "shaders/bones.frag", attribs);
  renderMaterial->setTexture(std::make_unique<RenderTexture>(path_str), 0);
  renderMaterial->setTexture(std::make_unique<RenderTexture>(path2_str), 1);
  return renderMaterial;
}

RenderMaterial_p createBufferMaterial()
{
  std::vector<AttribSpec> attribs;
  attribs.push_back(AttribSpec{"in_position", BufferType::VERTEX,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  attribs.push_back(AttribSpec{"in_texcoord", BufferType::TEXCOORD,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32});
  attribs.push_back(AttribSpec{"in_normal", BufferType::NORMAL,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  auto renderMaterial = std::make_unique<RenderMaterial>(
      "shaders/projected.vert", "shaders/plain.frag", attribs);
  // the texture will be set later
  return renderMaterial;
}
}  // namespace

Assets::~Assets() = default;

Assets::Assets() {
  // Load materials
  materials.push_back(createTextureMaterial("assets/train_small.png"));
  materials.push_back(createLitMaterial("assets/plane_light.png", "assets/plane_base.png"));
  materials.push_back(createLitMaterial("assets/tv1_light.png", "assets/tv1_base.png"));
  materials.push_back(createLitMaterial("assets/tv2_light.png", "assets/tv2_base.png"));
  materials.push_back(createLitMaterial("assets/tv3_light.png", "assets/tv3_base.png"));
  materials.push_back(createLitMaterial("assets/tv4_light.png", "assets/tv4_base.png"));
  materials.push_back(createLitMaterial("assets/tv5_light.png", "assets/tv5_base.png"));
  materials.push_back(createLitMaterial("assets/tv6_light.png", "assets/tv6_base.png"));
  materials.push_back(createLitMaterial("assets/tv7_light.png", "assets/tv7_base.png"));
  materials.push_back(createLitMaterial("assets/tv8_light.png", "assets/tv8_base.png"));
  materials.push_back(createLitMaterial("assets/tv9_light.png", "assets/tv9_base.png"));
  materials.push_back(createLitMaterial("assets/wiiu_light.png", "assets/wiiu_base.png"));
  materials.push_back(createBufferMaterial());
}

std::vector<Model> Assets::createModels() const {
  std::vector<Model> models;

  // Load objects
  models.push_back(ModelFactory::load("assets/train.fbx", NULL,
                                      materials[ID_mat_train].get()));
  models.push_back(ModelFactory::load("assets/plane.fbx", NULL,
                                      materials[ID_mat_plane].get()));
  models.push_back(ModelFactory::load("assets/tv1.fbx", NULL,
                                      materials[ID_mat_tv1].get()));
  models.push_back(ModelFactory::load("assets/tv2.fbx", NULL,
                                      materials[ID_mat_tv2].get()));
  models.push_back(ModelFactory::load("assets/tv3.fbx", NULL,
                                      materials[ID_mat_tv3].get()));
  models.push_back(ModelFactory::load("assets/tv4.fbx", NULL,
                                      materials[ID_mat_tv4].get()));
  models.push_back(ModelFactory::load("assets/tv5.fbx", NULL,
                                      materials[ID_mat_tv5].get()));
  models.push_back(ModelFactory::load("assets/tv6.fbx", NULL,
                                      materials[ID_mat_tv6].get()));
  models.push_back(ModelFactory::load("assets/tv7.fbx", NULL,
                                      materials[ID_mat_tv7].get()));
  models.push_back(ModelFactory::load("assets/tv8.fbx", NULL,
                                      materials[ID_mat_tv8].get()));
  models.push_back(ModelFactory::load("assets/tv9.fbx", NULL,
                                      materials[ID_mat_tv9].get()));
  models.push_back(ModelFactory::load("assets/wiiu.fbx", NULL,
                                      materials[ID_mat_wiiu].get()));
  models.push_back(ModelFactory::load("assets/screens.fbx", NULL,
                                      materials[ID_mat_screens].get()));

  return models;
}
