#include <iostream>
#include "fbxLoader.h"
#include <fbxsdk.h>
#include "fbxsdk/utils/fbxgeometryconverter.h"
#include <stdlib.h>
#include "glm/glm.hpp"

int addMeshToScene(Scene& scene, FbxMesh* fbxMesh);
bool nodeStoresMesh(FbxNode* node);

int meshIdx = 0;
Scene loadFbx(const char* fbxFilePath) {
	// initiliaze fbx loader/manager objects
	FbxManager* fbxManager = FbxManager::Create();
	FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	ioSettings->SetBoolProp(IMP_ANIMATION, false);
	ioSettings->SetBoolProp(IOSN_TRIANGULATE, true);
	fbxManager->SetIOSettings(ioSettings);
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");

	Scene scene = {};
	for (int i = 0; i < MAX_NUM_MESHES_PER_SCENE; i++) {
		scene.topLevelMeshIdxs[i] = -1;
	}

	if (!fbxImporter->Initialize(fbxFilePath, -1, fbxManager->GetIOSettings())) {
		std::cout << "fbx import failed" << std::endl;
		std::cout << "ERROR: " << fbxImporter->GetStatus().GetErrorString() << std::endl;
		scene.numMeshes = -1;
		return scene;
	}
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "scene");

	fbxImporter->Import(fbxScene);

	// triangle imported file data
	FbxGeometryConverter converter(fbxManager);
	for (int nodeIdx = 0; nodeIdx < fbxScene->GetNodeCount(); nodeIdx++) {
		if (nodeStoresMesh(fbxScene->GetNode(nodeIdx))) {
			converter.Triangulate(fbxScene->GetNode(nodeIdx)->GetNodeAttributeByIndex(0), true, false);
		}
	}

	fbxImporter->Destroy();

	FbxNode* rootNode = fbxScene->GetRootNode();
	meshIdx = 0;

	if (rootNode) {
		int topLvlIdxIter = 0;
		for (int i = 0; i < rootNode->GetChildCount(); i++) {
			FbxNode* childNode = rootNode->GetChild(i);
			if (nodeStoresMesh(childNode)) {
				int topLevelMeshIdx = addMeshToScene(scene, (FbxMesh*)childNode->GetNodeAttributeByIndex(0));
				if (topLevelMeshIdx != -1) {
					// store idx to denote this is the highest level parent object
					scene.topLevelMeshIdxs[topLvlIdxIter] = topLevelMeshIdx;
					topLvlIdxIter += 1;
				}
			}
		}
	}
	else {
		scene.numMeshes = -1;
		return scene;
	}

	fbxManager->Destroy();

	return scene;
}

float randNum() {
	return ((rand() % 10) / 10.0f);
}

struct NormalData {
	glm::vec3 splitNormals[5];
	glm::vec3 avgNormal;
	int numCoords;
};

bool nodeStoresMesh(FbxNode* node) {
	for (int nodeAttrIdx = 0; nodeAttrIdx < node->GetNodeAttributeCount(); nodeAttrIdx++) {
		FbxNodeAttribute* nodeAttribute = node->GetNodeAttributeByIndex(nodeAttrIdx);
		FbxNodeAttribute::EType nodeType = nodeAttribute->GetAttributeType();
		switch (nodeType) {
		case FbxNodeAttribute::eMesh: {
			return true;
		}
									break;
		}

	}
	return false;
}

int addMeshToScene(Scene& scene, FbxMesh* fbxMesh) {

	if (scene.numMeshes == MAX_NUM_MESHES_PER_SCENE) return -1;

	Mesh mesh;

	for (int i = 0; i < MAX_NUM_MESHES_PER_SCENE; i++) {
		mesh.childMeshIdxs[i] = -1;
	}

	FbxNode* node = fbxMesh->GetNode();

	const char* meshName = fbxMesh->GetName();
	mesh.name = std::string(meshName);

	// get transformation data
	FbxDouble3 translation = node->LclTranslation.Get();
	FbxDouble3 rotation = node->LclRotation.Get();
	FbxDouble3 scaling = node->LclScaling.Get();

	mesh.transform.position = glm::vec3(translation[0], translation[1], translation[2]);
	mesh.transform.rotation = glm::vec3(rotation[0], rotation[1], rotation[2]);
	mesh.transform.scale = glm::vec3(scaling[0], scaling[1], scaling[2]);

	// get UV mapping data
	FbxGeometryElementUV* uvElement = fbxMesh->GetElementUV(0);
	FbxGeometryElement::EMappingMode uvMappingMode = uvElement->GetMappingMode();
	FbxGeometryElement::EReferenceMode uvRefMode = uvElement->GetReferenceMode();

	// get normals data
	int numNormalEls = fbxMesh->GetElementNormalCount();
	FbxGeometryElementNormal* normalsElement = fbxMesh->GetElementNormal(0);
	FbxGeometryElement::EMappingMode normalsMappingMode = normalsElement->GetMappingMode();
	FbxGeometryElement::EReferenceMode normalsRefMode = normalsElement->GetReferenceMode();

	int numPositions = fbxMesh->GetControlPointsCount();

	NormalData* normalData = new NormalData[numPositions];

	for (int i = 0; i < numPositions; i++) {
		normalData[i].avgNormal = glm::vec3(0.0f, 0.0f, 0.0f);
		normalData[i].numCoords = 0;
	}

	// get unique positions on this fbx mesh
	FbxVector4* fbxLclPositions = fbxMesh->GetControlPoints();
	/*
	- making as many vertices as indicies b/c diff verts may have same pos but diff uv/normals

	TODO: maybe write optimization algorithm later on to merge info together and create indicies on my own that
	account for uv/normals/other info
	*/
	int numIndicies = fbxMesh->GetPolygonVertexCount();
	mesh.vertexCount = numIndicies;
	mesh.vertices = new Vertex[mesh.vertexCount];

	int vertexId = 0;

	int vertPosIds[3000] = {};

	for (int polygonId = 0; polygonId < fbxMesh->GetPolygonCount(); polygonId++) {
		for (int polyVertIdx = 0; polyVertIdx < fbxMesh->GetPolygonSize(polygonId); polyVertIdx++) {
			if (vertexId == 3000) break;
			// get vertex position index to use in positions array
			int positionId = fbxMesh->GetPolygonVertex(polygonId, polyVertIdx);
			vertPosIds[vertexId] = positionId;
			Vertex& vertex = mesh.vertices[vertexId];
			FbxDouble* position = fbxLclPositions[positionId];

			vertex.position[0] = position[0];
			vertex.position[1] = position[1];
			vertex.position[2] = position[2];

			// setting colors randomly right now
			vertex.color[0] = (rand() % 10) / 10.0f;
			vertex.color[1] = (rand() % 10) / 10.0f;
			vertex.color[2] = (rand() % 10) / 10.0f;

			FbxVector2 uv;
			switch (uvMappingMode) {
			case FbxGeometryElement::eByControlPoint: {
				// mapping per vertex 
				switch (uvRefMode) {
				case FbxGeometryElement::eDirect: {
					// can get data directly with array
					uv = uvElement->GetDirectArray().GetAt(positionId);
				}
												break;
				case FbxGeometryElement::eIndexToDirect: {
					// must get index then access array
					int idx = uvElement->GetIndexArray().GetAt(positionId);
					uv = uvElement->GetDirectArray().GetAt(idx);
				}
													   break;
				}
			}
													break;
			case FbxGeometryElement::eByPolygonVertex: {
				// mapping per vertex per polygon
				switch (uvRefMode) {
				case FbxGeometryElement::eDirect:
				case FbxGeometryElement::eIndexToDirect: {
					int uvIdx = fbxMesh->GetTextureUVIndex(polygonId, polyVertIdx);
					uv = uvElement->GetDirectArray().GetAt(uvIdx);
				}
													   break;
				}
			}
													 break;
			}

			vertex.uvs[0] = uv[0];
			vertex.uvs[1] = uv[1];

			FbxVector4 normal;
			switch (normalsMappingMode) {
			case FbxGeometryElement::eByControlPoint: {
				// get data directly from normals data
				normal = normalsElement->GetDirectArray().GetAt(positionId);
			}
													break;
			case FbxGeometryElement::eByPolygonVertex: {
				switch (normalsRefMode) {
				case FbxGeometryElement::eDirect: {
					// get data directly from normals data
					normal = normalsElement->GetDirectArray().GetAt(vertexId);
				}
												break;
				case FbxGeometryElement::eIndexToDirect: {
					// get data directly from normals index array to get normals data
					int idx = normalsElement->GetIndexArray().GetAt(vertexId);
					normal = normalsElement->GetDirectArray().GetAt(idx);
				}
													   break;
				}
				break;
			}
			}
			vertex.normal[0] = normal[0];
			vertex.normal[1] = normal[1];
			vertex.normal[2] = normal[2];

			// add current normal data to the vertex corresponding to the vertex position
			glm::vec3 normalGlm(normal[0], normal[1], normal[2]);
			normalGlm = glm::normalize(normalGlm);
			NormalData& data = normalData[positionId];
			if (normalData[positionId].numCoords < 5) {
				int curSplitNormalIdx = normalData[positionId].numCoords;
				normalData[positionId].splitNormals[curSplitNormalIdx] = normalGlm;
				normalData[positionId].numCoords += 1;
			}

			vertexId += 1;
		}
	}

	// averaging out normal data
	for (int i = 0; i < numPositions; i++) {
		NormalData& data = normalData[i];
		data.avgNormal = glm::vec3(0, 0, 0);

		FbxVector4 position = fbxLclPositions[i];

		for (int i = 0; i < data.numCoords; i++) {
			glm::vec3 normal = data.splitNormals[i];
			data.avgNormal += normal;
		}

		data.avgNormal /= data.numCoords;
	}

	// populating average normal data for vertices
	for (int i = 0; i < mesh.vertexCount; i++) {
		Vertex* vertices = mesh.vertices;
		int meshPosId = vertPosIds[i];
		glm::vec3 avgNormalForPos = normalData[meshPosId].avgNormal;
		vertices[i].avgNormal[0] = avgNormalForPos.x;
		vertices[i].avgNormal[1] = avgNormalForPos.y;
		vertices[i].avgNormal[2] = avgNormalForPos.z;
	}

	int curMeshIdx = meshIdx;
	scene.meshes[curMeshIdx] = mesh;
	scene.numMeshes++;
	meshIdx++;

	// call function recursively for the mesh's children
	mesh.numChildren = 0;
	int childCount = node->GetChildCount();
	for (int i = 0; i < childCount; i++) {
		FbxNode* childNode = node->GetChild(i);
		if (nodeStoresMesh(childNode)) {
			int childMeshIdx = addMeshToScene(scene, (FbxMesh*)childNode->GetNodeAttributeByIndex(0));
			if (childMeshIdx != -1) {
				Mesh& curMesh = scene.meshes[curMeshIdx];
				int curNumChildren = curMesh.numChildren;
				if (curNumChildren < MAX_NUM_MESHES_PER_SCENE) {
					curMesh.childMeshIdxs[curNumChildren] = childMeshIdx;
					curMesh.numChildren += 1;
				}
			}
		}
	}

	return curMeshIdx;
}
