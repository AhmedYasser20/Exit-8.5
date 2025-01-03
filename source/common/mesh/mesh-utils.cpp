#include "mesh-utils.hpp"

// We will use "Tiny OBJ Loader" to read and process '.obj" files
#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobj/tiny_obj_loader.h>
#include <reactphysics3d/reactphysics3d.h>
namespace rp3d = reactphysics3d;

#include <iostream>
#include <vector>
#include <unordered_map>

our::Mesh* our::mesh_utils::loadOBJ(const std::string& filename) {

    // The data that we will use to initialize our mesh
    std::vector<our::Vertex> vertices;
    std::vector<GLuint> elements;

    // Since the OBJ can have duplicated vertices, we make them unique using this map
    // The key is the vertex, the value is its index in the vector "vertices".
    // That index will be used to populate the "elements" vector.
    std::unordered_map<our::Vertex, GLuint> vertex_map;

    // The data loaded by Tiny OBJ Loader
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    std::cout<< "filename :: " << filename << std::endl;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())) {
        std::cerr << "Failed to load obj file \"" << filename << "\" due to error: " << err << std::endl;
        return nullptr;
    }
    if (!warn.empty()) {
        std::cout << "WARN while loading obj file \"" << filename << "\": " << warn << std::endl;
    }

    // An obj file can have multiple shapes where each shape can have its own material
    // Ideally, we would load each shape into a separate mesh or store the start and end of it in the element buffer to be able to draw each shape separately
    // But we ignored this fact since we don't plan to use multiple materials in the examples
    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            Vertex vertex = {};

            // Read the data for a vertex from the "attrib" object
            vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
            };

            vertex.tex_coord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
            };


            vertex.color = {
                    attrib.colors[3 * index.vertex_index + 0] * 255,
                    attrib.colors[3 * index.vertex_index + 1] * 255,
                    attrib.colors[3 * index.vertex_index + 2] * 255,
                    255
            };

            // See if we already stored a similar vertex
            auto it = vertex_map.find(vertex);
            if (it == vertex_map.end()) {
                // if no, add it to the vertices and record its index
                auto new_vertex_index = static_cast<GLuint>(vertices.size());
                vertex_map[vertex] = new_vertex_index;
                elements.push_back(new_vertex_index);
                vertices.push_back(vertex);
            } else {
                // if yes, just add its index in the elements vector
                elements.push_back(it->second);
            }
        }
    }

    return new our::Mesh(vertices, elements);
}

// Create a sphere (the vertex order in the triangles are CCW from the outside)
// Segments define the number of divisions on the both the latitude and the longitude
our::Mesh* our::mesh_utils::sphere(const glm::ivec2& segments){
    std::vector<our::Vertex> vertices;
    std::vector<GLuint> elements;

    // We populate the sphere vertices by looping over its longitude and latitude
    for(int lat = 0; lat <= segments.y; lat++){
        float v = (float)lat / segments.y;
        float pitch = v * glm::pi<float>() - glm::half_pi<float>();
        float cos = glm::cos(pitch), sin = glm::sin(pitch);
        for(int lng = 0; lng <= segments.x; lng++){
            float u = (float)lng/segments.x;
            float yaw = u * glm::two_pi<float>();
            glm::vec3 normal = {cos * glm::cos(yaw), sin, cos * glm::sin(yaw)};
            glm::vec3 position = normal;
            glm::vec2 tex_coords = glm::vec2(u, v);
            our::Color color = our::Color(255, 255, 255, 255);
            vertices.push_back({position, color, tex_coords, normal});
        }
    }

    for(int lat = 1; lat <= segments.y; lat++){
        int start = lat*(segments.x+1);
        for(int lng = 1; lng <= segments.x; lng++){
            int prev_lng = lng-1;
            elements.push_back(lng + start);
            elements.push_back(lng + start - segments.x - 1);
            elements.push_back(prev_lng + start - segments.x - 1);
            elements.push_back(prev_lng + start - segments.x - 1);
            elements.push_back(prev_lng + start);
            elements.push_back(lng + start);
        }
    }

    return new our::Mesh(vertices, elements);
}
    
    our::Mesh* our::mesh_utils::triangle(const rp3d::Vector3& v0,const rp3d::Vector3& v1,const rp3d::Vector3& v2, const uint32_t& c0, const uint32_t& c1, const uint32_t& c2)
    {
        std::vector<our::Vertex> vertices;
        std::vector<GLuint> elements;
        // colors are encoded R=color>>24 & 0xFF, G=color>>16 & 0xFF, B=color>>8 & 0xFF, A=color & 0xFF
        our::Color color0 = our::Color(c0>>24 & 0xFF, c0>>16 & 0xFF, c0>>8 & 0xFF, c0 & 0xFF);
        our::Color color1 = our::Color(c1>>24 & 0xFF, c1>>16 & 0xFF, c1>>8 & 0xFF, c1 & 0xFF);
        our::Color color2 = our::Color(c2>>24 & 0xFF, c2>>16 & 0xFF, c2>>8 & 0xFF, c2 & 0xFF);
        vertices.push_back({glm::vec3(v0.x, v0.y, v0.z), color0, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)});
        vertices.push_back({glm::vec3(v1.x, v1.y, v1.z), color1, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)});
        vertices.push_back({glm::vec3(v2.x, v2.y, v2.z), color2, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)});
        elements.push_back(0);
        elements.push_back(1);
        elements.push_back(2);
        return new our::Mesh(vertices, elements);
    }

    our::Mesh* our::mesh_utils::line(const rp3d::Vector3& v0,const rp3d::Vector3& v1, const uint32_t& c0, const uint32_t& c1){
    std::vector<our::Vertex> vertices;
    std::vector<GLuint> elements;
    // colors are encoded R=color>>24 & 0xFF, G=color>>16 & 0xFF, B=color>>8 & 0xFF, A=color & 0xFF
    our::Color color0 = our::Color(c0>>24 & 0xFF, c0>>16 & 0xFF, c0>>8 & 0xFF, c0 & 0xFF);
    our::Color color1 = our::Color(c1>>24 & 0xFF, c1>>16 & 0xFF, c1>>8 & 0xFF, c1 & 0xFF);
    vertices.push_back({glm::vec3(v0.x, v0.y, v0.z), color0, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)});
    vertices.push_back({glm::vec3(v1.x, v1.y, v1.z), color1, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)});
    elements.push_back(0);
    elements.push_back(1);
    return new our::Mesh(vertices, elements);
}
