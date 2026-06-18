/**
 * @file    `ObjLoader.cpp` 
 * @brief   Loader de modelos geométricos no formato ".obj" 
 */

#include "glad/glad.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "Loaders/ObjLoader.hpp"
#include "matrices.h"

#define CHECK_GL_ERROR() do { \
    GLenum err = glGetError(); \
    if (err != GL_NO_ERROR) { \
        fprintf(stderr, "OpenGL error %d at %s:%d\n", err, __FILE__, __LINE__); \
    } \
} while(0)

/**
 * @brief   Mapa de objetos da cena virtual
 * 
 */
std::map<std::string, SceneObject> g_VirtualScene;

// In ObjLoader.cpp, after the includes:

ObjModel::ObjModel(const char* filename, const char* basepath, bool triangulate) {
    printf("Carregando objetos do arquivo \"%s\"...\n", filename);

    std::string fullpath(filename);
    std::string dirname;
    if (basepath == NULL) {
        auto i = fullpath.find_last_of("/");
        if (i != std::string::npos) {
            dirname = fullpath.substr(0, i+1);
            basepath = dirname.c_str();
        }
    }

    printf("Base path: \"%s\"...", basepath);
    printf("Triangulate: %s...", triangulate ? "true" : "false");
    printf("Carregando...");

    std::string warn;
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate);

    if (!err.empty())
        fprintf(stderr, "\n%s\n", err.c_str());
    if (!ret)
        throw std::runtime_error("Erro ao carregar modelo.");

    if (!warn.empty())
        fprintf(stderr, "\n%s\n", warn.c_str());

    for (size_t shape = 0; shape < shapes.size(); ++shape) {
        if (shapes[shape].name.empty()) {
            fprintf(stderr,
                    "*********************************************\n"
                    "Erro: Objeto sem nome dentro do arquivo '%s'.\n"
                    "Veja https://www.inf.ufrgs.br/~eslgastal/fcg-faq-etc.html#Modelos-3D-no-formato-OBJ .\n"
                    "*********************************************\n",
                filename);
            throw std::runtime_error("Objeto sem nome.");
        }
        printf("Objeto '%s' carregado com sucesso.\n", shapes[shape].name.c_str());
    }

    printf("Carregados %d objetos.\n", shapes.size());
    printf("OK.\n");
}

// Constrói triângulos para futura renderização a partir de um ObjModel.
void BuildTrianglesAndAddToVirtualScene(ObjModel* model)
{
    double startTime = glfwGetTime();

    std::cout
        << "\n=== BuildTrianglesAndAddToVirtualScene ===\n";

    std::cout
        << "Shapes: "
        << model->shapes.size()
        << "\n";

    std::cout
        << "OBJ vertices: "
        << model->attrib.vertices.size() / 3
        << "\n";

    std::cout
        << "OBJ normals: "
        << model->attrib.normals.size() / 3
        << "\n";

    std::cout
        << "OBJ texcoords: "
        << model->attrib.texcoords.size() / 2
        << "\n";

    glfwPollEvents();

    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    std::vector<GLuint> indices;
    std::vector<float> model_coefficients;
    std::vector<float> normal_coefficients;
    std::vector<float> texture_coefficients;

    size_t totalTriangles = 0;

    for(size_t shape = 0;
        shape < model->shapes.size();
        ++shape)
    {
        glfwPollEvents();

        size_t first_index =
            indices.size();

        size_t num_triangles =
            model->shapes[shape]
            .mesh
            .num_face_vertices
            .size();

        totalTriangles += num_triangles;

        const float minval =
            std::numeric_limits<float>::lowest();

        const float maxval =
            std::numeric_limits<float>::max();

        glm::vec3 bbox_min(
            maxval,
            maxval,
            maxval
        );

        glm::vec3 bbox_max(
            minval,
            minval,
            minval
        );

        for(size_t triangle = 0;
            triangle < num_triangles;
            ++triangle)
        {
            assert(
                model->shapes[shape]
                .mesh
                .num_face_vertices[triangle]
                == 3
            );

            for(size_t vertex = 0;
                vertex < 3;
                ++vertex)
            {
                tinyobj::index_t idx =
                    model->shapes[shape]
                    .mesh
                    .indices[3 * triangle + vertex];

                indices.push_back(
                    first_index +
                    3 * triangle +
                    vertex
                );

                const float vx =
                    model->attrib.vertices[
                        3 * idx.vertex_index + 0
                    ];

                const float vy =
                    model->attrib.vertices[
                        3 * idx.vertex_index + 1
                    ];

                const float vz =
                    model->attrib.vertices[
                        3 * idx.vertex_index + 2
                    ];

                model_coefficients.push_back(vx);
                model_coefficients.push_back(vy);
                model_coefficients.push_back(vz);
                model_coefficients.push_back(1.0f);

                bbox_min.x =
                    std::min(
                        bbox_min.x,
                        vx
                    );

                bbox_min.y =
                    std::min(
                        bbox_min.y,
                        vy
                    );

                bbox_min.z =
                    std::min(
                        bbox_min.z,
                        vz
                    );

                bbox_max.x =
                    std::max(
                        bbox_max.x,
                        vx
                    );

                bbox_max.y =
                    std::max(
                        bbox_max.y,
                        vy
                    );

                bbox_max.z =
                    std::max(
                        bbox_max.z,
                        vz
                    );

                if(idx.normal_index != -1)
                {
                    const float nx =
                        model->attrib.normals[
                            3 * idx.normal_index + 0
                        ];

                    const float ny =
                        model->attrib.normals[
                            3 * idx.normal_index + 1
                        ];

                    const float nz =
                        model->attrib.normals[
                            3 * idx.normal_index + 2
                        ];

                    normal_coefficients.push_back(nx);
                    normal_coefficients.push_back(ny);
                    normal_coefficients.push_back(nz);
                    normal_coefficients.push_back(0.0f);
                }

                if(idx.texcoord_index != -1)
                {
                    const float u =
                        model->attrib.texcoords[
                            2 * idx.texcoord_index + 0
                        ];

                    const float v =
                        model->attrib.texcoords[
                            2 * idx.texcoord_index + 1
                        ];

                    texture_coefficients.push_back(u);
                    texture_coefficients.push_back(v);
                }
            }
        }

        size_t last_index =
            indices.size() - 1;

        SceneObject object;

        object.name =
            model->shapes[shape].name;

        object.first_index =
            first_index;

        object.num_indices =
            last_index -
            first_index +
            1;

        object.rendering_mode =
            GL_TRIANGLES;

        object.vertex_array_object_id =
            vertex_array_object_id;

        object.bbox_min =
            bbox_min;

        object.bbox_max =
            bbox_max;

        g_VirtualScene[
            model->shapes[shape].name
        ] = object;
    }

    std::cout
        << "Total triangles: "
        << totalTriangles
        << "\n";

    std::cout
        << "Generated GPU vertices: "
        << model_coefficients.size() / 4
        << "\n";

    std::cout
        << "Generated indices: "
        << indices.size()
        << "\n";

    std::cout
        << "Vertex buffer size: "
        << (model_coefficients.size() * sizeof(float))
            / (1024.0 * 1024.0)
        << " MB\n";

    glfwPollEvents();

    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER,
                 VBO_model_coefficients_id);

    glBufferData(
        GL_ARRAY_BUFFER,
        model_coefficients.size()
            * sizeof(float),
        model_coefficients.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        4,
        GL_FLOAT,
        GL_FALSE,
        0,
        nullptr
    );

    glEnableVertexAttribArray(0);

    if(!normal_coefficients.empty())
    {
        GLuint vboNormals;

        glGenBuffers(
            1,
            &vboNormals
        );

        glBindBuffer(
            GL_ARRAY_BUFFER,
            vboNormals
        );

        glBufferData(
            GL_ARRAY_BUFFER,
            normal_coefficients.size()
                * sizeof(float),
            normal_coefficients.data(),
            GL_STATIC_DRAW
        );

        glVertexAttribPointer(
            1,
            4,
            GL_FLOAT,
            GL_FALSE,
            0,
            nullptr
        );

        glEnableVertexAttribArray(1);
    }

    if(!texture_coefficients.empty())
    {
        GLuint vboTex;

        glGenBuffers(
            1,
            &vboTex
        );

        glBindBuffer(
            GL_ARRAY_BUFFER,
            vboTex
        );

        glBufferData(
            GL_ARRAY_BUFFER,
            texture_coefficients.size()
                * sizeof(float),
            texture_coefficients.data(),
            GL_STATIC_DRAW
        );

        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            0,
            nullptr
        );

        glEnableVertexAttribArray(2);
    }

    GLuint indices_id;

    glGenBuffers(
        1,
        &indices_id
    );

    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        indices_id
    );

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size()
            * sizeof(GLuint),
        indices.data(),
        GL_STATIC_DRAW
    );

    glBindVertexArray(0);

    std::cout
        << "Build time: "
        << glfwGetTime() - startTime
        << " s\n";

    std::cout
        << "====================================\n";
}


// Função que computa as normais de um ObjModel, caso elas não tenham sido
// especificadas dentro do arquivo ".obj"
void ComputeNormals(ObjModel* model)
{
    if ( !model->attrib.normals.empty() )
        return;

    // Primeiro computamos as normais para todos os TRIÂNGULOS.
    // Segundo, computamos as normais dos VÉRTICES através do método proposto
    // por Gouraud, onde a normal de cada vértice vai ser a média das normais de
    // todas as faces que compartilham este vértice e que pertencem ao mesmo "smoothing group".

    // Obtemos a lista dos smoothing groups que existem no objeto
    std::set<unsigned int> sgroup_ids;
    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        assert(model->shapes[shape].mesh.smoothing_group_ids.size() == num_triangles);

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);
            unsigned int sgroup = model->shapes[shape].mesh.smoothing_group_ids[triangle];
            assert(sgroup >= 0);
            sgroup_ids.insert(sgroup);
        }
    }

    size_t num_vertices = model->attrib.vertices.size() / 3;
    model->attrib.normals.reserve( 3*num_vertices );

    // Processamos um smoothing group por vez
    for (const unsigned int & sgroup : sgroup_ids)
    {
        std::vector<int> num_triangles_per_vertex(num_vertices, 0);
        std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f,0.0f,0.0f,0.0f));

        // Acumulamos as normais dos vértices de todos triângulos deste smoothing group
        for (size_t shape = 0; shape < model->shapes.size(); ++shape)
        {
            size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

            for (size_t triangle = 0; triangle < num_triangles; ++triangle)
            {
                unsigned int sgroup_tri = model->shapes[shape].mesh.smoothing_group_ids[triangle];

                if (sgroup_tri != sgroup)
                    continue;

                glm::vec4  vertices[3];
                for (size_t vertex = 0; vertex < 3; ++vertex)
                {
                    tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                    const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                    const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                    const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                    vertices[vertex] = glm::vec4(vx,vy,vz,1.0);
                }

                const glm::vec4  a = vertices[0];
                const glm::vec4  b = vertices[1];
                const glm::vec4  c = vertices[2];

                const glm::vec4  n = crossproduct(b-a,c-a);

                for (size_t vertex = 0; vertex < 3; ++vertex)
                {
                    tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                    num_triangles_per_vertex[idx.vertex_index] += 1;
                    vertex_normals[idx.vertex_index] += n;
                }
            }
        }

        // Computamos a média das normais acumuladas
        std::vector<size_t> normal_indices(num_vertices, 0);

        for (size_t vertex_index = 0; vertex_index < vertex_normals.size(); ++vertex_index)
        {
            if (num_triangles_per_vertex[vertex_index] == 0)
                continue;

            glm::vec4 n = vertex_normals[vertex_index] / (float)num_triangles_per_vertex[vertex_index];
            n /= norm(n);

            model->attrib.normals.push_back( n.x );
            model->attrib.normals.push_back( n.y );
            model->attrib.normals.push_back( n.z );

            size_t normal_index = (model->attrib.normals.size() / 3) - 1;
            normal_indices[vertex_index] = normal_index;
        }

        // Escrevemos os índices das normais para os vértices dos triângulos deste smoothing group
        for (size_t shape = 0; shape < model->shapes.size(); ++shape)
        {
            size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

            for (size_t triangle = 0; triangle < num_triangles; ++triangle)
            {
                unsigned int sgroup_tri = model->shapes[shape].mesh.smoothing_group_ids[triangle];

                if (sgroup_tri != sgroup)
                    continue;

                for (size_t vertex = 0; vertex < 3; ++vertex)
                {
                    tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                    model->shapes[shape].mesh.indices[3*triangle + vertex].normal_index =
                        normal_indices[ idx.vertex_index ];
                }
            }
        }

    }
}

// Função para debugging: imprime no terminal todas informações de um modelo
// geométrico carregado de um arquivo ".obj".
// Veja: https://github.com/syoyo/tinyobjloader/blob/22883def8db9ef1f3ffb9b404318e7dd25fdbb51/loader_example.cc#L98
void PrintObjModelInfo(ObjModel* model)
{
  const tinyobj::attrib_t                & attrib    = model->attrib;
  const std::vector<tinyobj::shape_t>    & shapes    = model->shapes;
  const std::vector<tinyobj::material_t> & materials = model->materials;

  printf("# of vertices  : %d\n", (int)(attrib.vertices.size() / 3));
  printf("# of normals   : %d\n", (int)(attrib.normals.size() / 3));
  printf("# of texcoords : %d\n", (int)(attrib.texcoords.size() / 2));
  printf("# of shapes    : %d\n", (int)shapes.size());
  printf("# of materials : %d\n", (int)materials.size());

  for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
    printf("  v[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.vertices[3 * v + 0]),
           static_cast<const double>(attrib.vertices[3 * v + 1]),
           static_cast<const double>(attrib.vertices[3 * v + 2]));
  }

  for (size_t v = 0; v < attrib.normals.size() / 3; v++) {
    printf("  n[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.normals[3 * v + 0]),
           static_cast<const double>(attrib.normals[3 * v + 1]),
           static_cast<const double>(attrib.normals[3 * v + 2]));
  }

  for (size_t v = 0; v < attrib.texcoords.size() / 2; v++) {
    printf("  uv[%ld] = (%f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.texcoords[2 * v + 0]),
           static_cast<const double>(attrib.texcoords[2 * v + 1]));
  }

  // For each shape
  for (size_t i = 0; i < shapes.size(); i++) {
    printf("shape[%ld].name = %s\n", static_cast<long>(i),
           shapes[i].name.c_str());
    printf("Size of shape[%ld].indices: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.indices.size()));

    size_t index_offset = 0;

    assert(shapes[i].mesh.num_face_vertices.size() ==
           shapes[i].mesh.material_ids.size());

    printf("shape[%ld].num_faces: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.num_face_vertices.size()));

    // For each face
    for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
      size_t fnum = shapes[i].mesh.num_face_vertices[f];

      printf("  face[%ld].fnum = %ld\n", static_cast<long>(f),
             static_cast<unsigned long>(fnum));

      // For each vertex in the face
      for (size_t v = 0; v < fnum; v++) {
        tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
        printf("    face[%ld].v[%ld].idx = %d/%d/%d\n", static_cast<long>(f),
               static_cast<long>(v), idx.vertex_index, idx.normal_index,
               idx.texcoord_index);
      }

      printf("  face[%ld].material_id = %d\n", static_cast<long>(f),
             shapes[i].mesh.material_ids[f]);

      index_offset += fnum;
    }

    printf("shape[%ld].num_tags: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.tags.size()));
    for (size_t t = 0; t < shapes[i].mesh.tags.size(); t++) {
      printf("  tag[%ld] = %s ", static_cast<long>(t),
             shapes[i].mesh.tags[t].name.c_str());
      printf(" ints: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].intValues.size(); ++j) {
        printf("%ld", static_cast<long>(shapes[i].mesh.tags[t].intValues[j]));
        if (j < (shapes[i].mesh.tags[t].intValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");

      printf(" floats: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].floatValues.size(); ++j) {
        printf("%f", static_cast<const double>(
                         shapes[i].mesh.tags[t].floatValues[j]));
        if (j < (shapes[i].mesh.tags[t].floatValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");

      printf(" strings: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].stringValues.size(); ++j) {
        printf("%s", shapes[i].mesh.tags[t].stringValues[j].c_str());
        if (j < (shapes[i].mesh.tags[t].stringValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");
      printf("\n");
    }
  }

  for (size_t i = 0; i < materials.size(); i++) {
    printf("material[%ld].name = %s\n", static_cast<long>(i),
           materials[i].name.c_str());
    printf("  material.Ka = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].ambient[0]),
           static_cast<const double>(materials[i].ambient[1]),
           static_cast<const double>(materials[i].ambient[2]));
    printf("  material.Kd = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].diffuse[0]),
           static_cast<const double>(materials[i].diffuse[1]),
           static_cast<const double>(materials[i].diffuse[2]));
    printf("  material.Ks = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].specular[0]),
           static_cast<const double>(materials[i].specular[1]),
           static_cast<const double>(materials[i].specular[2]));
    printf("  material.Tr = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].transmittance[0]),
           static_cast<const double>(materials[i].transmittance[1]),
           static_cast<const double>(materials[i].transmittance[2]));
    printf("  material.Ke = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].emission[0]),
           static_cast<const double>(materials[i].emission[1]),
           static_cast<const double>(materials[i].emission[2]));
    printf("  material.Ns = %f\n",
           static_cast<const double>(materials[i].shininess));
    printf("  material.Ni = %f\n", static_cast<const double>(materials[i].ior));
    printf("  material.dissolve = %f\n",
           static_cast<const double>(materials[i].dissolve));
    printf("  material.illum = %d\n", materials[i].illum);
    printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
    printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
    printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
    printf("  material.map_Ns = %s\n",
           materials[i].specular_highlight_texname.c_str());
    printf("  material.map_bump = %s\n", materials[i].bump_texname.c_str());
    printf("  material.map_d = %s\n", materials[i].alpha_texname.c_str());
    printf("  material.disp = %s\n", materials[i].displacement_texname.c_str());
    printf("  <<PBR>>\n");
    printf("  material.Pr     = %f\n", materials[i].roughness);
    printf("  material.Pm     = %f\n", materials[i].metallic);
    printf("  material.Ps     = %f\n", materials[i].sheen);
    printf("  material.Pc     = %f\n", materials[i].clearcoat_thickness);
    printf("  material.Pcr    = %f\n", materials[i].clearcoat_thickness);
    printf("  material.aniso  = %f\n", materials[i].anisotropy);
    printf("  material.anisor = %f\n", materials[i].anisotropy_rotation);
    printf("  material.map_Ke = %s\n", materials[i].emissive_texname.c_str());
    printf("  material.map_Pr = %s\n", materials[i].roughness_texname.c_str());
    printf("  material.map_Pm = %s\n", materials[i].metallic_texname.c_str());
    printf("  material.map_Ps = %s\n", materials[i].sheen_texname.c_str());
    printf("  material.norm   = %s\n", materials[i].normal_texname.c_str());
    std::map<std::string, std::string>::const_iterator it(
        materials[i].unknown_parameter.begin());
    std::map<std::string, std::string>::const_iterator itEnd(
        materials[i].unknown_parameter.end());

    for (; it != itEnd; it++) {
      printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
    }
    printf("\n");
  }
};


void DrawVirtualObject(const char* name) {
    auto it = g_VirtualScene.find(name);
    if (it == g_VirtualScene.end()) {
        fprintf(stderr, "DrawVirtualObject: objeto '%s' não encontrado\n", name);
        return;
    }
    const SceneObject& obj = it->second;

    // Verifica se o VAO é válido
    if (obj.vertex_array_object_id == 0) {
        fprintf(stderr, "DrawVirtualObject: VAO inválido para '%s'\n", name);
        return;
    };

    // Envia bounding boxes (se os uniforms existirem)
    extern GLint g_bbox_min_uniform;
    extern GLint g_bbox_max_uniform;

    // Verifica se o shader program está ativo (opcional)
    GLint current_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
    if (current_program == 0) {
        fprintf(stderr, "DrawVirtualObject: nenhum shader program ativo!\n");
        return;
    }

    // Se os uniforms são válidos (diferentes de -1), envia
    if (g_bbox_min_uniform >= 0)
    {
        glUniform3fv(
            g_bbox_min_uniform,
            1,
            glm::value_ptr(obj.bbox_min)
        );
    }

    if (g_bbox_max_uniform >= 0)
    {
        glUniform3fv(
            g_bbox_max_uniform,
            1,
            glm::value_ptr(obj.bbox_max)
        );
    }

    //  printf("Drawing object: %s\n", name);
    /**
    printf(
        "bbox uniforms = %d %d\n",
        g_bbox_min_uniform,
        g_bbox_max_uniform
    );  */

    // Verifica se o VAO está realmente vinculado (glGetIntegerv(GL_VERTEX_ARRAY_BINDING))
    GLint current_vao;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current_vao);
    if (current_vao != (GLint)obj.vertex_array_object_id) {
        glBindVertexArray(obj.vertex_array_object_id);
    }

    // Verifica se o EBO está vinculado ao VAO
    glBindVertexArray(obj.vertex_array_object_id);
    GLint current_ebo;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &current_ebo);
    if (current_ebo == 0) {
        fprintf(stderr, "DrawVirtualObject: nenhum EBO vinculado ao VAO para '%s'\n", name);
        return;
    }

    // Verifica se o número de índices é válido
    if (obj.num_indices == 0) {
        fprintf(stderr, "DrawVirtualObject: objeto '%s' tem 0 índices\n", name);
        return;
    }

    // Verifica o modo de renderização
    if (obj.rendering_mode != GL_TRIANGLES && obj.rendering_mode != GL_TRIANGLE_STRIP && obj.rendering_mode != GL_TRIANGLE_FAN) {
        fprintf(stderr, "DrawVirtualObject: modo de renderização inválido para '%s'\n", name);
        return;
    }

    glGetIntegerv(
        GL_ELEMENT_ARRAY_BUFFER_BINDING,
        &current_ebo
    );

    /**
    printf(
        "Drawing %s | VAO=%u EBO=%d first=%zu count=%zu mode=%u\n",
        name,
        obj.vertex_array_object_id,
        current_ebo,
        obj.first_index,
        obj.num_indices,
        obj.rendering_mode
    );
    */

    while(glGetError() != GL_NO_ERROR);

    // printf("Error before draw = %d\n", glGetError());

    // Desenha
    glDrawElements(obj.rendering_mode, (GLsizei)obj.num_indices, GL_UNSIGNED_INT, (void*)(obj.first_index * sizeof(GLuint)));
    CHECK_GL_ERROR(); // Após glDrawElements

    GLenum err = glGetError();
    // printf("Error after draw = %u\n", err);
    glBindVertexArray(0);
}