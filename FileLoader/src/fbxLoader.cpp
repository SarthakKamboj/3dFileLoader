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

	SceneData fbxData = {};

	if (!fbxImporter->Initialize(fbxFilePath, -1, fbxManager->GetIOSettings())) {
		std::cout << "fbx import failed" << std::endl;
		std::cout << "ERROR: " << fbxImporter->GetStatus().GetErrorString() << std::endl;
		fbxData.meshCount = -1;
		return fbxData;
	}
	FbxScene* scene = FbxScene::Create(fbxManager, "scene");

	FbxGeometryConverter fbxGeometryConverter(fbxManager);
	if (!fbxGeometryConverter.Triangulate(scene, false)) {
		std::cout << "could not triangulate scene" << std::endl;
		fbxData.meshCount = -1;
		return fbxData;
	}
	//Triangulate(scene, false);

	fbxImporter->Import(scene);
	fbxImporter->Destroy();

	std::cout << "successfully loaded FBX" << std::endl;

	FbxNode* rootNode = scene->GetRootNode();
	Mesh* meshes = new Mesh[rootNode->GetChildCount()];
	fbxData.meshes = meshes;
	fbxData.meshCount = rootNode->GetChildCount();
	if (rootNode) {
		for (int i = 0; i < rootNode->GetChildCount(); i++) {
			FbxNode* childNode = rootNode->GetChild(i);
			Mesh mesh = getNodeData(childNode);
			meshes[i] = mesh;
		}
	}

	fbxManager->Destroy();

	return fbxData;

}

FbxString getAttributeTypeName(FbxNodeAttribute::EType type) {
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
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

	printf("node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n",
		nodeName,
		translation[0], translation[1], translation[2],
		rotation[0], rotation[1], rotation[2],
		scaling[0], scaling[1], scaling[2]
	);

	mesh.position = glm::vec3(translation[0], translation[1], translation[2]);
	mesh.rotation = glm::vec3(rotation[0], rotation[1], rotation[2]);
	mesh.scale = glm::vec3(scaling[0], scaling[1], scaling[2]);

	for (int i = 0; i < node->GetNodeAttributeCount(); i++) {
		FbxNodeAttribute* nodeAttribute = node->GetNodeAttributeByIndex(i);
		const char* nodeAttributeName = nodeAttribute->GetName();
		FbxString nodeType = getAttributeTypeName(nodeAttribute->GetAttributeType());
		if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
			FbxMesh* fbxMesh = (FbxMesh*)nodeAttribute;
			std::cout << "node attribute " << nodeAttributeName << " is of type " << nodeType.Buffer() << " and has " << fbxMesh->GetControlPointsCount() << " vertices" << std::endl;;

			mesh.vertexCount = fbxMesh->GetControlPointsCount();
			Vertex* vertices = new Vertex[mesh.vertexCount];
			mesh.vertices = vertices;

			FbxVector4* fbxVertices = fbxMesh->GetControlPoints();
			for (int vertexId = 0; vertexId < fbxMesh->GetControlPointsCount(); vertexId++) {
				FbxDouble* data = fbxVertices[vertexId].mData;

				Vertex& vertex = vertices[vertexId];
				vertex.position[0] = data[0];
				vertex.position[1] = data[1];
				vertex.position[2] = data[2];

				vertex.color[0] = randNum();
				vertex.color[1] = randNum();
				vertex.color[2] = randNum();

				// seem to be relative to parent
				std::cout << "vertex " << vertexId << ": (" << data[0] << ", " << data[1] << ", " << data[2] << ", " << data[3] << std::endl;
			}
			int polygonCount = fbxMesh->GetPolygonCount();
			std::cout << "getPolygonVertexCount: " << fbxMesh->GetPolygonVertexCount() << std::endl;
			int numIndicies = fbxMesh->GetPolygonVertexCount();
			unsigned int* indicies = new unsigned int[numIndicies];
			mesh.indexCount = numIndicies;
			mesh.indicies = indicies;
			int i = 0;
			for (int polygonId = 0; polygonId < polygonCount; polygonId++) {
				int numVerticesOnPolygon = fbxMesh->GetPolygonSize(polygonId);
				std::cout << "polygon " << polygonId << " has " << numVerticesOnPolygon << " vertices" << std::endl;
				for (int polyVertIdx = 0; polyVertIdx < numVerticesOnPolygon; polyVertIdx++) {
					int vertexId = fbxMesh->GetPolygonVertex(polygonId, polyVertIdx);
					indicies[i] = vertexId;
					i += 1;
				}
			}
		}
	}

	std::cout << std::endl;

	return mesh;

	/*
	for (int i = 0; i < node->GetChildCount(); i++) {
		getNodeData(node->GetChild(i));
	}
	*/
}
