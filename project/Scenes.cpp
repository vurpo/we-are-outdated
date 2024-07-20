#include "Scenes.h"

enum sceneID : int { // int because we convert this from a float from the sync
                     // system and unsigned is dangerous
  SCENE_RealScene,
  _MAX_SCENES
};

SceneBase *getScene(int id) {
  sceneID id_conv = (sceneID)std::max(0, std::min(id, (int)_MAX_SCENES - 1));
  switch (id_conv) {
  case SCENE_RealScene:
  default:
    return new RealScene();
  }

  // should be unreachable
  return nullptr;
}
