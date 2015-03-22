void QErrorMessage(QString error_str) {
	QMessageBox messageBox;
	messageBox.critical(0, "Error", error_str);
	messageBox.setFixedSize(500, 200);
}