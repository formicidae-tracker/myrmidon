import fort_myrmidon as m
import fort_myrmidon_utestdata as ud
import unittest


class ZoneTestCase(unittest.TestCase):
    def setUp(self):
        self.experiment = m.Experiment(str(ud.UData().Basedir / "zone-utests.myrmidon"))
        self.space = self.experiment.CreateSpace("nest")

    def test_field_manipulation(self):
        zone = self.space.CreateZone("exit")
        self.assertEqual(zone.Name, "exit")
        zone.Name = "food"
        self.assertEqual(zone.Name, "food")

    def test_zone_definition_manipulation(self):
        zone = self.space.CreateZone("exit")
        definitions = [
            zone.AddDefinition([], start=m.Time()),
            zone.AddDefinition([], end=m.Time()),
        ]
        with self.assertRaises(RuntimeError):
            zone.AddDefinition([], start=m.Time().Add(-1), end=m.Time().Add(1))

        self.assertEqual(len(zone.Definitions), 2)
        self.assertEqual(zone.Definitions[0], definitions[1])
        self.assertEqual(zone.Definitions[1], definitions[0])

        with self.assertRaises(IndexError):
            zone.DeleteDefinition(42)

        zone.DeleteDefinition(0)
        self.assertEqual(zone.Definitions[0], definitions[0])

    def test_zones_have_experiment_unique_ID(self):
        space2 = self.experiment.CreateSpace("foraging")
        zones = [
            self.space.CreateZone("zone"),
            space2.CreateZone("zone"),
            self.space.CreateZone("zone"),
        ]
        for i, z in enumerate(zones):
            self.assertEqual(z.ID, i + 1)

    def test_zone_definition_have_a_shape(self):
        zone = self.space.CreateZone("food")
        definition = zone.AddDefinition([])
        circle = m.Circle(Center=[0, 0], Radius=1)
        capsule = m.Capsule(C1=[0, 0], C2=[1, 1], R1=1, R2=1)
        self.assertEqual(len(definition.Shapes), 0)
        definition.Shapes = [circle, capsule]
        self.assertEqual(len(definition.Shapes), 2)
        self.assertEqual(definition.Shapes[0], circle)
        self.assertEqual(definition.Shapes[1], capsule)

    def test_zone_definition_have_time_validity(self):
        zone = self.space.CreateZone("food")
        definitions = [
            zone.AddDefinition([], end=m.Time()),
            zone.AddDefinition([], start=m.Time()),
        ]
        with self.assertRaises(ValueError):
            zone.AddDefinition([], start=m.Time.Forever(), end=m.Time.SinceEver())

        with self.assertRaises(RuntimeError):
            definitions[0].End = m.Time().Add(1)

        with self.assertRaises(RuntimeError):
            definitions[1].Start = m.Time().Add(-1)

        definitions[0].End = m.Time().Add(-1)
        definitions[1].Start = m.Time().Add(1)
        zone.AddDefinition([], start=m.Time().Add(-1), end=m.Time().Add(1))
