#pragma once

#include <memory>
#include <vector>
#include "../graphics/Model.h"

enum materialID : size_t
{
  ID_mat_train,
  ID_mat_plane,
  ID_mat_tv1,
  ID_mat_tv2,
  ID_mat_tv3,
  ID_mat_tv4,
  ID_mat_tv5,
  ID_mat_tv6,
  ID_mat_tv7,
  ID_mat_tv8,
  ID_mat_tv9,
  ID_mat_wiiu,
  ID_mat_screens,
};

enum objectID : size_t
{
  ID_train,
  ID_plane,
  ID_tv1,
  ID_tv2,
  ID_tv3,
  ID_tv4,
  ID_tv5,
  ID_tv6,
  ID_tv7,
  ID_tv8,
  ID_tv9,
  ID_wiiu,
  ID_screens,

  _ID_ASSETS_MAX // keep at end
};

struct Renderer;
struct RenderMaterial;

struct Assets {
  std::vector<std::unique_ptr<RenderMaterial>> materials;

  Assets();
  ~Assets();

  std::vector<Model> createModels() const;
};
