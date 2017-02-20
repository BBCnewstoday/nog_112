// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <noggit/Environment.h>
#include <noggit/Misc.h>
#include <noggit/ModelInstance.h>
#include <noggit/application.h> // fonts
#include <noggit/Settings.h>
#include <noggit/ui/Button.h>
#include <noggit/ui/CheckBox.h>
#include <noggit/ui/MapViewGUI.h>
#include <noggit/ui/ModelImport.h>
#include <noggit/ui/ObjectEditor.h>
#include <noggit/ui/RotationEditor.h>
#include <noggit/ui/StatusBar.h>
#include <noggit/ui/TextBox.h>
#include <noggit/ui/Text.h>
#include <noggit/Video.h> // video
#include <noggit/WMOInstance.h> // WMOInstance
#include <noggit/World.h>

#include <QGridLayout>
#include <QGroupBox>
#include <QCheckbox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

void updateMinRotation(UITextBox::Ptr textBox, const std::string& value)
{
  float v = std::max(0.0f, (float)std::atof(value.c_str()));
  v = std::min(v, Environment::getInstance()->maxRotation);

  Environment::getInstance()->minRotation = v;
  textBox->value(misc::floatToStr(v));
}

void updateMaxRotation(UITextBox::Ptr textBox, const std::string& value)
{
  float v = std::min(360.0f, (float)std::atof(value.c_str()));
  v = std::max(v, Environment::getInstance()->minRotation);

  Environment::getInstance()->maxRotation = v;
  textBox->value(misc::floatToStr(v));
}

void updateMinTilt(UITextBox::Ptr textBox, const std::string& value)
{
  float v = std::max(-180.0f, (float)std::atof(value.c_str()));
  v = std::min(v, Environment::getInstance()->maxTilt);

  Environment::getInstance()->minTilt = v;
  textBox->value(misc::floatToStr(v));
}

void updateMaxTilt(UITextBox::Ptr textBox, const std::string& value)
{
  float v = std::min(180.0f, (float)std::atof(value.c_str()));
  v = std::max(v, Environment::getInstance()->minTilt);

  Environment::getInstance()->maxTilt = v;
  textBox->value(misc::floatToStr(v));
}

void updateMinScale(UITextBox::Ptr textBox, const std::string& value)
{
  float v = std::max(0.01f, (float)std::atof(value.c_str()));
  v = std::min(v, Environment::getInstance()->maxScale);

  Environment::getInstance()->minScale = v;
  textBox->value(misc::floatToStr(v));
}

void updateMaxScale(UITextBox::Ptr textBox, const std::string& value)
{
  float v = std::min(63.0f, (float)std::atof(value.c_str()));
  v = std::max(v, Environment::getInstance()->minScale);

  Environment::getInstance()->maxScale = v;
  textBox->value(misc::floatToStr(v));
}

void UIObjectEditor::toggleRotationEditor()
{
  mainGui->rotationEditor->setVisible(!mainGui->rotationEditor->isVisible());
}

void UIObjectEditor::SaveObjecttoTXT()
{
  if (!gWorld->HasSelection())
    return;
  std::string path;

  if (gWorld->IsSelection(eEntry_WMO))
  {
    path = boost::get<selected_wmo_type> (*gWorld->GetCurrentSelection())->wmo->filename();
  }
  else if (gWorld->IsSelection(eEntry_Model))
  {
    path = boost::get<selected_model_type> (*gWorld->GetCurrentSelection())->model->_filename;
  }

  std::ofstream stream(Settings::getInstance()->importFile, std::ios_base::app);
  stream << path << std::endl;
  stream.close();
    
  modelImport->buildModelList();
}

UIObjectEditor::UIObjectEditor(float x, float y, UIMapViewGUI* mainGui)
   : QDockWidget("Object Editor", nullptr, nullptr)
   , selected()
   , pasteMode(PASTE_ON_TERRAIN)
{
  filename = new UIStatusBar(0.0f, (float)video.yres() - 60.0f, (float)video.xres(), 30.0f);
  filename->hide();
  this->mainGui = mainGui;
  mainGui->addChild(filename);
  
  setFloating(true);
  QWidget *content = new QWidget(nullptr);
  auto inspectorGrid = new QGridLayout (content);

  Environment* env = Environment::getInstance();

  QGroupBox *copyBox = new QGroupBox(content);
    auto copyGrid = new QGridLayout (copyBox);
    QCheckBox *randRotCheck = new QCheckBox("Random rotation", content);
    QCheckBox *randTiltCheck = new QCheckBox("Random tilt", content);
    QCheckBox *randScaleCheck = new QCheckBox("Random scale", content);
    QCheckBox *copyAttributesCheck = new QCheckBox("Copy rotation, tilt, and scale", content);
    
    QDoubleSpinBox *rotRangeStart = new QDoubleSpinBox(content);
    QDoubleSpinBox *rotRangeEnd = new QDoubleSpinBox(content);
    QDoubleSpinBox *tiltRangeStart = new QDoubleSpinBox(content);
    QDoubleSpinBox *tiltRangeEnd = new QDoubleSpinBox(content);
    QDoubleSpinBox *scaleRangeStart = new QDoubleSpinBox(content);
    QDoubleSpinBox *scaleRangeEnd = new QDoubleSpinBox(content);
    
    QLabel *minLabel = new QLabel("Min", content);
    QLabel *maxLabel = new QLabel("Max", content);
    
    rotRangeStart->setMaximumWidth(85);
    rotRangeEnd->setMaximumWidth(85);
    tiltRangeStart->setMaximumWidth(85);
    tiltRangeEnd->setMaximumWidth(85);
    scaleRangeStart->setMaximumWidth(85);
    scaleRangeEnd->setMaximumWidth(85);
    
    rotRangeStart->setDecimals(3);
    rotRangeEnd->setDecimals(3);
    tiltRangeStart->setDecimals(3);
    tiltRangeEnd->setDecimals(3);
    scaleRangeStart->setDecimals(3);
    scaleRangeEnd->setDecimals(3);
    
    rotRangeStart->setRange (-180.f, 180.f);
    rotRangeEnd->setRange (-180.f, 180.f);
    tiltRangeStart->setRange (-180.f, 180.f);
    tiltRangeEnd->setRange (-180.f, 180.f);
    scaleRangeStart->setRange (-180.f, 180.f);
    scaleRangeEnd->setRange (-180.f, 180.f);
    
    copyBox->setTitle("Copy Options");
    copyGrid->addWidget(minLabel, 0, 1, 1, 1, Qt::AlignCenter);
    copyGrid->addWidget(maxLabel, 0, 2, 1, 1, Qt::AlignCenter);
    
    copyGrid->addWidget(randRotCheck, 1, 0, 1, 1);
    copyGrid->addWidget(rotRangeStart, 1, 1, 1, 1);
    copyGrid->addWidget(rotRangeEnd, 1, 2, 1, 1);
    copyGrid->addWidget(randTiltCheck, 3, 0, 1, 1);
    copyGrid->addWidget(tiltRangeStart, 3, 1, 1, 1);
    copyGrid->addWidget(tiltRangeEnd, 3, 2, 1, 1);
    copyGrid->addWidget(randScaleCheck, 4, 0, 1, 1);
    copyGrid->addWidget(scaleRangeStart, 4, 1, 1, 1);
    copyGrid->addWidget(scaleRangeEnd, 4, 2, 1, 1);
    copyGrid->addWidget(copyAttributesCheck, 5, 0, 1, 3);
    
    QGroupBox *pasteBox = new QGroupBox(content);
    auto pasteGrid = new QGridLayout (pasteBox);
    QRadioButton *terrainButton = new QRadioButton("Terrain");
    QRadioButton *selectionButton = new QRadioButton("Selection");
    QRadioButton *cameraButton = new QRadioButton("Camera");
    
    auto pasteModeGroup(new QButtonGroup(content));
    pasteModeGroup->addButton(terrainButton, 0);
    pasteModeGroup->addButton(selectionButton, 1);
    pasteModeGroup->addButton(cameraButton, 2);
    
    QCheckBox *cursorPosCheck = new QCheckBox("Move model to cursor position", content);
    
    pasteBox->setTitle("Paste Options");
    pasteGrid->addWidget(terrainButton);
    pasteGrid->addWidget(selectionButton, 0, 1, 1, 1);
    pasteGrid->addWidget(cameraButton, 0, 2, 1, 1);
    pasteGrid->addWidget(cursorPosCheck, 1, 0, 1, 3);
    
    QPushButton *rotEditorButton = new QPushButton("Rotation Editor", content);
    QPushButton *visToggleButton = new QPushButton("Toggle Visibility", content);
    QPushButton *clearListButton = new QPushButton("Clear List", content);
    
    QGroupBox *importBox = new QGroupBox(content);
    new QGridLayout (importBox);
    importBox->setTitle("Import");
    
    QPushButton *toTxt = new QPushButton("To Text File", content);
    QPushButton *fromTxt = new QPushButton("From Text File", content);
    importBox->layout()->addWidget(toTxt);
    importBox->layout()->addWidget(fromTxt);
    
    inspectorGrid->addWidget(copyBox, 0, 0, 1, 2);
    inspectorGrid->addWidget(pasteBox, 1, 0, 1, 2);
    inspectorGrid->addWidget(rotEditorButton, 2, 0, 1, 1);
    inspectorGrid->addWidget(visToggleButton, 3, 0, 1, 1);
    inspectorGrid->addWidget(clearListButton, 4, 0, 1, 1);
    inspectorGrid->addWidget(importBox, 2, 1, 3, 1);
    
    setWidget(content);
    
    randRotCheck->setChecked(Settings::getInstance()->random_rotation);
    connect (copyAttributesCheck, &QCheckBox::stateChanged, [] (int s)
             { Settings::getInstance()->random_rotation = s; });
    
    rotRangeStart->setValue(env->minRotation);
    rotRangeEnd->setValue(env->maxRotation);
    
    tiltRangeStart->setValue(env->minTilt);
    tiltRangeEnd->setValue(env->maxTilt);
    
    scaleRangeStart->setValue(env->minScale);
    scaleRangeEnd->setValue(env->maxScale);
    
    connect ( rotRangeStart, static_cast<void (QDoubleSpinBox::*) (double)> (&QDoubleSpinBox::valueChanged)
             , [&] (double v)
             {
                 env->minRotation = v;
             }
             );
    
    connect ( rotRangeEnd, static_cast<void (QDoubleSpinBox::*) (double)> (&QDoubleSpinBox::valueChanged)
             , [&] (double v)
             {
                 env->maxRotation = v;
             }
             );
    
    connect ( tiltRangeStart, static_cast<void (QDoubleSpinBox::*) (double)> (&QDoubleSpinBox::valueChanged)
             , [&] (double v)
             {
                 env->minTilt = v;
             }
             );
    
    connect ( tiltRangeEnd, static_cast<void (QDoubleSpinBox::*) (double)> (&QDoubleSpinBox::valueChanged)
             , [&] (double v)
             {
                 env->maxTilt = v;
             }
             );
    
    connect ( scaleRangeStart, static_cast<void (QDoubleSpinBox::*) (double)> (&QDoubleSpinBox::valueChanged)
             , [&] (double v)
             {
                 env->minScale = v;
             }
             );
    
    connect ( scaleRangeEnd, static_cast<void (QDoubleSpinBox::*) (double)> (&QDoubleSpinBox::valueChanged)
             , [&] (double v)
             {
                 env->maxScale = v;
             }
             );
    
    randTiltCheck->setChecked(Settings::getInstance()->random_tilt);
    connect (copyAttributesCheck, &QCheckBox::stateChanged, [] (int s)
             { Settings::getInstance()->random_tilt = s; });
    
    randScaleCheck->setChecked(Settings::getInstance()->random_tilt);
    connect (copyAttributesCheck, &QCheckBox::stateChanged, [] (int s)
             { Settings::getInstance()->random_size = s; });
    
    copyAttributesCheck->setChecked(Settings::getInstance()->copyModelStats);
    connect (copyAttributesCheck, &QCheckBox::stateChanged, [] (int s)
             { Settings::getInstance()->copyModelStats = s; });
    
    pasteModeGroup->button(pasteMode)->setChecked(true);
    
    connect ( pasteModeGroup, static_cast<void (QButtonGroup::*) (int)> (&QButtonGroup::buttonClicked)
             , [&] (int id)
             {
                 pasteMode = id;
             }
             );
    
    cursorPosCheck->setChecked(Environment::getInstance()->moveModelToCursorPos);
    connect (copyAttributesCheck, &QCheckBox::stateChanged, [=] (int s)
             { env->moveModelToCursorPos = s; });
    
    connect(rotEditorButton, &QPushButton::clicked, [=]() {
        toggleRotationEditor();
    });
    
    connect(visToggleButton, &QPushButton::clicked, [=]() {
        mainGui->theMapview->_draw_hidden_models
            = !mainGui->theMapview->_draw_hidden_models;
    });
    
    connect(clearListButton, &QPushButton::clicked, [=]() {
        mainGui->theMapview->_hidden_map_objects.clear();
        mainGui->theMapview->_hidden_models.clear();
    });
    
    connect(toTxt, &QPushButton::clicked, [=]() {
        SaveObjecttoTXT();
    });
    
    connect(fromTxt, &QPushButton::clicked, [=]() {
        showImportModels();
    });
}

void UIObjectEditor::showImportModels()
{
    modelImport->show();
}

void UIObjectEditor::pasteObject (math::vector_3d pos)
{
  if (!hasSelection() || selected->which() == eEntry_MapChunk)
  {
    return;
  }

  switch (pasteMode)
  {
    case PASTE_ON_TERRAIN: // use cursor pos
      break;
    case PASTE_ON_SELECTION:
      if (gWorld->HasSelection())
      {
        auto selection = *gWorld->GetCurrentSelection();
        if (selection.which() == eEntry_Model)
        {
          pos = boost::get<selected_model_type> (selection)->pos;
        }
        else if (selection.which() == eEntry_WMO)
        {
          pos = boost::get<selected_wmo_type> (selection)->pos;
        }
      } // else: use cursor pos
      break;
    case PASTE_ON_CAMERA:
      pos = gWorld->camera;
      break;
    default:
      LogDebug << "UIObjectEditor::pasteObject: Unknown pasteMode " << pasteMode << std::endl;
      break;
  }

  gWorld->addModel(selected.get(), pos, true);
}

void UIObjectEditor::togglePasteMode()
{
  pasteModeGroup->Activate((pasteMode + 1) % PASTE_MODE_COUNT);
}

bool UIObjectEditor::hasSelection() const
{
  return !!selected;
}

void UIObjectEditor::copy(selection_type entry)
{
  if (entry.which() == eEntry_Model)
  {
    auto clone = new ModelInstance(boost::get<selected_model_type> (entry)->model->_filename);
    clone->sc = boost::get<selected_model_type> (entry)->sc;
    clone->dir = boost::get<selected_model_type> (entry)->dir;
    clone->ldir = boost::get<selected_model_type> (entry)->ldir;
    selected = clone;
    setModelName (boost::get<selected_model_type> (entry)->model->_filename);
  }
  else if (entry.which() == eEntry_WMO)
  {
    auto clone = new WMOInstance(boost::get<selected_wmo_type> (entry)->wmo->_filename);
    clone->dir = boost::get<selected_wmo_type> (entry)->dir;
    selected = clone;
    setModelName(boost::get<selected_wmo_type> (entry)->wmo->_filename);
  }
  else
  {
    Environment::getInstance()->clear_clipboard();
    return;
  }

  Environment::getInstance()->set_clipboard(selected);
}

void UIObjectEditor::setModelName(const std::string &name)
{
  std::stringstream ss;
  ss << "Model: " << name;
  filename->setLeftInfo(ss.str());
}
