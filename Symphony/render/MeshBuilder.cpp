#include "MeshBuilder.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Renderer.h"

namespace std {
    template<> struct hash<symphony::Vertex> {
        size_t operator()(symphony::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.Position) ^ (hash<glm::vec3>()(vertex.Color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.TexCoords) << 1);
        }
    };
}

namespace symphony
{
    static std::mutex s_MeshesMutex;

    ModelData MeshBuilder::MultiThreadedLoadMesh(const char* filepath, const char* texturePath)
    {
        auto mesh = MeshBuilder::LoadModelData(filepath, texturePath);

        std::lock_guard<std::mutex> lock(s_MeshesMutex);
        return mesh;
    }

	std::pair<std::vector<Vertex>, std::vector<uint32_t>> MeshBuilder::CreateModelFromOBJ(const char* filepath)
	{
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath)) {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};

                vertex.Position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.TexCoords = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.Color = { 1.0f, 1.0f, 1.0f };

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }

        return std::make_pair(vertices, indices);
	}

    std::vector<Vertex> MeshBuilder::CreateModelFromOBJVertexOnly(const char* filepath)
    {
        std::vector<Vertex> vertices;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath)) {
            throw std::runtime_error(warn + err);
        }

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};

                vertex.Position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.TexCoords = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.Color = { 1.0f, 1.0f, 1.0f };

                vertices.push_back(vertex);
            }
        }

        return vertices;
    }

    ModelData MeshBuilder::LoadModelData(const char* meshFile, const char* textureFile)
    {
        ModelData returnData{};
        returnData.RendererResources = MeshBuilder::CreateModelFromOBJ(meshFile);
        returnData.TextureFilepath = textureFile;
        return returnData;
    }

    ModelData MeshBuilder::LoadModelDataAsync(const char* meshFile, const char* textureFile)
    {
        std::future<ModelData> result = std::async(std::launch::async, MultiThreadedLoadMesh, meshFile, textureFile);
        return result.get();
    }
}