#include "tagbutton.h"

const static QString TAG_STYLE_SHEET = "\
QPushButton { \
        border-radius: 8px; \
        border-style:  solid; \
        border-width:1px; \
        border-color: #999; \
        color: #666; \
        background: white; \
        padding: 3px 10px; \
}\n\
QPushButton:hover:!pressed { \
        background: #eee; \
}\n\
QPushButton:pressed { \
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
    setObjectName("tagButton");
}

TagButton::TagButton(const QString text, QWidget *parent)
    : TagButton(parent)
{
    setText(text);
    setObjectName(text);
}
