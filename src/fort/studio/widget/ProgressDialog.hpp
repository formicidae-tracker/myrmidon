#pragma once

#include "fort/myrmidon/types/Reporter.hpp"

class QProgressDialog;
class QString;
class QWidget;

std::tuple<QProgressDialog *, std::unique_ptr<fort::myrmidon::ProgressReporter>>
OpenItemProgressDialog(const QString &title, QWidget *parent);
