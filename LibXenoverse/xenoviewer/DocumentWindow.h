// -------------------------------------------------------------------------------------------------
/**
*  @file
*  @brief
*  @author Gerolf Reinwardt
*  @date   30. march 2011
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

#ifndef WIDGET_H
#define WIDGET_H

// ----- local includes ----------------------------------------------------------------------------
// ----- pre defines -------------------------------------------------------------------------------

// ----- class definition --------------------------------------------------------------------------
/**
*  @short
*
*  The usage is fairly easy. Derive your own MainWindow class from DocumentWindow instead of QMainWindow
*  and implement the pure virtual function openFiles.
*
*  @code
MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags) :
DocumentWindow(parent, flags)
{
...

setWindowTitle(tr("MDI"));
setUnifiedTitleAndToolBarOnMac(true);
}
@endcode
*
*  Aditionally, the openFiles must be implermented:
*
*  @code
bool MyClass::openFiles(const QStringList& pathList)
{
bool success = true;
for (int i = 0; i < pathList.size() && i < 32; ++i)
{
MdiChild *child = createMdiChild();
if (child->loadFile(pathList.at(i)))
{
statusBar()->showMessage(tr("File loaded"), 2000);
child->show();
}
else
{
child->close();
}
}
return success;
}
@endcode
*
*/
class DocumentWindow : public QMainWindow
{
	Q_OBJECT
public:
	// ----- enums ---------------------------------------------------------------------------------
	// ----- construction --------------------------------------------------------------------------
	/**
	*  Constructor.
	*
	*  Creates a DocumentWindow with a given @arg parent and @arg flags.
	*/
	explicit DocumentWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);

	/**
	*  Destructor
	*/
	~DocumentWindow();

	// ----- operators -----------------------------------------------------------------------------
	// ----- methods -------------------------------------------------------------------------------
	// ----- accessors -----------------------------------------------------------------------------
	// ----- members -------------------------------------------------------------------------------

protected:
	// ----- events --------------------------------------------------------------------------------
	/**
	*  this event is called when the mouse enters the widgets area during a drag/drop operation
	*/
	void dragEnterEvent(QDragEnterEvent* event);

	/**
	*  this event is called when the mouse moves inside the widgets area during a drag/drop operation
	*/
	void dragMoveEvent(QDragMoveEvent* event);

	/**
	*  this event is called when the mouse leaves the widgets area during a drag/drop operation
	*/
	void dragLeaveEvent(QDragLeaveEvent* event);

	/**
	*  this event is called when the drop operation is initiated at the widget
	*/
	void dropEvent(QDropEvent* event);

	// ----- helpers -------------------------------------------------------------------------------
	/**
	*  This method must be implemented by the client. It is used to opened the dropped files.
	*
	*  @param pathList list of urls given by the drop event
	*  @retval true if successfull otherwise false
	*
	*  Here is an example implementation:
	*
	*  @code
	bool MyClass::openFiles(const QStringList& pathList)
	{
	bool success = true;
	for (int i = 0; i < pathList.size() && i < 32; ++i)
	{
	MdiChild *child = createMdiChild();
	if (child->loadFile(pathList.at(i)))
	{
	statusBar()->showMessage(tr("File loaded"), 2000);
	child->show();
	}
	else
	{
	child->close();
	}
	}
	return success;
	}
	@endcode
	*/
	virtual bool openFiles(const QStringList& pathList) = 0;

private:
};

#endif // WIDGET_H
