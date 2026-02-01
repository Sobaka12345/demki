#pragma once

#include "../abstract/widget.hpp"
#include "vk/widget.hpp"
#include "ogl/widget.hpp"

#include <surface.hpp>

#include <QWidget>

namespace shell::qt {

namespace __private {

class BaseQWidget: public QWidget {
    Q_OBJECT
public:
    BaseQWidget(QWidget* parent = nullptr);
    virtual ~BaseQWidget() override;

    virtual bool event(QEvent* e) override;
    virtual bool eventFilter(QObject* watched, QEvent* e) override;

// signals:
    // TO DO
};

}


template <typename GApiT>
class Widget : public ComplexLinearHierarchy<TypeList<GApiT>, 
    SimpleNonLinearHierarchy<
        TypeList<abstract::Widget<GApiT>, 
        __private::BaseQWidget>
    >, 
    __private::Widget>
{
public:
    Widget(QWidget* parent);

    // TO DO
};

}    //  namespace shell::qt
