#include "Engine/Core/OBJLoader.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Core/Vertex_PNCU.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Time.hpp"

void LoadOBJByFileName(const char* filename, Mat44 transformMat, std::vector<Vertex_PCUTBN>& outVertexes, std::vector<unsigned int>& outIndexes)
{

	float startTime = (float)GetCurrentTimeSeconds();
	std::string fileContent;
	FileReadToString(fileContent, filename);
	std::vector<Vec3> v_positions;
	std::vector<Vec2> v_textures;
	std::vector<Vec3> v_normals;
	int faceCounter = 0;
	int triCounter = 0;
	Strings splitByLine;
	float parseTime = (float)GetCurrentTimeSeconds();
	splitByLine = SplitStringOnDelimiter(fileContent, "\r\n");

	if ((int)splitByLine.size() <= 1) {
		splitByLine = SplitStringOnDelimiter(fileContent, "\n");
	}

	for (int lineIndex = 0; lineIndex < (int)splitByLine.size(); lineIndex++) {
		int lineLength = (int)splitByLine[lineIndex].size();
		std::string& nowLine = splitByLine[lineIndex];
		if (lineLength < 2)
			continue;
		if (nowLine[0] == 'v') {
			// texture coordinates
			if (nowLine[1] == 't') {
				Strings splitBySpace = SplitStringOnDelimiter(nowLine, " ");
				int nowSet = 0;
				Vec2 vt;
				for (int index = 1; index < (int)splitBySpace.size() && nowSet < 2; index++) {
					if (splitBySpace[index].size() > 0) {
						if (nowSet == 0)
							vt.x = static_cast<float>(atof(splitBySpace[index].c_str()));
						if (nowSet == 1)
							vt.y = static_cast<float>(atof(splitBySpace[index].c_str()));
						nowSet++;
					}
				}
				if (nowSet != 0) {
					v_textures.push_back(vt);
				}
			}
			// normals
			else if (nowLine[1] == 'n') {
				Strings splitBySpace = SplitStringOnDelimiter(nowLine, " ");
				int nowSet = 0;
				Vec3 vn;
				for (int index = 1; index < (int)splitBySpace.size() && nowSet < 3; index++) {
					if (splitBySpace[index].size() > 0) {
						if (nowSet == 0)
							vn.x = static_cast<float>(atof(splitBySpace[index].c_str()));
						if (nowSet == 1)
							vn.y = static_cast<float>(atof(splitBySpace[index].c_str()));
						if (nowSet == 2)
							vn.z = static_cast<float>(atof(splitBySpace[index].c_str()));
						nowSet++;
					}
				}
				if (nowSet != 0) {
					if (vn.GetLengthSquared() != 1.f)
						vn.Normalize();
					v_normals.push_back(vn);
				}
			}
			// positions
			else {
				Strings splitBySpace = SplitStringOnDelimiter(nowLine, " ");
				int nowSet = 0;
				Vec3 v;
				for (int index = 1; index < (int)splitBySpace.size() && nowSet < 3; index++) {
					if (splitBySpace[index].size() > 0) {
						if (nowSet == 0)
							v.x = static_cast<float>(atof(splitBySpace[index].c_str()));
						if (nowSet == 1)
							v.y = static_cast<float>(atof(splitBySpace[index].c_str()));
						if (nowSet == 2)
							v.z = static_cast<float>(atof(splitBySpace[index].c_str()));
						nowSet++;
					}
				}
				if (nowSet != 0) {
					v_positions.push_back(v);
				}
			}
		}
		
		else if (nowLine[0] == 'f') {
			faceCounter++;
			Strings splitBySpace = SplitStringOnDelimiter(nowLine, " ");
			Strings facesLine;
			// remove empty string caused by extra spaces
			for (int i = 1; i < (int)splitBySpace.size(); i++) {
				if (splitBySpace[i].size() > 0) {
					facesLine.push_back(splitBySpace[i]);
				}
			}
			// Get all vertexes of the face
			std::vector<Vertex_PCUTBN> faceVertexes;
			std::vector<unsigned int> faceIndexes;
			int inital = (int)outVertexes.size();
			for (int vertexIndex = 0; vertexIndex < (int)facesLine.size(); vertexIndex++) {
				faceIndexes.push_back(inital++);
				Strings faceInfo = SplitStringOnDelimiter(facesLine[vertexIndex], "/");
				int vIndex = -1;
				int vtIndex = -1;
				int vnIndex = -1;
				if (faceInfo[0].size() > 0)
					vIndex = static_cast<int>(atoi(faceInfo[0].c_str())) - 1;
				if (faceInfo.size() > 1 && faceInfo[1].size() > 0)
					vtIndex = static_cast<int>(atoi(faceInfo[1].c_str())) - 1;
				if (faceInfo.size() > 2 && faceInfo[2].size() > 0)
					vnIndex = static_cast<int>(atoi(faceInfo[2].c_str())) - 1;

				Vertex_PCUTBN vertex;
				vertex.m_color = Rgba8::WHITE;
				vertex.m_position = v_positions[vIndex];
				if (vtIndex != -1)
					vertex.m_uvTexCoords = v_textures[vtIndex];
				if (vnIndex != -1)
					vertex.m_normal = v_normals[vnIndex];
				faceVertexes.push_back(vertex);
			}
			// Caculate the normal if it's not given
			if (faceVertexes[0].m_normal == Vec3()) {
				Vec3 normal = CrossProduct3D((faceVertexes[1].m_position - faceVertexes[0].m_position), (faceVertexes[2].m_position - faceVertexes[1].m_position));
				normal.Normalize();
				for (int vertexIndex = 0; vertexIndex < (int)faceVertexes.size(); vertexIndex++) {
					faceVertexes[vertexIndex].m_normal = normal;
				}
			}
			for (int vertexIndex = 0; vertexIndex < (int)faceVertexes.size(); vertexIndex++) {
				outVertexes.push_back(faceVertexes[vertexIndex]);

			}
			for (int vertexIndex = 1; vertexIndex < (int)faceIndexes.size() - 1; vertexIndex++) {

				outIndexes.push_back(faceIndexes[0]);
				outIndexes.push_back(faceIndexes[vertexIndex]);
				outIndexes.push_back(faceIndexes[vertexIndex + 1]);
				triCounter++;
			}
		}
	}

	if (outVertexes.size() < v_positions.size()) {
		for (int i = 0; i + 2 < v_positions.size(); i += 3) {
			Vertex_PCUTBN v0, v1, v2;
			v0.m_position = v_positions[i];
			v1.m_position = v_positions[i + 1];
			v2.m_position = v_positions[i + 2];

			v0.m_color = Rgba8::WHITE;
			v1.m_color = Rgba8::WHITE;
			v2.m_color = Rgba8::WHITE;

			Vec3 normal = CrossProduct3D((v_positions[i + 1] - v_positions[i]), (v_positions[i + 2] - v_positions[i + 1]));
			normal.Normalize();

			v0.m_normal = normal;
			v1.m_normal = normal;
			v2.m_normal = normal;

			outVertexes.push_back(v0);
			outVertexes.push_back(v1);
			outVertexes.push_back(v2);
		}
		for (int i = 0; i < (int)outVertexes.size(); i++) {
			outIndexes.push_back(i);
		}
	}
	
	TransformVertexArray3D(outVertexes, transformMat);

	CalculateTangentSpaceBasisVectors(outVertexes, outIndexes);

	float createTime = (float)GetCurrentTimeSeconds();

	DebuggerPrintf( Stringf("\n------------------------------------------- \nLoaded .obj file %s\n [file data]	vertexes: %d  texture coordinates: %d  normals: %d  faces: %d  triangles: %d\n [loaded mesh]	vertexes: %d	indexes: %d\n [time]		parse: %.6f seconds	create: %.6f seconds \n-------------------------------------------\n\n"
		, filename
		, (int)v_positions.size(), (int)v_textures.size(), (int)v_normals.size(), faceCounter, triCounter
		, (int)outVertexes.size(), (int)outIndexes.size()
		, (parseTime - startTime), (createTime - parseTime)).c_str());
	
}
