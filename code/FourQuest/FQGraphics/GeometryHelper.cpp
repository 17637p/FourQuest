#include "GeometryHelper.h"

namespace fq::graphics
{
	using namespace DirectX::SimpleMath;
	
	constexpr float Pi = 3.1415926535f;

	float AngleFromXY(float x, float y)
	{
		// atan2쓰면 되는 거 아닌가 이거..?
		float theta = 0.0f;

		// Quadrant I or IV
		if (x >= 0.0f)
		{
			// If x = 0, then atanf(y/x) = +pi/2 if y > 0
			//                atanf(y/x) = -pi/2 if y < 0
			theta = atanf(y / x); // in [-pi/2, +pi/2]

			if (theta < 0.0f)
			{
				theta += 2.0f * Pi; // in [0, 2*pi).
			}
		}

		// Quadrant II or III
		else
		{
			theta = atanf(y / x) + Pi; // in [0, 2*pi).
		}

		return theta;
	}

	GeometryHelper::Vertex::Vertex(const Vector3& position, const Vector3& normal, const Vector3& tangent, const Vector2& texture)
		: Position(position)
		, Normal(normal)
		, TangentU(tangent)
		, TexC(texture)
	{
	}

	GeometryHelper::Vertex::Vertex(float px, float py, float pz, float nx, float ny, float nz, float tanx, float tany, float tanz, float tx, float ty)
		: Position(px, py, pz)
		, Normal(nx, ny, nz)
		, TangentU(tanx, tany, tanz)
		, TexC(tx, ty)
	{
	}

	void GeometryHelper::CreateBox(float width, float height, float depth, MeshData* outMeshData)
	{
		Vertex v[24];

		float w2 = 0.5f * width;
		float h2 = 0.5f * height;
		float d2 = 0.5f * depth;

		// Fill in the front face vertex data.
		v[0] = Vertex({ -w2, -h2, -d2 }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f });
		v[1] = Vertex({ -w2, +h2, -d2 }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f });
		v[2] = Vertex({ +w2, +h2, -d2 }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f });
		v[3] = Vertex({ +w2, -h2, -d2 }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f });

		// Fill in the back face vertex data.
		v[4] = Vertex({ -w2, -h2, +d2 }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f });
		v[5] = Vertex({ +w2, -h2, +d2 }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f });
		v[6] = Vertex({ +w2, +h2, +d2 }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f });
		v[7] = Vertex({ -w2, +h2, +d2 }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f });

		// Fill in the top face vertex data.
		v[8] = Vertex({ -w2, +h2, -d2 }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f });
		v[9] = Vertex({ -w2, +h2, +d2 }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f });
		v[10] = Vertex({ +w2, +h2, +d2 }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f });
		v[11] = Vertex({ +w2, +h2, -d2 }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f });

		// Fill in the bottom face vertex data.
		v[12] = Vertex({ -w2, -h2, -d2 }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f });
		v[13] = Vertex({ +w2, -h2, -d2 }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f });
		v[14] = Vertex({ +w2, -h2, +d2 }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f });
		v[15] = Vertex({ -w2, -h2, +d2 }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f });

		// Fill in the left face vertex data.
		v[16] = Vertex({ -w2, -h2, +d2 }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f });
		v[17] = Vertex({ -w2, +h2, +d2 }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f });
		v[18] = Vertex({ -w2, +h2, -d2 }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f });
		v[19] = Vertex({ -w2, -h2, -d2 }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f });

		// Fill in the right face vertex data.
		v[20] = Vertex({ +w2, -h2, -d2 }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f });
		v[21] = Vertex({ +w2, +h2, -d2 }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f });
		v[22] = Vertex({ +w2, +h2, +d2 }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f });
		v[23] = Vertex({ +w2, -h2, +d2 }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f });

		outMeshData->Vertices.assign(&v[0], &v[24]);

		UINT i[36];

		// Fill in the front face index data
		i[0] = 0; i[1] = 1; i[2] = 2;
		i[3] = 0; i[4] = 2; i[5] = 3;

		// Fill in the back face index data
		i[6] = 4; i[7] = 5; i[8] = 6;
		i[9] = 4; i[10] = 6; i[11] = 7;

		// Fill in the top face index data
		i[12] = 8; i[13] = 9; i[14] = 10;
		i[15] = 8; i[16] = 10; i[17] = 11;

		// Fill in the bottom face index data
		i[18] = 12; i[19] = 13; i[20] = 14;
		i[21] = 12; i[22] = 14; i[23] = 15;

		// Fill in the left face index data
		i[24] = 16; i[25] = 17; i[26] = 18;
		i[27] = 16; i[28] = 18; i[29] = 19;

		// Fill in the right face index data
		i[30] = 20; i[31] = 21; i[32] = 22;
		i[33] = 20; i[34] = 22; i[35] = 23;

		outMeshData->Indices.assign(&i[0], &i[36]);
	}

	void GeometryHelper::CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData* outMeshData)
	{
		outMeshData->Vertices.clear();
		outMeshData->Indices.clear();

		Vertex topVertex;
		topVertex.Position = { 0.0f, +radius, 0.0f };
		topVertex.Normal = { 0.0f, +1.0f, 0.0f };
		topVertex.TangentU = { 1.0f, 0.0f, 0.0f };
		topVertex.TexC = { 0.0f, 0.0f };

		Vertex bottomVertex;
		bottomVertex.Position = { 0.0f, -radius, 0.0f };
		bottomVertex.Normal = { 0.0f, -1.0f, 0.0f };
		bottomVertex.TangentU = { 1.0f, 0.0f, 0.0f };
		bottomVertex.TexC = { 0.0f, 1.0f };

		outMeshData->Vertices.push_back(topVertex);

		float phiStep = DirectX::XM_PI / stackCount;
		float thetaStep = 2.0f * DirectX::XM_PI / sliceCount;

		for (UINT i = 1; i <= stackCount - 1; ++i)
		{
			float phi = i * phiStep;

			for (UINT j = 0; j <= sliceCount; ++j)
			{
				float theta = j * thetaStep;

				Vertex v;

				// spherical to cartesian
				v.Position.x = radius * sinf(phi) * cosf(theta);
				v.Position.y = radius * cosf(phi);
				v.Position.z = radius * sinf(phi) * sinf(theta);

				// Partial derivative of P with respect to theta
				v.TangentU.x = -radius * sinf(phi) * sinf(theta);
				v.TangentU.y = 0.0f;
				v.TangentU.z = +radius * sinf(phi) * cosf(theta);

				v.TangentU.Normalize();

				v.Normal = v.Position;
				v.Normal.Normalize();

				v.TexC.x = theta / DirectX::XM_2PI;
				v.TexC.y = phi / DirectX::XM_PI;

				outMeshData->Vertices.push_back(v);
			}
		}

		outMeshData->Vertices.push_back(bottomVertex);

		for (UINT i = 1; i <= sliceCount; ++i)
		{
			outMeshData->Indices.push_back(0);
			outMeshData->Indices.push_back(i + 1);
			outMeshData->Indices.push_back(i);
		}

		UINT baseIndex = 1;
		UINT ringVertexCount = sliceCount + 1;
		for (UINT i = 0; i < stackCount - 2; ++i)
		{
			for (UINT j = 0; j < sliceCount; ++j)
			{
				outMeshData->Indices.push_back(baseIndex + i * ringVertexCount + j);
				outMeshData->Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
				outMeshData->Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

				outMeshData->Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				outMeshData->Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
				outMeshData->Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
			}
		}

		//
		// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
		// and connects the bottom pole to the bottom ring.
		//

		// South pole vertex was added last.
		UINT southPoleIndex = (UINT)outMeshData->Vertices.size() - 1;

		// Offset the indices to the index of the first vertex in the last ring.
		baseIndex = southPoleIndex - ringVertexCount;

		for (UINT i = 0; i < sliceCount; ++i)
		{
			outMeshData->Indices.push_back(southPoleIndex);
			outMeshData->Indices.push_back(baseIndex + i);
			outMeshData->Indices.push_back(baseIndex + i + 1);
		}
	}

	void GeometryHelper::CreateGeosphere(float radius, UINT numSubdivisions, MeshData* outMeshData)
	{
		numSubdivisions = min(5u, numSubdivisions);

		const float X = 0.525731f;
		const float Z = 0.850651f;

		DirectX::SimpleMath::Vector3 positions[12] =
		{
			{ -X, 0.0f, Z }, { X, 0.0f, Z },
			{ -X, 0.0f, -Z }, { X, 0.0f, -Z },
			{ 0.0f, Z, X }, { 0.0f, Z, -X },
			{ 0.0f, -Z, X }, { 0.0f, -Z, -X },
			{ Z, X, 0.0f }, { -Z, X, 0.0f },
			{ Z, -X, 0.0f }, { -Z , -X, 0.0f }
		};

		DWORD k[60] =
		{
			1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4,
			1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2,
			3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
			10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
		};

		outMeshData->Vertices.resize(12);
		outMeshData->Indices.resize(60);

		for (size_t i = 0; i < 12; ++i)
		{
			outMeshData->Vertices[i].Position = positions[i];
		}

		for (size_t i = 0; i < 60; ++i)
		{
			outMeshData->Indices[i] = k[i];
		}

		for (size_t i = 0; i < numSubdivisions; ++i)
		{
			subdivide(outMeshData);
		}

		for (size_t i = 0; i < outMeshData->Vertices.size(); ++i)
		{
			Vertex& currentVertex = outMeshData->Vertices[i];

			currentVertex.Normal = currentVertex.Position;
			currentVertex.Normal.Normalize();

			currentVertex.Position = currentVertex.Normal * radius;

			// 구면 좌표계 좀 공부해야할듯
			float theta = AngleFromXY(currentVertex.Position.x, currentVertex.Position.y);

			float phi = acosf(currentVertex.Position.y / radius);

			currentVertex.TexC.x = theta / DirectX::XM_2PI;
			currentVertex.TexC.y = phi / DirectX::XM_PI;

			currentVertex.TangentU.x = -radius * sinf(phi) * sinf(theta);
			currentVertex.TangentU.y = 0.0f;
			currentVertex.TangentU.z = +radius * sinf(phi) * cosf(theta);

			currentVertex.TangentU.Normalize();
		}
	}

	void GeometryHelper::CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData* outMeshData)
	{
		// stackCount : 옆면을 이루는 더미 개수 
		// sliceCount : 마개를 구성하는 조각 개수

		assert(outMeshData != nullptr);

		outMeshData->Vertices.clear();
		outMeshData->Indices.clear();

		float stackHeight = height / stackCount;
		float radiusStep = (topRadius - bottomRadius) / stackCount;
		UINT ringCount = stackCount + 1;
		float dr = bottomRadius - topRadius; // 너비 차이?

		for (UINT i = 0; i < ringCount; ++i)
		{
			float y = -0.5f * height + i * stackHeight; // 최소 높이부터 시작함
			float r = bottomRadius + i * radiusStep; // 각 고리의 반지름

			float dTheta = 2.0f * DirectX::XM_PI / sliceCount; // 각도
			for (UINT j = 0; j <= sliceCount; ++j)
			{
				Vertex vertex;

				float c = cosf(j * dTheta); // 슬라이스 개수만큼 cos과 sin값을 구한다.
				float s = sinf(j * dTheta);

				vertex.Position = { r * c, y, r * s }; // 반지름에 대하여 포지션을 설정한다.

				vertex.TexC.x = (float)j / sliceCount;
				vertex.TexC.y = 1.0f - (float)i / stackCount;

				vertex.TangentU = { -s, 0.0f, c }; // 탄젠트 방향이 왜 이쪽일까?

				DirectX::SimpleMath::Vector3 bitangent{ dr * c, -height, dr * s }; // 이건 잘 이해가 안되네

				vertex.Normal = vertex.TangentU.Cross(bitangent);

				outMeshData->Vertices.push_back(vertex);
			}
		}


		UINT ringVertexCount = sliceCount + 1;

		for (UINT i = 0; i < stackCount; ++i)
		{
			for (UINT j = 0; j < sliceCount; ++j)
			{
				outMeshData->Indices.push_back(i * ringVertexCount + j);
				outMeshData->Indices.push_back((i + 1) * ringVertexCount + j);
				outMeshData->Indices.push_back((i + 1) * ringVertexCount + j + 1);

				outMeshData->Indices.push_back(i * ringVertexCount + j); //
				outMeshData->Indices.push_back((i + 1) * ringVertexCount + j + 1);
				outMeshData->Indices.push_back(i * ringVertexCount + j + 1);
			}
		}

		buildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, outMeshData);
		buildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, outMeshData);
	}
	void GeometryHelper::CreateGrid(float width, float depth, UINT m, UINT n, MeshData* outMeshData)
	{
		/*
		o--o--o
		|  |  |
		o--o--o
		|  |  |
		o--o--o
		3x3이면 사각형 개수가 2x2
		삼각형(face) 개수는 사각형(cell) 개수 * 2
		*/
		assert(outMeshData != nullptr);

		UINT vertexCount = m * n;
		UINT faceCount = (m - 1) * (n - 1) * 2;

		float halfWidth = 0.5f * width;
		float halfDepth = 0.5f * depth;

		float dx = width / (n - 1);
		float dz = depth / (m - 1);

		float du = 1.0f / (n - 1);
		float dv = 1.0f / (m - 1);

		outMeshData->Vertices.resize(vertexCount);

		for (UINT i = 0; i < m; ++i)
		{
			float z = halfDepth - i * dz;

			for (UINT j = 0; j < n; ++j)
			{
				float x = -halfWidth + j * dx;
				Vertex& currentVertex = outMeshData->Vertices[i * n + j];

				currentVertex.Position = { x, 0.0f, z };

				currentVertex.Normal = { 0.0f, 1.0f, 0.0f };
				currentVertex.TangentU = { 1.0f, 0.0f, 0.0f };

				currentVertex.TexC.x = j * du;
				currentVertex.TexC.y = i * dv;
			}
		}

		outMeshData->Indices.resize(faceCount * 3);
		UINT k = 0;

		for (UINT i = 0; i < m - 1; ++i)
		{
			for (UINT j = 0; j < n - 1; ++j)
			{
				outMeshData->Indices[k++] = i * n + j;
				outMeshData->Indices[k++] = i * n + j + 1;
				outMeshData->Indices[k++] = (i + 1) * n + j;
				outMeshData->Indices[k++] = (i + 1) * n + j;
				outMeshData->Indices[k++] = i * n + j + 1;
				outMeshData->Indices[k++] = (i + 1) * n + j + 1;
			}
		}

		// 인덱스 설정
	}

	void GeometryHelper::CreateFullscreenQuad(MeshData* outMeshData)
	{
		outMeshData->Vertices.resize(4);
		outMeshData->Indices.resize(6);

		outMeshData->Vertices[0] = Vertex(
			-1.0f, -1.0f, 0.0f,
			0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f);

		outMeshData->Vertices[1] = Vertex(
			-1.0f, +1.0f, 0.0f,
			0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 0.0f);
		
		outMeshData->Vertices[2] = Vertex(
			+1.0f, +1.0f, 0.0f,
			0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f);

		outMeshData->Vertices[3] = Vertex(
			+1.0f, -1.0f, 0.0f,
			0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 1.0f);

		outMeshData->Indices[0] = 0;
		outMeshData->Indices[1] = 1;
		outMeshData->Indices[2] = 2;

		outMeshData->Indices[3] = 0;
		outMeshData->Indices[4] = 2;
		outMeshData->Indices[5] = 3;
	}

	void GeometryHelper::subdivide(MeshData* meshData)
	{
		MeshData inputCopy = *meshData;

		meshData->Vertices.resize(0);
		meshData->Indices.resize(0);

		//       v1
		//       *
		//      / \
				//     /   \
				//  m0*-----*m1
				//   / \   / \
				//  /   \ /   \
				// *-----*-----*
				// v0    m2     v2

		UINT numTris = inputCopy.Indices.size() / 3;
		for (UINT i = 0; i < numTris; ++i)
		{
			Vertex v0 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 0]];
			Vertex v1 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 1]];
			Vertex v2 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 2]];

			//
			// Generate the midpoints.
			//

			Vertex m0, m1, m2;

			// For subdivision, we just care about the position component.  We derive the other
			// vertex components in CreateGeosphere.

			m0.Position = {
				0.5f * (v0.Position.x + v1.Position.x),
				0.5f * (v0.Position.y + v1.Position.y),
				0.5f * (v0.Position.z + v1.Position.z)
			};

			m1.Position = {
				0.5f * (v1.Position.x + v2.Position.x),
				0.5f * (v1.Position.y + v2.Position.y),
				0.5f * (v1.Position.z + v2.Position.z)
			};

			m2.Position = {
				0.5f * (v0.Position.x + v2.Position.x),
				0.5f * (v0.Position.y + v2.Position.y),
				0.5f * (v0.Position.z + v2.Position.z)
			};

			//
			// Add new geometry.
			//

			meshData->Vertices.push_back(v0); // 0
			meshData->Vertices.push_back(v1); // 1
			meshData->Vertices.push_back(v2); // 2
			meshData->Vertices.push_back(m0); // 3
			meshData->Vertices.push_back(m1); // 4
			meshData->Vertices.push_back(m2); // 5

			meshData->Indices.push_back(i * 6 + 0);
			meshData->Indices.push_back(i * 6 + 3);
			meshData->Indices.push_back(i * 6 + 5);

			meshData->Indices.push_back(i * 6 + 3);
			meshData->Indices.push_back(i * 6 + 4);
			meshData->Indices.push_back(i * 6 + 5);

			meshData->Indices.push_back(i * 6 + 5);
			meshData->Indices.push_back(i * 6 + 4);
			meshData->Indices.push_back(i * 6 + 2);

			meshData->Indices.push_back(i * 6 + 3);
			meshData->Indices.push_back(i * 6 + 1);
			meshData->Indices.push_back(i * 6 + 4);
		}
	}

	void GeometryHelper::buildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData* outMeshData)
	{
		assert(outMeshData != nullptr);

		UINT baseIndex = (UINT)outMeshData->Vertices.size();

		float y = 0.5f * height;
		float dTheta = 2.0f * DirectX::XM_PI / sliceCount;

		for (UINT i = 0; i <= sliceCount; ++i)
		{
			float x = topRadius * cosf(i * dTheta);
			float z = topRadius * sinf(i * dTheta);

			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			Vertex vertex;
			vertex.Position = { x, y, z };
			vertex.Normal = { 0.0f, 1.0f, 0.0f };
			vertex.TangentU = { 1.0f, 0.0f, 0.0f };
			vertex.TexC = { u, v };
			outMeshData->Vertices.push_back(vertex);
		}

		Vertex center;
		center.Position = { 0.0f, y, 0.0f };
		center.Normal = { 0.0f, 1.0f, 0.0f };
		center.TangentU = { 1.0f, 0.0f, 0.0f };
		center.TexC = { 0.5f, 0.5f };
		outMeshData->Vertices.push_back(center);

		UINT centerIndex = (UINT)outMeshData->Vertices.size() - 1;

		for (UINT i = 0; i < sliceCount; ++i)
		{
			outMeshData->Indices.push_back(centerIndex);
			outMeshData->Indices.push_back(baseIndex + i + 1);
			outMeshData->Indices.push_back(baseIndex + i);
		}
	}

	void GeometryHelper::buildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData* outMeshData)
	{
		assert(outMeshData != nullptr);

		UINT baseIndex = (UINT)outMeshData->Vertices.size();

		float y = -0.5f * height;
		float dTheta = 2.0f * DirectX::XM_PI / sliceCount;

		for (UINT i = 0; i <= sliceCount; ++i)
		{
			float x = bottomRadius * cosf(i * dTheta);
			float z = bottomRadius * sinf(i * dTheta);

			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			Vertex vertex;
			vertex.Position = { x, y, z };
			vertex.Normal = { 0.0f, -1.0f, 0.0f };
			vertex.TangentU = { 1.0f, 0.0f, 0.0f };
			vertex.TexC = { u, v };
			outMeshData->Vertices.push_back(vertex);
		}

		Vertex center;
		center.Position = { 0.0f, y, 0.0f };
		center.Normal = { 0.0f, -1.0f, 0.0f };
		center.TangentU = { 1.0f, 0.0f, 0.0f };
		center.TexC = { 0.5f, 0.5f };
		outMeshData->Vertices.push_back(center);

		UINT centerIndex = (UINT)outMeshData->Vertices.size() - 1;

		for (UINT i = 0; i < sliceCount; ++i)
		{
			outMeshData->Indices.push_back(centerIndex);
			outMeshData->Indices.push_back(baseIndex + i);
			outMeshData->Indices.push_back(baseIndex + i + 1);
		}
	}
}