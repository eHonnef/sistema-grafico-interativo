#ifndef MATRIX_H
#define MATRIX_H

#include "Point.cc"

class Matrix {
 public:
  Matrix(int rows, int cols) : rows_(rows), cols_(cols) {
    allocSpace();
    for (int i = 0; i < rows_; ++i) {
      for (int j = 0; j < cols_; ++j) {
        p[i][j] = 0;
      }
    }
  }

  Matrix() : rows_(1), cols_(1) {
    allocSpace();
    p[0][0] = 0;
  }

  ~Matrix() {
    for (int i = 0; i < rows_; ++i) {
      delete[] p[i];
    }
    delete[] p;
  }

  Matrix(const Matrix& m) : rows_(m.rows_), cols_(m.cols_) {
    allocSpace();
    for (int i = 0; i < rows_; ++i) {
      for (int j = 0; j < cols_; ++j) {
        p[i][j] = m.p[i][j];
      }
    }
  }

  double* operator[](int index) { return p[index]; }

  Matrix& operator=(const Matrix& m) {
    if (this == &m) {
      return *this;
    }

    if (rows_ != m.rows_ || cols_ != m.cols_) {
      for (int i = 0; i < rows_; ++i) {
        delete[] p[i];
      }
      delete[] p;

      rows_ = m.rows_;
      cols_ = m.cols_;
      allocSpace();
    }

    for (int i = 0; i < rows_; ++i) {
      for (int j = 0; j < cols_; ++j) {
        p[i][j] = m.p[i][j];
      }
    }
    return *this;
  }

  Matrix& operator+=(const Matrix& m) {
    for (int i = 0; i < rows_; ++i) {
      for (int j = 0; j < cols_; ++j) {
        p[i][j] += m.p[i][j];
      }
    }
    return *this;
  }

  Matrix& operator-=(const Matrix& m) {
    for (int i = 0; i < rows_; ++i) {
      for (int j = 0; j < cols_; ++j) {
        p[i][j] -= m.p[i][j];
      }
    }
    return *this;
  }

  Matrix& operator*=(const Matrix& m) {
    Matrix temp(rows_, m.cols_);
    for (int i = 0; i < temp.rows_; ++i) {
      for (int j = 0; j < temp.cols_; ++j) {
        for (int k = 0; k < cols_; ++k) {
          temp.p[i][j] += (p[i][k] * m.p[k][j]);
        }
      }
    }
    return (*this = temp);
  }

  Matrix& operator*=(double num) {
    for (int i = 0; i < rows_; ++i) {
      for (int j = 0; j < cols_; ++j) {
        p[i][j] *= num;
      }
    }
    return *this;
  }

  Matrix& operator/=(double num) {
    for (int i = 0; i < rows_; ++i) {
      for (int j = 0; j < cols_; ++j) {
        p[i][j] /= num;
      }
    }
    return *this;
  }

  void swapRows(int r1, int r2) {
    double* temp = p[r1];
    p[r1] = p[r2];
    p[r2] = temp;
  }

  Matrix transpose() {
    Matrix ret(cols_, rows_);
    for (int i = 0; i < rows_; ++i) {
      for (int j = 0; j < cols_; ++j) {
        ret.p[j][i] = p[i][j];
      }
    }
    return ret;
  }

  Matrix createIdentity(int size) {
    Matrix temp(size, size);
    for (int i = 0; i < temp.rows_; ++i) {
      for (int j = 0; j < temp.cols_; ++j) {
        if (i == j) {
          temp.p[i][j] = 1;
        } else {
          temp.p[i][j] = 0;
        }
      }
    }
    return temp;
  }

  Matrix operator+(const Matrix& m2) { return (*this += m2); }

  Matrix operator-(const Matrix& m2) { return (*this -= m2); }

  Matrix operator*(const Matrix& m2) { return (*this *= m2); }

  static double dotProduct(Matrix a, Matrix b) {
    double sum = 0;
    for (int i = 0; i < a.rows_; ++i) {
      sum += (a[i][0] * b[i][0]);
    }
    return sum;
  }

  static double dotProduct(Point pointA, Point pointB) {
    Matrix a, b;
    a = Matrix(3, 1);
    b = Matrix(3, 1);
    a[0][0] = pointA.x;
    a[1][0] = pointA.y;
    a[2][0] = pointA.z;
    b[0][0] = pointB.x;
    b[1][0] = pointB.y;
    b[2][0] = pointB.z;
    return Matrix::dotProduct(a, b);
  }

 private:
  void allocSpace() {
    p = new double*[rows_];
    for (int i = 0; i < rows_; ++i) {
      p[i] = new double[cols_];
    }
  }

  int rows_, cols_;
  double** p;
};

#endif  // MATRIX_H