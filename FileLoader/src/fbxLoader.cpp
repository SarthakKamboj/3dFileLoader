#include <iostream>
#include "fbxLoader.h"
#include <fbxsdk.h>

void printNode(FbxNode* node);
FbxString getAttributeTypeName(FbxNodeAttribute::EType type);

void loadFbx() {
	const char* fbxFilePath = "C:\\Sarthak\\product_anim\\arrow\\arrow.fbx";
	FbxManager* fbxManager = FbxManager::Create();
	FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ioSettings);
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	if (!fbxImporter->Initialize(fbxFilePath, -1, fbxManager->GetIOSettings())) {
		std::cout << "fbx import failed" << std::endl;
		std::cout << "ERROR: " << fbxImporter->GetStatus().GetErrorString() << std::endl;
		return;
	}
	FbxScene* scene = FbxScene::Create(fbxManager, "scene");
	fbxImporter->Import(scene);
	fbxImporter->Destroy();

	std::cout << "successfully loaded FBX" << std::endl;

	FbxNode* rootNode = scene->GetRootNode();
	if (rootNode) {
		for (int i = 0; i < rootNode->GetChildCount(); i++) {
			FbxNode* childNode = rootNode->GetChild(i);
			printNode(childNode);
		}
	}

	fbxManager->Destroy();
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

void printNode(FbxNode* node) {
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

	for (int i = 0; i < node->GetNodeAttributeCount(); i++) {
		FbxNodeAttribute* nodeAttribute = node->GetNodeAttributeByIndex(i);
		const char* nodeAttributeName = nodeAttribute->GetName();
		FbxString nodeType = getAttributeTypeName(nodeAttribute->GetAttributeType());
		std::cout << "node attribute " << nodeAttributeName << " is of type " << nodeType.Buffer() << std::endl;;
	}

	std::cout << std::endl;

	for (int i = 0; i < node->GetChildCount(); i++) {
		printNode(node->GetChild(i));
	}
}
