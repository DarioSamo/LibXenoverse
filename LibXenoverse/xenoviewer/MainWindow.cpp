/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include "MainWindow.h"
#include "MainViewer.h"

MainWindow::MainWindow()
{
	QWidget *central_widget = new QWidget(this);

	// Build Left Panel
	QWidget *view_widget = new QWidget(central_widget);
	QVBoxLayout *view_widget_layout = new QVBoxLayout;

	ogre_widget = new QtOgre::OgreWidget(view_widget);
	ogre_widget->setMinimumWidth(500);
	ogre_widget->setMinimumHeight(500);

	QPushButton *reset_camera_button = new QPushButton("Reset Camera", view_widget);
	view_widget_layout->addWidget(ogre_widget);
	view_widget_layout->addWidget(reset_camera_button);
	view_widget->setLayout(view_widget_layout);

	// Build Right Panel
	MainViewer *main_viewer = new MainViewer(central_widget);

	// Build Main Central Widget
	QHBoxLayout *main_horizontal_layout = new QHBoxLayout;
	main_horizontal_layout->addWidget(view_widget);
	main_horizontal_layout->addWidget(main_viewer);
	central_widget->setLayout(main_horizontal_layout);
	setCentralWidget(central_widget);
	
	createActions();
	createMenus();

	resize(QSize(1600, 800));
	move(QPoint(160, 140));

	connect(reset_camera_button, SIGNAL(released()), this, SLOT(resetCamera()));

}

void MainWindow::closeEvent(QCloseEvent *event)
{
}

void MainWindow::open()
{
	QString fileName = QFileDialog::getOpenFileName(this);
	if (!fileName.isEmpty()) {
	}
}

bool MainWindow::save()
{
	QString fileName = QFileDialog::getSaveFileName(this);
	if (fileName.isEmpty())
		return false;

	return true;
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About Xenoviewer"),
		tr("<b>Xenoviewer</b> is an open-source Model/Skeleton/Animation viewer for the PC version of DRAGON BALL XENOVERSE. Dependencies used:"
		"<ul>"
		"<li><b>LibXenoverse</b>: <a href=\"https://github.com/DarioSamo/LibXenoverse\">Github Repository</a></li>"
		"<li><b>Ogre3D 2.0</b>: <a href=\"http://www.ogre3d.org/\">Homepage</a></li>"
		"<li><b>QtProject 5.4.1</b>: <a href=\"http://qt-project.org/\">Homepage</a></li>"
		"</ul>"
		));
}


void MainWindow::createActions()
{
	openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Open an existing file"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save..."), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("Save the document to disk"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(tr("About &Qt"), this);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	menuBar()->addSeparator();

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
}

void MainWindow::resetCamera() {
	ogre_widget->resetCamera();
}