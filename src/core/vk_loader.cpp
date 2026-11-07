#include "vk_loader.h"
#include "fmt/base.h"
#include "stb_image.h"
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>
#include <iostream>

#include <core/vk_engine.h>
#include <core/vk_initializers.h>
#include <core/vk_types.h>
#include <glm/gtx/quaternion.hpp>
#include <memory>

#include "fastgltf/core.hpp"
#include "fastgltf/glm_element_traits.hpp"
#include "fastgltf/tools.hpp"
#include "fastgltf/types.hpp"
#include "fastgltf/util.hpp"

std::optional<std::vector<std::shared_ptr<MeshAsset>>>
loadGltfMeshes(VulkanEngine *engine, std::filesystem::path filePath) {
  std::cout << "Loading GLTF: " << filePath << std::endl;

  auto dataRes = fastgltf::GltfDataBuffer::FromPath(filePath);
  if (dataRes.error() != fastgltf::Error::None) {
    fmt::println("Failed to read file: {}",
                 fastgltf::getErrorMessage(dataRes.error()));
    return {};
  }

  auto &data = dataRes.get();
  constexpr auto gltfOptions = fastgltf::Options::LoadExternalBuffers;

  fastgltf::Asset gltf;
  fastgltf::Parser parser{};
  auto load = parser.loadGltfBinary(data, filePath.parent_path(), gltfOptions);
  if (load) {
    gltf = std::move(load.get());
  } else {
    fmt::print("Failed to load GLTF: {}\n",
               fastgltf::to_underlying(load.error()));
    return {};
  }

  std::vector<std::shared_ptr<MeshAsset>> meshes;
  std::vector<uint32_t> indices;
  std::vector<Vertex> vertices;

  for (fastgltf::Mesh &mesh : gltf.meshes) {
    MeshAsset newMesh;
    newMesh.name = mesh.name;

    indices.clear();
    vertices.clear();

    for (auto &&p : mesh.primitives) {
      GeoSurface newSurface;
      newSurface.startIndex = (uint32_t)indices.size();

      size_t initial_vtx = vertices.size();

      // load indices
      {
        fastgltf::Accessor &indexAccessor =
            gltf.accessors[p.indicesAccessor.value()];
        indices.reserve(indices.size() * indexAccessor.count);

        fastgltf::iterateAccessor<std::uint32_t>(
            gltf, indexAccessor,
            [&](std::uint32_t idx) { indices.push_back(idx + initial_vtx); });
      }

      // load vertex positions
      {
        fastgltf::Accessor &posAccessor =
            gltf.accessors[p.findAttribute("POSITION")->accessorIndex];
        vertices.resize(vertices.size() + posAccessor.count);

        fastgltf::iterateAccessorWithIndex<glm::vec3>(
            gltf, posAccessor, [&](glm::vec3 v, size_t index) {
              Vertex newvtx;
              newvtx.position = v;
              newvtx.normal = {1, 0, 0};
              newvtx.color = glm::vec4{1.f};
              newvtx.uv_x = 0;
              newvtx.uv_y = 0;
              vertices[initial_vtx + index] = newvtx;
            });
      }

      // load normals
      auto normals = p.findAttribute("NORMAL");
      if (normals != p.attributes.end()) {
        fastgltf::iterateAccessorWithIndex<glm::vec3>(
            gltf, gltf.accessors[(*normals).accessorIndex],
            [&](glm::vec3 v, size_t index) {
              vertices[initial_vtx + index].normal = v;
            });
      }

      // load uv
      auto uv = p.findAttribute("TEXCOORD_0");
      if (uv != p.attributes.end()) {
        fastgltf::iterateAccessorWithIndex<glm::vec2>(
            gltf, gltf.accessors[(*uv).accessorIndex],
            [&](glm::vec2 v, size_t index) {
              vertices[initial_vtx + index].uv_x = v.x;
              vertices[initial_vtx + index].uv_y = v.y;
            });
      }

      // load colors
      auto colors = p.findAttribute("COLOR_0");
      if (colors != p.attributes.end()) {
        fastgltf::iterateAccessorWithIndex<glm::vec4>(
            gltf, gltf.accessors[(*colors).accessorIndex],
            [&](glm::vec4 v, size_t index) {
              vertices[initial_vtx + index].color = v;
            });
      }
      newMesh.surfaces.push_back(newSurface);
    }

    constexpr bool OverrideColors = true;
    if (OverrideColors) {
      for (Vertex &v : vertices) {
        v.color = glm::vec4(v.normal, 1.f);
      }
    }

    newMesh.meshBuffers = engine->uploadMesh(indices, vertices);
    meshes.emplace_back(std::make_shared<MeshAsset>(std::move(newMesh)));
  }

  return meshes;
}
