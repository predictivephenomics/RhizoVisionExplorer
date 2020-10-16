/****************************************************************************
** Meta object code from reading C++ file 'MainUI.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MainUI.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainUI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CheckEnabler_t {
    QByteArrayData data[4];
    char stringdata0[39];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CheckEnabler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CheckEnabler_t qt_meta_stringdata_CheckEnabler = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CheckEnabler"
QT_MOC_LITERAL(1, 13, 12), // "stateChanged"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 11) // "textChanged"

    },
    "CheckEnabler\0stateChanged\0\0textChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CheckEnabler[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,
       3,    0,   25,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CheckEnabler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CheckEnabler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stateChanged(); break;
        case 1: _t->textChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CheckEnabler::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CheckEnabler::stateChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CheckEnabler::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CheckEnabler::textChanged)) {
                *result = 1;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject CheckEnabler::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_CheckEnabler.data,
    qt_meta_data_CheckEnabler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CheckEnabler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CheckEnabler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CheckEnabler.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CheckEnabler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void CheckEnabler::stateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void CheckEnabler::textChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
struct qt_meta_stringdata_StringCheckEnabler_t {
    QByteArrayData data[4];
    char stringdata0[45];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_StringCheckEnabler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_StringCheckEnabler_t qt_meta_stringdata_StringCheckEnabler = {
    {
QT_MOC_LITERAL(0, 0, 18), // "StringCheckEnabler"
QT_MOC_LITERAL(1, 19, 12), // "stateChanged"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 11) // "textChanged"

    },
    "StringCheckEnabler\0stateChanged\0\0"
    "textChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_StringCheckEnabler[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,
       3,    0,   25,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void StringCheckEnabler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<StringCheckEnabler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stateChanged(); break;
        case 1: _t->textChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (StringCheckEnabler::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&StringCheckEnabler::stateChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (StringCheckEnabler::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&StringCheckEnabler::textChanged)) {
                *result = 1;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject StringCheckEnabler::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_StringCheckEnabler.data,
    qt_meta_data_StringCheckEnabler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *StringCheckEnabler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *StringCheckEnabler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_StringCheckEnabler.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int StringCheckEnabler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void StringCheckEnabler::stateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void StringCheckEnabler::textChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
struct qt_meta_stringdata_MainUI_t {
    QByteArrayData data[48];
    char stringdata0[569];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainUI_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainUI_t qt_meta_stringdata_MainUI = {
    {
QT_MOC_LITERAL(0, 0, 6), // "MainUI"
QT_MOC_LITERAL(1, 7, 18), // "updateVisualOutput"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 7), // "cv::Mat"
QT_MOC_LITERAL(4, 35, 1), // "m"
QT_MOC_LITERAL(5, 37, 14), // "updateProgress"
QT_MOC_LITERAL(6, 52, 6), // "status"
QT_MOC_LITERAL(7, 59, 7), // "getName"
QT_MOC_LITERAL(8, 67, 11), // "std::string"
QT_MOC_LITERAL(9, 79, 18), // "getApplicationType"
QT_MOC_LITERAL(10, 98, 7), // "AppType"
QT_MOC_LITERAL(11, 106, 13), // "getOutputType"
QT_MOC_LITERAL(12, 120, 10), // "OutputType"
QT_MOC_LITERAL(13, 131, 16), // "getAlgorithmName"
QT_MOC_LITERAL(14, 148, 13), // "getParameters"
QT_MOC_LITERAL(15, 162, 28), // "std::vector<IParameterInfo*>"
QT_MOC_LITERAL(16, 191, 11), // "getPluginUI"
QT_MOC_LITERAL(17, 203, 8), // "QWidget*"
QT_MOC_LITERAL(18, 212, 8), // "setImage"
QT_MOC_LITERAL(19, 221, 5), // "image"
QT_MOC_LITERAL(20, 227, 9), // "imagename"
QT_MOC_LITERAL(21, 237, 7), // "execute"
QT_MOC_LITERAL(22, 245, 15), // "getOutputImages"
QT_MOC_LITERAL(23, 261, 20), // "std::vector<cv::Mat>"
QT_MOC_LITERAL(24, 282, 15), // "getDisplayIndex"
QT_MOC_LITERAL(25, 298, 15), // "setDisplayIndex"
QT_MOC_LITERAL(26, 314, 3), // "idx"
QT_MOC_LITERAL(27, 318, 11), // "getFeatures"
QT_MOC_LITERAL(28, 330, 19), // "std::vector<double>"
QT_MOC_LITERAL(29, 350, 6), // "roiidx"
QT_MOC_LITERAL(30, 357, 12), // "setBatchMode"
QT_MOC_LITERAL(31, 370, 2), // "on"
QT_MOC_LITERAL(32, 373, 10), // "saveOutput"
QT_MOC_LITERAL(33, 384, 6), // "savloc"
QT_MOC_LITERAL(34, 391, 13), // "inputfilename"
QT_MOC_LITERAL(35, 405, 12), // "saveMetadata"
QT_MOC_LITERAL(36, 418, 6), // "imgloc"
QT_MOC_LITERAL(37, 425, 8), // "filepath"
QT_MOC_LITERAL(38, 434, 12), // "loadMetadata"
QT_MOC_LITERAL(39, 447, 11), // "writeHeader"
QT_MOC_LITERAL(40, 459, 8), // "filename"
QT_MOC_LITERAL(41, 468, 17), // "getCSVColumnNames"
QT_MOC_LITERAL(42, 486, 20), // "std::vector<QString>"
QT_MOC_LITERAL(43, 507, 16), // "getProgressSteps"
QT_MOC_LITERAL(44, 524, 5), // "abort"
QT_MOC_LITERAL(45, 530, 8), // "getChart"
QT_MOC_LITERAL(46, 539, 7), // "QChart*"
QT_MOC_LITERAL(47, 547, 21) // "updatesegmentedoutput"

    },
    "MainUI\0updateVisualOutput\0\0cv::Mat\0m\0"
    "updateProgress\0status\0getName\0std::string\0"
    "getApplicationType\0AppType\0getOutputType\0"
    "OutputType\0getAlgorithmName\0getParameters\0"
    "std::vector<IParameterInfo*>\0getPluginUI\0"
    "QWidget*\0setImage\0image\0imagename\0"
    "execute\0getOutputImages\0std::vector<cv::Mat>\0"
    "getDisplayIndex\0setDisplayIndex\0idx\0"
    "getFeatures\0std::vector<double>\0roiidx\0"
    "setBatchMode\0on\0saveOutput\0savloc\0"
    "inputfilename\0saveMetadata\0imgloc\0"
    "filepath\0loadMetadata\0writeHeader\0"
    "filename\0getCSVColumnNames\0"
    "std::vector<QString>\0getProgressSteps\0"
    "abort\0getChart\0QChart*\0updatesegmentedoutput"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainUI[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      28,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  154,    2, 0x06 /* Public */,
       5,    1,  157,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,  160,    2, 0x0a /* Public */,
       9,    0,  161,    2, 0x0a /* Public */,
      11,    0,  162,    2, 0x0a /* Public */,
      13,    0,  163,    2, 0x0a /* Public */,
      14,    0,  164,    2, 0x0a /* Public */,
      16,    0,  165,    2, 0x0a /* Public */,
      18,    2,  166,    2, 0x0a /* Public */,
      18,    1,  171,    2, 0x2a /* Public | MethodCloned */,
      21,    0,  174,    2, 0x0a /* Public */,
      22,    0,  175,    2, 0x0a /* Public */,
      24,    0,  176,    2, 0x0a /* Public */,
      25,    1,  177,    2, 0x0a /* Public */,
      27,    0,  180,    2, 0x0a /* Public */,
      27,    1,  181,    2, 0x0a /* Public */,
      30,    1,  184,    2, 0x0a /* Public */,
      32,    2,  187,    2, 0x0a /* Public */,
      35,    3,  192,    2, 0x0a /* Public */,
      35,    2,  199,    2, 0x2a /* Public | MethodCloned */,
      38,    1,  204,    2, 0x0a /* Public */,
      39,    2,  207,    2, 0x0a /* Public */,
      39,    1,  212,    2, 0x2a /* Public | MethodCloned */,
      41,    0,  215,    2, 0x0a /* Public */,
      43,    0,  216,    2, 0x0a /* Public */,
      44,    0,  217,    2, 0x0a /* Public */,
      45,    0,  218,    2, 0x0a /* Public */,
      47,    0,  219,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,

 // slots: parameters
    0x80000000 | 8,
    0x80000000 | 10,
    0x80000000 | 12,
    0x80000000 | 8,
    0x80000000 | 15,
    0x80000000 | 17,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,   19,   20,
    QMetaType::Void, 0x80000000 | 3,   19,
    QMetaType::Void,
    0x80000000 | 23,
    QMetaType::Int,
    QMetaType::Void, QMetaType::Int,   26,
    0x80000000 | 28,
    0x80000000 | 28, QMetaType::Int,   29,
    QMetaType::Void, QMetaType::Bool,   31,
    QMetaType::Int, QMetaType::QString, QMetaType::QString,   33,   34,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   36,   33,   37,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   36,   33,
    QMetaType::Void, QMetaType::QString,   37,
    QMetaType::Int, QMetaType::QString, QMetaType::QString,   33,   40,
    QMetaType::Int, QMetaType::QString,   33,
    0x80000000 | 42,
    QMetaType::Int,
    QMetaType::Void,
    0x80000000 | 46,
    QMetaType::Void,

       0        // eod
};

void MainUI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainUI *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateVisualOutput((*reinterpret_cast< cv::Mat(*)>(_a[1]))); break;
        case 1: _t->updateProgress((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: { std::string _r = _t->getName();
            if (_a[0]) *reinterpret_cast< std::string*>(_a[0]) = std::move(_r); }  break;
        case 3: { AppType _r = _t->getApplicationType();
            if (_a[0]) *reinterpret_cast< AppType*>(_a[0]) = std::move(_r); }  break;
        case 4: { OutputType _r = _t->getOutputType();
            if (_a[0]) *reinterpret_cast< OutputType*>(_a[0]) = std::move(_r); }  break;
        case 5: { std::string _r = _t->getAlgorithmName();
            if (_a[0]) *reinterpret_cast< std::string*>(_a[0]) = std::move(_r); }  break;
        case 6: { std::vector<IParameterInfo*> _r = _t->getParameters();
            if (_a[0]) *reinterpret_cast< std::vector<IParameterInfo*>*>(_a[0]) = std::move(_r); }  break;
        case 7: { QWidget* _r = _t->getPluginUI();
            if (_a[0]) *reinterpret_cast< QWidget**>(_a[0]) = std::move(_r); }  break;
        case 8: _t->setImage((*reinterpret_cast< cv::Mat(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 9: _t->setImage((*reinterpret_cast< cv::Mat(*)>(_a[1]))); break;
        case 10: _t->execute(); break;
        case 11: { std::vector<cv::Mat> _r = _t->getOutputImages();
            if (_a[0]) *reinterpret_cast< std::vector<cv::Mat>*>(_a[0]) = std::move(_r); }  break;
        case 12: { int _r = _t->getDisplayIndex();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 13: _t->setDisplayIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: { std::vector<double> _r = _t->getFeatures();
            if (_a[0]) *reinterpret_cast< std::vector<double>*>(_a[0]) = std::move(_r); }  break;
        case 15: { std::vector<double> _r = _t->getFeatures((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< std::vector<double>*>(_a[0]) = std::move(_r); }  break;
        case 16: _t->setBatchMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: { int _r = _t->saveOutput((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 18: _t->saveMetadata((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 19: _t->saveMetadata((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 20: _t->loadMetadata((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 21: { int _r = _t->writeHeader((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 22: { int _r = _t->writeHeader((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 23: { std::vector<QString> _r = _t->getCSVColumnNames();
            if (_a[0]) *reinterpret_cast< std::vector<QString>*>(_a[0]) = std::move(_r); }  break;
        case 24: { int _r = _t->getProgressSteps();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 25: _t->abort(); break;
        case 26: { QChart* _r = _t->getChart();
            if (_a[0]) *reinterpret_cast< QChart**>(_a[0]) = std::move(_r); }  break;
        case 27: _t->updatesegmentedoutput(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainUI::*)(cv::Mat );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainUI::updateVisualOutput)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MainUI::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainUI::updateProgress)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainUI::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_MainUI.data,
    qt_meta_data_MainUI,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainUI::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainUI.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "IPlugin"))
        return static_cast< IPlugin*>(this);
    if (!strcmp(_clname, "org.plugin.ImageProcessing.Segmentation.IPlugin"))
        return static_cast< IPlugin*>(this);
    return QObject::qt_metacast(_clname);
}

int MainUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 28)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 28;
    }
    return _id;
}

// SIGNAL 0
void MainUI::updateVisualOutput(cv::Mat _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainUI::updateProgress(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

QT_PLUGIN_METADATA_SECTION
static constexpr unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', '!',
    // metadata version, Qt version, architectural requirements
    0, QT_VERSION_MAJOR, QT_VERSION_MINOR, qPluginArchRequirements(),
    0xbf, 
    // "IID"
    0x02,  0x78,  0x2f,  'o',  'r',  'g',  '.',  'p', 
    'l',  'u',  'g',  'i',  'n',  '.',  'I',  'm', 
    'a',  'g',  'e',  'P',  'r',  'o',  'c',  'e', 
    's',  's',  'i',  'n',  'g',  '.',  'S',  'e', 
    'g',  'm',  'e',  'n',  't',  'a',  't',  'i', 
    'o',  'n',  '.',  'I',  'P',  'l',  'u',  'g', 
    'i',  'n', 
    // "className"
    0x03,  0x66,  'M',  'a',  'i',  'n',  'U',  'I', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN(MainUI, MainUI)

QT_WARNING_POP
QT_END_MOC_NAMESPACE
