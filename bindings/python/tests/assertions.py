import fort_myrmidon as m
import numpy.testing as npt
import numpy as np


class CustomAssertion:
    def assertVector2dEqual(self, a, b):
        npt.assert_almost_equal(a, b)

    def assertTimeEqual(self, a, b):
        self.assertIsInstance(a, m.Time)
        self.assertIsInstance(b, m.Time)
        if a.Equals(b) == False:
            raise AssertionError(
                "time a: " + str(a) + " and b: " + str(b) + " are not equals"
            )

    def assertCapsuleEqual(self, a, b):
        self.assertVector2dEqual(a.C1, b.C1)
        self.assertVector2dEqual(a.C2, b.C2)
        self.assertEqual(a.R1, b.R1)
        self.assertEqual(a.R2, b.R2)

    def assertSingleTagStatEqual(self, a, b):
        self.assertEqual(a.ID, b.ID)
        self.assertTimeEqual(a.FirstSeen, b.FirstSeen)
        self.assertTimeEqual(a.LastSeen, b.LastSeen)
        npt.assert_equal(a.Counts, b.Counts)

    def assertTagStatisticsEqual(self, a, b):
        self.assertEqual(len(a), len(b))
        for tagID, expectedTagStat in b.items():
            if tagID not in a:
                raise AssertionError(
                    "Could not found TagID " + m.FormatTagID(tagID) + " in a"
                )
            self.assertSingleTagStatEqual(a[tagID], expectedTagStat)

    def assertIdentifiedFrameEqual(self, a, b):
        self.assertTimeEqual(a.FrameTime, b.FrameTime)
        self.assertEqual(a.Space, b.Space)
        self.assertEqual(a.Height, b.Height)
        self.assertEqual(a.Width, b.Width)
        self.assertEqual(a.Positions.shape, b.Positions.shape)
        for bRow in b.Positions:
            antID = int(bRow[0])
            try:
                aRow = next(x for x in a.Positions if x[0] == antID)
                npt.assert_almost_equal(aRow, bRow)
            except StopIteration:
                raise AssertionError(
                    "Could not found AntID " + m.FormatAntID(antID) + " in a"
                )

    def assertInteractionTypesEqual(self, a, b):
        self.assertEqual(a.shape, b.shape)
        for i in range(b.shape[0]):
            if np.all(np.any(a == b[i, :], axis=0)) == False:
                raise AssertionError(
                    "Could not find InteractionType("
                    + str(b[i, 0])
                    + ","
                    + str(b[i, 1])
                    + ") in "
                    + str(a)
                )

    def assertCollisionEqual(self, a, b):
        self.assertEqual(a.IDs, b.IDs)
        self.assertEqual(a.Zone, b.Zone)
        self.assertInteractionTypesEqual(a.Types, b.Types)

    def assertCollisionFrameEqual(self, a, b):
        self.assertTimeEqual(a.FrameTime, b.FrameTime)
        self.assertEqual(a.Space, b.Space)
        self.assertEqual(len(a.Collisions), len(b.Collisions))
        for expectedCollision in b.Collisions:
            try:
                aCollision = next(
                    x for x in b.Collisions if x.IDs == expectedCollision.IDs
                )
                self.assertCollisionEqual(aCollision, expectedCollision)
            except StopIteration:
                raise AssertionError(
                    "Could not found Collision("
                    + str(expectedCollisions.IDs[0])
                    + ","
                    + str(expectedCollisions.IDs[0])
                    + ") in a"
                )

    def assertAntTrajectoryEqual(self, a, b):
        self.assertEqual(a.Ant, b.Ant)
        self.assertEqual(a.Space, b.Space)
        self.assertTimeEqual(a.Start, b.Start)
        npt.assert_almost_equal(a.Positions, b.Positions)

    def assertAntTrajectorySegmentEqual(self, a, b):
        self.assertAntTrajectoryEqual(a.Trajectory, b.Trajectory)
        self.assertEqual(a.Begin, b.Begin)
        self.assertEqual(a.End, b.End)

    def assertAntTrajectorySummaryEqual(self, a, b):
        npt.assert_almost_equal(a.Mean, b.Mean)
        self.assertEqual(a.Zones, b.Zones)

    def assertAntInteractionEqual(self, a, b):
        self.assertEqual(a.IDs, b.IDs)
        self.assertEqual(a.Space, b.Space)
        self.assertInteractionTypesEqual(a.Types, b.Types)
        self.assertTimeEqual(a.Start, b.Start)
        self.assertTimeEqual(a.End, b.End)
        for i in range(2):
            if type(b.Trajectories[i]) == m.AntTrajectorySegment:
                self.assertAntTrajectorySegmentEqual(
                    a.Trajectories[i], b.Trajectories[i]
                )
            else:
                self.assertAntTrajectorySummaryEqual(
                    a.Trajectories[i], b.Trajectories[i]
                )

    def assertVideoFrameDataEqual(self, a, b):
        self.assertEqual(a.Position, b.Position)
        self.assertTimeEqual(a.Time, b.Time)
        if b.Identified:
            self.assertIsNotNone(a.Identified)
            self.assertIdentifiedFrameEqual(a.Identified, b.Identified)
        else:
            self.assertIsNone(a.Identified)
        if b.Collided:
            self.assertIsNotNone(a.Collided)
            self.assertCollisionFrameEqual(a.Collided, b.Collided)
        else:
            self.assertIsNone(a.Collided)

        self.assertEqual(len(a.Trajectories), len(b.Trajectories))
        for t, e in zip(a.Trajectories, b.Trajectories):
            self.assertAntTrajectoryEqual(t, e)
        self.assertEqual(len(a.Interactions), len(b.Interactions))
        for i, e in zip(a.Interactions, b.Interactions):
            self.assertAntInteractionEqual(i, e)

    def assertVideoSegmentEqual(self, a, b):
        self.assertEqual(a.Space, b.Space)
        self.assertEqual(a.AbsoluteFilePath, b.AbsoluteFilePath)
        self.assertEqual(a.Begin, b.Begin)
        self.assertEqual(a.End, b.End)
        self.assertEqual(len(a.Data), len(b.Data))
        for d, e in zip(a.Data, b.Data):
            self.assertVideoFrameDataEqual(d, e)
