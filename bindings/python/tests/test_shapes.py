import py_fort_myrmidon as m
import numpy.testing as npt
import unittest
import datetime

class ShapeTestCase(unittest.TestCase):
    def setUp(self):
        self.circle = m.Circle(center = [0,0],radius = 1.0)
        self.capsule = m.Capsule(c1 = [0,0], c2 = [1,1],r1 = 1.0,r2 = 1.0)
        self.polygon = m.Polygon(vertices = [
            [1,1],
            [-1,1],
            [-1,-1],
            [1,-1],
        ])



    def test_type(self):
        self.assertEqual(self.circle.ShapeType,m.Shape.Type.CIRCLE)
        self.assertEqual(self.capsule.ShapeType,m.Shape.Type.CAPSULE)
        self.assertEqual(self.polygon.ShapeType,m.Shape.Type.POLYGON)


    def test_circle_fields_manipulation(self):
        npt.assert_almost_equal(self.circle.Center,[0,0])
        npt.assert_almost_equal(self.circle.Radius,1.0)
        self.circle.Center = [1,2]
        self.circle.Radius = 3.0
        npt.assert_almost_equal(self.circle.Center,[1,2])
        npt.assert_almost_equal(self.circle.Radius,3)

    def test_capsule_fields_manipulation(self):
        npt.assert_almost_equal(self.capsule.C1,[0,0])
        npt.assert_almost_equal(self.capsule.R1,1.0)
        npt.assert_almost_equal(self.capsule.C2,[1,1])
        npt.assert_almost_equal(self.capsule.R2,1.0)
        self.capsule.C1 = [-1,0]
        self.capsule.R1 = 1.0
        self.capsule.C2 = [2,3]
        self.capsule.R2 = 4.0
        npt.assert_almost_equal(self.capsule.C1,[-1,0])
        npt.assert_almost_equal(self.capsule.R1,1.0)
        npt.assert_almost_equal(self.capsule.C2,[2,3])
        npt.assert_almost_equal(self.capsule.R2,4.0)

    def test_polygon_fields_manipulation(self):
        self.assertEqual(self.polygon.Size(),4)
        npt.assert_almost_equal(self.polygon.Vertex(0),[1,1])
        npt.assert_almost_equal(self.polygon.Vertex(1),[-1,1])
        npt.assert_almost_equal(self.polygon.Vertex(2),[-1,-1])
        npt.assert_almost_equal(self.polygon.Vertex(3),[1,-1])
        with self.assertRaises(IndexError):
            self.polygon.Vertex(4)
        with self.assertRaises(IndexError):
            self.polygon.SetVertex(4,[0,0])

        self.polygon.SetVertex(1,[0,0])
        npt.assert_almost_equal(self.polygon.Vertex(1),[0,0])
