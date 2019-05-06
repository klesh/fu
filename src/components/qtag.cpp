#include "qtag.h"

const static QString TAG_STYLE_SHEET = "\
* { \
        border-radius: 8px; \
        border-style:  solid; \
        border-width:1px; \
        border-color: #333; \
        background: white; \
        padding: 3px 10px; \
        color: #333; \
}\n\
:hover:!pressed { \
        background: #eee; \
}\n\
:pressed { \
        background: #28a745; \
        color: white; \
}\n\
" ;

QTag::QTag(QWidget *parent)
    : QPushButton (parent)
{
    setStyleSheet(TAG_STYLE_SHEET);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QTag::QTag(const QString text, QWidget *parent)
    : QTag(parent)
{
    setText(text);
}
