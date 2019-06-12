#include "tagbutton.h"

const static QString TAG_STYLE_SHEET = "\
* { \
        border-radius: 8px; \
        border-style:  solid; \
        border-width:1px; \
        border-color: #999; \
        color: #666; \
        background: white; \
        padding: 3px 10px; \
}\n\
:hover:!pressed { \
        background: #eee; \
}\n\
:pressed { \
        background: #28a745; \
        color: white; \
}\n\
" ;

TagButton::TagButton(QWidget *parent)
    : QPushButton (parent)
{
    setStyleSheet(TAG_STYLE_SHEET);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMaximumHeight(20);
}

TagButton::TagButton(const QString text, QWidget *parent)
    : TagButton(parent)
{
    setText(text);
    setObjectName(text);
}
