#include "tagsedit.h"
#include "flowlayout.h"

#include <QVBoxLayout>

TagsEdit::TagsEdit(QWidget *parent)
    : QFrame(parent)
{
    QLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);

    QScrollArea *sclSelected = new QScrollArea(this);
    layout->addWidget(sclSelected);

    laySelected = new FlowLayout(sclSelected);
    sclSelected->setMinimumHeight(100);
    sclSelected->setLayout(laySelected);

    cbxPicker = new QComboBox(this);
    cbxPicker->setEditable(true);
    layout->addWidget(cbxPicker);


    // mock
    cbxPicker->addItems({"Hello", "world", "Mr.Robot", "idiot", "Funny", "The Expanse"});
    cbxPicker->setCurrentIndex(-1);
}

/*
TagsEdit::~TagsEdit()
{
    QLayoutItem *item;
    while ((item = layout()->takeAt(0)))
        delete item;
    delete layout();
}
*/
