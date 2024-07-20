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

static const SplineSegment track[] = {
    {{0.f, 0.f, 60.f}, {10.f, 0.f, 0.f}},
    {{60.f, 0.f, 40.f}, {0.f, 0.f, -10.f}},
    {{40.f, 0.f, 0.f}, {-10.f, 0.f, 0.f}},
    {{20.f, 0.f, -10.f}, {0.f, 0.f, -10.f}},
    {{50.f, 0.f, -40.f}, {0.f, 0.f, -15.f}},
    {{20.f, 0.f, -60.f}, {-10.f, 0.f, 0.f}},
    {{-80.f, 0.f, -60.f}, {-10.f, 0.f, 0.f}},
    {{-146.f, 0.f, -60.f}, {-4.f, 0.f, 0.f}},
    {{-156.f, -9.5f, -60.f}, {-2.f, 0.f, 0.f}},
    {{-220.f, -9.5f, -60.f}, {-10.f, 0.f, 0.f}},
    {{-220.f, -9.5f, -79.f}, {10.f, 0.f, 0.f}},
    {{-176.f, -4.5f, -79.f}, {8.f, 0.f, 0.f}},
    {{-156.f, -9.5f, -70.f}, {2.f, 0.f, 0.f}},
    {{-146.f, 0.f, -70.f}, {4.f, 0.f, 0.f}},
    {{-80.f, 0.f, -60.f}, {10.f, 0.f, 0.f}},
    {{20.f, 0.f, -60.f}, {20.f, 0.f, 0.f}},
    {{50.f, 0.f, -40.f}, {0.f, 0.f, 15.f}},
    {{20.f, 0.f, -10.f}, {0.f, 0.f, 10.f}},
    {{40.f, 0.f, 0.f}, {10.f, 0.f, 0.f}},
    {{60.f, 0.f, 40.f}, {0.f, 0.f, 10.f}},
    {{0.f, 0.f, 60.f}, {-10.f, 0.f, 0.f}}};

static const SplineSegment busRoute[] = {
    {{-55.f, 0.f, -88.f}, {-4.f, 0.f, 0.f}},
    {{-80.f, 0.f, -88.f}, {-6.f, 0.f, 0.f}},
    {{-90.f, 0.f, -100.f}, {-4.f, 0.f, 2.f}},
    {{-102.f, 0.f, -80.f}, {0.5f, 0.f, 4.f}},
    {{-100.f, 0.f, -69.f}, {0.f, 0.f, 2.f}},
    {{-100.f, 0.f, -30.f}, {0.f, 0.f, 10.f}},
    {{-120.f, 0.f, 0.f}, {0.f, 0.f, 12.f}},
    {{-105.f, 0.f, 40.f}, {8.f, 0.f, 0.f}},
    {{-55.f, 0.f, 30.f}, {10.f, 0.f, 0.f}}};

} // namespace

void RealScene::setup() {
  instances.reserve(1);
  instances.emplace_back(ID_train);
}

void RealScene::update(double time) {
  auto const beattime = float(time) * (100.f / 60.f);
  auto const beat = std::floor(beattime);
  auto const frac = beattime - beat;

  float const t1 = 0;
  auto const pos = spline(track, t1);
  auto const dir = splineDir(track, t1);

  instances[0].transform =
      glm::translate(glm::mat4(1.f), pos) *
      glm::transpose(
          glm::lookAt(glm::vec3(0.f), -dir, glm::vec3(0.f, 1.f, 0.f)));


  cameraOptions =
      glm::vec4(syncVal("Camera:FocalDist"), syncVal("Camera:FocalLen"), 0.f,
                syncVal("Global:FresnelPow"));
  processOptions =
      glm::vec4(syncVal("Global:Vignette"), syncVal("Global:Fade"), 0.0f, 0.0f);

  auto const cam = unsigned(syncVal("Scene:CamID"));
  switch (cam) {
  default: {
    auto const view =
        glm::vec3(syncVal("Camera:Pos:X"), syncVal("Camera:Pos:Y"),
                  syncVal("Camera:Pos:Z"));
    cameraProjection =
        glm::perspective(glm::radians(45.f), 1920.0f / 1080.0f, 0.1f, 2000.f);
    cameraView =
        glm::lookAt(glm::vec3(pos + view), pos + glm::vec3(0.f, 1.f, 0.f),
                    glm::vec3(0.0f, 1.0f, 0.0f));
  } break;

  }
}
