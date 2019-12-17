#include "pch.h"

/**
 * Funcoes auxiliares
 **/
std::vector<Point>* in(std::vector<Point>* points, double x, double y) {
  points->push_back(Point(x, y));
  return points;
}

void testaReta(std::vector<Point> points, double x0, double y0, double x1,
               double y1) {
  ASSERT_DOUBLE_EQ(points[0].x, x0);
  ASSERT_DOUBLE_EQ(points[0].y, y0);
  ASSERT_DOUBLE_EQ(points[1].x, x1);
  ASSERT_DOUBLE_EQ(points[1].y, y1);
}

void testaClip_reta(ClippingAlgorithm* ca) {
  std::vector<Point> r1, r2, r3, r4, r5, r6, r7, r8, r9;

  in(&r1, -1.5, 1)->push_back(Point(-1.82, -0.2));
  in(&r2, -1.3, -0.39)->push_back(Point(-0.5, 0.5));
  in(&r3, 0.64, 0.43)->push_back(Point(1.33, 0.11));
  in(&r4, 0.39, 0.36)->push_back(Point(0.39, -0.28));
  in(&r5, -0.68, 1.12)->push_back(Point(0.44, 1.06));
  in(&r6, 0.58, 1.18)->push_back(Point(1.27, 0.59));
  in(&r7, 1.18, -1.13)->push_back(Point(-1.33, 1.33));
  in(&r8, 0.47, -1.31)->push_back(Point(0.22, -0.72));
  in(&r9, 0.97, -0.42)->push_back(Point(0.93, -0.78));

  ca->clip(&r1);
  ca->clip(&r2);
  ca->clip(&r3);
  ca->clip(&r4);
  ca->clip(&r5);
  ca->clip(&r6);
  ca->clip(&r7);
  ca->clip(&r8);
  ca->clip(&r9);

  /*
   * Totalmente fora
   */
  ASSERT_EQ(r1.size(), 0);
  ASSERT_EQ(r5.size(), 0);
  /*
   * Totalmente dentro
   */
  // ASSERT_EQ(r4.size(), 2);
  // testaReta(r4, 0.39, 0.36, 0.39, -0.28);

  ASSERT_EQ(r9.size(), 2);
  testaReta(r9, 0.97, -0.42, 0.93, -0.78);
  /*
   * Parcialmente dentro
   */
  ASSERT_EQ(r7.size(), 2);
  testaReta(r7, 1, -0.95358565737051793, -0.99329268292682915, 1);

  ASSERT_EQ(r2.size(), 2);
  testaReta(r2, -1, -0.056249999999999967, -0.5, 0.5);

  ASSERT_EQ(r3.size(), 2);
  testaReta(r3, 0.64, 0.43, 1, 0.2630434782608696);

  ASSERT_EQ(r6.size(), 2);
  testaReta(r6, 0.79050847457627116, 1, 1, 0.82086956521739129);

  ASSERT_EQ(r8.size(), 2);
  testaReta(r8, 0.33864406779661016, -1, 0.22, -0.72);
}

/**
 * Funcoes de teste
 **/

TEST(Point_Clipping, CorrectValues) {
  std::vector<Point> p0, p1, p2, p3;

  p0.push_back(Point(1.2, 3));
  p1.push_back(Point(0.5, -0.1));
  p2.push_back(Point(0.2, 0.6));
  p3.push_back(Point(0, 0));

  std::unique_ptr<ClippingAlgorithm> p_clip(new PointClip);
  p_clip->clip(&p0);
  p_clip->clip(&p1);
  p_clip->clip(&p2);
  p_clip->clip(&p3);

  ASSERT_EQ(p0.size(), 0);
  ASSERT_EQ(p1.size(), 1);
  ASSERT_EQ(p2.size(), 1);
  ASSERT_EQ(p3.size(), 1);
}

TEST(LINE_Cohen_Sutherland_Clipping, CorrectValues) {
  ClippingAlgorithm* clipA = new CohenSutherland();
  testaClip_reta(clipA);
}

TEST(LINE_Nicholl_Lee_Nicholl, CorrectValues) {
  // std::unique_ptr<ClippingAlgorithm> clipA(new LiangBarsky);
  ClippingAlgorithm* clipA = new NichollLeeNicholl();
  testaClip_reta(clipA);
}

TEST(POLYGON_Clockwise, CorrectValues) {
  std::unique_ptr<GraphicElement> ge0(
      new GraphicElement("house", GraphicType::WIREFRAME, 0, 1));
  ge0->addPoint(200, 300)
      ->addPoint(200, 350)
      ->addPoint(250, 375)
      ->addPoint(300, 350)
      ->addPoint(300, 300);

  ASSERT_TRUE(PointsOperations::isClockwiseOriented(ge0->points));
  PointsOperations::reversePoints(&ge0->points);
  ASSERT_FALSE(PointsOperations::isClockwiseOriented(ge0->points));

  std::unique_ptr<GraphicElement> ge1(
      new GraphicElement("house_inverted", GraphicType::WIREFRAME, 0, 1));
  ge1->addPoint(200, 300)
      ->addPoint(300, 300)
      ->addPoint(300, 350)
      ->addPoint(250, 375)
      ->addPoint(200, 350);

  ASSERT_FALSE(PointsOperations::isClockwiseOriented(ge1->points));
  PointsOperations::reversePoints(&ge1->points);
  ASSERT_TRUE(PointsOperations::isClockwiseOriented(ge1->points));
}

TEST(Line_Intersection, CorrectValues) {
  // l0 e l1 sao retas que se cruzam
  // l2 nao cruza nenhuma
  // @TODO: calcular o ponto em que se cruzam e verificar

  std::vector<Point> r0({Point(1, 2), Point(7, 3)});
  std::vector<Point> r1({Point(2, 4), Point(5, 1)});
  std::vector<Point> r2({Point(-1, -2), Point(-7, -3)});
}

TEST(POLYGON_WeilerAtherton_Algorithm, CorrectValues) {
  // WeilerAtherton
  std::vector<Point> points(
      {Point(0.5, 0.5), Point(0.5, 1.5), Point(1.5, 1.5), Point(1.5, 0.5)});

  // apos o clip os pontos devem ser (0.5,0.5)(0.5,1)(1,1)(1,0.5)

  ASSERT_TRUE(PointsOperations::isClockwiseOriented(points));
  SutherlandHodgman().clip(&points);
  ASSERT_EQ(points.size(), 4);
  ASSERT_DOUBLE_EQ(points.at(0).x, 0.5);
  ASSERT_DOUBLE_EQ(points.at(0).y, 0.5);

  ASSERT_DOUBLE_EQ(points.at(1).x, 0.5);
  ASSERT_DOUBLE_EQ(points.at(1).y, 1);

  ASSERT_DOUBLE_EQ(points.at(2).x, 1);
  ASSERT_DOUBLE_EQ(points.at(2).y, 1);

  ASSERT_DOUBLE_EQ(points.at(3).x, 1);
  ASSERT_DOUBLE_EQ(points.at(3).y, 0.5);
  // ASSERT_TRUE(PointsOperations::isPointsEqual(points.at(0), Point());
}

TEST(OBJ_Loader, CorrectValues) {
  WavefrontFile wff = WavefrontFile("../simple.obj");
  for (unsigned i = 0; i < wff.LoadedMeshes.size(); ++i) {
    std::cout << wff.LoadedMeshes[i].MeshName << std::endl;
    for (unsigned j = 0; j < wff.LoadedMeshes[i].Vertices.size(); ++j) {
      std::cout << std::to_string(wff.LoadedMeshes[i].Vertices[j].Position.X) << ",";
      std::cout << std::to_string(wff.LoadedMeshes[i].Vertices[j].Position.Y) << ",";
      std::cout << std::to_string(wff.LoadedMeshes[i].Vertices[j].Position.Z) << std::endl;
    }
  }
}