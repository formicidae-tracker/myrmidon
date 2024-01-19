#include "AntGeometryWorkspace.hpp"

#include <QAction>
#include <QComboBox>
#include <QMainWindow>

#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/utest-data/UTestData.hpp>

#include <fort/studio/bridge/AntShapeBridge.hpp>
#include <fort/studio/bridge/AntShapeTypeBridge.hpp>
#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/IdentifierBridge.hpp>

void AntShapeWorkspaceUTest::SetUp() {
	shapeWs = new AntShapeWorkspace();
	WorkspaceUTest::SetUp(
	    fmp::Experiment::Create(
	        TestSetup::UTestData().Basedir() / "ant-shape-workspace.myrmidon"
	    ),
	    shapeWs
	);

	bridge->createAnt();
	bridge->identifier()->addIdentification(
	    1,
	    1,
	    fort::Time::SinceEver(),
	    fort::Time::Forever()
	);

	bridge->antShapeTypes()->addType("foo");

	shapeWs->setTagCloseUp(std::make_shared<fmp::TagCloseUp>(
	    TestSetup::UTestData().Basedir() / "",
	    fmp::FrameReference("foo.0000", 1, fort::Time()),
	    1,
	    Eigen::Vector2d::Zero(),
	    0.0,
	    fmp::Vector2dList(
	        {Eigen::Vector2d(1, 1),
	         Eigen::Vector2d(1, -1),
	         Eigen::Vector2d(-1, -1),
	         Eigen::Vector2d(-1, 1)}
	    )
	));

	bridge->selectAnt(1);
}

void AntShapeWorkspaceUTest::TearDown() {
	delete shapeWs;
	WorkspaceUTest::TearDown();
}


TEST_F(AntShapeWorkspaceUTest,AddCapsuleActionState) {
	shapeWs->typeBox()->setCurrentIndex(-1);
	EXPECT_EQ(shapeWs->insertAction()->isEnabled(),false);
	shapeWs->typeBox()->setCurrentIndex(0);
	EXPECT_EQ(shapeWs->insertAction()->isEnabled(),true);
}

TEST_F(AntShapeWorkspaceUTest,CloneShapeAction) {
	EXPECT_EQ(shapeWs->cloneAntShapeAction()->isEnabled(),false) << "Should be disabled if current ant has no shape";
	bridge->antShapes()->addCapsule(1,1,std::make_shared<fmp::Capsule>(Eigen::Vector2d(0,-1),
	                                                                   Eigen::Vector2d(0,1),
	                                                                   0.3,0.3));
	EXPECT_EQ(shapeWs->cloneAntShapeAction()->isEnabled(),true) << "Should be enabled if current ant has a shape";
}
