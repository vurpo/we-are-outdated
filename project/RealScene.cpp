#include "Scenes.h"

#include "../sync/Sync.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Assets.h"
#include "Spline.h"

namespace {
enum Cameras {
  CAM_follow,
};
} // namespace

void RealScene::setup() {
  instances.reserve(12);
  instances.emplace_back(ID_plane);
  instances.emplace_back(ID_tv1);
  instances.emplace_back(ID_tv2);
  instances.emplace_back(ID_tv3);
  instances.emplace_back(ID_tv4);
  instances.emplace_back(ID_tv5);
  instances.emplace_back(ID_tv6);
  instances.emplace_back(ID_tv7);
  instances.emplace_back(ID_tv8);
  instances.emplace_back(ID_tv9);
  instances.emplace_back(ID_wiiu);
  instances.emplace_back(ID_screens);
}

void RealScene::update(double time) {
  auto const scale = glm::scale(glm::mat4(1.f), glm::vec3(0.01f,0.01f,0.01f));
  instances[0].transform = scale;
  instances[1].transform = scale;
  instances[2].transform = scale;
  instances[3].transform = scale;
  instances[4].transform = scale;
  instances[5].transform = scale;
  instances[6].transform = scale;
  instances[7].transform = scale;
  instances[8].transform = scale;
  instances[9].transform = scale;
  instances[10].transform = scale;
  instances[11].transform = scale;

  cameraOptions =
      glm::vec4(syncVal("Camera:FocalDist"), syncVal("Camera:FocalLen"), 0.f,
                syncVal("Global:FresnelPow"));
  processOptions =
      glm::vec4(syncVal("Global:Vignette"), syncVal("Global:Fade"), 0.0f, 0.0f);

  auto const cam = unsigned(syncVal("Scene:CamID"));
  switch (cam) {
  default: {
    float rotZ = syncVal("Camera:RotZ");
    auto const lookat =
        glm::vec3(
          syncVal("Camera:LookAt:X"),
          syncVal("Camera:LookAt:Y"),
          syncVal("Camera:LookAt:Z"));
    auto const pos = glm::vec3(glm::vec4(
          syncVal("Camera:Pos:X"),
          syncVal("Camera:Pos:Y"), 
          syncVal("Camera:Pos:Z"), 1.0)*glm::rotate(glm::mat4(1.f), rotZ, glm::vec3(0.f, 0.f, 1.f)));
    cameraProjection =
        glm::perspective(glm::radians(45.f), 1920.0f / 1080.0f, 0.1f, 2000.f);
    cameraView =
        glm::lookAt(
            lookat+pos,
            lookat,
            glm::vec3(0.f, 0.f, 1.f)
            // glm::vec3(pos + view),
            // pos + glm::vec3(0.f, 1.f, 0.f),
            // glm::vec3(0.0f, 1.0f, 0.0f)
        );
  } break;

  }
}
