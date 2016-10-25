/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Weiqi Shi (weiqi.shi@yale.edu)

 - License:  GNU General Public License Usage
   Alternatively, this file may be used under the terms of the GNU General
   Public License version 3.0 as published by the Free Software Foundation
   and appearing in the file LICENSE.GPL included in the packaging of this
   file. Please review the following information to ensure the GNU General
   Public License version 3.0 requirements will be met:
   http://www.gnu.org/copyleft/gpl.html.

 - Warranty: This software is distributed WITHOUT ANY WARRANTY; without even
   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
   PURPOSE.

 - Acknowledgments: Some portions of this file are based on the example codes
   of ITK/VTK library from Kitware, QT API from Nokia. I would like to thank
   anonymous help from various software engineering communities.

   This file defines the different classes of bookmark that the user may work
   with (corresponding to the different file types that may be displayed).

*****************************************************************************/
#include "saveProjectAsDialog.h"

#define SIZE (1024)

SaveProjectAsDialog::SaveProjectAsDialog(const QString user, const QString keyword, const QString affiliation, const QString description, const QString path)
{
	mDialog = new QDialog();
	mVbox = new QVBoxLayout();

	mLastPath = path;

	mDialog->setWindowTitle(tr("Save Project As.."));

	mUserNameLabel = new QLabel(tr("User Name:"));
	mUserNameLineEdit = new QLineEdit();
	mUserNameLineEdit->setFixedWidth(350);
	mUserNameLineEdit->setText(user);
    mUserNameLabel->setBuddy(mUserNameLineEdit);
	mUserNameHBox = new QHBoxLayout();
	mUserNameHBox->addWidget(mUserNameLabel);
	mUserNameHBox->addWidget(mUserNameLineEdit, 0 , Qt::AlignRight);

    mProjectNameLabel = new QLabel(tr("Name:  "));
	mProjectNameEdit = new QLineEdit();
	mProjectNameEdit->setFixedWidth(350);
    mProjectNameLabel->setBuddy(mProjectNameEdit);
	connect(mProjectNameEdit, SIGNAL(textChanged(QString)), this, SLOT(projectNameChanged(QString)));
	mProjectHBox = new QHBoxLayout();
	mProjectHBox->addWidget(mProjectNameLabel, 0 , Qt::AlignLeft);
    mProjectHBox->addWidget(mProjectNameEdit, 0 , Qt::AlignRight);
	
    mLocationLabel = new QLabel(tr("Location:"));
	mLocationLineEdit = new QLineEdit();
    mLocationLabel->setBuddy(mLocationLineEdit);
	connect(mLocationLineEdit, SIGNAL(textChanged(QString)), this, SLOT(projectPathChanged(QString)));
	mLocationLineEdit->setText(QCoreApplication::applicationDirPath());
	mProjectPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
	mLocationLineEdit->setFixedWidth(270);
	mLocationBrowseButton = new QPushButton("Browse");
	connect(mLocationBrowseButton, SIGNAL(clicked()), this, SLOT(locationBrowse()));
	mLocationHBox = new QHBoxLayout();
	mLocationHBox->addWidget(mLocationLabel);
	mLocationHBox->addWidget(mLocationLineEdit, 0 , Qt::AlignRight);
    mLocationHBox->addWidget(mLocationBrowseButton);

	mKeywordLabel = new QLabel(tr("Keywords:"));
	mKeywordLineEdit = new QLineEdit();
	mKeywordLineEdit->setFixedWidth(350);
	mKeywordLineEdit->setText(keyword);
    mKeywordLabel->setBuddy(mKeywordLineEdit);
	mKeywordHBox = new QHBoxLayout();
	mKeywordHBox->addWidget(mKeywordLabel);
    mKeywordHBox->addWidget(mKeywordLineEdit, 0 , Qt::AlignRight);

	mAffiliationLabel = new QLabel(tr("Affiliation:"));
	mAffiliationLineEdit = new QLineEdit();
	mAffiliationLineEdit->setFixedWidth(350);
	mAffiliationLineEdit->setText(affiliation);
    mAffiliationLabel->setBuddy(mAffiliationLineEdit);
	mAffiliationHBox = new QHBoxLayout();
	mAffiliationHBox->addWidget(mAffiliationLabel);
    mAffiliationHBox->addWidget(mAffiliationLineEdit, 0 , Qt::AlignRight);

	mDescriptionLabel = new QLabel(tr("Description:"));
	mDescriptionEdit = new QTextEdit(mDialog);
	mDescriptionEdit->setText(description);

	mButtonGridBox = new QGridLayout();
	mNextButton = new QPushButton("Next");
	mNextButton->setEnabled(false);
	connect(mNextButton, SIGNAL(clicked()), this, SLOT(next()));
	mCancelButton = new QPushButton("Cancel");
	connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	mButtonGridBox->addWidget(mNextButton, 0, 5, 1, 4,  Qt::AlignVCenter | Qt::AlignRight);
	mButtonGridBox->addWidget(mCancelButton, 0, 5, 1, 5, Qt::AlignVCenter | Qt::AlignRight);
	

	mVbox->addLayout(mProjectHBox);
	mVbox->addLayout(mLocationHBox);
	mVbox->addLayout(mUserNameHBox);
	mVbox->addLayout(mKeywordHBox);
	mVbox->addLayout(mAffiliationHBox);
	mVbox->addWidget(mDescriptionLabel);
	mVbox->addWidget(mDescriptionEdit);
	mVbox->addLayout(mButtonGridBox);
	
	connect(this, SIGNAL(infoUpdate()), this, SLOT(enableNextButton()));
	mDialog->setMinimumWidth(350);
	mDialog->setLayout(mVbox);
	isOk = false;
}

void SaveProjectAsDialog::locationBrowse()
{
	
	mProjectPath = QFileDialog::getExistingDirectory(this, tr("Location"), mLastPath,
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);
	mLocationLineEdit->setText(mProjectPath);
	mLocationLineEdit->displayText();
}


void SaveProjectAsDialog::next()
{
	mFullPath = mProjectPath.simplified();
	if (!QDir().exists(mProjectPath))
	{
		qDebug()<<"the path do not exist";
		QString notice;
		notice.append(QString("Error!\nThe directory ") + mProjectPath + QString(" does not exist!"));
		//mError = new ErrorDialog(notice);
		QMessageBox::critical(this, tr("Object Opening Error"), "Unable to open " + mProjectPath + ".");
		return;
	}
	if (mFullPath.split(QDir::separator())[1] == QString())
	{
		mFullPath.append(mProjectName);
	}
	else
	{
		mFullPath.append(QDir::separator() + mProjectName);
	}
	if (QDir().exists(mFullPath))
	{
		QString message("The project with specified name already exists in the specified location!\n");
		message.append(QString("Do you want to overwrite the existing project with this new project?"));

		QMessageBox::StandardButton reply;
		reply = QMessageBox::warning(this, tr("WARNING"), message, QMessageBox::Ok|QMessageBox::No);
		if (reply == QMessageBox::Ok) 
		{
			nextReady();
		}
		else 
		{
			nextCancel();
		}
	}
	else
	{
		nextReady();
	}
}

void SaveProjectAsDialog::nextReady()
{
	isOk = true;
	mDialog->hide();
	
}

void SaveProjectAsDialog::nextCancel()
{
	mProjectNameEdit->clear();
}


void SaveProjectAsDialog::cancel()
{
	mDialog->hide();
}

void SaveProjectAsDialog::projectNameChanged(QString name)
{
	mProjectName = name;
	emit infoUpdate();
}

void SaveProjectAsDialog::projectPathChanged(QString path)
{
	mProjectPath = path;
	emit infoUpdate();
}

void SaveProjectAsDialog::enableNextButton()
{
	if (!mProjectPath.isEmpty() && !mProjectName.isEmpty())
	{
		mNextButton->setEnabled(true);
	}
	else
	{
		mNextButton->setEnabled(false);
	}
}
