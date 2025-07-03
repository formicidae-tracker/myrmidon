import os
import re
import sys
import unittest

import fort_myrmidon as m
import fort_myrmidon_utestdata as ud

from . import assertions


class ExperimentTestCase(unittest.TestCase, assertions.CustomAssertion):
    def setUp(self):
        self.experiment = m.Experiment(
            str(ud.UData().Basedir / "public-experiment.myrmidon")
        )

    def tearDown(self):
        self.experiment = None

    def test_opening_dataless(self):
        self.experiment = m.Experiment.Open(
            str(ud.UData().CurrentVersionFile.AbsoluteFilePath)
        )
        self.assertTrue(self.experiment != None)
        dataInformation = m.Query.GetDataInformations(self.experiment)
        self.assertEqual(len(dataInformation.Spaces), 2)
        self.assertEqual(
            len(dataInformation.Spaces[1].TrackingDataDirectories),
            len(ud.UData().NestDataDirs),
        )
        for idx, info in enumerate(ud.UData().NestDataDirs):
            self.assertEqual(
                dataInformation.Spaces[1].TrackingDataDirectories[idx].URI,
                info.AbsoluteFilePath.name,
            )
        self.assertEqual(
            len(dataInformation.Spaces[2].TrackingDataDirectories),
            len(ud.UData().ForagingDataDirs),
        )
        for idx, info in enumerate(ud.UData().ForagingDataDirs):
            self.assertEqual(
                dataInformation.Spaces[2].TrackingDataDirectories[idx].URI,
                info.AbsoluteFilePath.name,
            )

        dataless = m.Experiment.OpenDataLess(
            str(ud.UData().CurrentVersionFile.AbsoluteFilePath)
        )
        dataInformation = m.Query.GetDataInformations(dataless)
        self.assertEqual(len(dataInformation.Spaces), 2)
        self.assertEqual(len(dataInformation.Spaces[1].TrackingDataDirectories), 0)
        self.assertEqual(len(dataInformation.Spaces[2].TrackingDataDirectories), 0)

        self.assertEqual(len(self.experiment.Ants), 3)
        self.assertEqual(len(dataless.Ants), 3)

        for antID, expected in self.experiment.Ants.items():
            self.assertTrue(antID in dataless.Ants)
            ant = dataless.Ants[antID]
            self.assertEqual(len(ant.Capsules), len(expected.Capsules))
            for i, tc in enumerate(expected.Capsules):
                eShapeType, eCapsule = tc
                shapeType, capsule = ant.Capsules[i]
                self.assertEqual(shapeType, eShapeType)
                self.assertCapsuleEqual(capsule, eCapsule)
            self.assertEqual(len(ant.Identifications), len(expected.Identifications))
            for i, eIdentification in enumerate(expected.Identifications):
                identification = ant.Identifications[i]
                self.assertEqual(identification.TagValue, eIdentification.TagValue)
                self.assertTimeEqual(identification.Start, eIdentification.Start)
                self.assertTimeEqual(identification.End, eIdentification.End)

        self.assertEqual(
            self.experiment.AbsoluteFilePath,
            str(ud.UData().CurrentVersionFile.AbsoluteFilePath),
        )
        self.assertEqual(
            dataless.AbsoluteFilePath,
            str(ud.UData().CurrentVersionFile.AbsoluteFilePath),
        )

        with self.assertRaises(RuntimeError):
            e = m.Experiment.Open(str(ud.UData().Basedir / "does-not-exists.myrmidon"))

        with self.assertRaises(RuntimeError):
            e = m.Experiment.OpenDataLess(
                str(ud.UData().Basedir / "does-not-exists.myrmidon")
            )

    def test_file_manipulation(self):
        dirs = [
            ud.UData().Basedir / "test-manipulation",
            ud.UData().Basedir / "test-manipulation-new",
        ]
        for d in dirs:
            try:
                os.makedirs(d)
            except FileExistsError:
                pass

        filepath = str(dirs[0] / "test.myrmidon")
        goodNewPath = str(dirs[0] / "test2.myrmidon")
        badNewPath = str(dirs[1] / "test.myrmidon")
        e = m.Experiment(filepath)
        e.Save(filepath)
        e.Save(goodNewPath)
        with self.assertRaises(ValueError):
            e.Save(badNewPath)

    def test_space_manipulation(self):
        spaces = [
            self.experiment.CreateSpace("nest"),
            self.experiment.CreateSpace("foraging"),
        ]
        self.assertEqual(len(self.experiment.Spaces), 2)
        for s in spaces:
            self.assertEqual(self.experiment.Spaces[s.ID], s)
        with self.assertRaises(IndexError):
            self.experiment.DeleteSpace(42)
        self.experiment.DeleteSpace(spaces[0].ID)
        self.assertEqual(len(self.experiment.Spaces), 1)
        self.assertFalse(spaces[0].ID in self.experiment.Spaces)
        self.assertTrue(spaces[1].ID in self.experiment.Spaces)

        tddInfo = ud.UData().ForagingDataDirs[0]
        self.experiment.AddTrackingDataDirectory(
            spaces[1].ID, str(tddInfo.AbsoluteFilePath)
        )
        with self.assertRaises(RuntimeError):
            self.experiment.DeleteSpace(spaces[1].ID)

    def test_TDD_manipulation(self):
        foragingID = self.experiment.CreateSpace("foraging").ID
        nestID = self.experiment.CreateSpace("nest").ID

        foragingTDDPath = ud.UData().ForagingDataDirs[0].AbsoluteFilePath
        nestTDDPath = ud.UData().NestDataDirs[0].AbsoluteFilePath
        badTDDPath = ud.UData().Basedir / "does-not-exist.0000"
        with self.assertRaises(IndexError):
            self.experiment.AddTrackingDataDirectory(42, str(foragingTDDPath))
        with self.assertRaises(RuntimeError):
            self.experiment.AddTrackingDataDirectory(foragingID, str(badTDDPath))
        URI = self.experiment.AddTrackingDataDirectory(foragingID, str(foragingTDDPath))
        self.assertEqual(URI, foragingTDDPath.name)
        with self.assertRaises(ValueError):
            self.experiment.AddTrackingDataDirectory(foragingID, str(nestTDDPath))

        with self.assertRaises(ValueError):
            self.experiment.AddTrackingDataDirectory(nestID, str(foragingTDDPath))
        URI = self.experiment.AddTrackingDataDirectory(nestID, str(nestTDDPath))
        self.assertEqual(URI, nestTDDPath.name)

        with self.assertRaises(ValueError):
            self.experiment.RemoveTrackingDataDirectory(badTDDPath.name)

        self.experiment.RemoveTrackingDataDirectory(URI)

    def test_ant_manipulation(self):
        a = self.experiment.CreateAnt()
        self.assertTrue(a.ID in self.experiment.Ants)
        self.assertEqual(self.experiment.Ants[a.ID], a)

        with self.assertRaises(IndexError):
            self.experiment.DeleteAnt(42)

        self.experiment.AddIdentification(a.ID, 0, m.Time.SinceEver(), m.Time.Forever())

        with self.assertRaises(RuntimeError):
            self.experiment.DeleteAnt(a.ID)

        self.experiment.DeleteIdentification(a.Identifications[0])
        self.experiment.DeleteAnt(a.ID)

    def test_identification_manipulation(self):
        ants = [self.experiment.CreateAnt(), self.experiment.CreateAnt()]
        with self.assertRaises(IndexError):
            self.experiment.AddIdentification(
                42, 0, m.Time.SinceEver(), m.Time.Forever()
            )
        self.experiment.AddIdentification(
            ants[0].ID, 0, m.Time.SinceEver(), m.Time.Forever()
        )

        with self.assertRaises(m.OverlappingIdentification):
            self.experiment.AddIdentification(
                ants[0].ID, 1, m.Time.SinceEver(), m.Time.Forever()
            )

        with self.assertRaises(m.OverlappingIdentification):
            self.experiment.AddIdentification(
                ants[1].ID, 0, m.Time.SinceEver(), m.Time.Forever()
            )

        self.experiment.AddIdentification(
            ants[1].ID, 1, m.Time.SinceEver(), m.Time.Forever()
        )

        e2 = m.Experiment("foo.myrmidon")
        a2 = e2.CreateAnt()
        i2 = e2.AddIdentification(a2.ID, 0, m.Time.SinceEver(), m.Time.Forever())

        with self.assertRaises(ValueError):
            self.experiment.DeleteIdentification(i2)

        ants[0].Identifications[0].End = m.Time()
        with self.assertRaises(RuntimeError):
            self.experiment.FreeIdentificationRangeAt(
                ants[0].Identifications[0].TagValue, m.Time().Add(-1)
            )

        low, high = self.experiment.FreeIdentificationRangeAt(
            ants[0].Identifications[0].TagValue, m.Time()
        )
        self.assertTimeEqual(low, m.Time())
        self.assertTimeEqual(high, m.Time.Forever())

        low, high = self.experiment.FreeIdentificationRangeAt(42, m.Time())
        self.assertTimeEqual(low, m.Time.SinceEver())
        self.assertTimeEqual(high, m.Time.Forever())

        identifications = self.experiment.IdentificationsAt(
            time=m.Time().Add(-1), removeUnidentifiedAnt=True
        )
        self.assertEqual(len(identifications), 2)
        self.assertEqual(identifications[ants[0].ID], 0)
        self.assertEqual(identifications[ants[1].ID], 1)
        identifications = self.experiment.IdentificationsAt(
            time=m.Time(), removeUnidentifiedAnt=True
        )
        self.assertEqual(len(identifications), 1)
        self.assertEqual(identifications[ants[1].ID], 1)
        self.assertFalse(ants[0].ID in identifications)
        identifications = self.experiment.IdentificationsAt(
            time=m.Time(), removeUnidentifiedAnt=False
        )
        self.assertEqual(len(identifications), 2)
        self.assertEqual(identifications[ants[0].ID], 2 ** 32 - 1)
        self.assertEqual(identifications[ants[1].ID], 1)

    def test_fields_manipulation(self):
        self.assertEqual(self.experiment.Name, "")
        self.experiment.Name = "foo"
        self.assertEqual(self.experiment.Name, "foo")

        self.assertEqual(self.experiment.Author, "")
        self.experiment.Author = "bar"
        self.assertEqual(self.experiment.Author, "bar")

        self.assertEqual(self.experiment.Comment, "")
        self.experiment.Comment = "baz"
        self.assertEqual(self.experiment.Comment, "baz")

        self.assertEqual(self.experiment.Family, m.TagFamily.Undefined)
        spaceID = self.experiment.CreateSpace("foraging").ID
        tddInfo = ud.UData().ForagingDataDirs[0]
        self.experiment.AddTrackingDataDirectory(spaceID, str(tddInfo.AbsoluteFilePath))
        self.assertEqual(self.experiment.Family, tddInfo.Family)

        self.assertEqual(self.experiment.DefaultTagSize, 1.0)
        self.experiment.DefaultTagSize = 2.5
        self.assertEqual(self.experiment.DefaultTagSize, 2.5)

    def test_measurement_type_manipulation(self):
        mtID = self.experiment.CreateMeasurementType("antennas")
        self.assertEqual(len(self.experiment.MeasurementTypeNames), 2)
        self.assertEqual(self.experiment.MeasurementTypeNames[mtID], "antennas")
        self.assertEqual(self.experiment.MeasurementTypeNames[1], "head-tail")
        with self.assertRaises(IndexError):
            self.experiment.SetMeasurementTypeName(42, "foo")

        self.experiment.SetMeasurementTypeName(1, "foo")

        with self.assertRaises(IndexError):
            self.experiment.DeleteMeasurementType(42)

        with self.assertRaises(ValueError):
            self.experiment.DeleteMeasurementType(1)

        self.experiment.DeleteMeasurementType(mtID)

    def test_ant_shape_type_manipulation(self):
        bodyID = self.experiment.CreateAntShapeType("body")
        headID = self.experiment.CreateAntShapeType("head")
        self.assertTrue(bodyID in self.experiment.AntShapeTypeNames)
        self.assertTrue(headID in self.experiment.AntShapeTypeNames)
        self.assertEqual(self.experiment.AntShapeTypeNames[bodyID], "body")
        self.assertEqual(self.experiment.AntShapeTypeNames[headID], "head")

        with self.assertRaises(IndexError):
            self.experiment.SetAntShapeTypeName(42, "foo")

        self.experiment.SetAntShapeTypeName(bodyID, "foo")

        with self.assertRaises(IndexError):
            self.experiment.DeleteAntShapeType(42)

        a = self.experiment.CreateAnt()
        a.AddCapsule(bodyID, m.Capsule([0, 0], [1, 1], 1, 1))

        with self.assertRaises(RuntimeError):
            self.experiment.DeleteAntShapeType(bodyID)

        self.experiment.DeleteAntShapeType(headID)

    def test_meta_data_key_manipulation(self):
        self.experiment.SetMetaDataKey("alive", True)
        self.experiment.SetMetaDataKey("group", "worker")
        self.assertEqual(len(self.experiment.MetaDataKeys), 2)
        self.assertTrue("alive" in self.experiment.MetaDataKeys)
        self.assertTrue("group" in self.experiment.MetaDataKeys)
        self.assertEqual(self.experiment.MetaDataKeys["alive"], True)
        self.assertEqual(self.experiment.MetaDataKeys["group"], "worker")

        a = self.experiment.CreateAnt()
        a.SetValue(key="group", value="nurse", time=m.Time.SinceEver())

        with self.assertRaises(IndexError):
            self.experiment.DeleteMetaDataKey("foo")

        with self.assertRaises(RuntimeError):
            self.experiment.DeleteMetaDataKey("group")

        with self.assertRaises(IndexError):
            self.experiment.RenameMetaDataKey(oldKey="foo", newKey="bar")

        with self.assertRaises(ValueError):
            self.experiment.RenameMetaDataKey(oldKey="alive", newKey="group")

        self.experiment.RenameMetaDataKey(oldKey="alive", newKey="death-date")
        self.experiment.SetMetaDataKey("death-date", m.Time.Forever())
        self.experiment.DeleteMetaDataKey("death-date")
        self.experiment.SetMetaDataKey("group", "forager")

        with self.assertRaises(RuntimeError):
            self.experiment.SetMetaDataKey("group", 42)

        a.SetValue(key="group", value="forager", time=m.Time())
        a.SetValue(key="group", value="worker", time=m.Time().Add(1))
        self.experiment.SetMetaDataKey("group", "worker")
        self.assertEqual(a.GetValue("group", m.Time()), "worker")
        self.assertEqual(a.GetValue("group", m.Time().Add(1)), "worker")

        a.DeleteValue("group", m.Time.SinceEver())
        a.DeleteValue("group", m.Time())
        a.DeleteValue("group", m.Time().Add(1))
        self.experiment.SetMetaDataKey("group", 42)
        self.assertEqual(self.experiment.MetaDataKeys["group"], 42)

    def test_can_open_corrupted_data_dir(self):
        s = self.experiment.CreateSpace("main")
        corruptedPath = str(ud.UData().CorruptedDataDir.AbsoluteFilePath)
        with self.assertRaises(m.FixableError) as e:
            self.experiment.AddTrackingDataDirectory(
                spaceID=s.ID, filepath=corruptedPath
            )

        URI = self.experiment.AddTrackingDataDirectory(
            spaceID=s.ID, filepath=corruptedPath, fixCorruptedData=True
        )
        self.experiment.RemoveTrackingDataDirectory(URI)
        self.experiment.AddTrackingDataDirectory(spaceID=s.ID, filepath=corruptedPath)
        self.maxDiff = None
        match = re.search("Could not find frame .* in (.*)", str(e.exception))
        filename = match.group(1)
        os.replace(filename + ".bak", filename)

        with self.assertRaises(m.FixableError):
            self.experiment.EnsureAllDataIsLoaded(fixCorruptedData=False)

        self.experiment.RemoveTrackingDataDirectory(URI)
        # no need to fix here, the cache has the fix.
        self.experiment.AddTrackingDataDirectory(spaceID=s.ID, filepath=corruptedPath)
        self.experiment.EnsureAllDataIsLoaded(fixCorruptedData=True)

        self.experiment.RemoveTrackingDataDirectory(URI)
        self.experiment.AddTrackingDataDirectory(spaceID=s.ID, filepath=corruptedPath)
        # here was fixed with previous load
        self.experiment.EnsureAllDataIsLoaded(fixCorruptedData=False)
