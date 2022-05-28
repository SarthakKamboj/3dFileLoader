#include <iostream>
#include "fbxLoader.h"
#include <fbxsdk.h>
#include "fbxsdk/utils/fbxgeometryconverter.h"
#include <stdlib.h>

Mesh getNodeData(FbxNode* node);
FbxString getAttributeTypeName(FbxNodeAttribute::EType type);

SceneData loadFbx(const char* fbxFilePath) {
	// const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\arrow.fbx";
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

	/*
	FbxGeometryConverter fbxGeometryConverter(fbxManager);
	if (!fbxGeometryConverter.Triangulate(scene, false)) {
		std::cout << "could not triangulate scene" << std::endl;
		fbxData.meshCount = -1;
		return fbxData;
	}
	*/

	fbxImporter->Import(scene);
	fbxImporter->Destroy();

	std::cout << "successfully loaded FBX" << std::endl;

	FbxNode* rootNode = scene->GetRootNode();
	Mesh* meshes = new Mesh[rootNode->GetChildCount()];
	// Mesh* meshes = new Mesh[1];
	sceneData.meshes = meshes;
	sceneData.meshCount = rootNode->GetChildCount();
	// sceneData.meshCount = 1;
	if (rootNode) {
		for (int i = 0; i < rootNode->GetChildCount(); i++) {
			// for (int i = 0; i < 1; i++) {
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

Mesh getNodeData(FbxNode* node) {
	Mesh mesh;

	const char* nodeName = node->GetName();

	FbxDouble3 translation = node->LclTranslation.Get();
	FbxDouble3 rotation = node->LclRotation.Get();
	FbxDouble3 scaling = node->LclScaling.Get();

	mesh.position = glm::vec3(translation[0], translation[1], translation[2]);
	mesh.rotation = glm::vec3(rotation[0], rotation[1], rotation[2]);
	mesh.scale = glm::vec3(scaling[0], scaling[1], scaling[2]);

	for (int i = 0; i < node->GetNodeAttributeCount(); i++) {
		FbxNodeAttribute* nodeAttribute = node->GetNodeAttributeByIndex(i);
		const char* nodeAttributeName = nodeAttribute->GetName();
		if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
			FbxMesh* fbxMesh = (FbxMesh*)nodeAttribute;

			FbxGeometryElementUV* uvElement = fbxMesh->GetElementUV(0);
			FbxGeometryElement::EMappingMode mappingMode = uvElement->GetMappingMode();
			FbxGeometryElement::EReferenceMode refMode = uvElement->GetReferenceMode();

			int controlPointsSize = fbxMesh->GetControlPointsCount();
			FbxVector4* fbxLclPositions = fbxMesh->GetControlPoints();
			int numIndicies = fbxMesh->GetPolygonVertexCount();
			mesh.vertexCount = numIndicies;
			mesh.vertices = new Vertex[mesh.vertexCount];
			mesh.indexCount = 0;
			mesh.indicies = NULL;

			int i = 0;
			for (int polygonId = 0; polygonId < fbxMesh->GetPolygonCount(); polygonId++) {
				for (int polyVertIdx = 0; polyVertIdx < fbxMesh->GetPolygonSize(polygonId); polyVertIdx++) {
					int positionId = fbxMesh->GetPolygonVertex(polygonId, polyVertIdx);
					Vertex& vertex = mesh.vertices[i];
					FbxDouble* position = fbxLclPositions[positionId].mData;

					vertex.position[0] = position[0];
					vertex.position[1] = position[1];
					vertex.position[2] = position[2];

					vertex.color[0] = (rand() % 10) / 10.0f;
					vertex.color[1] = (rand() % 10) / 10.0f;
					vertex.color[2] = (rand() % 10) / 10.0f;

					i += 1;

					switch (mappingMode) {
						// mapping per vertex 
					case FbxGeometryElement::eByControlPoint:
						switch (refMode) {
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
						switch (refMode) {
						case FbxGeometryElement::eDirect:
						case FbxGeometryElement::eIndexToDirect:
							int uvIdx = fbxMesh->GetTextureUVIndex(polygonId, polyVertIdx);
							uv = uvElement->GetDirectArray().GetAt(uvIdx);

							vertex.uvs[0] = uv.mData[0];
							vertex.uvs[1] = uv.mData[1];
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

				}
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
