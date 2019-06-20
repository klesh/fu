#include "tagsedit.h"
#include "flowlayout.h"
#include "tagbutton.h"
#include "../application.h"

#include <QVBoxLayout>

TagsEdit::TagsEdit(QWidget *parent)
    : QFrame(parent)
{
    QLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);

    sclSelected = new QScrollArea(this);
    layout->addWidget(sclSelected);

    laySelected = new FlowLayout(sclSelected);
    sclSelected->setMinimumHeight(100);
    sclSelected->setLayout(laySelected);

    cbxPicker = new QComboBox(this);
    cbxPicker->setEditable(true);
    layout->addWidget(cbxPicker);

    connect(cbxPicker, SIGNAL(activated(const QString&)), this, SLOT(selectTag(const QString&)));

    for (auto &tag : APP->tagService()->getAll()) {
        cbxPicker->addItem(tag.name, tag.id);
    }
    cbxPicker->setCurrentIndex(-1);
}

bool TagsEdit::isTagSelected(const QString &tag)
{
    return sclSelected->findChild<QPushButton*>(tag);
}

QStringList TagsEdit::tags()
{
    QStringList tags;
    for (auto &btn : sclSelected->findChildren<QPushButton*>()) {
        if (btn->isChecked()) {
            tags.append(btn->text());
        }
    }
    return tags;
}

void TagsEdit::deselectTag(const QString &tag)
{
    auto tagButton = sclSelected->findChild<QPushButton*>(tag);
    if (!tagButton)
        return;

    laySelected->removeWidget(tagButton);
    delete tagButton;
}

void TagsEdit::selectTag(const QString &tag)
{
    if (isTagSelected(tag))
        return;

    auto tagButton = new TagButton(tag, sclSelected);
    connect(tagButton, &QPushButton::clicked, [=](void){this->deselectTag(tag);});
    laySelected->addWidget(tagButton);

    cbxPicker->setCurrentIndex(-1);
}

void TagsEdit::keyPressEvent(QKeyEvent *evt)
{
    if ( (evt->key()==Qt::Key_Enter) || (evt->key()==Qt::Key_Return) ) {
        evt->accept();
    }
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
