/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "../simple_test_framework.h"
#include "kddockwidgets/core/FloatingWindow.h"
#include "kddockwidgets/core/Group.h"
#include "kddockwidgets/core/TitleBar.h"
#include "kddockwidgets/core/DockWidget.h"
#include "kddockwidgets/core/ViewFactory.h"
#include "kddockwidgets/core/Platform.h"
#include "Config.h"

#include <QPointer>

using namespace KDDockWidgets;
using namespace KDDockWidgets::Core;

KDDW_QCORO_TASK tst_floatingWindowCtor()
{
    auto dw = Config::self().viewFactory()->createDockWidget("dw1")->asDockWidgetController();
    CHECK(dw->view()->rootView()->is(ViewType::DockWidget));
    CHECK(!dw->view()->parentView());

    dw->view()->show();
    CHECK(dw->view()->parentView());
    CHECK(dw->view()->rootView()->is(ViewType::FloatingWindow));

    CHECK(dw->floatingWindow());

    /// Wait for FloatingWindow to be created
    KDDW_CO_AWAIT Platform::instance()->tests_wait(100);

    auto rootView = dw->view()->rootView();
    CHECK(rootView);

    CHECK(rootView->is(ViewType::FloatingWindow));
    CHECK(rootView->controller());
    CHECK(rootView->controller()->is(ViewType::FloatingWindow));
    CHECK(rootView->controller()->isVisible());

    Core::FloatingWindow *fw = dw->floatingWindow();
    CHECK(fw);
    CHECK(fw->view()->equals(rootView));

    delete dw;


    KDDW_TEST_RETURN(true);
}

KDDW_QCORO_TASK tst_floatingWindowClose()
{
    // Tests that a floating window is deleted after being closed

    auto dw = Config::self().viewFactory()->createDockWidget("dw1")->asDockWidgetController();
    dw->view()->show();
    QPointer<Core::FloatingWindow> fw = dw->floatingWindow();
    CHECK(fw);

    auto titleBar = fw->titleBar();
    CHECK(titleBar);
    CHECK(titleBar->isVisible());
    titleBar->onCloseClicked();
    CHECK(!dw->isOpen());
    CHECK(KDDW_CO_AWAIT Platform::instance()->tests_waitForDeleted2(fw));
    CHECK(!fw);

    delete dw;
    KDDW_TEST_RETURN(true);
}

static const auto s_tests = std::vector<std::function<KDDW_QCORO_TASK()>> { tst_floatingWindowClose, tst_floatingWindowCtor };

#include "../tests_main.h"
