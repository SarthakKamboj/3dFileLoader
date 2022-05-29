#include <iostream>
#include "fbxLoader.h"
#include <fbxsdk.h>
#include "fbxsdk/utils/fbxgeometryconverter.h"
#include <stdlib.h>
#include "glm/glm.hpp"
#include <vector>

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

struct NormalData {
	std::vector<glm::vec3> splitNormals;
	glm::vec3 avgNormal;
	int numCoords;
};

void DisplayPolygons(FbxMesh* mesh);
void DisplayNormals(FbxMesh* pMesh);

Mesh getNodeData(FbxNode* node) {
	// DisplayNormals((FbxMesh*)node->GetNodeAttributeByIndex(0));
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

			/*
			for (int positionId = 0; positionId < numPositions; positionId++) {
				FbxVector4 normal;
				if (normalsRefMode == FbxGeometryElement::eIndexToDirect) {
					int idx = normalsElement->GetIndexArray().GetAt(positionId);
					normal = normalsElement->GetDirectArray().GetAt(idx);
				}
				else if (normalsRefMode == FbxGeometryElement::eDirect) {
					normal = normalsElement->GetDirectArray().GetAt(positionId);
				}
				FbxVector4 position = fbxLclPositions[positionId];
				std::cout << "positionId: " << positionId << std::endl;
				std::cout << "position: (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
				std::cout << "normal: (" << normal[0] << ", " << normal[1] << ", " << normal[2] << ")" << std::endl;
				std::cout << std::endl;
			}
			*/

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

							/*
							std::cout << "vertexId: " << vertexId << std::endl;
							std::cout << "position: (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
							std::cout << "normal: (" << normal[0] << ", " << normal[1] << ", " << normal[2] << ")" << std::endl;
							*/
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
			for (int i = 0; i < numPositions; i++) {
				NormalData& data = normalData[i];
				data.avgNormal = glm::vec3(0, 0, 0);

				FbxVector4 position = fbxLclPositions[i];
				std::cout << "position: (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
				std::cout << "numCoords: " << data.numCoords << std::endl;

				for (int i = 0; i < data.numCoords; i++) {
					glm::vec3 normal = data.splitNormals[i];
					std::cout << "normal " << i << " : (" << normal[0] << ", " << normal[1] << ", " << normal[2] << ")" << std::endl;
					data.avgNormal += normal;
				}

				data.avgNormal /= data.numCoords;
				// data.normal = glm::normalize(data.normal);

				glm::vec3 avgNormal = data.avgNormal;
				std::cout << "avgNormal " << i << " : (" << avgNormal[0] << ", " << avgNormal[1] << ", " << avgNormal[2] << ")" << std::endl;

				std::cout << std::endl;

			}

			for (int i = 0; i < mesh.vertexCount; i++) {
				Vertex* vertices = mesh.vertices;
				glm::vec3 avgNormalForPos = normalData[mesh.vertices[i].positionId].avgNormal;
				vertices[i].avgNormal[0] = avgNormalForPos.x;
				vertices[i].avgNormal[1] = avgNormalForPos.y;
				vertices[i].avgNormal[2] = avgNormalForPos.z;
				// mesh[i].vertices.avgNormal = normalData[mesh.vertices[i].positionId];
			}

			/*
			bool sameMatForWholeMesh = true;
			std::cout << "mesh has " << fbxMesh->GetElementMaterialCount() << " materials" << std::endl;
			for (int matIdx = 0; matIdx < fbxMesh->GetElementMaterialCount(); matIdx++) {
				FbxGeometryElementMaterial* elMaterial = fbxMesh->GetElementMaterial(matIdx);
				FbxGeometryElement::EMappingMode mappingMode = elMaterial->GetMappingMode();
				if (mappingMode == FbxGeometryElement::eByPolygon) {
					sameMatForWholeMesh = false;
				}
				int idx = elMaterial->GetIndexArray().GetAt(0);
				FbxSurfaceMaterial* material = fbxMesh->GetNode()->GetMaterial(idx);
				if (idx >= 0) {
					FbxProperty diffuseProperty = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
					int numTextures = diffuseProperty.GetSrcObjectCount<FbxTexture>();
					int numLayeredTextures = diffuseProperty.GetSrcObjectCount<FbxLayeredTexture>();

					for (int layerTexId = 0; layerTexId < numLayeredTextures; layerTexId++) {
						FbxLayeredTexture* layeredTex = diffuseProperty.GetSrcObject<FbxLayeredTexture>(layerTexId);
						int textureCount = layeredTex->GetSrcObjectCount<FbxTexture>();
					}
				}
			}
			*/

		}
	}

	return mesh;

	/*
	for (int i = 0; i < node->GetChildCount(); i++) {
		getNodeData(node->GetChild(i));
	}
	*/
}

void DisplayNormals(FbxMesh* pMesh) {
	int i, j, lPolygonCount = pMesh->GetPolygonCount();
	FbxVector4* lControlPoints = pMesh->GetControlPoints();
	char header[100];

	int vertexId = 0;
	for (int i = 0; i < lPolygonCount; i++)
	{
		int lPolygonSize = pMesh->GetPolygonSize(i);

		for (int j = 0; j < lPolygonSize; j++)
		{
			int lControlPointIndex = pMesh->GetPolygonVertex(i, j);
			FbxVector4 coords = lControlPoints[lControlPointIndex];
			std::cout << "coords: " << coords[0] << ", " << coords[1] << ", " << coords[2] << std::endl;

			for (int l = 0; l < pMesh->GetElementNormalCount(); ++l)
			{
				FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(l);
				FBXSDK_sprintf(header, 100, "            Normal: ");

				if (leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						// Display3DVector(header, leNormal->GetDirectArray().GetAt(vertexId));
						FbxVector4 normals = leNormal->GetDirectArray().GetAt(vertexId);
						std::cout << "normals: " << normals[0] << ", " << normals[1] << ", " << normals[2] << std::endl;
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leNormal->GetIndexArray().GetAt(vertexId);
						// Display3DVector(header, leNormal->GetDirectArray().GetAt(id));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}

			vertexId++;
			std::cout << std::endl;
		} // for polygonSize
	} // for polygonCount
}

void DisplayPolygons(FbxMesh* pMesh)
{
	int i, j, lPolygonCount = pMesh->GetPolygonCount();
	FbxVector4* lControlPoints = pMesh->GetControlPoints();
	char header[100];

	int vertexId = 0;
	for (i = 0; i < lPolygonCount; i++)
	{
		int l;

		int lPolygonSize = pMesh->GetPolygonSize(i);

		for (j = 0; j < lPolygonSize; j++)
		{
			int lControlPointIndex = pMesh->GetPolygonVertex(i, j);
			FbxVector4 coords = lControlPoints[lControlPointIndex];
			std::cout << "coords: " << coords[0] << ", " << coords[1] << ", " << coords[2] << std::endl;
			/*
			if (lControlPointIndex < 0)
			{
				DisplayString("            Coordinates: Invalid index found!");
				continue;
			}
			else
				Display3DVector("            Coordinates: ", lControlPoints[lControlPointIndex]);

			for (l = 0; l < pMesh->GetElementVertexColorCount(); l++)
			{
				FbxGeometryElementVertexColor* leVtxc = pMesh->GetElementVertexColor(l);
				FBXSDK_sprintf(header, 100, "            Color vertex: ");

				switch (leVtxc->GetMappingMode())
				{
				default:
					break;
				case FbxGeometryElement::eByControlPoint:
					switch (leVtxc->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						DisplayColor(header, leVtxc->GetDirectArray().GetAt(lControlPointIndex));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
						DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					switch (leVtxc->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						DisplayColor(header, leVtxc->GetDirectArray().GetAt(vertexId));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leVtxc->GetIndexArray().GetAt(vertexId);
						DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				}
			}
			*/
			for (l = 0; l < pMesh->GetElementUVCount(); ++l)
			{
				FbxGeometryElementUV* leUV = pMesh->GetElementUV(l);
				FBXSDK_sprintf(header, 100, "            Texture UV: ");

				switch (leUV->GetMappingMode())
				{
				default:
					break;
				case FbxGeometryElement::eByControlPoint:
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						// Display2DVector(header, leUV->GetDirectArray().GetAt(lControlPointIndex));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
						// Display2DVector(header, leUV->GetDirectArray().GetAt(id));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect:
					{
						FbxVector2 uvCoords = leUV->GetDirectArray().GetAt(lTextureUVIndex);
						std::cout << "uv coords: " << uvCoords[0] << ", " << uvCoords[1] << std::endl;
						// Display2DVector(header, leUV->GetDirectArray().GetAt(lTextureUVIndex));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				}
			}
			for (l = 0; l < pMesh->GetElementNormalCount(); ++l)
			{
				FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(l);
				FBXSDK_sprintf(header, 100, "            Normal: ");

				if (leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						// Display3DVector(header, leNormal->GetDirectArray().GetAt(vertexId));
						FbxVector4 normals = leNormal->GetDirectArray().GetAt(vertexId);
						std::cout << "normals: " << normals[0] << ", " << normals[1] << ", " << normals[2] << std::endl;
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leNormal->GetIndexArray().GetAt(vertexId);
						// Display3DVector(header, leNormal->GetDirectArray().GetAt(id));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}
			/*
			for (l = 0; l < pMesh->GetElementTangentCount(); ++l)
			{
				FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent(l);
				FBXSDK_sprintf(header, 100, "            Tangent: ");

				if (leTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leTangent->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						Display3DVector(header, leTangent->GetDirectArray().GetAt(vertexId));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leTangent->GetIndexArray().GetAt(vertexId);
						Display3DVector(header, leTangent->GetDirectArray().GetAt(id));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}
			for (l = 0; l < pMesh->GetElementBinormalCount(); ++l)
			{

				FbxGeometryElementBinormal* leBinormal = pMesh->GetElementBinormal(l);

				FBXSDK_sprintf(header, 100, "            Binormal: ");
				if (leBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leBinormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						Display3DVector(header, leBinormal->GetDirectArray().GetAt(vertexId));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leBinormal->GetIndexArray().GetAt(vertexId);
						Display3DVector(header, leBinormal->GetDirectArray().GetAt(id));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
			}
			*/
			vertexId++;
			std::cout << std::endl;
		} // for polygonSize
	} // for polygonCount


	//check visibility for the edges of the mesh
	/*
	for (int l = 0; l < pMesh->GetElementVisibilityCount(); ++l)
	{
		FbxGeometryElementVisibility* leVisibility = pMesh->GetElementVisibility(l);
		FBXSDK_sprintf(header, 100, "    Edge Visibility : ");
		DisplayString(header);
		switch (leVisibility->GetMappingMode())
		{
		default:
			break;
			//should be eByEdge
		case FbxGeometryElement::eByEdge:
			//should be eDirect
			for (j = 0; j != pMesh->GetMeshEdgeCount(); ++j)
			{
				DisplayInt("        Edge ", j);
				DisplayBool("              Edge visibility: ", leVisibility->GetDirectArray().GetAt(j));
			}

			break;
		}
	}
	DisplayString("");
	*/
}
