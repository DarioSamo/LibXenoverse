#include "MainViewer.h"

MainViewer::MainViewer(QWidget *parent)
	: QWidget(parent) {
	setupUi(this);

	FileTree->acceptDrops();
}