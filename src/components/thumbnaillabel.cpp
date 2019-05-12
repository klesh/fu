#include "thumbnaillabel.h"

ThumbnailLabel::ThumbnailLabel(QWidget *parent)
    : QLabel(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumSize(160, 160);
    setMaximumSize(160, 160);

    setAlignment(Qt::AlignCenter);
    setStyleSheet("background: white");
}
