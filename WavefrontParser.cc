#ifndef WAVEFRONTPARSER_H
#define WAVEFRONTPARSER_H

#pragma once

#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Structure: Vector2
//
// Description: A 2D Vector that Holds Positional Data
class Vector2 {
 public:
  // Default Constructor
  Vector2() {
    X = 0.0f;
    Y = 0.0f;
  }
  // Variable Set Constructor
  Vector2(double X_, double Y_) {
    X = X_;
    Y = Y_;
  }
  // Bool Equals Operator Overload
  bool operator==(const Vector2& other) const {
    return (this->X == other.X && this->Y == other.Y);
  }
  // Bool Not Equals Operator Overload
  bool operator!=(const Vector2& other) const {
    return !(this->X == other.X && this->Y == other.Y);
  }
  // Addition Operator Overload
  Vector2 operator+(const Vector2& right) const {
    return Vector2(this->X + right.X, this->Y + right.Y);
  }
  // Subtraction Operator Overload
  Vector2 operator-(const Vector2& right) const {
    return Vector2(this->X - right.X, this->Y - right.Y);
  }
  // double Multiplication Operator Overload
  Vector2 operator*(const double& other) const {
    return Vector2(this->X * other, this->Y * other);
  }

  // Positional Variables
  double X;
  double Y;
};

// Structure: Vector3
//
// Description: A 3D Vector that Holds Positional Data
class Vector3 {
 public:
  // Default Constructor
  Vector3() {
    X = 0.0f;
    Y = 0.0f;
    Z = 0.0f;
  }
  // Variable Set Constructor
  Vector3(double X_, double Y_, double Z_) {
    X = X_;
    Y = Y_;
    Z = Z_;
  }
  // Bool Equals Operator Overload
  bool operator==(const Vector3& other) const {
    return (this->X == other.X && this->Y == other.Y && this->Z == other.Z);
  }
  // Bool Not Equals Operator Overload
  bool operator!=(const Vector3& other) const {
    return !(this->X == other.X && this->Y == other.Y && this->Z == other.Z);
  }
  // Addition Operator Overload
  Vector3 operator+(const Vector3& right) const {
    return Vector3(this->X + right.X, this->Y + right.Y, this->Z + right.Z);
  }
  // Subtraction Operator Overload
  Vector3 operator-(const Vector3& right) const {
    return Vector3(this->X - right.X, this->Y - right.Y, this->Z - right.Z);
  }
  // double Multiplication Operator Overload
  Vector3 operator*(const double& other) const {
    return Vector3(this->X * other, this->Y * other, this->Z * other);
  }
  // double Division Operator Overload
  Vector3 operator/(const double& other) const {
    return Vector3(this->X / other, this->Y / other, this->Z / other);
  }

  // Positional Variables
  double X;
  double Y;
  double Z;
};

// Structure: Vertex
//
// Description: Model Vertex object that holds
//	a Position, Normal, and Texture Coordinate
class Vertex {
 public:
  // Position Vector
  Vector3 Position;

  // Normal Vector
  Vector3 Normal;

  // Texture Coordinate Vector
  Vector2 TextureCoordinate;
};

class Material {
 public:
  Material() {
    name;
    Ns = 0.0f;
    Ni = 0.0f;
    d = 0.0f;
    illum = 0;
  }

  // Material Name
  std::string name;
  // Ambient Color
  Vector3 Ka;
  // Diffuse Color
  Vector3 Kd;
  // Specular Color
  Vector3 Ks;
  // Specular Exponent
  double Ns;
  // Optical Density
  double Ni;
  // Dissolve
  double d;
  // Illumination
  int illum;
  // Ambient Texture Map
  std::string map_Ka;
  // Diffuse Texture Map
  std::string map_Kd;
  // Specular Texture Map
  std::string map_Ks;
  // Specular Hightlight Map
  std::string map_Ns;
  // Alpha Texture Map
  std::string map_d;
  // Bump Map
  std::string map_bump;
};

// Structure: Mesh
//
// Description: A Simple Mesh Object that holds
//	a name, a vertex list, and an index list
class Mesh {
 public:
  // Default Constructor
  Mesh() {}
  // Variable Set Constructor
  Mesh(std::vector<Vertex>& _Vertices, std::vector<unsigned int>& _Indices) {
    Vertices = _Vertices;
    Indices = _Indices;
  }
  // Mesh Name
  std::string MeshName;
  // Vertex List
  std::vector<Vertex> Vertices;
  // Index List
  std::vector<unsigned int> Indices;

  // Material
  Material MeshMaterial;
};

// Namespace: Math
//
// Description: The namespace that holds all of the math
//	functions need for OBJL
class ObjMath {
 public:
  // Vector3 Cross Product
  static Vector3 CrossV3(const Vector3 a, const Vector3 b) {
    return Vector3(a.Y * b.Z - a.Z * b.Y, a.Z * b.X - a.X * b.Z,
                   a.X * b.Y - a.Y * b.X);
  }

  // Vector3 Magnitude Calculation
  static double MagnitudeV3(const Vector3 in) {
    return (sqrtf(powf(in.X, 2) + powf(in.Y, 2) + powf(in.Z, 2)));
  }

  // Vector3 DotProduct
  static double DotV3(const Vector3 a, const Vector3 b) {
    return (a.X * b.X) + (a.Y * b.Y) + (a.Z * b.Z);
  }

  // Angle between 2 Vector3 Objects
  static double AngleBetweenV3(const Vector3 a, const Vector3 b) {
    double angle = DotV3(a, b);
    angle /= (MagnitudeV3(a) * MagnitudeV3(b));
    return angle = acosf(angle);
  }

  // Projection Calculation of a onto b
  static Vector3 ProjV3(const Vector3 a, const Vector3 b) {
    Vector3 bn = b / MagnitudeV3(b);
    return bn * DotV3(a, bn);
  }
};

// Namespace: Algorithm
//
// Description: The namespace that holds all of the
// Algorithms needed for OBJL
class ObjAlgorithm {
 public:
  // Vector3 Multiplication Opertor Overload
  /*
  Vector3 operator*(const double& left, const Vector3& right) {
    return Vector3(right.X * left, right.Y * left, right.Z * left);
  }
  */
  // A test to see if P1 is on the same side as P2 of a line segment ab
  static bool SameSide(Vector3 p1, Vector3 p2, Vector3 a, Vector3 b) {
    Vector3 cp1 = ObjMath::CrossV3(b - a, p1 - a);
    Vector3 cp2 = ObjMath::CrossV3(b - a, p2 - a);

    if (ObjMath::DotV3(cp1, cp2) >= 0)
      return true;
    else
      return false;
  }

  // Generate a cross produect normal for a triangle
  static Vector3 GenTriNormal(Vector3 t1, Vector3 t2, Vector3 t3) {
    Vector3 u = t2 - t1;
    Vector3 v = t3 - t1;

    Vector3 normal = ObjMath::CrossV3(u, v);

    return normal;
  }

  // Check to see if a Vector3 Point is within a 3 Vector3 Triangle
  static bool inTriangle(Vector3 point, Vector3 tri1, Vector3 tri2,
                         Vector3 tri3) {
    // Test to see if it is within an infinite prism that the triangle outlines.
    bool within_tri_prisim = SameSide(point, tri1, tri2, tri3) &&
                             SameSide(point, tri2, tri1, tri3) &&
                             SameSide(point, tri3, tri1, tri2);

    // If it isn't it will never be on the triangle
    if (!within_tri_prisim) return false;

    // Calulate Triangle's Normal
    Vector3 n = GenTriNormal(tri1, tri2, tri3);

    // Project the point onto this normal
    Vector3 proj = ObjMath::ProjV3(point, n);

    // If the distance from the triangle to the point is 0
    //	it lies on the triangle
    if (ObjMath::MagnitudeV3(proj) == 0)
      return true;
    else
      return false;
  }

  // Split a String into a string array at a given token
  static inline void split(const std::string& in, std::vector<std::string>& out,
                           std::string token) {
    out.clear();

    std::string temp;

    for (int i = 0; i < int(in.size()); i++) {
      std::string test = in.substr(i, token.size());

      if (test == token) {
        if (!temp.empty()) {
          out.push_back(temp);
          temp.clear();
          i += (int)token.size() - 1;
        } else {
          out.push_back("");
        }
      } else if (i + token.size() >= in.size()) {
        temp += in.substr(i, token.size());
        out.push_back(temp);
        break;
      } else {
        temp += in[i];
      }
    }
  }

  // Get tail of string after first token and possibly following spaces
  static inline std::string tail(const std::string& in) {
    size_t token_start = in.find_first_not_of(" \t");
    size_t space_start = in.find_first_of(" \t", token_start);
    size_t tail_start = in.find_first_not_of(" \t", space_start);
    size_t tail_end = in.find_last_not_of(" \t");
    if (tail_start != std::string::npos && tail_end != std::string::npos) {
      return in.substr(tail_start, tail_end - tail_start + 1);
    } else if (tail_start != std::string::npos) {
      return in.substr(tail_start);
    }
    return "";
  }

  // Get first token of string
  static inline std::string firstToken(const std::string& in) {
    if (!in.empty()) {
      size_t token_start = in.find_first_not_of(" \t");
      size_t token_end = in.find_first_of(" \t", token_start);
      if (token_start != std::string::npos && token_end != std::string::npos) {
        return in.substr(token_start, token_end - token_start);
      } else if (token_start != std::string::npos) {
        return in.substr(token_start);
      }
    }
    return "";
  }

  // Get element at given index position
  template <class T>
  static inline const T& getElement(const std::vector<T>& elements,
                                    std::string& index) {
    int idx = std::stoi(index);
    if (idx < 0)
      idx = int(elements.size()) + idx;
    else
      idx--;
    return elements[idx];
  }
};

#endif  // !WAVEFRONTPARSER_H