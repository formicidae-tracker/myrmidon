#pragma once


#include <QObject>

#include "Error.hpp"

#include <myrmidon/priv/Experiment.hpp>
#include <myrmidon/priv/Ant.hpp>



class ExperimentController : public QObject {
	Q_OBJECT
public:
	virtual ~ExperimentController();

	const fort::myrmidon::priv::Experiment & experiment() const;

	bool isModified() const;

	static ExperimentController * open(const QString & path, QObject * parent, Error & error);
	static ExperimentController * create(const QString & path, QObject * parent, Error & error);


signals:
	void modified(bool);
	void dataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath & );
	void antListModified(const fort::myrmidon::priv::AntByID & );
	void newAntIdentification(const fort::myrmidon::priv::IdentificationPtr &);
public slots:

	Error addDataDirectory(const QString & path);
	Error removeDataDirectory(const QString & path);
	Error save(const QString & path);
	void  setModified(bool modified);

	fort::myrmidon::priv::Ant::Ptr createAnt();
	Error removeAnt(fort::myrmidon::Ant::ID ID);

	Error addIdentification(fort::myrmidon::Ant::ID ID,
	                        uint32_t tagValue,
	                        const fort::myrmidon::priv::FramePointer::Ptr & start,
	                        const fort::myrmidon::priv::FramePointer::Ptr & end);


	void setName(const QString & name);
	void setAuthor(const QString & author);
	void setComment(const QString & comment);
	void setTagFamily(fort::myrmidon::priv::Experiment::TagFamily tf);
	void setThreshold(uint8_t th);
private:
	fort::myrmidon::priv::Experiment::Ptr d_experiment;
	bool d_modified;

	ExperimentController(fort::myrmidon::priv::Experiment::Ptr & experiment,
	                     QObject * parent = NULL);

};
