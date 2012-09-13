/********************************************************************************
** Form generated from reading UI file 'settings.ui'
**
** Created: Thu 13. Sep 07:58:19 2012
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UrlPreviewSettingsForm
{
public:
    QFormLayout *formLayout_2;
    QLabel *label_2;
    QSpinBox *maxFileSize;
    QLabel *label_3;
    QSpinBox *maxHeight;
    QLabel *label_5;
    QSpinBox *maxWidth;
    QCheckBox *imagesPreview;
    QCheckBox *youtubePreview;
    QCheckBox *HTML5Audio;
    QCheckBox *HTML5Video;
    QCheckBox *sitePreview;

    void setupUi(QWidget *UrlPreviewSettingsForm)
    {
        if (UrlPreviewSettingsForm->objectName().isEmpty())
            UrlPreviewSettingsForm->setObjectName(QString::fromUtf8("UrlPreviewSettingsForm"));
        UrlPreviewSettingsForm->resize(250, 223);
        formLayout_2 = new QFormLayout(UrlPreviewSettingsForm);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        label_2 = new QLabel(UrlPreviewSettingsForm);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setEnabled(true);

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_2);

        maxFileSize = new QSpinBox(UrlPreviewSettingsForm);
        maxFileSize->setObjectName(QString::fromUtf8("maxFileSize"));
        maxFileSize->setMinimum(100000);
        maxFileSize->setMaximum(100000000);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, maxFileSize);

        label_3 = new QLabel(UrlPreviewSettingsForm);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_3);

        maxHeight = new QSpinBox(UrlPreviewSettingsForm);
        maxHeight->setObjectName(QString::fromUtf8("maxHeight"));
        maxHeight->setMinimum(0);
        maxHeight->setMaximum(4096);
        maxHeight->setValue(600);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, maxHeight);

        label_5 = new QLabel(UrlPreviewSettingsForm);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_5);

        maxWidth = new QSpinBox(UrlPreviewSettingsForm);
        maxWidth->setObjectName(QString::fromUtf8("maxWidth"));
        maxWidth->setMinimum(0);
        maxWidth->setMaximum(4096);
        maxWidth->setValue(800);

        formLayout_2->setWidget(2, QFormLayout::FieldRole, maxWidth);

        imagesPreview = new QCheckBox(UrlPreviewSettingsForm);
        imagesPreview->setObjectName(QString::fromUtf8("imagesPreview"));
        imagesPreview->setChecked(true);

        formLayout_2->setWidget(3, QFormLayout::FieldRole, imagesPreview);

        youtubePreview = new QCheckBox(UrlPreviewSettingsForm);
        youtubePreview->setObjectName(QString::fromUtf8("youtubePreview"));
        youtubePreview->setChecked(true);

        formLayout_2->setWidget(4, QFormLayout::FieldRole, youtubePreview);

        HTML5Audio = new QCheckBox(UrlPreviewSettingsForm);
        HTML5Audio->setObjectName(QString::fromUtf8("HTML5Audio"));
        HTML5Audio->setChecked(true);

        formLayout_2->setWidget(6, QFormLayout::FieldRole, HTML5Audio);

        HTML5Video = new QCheckBox(UrlPreviewSettingsForm);
        HTML5Video->setObjectName(QString::fromUtf8("HTML5Video"));
        HTML5Video->setChecked(true);

        formLayout_2->setWidget(7, QFormLayout::FieldRole, HTML5Video);

        sitePreview = new QCheckBox(UrlPreviewSettingsForm);
        sitePreview->setObjectName(QString::fromUtf8("sitePreview"));
        sitePreview->setChecked(true);

        formLayout_2->setWidget(5, QFormLayout::FieldRole, sitePreview);


        retranslateUi(UrlPreviewSettingsForm);

        QMetaObject::connectSlotsByName(UrlPreviewSettingsForm);
    } // setupUi

    void retranslateUi(QWidget *UrlPreviewSettingsForm)
    {
        UrlPreviewSettingsForm->setWindowTitle(QApplication::translate("UrlPreviewSettingsForm", "Form", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("UrlPreviewSettingsForm", "Max filesize", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        maxFileSize->setToolTip(QApplication::translate("UrlPreviewSettingsForm", "Max picture size to preview ", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        maxFileSize->setSuffix(QApplication::translate("UrlPreviewSettingsForm", " bytes", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("UrlPreviewSettingsForm", "Height", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        maxHeight->setToolTip(QApplication::translate("UrlPreviewSettingsForm", "Max height of images preview.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        maxHeight->setSuffix(QApplication::translate("UrlPreviewSettingsForm", "px", 0, QApplication::UnicodeUTF8));
        maxHeight->setPrefix(QString());
        label_5->setText(QApplication::translate("UrlPreviewSettingsForm", "Width", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        maxWidth->setToolTip(QApplication::translate("UrlPreviewSettingsForm", "Max width of images preview.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        maxWidth->setSuffix(QApplication::translate("UrlPreviewSettingsForm", "px", 0, QApplication::UnicodeUTF8));
        maxWidth->setPrefix(QString());
        imagesPreview->setText(QApplication::translate("UrlPreviewSettingsForm", "Images preview", 0, QApplication::UnicodeUTF8));
        youtubePreview->setText(QApplication::translate("UrlPreviewSettingsForm", "YouTube preview", 0, QApplication::UnicodeUTF8));
        HTML5Audio->setText(QApplication::translate("UrlPreviewSettingsForm", "HTML5 Audio", 0, QApplication::UnicodeUTF8));
        HTML5Video->setText(QApplication::translate("UrlPreviewSettingsForm", "HTML5 Video", 0, QApplication::UnicodeUTF8));
        sitePreview->setText(QApplication::translate("UrlPreviewSettingsForm", "Site preview", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class UrlPreviewSettingsForm: public Ui_UrlPreviewSettingsForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_H
