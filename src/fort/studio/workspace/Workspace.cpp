#include "Workspace.hpp"

#include <QToolBar>
#include <QKeySequence>
#include <QObject>

Workspace::Workspace(bool showAntSelector,
                     QWidget * parent)
	: QWidget(parent) {
}

Workspace::~Workspace() {
}


NavigationAction::NavigationAction(QWidget * parent)
	: NextTag(nullptr)
	, PreviousTag(nullptr)
	, NextCloseUp(nullptr)
	, PreviousCloseUp(nullptr)
	, CopyCurrentTime(nullptr)
	, JumpToTime(nullptr)
	, NavigationToolBar(nullptr) {

	NavigationToolBar = new QToolBar(parent);

#define set_button(res,symbolStr,legendStr,shortCutStr,toolTipStr) do {	  \
		(res) = NavigationToolBar->addAction(QIcon::fromTheme(symbolStr), \
		                                     QObject::tr(legendStr)); \
		(res)->setShortcut(QKeySequence(shortCutStr)); \
		(res)->setToolTip(QObject::tr(toolTipStr " (" shortCutStr ")")); \
		(res)->setStatusTip((res)->toolTip()); \
		(res)->setEnabled(false); \
	}while(0)
	set_button(PreviousTag,
	           "go-first-symbolic",
	           "Previous Tag",
	           "Alt+W",
	           "Jump to previous tag");

	set_button(PreviousCloseUp,
	           "go-previous-symbolic",
	           "Previous Close Up",
	           "Alt+A",
	           "Jump to previous close-up");

	set_button(NextCloseUp,
	           "go-next-symbolic",
	           "Next Close Up",
	           "Alt+D",
	           "Jump to next close-up");

	set_button(NextTag,
	           "go-last-symbolic",
	           "Next Tag",
	           "Alt+S",
	           "Jump to next tag");

	NavigationToolBar->addSeparator();
	set_button(CopyCurrentTime,
	           "edit-copy-symbolic",
	           "Copy Current Time",
	           "Ctrl+Shift+C",
	           "Copy time of currently displayed frame or close-up");

	NavigationToolBar->addSeparator();
	set_button(JumpToTime,
	           "go-jump-symbolic",
	           "Jump To Time",
	           "Ctrl+T",
	           "Jump current movie to time");
#undef set_button

}
