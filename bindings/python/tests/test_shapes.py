import fort_myrmidon as m
import numpy.testing as npt
import unittest


class Vector2dListTestCase(unittest.TestCase):
    def test_initialization(self):
        p = m.Vector2dList()
        self.assertEqual(len(p), 0)
        p = m.Vector2dList([[1, 1]])
        self.assertEqual(len(p), 1)
        with self.assertRaises(RuntimeError):
            p = m.Vector2dList([1, 2, 3])

    def test_is_subscribable(self):
        p = m.Vector2dList([[1, 2], [3, 4]])
        self.assertEqual(len(p), 2)
        npt.assert_almost_equal(p[0], [1, 2])
        npt.assert_almost_equal(p[1], [3, 4])
        p[0] = [0, 0]
        npt.assert_almost_equal(p[0], [0, 0])

    def test_is_extendable(self):
        p = m.Vector2dList()
        p.append([1, 2])
        p.extend([[3, 4], [5, 6]])
        self.assertEqual(len(p), 3)
        npt.assert_almost_equal(p[0], [1, 2])
        npt.assert_almost_equal(p[1], [3, 4])
        npt.assert_almost_equal(p[2], [5, 6])

    def test_is_clearable(self):
        p = m.Vector2dList([[1, 1]])
        self.assertEqual(len(p), 1)
        p.clear()
        self.assertEqual(len(p), 0)

    def test_is_popable(self):
        p = m.Vector2dList([[0, 0], [1, 1], [2, 2]])
        npt.assert_almost_equal(p.pop(), [2, 2])
        self.assertEqual(len(p), 2)
        npt.assert_almost_equal(p.pop(1), [1, 1])
        self.assertEqual(len(p), 1)
        npt.assert_almost_equal(p.pop(0), [0, 0])
        self.assertEqual(len(p), 0)
        with self.assertRaises(IndexError):
            p.pop()

    def test_is_iterable(self):
        p = m.Vector2dList([[0, 0], [1, 1], [2, 2]])
        for i, v in p:
            npt.assert_almost_equal(v, [i, i])


class ShapeTestCase(unittest.TestCase):
    def setUp(self):
        self.circle = m.Circle(Center=[0, 0], Radius=1.0)
        self.capsule = m.Capsule(C1=[0, 0], C2=[1, 1], R1=1.0, R2=1.0)
        self.polygon = m.Polygon(
            Vertices=[
                [1, 1],
                [-1, 1],
                [-1, -1],
                [1, -1],
            ]
        )

    def test_type(self):
        self.assertEqual(self.circle.ShapeType, m.Shape.Type.CIRCLE)
        self.assertEqual(self.capsule.ShapeType, m.Shape.Type.CAPSULE)
        self.assertEqual(self.polygon.ShapeType, m.Shape.Type.POLYGON)

    def test_circle_fields_manipulation(self):
        npt.assert_almost_equal(self.circle.Center, [0, 0])
        npt.assert_almost_equal(self.circle.Radius, 1.0)
        self.circle.Center = [1, 2]
        self.circle.Radius = 3.0
        npt.assert_almost_equal(self.circle.Center, [1, 2])
        npt.assert_almost_equal(self.circle.Radius, 3)

    def test_capsule_fields_manipulation(self):
        npt.assert_almost_equal(self.capsule.C1, [0, 0])
        npt.assert_almost_equal(self.capsule.R1, 1.0)
        npt.assert_almost_equal(self.capsule.C2, [1, 1])
        npt.assert_almost_equal(self.capsule.R2, 1.0)
        self.capsule.C1 = [-1, 0]
        self.capsule.R1 = 1.0
        self.capsule.C2 = [2, 3]
        self.capsule.R2 = 4.0
        npt.assert_almost_equal(self.capsule.C1, [-1, 0])
        npt.assert_almost_equal(self.capsule.R1, 1.0)
        npt.assert_almost_equal(self.capsule.C2, [2, 3])
        npt.assert_almost_equal(self.capsule.R2, 4.0)

    def test_polygon_fields_manipulation(self):
        self.assertEqual(len(self.polygon.Vertices), 4)
        npt.assert_almost_equal(self.polygon.Vertices[0], [1, 1])
        npt.assert_almost_equal(self.polygon.Vertices[1], [-1, 1])
        npt.assert_almost_equal(self.polygon.Vertices[2], [-1, -1])
        npt.assert_almost_equal(self.polygon.Vertices[3], [1, -1])
        self.polygon.Vertices.pop(1)
        self.polygon.Vertices.append([2, 3])
        self.assertEqual(len(self.polygon.Vertices), 4)
        npt.assert_almost_equal(self.polygon.Vertices[0], [1, 1])
        npt.assert_almost_equal(self.polygon.Vertices[1], [-1, -1])
        npt.assert_almost_equal(self.polygon.Vertices[2], [1, -1])
        npt.assert_almost_equal(self.polygon.Vertices[3], [2, 3])


class ShapeListTestCase(unittest.TestCase):
    def test_initialization(self):
        p = m.ShapeList()
        self.assertEqual(len(p), 0)
        p = m.ShapeList([m.Circle()])
        self.assertEqual(len(p), 1)
        with self.assertRaises(RuntimeError):
            p = m.ShapeList([1, 2, 3])

    def test_is_subscribable(self):
        p = m.ShapeList([m.Circle(), m.Capsule()])
        self.assertEqual(len(p), 2)
        npt.assert_almost_equal(p[0].Center, [0, 0])
        npt.assert_almost_equal(p[1].C2, [1, 1])
        p[0] = m.Capsule()
        npt.assert_almost_equal(p[0].C2, [1, 1])

    def test_is_extendable(self):
        p = m.ShapeList()
        self.assertEqual(len(p), 0)
        p.append(m.Circle())
        self.assertEqual(len(p), 1)
        p.extend([m.Capsule(), m.Polygon()])
        self.assertEqual(len(p), 3)
        npt.assert_almost_equal(p[0].Center, [0, 0])
        npt.assert_almost_equal(p[1].C2, [1, 1])
        npt.assert_almost_equal(p[2].Vertices[0], [1, 1])

    def test_is_clearable(self):
        p = m.ShapeList([m.Circle()])
        self.assertEqual(len(p), 1)
        p.clear()
        self.assertEqual(len(p), 0)

    def test_is_popable(self):
        p = m.ShapeList([m.Circle(), m.Capsule(), m.Polygon()])
        self.assertEqual(type(p.pop()), m.Polygon)
        self.assertEqual(len(p), 2)
        self.assertEqual(type(p.pop(1)), m.Capsule)
        self.assertEqual(len(p), 1)
        self.assertEqual(type(p.pop(0)), m.Circle)
        self.assertEqual(len(p), 0)
        with self.assertRaises(IndexError):
            p.pop()

    def test_is_iterable(self):
        p = m.ShapeList([m.Capsule(), m.Circle(), m.Polygon()])
        for i, v in enumerate(p):
            self.assertEqual(int(v.ShapeType), i)

    def test_wrap_mutable_elements(self):
        p = m.ShapeList([m.Circle(), m.Capsule(), m.Polygon()])
        p[0].Center = [1, 2]
        p[1].C2 = [3, 4]
        p[2].Vertices[3] = [5, 6]
        npt.assert_almost_equal(p[0].Center, [1, 2])
        npt.assert_almost_equal(p[0].Radius, 1)
        npt.assert_almost_equal(p[1].C1, [0, 0])
        npt.assert_almost_equal(p[1].R1, 1)
        npt.assert_almost_equal(p[1].C2, [3, 4])
        npt.assert_almost_equal(p[1].R2, 1)
        npt.assert_almost_equal(p[2].Vertices[0], [1, 1])
        npt.assert_almost_equal(p[2].Vertices[1], [-1, 1])
        npt.assert_almost_equal(p[2].Vertices[2], [-1, -1])
        npt.assert_almost_equal(p[2].Vertices[3], [5, 6])
