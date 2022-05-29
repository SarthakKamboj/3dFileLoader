#include <iostream>
#include "fbxLoader.h"
#include <fbxsdk.h>
#include "fbxsdk/utils/fbxgeometryconverter.h"
#include <stdlib.h>
#include "glm/glm.hpp"
#include <vector>

Mesh getNodeData(FbxNode* node);

SceneData loadFbx(const char* fbxFilePath) {
	FbxManager* fbxManager = FbxManager::Create();
	FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ioSettings);
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");

	SceneData sceneData = {};

	if (!fbxImporter->Initialize(fbxFilePath, -1, fbxManager->GetIOSettings())) {
		std::cout << "fbx import failed" << std::endl;
		std::cout << "ERROR: " << fbxImporter->GetStatus().GetErrorString() << std::endl;
		sceneData.meshCount = -1;
		sceneData.meshes = NULL;
		return sceneData;
	}
	FbxScene* scene = FbxScene::Create(fbxManager, "scene");

	fbxImporter->Import(scene);
	fbxImporter->Destroy();

	std::cout << "successfully loaded FBX" << std::endl;

	FbxNode* rootNode = scene->GetRootNode();
	Mesh* meshes = new Mesh[rootNode->GetChildCount()];
	sceneData.meshes = meshes;
	sceneData.meshCount = rootNode->GetChildCount();
	if (rootNode) {
		for (int i = 0; i < rootNode->GetChildCount(); i++) {
			FbxNode* childNode = rootNode->GetChild(i);
			Mesh mesh = getNodeData(childNode);
			if (mesh.vertices == NULL) {
				sceneData.meshCount = -1;
				sceneData.meshes = NULL;
				return sceneData;
			}
			meshes[i] = mesh;
		}
	}

	fbxManager->Destroy();

	return sceneData;

}

float randNum() {
	return ((rand() % 10) / 10.0f);
}

struct NormalData {
	std::vector<glm::vec3> splitNormals;
	glm::vec3 avgNormal;
	int numCoords;
};

Mesh getNodeData(FbxNode* node) {
	Mesh mesh = {};

	const char* nodeName = node->GetName();

	FbxDouble3 translation = node->LclTranslation.Get();
	FbxDouble3 rotation = node->LclRotation.Get();
	FbxDouble3 scaling = node->LclScaling.Get();

	mesh.position = glm::vec3(translation[0], translation[1], translation[2]);
	mesh.rotation = glm::vec3(rotation[0], rotation[1], rotation[2]);
	mesh.scale = glm::vec3(scaling[0], scaling[1], scaling[2]);

	for (int nodeAttrIdx = 0; nodeAttrIdx < node->GetNodeAttributeCount(); nodeAttrIdx++) {
		FbxNodeAttribute* nodeAttribute = node->GetNodeAttributeByIndex(nodeAttrIdx);
		const char* nodeAttributeName = nodeAttribute->GetName();
		if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
			FbxMesh* fbxMesh = (FbxMesh*)nodeAttribute;

			FbxGeometryElementUV* uvElement = fbxMesh->GetElementUV(0);
			FbxGeometryElement::EMappingMode uvMappingMode = uvElement->GetMappingMode();
			FbxGeometryElement::EReferenceMode uvRefMode = uvElement->GetReferenceMode();

			int numNormalEls = fbxMesh->GetElementNormalCount();
			FbxGeometryElementNormal* normalsElement = fbxMesh->GetElementNormal(0);
			FbxGeometryElement::EMappingMode normalsMappingMode = normalsElement->GetMappingMode();
			FbxGeometryElement::EReferenceMode normalsRefMode = normalsElement->GetReferenceMode();
			std::cout << "numNormalEls: " << numNormalEls << std::endl;

			int numPositions = fbxMesh->GetControlPointsCount();

			NormalData* normalData = new NormalData[numPositions];

			for (int i = 0; i < numPositions; i++) {
				normalData[i].avgNormal = glm::vec3(0.0f, 0.0f, 0.0f);
				normalData[i].numCoords = 0;
			}

			FbxVector4* fbxLclPositions = fbxMesh->GetControlPoints();
			int numIndicies = fbxMesh->GetPolygonVertexCount();
			mesh.vertexCount = numIndicies;
			mesh.vertices = new Vertex[mesh.vertexCount];
			mesh.indexCount = 0;
			mesh.indicies = NULL;

			int vertexId = 0;
			for (int polygonId = 0; polygonId < fbxMesh->GetPolygonCount(); polygonId++) {
				for (int polyVertIdx = 0; polyVertIdx < fbxMesh->GetPolygonSize(polygonId); polyVertIdx++) {
					int positionId = fbxMesh->GetPolygonVertex(polygonId, polyVertIdx);
					Vertex& vertex = mesh.vertices[vertexId];
					FbxDouble* position = fbxLclPositions[positionId];

					vertex.position[0] = position[0];
					vertex.position[1] = position[1];
					vertex.position[2] = position[2];

					vertex.color[0] = (rand() % 10) / 10.0f;
					vertex.color[1] = (rand() % 10) / 10.0f;
					vertex.color[2] = (rand() % 10) / 10.0f;

					vertex.positionId = positionId;

					switch (uvMappingMode) {
						// mapping per vertex 
					case FbxGeometryElement::eByControlPoint:
						switch (uvRefMode) {
							// can get data directly with array
						case FbxGeometryElement::eDirect:
							break;
							// must get index then access array
						case FbxGeometryElement::eIndexToDirect:
							break;
						}
						break;
						// mapping per vertex per polygon
					case FbxGeometryElement::eByPolygonVertex: {
						FbxVector2 uv;
						int idx;
						switch (uvRefMode) {
						case FbxGeometryElement::eDirect:
						case FbxGeometryElement::eIndexToDirect: {
							int uvIdx = fbxMesh->GetTextureUVIndex(polygonId, polyVertIdx);
							uv = uvElement->GetDirectArray().GetAt(uvIdx);

							vertex.uvs[0] = uv[0];
							vertex.uvs[1] = uv[1];
						}
															   break;
						}
					}
															 break;
					case FbxGeometryElement::eByEdge:
						std::cout << "mapping per edge" << std::endl;
						break;
					case FbxGeometryElement::eByPolygon:
						std::cout << "mapping per entire polygon" << std::endl;
						break;
					case FbxGeometryElement::eAllSame:
						std::cout << "same for entire mesh" << std::endl;
						break;
					}

					switch (normalsMappingMode) {
					case FbxGeometryElement::eByControlPoint:
						break;
					case FbxGeometryElement::eByPolygonVertex: {
						switch (normalsRefMode) {
						case FbxGeometryElement::eDirect: {
							FbxVector4 normal = normalsElement->GetDirectArray().GetAt(vertexId);
							vertex.normal[0] = normal[0];
							vertex.normal[1] = normal[1];
							vertex.normal[2] = normal[2];

							glm::vec3 normalGlm(normal[0], normal[1], normal[2]);
							normalGlm = glm::normalize(normalGlm);
							NormalData& data = normalData[positionId];

							normalData[positionId].splitNormals.push_back(normalGlm);
							normalData[positionId].numCoords += 1;

						}
														break;
						case FbxGeometryElement::eIndexToDirect: {
							int idx = normalsElement->GetIndexArray().GetAt(vertexId);
							FbxVector4 normal = normalsElement->GetDirectArray().GetAt(idx);
							vertex.normal[0] = normal[0];
							vertex.normal[1] = normal[1];
							vertex.normal[2] = normal[2];
						}
															   break;
						}
						break;
					}
					}

					vertexId += 1;
				}
			}

			// averaging out normal data
			// TOOD: seems like number of split normals effects avgNormal rn, giving unstable results
			for (int i = 0; i < numPositions; i++) {
				NormalData& data = normalData[i];
				data.avgNormal = glm::vec3(0, 0, 0);

				FbxVector4 position = fbxLclPositions[i];

				for (int i = 0; i < data.numCoords; i++) {
					glm::vec3 normal = data.splitNormals[i];
					data.avgNormal += normal;
				}

				data.avgNormal /= data.numCoords;

				glm::vec3 avgNormal = data.avgNormal;
			}

			for (int i = 0; i < mesh.vertexCount; i++) {
				Vertex* vertices = mesh.vertices;
				glm::vec3 avgNormalForPos = normalData[mesh.vertices[i].positionId].avgNormal;
				vertices[i].avgNormal[0] = avgNormalForPos.x;
				vertices[i].avgNormal[1] = avgNormalForPos.y;
				vertices[i].avgNormal[2] = avgNormalForPos.z;
			}

		}
	}

	return mesh;

	/*
	for (int i = 0; i < node->GetChildCount(); i++) {
		getNodeData(node->GetChild(i));
	}
	*/
}

