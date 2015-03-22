// -------------------------------------------------------------------------------------------------
/**
*  @file
*  @brief
*  @author Gerolf Reinwardt
*  @date   30.01.2011
*
*  Copyright (c) 2011, Gerolf Reinwardt. All rights reserved.
*
*  Simplified BSD License
*
*  Redistribution and use in source and binary forms, with or without modification, are
*  permitted provided that the following conditions are met:
*
*  1. Redistributions of source code must retain the above copyright notice, this list of
*     conditions and the following disclaimer.
*
*  2. Redistributions in binary form must reproduce the above copyright notice, this list
*     of conditions and the following disclaimer in the documentation and/or other materials
*     provided with the distribution.
*
*  THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY EXPRESS OR IMPLIED
*  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
*  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR
*  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*  SERVICES; LOSS OF USE, DATA, OR #######; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
*  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*  The views and conclusions contained in the software and documentation are those of the
*  authors and should not be interpreted as representing official policies, either expressed
*  or implied, of Gerolf Reinwardt.
*/
// -------------------------------------------------------------------------------------------------

// ----- local includes ----------------------------------------------------------------------------
#include "DocumentWindow.h"


// ----- construction ------------------------------------------------------------------------------
DocumentWindow::DocumentWindow(QWidget* parent, Qt::WindowFlags flags) :
QMainWindow(parent, flags)
{
	setAcceptDrops(true);
}

DocumentWindow::~DocumentWindow()
{
}

// ----- operators ---------------------------------------------------------------------------------
// ----- methods -----------------------------------------------------------------------------------
// ----- accessors ---------------------------------------------------------------------------------
// ----- public slots ------------------------------------------------------------------------------
// ----- protected slots ---------------------------------------------------------------------------
// ----- events ------------------------------------------------------------------------------------
void DocumentWindow::dragEnterEvent(QDragEnterEvent* event)
{
	// if some actions should not be usable, like move, this code must be adopted
	event->acceptProposedAction();
}

void DocumentWindow::dragMoveEvent(QDragMoveEvent* event)
{
	// if some actions should not be usable, like move, this code must be adopted
	event->acceptProposedAction();
}


void DocumentWindow::dragLeaveEvent(QDragLeaveEvent* event)
{
	event->accept();
}

void DocumentWindow::dropEvent(QDropEvent* event)
{
	const QMimeData* mimeData = event->mimeData();

	if (mimeData->hasUrls())
	{
		QStringList pathList;
		QList<QUrl> urlList = mimeData->urls();

		for (int i = 0; i < urlList.size() && i < 32; ++i)
		{
			pathList.append(urlList.at(i).toLocalFile());
		}

		if (openFiles(pathList))
			event->acceptProposedAction();
	}
}

// ----- private slots -----------------------------------------------------------------------------
// ----- private helpers ---------------------------------------------------------------------------
