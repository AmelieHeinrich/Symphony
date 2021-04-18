#include "MeshBuilder.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define TINYGLTFLOADER_IMPLEMENTATION
#include <tiny_gltf_loader.h>

#include "Renderer.h"
#include "core/Application.h"

namespace std {
    template<> struct hash<symphony::Vertex> {
        size_t operator()(symphony::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.Position) ^ (hash<glm::vec3>()(vertex.Color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.TexCoords) << 1);
        }
    };
}

namespace symphony
{
	std::pair<std::vector<Vertex>, std::vector<uint32_t>> MeshBuilder::CreateModelFromOBJ(const char* filepath, bool loadMaterials)
	{
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath)) {
            SY_CORE_ERROR("OBJ : Failed to load mesh!");
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

                vertex.Normals = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
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

    ModelData MeshBuilder::LoadModelDataInternalOBJ(const char* meshFile, const std::unordered_map<std::string, MaterialTextureType>& textures, bool loadMaterials)
    {
        ModelData returnData{};
        returnData.RendererResources = MeshBuilder::CreateModelFromOBJ(meshFile);
        returnData.Textures = textures;
        return returnData;
    }

    ModelData MeshBuilder::LoadModelData(const char* meshFile, const std::unordered_map<std::string, MaterialTextureType>& textures, bool loadMaterials)
    {
        ThreadPool& pool = Application::Get().GetThreadPool();
        std::future<ModelData> result = pool.Submit(LoadModelDataInternalOBJ, meshFile, textures, loadMaterials);
        return result.get();
    }

    std::vector<MTLData> MeshBuilder::LoadMTLData(const char* meshFile)
    {
        std::vector<MTLData> result;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, meshFile)) {
            SY_CORE_ERROR("OBJ : Failed to load mesh!");
        }

        for (const auto material : materials)
        {
            MTLData temp{};
            
            if (!material.ambient_texname.empty()) temp.ambient_texname = material.ambient_texname;
            if (!material.diffuse_texname.empty()) temp.diffuse_texname = material.diffuse_texname;
            if (!material.specular_texname.empty()) temp.specular_texname = material.specular_texname;
            if (!material.specular_highlight_texname.empty()) temp.specular_highlight_texname = material.specular_highlight_texname;
            if (!material.bump_texname.empty()) temp.bump_texname = material.bump_texname;
            if (!material.displacement_texname.empty()) temp.displacement_texname = material.displacement_texname;
            if (!material.alpha_texname.empty()) temp.alpha_texname = material.alpha_texname;
            if (!material.reflection_texname.empty()) temp.reflection_texname = material.reflection_texname;

            // PBR
            if (!material.roughness_texname.empty()) temp.roughness_texname = material.roughness_texname;
            if (!material.metallic_texname.empty()) temp.metallic_texname = material.metallic_texname;
            if (!material.sheen_texname.empty()) temp.sheen_texname = material.sheen_texname;
            if (!material.emissive_texname.empty()) temp.emissive_texname = material.emissive_texname;
            if (!material.normal_texname.empty()) temp.normal_texname = material.normal_texname;

            temp.roughness = material.roughness;
            temp.metallic = material.metallic;
            temp.sheen = material.sheen;
            temp.clearcoat_thickness = material.clearcoat_thickness;
            temp.clearcoat_roughness = material.clearcoat_roughness;

            temp.ambient[0] = material.ambient[0]; temp.ambient[1] = material.ambient[1]; temp.ambient[2] = material.ambient[2];
            temp.diffuse[0] = material.diffuse[0]; temp.diffuse[1] = material.diffuse[1]; temp.diffuse[2] = material.diffuse[2];
            temp.specular[0] = material.specular[0]; temp.specular[1] = material.specular[1]; temp.specular[2] = material.specular[2];
            temp.transmittance[0] = material.transmittance[0]; temp.transmittance[1] = material.transmittance[1]; temp.transmittance[2] = material.transmittance[2];
            temp.emission[0] = material.emission[0]; temp.emission[1] = material.emission[1]; temp.emission[2] = material.emission[2];

            temp.shininess = material.shininess;
            temp.ior = material.ior;
            temp.dissolve = material.dissolve;
        }

        return result;
    }
}