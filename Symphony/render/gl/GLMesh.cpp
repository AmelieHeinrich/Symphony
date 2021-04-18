#include "GLMesh.h"

namespace symphony
{
	GLMesh::GLMesh(ModelData data)
		: Mesh(data)
	{
		glCreateVertexArrays(1, &MeshVAO);
		glBindVertexArray(MeshVAO);
		MeshVBO = std::make_shared<GLVertexBuffer>(data.RendererResources.first);
		MeshEBO = std::make_shared<GLIndexBuffer>(data.RendererResources.second);
		
		for (auto texture : data.Textures)
			MeshT2D.push_back(std::make_shared<GLTexture2D>(texture.first.c_str()));

		MeshVBO->Bind();
		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, Color));
		glEnableVertexAttribArray(1);
		//texcoords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, TexCoords));
		glEnableVertexAttribArray(2);
		MeshVBO->Unbind();
		glBindVertexArray(0);
	}

	GLMesh::~GLMesh()
	{
		glDeleteVertexArrays(1, &MeshVAO);
		MeshVBO.reset();
		MeshEBO.reset();
		for (auto texture : MeshT2D)
			texture.reset();
		MeshT2D.clear();
	}

	void GLMesh::Draw()
	{
		glBindVertexArray(MeshVAO);
		MeshVBO->Bind();
		MeshEBO->Bind();
		
		for (int i = 0; i < MeshT2D.size(); i++)
			MeshT2D[i]->Bind(i);
	
		glDrawElements(GL_TRIANGLES, MeshEBO->GetIndicesSize() * sizeof(uint32_t), GL_UNSIGNED_INT, nullptr);

		MeshVBO->Unbind();
		MeshEBO->Unbind();
		for (int i = 0; i < MeshT2D.size(); i++)
			MeshT2D[i]->Unbind();
		glBindVertexArray(0);
	}
}