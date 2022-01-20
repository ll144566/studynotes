#pragma once

#include <QtWidgets/QWidget>
#include "ui_AudioPlay2.h"

class AudioPlay2 : public QWidget
{
    Q_OBJECT

public:
    AudioPlay2(QWidget *parent = Q_NULLPTR);

    Ui::AudioPlay2Class ui;
};
