#ifndef MESH_UTILS_H
#define MESH_UTILS_H

#include <Utils/OBJLoader.h>
#include <Utils/PLYLoader.h>
#include <Utils/IndexedFaceMesh.h>
#include <Simulation/ParticleData.h>

using namespace Utilities;
using namespace PBD;

namespace MESH {

inline void loadOBJ(const std::string& filename, VertexData& vd, IndexedFaceMesh& mesh, 
    const Vector3r& translation, const Matrix3r& rotation, const Vector3r& scale)
{
    std::vector<std::array<float, 3>> x;
    std::vector<OBJLoader::Vec3f> normals;
    std::vector<OBJLoader::Vec2f> texCoords;
    std::vector<MeshFaceIndices> faces;
    OBJLoader::Vec3f s = { (float)scale[0], (float)scale[1], (float)scale[2] };
    OBJLoader::loadObj(filename, &x, &faces, &normals, &texCoords, s);

    mesh.release();
    const unsigned int nPoints = (unsigned int)x.size();
    const unsigned int nFaces = (unsigned int)faces.size();
    const unsigned int nTexCoords = (unsigned int)texCoords.size();
    mesh.initMesh(nPoints, nFaces * 2, nFaces);

    vd.reserve(nPoints);
    for (unsigned int i = 0; i < nPoints; i++)
    {
        vd.addVertex(Vector3r(x[i][0], x[i][1], x[i][2]));
    }
    for (unsigned int i = 0; i < nTexCoords; i++)
    {
        mesh.addUV(texCoords[i][0], texCoords[i][1]);
    }
    for (unsigned int i = 0; i < nFaces; i++)
    {
        int posIndices[3];
        int texIndices[3];
        for (int j = 0; j < 3; j++)
        {
            posIndices[j] = faces[i].posIndices[j];
            if (nTexCoords > 0)
            {
                texIndices[j] = faces[i].texIndices[j];
                mesh.addUVIndex(texIndices[j]);
            }
        }

        mesh.addFace(&posIndices[0]);
    }
    mesh.buildNeighbors();

    mesh.updateNormals(vd, 0);
    mesh.updateVertexNormals(vd);
};

inline void loadPLY(const std::string& filename, VertexData& vd, IndexedFaceMesh& mesh, 
    const Vector3r& translation, const Matrix3r& rotation, const Vector3r& scale)
{
    std::vector<std::array<int, 3>> faces;
    OBJLoader::Vec3f s = { (float)scale[0], (float)scale[1], (float)scale[2] };
    std::vector<std::array<float, 3>> x;
    PLYLoader::loadPly(filename, x, faces, s);

    mesh.release();
    const unsigned int nPoints = (unsigned int)x.size();
    const unsigned int nFaces = (unsigned int)faces.size();
    mesh.initMesh(nPoints, nFaces * 2, nFaces);
    vd.reserve(nPoints);
    for (unsigned int i = 0; i < nPoints; i++)
    {
        vd.addVertex(Vector3r(x[i][0], x[i][1], x[i][2]));
    }
    for (unsigned int i = 0; i < nFaces; i++)
    {
        int posIndices[3];
        for (int j = 0; j < 3; j++)
            posIndices[j] = faces[i][j];

        mesh.addFace(&posIndices[0]);
    }
};

template<class PositionData>
inline void drawMesh(const PositionData &pd, const IndexedFaceMesh &mesh, const unsigned int offset, const float * const color)
{
    // draw mesh 
    const unsigned int *faces = mesh.getFaces().data();
    const unsigned int nFaces = mesh.numFaces();
    const Vector3r *vertexNormals = mesh.getVertexNormals().data();

    if (MiniGL::checkOpenGLVersion(3, 3))
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_REAL, GL_FALSE, 0, &pd.getPosition(offset)[0]);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_REAL, GL_FALSE, 0, &vertexNormals[0][0]);
    }
    else
    {
        float speccolor[4] = { 1.0, 1.0, 1.0, 1.0 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, speccolor);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);
        glColor3fv(color);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glVertexPointer(3, GL_REAL, 0, &pd.getPosition(0)[0]);
        glNormalPointer(GL_REAL, 0, &vertexNormals[0][0]);
    }

    glDrawElements(GL_TRIANGLES, (GLsizei)3 * mesh.numFaces(), GL_UNSIGNED_INT, mesh.getFaces().data());

    if (MiniGL::checkOpenGLVersion(3, 3))
    {
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(2);
    }
    else
    {
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
    }
}

}

#endif