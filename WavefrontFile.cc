#ifndef WAVEFRONTFILE_H
#define WAVEFRONTFILE_H

#pragma once

#include <math.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "GraphicElement.cc"
#include "WavefrontParser.cc"

class WavefrontFile {
 public:
  WavefrontFile(const std::string& filename) {
    this->filename = filename;
    LoadFile(filename);
  }

  virtual ~WavefrontFile() { LoadedMeshes.clear(); }

  // Loaded Mesh Objects
  std::vector<Mesh> LoadedMeshes;
  // Loaded Vertex Objects
  std::vector<Vertex> LoadedVertices;
  // Loaded Index Positions
  std::vector<unsigned int> LoadedIndices;
  // Loaded Material Objects
  std::vector<Material> LoadedMaterials;

  bool saveGraphicElement(const GraphicElement* ge) {}
  bool reloadFile() { return LoadFile(filename); }

 private:
  // File name
  std::string filename;

  // Load a file into the loader
  //
  // If file is loaded return true
  //
  // If the file is unable to be found
  // or unable to be loaded return false
  bool LoadFile(const std::string& Path) {
    // If the file is not an .obj file return false
    if (Path.substr(Path.size() - 4, 4) != ".obj") return false;

    std::ifstream file(Path);

    if (!file.is_open()) return false;

    LoadedMeshes.clear();
    LoadedVertices.clear();
    LoadedIndices.clear();

    std::vector<Vector3> Positions;
    std::vector<Vector2> TCoords;
    std::vector<Vector3> Normals;

    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    std::vector<std::string> MeshMatNames;

    bool listening = false;
    std::string meshname;

    Mesh tempMesh;

    std::string curline;
    while (std::getline(file, curline)) {
      // Generate a Mesh Object or Prepare for an object to be created
      if (ObjAlgorithm::firstToken(curline) == "o" ||
          ObjAlgorithm::firstToken(curline) == "g" || curline[0] == 'g') {
        if (!listening) {
          listening = true;

          if (ObjAlgorithm::firstToken(curline) == "o" ||
              ObjAlgorithm::firstToken(curline) == "g") {
            meshname = ObjAlgorithm::tail(curline);
          } else {
            meshname = "unnamed";
          }
        } else {
          // Generate the Mesh to put into the array

          if (!Indices.empty() && !Vertices.empty()) {
            // Create Mesh
            tempMesh = Mesh(Vertices, Indices);
            tempMesh.MeshName = meshname;

            // Insert Mesh
            LoadedMeshes.push_back(tempMesh);

            // Cleanup
            Vertices.clear();
            Indices.clear();
            meshname.clear();

            meshname = ObjAlgorithm::tail(curline);
          } else {
            if (ObjAlgorithm::firstToken(curline) == "o" ||
                ObjAlgorithm::firstToken(curline) == "g") {
              meshname = ObjAlgorithm::tail(curline);
            } else {
              meshname = "unnamed";
            }
          }
        }
      }
      // Generate a Vertex Position
      if (ObjAlgorithm::firstToken(curline) == "v") {
        std::vector<std::string> spos;
        Vector3 vpos;
        ObjAlgorithm::split(ObjAlgorithm::tail(curline), spos, " ");

        vpos.X = std::stof(spos[0]);
        vpos.Y = std::stof(spos[1]);
        vpos.Z = std::stof(spos[2]);

        Positions.push_back(vpos);
      }
      // Generate a Vertex Texture Coordinate
      if (ObjAlgorithm::firstToken(curline) == "vt") {
        std::vector<std::string> stex;
        Vector2 vtex;
        ObjAlgorithm::split(ObjAlgorithm::tail(curline), stex, " ");

        vtex.X = std::stof(stex[0]);
        vtex.Y = std::stof(stex[1]);

        TCoords.push_back(vtex);
      }
      // Generate a Vertex Normal;
      if (ObjAlgorithm::firstToken(curline) == "vn") {
        std::vector<std::string> snor;
        Vector3 vnor;
        ObjAlgorithm::split(ObjAlgorithm::tail(curline), snor, " ");

        vnor.X = std::stof(snor[0]);
        vnor.Y = std::stof(snor[1]);
        vnor.Z = std::stof(snor[2]);

        Normals.push_back(vnor);
      }
      // Generate a Face (vertices & indices)
      if (ObjAlgorithm::firstToken(curline) == "f") {
        // Generate the vertices
        std::vector<Vertex> vVerts;
        GenVerticesFromRawOBJ(vVerts, Positions, TCoords, Normals, curline);

        // Add Vertices
        for (int i = 0; i < int(vVerts.size()); i++) {
          Vertices.push_back(vVerts[i]);

          LoadedVertices.push_back(vVerts[i]);
        }

        std::vector<unsigned int> iIndices;

        VertexTriangluation(iIndices, vVerts);

        // Add Indices
        for (int i = 0; i < int(iIndices.size()); i++) {
          unsigned int indnum =
              (unsigned int)((Vertices.size()) - vVerts.size()) + iIndices[i];
          Indices.push_back(indnum);

          indnum = (unsigned int)((LoadedVertices.size()) - vVerts.size()) +
                   iIndices[i];
          LoadedIndices.push_back(indnum);
        }
      }
      // Get Mesh Material Name
      if (ObjAlgorithm::firstToken(curline) == "usemtl") {
        MeshMatNames.push_back(ObjAlgorithm::tail(curline));

        // Create new Mesh, if Material changes within a group
        if (!Indices.empty() && !Vertices.empty()) {
          // Create Mesh
          tempMesh = Mesh(Vertices, Indices);
          tempMesh.MeshName = meshname;
          int i = 2;
          while (1) {
            tempMesh.MeshName = meshname + "_" + std::to_string(i);

            for (auto& m : LoadedMeshes)
              if (m.MeshName == tempMesh.MeshName) continue;
            break;
          }

          // Insert Mesh
          LoadedMeshes.push_back(tempMesh);

          // Cleanup
          Vertices.clear();
          Indices.clear();
        }
      }
      // Load Materials
      if (ObjAlgorithm::firstToken(curline) == "mtllib") {
        // Generate LoadedMaterial

        // Generate a path to the Material file
        std::vector<std::string> temp;
        ObjAlgorithm::split(Path, temp, "/");

        std::string pathtomat = "";

        if (temp.size() != 1) {
          for (int i = 0; i < temp.size() - 1; i++) {
            pathtomat += temp[i] + "/";
          }
        }
        pathtomat += ObjAlgorithm::tail(curline);
        // Load Materials
        LoadMaterials(pathtomat);
      }
    }

    // Deal with last Mesh
    if (!Indices.empty() && !Vertices.empty()) {
      // Create Mesh
      tempMesh = Mesh(Vertices, Indices);
      tempMesh.MeshName = meshname;

      // Insert Mesh
      LoadedMeshes.push_back(tempMesh);
    }

    file.close();

    // Set Materials for each Mesh
    for (int i = 0; i < MeshMatNames.size(); i++) {
      std::string matname = MeshMatNames[i];

      // Find corresponding Material name in loaded materials
      // when found copy Material variables into Mesh Material
      for (int j = 0; j < LoadedMaterials.size(); j++) {
        if (LoadedMaterials[j].name == matname) {
          LoadedMeshes[i].MeshMaterial = LoadedMaterials[j];
          break;
        }
      }
    }

    if (LoadedMeshes.empty() && LoadedVertices.empty() &&
        LoadedIndices.empty()) {
      return false;
    } else {
      return true;
    }
  }

 private:
  // Generate vertices from a list of positions,
  //	tcoords, normals and a face line
  void GenVerticesFromRawOBJ(std::vector<Vertex>& oVerts,
                             const std::vector<Vector3>& iPositions,
                             const std::vector<Vector2>& iTCoords,
                             const std::vector<Vector3>& iNormals,
                             std::string icurline) {
    std::vector<std::string> sface, svert;
    Vertex vVert;
    ObjAlgorithm::split(ObjAlgorithm::tail(icurline), sface, " ");

    bool noNormal = false;

    // For every given Vertex do this
    for (int i = 0; i < int(sface.size()); i++) {
      // See What type the Vertex is.
      int vtype;

      ObjAlgorithm::split(sface[i], svert, "/");

      // Check for just position - v1
      if (svert.size() == 1) {
        // Only position
        vtype = 1;
      }

      // Check for position & texture - v1/vt1
      if (svert.size() == 2) {
        // Position & Texture
        vtype = 2;
      }

      // Check for Position, Texture and Normal - v1/vt1/vn1
      // or if Position and Normal - v1//vn1
      if (svert.size() == 3) {
        if (svert[1] != "") {
          // Position, Texture, and Normal
          vtype = 4;
        } else {
          // Position & Normal
          vtype = 3;
        }
      }

      // Calculate and store the Vertex
      switch (vtype) {
        case 1:  // P
        {
          vVert.Position = ObjAlgorithm::getElement(iPositions, svert[0]);
          vVert.TextureCoordinate = Vector2(0, 0);
          noNormal = true;
          oVerts.push_back(vVert);
          break;
        }
        case 2:  // P/T
        {
          vVert.Position = ObjAlgorithm::getElement(iPositions, svert[0]);
          vVert.TextureCoordinate =
              ObjAlgorithm::getElement(iTCoords, svert[1]);
          noNormal = true;
          oVerts.push_back(vVert);
          break;
        }
        case 3:  // P//N
        {
          vVert.Position = ObjAlgorithm::getElement(iPositions, svert[0]);
          vVert.TextureCoordinate = Vector2(0, 0);
          vVert.Normal = ObjAlgorithm::getElement(iNormals, svert[2]);
          oVerts.push_back(vVert);
          break;
        }
        case 4:  // P/T/N
        {
          vVert.Position = ObjAlgorithm::getElement(iPositions, svert[0]);
          vVert.TextureCoordinate =
              ObjAlgorithm::getElement(iTCoords, svert[1]);
          vVert.Normal = ObjAlgorithm::getElement(iNormals, svert[2]);
          oVerts.push_back(vVert);
          break;
        }
        default: { break; }
      }
    }

    // take care of missing normals
    // these may not be truly acurate but it is the
    // best they get for not compiling a Mesh with normals
    if (noNormal) {
      Vector3 A = oVerts[0].Position - oVerts[1].Position;
      Vector3 B = oVerts[2].Position - oVerts[1].Position;

      Vector3 normal = ObjMath::CrossV3(A, B);

      for (int i = 0; i < int(oVerts.size()); i++) {
        oVerts[i].Normal = normal;
      }
    }
  }

  // Triangulate a list of vertices into a face by printing
  //	inducies corresponding with triangles within it
  void VertexTriangluation(std::vector<unsigned int>& oIndices,
                           const std::vector<Vertex>& iVerts) {
    // If there are 2 or less verts,
    // no triangle can be created,
    // so exit
    if (iVerts.size() < 3) {
      return;
    }
    // If it is a triangle no need to calculate it
    if (iVerts.size() == 3) {
      oIndices.push_back(0);
      oIndices.push_back(1);
      oIndices.push_back(2);
      return;
    }

    // Create a list of vertices
    std::vector<Vertex> tVerts = iVerts;

    while (true) {
      // For every Vertex
      for (int i = 0; i < int(tVerts.size()); i++) {
        // pPrev = the previous Vertex in the list
        Vertex pPrev;
        if (i == 0) {
          pPrev = tVerts[tVerts.size() - 1];
        } else {
          pPrev = tVerts[i - 1];
        }

        // pCur = the current Vertex;
        Vertex pCur = tVerts[i];

        // pNext = the next Vertex in the list
        Vertex pNext;
        if (i == tVerts.size() - 1) {
          pNext = tVerts[0];
        } else {
          pNext = tVerts[i + 1];
        }

        // Check to see if there are only 3 verts left
        // if so this is the last triangle
        if (tVerts.size() == 3) {
          // Create a triangle from pCur, pPrev, pNext
          for (int j = 0; j < int(tVerts.size()); j++) {
            if (iVerts[j].Position == pCur.Position) oIndices.push_back(j);
            if (iVerts[j].Position == pPrev.Position) oIndices.push_back(j);
            if (iVerts[j].Position == pNext.Position) oIndices.push_back(j);
          }

          tVerts.clear();
          break;
        }
        if (tVerts.size() == 4) {
          // Create a triangle from pCur, pPrev, pNext
          for (int j = 0; j < int(iVerts.size()); j++) {
            if (iVerts[j].Position == pCur.Position) oIndices.push_back(j);
            if (iVerts[j].Position == pPrev.Position) oIndices.push_back(j);
            if (iVerts[j].Position == pNext.Position) oIndices.push_back(j);
          }

          Vector3 tempVec;
          for (int j = 0; j < int(tVerts.size()); j++) {
            if (tVerts[j].Position != pCur.Position &&
                tVerts[j].Position != pPrev.Position &&
                tVerts[j].Position != pNext.Position) {
              tempVec = tVerts[j].Position;
              break;
            }
          }

          // Create a triangle from pCur, pPrev, pNext
          for (int j = 0; j < int(iVerts.size()); j++) {
            if (iVerts[j].Position == pPrev.Position) oIndices.push_back(j);
            if (iVerts[j].Position == pNext.Position) oIndices.push_back(j);
            if (iVerts[j].Position == tempVec) oIndices.push_back(j);
          }

          tVerts.clear();
          break;
        }

        // If Vertex is not an interior Vertex
        double angle = ObjMath::AngleBetweenV3(pPrev.Position - pCur.Position,
                                               pNext.Position - pCur.Position) *
                       (180 / 3.14159265359);
        if (angle <= 0 && angle >= 180) continue;

        // If any vertices are within this triangle
        bool inTri = false;
        for (int j = 0; j < int(iVerts.size()); j++) {
          if (ObjAlgorithm::inTriangle(iVerts[j].Position, pPrev.Position,
                                       pCur.Position, pNext.Position) &&
              iVerts[j].Position != pPrev.Position &&
              iVerts[j].Position != pCur.Position &&
              iVerts[j].Position != pNext.Position) {
            inTri = true;
            break;
          }
        }
        if (inTri) continue;

        // Create a triangle from pCur, pPrev, pNext
        for (int j = 0; j < int(iVerts.size()); j++) {
          if (iVerts[j].Position == pCur.Position) oIndices.push_back(j);
          if (iVerts[j].Position == pPrev.Position) oIndices.push_back(j);
          if (iVerts[j].Position == pNext.Position) oIndices.push_back(j);
        }

        // Delete pCur from the list
        for (int j = 0; j < int(tVerts.size()); j++) {
          if (tVerts[j].Position == pCur.Position) {
            tVerts.erase(tVerts.begin() + j);
            break;
          }
        }

        // reset i to the start
        // -1 since loop will add 1 to it
        i = -1;
      }

      // if no triangles were created
      if (oIndices.size() == 0) break;

      // if no more vertices
      if (tVerts.size() == 0) break;
    }
  }

  // Load Materials from .mtl file
  bool LoadMaterials(std::string path) {
    // If the file is not a Material file return false
    if (path.substr(path.size() - 4, path.size()) != ".mtl") return false;

    std::ifstream file(path);

    // If the file is not found return false
    if (!file.is_open()) return false;

    Material tempMaterial;

    bool listening = false;

    // Go through each line looking for Material variables
    std::string curline;
    while (std::getline(file, curline)) {
      // new Material and Material name
      if (ObjAlgorithm::firstToken(curline) == "newmtl") {
        if (!listening) {
          listening = true;

          if (curline.size() > 7) {
            tempMaterial.name = ObjAlgorithm::tail(curline);
          } else {
            tempMaterial.name = "none";
          }
        } else {
          // Generate the Material

          // Push Back loaded Material
          LoadedMaterials.push_back(tempMaterial);

          // Clear Loaded Material
          tempMaterial = Material();

          if (curline.size() > 7) {
            tempMaterial.name = ObjAlgorithm::tail(curline);
          } else {
            tempMaterial.name = "none";
          }
        }
      }
      // Ambient Color
      if (ObjAlgorithm::firstToken(curline) == "Ka") {
        std::vector<std::string> temp;
        ObjAlgorithm::split(ObjAlgorithm::tail(curline), temp, " ");

        if (temp.size() != 3) continue;

        tempMaterial.Ka.X = std::stof(temp[0]);
        tempMaterial.Ka.Y = std::stof(temp[1]);
        tempMaterial.Ka.Z = std::stof(temp[2]);
      }
      // Diffuse Color
      if (ObjAlgorithm::firstToken(curline) == "Kd") {
        std::vector<std::string> temp;
        ObjAlgorithm::split(ObjAlgorithm::tail(curline), temp, " ");

        if (temp.size() != 3) continue;

        tempMaterial.Kd.X = std::stof(temp[0]);
        tempMaterial.Kd.Y = std::stof(temp[1]);
        tempMaterial.Kd.Z = std::stof(temp[2]);
      }
      // Specular Color
      if (ObjAlgorithm::firstToken(curline) == "Ks") {
        std::vector<std::string> temp;
        ObjAlgorithm::split(ObjAlgorithm::tail(curline), temp, " ");

        if (temp.size() != 3) continue;

        tempMaterial.Ks.X = std::stof(temp[0]);
        tempMaterial.Ks.Y = std::stof(temp[1]);
        tempMaterial.Ks.Z = std::stof(temp[2]);
      }
      // Specular Exponent
      if (ObjAlgorithm::firstToken(curline) == "Ns") {
        tempMaterial.Ns = std::stof(ObjAlgorithm::tail(curline));
      }
      // Optical Density
      if (ObjAlgorithm::firstToken(curline) == "Ni") {
        tempMaterial.Ni = std::stof(ObjAlgorithm::tail(curline));
      }
      // Dissolve
      if (ObjAlgorithm::firstToken(curline) == "d") {
        tempMaterial.d = std::stof(ObjAlgorithm::tail(curline));
      }
      // Illumination
      if (ObjAlgorithm::firstToken(curline) == "illum") {
        tempMaterial.illum = std::stoi(ObjAlgorithm::tail(curline));
      }
      // Ambient Texture Map
      if (ObjAlgorithm::firstToken(curline) == "map_Ka") {
        tempMaterial.map_Ka = ObjAlgorithm::tail(curline);
      }
      // Diffuse Texture Map
      if (ObjAlgorithm::firstToken(curline) == "map_Kd") {
        tempMaterial.map_Kd = ObjAlgorithm::tail(curline);
      }
      // Specular Texture Map
      if (ObjAlgorithm::firstToken(curline) == "map_Ks") {
        tempMaterial.map_Ks = ObjAlgorithm::tail(curline);
      }
      // Specular Hightlight Map
      if (ObjAlgorithm::firstToken(curline) == "map_Ns") {
        tempMaterial.map_Ns = ObjAlgorithm::tail(curline);
      }
      // Alpha Texture Map
      if (ObjAlgorithm::firstToken(curline) == "map_d") {
        tempMaterial.map_d = ObjAlgorithm::tail(curline);
      }
      // Bump Map
      if (ObjAlgorithm::firstToken(curline) == "map_Bump" ||
          ObjAlgorithm::firstToken(curline) == "map_bump" ||
          ObjAlgorithm::firstToken(curline) == "bump") {
        tempMaterial.map_bump = ObjAlgorithm::tail(curline);
      }
    }

    // Deal with last Material

    // Push Back loaded Material
    LoadedMaterials.push_back(tempMaterial);

    // Test to see if anything was loaded
    // If not return false
    if (LoadedMaterials.empty()) return false;
    // If so return true
    else
      return true;
  }
};

#endif  // !WAVEFRONTFILE_H
