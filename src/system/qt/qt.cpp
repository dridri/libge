/*
	The Gamma Engine Library is a multiplatform library made to make games
	Copyright (C) 2014  Aubry Adrien (dridri85)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//#include <QApplication>
//#include <QGLWidget>

#include "../../ge_internal.h"

extern "C" void QtInit(LibGE_QtContext* context)
{
/*
	int ac = 1;
	char* av[] = { (char*)"" };

	QApplication* app = 0;
	app = new QApplication(ac, av);
	QGLWidget* widget = new QGLWidget();

	widget->resize(libge_context->width, libge_context->height);
	widget->show();

	context->qt = (void*)app;
	context->widget = (void*)widget;
*/
}

extern "C" void QtSwapBuffers()
{
/*
	LibGE_QtContext* context = (LibGE_QtContext*)libge_context->syscontext;
	QApplication* app = (QApplication*)context->qt;
	QGLWidget* widget = (QGLWidget*)context->widget;

	widget->swapBuffers();
	app->processEvents();
*/
}
