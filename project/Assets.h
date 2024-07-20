#pragma once

#include <memory>
#include <vector>
#include "../graphics/Model.h"

enum materialID : size_t {
  ID_mat_train,
};

enum objectID : size_t {
  ID_train,

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
